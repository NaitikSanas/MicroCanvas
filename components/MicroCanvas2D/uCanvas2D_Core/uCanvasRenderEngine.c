
#include "uCanvasRenderEngine.h"
#include "esp_timer.h"
#include "uCanvas_Draw.h" 
#include "uCanvas2D_Display_Setup.h"
#include "uCanvas2D_ST7789_Port.h"
#include "uCanvas2D_EK79007Port.h"
#include "uCanvas2D_Acceleration.h"

extern SemaphoreHandle_t active_scene_mutex;

TaskHandle_t uCanvas_taskhandle;
extern uCanvas_Scene_t* active_scene;
uint16_t IRAM_ATTR convertToRGB565(color_t color) {
    // Combine into RGB565 format
  	return (((color.red * 31) / 255) << 11) | (((color.green * 63) / 255) << 5) | ((color.blue * 31) / 255);
}

void IRAM_ATTR draw_universal_object_to_target_render_buffer(uCanvas_universal_obj_t* obj, uCanvas2D_RenderBuffer_t* framebuffer,uCanvas2D_Display_Panel_t* panel){
    uint16_t color = convertToRGB565(obj->properties.color);
    switch (obj->properties.type)
    {
    case RECTANGLE:{
        // printf("draw rect %d,%d,%d,%d \r\n",obj->properties.position.x,
            // obj->properties.position.y,
            // obj->width,
            // obj->height);
        uCanvas2D_DrawRect(
            framebuffer, // Use your global or current render buffer
            obj->properties.position.x,
            obj->properties.position.y,
            obj->width,
            obj->height,
            color,
            obj->properties.fill,1
        );
        break;
    }

    case ADV_TEXTBOX : {

        uCanvas_Draw_SFONT_Advanced_TextBox(framebuffer,obj);
        // printf("draw text %d,%d,%d,%d \r\n",obj->properties.position.x,
            // obj->properties.position.y,
            // obj->width,
            // obj->height);
        // uCanvas_Draw_Text(framebuffer,obj->properties.position.x,obj->properties.position.y,obj->text,obj->font_properties.font_type,color,0x0000,obj->font_properties.Font_Draw_Direction,0);
        // uCanvas_Draw_Text(obj->text,obj->properties.position.x,obj->properties.position.y,obj->properties.color,obj->font_properties);
        break;
    }
    case TEXTBOX : {
        uCanvas_Draw_SFONT_TextBox(framebuffer,obj);
        break;
    }
    case CIRCLE : {
        uCanvas2D_DrawCircle(
            framebuffer,
            obj->properties.position.x,
            obj->properties.position.y,
            obj->r1,
           color,
            obj->properties.fill,1
        );
        break;
    }

    case LINE : {
        uCanvas2D_DrawLine(
            framebuffer,
            obj->point1.x,
            obj->point1.y,
            obj->point2.x,
            obj->point2.y,
            color,
            2
        );
        break;
    }

    case TRIANGLE : {
        Coordinate2D_t p1,p2,p3;
        p1.x = obj->point1.x + obj->properties.position.x;
        p1.y = obj->point1.y + obj->properties.position.y;
        p2.x = obj->point2.x + obj->properties.position.x;
        p2.y = obj->point2.y + obj->properties.position.y;
        p3.x = obj->point3.x + obj->properties.position.x;
        p3.y = obj->point3.y + obj->properties.position.y;
        
        // uCanvas2D_DrawTriangle(
        //     framebuffer,
        //     p1.x, p1.y,
        //     p2.x, p2.y,
        //     p3.x, p3.y,
        //     color,
        //     obj->properties.fill,1
        // );
        break;
    }

    case SPRITE2D: {
        uCanvas2D_DrawSprite(
            framebuffer,
            obj->properties.position.x,
            obj->properties.position.y,
            (const uint16_t*)obj->pixel_data,
            obj->width,
            obj->height,
            obj->color_format
        );
        break;
    }

    case ELLIPSE : {
        uCanvas2D_DrawEllipse(
            framebuffer,
            obj->properties.position.x,
            obj->properties.position.y,
            obj->r1,
            obj->r2,
            color,
            obj->properties.fill,1
        );
        break;
    }
    
    case WINDOW : {
        // if(obj->requires_update){
            uCanvas2D_Instance_t* Window_instance = (uCanvas2D_Instance_t*)obj->ctx_data;
            LOCK_RESOURCE(Window_instance->render_buffer_lock);
                uCanvas2D_DrawSprite(
                    framebuffer,
                    obj->properties.position.x,
                    obj->properties.position.y,
                    obj->pixel_data,
                    obj->width,
                    obj->height,
                    obj->color_format
                );
            UNLOCK_RESOURCE(Window_instance->render_buffer_lock);
        // }
    }

    default:
        break;
    }
}

int64_t uCanvas_Get_FPS(uCanvas2D_Instance_t* instance){
    if(instance == NULL)return 0;
    return 1000000/instance->fps;
}

void uCanvas_Set_Render_Mode(uCanvas2D_Instance_t* instance,uCanvas2D_Render_Mode_t mode){
    instance->Render_Mode = mode;
    instance->signal_scene_refresh = xSemaphoreCreateBinary();
    instance->scene_refresh_complete = xSemaphoreCreateBinary();
}

void uCanvas_Send_Refresh_Signal_To_Renderer(uCanvas2D_Instance_t* instance){
    if(instance->Render_Mode == AUTO_REFRESH)return;
    if((instance->Render_Mode == ASYNC_FRAME_QUEUED || instance->Render_Mode == ASYNC_FRAME_COMMIT) && instance->signal_scene_refresh != NULL ){
        xSemaphoreGive(instance->signal_scene_refresh);
    }

    if(instance->Render_Mode == ASYNC_FRAME_COMMIT){
        xSemaphoreTake(instance->scene_refresh_complete,portMAX_DELAY);
    }
}

void wait_on_referesh_signal(uCanvas2D_Instance_t* instance){
    xSemaphoreGive(instance->scene_refresh_complete);
    if(instance->Render_Mode == ASYNC_FRAME_QUEUED && instance->signal_scene_refresh != NULL){
        xSemaphoreTake(instance->signal_scene_refresh,portMAX_DELAY); 
    }
}

void uCanvas_renderer_task(void*arg){
    uCanvas2D_Instance_t* instance = (uCanvas2D_Instance_t*)arg;
    uCanvas2D_RenderBuffer_t* current_bufffer = instance->render_buffer;
    uCanvas2D_RenderBuffer_t* draw_bufffer = instance->render_buffer;
    if(instance == NULL || instance->render_buffer == NULL){
        printf("Invalid uCanvas2D_Instance_t\r\n");
        return;
    }

    #if UCANVAS_USE_DOUBLE_BUFFERING
        int switch_buffer = 0;
    #endif
    printf("Renderer task started \r\n");
    uint32_t obuf_size = 0;
    float  scaled_x = 0;
    float  scaled_y = 0;
    if(instance->scale_output){
        scaled_x = instance->scale_x;
        scaled_y = instance->scale_y;
        obuf_size = instance->post_processing_frame_buf->width*instance->post_processing_frame_buf->height*sizeof(uint16_t);
    }
    while(1){ 
        // int64_t start = esp_timer_get_time();
        int64_t start = esp_timer_get_time();
        uCanvas_Scene_t* _scene = instance->active_scene;
		vTaskDelay(pdMS_TO_TICKS(instance->refresh_delay));

        //check if manual refresh on signal is enabled.
        
        wait_on_referesh_signal(instance);

        //Switch Render Buffer when Double buffering enabled.
        #if UCANVAS_USE_DOUBLE_BUFFERING
        if(switch_buffer) current_bufffer = instance->render_buffer_aux;
        else current_bufffer = instance->render_buffer;
        switch_buffer = !switch_buffer;
        #endif
        if(current_bufffer == NULL){
            printf("Null Render Buffer\r\n");
            return;
        }

		if((_scene != NULL) && (_scene->_2D_Object_Ptr > 0)){
			if(LOCK_RESOURCE(instance->render_buffer_lock)){ 
                //Clear Display
                if(instance->Clear_On_Refresh){
                    if(instance->render_buffer->use_ppa){
                        ppa_helper_fill(
                        current_bufffer->pixels,
                        current_bufffer->width * current_bufffer->height * sizeof(uint16_t),
                        current_bufffer->width,
                        current_bufffer->height,
                        0, 0, current_bufffer->width, current_bufffer->height, 0x0000,0
                        );
                    }
                    else{
                        memset(current_bufffer->pixels, 0x0000, current_bufffer->width * current_bufffer->height * sizeof(uint16_t)); 
                    }
                }
                //Push All elements to display 
				for (int i = 0; i < _scene->_2D_Object_Ptr; i++)
				{
					if(_scene->_2D_Objects[i]==NULL)break;
                    if(_scene->_2D_Objects[i]->properties.visiblity == INVISIBLE)continue;
                    uCanvas_universal_obj_t* obj = _scene->_2D_Objects[i];
                    draw_universal_object_to_target_render_buffer(obj,current_bufffer,instance->panel_1);
				}

                //Update display panel (This is non blockig (while not scaling output))
                draw_bufffer = current_bufffer;
                if(instance->scale_output){
                    scale_buffer_with_factor(draw_bufffer->pixels,instance->post_processing_frame_buf->pixels,obuf_size,draw_bufffer->width,draw_bufffer->height,scaled_x,scaled_y);
                    if(instance->panel_1 != NULL)instance->panel_1->push_render_buffer(0, 0, instance->post_processing_frame_buf); 
                }
                else {
                    if(instance->panel_1 != NULL)instance->panel_1->push_render_buffer(draw_bufffer->offset_x, draw_bufffer->offset_y, draw_bufffer);
                }              
                UNLOCK_RESOURCE(instance->render_buffer_lock);
                uint64_t now = esp_timer_get_time() ;
                instance->fps = (now - start);
			}
        }else {
            printf("No active scene to render\r\n");
        }

        // //Synchronizer for parent uCanvas Instance
        // if(instance->synchronize){
        //     uCanvas2D_Instance_t* w_inst = (uCanvas2D_Instance_t*)instance->window_instance;
        //     UNLOCK_RESOURCE(w_inst->render_buffer_lock);
        // }
        // instance->signal_scene_refresh = false;
	}
}




uCanvas2D_Instance_t* New_uCanvas_Instance(uCanvas_Scene_t* scene, uCanvas2D_Display_Panel_t* panel_1,uCanvas2D_Display_Panel_t* panel_2, int width, int height, int offset_x, int offset_y) {
    printf("Creating new uCanvas2D_Instance_t\r\n");
    uCanvas2D_Instance_t* instance =  (uCanvas2D_Instance_t*) heap_caps_aligned_alloc(32, sizeof(uCanvas2D_Instance_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);//malloc(sizeof(uCanvas2D_Instance_t));
    if (instance == NULL) {
        printf("Failed to allocate memory for uCanvas2D_Instance_t\r\n");
        return NULL;
    }
    instance->synchronize = false;
    instance->window_instance = NULL;
    instance->refresh_delay = 9;

    // Plug in Scene and panels to the instance
    instance->active_scene = scene;
    instance->panel_1 = panel_1;
    instance->panel_2 = panel_2;
    instance->Render_Mode = AUTO_REFRESH;
    instance->signal_scene_refresh = xSemaphoreCreateBinary();
    // Create a new render buffer for the instance

    instance->render_buffer = (uCanvas2D_RenderBuffer_t*)malloc(sizeof(uCanvas2D_RenderBuffer_t));
    instance->render_buffer->width  = width;
    instance->render_buffer->height = height;
    instance->render_buffer->pitch = 0;
    instance->render_buffer->offset_x = offset_x;
    instance->render_buffer->offset_y = offset_y;
    instance->render_buffer->pixels = heap_caps_aligned_alloc(32,  instance->render_buffer->width * instance->render_buffer->height* sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);// (uint16_t*)malloc(panel_1->width * panel_1->height * sizeof(uint16_t));
    if (instance->render_buffer->pixels == NULL) {
        printf("Failed to allocate memory for render buffer pixels\r\n");
        free(instance->render_buffer);
        free(instance);
        return NULL;
    }
    memset(instance->render_buffer->pixels, 0x0000, instance->render_buffer->width * instance->render_buffer->height * sizeof(uint16_t)); // Initialize to black

    #if UCANVAS_USE_DOUBLE_BUFFERING
    // Create a new render buffer for the instance
    instance->render_buffer_aux = (uCanvas2D_RenderBuffer_t*)malloc(sizeof(uCanvas2D_RenderBuffer_t));
    instance->render_buffer_aux->width = width;
    instance->render_buffer_aux->height = height;
    instance->render_buffer_aux->pitch = 0;
    instance->render_buffer_aux->offset_x = offset_x;
    instance->render_buffer_aux->offset_y = offset_y;
    instance->render_buffer_aux->pixels = heap_caps_aligned_alloc(32,  instance->render_buffer->width * instance->render_buffer->height * sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);// (uint16_t*)malloc(panel_1->width * panel_1->height * sizeof(uint16_t));
    if (instance->render_buffer_aux->pixels == NULL) {
        printf("Failed to allocate memory for render buffer pixels\r\n");
        free(instance->render_buffer_aux);
        free(instance);
        return NULL;
    }
    memset(instance->render_buffer_aux->pixels, 0x0000, instance->render_buffer->width * instance->render_buffer->height * sizeof(uint16_t)); // Initialize to black
    printf("[INFO]:Using Double Buffering, Total size %d\r\n", (instance->render_buffer->width * instance->render_buffer->height* sizeof(uint16_t)*2)/1000);
    #else 
        // printf("[INFO]:Not Double Buffering, Total size %d\r\n", instance->render_buffer->width * instance->render_buffer->height * sizeof(uint16_t))/1000);
        instance->render_buffer_aux = NULL;
    #endif

    // Setup PPA if Target is ESP32P4 and PPA is enabled
    #if CONFIG_IDF_TARGET_ESP32P4 && USE_PPA_FOR_RENDERING
        Intialize_PPA();
        instance->render_buffer->use_ppa = true;
        printf("[INFO]:Using PPA\r\n");
    #else
        printf("[INFO]:Not Using PPA\r\n");
        if(instance->render_buffer)instance->render_buffer->use_ppa = false;
        if(instance->render_buffer_aux)instance->render_buffer_aux->use_ppa = false;
    #endif
    
    instance->Clear_On_Refresh = false;
    instance->render_buffer_lock = xSemaphoreCreateBinary();
    UNLOCK_RESOURCE(instance->render_buffer_lock);

    if(instance->panel_1 != NULL){
        instance->panel_1->init(1);
        instance->panel_1->set_backlight(8000);
    }
    if(instance->panel_2 != NULL){
        instance->panel_2->init(1);
        instance->panel_2->set_backlight(8000);
    }

    instance->pin_to_core = 1;
    xTaskCreatePinnedToCore(&uCanvas_renderer_task, "uCanvas_Render_Task", UCANVAS_RENDER_TASK_STACK_SIZE, instance, UCANVAS_RENDER_TASK_PRIORITY, &instance->render_task_handle, instance->pin_to_core);
    printf("uCanvas2D_Instance_t created successfully\r\n");
    return instance;
}

// int uCanvas_Attach_RenderBuffer(uCanvas2D_Instance_t* instance, int width, int height){
//     instance->render_buffer = NULL;
//     instance->render_buffer_aux = NULL;

//     instance->render_buffer = (uCanvas2D_RenderBuffer_t*)malloc(sizeof(uCanvas2D_RenderBuffer_t));
//     instance->render_buffer_aux = (uCanvas2D_RenderBuffer_t*)malloc(sizeof(uCanvas2D_RenderBuffer_t));
    
    
//     #if(UCANVAS_USE_DOUBLE_BUFFERING)
        
//     #endif

//     instance->render_buffer_lock = xSemaphoreCreateBinary();
//     UNLOCK_RESOURCE(instance->render_buffer_lock);
//     if(instance->render_buffer){
//         #if(UCANVAS_USE_DOUBLE_BUFFERING)
//         if(instance->render_buffer_aux){
//             uCanvas2D_Create_RenderBuffer(instance->render_buffer_aux,width,height);
//             uCanvas2D_Create_RenderBuffer(instance->render_buffer,width,height);
//             return 1;
//         }   
//         #endif
//         uCanvas2D_Create_RenderBuffer(instance->render_buffer,width,height);
//         return 1;
//     }
//     return 0;
// }

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

    if(instance->scale_output == true){
        uint16_t scaled_x = instance->render_buffer->width * instance->scale_x;
        uint16_t scaled_y = instance->render_buffer->height * instance->scale_y;
        if(scaled_x <= 1920 && scaled_y <= 1080){
            instance->post_processing_frame_buf = malloc(sizeof(uCanvas2D_RenderBuffer_t));
            uCanvas2D_Create_RenderBuffer(instance->post_processing_frame_buf,scaled_x,scaled_y);
        }else{
            printf("Invalid Scaling Factors, Forcing Disabling Scaling\r\n");
            instance->scale_output = false;
        }
    }
    return 1;
}

void uCanvas_Set_ViewPort_Position(uCanvas2D_Instance_t* instance, int x, int y){
    if(instance){
        instance->render_buffer->offset_x = x;
        instance->render_buffer->offset_y = y;
        #if UCANVAS_USE_DOUBLE_BUFFERING
            instance->render_buffer_aux->offset_x = x;
            instance->render_buffer_aux->offset_y = y;
        #endif
    }
}

void uCanvas_Attach_Scene(uCanvas2D_Instance_t* instance, uCanvas_Scene_t* scene){
    if(instance)instance->active_scene = scene;
    else printf("[Err]:Invalid Instance\r\n");
}


void uCanvas_Attach_Panel(uCanvas2D_Instance_t* instance, uCanvas2D_Display_Panel_t* panel){
    if(instance && panel)instance->panel_1 = panel;
    // if(panel){
    //     panel->init(1);
    //     panel->set_backlight(8000);
    // }
    else return;
}

void uCanvas_Attach_Renderer(uCanvas2D_Instance_t* instance, int core_id){
    instance->Render_Mode = AUTO_REFRESH;
    instance->Clear_On_Refresh = true;
    instance->refresh_delay = 2;
    instance->pin_to_core = core_id;
    xTaskCreatePinnedToCore(&uCanvas_renderer_task, "uCanvas_Render_Task", UCANVAS_RENDER_TASK_STACK_SIZE, instance, UCANVAS_RENDER_TASK_PRIORITY, &instance->render_task_handle, instance->pin_to_core);
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
    // Create a new render buffer for the instance
    instance->render_buffer = (uCanvas2D_RenderBuffer_t*)malloc(sizeof(uCanvas2D_RenderBuffer_t));
    instance->render_buffer->width = width;
    instance->render_buffer->height = height;
    instance->render_buffer->pitch = 0;
    instance->render_buffer->offset_x = 0;
    instance->render_buffer->offset_y = 0;
    instance->render_buffer->pixels = heap_caps_aligned_alloc(sizeof(uint16_t), width * height * sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);// (uint16_t*)malloc(panel_1->width * panel_1->height * sizeof(uint16_t));
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
    instance->render_buffer_aux->pixels = heap_caps_aligned_alloc(sizeof(uint16_t), width * height * sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);// (uint16_t*)malloc(panel_1->width * panel_1->height * sizeof(uint16_t));
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
    xTaskCreatePinnedToCore(&uCanvas_renderer_task, "uCanvas_Render_Task", UCANVAS_RENDER_TASK_STACK_SIZE, instance, UCANVAS_RENDER_TASK_PRIORITY, &instance->render_task_handle, instance->pin_to_core);
    printf("uCanvas2D_Instance_t created successfully\r\n");
    return instance;
}

void uCanvas_Pause_Instance(uCanvas2D_Instance_t* instance){
    if(instance == NULL || instance->render_task_handle == NULL){
        printf("Invalid uCanvas2D_Instance_t or render task handle\r\n");
        return;
    }
    vTaskSuspend(instance->render_task_handle);
}

void uCanvas_Resume_Instance(uCanvas2D_Instance_t* instance){
    if(instance == NULL || instance->render_task_handle == NULL){
        printf("Invalid uCanvas2D_Instance_t or render task handle\r\n");
        return;
    }
    vTaskResume(instance->render_task_handle);
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

void uCanvas_Set_Panel_RefreshDelay(uCanvas2D_Instance_t* instance, uint16_t RefreshDelay){
    if(instance)instance->refresh_delay = RefreshDelay;
}
void pause_uCanvas_engine(void){
    vTaskSuspend(uCanvas_taskhandle);
}
void resume_uCanvas_engine(void){
    vTaskResume(uCanvas_taskhandle);
}

