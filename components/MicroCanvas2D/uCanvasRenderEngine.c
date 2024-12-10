
#include "uCanvas_display_port.h"
#include "uCanvasRenderEngine.h"
extern SemaphoreHandle_t active_scene_mutex;
#define LOCK_ACTIVE_SCENEB_BUF      xSemaphoreTake(active_scene_mutex,portMAX_DELAY)
#define UNLOCK_ACTIVE_SCENEB_BUF    xSemaphoreGive(active_scene_mutex);
TaskHandle_t uCanvas_taskhandle;
extern uCanvas_Scene_t* active_scene;
uint16_t rgba565_to_rgb565(uint16_t rgba565) {
    // Mask to extract RGB components
    uint16_t r = (rgba565 & 0xF800); // Red (5 bits)
    uint16_t g = (rgba565 & 0x07E0); // Green (6 bits)
    uint16_t b = (rgba565 & 0x003E) >> 1; // Blue (5 bits)
    
    // Combine RGB without the alpha channel
    uint16_t rgb565 = r | g | b;
    return rgb565;
}

void ssd1306_sprite_draw(uCanvas_universal_obj_t* obj){
    uint16_t offset_x       = obj->properties.position.x;
    uint16_t offset_y       = obj->properties.position.y;
    uint16_t sprite_width   = obj->sprite_resolution.x;
    uint16_t sprite_height  = obj->sprite_resolution.y;
    uint16_t x_ptr          = 0;
    uint16_t y_ptr          = 0;
    color_t c;
    Coordinate2D_t pos;
    if(obj->properties.visiblity == INVISIBLE){
    printf("[Warn]:It Should Not Return From Here \r\n");
    return;
    }
    for (int i = 0; i < (sprite_width*sprite_height); i++)
    { 
    c.monochrome_pixel = obj->invert_sprite_pixels ? !obj->sprite_buffer[i] : obj->sprite_buffer[i];
    pos.x = x_ptr + offset_x;
    pos.y = y_ptr + offset_y;
    if(obj->sprite_buffer[i]!=2)uCanvas_DrawPixel(pos,c);
    if(x_ptr < sprite_width-1){
        x_ptr++;
    }else{
        x_ptr=0;
        y_ptr++;
    }
}
}
void push_element_to_display(uCanvas_universal_obj_t* obj){
    switch (obj->properties.type)
    {
    case RECTANGLE:{
        uCanvas_Draw_Rectangle(obj->properties.position.x, obj->properties.position.y,
		    obj->height,obj->width,obj->properties.color,obj->properties.fill);
        break;
    }

    case TEXTBOX : {
        uCanvas_Draw_Text(obj->text,obj->properties.position.x,obj->properties.position.y,obj->properties.color);
      break;
    }

    case CIRCLE : {
        uCanvas_Draw_Circle(obj->properties.position.x,obj->properties.position.y, obj->r1,obj->properties.color, obj->properties.fill);
        break;
    }

    case LINE : {
      uCanvas_Draw_Line(obj->point1,obj->point2,obj->properties.color);
      break;
    }

    case TRIANGLE : {
      uCanvas_Draw_Triangle(obj->point1,obj->point2,obj->point3,obj->properties.color,obj->properties.fill);
      break;
    }

    case SPRITE2D: {
        #ifdef USE_SSD1306
        ssd1306_sprite_draw(obj);
        #endif

        #ifdef USE_ST7789
            uint16_t offset_x       = obj->properties.position.x;
            uint16_t offset_y       = obj->properties.position.y;
            uint16_t sprite_width   = obj->sprite_resolution.x;
            uint16_t sprite_height  = obj->sprite_resolution.y;
            uint16_t x_ptr          = 0;
            uint16_t y_ptr          = 0;
            color_t c;
            Coordinate2D_t pos;

            if (obj->properties.visiblity == INVISIBLE) {
                printf("[Warn]: It Should Not Return From Here \r\n");
                return;
            }

            for (int i = 0; i < (sprite_width * sprite_height); i++) {
                // Get the color with possible inversion
                c.monochrome_pixel = obj->invert_sprite_pixels ? !obj->sprite_buffer[i] : obj->sprite_buffer[i];

                // Adjust the X position based on the flip_x flag
                if (obj->properties.flip_x) {
                    pos.x = (sprite_width - 1 - x_ptr) + offset_x;  // Flip horizontally
                } else {
                    pos.x = x_ptr + offset_x; // Normal position
                }

                // Set the Y position
                pos.y = y_ptr + offset_y;

                // Draw the pixel if necessary
                if ((obj->sprite_buffer[i] & 0x01) == 1) {
                    uCanvas_DrawPixel565(pos, rgba565_to_rgb565(obj->sprite_buffer[i]));
                }

                // Update x_ptr and y_ptr
                if (x_ptr < sprite_width - 1) {
                    x_ptr++;
                } else {
                    x_ptr = 0;
                    y_ptr++;
                }
            }
            break;
            #endif
    }
    default:
      break;
    }
}

void uCanvas_bg_render_engine_task(void*arg){
    while(1){ 
		uCanvas_Delay(1);
		if((active_scene != NULL) && (active_scene->_2D_Object_Ptr > 0)){
			if(LOCK_ACTIVE_SCENEB_BUF){ 
				uCanvas_Display_clear_buffer();
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
				uCanvas_Update_Display();
				UNLOCK_ACTIVE_SCENEB_BUF;
				//printf("time to draw %dms", xTaskGetTickCount()-start);  
			}
		}
		// vTaskDelayUntil()
		
	}
}

void start_uCanvas_engine(void){
    uCanvas_Display_init();
    uCanvas_Display_clear_buffer();

    active_scene_mutex = xSemaphoreCreateBinary();
    UNLOCK_ACTIVE_SCENEB_BUF;

    xTaskCreatePinnedToCore(&uCanvas_bg_render_engine_task,"Task",UCANVAS_RENDER_TASK_STACK_SIZE,NULL,UCANVAS_RENDER_TASK_PRIORITY,&uCanvas_taskhandle,1);
}