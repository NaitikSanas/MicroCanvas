
#include "uCanvas_display_port.h"
#include "uCanvasRenderEngine.h"
extern SemaphoreHandle_t active_scene_mutex;
#define LOCK_ACTIVE_SCENEB_BUF      xSemaphoreTake(active_scene_mutex,portMAX_DELAY)
#define UNLOCK_ACTIVE_SCENEB_BUF    xSemaphoreGive(active_scene_mutex);
TaskHandle_t uCanvas_taskhandle;
extern uCanvas_Scene_t* active_scene;





// static inline uint16_t rgba565_to_rgb565(uint16_t rgba565) {
//     // Mask to extract RGB components
//     uint16_t r = (rgba565 & 0xF800); // Red (5 bits)
//     uint16_t g = (rgba565 & 0x07E0); // Green (6 bits)
//     uint16_t b = (rgba565 & 0x003E) >> 1; // Blue (5 bits)
    
//     // Combine RGB without the alpha channel
//     uint16_t rgb565 = r | g | b;
//     return rgb565;
// }

void ssd1306_sprite_draw(uCanvas_universal_obj_t* obj){
    uint16_t offset_x       = obj->properties.position.x;
    uint16_t offset_y       = obj->properties.position.y;
    uint16_t sprite_width   = obj->sprite_obj->width;
    uint16_t sprite_height  = obj->sprite_obj->height;
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
    c.monochrome_pixel = obj->invert_sprite_pixels ? !obj->sprite_obj->sprite_buf[i] : obj->sprite_obj->sprite_buf[i];
    pos.x = x_ptr + offset_x;
    pos.y = y_ptr + offset_y;
    if(obj->sprite_obj->sprite_buf[i]!=2)uCanvas_DrawPixel(pos,c);
    if(x_ptr < sprite_width-1){
        x_ptr++;
    }else{
        x_ptr=0;
        y_ptr++;
    }
}
}
// void IRAM_ATTR st7789_draw_sprite_optimized(uCanvas_universal_obj_t* obj) {
//     // Precompute frequently used properties
//     uint16_t offset_x       = obj->properties.position.x;
//     uint16_t offset_y       = obj->properties.position.y;
//     uint16_t sprite_width   = obj->sprite_obj->width;
//     uint16_t sprite_height  = obj->sprite_obj->height;
//     uint16_t total_pixels   = sprite_width * sprite_height;
//     bool invert_pixels      = obj->invert_sprite_pixels;
//     uint16_t *sprite_buf    = obj->sprite_obj->sprite_buf;
//     bool flip_x             = obj->properties.flip_x;

//     if (obj->properties.visiblity == INVISIBLE) {
//         return;
//     }

//     // Precompute flip lookup table if necessary
//     uint16_t x_lookup[sprite_width];
//     if (flip_x) {
//         for (uint16_t x = 0; x < sprite_width; x++) {
//             x_lookup[x] = sprite_width - 1 - x;
//         }
//     }

//     // Loop through sprite buffer
//     uint16_t y_ptr = offset_y;
//     for (uint16_t row = 0; row < sprite_height; row++, y_ptr++) {
//         uint16_t base_index = row * sprite_width;

//         for (uint16_t col = 0; col < sprite_width; col++) {
//             uint16_t i = base_index + col;
//             uint16_t pixel = sprite_buf[i];

//             // Skip inactive pixels
//             if ((pixel & 0x01) == 0) {
//                 continue;
//             }

//             // Compute X position
//             uint16_t x_pos = flip_x ? x_lookup[col] + offset_x : col + offset_x;

//             // Draw pixel
//             uCanvas_DrawPixel565((Coordinate2D_t){x_pos, y_ptr}, rgba565_to_rgb565(pixel));
//         }
//     }
// }

// void IRAM_ATTR st7789_draw_sprite2(uCanvas_universal_obj_t* obj) {
//     // Precompute frequently used properties
//     uint16_t offset_x       = obj->properties.position.x;
//     uint16_t offset_y       = obj->properties.position.y;
//     uint16_t sprite_width   = obj->sprite_obj->width;
//     uint16_t sprite_height  = obj->sprite_obj->height;
//     uint16_t total_pixels   = sprite_width * sprite_height;
//     bool invert_pixels      = obj->invert_sprite_pixels;
//     uint16_t *sprite_buf = obj->sprite_obj->sprite_buf;
//     uint16_t flip_x = obj->properties.flip_x;
//     if (obj->properties.visiblity == INVISIBLE) {
//         return;
//     }

//     // Loop through sprite buffer
//     for (int i = 0, y_ptr = 0; i < total_pixels;) {
//         uint16_t x_ptr = 0;
//         for (int j = 0; j < sprite_width; j++, i++) {
//             // Only draw active pixels
//             if ((sprite_buf[i] & 0x01) == 1) {
//                 //  uCanvas_DrawPixel565((Coordinate2D_t){(obj->properties.flip_x ? (sprite_width - 1 - x_ptr) + offset_x : x_ptr + offset_x), (y_ptr + offset_y)},
//                                     //   adjust_pixel_contrast(rgba565_to_rgb565(obj->sprite_buffer[i]), 600));
//                  uCanvas_DrawPixel565((Coordinate2D_t){(flip_x ? (sprite_width - 1 - x_ptr) + offset_x : x_ptr + offset_x), (y_ptr + offset_y)},rgba565_to_rgb565(sprite_buf[i]));
                 
//             }
//             x_ptr++;
//         }

//         // Increment y_ptr after completing a row
//         y_ptr++;
//     }
// }


// static inline void st7789_draw_sprite(uCanvas_universal_obj_t* obj){
//     uint16_t offset_x       = obj->properties.position.x;
//     uint16_t offset_y       = obj->properties.position.y;
//     uint16_t sprite_width   = obj->sprite_obj->width;
//     uint16_t sprite_height  = obj->sprite_obj->height;
//     uint16_t x_ptr          = 0;
//     uint16_t y_ptr          = 0;
//     color_t c;
//     Coordinate2D_t pos;

//     if (obj->properties.visiblity == INVISIBLE) {
//         printf("[Warn]: It Should Not Return From Here \r\n");
//         return;
//     }

//     for (int i = 0; i < (sprite_width * sprite_height); i++) {
//         // printf("index %d\r\n",i);
//         // Get the color with possible inversion
//         c.monochrome_pixel = obj->invert_sprite_pixels ? !obj->sprite_obj->sprite_buf[i] : obj->sprite_obj->sprite_buf[i];

//         // Adjust the X position based on the flip_x flag
//         if (obj->properties.flip_x) {
//             pos.x = (sprite_width - 1 - x_ptr) + offset_x;  // Flip horizontally
//         } else {
//             pos.x = x_ptr + offset_x; // Normal position
//         }

//         // Set the Y position
//         pos.y = y_ptr + offset_y;

//         // Draw the pixel if necessary
//         if ((obj->sprite_obj->sprite_buf[i] & 0x01) == 1) {
//             uCanvas_DrawPixel565(pos,(rgba565_to_rgb565(obj->sprite_obj->sprite_buf[i])));
//             // uCanvas_DrawPixel565(pos,rgba565_to_rgb565(obj->sprite_buffer[i]));
//         }

//         // Update x_ptr and y_ptr
//         if (x_ptr < sprite_width - 1) {
//             x_ptr++;
//         } else {
//             x_ptr = 0;
//             y_ptr++;
//         }
//     }
// }
#include "st7789.h"
extern  TFT_t dev;
int64_t time_to_draw_element = 1;
void IRAM_ATTR push_element_to_display(uCanvas_universal_obj_t* obj){
    int64_t start_time = esp_timer_get_time();
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
        st7789_draw_sprite_batch(&dev,obj);
            // st7789_draw_sprite(obj);
            // st7789_draw_sprite2(obj);
            // st7789_draw_sprite_optimized(obj);
        #endif
        break;
    }
    default:
      break;
    }
    time_to_draw_element = esp_timer_get_time() - start_time;
}
int64_t elapsed_time = 1;
int64_t time_to_draw_frame_buf = 1;
int64_t on_screen_draw_time = 1;
void IRAM_ATTR uCanvas_bg_render_engine_task(void*arg){
    uCanvas_universal_obj_t* obj;
    while(1){ 
        int64_t start_time = esp_timer_get_time();
		vTaskDelay( pdMS_TO_TICKS(1));
		if((active_scene != NULL) && (active_scene->_2D_Object_Ptr > 0)){
			if(LOCK_ACTIVE_SCENEB_BUF){ 
				// uCanvas_Display_clear_buffer();
                int64_t start_time2 = esp_timer_get_time();
				for (int i = 0; i < active_scene->_2D_Object_Ptr; i++)
				{
					obj = active_scene->_2D_Objects[i];
					if(obj->properties.visiblity == VISIBLE){
						push_element_to_display(obj);
					}
					else {
						// printf("hidden object\r\n");
					}   
				}
                time_to_draw_frame_buf = esp_timer_get_time() - start_time2;
                int64_t start_time3 = esp_timer_get_time();
				uCanvas_Update_Display();
                on_screen_draw_time = esp_timer_get_time() - start_time3;
				UNLOCK_ACTIVE_SCENEB_BUF;
				//printf("time to draw %dms", xTaskGetTickCount()-start);  
			}
        }
        // Calculate elapsed time
        elapsed_time = esp_timer_get_time() - start_time;

        
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