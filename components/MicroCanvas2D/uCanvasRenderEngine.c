#include "uCanvas_display_port.h"
#include "uCanvasRenderEngine.h"
#include "esp_timer.h"
#include "uCanvas_Draw.h" 
#include "uCanvas2D_Display_Setup.h"
#include "uCanvas2D_ST7789_Port.h"
#include "uCanvas2D_EK79007Port.h"

uCanvas2D_Display_Panel_t* g_panel;
uCanvas2D_RenderBuffer_t *g_framebuffer;
extern SemaphoreHandle_t active_scene_mutex;
#define LOCK_ACTIVE_SCENEB_BUF      xSemaphoreTake(active_scene_mutex,portMAX_DELAY)
#define UNLOCK_ACTIVE_SCENEB_BUF    xSemaphoreGive(active_scene_mutex);
TaskHandle_t uCanvas_taskhandle;
extern uCanvas_Scene_t* active_scene;


void ssd1306_sprite_draw(uCanvas_universal_obj_t* obj){
    uint16_t offset_x       = obj->properties.position.x;
    uint16_t offset_y       = obj->properties.position.y;
    uint16_t sprite_width   = obj->width ;
    uint16_t sprite_height  = obj->height;
    uint16_t x_ptr          = 0;
    uint16_t y_ptr          = 0;
    color_t c;
    Coordinate2D_t pos;
    if(obj == NULL){
        printf("null!!!!\r\n");
    }
    if(obj->properties.visiblity == INVISIBLE){
    return;
    }
    for (int i = 0; i < (sprite_width*sprite_height); i++)
    { 
    c.monochrome_pixel = obj->invert_sprite_pixels ? !obj->sprite_buffer[i] : obj->sprite_buffer[i];
    pos.x = x_ptr + offset_x;
    pos.y = y_ptr + offset_y;
    if(obj->sprite_buffer[i]!=2)uCanvas_DrawPixel((x_ptr + offset_x),(y_ptr + offset_y),c);
    if(x_ptr < sprite_width-1){
        x_ptr++;
    }else{
        x_ptr=0;
        y_ptr++;
    }
    }   
}
int64_t time_to_draw_element = 1;

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
            obj->properties.fill,1
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


// void IRAM_ATTR push_element_to_display(uCanvas_universal_obj_t* obj){
//     switch (obj->properties.type)
//     {
//     case RECTANGLE:{
//         uCanvas_Draw_Rectangle(obj->properties.position.x, obj->properties.position.y,
// 		    obj->height,obj->width,obj->properties.color,obj->properties.fill);
//         break;
//     }

//     case TEXTBOX : {
//         uCanvas_Draw_Text(obj->text,obj->properties.position.x,obj->properties.position.y,obj->properties.color,obj->font_properties);
//       break;
//     }

//     case CIRCLE : {
//         uCanvas_Draw_Circle(obj->properties.position.x,obj->properties.position.y, obj->r1,obj->properties.color, obj->properties.fill);
//         break;
//     }

//     case LINE : {
//       uCanvas_Draw_Line(obj->point1,obj->point2,obj->properties.color);
//       break;
//     }

//     case TRIANGLE : {
//         Coordinate2D_t p1,p2,p3;
//         p1.x = obj->point1.x + obj->properties.position.x;
//         p1.y = obj->point1.y + obj->properties.position.y;
//         p2.x = obj->point2.x + obj->properties.position.x;
//         p2.y = obj->point2.y + obj->properties.position.y;
//         p3.x = obj->point3.x + obj->properties.position.x;
//         p3.y = obj->point3.y + obj->properties.position.y;

//         uCanvas_Draw_Triangle(p1,
//                               p2,
//                               p3,
//                                 obj->properties.color,
//                                 obj->properties.fill);
//       break;
//     }

//     case SPRITE2D: {
//         #ifdef USE_SSD1306
//             ssd1306_sprite_draw(obj);
//         #endif

//         #ifdef USE_ST7789
//             st7789_draw_sprite_batch(obj);
//         #endif
//         break;
//     }

//     case ELLIPSE : {
//         uCanvas_Draw_Ellipse(obj->properties.position.x, obj->properties.position.y, obj->r1, obj->r2,obj->properties.color,obj->properties.fill);
//     }
//     default:
//       break;
//     }
// }

int64_t display_draw_time = 1;
int64_t time_to_draw_frame_buf = 1;
int64_t on_screen_draw_time = 1;
uCanvas2D_Display_Panel_t* panel_2;
uCanvas2D_RenderBuffer_t* upscale_render_buffer = NULL;
IRAM_ATTR void uCanvas2D_Upscale_RenderBuffer(uCanvas2D_RenderBuffer_t* upscaled_buf, uCanvas2D_RenderBuffer_t* src_buf, int src_height, int src_width) {
    // Calculate scaling factors for X and Y
    float scale_x = (float)upscaled_buf->width / (float)src_width;
    float scale_y = (float)upscaled_buf->height / (float)src_height;

    for (int y = 0; y < upscaled_buf->height; y++) {
        int src_y = (int)((float)y / scale_y);
        if (src_y >= src_height) src_y = src_height - 1;
        for (int x = 0; x < upscaled_buf->width; x++) {
            int src_x = (int)((float)x / scale_x);
            if (src_x >= src_width) src_x = src_width - 1;
            upscaled_buf->pixels[y * upscaled_buf->width + x] = src_buf->pixels[src_y * src_width + src_x];
        }
    }
}

void IRAM_ATTR uCanvas_bg_render_engine_task(void*arg){
    while(1){ 
        uint32_t tick = xTaskGetTickCount();
		vTaskDelay(pdMS_TO_TICKS(9));
		if((active_scene != NULL) && (active_scene->_2D_Object_Ptr > 0)){
			if(LOCK_ACTIVE_SCENEB_BUF){ 
				memset(g_framebuffer->pixels, 0x0000, g_framebuffer->width * g_framebuffer->height * sizeof(uint16_t)); 
				for (int i = 0; i < active_scene->_2D_Object_Ptr; i++)
				{
					uCanvas_universal_obj_t* obj = active_scene->_2D_Objects[i];
					if(obj->properties.visiblity == VISIBLE){
						push_element_to_display(obj,g_framebuffer);
					}
				}    
                int64_t start = esp_timer_get_time();  
                g_panel->push_render_buffer(0, 0, g_framebuffer);
                display_draw_time = esp_timer_get_time() - start;
                UNLOCK_ACTIVE_SCENEB_BUF;
			}
        }
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

void uCanvas_Set_Active_Scene(uCanvas2D_Instance_t *instance, uCanvas_Scene_t* scene) {
    if (instance == NULL || scene == NULL) {
        printf("Invalid uCanvas2D_Instance_t or uCanvas_Scene_t\r\n");
        return;
    }
    instance->active_scene = scene;
    active_scene = instance->active_scene;
}

void uCanvas_Set_RenderBuffer(uCanvas2D_Instance_t *instance, uCanvas2D_RenderBuffer_t* render_buffer) {
    if (instance == NULL || render_buffer == NULL) {
        printf("Invalid uCanvas2D_Instance_t or uCanvas2D_RenderBuffer_t\r\n");
        return;
    }
    instance->render_buffer = render_buffer;
}
void uCanvas_Set_Panel(uCanvas2D_Instance_t *instance, uCanvas2D_Display_Panel_t* panel) {
    if (instance == NULL || panel == NULL) {
        printf("Invalid uCanvas2D_Instance_t or uCanvas2D_Display_Panel_t\r\n");
        return;
    }
    instance->panel_1 = panel;
}
void uCanvas_Setup_Instance(uCanvas2D_Instance_t *instance) {
    if (instance == NULL || instance->panel_1 == NULL || instance->render_buffer == NULL) {
        printf("Invalid uCanvas2D_Instance_t, panel or render buffer\r\n");
        return;
    }
    instance->panel_1->init(1);
    instance->panel_1->set_backlight(8000); // Set backlight to 100% by default
}

void IRAM_ATTR uCanvas_renderer_task(void*arg){
    uCanvas2D_Instance_t* instance = (uCanvas2D_Instance_t*)arg;
    if(instance == NULL || instance->render_buffer == NULL){
        printf("Invalid uCanvas2D_Instance_t\r\n");
        return;
    }
    printf("render buffer addr: %p\r\n", instance->render_buffer);

    while(1){ 
        
        uCanvas_Scene_t* _scene = instance->active_scene;
		vTaskDelay(pdMS_TO_TICKS(9));
		if((_scene != NULL) && (_scene->_2D_Object_Ptr > 0)){
			if(LOCK_ACTIVE_SCENEB_BUF){ 
				memset(instance->render_buffer->pixels, 0x0000, instance->render_buffer->width * instance->render_buffer->height * sizeof(uint16_t)); 
				for (int i = 0; i < _scene->_2D_Object_Ptr; i++)
				{
					uCanvas_universal_obj_t* obj = _scene->_2D_Objects[i];
                    push_element_to_display(obj,instance->render_buffer);
				}    
                if(instance->panel_1 != NULL)instance->panel_1->push_render_buffer(0, 0, instance->render_buffer);
                if(instance->panel_2 != NULL)instance->panel_2->push_render_buffer(0, 0, instance->render_buffer);
                UNLOCK_ACTIVE_SCENEB_BUF;
			}
        }
        else {
        }
	}
}

void uCanvas_Attach_Render_Task(uCanvas2D_Instance_t* instance) {
    if (instance == NULL) {
        printf("Invalid uCanvas2D_Instance_t\r\n");
        return;
    }
    active_scene = instance->active_scene;
    active_scene_mutex = xSemaphoreCreateBinary();
    UNLOCK_ACTIVE_SCENEB_BUF;
    xTaskCreatePinnedToCore(&uCanvas_renderer_task, "uCanvas_Render_Task", UCANVAS_RENDER_TASK_STACK_SIZE, instance, UCANVAS_RENDER_TASK_PRIORITY, &uCanvas_taskhandle, 1);
}

void uCanvas_manually_render_scene(void){
    if((active_scene != NULL) && (active_scene->_2D_Object_Ptr > 0)){
        if(LOCK_ACTIVE_SCENEB_BUF){ 
            memset(g_framebuffer->pixels, 0xFFFF, g_framebuffer->width * g_framebuffer->height * sizeof(uint16_t));  
            for (int i = 0; i < active_scene->_2D_Object_Ptr; i++)
            {
                uCanvas_universal_obj_t* obj = active_scene->_2D_Objects[i];
                if(obj->properties.visiblity == VISIBLE){
                    push_element_to_display(obj,g_framebuffer);
                }
            }
            g_panel->push_render_buffer(0, 0, g_framebuffer);
            UNLOCK_ACTIVE_SCENEB_BUF;
        }
    }
}

void uCanvas_Configure_Panel(uCanvas2D_Display_Panel_t* panel,
                             uCanvas2D_RenderBuffer_t* render_buffer){
    if(panel == NULL) return;
    g_panel = panel;
    g_framebuffer = render_buffer;  
}

void uCanvas_RenderBuffer_Init(uCanvas2D_RenderBuffer_t* buf, int width, int height) {
    if (buf == NULL) return;
    buf->width = width;
    buf->height = height;
    buf->pitch = width * sizeof(uint16_t);
    buf->offset_x = 0;
    buf->offset_y = 0;
    buf->pixels = (uint16_t*)malloc(width * height * sizeof(uint16_t));
    if (buf->pixels == NULL) {
        printf("Failed to allocate memory for render buffer\r\n");
        return;
    }
    memset(buf->pixels, 0x0000, width * height * sizeof(uint16_t)); // Initialize to white
}
uCanvas2D_RenderBuffer_t* uCanvas2D_RenderBuffer_Create(void) {
    uCanvas2D_RenderBuffer_t* _framebuffer = (uCanvas2D_RenderBuffer_t*)malloc(sizeof(uCanvas2D_RenderBuffer_t));
    if (_framebuffer == NULL) {
        printf("Failed to allocate memory for render buffer\r\n");
        return NULL;
    }
    return _framebuffer;
}

void start_uCanvas_engine(void){
    uCanvas_Configure_Panel(uCanvas2D_Get_Panel_Driver_EK79007(),
                             uCanvas2D_RenderBuffer_Create());

    uCanvas_RenderBuffer_Init(g_framebuffer, g_panel->width, g_panel->height);
    if (g_framebuffer->pixels == NULL) {
        printf("Failed to initialize render buffer\r\n");
        return;
    }
    upscale_render_buffer = (uCanvas2D_RenderBuffer_t*)malloc(sizeof(uCanvas2D_RenderBuffer_t));
    uCanvas_RenderBuffer_Init(upscale_render_buffer, 1024, 600);
    if (upscale_render_buffer->pixels == NULL) {
        printf("Failed to initialize upscale render buffer\r\n");
        return;
    }
    g_panel->init(1);
    g_panel->push_render_buffer(0, 0, g_framebuffer);
    g_panel->set_backlight(8000); // Set backlight to 100% by default

    panel_2 = uCanvas2D_Get_Panel_Driver_ST7789();
    if (panel_2 != NULL) {
        panel_2->init(1);
        panel_2->push_render_buffer(0, 0, g_framebuffer);
        panel_2->set_backlight(8000); // Set backlight to 100% by default
    } else {
        printf("Failed to get ST7789 panel driver\r\n");
    }

    active_scene_mutex = xSemaphoreCreateBinary();
    UNLOCK_ACTIVE_SCENEB_BUF;
    xTaskCreatePinnedToCore(&uCanvas_bg_render_engine_task,"Task",UCANVAS_RENDER_TASK_STACK_SIZE,NULL,UCANVAS_RENDER_TASK_PRIORITY,&uCanvas_taskhandle,1);
}

uCanvas2D_Instance_t* New_uCanvas_Instance(uCanvas_Scene_t* scene, uCanvas2D_Display_Panel_t* panel_1,uCanvas2D_Display_Panel_t* panel_2) {
    printf("Creating new uCanvas2D_Instance_t\r\n");
    uCanvas2D_Instance_t* instance = (uCanvas2D_Instance_t*)malloc(sizeof(uCanvas2D_Instance_t));
    if (instance == NULL) {
        printf("Failed to allocate memory for uCanvas2D_Instance_t\r\n");
        return NULL;
    }

    instance->active_scene = scene;
    instance->panel_1 = panel_1;
    instance->panel_2 = panel_2;
    // Create a new render buffer for the instance
    instance->render_buffer = uCanvas2D_RenderBuffer_Create();
    instance->render_buffer->width = panel_1->width;
    instance->render_buffer->height = panel_1->height;
    if (instance->render_buffer->pixels == NULL) {
        printf("Failed to initialize render buffer\r\n");
        free(instance);
        return NULL;
    }
    instance->render_buffer->pitch = 0;
    instance->render_buffer->offset_x = 0;
    instance->render_buffer->offset_y = 0;
    instance->render_buffer->pixels = (uint16_t*)malloc(panel_1->width * panel_1->height * sizeof(uint16_t));
    if (instance->render_buffer->pixels == NULL) {
        printf("Failed to allocate memory for render buffer pixels\r\n");
        free(instance->render_buffer);
        free(instance);
        return NULL;
    }
    memset(instance->render_buffer->pixels, 0x0000, panel_1->width * panel_1->height * sizeof(uint16_t)); // Initialize to black

    active_scene_mutex = xSemaphoreCreateBinary();
    UNLOCK_ACTIVE_SCENEB_BUF;
    if(instance->panel_1 != NULL){
        instance->panel_1->init(1);
        instance->panel_1->set_backlight(8000);
    }
    if(instance->panel_2 != NULL){
        instance->panel_2->init(1);
        instance->panel_2->set_backlight(8000);
    }
    active_scene = instance->active_scene;
    xTaskCreatePinnedToCore(&uCanvas_renderer_task, "uCanvas_Render_Task", UCANVAS_RENDER_TASK_STACK_SIZE, instance, UCANVAS_RENDER_TASK_PRIORITY, &uCanvas_taskhandle, 1);
    printf("uCanvas2D_Instance_t created successfully\r\n");
    return instance;
}



void pause_uCanvas_engine(void){
    vTaskSuspend(uCanvas_taskhandle);
}
void resume_uCanvas_engine(void){
    vTaskResume(uCanvas_taskhandle);
}