
#include "uCanvas2D_Instance.h"
#include "esp_timer.h"
#include "uCanvas_Draw.h" 
#include "uCanvas2D_Display_Setup.h"
#include "uCanvas2D_ST7789_Port.h"
#include "uCanvas2D_EK79007Port.h"
#include "uCanvas2D_Acceleration.h"
#include "uCanvas_Render_Common.h"
#include "uCanvas_Render_Manager.h"
#include "uCanvas_Render_Classic.h"
#include "uCanvas_Render_Sketch.h"
#include "uCanvas_Settings.h"

int uCanvas_Attach_RenderBuffer(uCanvas2D_Instance_t* instance, int width, int height) {

    instance->render_buffer = NULL;
    instance->render_buffer_aux = NULL;

    // Always allocate primary
    
    
    instance->render_buffer = malloc(sizeof(uCanvas2D_RenderBuffer_t));
    if (!instance->render_buffer) return 0;
    instance->render_buffer->offset_x = 0;
    instance->render_buffer->offset_y = 0;
#if(UCANVAS_USE_DOUBLE_BUFFERING)
    
    instance->render_buffer_aux = malloc(sizeof(uCanvas2D_RenderBuffer_t));
    instance->render_buffer_aux->offset_x = 0;
    instance->render_buffer_aux->offset_y = 0;
    if (!instance->render_buffer_aux) {
        free(instance->render_buffer);
        instance->render_buffer = NULL;
        return 0;
    }
#endif

    instance->render_buffer_lock = xSemaphoreCreateBinary();
    if (!instance->render_buffer_lock) {
        free(instance->render_buffer);
#if(UCANVAS_USE_DOUBLE_BUFFERING)
        free(instance->render_buffer_aux);
#endif
        return 0;
    }
    UNLOCK_RESOURCE(instance->render_buffer_lock);
    
    uCanvas2D_Create_RenderBuffer(instance->render_buffer, width, height);
    
#if(UCANVAS_USE_DOUBLE_BUFFERING)
    uCanvas2D_Create_RenderBuffer(instance->render_buffer_aux, width, height);
#endif

    if(instance->upscale_instance_output == true){
        uint16_t scaled_x = instance->render_buffer->width * instance->scale_x;
        uint16_t scaled_y = instance->render_buffer->height * instance->scale_y;
        if(scaled_x <= 1920 && scaled_y <= 1080){
            instance->post_processing_frame_buf = malloc(sizeof(uCanvas2D_RenderBuffer_t));
            uCanvas2D_Create_RenderBuffer(instance->post_processing_frame_buf,scaled_x,scaled_y);
        }else{
            printf("Invalid Scaling Factors, Forcing Disabling Scaling\r\n");
            instance->upscale_instance_output = false;
        }
    }
    return 1;
}

void uCanvas_Set_ViewPort_Position(uCanvas2D_Instance_t* instance, int x, int y){
    if(instance){
        instance->render_buffer->offset_x = x;
        instance->render_buffer->offset_y = y;
        #if (UCANVAS_USE_DOUBLE_BUFFERING == ENABLED)
            instance->render_buffer_aux->offset_x = x;
            instance->render_buffer_aux->offset_y = y;
        #endif
        // Force a redraw/push because the output region moved.
        instance->last_rendered_scene_seq = 0;
    }
}

void uCanvas_Attach_Scene(uCanvas2D_Instance_t* instance, uCanvas_Scene_t* scene){
    if(instance){
        instance->active_scene = scene;
        instance->last_rendered_scene_seq = 0;
        if(scene) scene->dirty_seq++;
    }
    else printf("[Err]:Invalid Instance\r\n");
}


void uCanvas_Attach_Panel(uCanvas2D_Instance_t* instance, uCanvas2D_Display_Panel_t* panel){
    if(instance && panel)instance->panel_1 = panel;
    else return;
}

void uCanvas_Set_Canvas_Clear_Color(uCanvas2D_Instance_t* Instance,uint8_t r, uint8_t g, uint8_t b){
    if(Instance){
        Instance->canvas_clear_color.red = r;
        Instance->canvas_clear_color.green = g;
        Instance->canvas_clear_color.blue = b;
    }
}

uCanvas2D_Instance_t* New_uCanvas_Window_Instance(uCanvas_Scene_t* scene,int width, int height) {
    printf("Creating new uCanvas2D_Instance_t\r\n");
    uCanvas2D_Instance_t* instance = (uCanvas2D_Instance_t*)malloc(sizeof(uCanvas2D_Instance_t));
    if (instance == NULL) {
        printf("Failed to allocate memory for uCanvas2D_Instance_t\r\n");
        return NULL;
    }
    instance->panel_1 = NULL;
    instance->panel_2 = NULL;
    instance->refresh_delay = 9;
    instance->synchronize = false;
    instance->window_instance = NULL;
    // Plug in Scene and panels to the instance
    instance->active_scene = scene;
    instance->Render_Mode = AUTO_REFRESH;
    instance->Clear_On_Refresh = true;
    instance->last_rendered_scene_seq = 0;
    instance->last_frame_time_us = 0;
    instance->fps_smoothed = 0.0f;
    // Create a new render buffer for the instance
    instance->render_buffer = (uCanvas2D_RenderBuffer_t*)malloc(sizeof(uCanvas2D_RenderBuffer_t));
    instance->render_buffer->width = width;
    instance->render_buffer->height = height;
    instance->render_buffer->pitch = 0;
    instance->render_buffer->offset_x = 0;
    instance->render_buffer->offset_y = 0;
    instance->render_buffer->pixels = heap_caps_aligned_alloc(sizeof(uint16_t), width * height * sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
    if (instance->render_buffer->pixels == NULL) {
        printf("Failed to allocate memory for render buffer pixels\r\n");
        free(instance->render_buffer);
        free(instance);
        return NULL;
    }
    memset(instance->render_buffer->pixels, 0x0000, width * height * sizeof(uint16_t)); // Initialize to black


    instance->render_buffer_aux = (uCanvas2D_RenderBuffer_t*)malloc(sizeof(uCanvas2D_RenderBuffer_t));
    instance->render_buffer_aux->width = width;
    instance->render_buffer_aux->height = height;
    instance->render_buffer_aux->pitch = 0;
    instance->render_buffer_aux->offset_x = 0;
    instance->render_buffer_aux->offset_y = 0;
    instance->render_buffer_aux->pixels = heap_caps_aligned_alloc(sizeof(uint16_t), width * height * sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
    if (instance->render_buffer_aux->pixels == NULL) {
        printf("Failed to allocate memory for render buffer pixels\r\n");
        free(instance->render_buffer_aux);
        free(instance);
        return NULL;
    }
    memset(instance->render_buffer->pixels, 0x0000, width * height * sizeof(uint16_t)); // Initialize to black


    // Setup PPA if Target is ESP32P4 and PPA is enabled
    #if CONFIG_IDF_TARGET_ESP32P4 && USE_PPA_FOR_RENDERING
        Intialize_PPA();
        instance->render_buffer->use_ppa = true;
        printf("[INFO]:Using PPA\r\n");
    #else
        printf("[INFO]:Not Using PPA\r\n");
        instance->render_buffer->use_ppa = false;
    #endif

    
    instance->render_buffer_lock = xSemaphoreCreateBinary();
    UNLOCK_RESOURCE(instance->render_buffer_lock);
    
    instance->pin_to_core = 1; // Default to core 0, can be changed later
    #if UCANVAS_USE_DIRTY_RECT_RENDERER
        xTaskCreatePinnedToCore(&uCanvas_renderer_task_dirtyrect, "uCanvas_Render_Task", UCANVAS_RENDER_TASK_STACK_SIZE, instance, UCANVAS_RENDER_TASK_PRIORITY, &instance->render_task_handle, instance->pin_to_core);
    #else
        xTaskCreatePinnedToCore(&uCanvas_renderer_task_classic, "uCanvas_Render_Task", UCANVAS_RENDER_TASK_STACK_SIZE, instance, UCANVAS_RENDER_TASK_PRIORITY, &instance->render_task_handle, instance->pin_to_core);
    #endif
    printf("uCanvas2D_Instance_t created successfully\r\n");
    return instance;
}

void uCanvas_Destroy_Instance(uCanvas2D_Instance_t* instance) {
    if (instance == NULL) {
        printf("Invalid uCanvas2D_Instance_t\r\n");
        return;
    }
    if (instance->render_buffer != NULL) {
        if (instance->render_buffer->pixels != NULL) {
            free(instance->render_buffer->pixels);
        }
        free(instance->render_buffer);
    }
    if (instance->render_task_handle != NULL) {
        vTaskDelete(instance->render_task_handle);
    }
    free(instance);
    printf("uCanvas2D_Instance_t destroyed successfully\r\n");
}
