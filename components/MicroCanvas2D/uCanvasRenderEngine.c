
#include "uCanvas_display_port.h"
#include "uCanvasRenderEngine.h"

TaskHandle_t uCanvas_taskhandle;
extern uCanvas_Scene_t* active_scene;

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

    case SPRITE2D :{
      // printf("sprint2d-->\r\n");
      uint16_t offset_x       = obj->properties.position.x;
      uint16_t offset_y       = obj->properties.position.y;
      uint16_t sprite_width   = obj->sprite_resolution.x;
      uint16_t sprite_height  = obj->sprite_resolution.y;
      uint16_t x_ptr          = 0;
      uint16_t y_ptr          = 0;
      color_t c;
      Coordinate2D_t pos;
      if(obj->properties.visiblity == INVISIBLE)return;
      // printf("----start----\r\n");
      for (int i = 0; i < (sprite_width*sprite_height); i++)
      {
        
       
        c.monochrome_pixel = obj->invert_sprite_pixels ? !obj->sprite_buffer[i] : obj->sprite_buffer[i];
        pos.x = x_ptr + offset_x;
        pos.y = y_ptr + offset_y;
        // printf("(%d,%d),",pos.x,pos.y);
        // printf("(%d,%d) : %d|%d\r\n",x_ptr,y_ptr,i,obj->sprite_buffer[i]);
        if(obj->sprite_buffer[i]!=2)uCanvas_DrawPixel(pos,c);
        if(x_ptr < sprite_width-1){
          x_ptr++;
        }else{
          x_ptr=0;
          y_ptr++;
        }
      }
      // printf("----END----\r\n");
      break;
    }
    default:
      break;
    }
}

void uCanvas_bg_render_engine_task(void*arg){
    while(1){ 
    
		if((active_scene != NULL) && (active_scene->_2D_Object_Ptr > 0)){
			if(LOCK_ACTIVE_SCENEB_BUF){
      // int start = xTaskGetTickCount();  
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
        // if(obj->properties.type==SPRITE2D){
        //   printf("found\r\n");
        // }  
			}
			uCanvas_Update_Display();
      UNLOCK_ACTIVE_SCENEB_BUF;
      // printf("time to draw %dms", xTaskGetTickCount()-start);  

			}
		}
		uCanvas_Delay(1);

    
		}
}

void start_uCanvas_engine(void){
    uCanvas_Display_init();
    uCanvas_Display_clear_buffer();

    active_scene_mutex = xSemaphoreCreateBinary();
    UNLOCK_ACTIVE_SCENEB_BUF;

    xTaskCreatePinnedToCore(&uCanvas_bg_render_engine_task,"Task",UCANVAS_RENDER_TASK_STACK_SIZE,NULL,UCANVAS_RENDER_TASK_PRIORITY,uCanvas_taskhandle,1);
}