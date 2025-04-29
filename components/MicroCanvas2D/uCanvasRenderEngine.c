
#include "uCanvas_display_port.h"
#include "uCanvasRenderEngine.h"
extern SemaphoreHandle_t active_scene_mutex;
#define LOCK_ACTIVE_SCENEB_BUF      xSemaphoreTake(active_scene_mutex,portMAX_DELAY)
#define UNLOCK_ACTIVE_SCENEB_BUF    xSemaphoreGive(active_scene_mutex);
TaskHandle_t uCanvas_taskhandle;
extern uCanvas_Scene_t* active_scene;
#include "uCanvas_Frame_buffer.h"
static Framebuffer_t uCanvas_FrameBuffer;
extern FontxFile fx16G[2];
extern FontxFile fx24G[2];
extern FontxFile fx32G[2];
extern FontxFile fx32L[2];
extern FontxFile fx16M[2];
extern FontxFile fx24M[2];
extern FontxFile fx10M[2];

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
void IRAM_ATTR push_element_to_display(uCanvas_universal_obj_t* obj){
    uint16_t color = convertToRGB565(obj->properties.color);
    uint16_t x = obj->properties.position.x;
    uint16_t y = obj->properties.position.y;
    switch (obj->properties.type)
    {
    case RECTANGLE:{
        if(obj->properties.fill==FILL){
            uCanvas_framebuffer_draw_filled_rectangle(&uCanvas_FrameBuffer,x,y,obj->width,obj->height,color);
        }
        else {
            uCanvas_framebuffer_draw_rectangle(&uCanvas_FrameBuffer,x,y,obj->width,obj->height,color);
        }
        break;
    }

    case TEXTBOX : {
        FontxFile* activefont = NULL;
        switch (obj->font_properties.font_type)
        {
        case FONT_16G:	
            activefont = fx16G;
            break;
        case FONT_24G:	
            activefont = fx24G;
            break;
        case FONT_32G:
            activefont = fx32G;
            break;
        case FONT_32L:
            activefont = fx32L;
            break;
        case FONT_16M:
            activefont = fx16M;
            break;
        case FONT_24M :
            activefont = fx24M;
            break;
        case FONT_10M : 
            activefont = fx10M;
            break;
        default:
            activefont = fx10M;
            break;
        }
        if(activefont){
            uCanvas_Framebuffer_Draw_Text(&uCanvas_FrameBuffer,x,y,obj->text,activefont,color,color,uCanvas_Font_Dir_0,0);
        }
      break;
    }

    case CIRCLE : {
        if(obj->properties.fill==FILL){
            uCanvas_framebuffer_draw_filled_circle(&uCanvas_FrameBuffer,x,y,obj->r1,color);
        }
        else {
            uCanvas_framebuffer_draw_circle(&uCanvas_FrameBuffer,x,y,obj->r1,color);
        }
        break;
    }

    case LINE : {
        uCanvas_framebuffer_draw_line(&uCanvas_FrameBuffer,
            obj->point1.x,
            obj->point1.y,
            obj->point2.x,
            obj->point2.y,
            color
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

        if(obj->properties.fill==FILL)uCanvas_framebuffer_draw_filled_triangle(&uCanvas_FrameBuffer,p1.x,p1.y,p2.x,p2.y,p3.x,p3.y,color);
        else uCanvas_framebuffer_draw_triangle(&uCanvas_FrameBuffer,p1.x,p1.y,p2.x,p2.y,p3.x,p3.y,color);
      break;
    }

    case SPRITE2D: {
        uCanvas_framebuffer_draw_bitmap(&uCanvas_FrameBuffer,obj);
        break;
    }

    case ELLIPSE : {
        if(obj->properties.fill==FILL)uCanvas_framebuffer_draw_filled_ellipse(&uCanvas_FrameBuffer,x,y,obj->r1,obj->r2,color);
        else uCanvas_framebuffer_draw_ellipse(&uCanvas_FrameBuffer,x,y,obj->r1,obj->r2,color);
    }
    default:
      break;
    }
}
int64_t elapsed_time = 1;
int64_t time_to_draw_frame_buf = 1;
int64_t on_screen_draw_time = 1;
extern FontxFile fx16M[2];
void IRAM_ATTR uCanvas_bg_render_engine_task(void*arg){
    if(uCanvas_framebuffer_init(&uCanvas_FrameBuffer,240,320)){
        uCanvas_framebuffer_clear(&uCanvas_FrameBuffer,0x00);  
        while(1){ 
            
            vTaskDelay(pdMS_TO_TICKS(9));
            // uCanvas_Delay(1);
            
            if((active_scene != NULL) && (active_scene->_2D_Object_Ptr > 0)){
                if(LOCK_ACTIVE_SCENEB_BUF){ 
                    uCanvas_framebuffer_clear(&uCanvas_FrameBuffer,0x00);
                    // uCanvas_framebuffer_Draw_Char(&uCanvas_FrameBuffer,30,30,'S',fx16M,0xFFFF,RED,uCanvas_Font_Dir_0,1);
                    // uCanvas_Framebuffer_Draw_Text(&uCanvas_FrameBuffer,30,30,"Hello world",fx16M,0xFFFF,RED,uCanvas_Font_Dir_0,1);
                    for (int i = 0; i < active_scene->_2D_Object_Ptr; i++)
                    {
                        uCanvas_universal_obj_t* obj = active_scene->_2D_Objects[i];
                        if(obj->properties.visiblity == VISIBLE){
                            push_element_to_display(obj);
                        }
                        else {
                            // printf("hidden object\r\n");
                        }   
                    }

                    uCanvas_FrameBuf_Dispatch(&uCanvas_FrameBuffer);
                    UNLOCK_ACTIVE_SCENEB_BUF;
                    //printf("time to draw %dms", xTaskGetTickCount()-start);  
                }
            }
        }
    }
}

void uCanvas_manually_render_scene(void){
    if((active_scene != NULL) && (active_scene->_2D_Object_Ptr > 0)){
        if(LOCK_ACTIVE_SCENEB_BUF){ 
            uCanvas_Display_clear_buffer();
            
            for (int i = 0; i < active_scene->_2D_Object_Ptr; i++)
            {
                uCanvas_universal_obj_t* obj = active_scene->_2D_Objects[i];
                if(obj->properties.visiblity == VISIBLE){
                    push_element_to_display(obj);
                }
            }
           
            UNLOCK_ACTIVE_SCENEB_BUF;
        }
    }
}

extern TFT_t dev;



void start_uCanvas_engine(void){
    // uCanvas_Display_init();
    // uCanvas_Display_clear_buffer();
    uCanvas_Display_init();
    // spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);   
	// lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, 0, 0);
    uCanvas_Set_Display_Brightness(255);
    
    
        // uCanvas_FrameBuf_Dispatch(&uCanvas_FrameBuffer);
        // uCanvas_framebuffer_draw_filled_rectangle(&uCanvas_FrameBuffer,20,20,50,20,convertToRGB565((color_t){255,0,0}));
        // uCanvas_framebuffer_draw_rectangle(&uCanvas_FrameBuffer,20,20,50,20,convertToRGB565((color_t){255,255,0}));
        
        // uCanvas_framebuffer_draw_circle(&uCanvas_FrameBuffer,20,40,20,convertToRGB565((color_t){255,255,0}));
        // uCanvas_framebuffer_draw_filled_circle(&uCanvas_FrameBuffer,20,90,20,convertToRGB565((color_t){255,255,0}));
        
        // uCanvas_framebuffer_draw_ellipse(&uCanvas_FrameBuffer,100,90,20,30,convertToRGB565((color_t){255,255,0}));
        // uCanvas_framebuffer_draw_filled_ellipse(&uCanvas_FrameBuffer,100,90,20,30,convertToRGB565((color_t){255,255,0}));
    
    active_scene_mutex = xSemaphoreCreateBinary();
    UNLOCK_ACTIVE_SCENEB_BUF;

    xTaskCreatePinnedToCore(&uCanvas_bg_render_engine_task,"Task",UCANVAS_RENDER_TASK_STACK_SIZE,NULL,UCANVAS_RENDER_TASK_PRIORITY,&uCanvas_taskhandle,1);
}

void pause_uCanvas_engine(void){
    vTaskSuspend(uCanvas_taskhandle);
}
void resume_uCanvas_engine(void){
    vTaskResume(uCanvas_taskhandle);
}