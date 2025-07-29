
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

void IRAM_ATTR push_element_to_display(uCanvas_universal_obj_t* obj, uCanvas2D_RenderBuffer_t* framebuffer){
    uint16_t color = obj->properties.color.red << 11 | obj->properties.color.green << 5 | obj->properties.color.blue;
    switch (obj->properties.type)
    {
    case RECTANGLE:{
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

    case TEXTBOX : {
        // uCanvas_Draw_Text(obj->text,obj->properties.position.x,obj->properties.position.y,obj->properties.color,obj->font_properties);
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
            obj->point1.x + obj->properties.position.x,
            obj->point1.y + obj->properties.position.y,
            obj->point2.x + obj->properties.position.x,
            obj->point2.y + obj->properties.position.y,
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

        uCanvas2D_DrawTriangle(
            framebuffer,
            p1.x, p1.y,
            p2.x, p2.y,
            p3.x, p3.y,
            color,
            obj->properties.fill,1
        );
        break;
    }

    case SPRITE2D: {
        uCanvas2D_DrawSprite(
            framebuffer,
            obj->properties.position.x,
            obj->properties.position.y,
            (const uint16_t*)obj->sprite_buffer,
            obj->width,
            obj->height,
            obj->sprite_color_format
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
    default:
        break;
    }
}

uCanvas2D_Instance_t* uCanvas_New_Instance(uCanvas_Scene_t* scene, uCanvas2D_Display_Panel_t* panel, uCanvas2D_RenderBuffer_t* render_buffer) {
    uCanvas2D_Instance_t* instance = (uCanvas2D_Instance_t*)malloc(sizeof(uCanvas2D_Instance_t));
    if (instance == NULL) {
        printf("Failed to allocate memory for uCanvas2D_Instance_t\r\n");
        return NULL;
    }
    instance->active_scene = scene;
    instance->panel_1 = panel;
    instance->render_buffer = render_buffer;
    return instance;
}
int64_t fps=0;
int64_t uCanvas_Get_FPS(void){
    return fps;
}

void IRAM_ATTR uCanvas_renderer_task(void*arg){
    uCanvas2D_Instance_t* instance = (uCanvas2D_Instance_t*)arg;
    if(instance == NULL || instance->render_buffer == NULL){
        printf("Invalid uCanvas2D_Instance_t\r\n");
        return;
    }
    while(1){ 
        uCanvas_Scene_t* _scene = instance->active_scene;
		vTaskDelay(pdMS_TO_TICKS(9));
		if((_scene != NULL) && (_scene->_2D_Object_Ptr > 0)){
			if(LOCK_RESOURCE(instance->render_buffer_lock)){ 
                int64_t start = esp_timer_get_time();
                if(instance->render_buffer->use_ppa){
                    ppa_helper_fill(
                    instance->render_buffer->pixels,
                    instance->render_buffer->width * instance->render_buffer->height * sizeof(uint16_t),
                    instance->render_buffer->width,
                    instance->render_buffer->height,
                    0, 0, instance->render_buffer->width, instance->render_buffer->height, 0x0000
                    );
                }
                else{
                    memset(instance->render_buffer->pixels, 0x0000, instance->render_buffer->width * instance->render_buffer->height * sizeof(uint16_t)); 
                }

				for (int i = 0; i < _scene->_2D_Object_Ptr; i++)
				{
					uCanvas_universal_obj_t* obj = _scene->_2D_Objects[i];
                    push_element_to_display(obj,instance->render_buffer);
				}    
                if(instance->panel_1 != NULL)instance->panel_1->push_render_buffer(0, 0, instance->render_buffer);
                if(instance->panel_2 != NULL)instance->panel_2->push_render_buffer(0, 0, instance->render_buffer);
                fps = 1000000 / (esp_timer_get_time() - start);
                UNLOCK_RESOURCE(instance->render_buffer_lock);
			}
        }
	}
}

uCanvas2D_Instance_t* New_uCanvas_Instance(uCanvas_Scene_t* scene, uCanvas2D_Display_Panel_t* panel_1,uCanvas2D_Display_Panel_t* panel_2) {
    printf("Creating new uCanvas2D_Instance_t\r\n");
    uCanvas2D_Instance_t* instance = (uCanvas2D_Instance_t*)malloc(sizeof(uCanvas2D_Instance_t));
    if (instance == NULL) {
        printf("Failed to allocate memory for uCanvas2D_Instance_t\r\n");
        return NULL;
    }
    // Plug in Scene and panels to the instance
    instance->active_scene = scene;
    instance->panel_1 = panel_1;
    instance->panel_2 = panel_2;

    // Create a new render buffer for the instance
    instance->render_buffer = (uCanvas2D_RenderBuffer_t*)malloc(sizeof(uCanvas2D_RenderBuffer_t));
    instance->render_buffer->width = panel_1->width;
    instance->render_buffer->height = panel_1->height;
    instance->render_buffer->pitch = 0;
    instance->render_buffer->offset_x = 0;
    instance->render_buffer->offset_y = 0;
    instance->render_buffer->pixels = heap_caps_aligned_alloc(32, panel_1->width * panel_1->height * sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);// (uint16_t*)malloc(panel_1->width * panel_1->height * sizeof(uint16_t));
    if (instance->render_buffer->pixels == NULL) {
        printf("Failed to allocate memory for render buffer pixels\r\n");
        free(instance->render_buffer);
        free(instance);
        return NULL;
    }
    memset(instance->render_buffer->pixels, 0x0000, panel_1->width * panel_1->height * sizeof(uint16_t)); // Initialize to black

    // Setup PPA if Target is ESP32P4 and PPA is enabled
    #if CONFIG_IDF_TARGET_ESP32P4 && USE_PPA_FOR_RENDERING
        Intialize_PPA();
        instance->render_buffer->use_ppa = true;
    #else
        instance->render_buffer->use_ppa = false;
    #endif

    
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

    xTaskCreatePinnedToCore(&uCanvas_renderer_task, "uCanvas_Render_Task", UCANVAS_RENDER_TASK_STACK_SIZE, instance, UCANVAS_RENDER_TASK_PRIORITY, &instance->render_task_handle, 1);
    printf("uCanvas2D_Instance_t created successfully\r\n");
    return instance;
}



void pause_uCanvas_engine(void){
    vTaskSuspend(uCanvas_taskhandle);
}
void resume_uCanvas_engine(void){
    vTaskResume(uCanvas_taskhandle);
}