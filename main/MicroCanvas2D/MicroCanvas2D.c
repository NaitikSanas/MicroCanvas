#include "MicroCanvas2D.h"
#include "esp_log.h"
#include "freertos/semphr.h"

SemaphoreHandle_t cur_page_smphr = NULL;
page_t* current_page_ptr = NULL;
TaskHandle_t render_task_handle;

void set_current_page(page_t* page){
    if (xSemaphoreTake(cur_page_smphr, portMAX_DELAY) == pdTRUE){
        current_page_ptr = page;
        vTaskDelete(render_task_handle);
        xTaskCreate(&display_render_task,"Task",4096,NULL,1,&render_task_handle);
        xSemaphoreGive(cur_page_smphr);
    }
}


void page_init(page_t* page){
    page->elements_ptr = -1;
    page->page_idx = 0;
}

void destroy_page(page_t* page){
    free(page);
}

int add_into_elements_list(ucanvas_2d_obj* shape){
    if(current_page_ptr->elements_ptr < MAX_ELEMENTS){
        current_page_ptr->elements_ptr++;
        shape->index      = current_page_ptr->elements_ptr; 
        current_page_ptr->elements[current_page_ptr->elements_ptr] = shape;
        return 0;   
    }
    else{
        ESP_LOGI("UI","EBUF_FULL");
        //current_page_ptr->elements_ptr = -1;
        return 1;
    }
}

ucanvas_2d_obj* create_textbox(char* str, FontDef_t* font_type, int pos_x, int pos_y){
    // Create Object. 
    ucanvas_2d_obj* text_box = (ucanvas_2d_obj*)malloc(sizeof(ucanvas_2d_obj));
    shape_reposition(text_box,pos_x,pos_y);
    text_box->ucanvas_2d_objype = TEXT_BOX;
    text_box->font_type  = font_type;
    sprintf(text_box->str,"%s",str);
    set_visiblity(text_box,VISIBLE);
    if(add_into_elements_list(text_box))return NULL; //Push into elements array.
    return text_box;
}

ucanvas_2d_obj*  create_rectangle (int width, int height, int pos_x, int pos_y, fill_t fill){
    ucanvas_2d_obj* rect = (ucanvas_2d_obj*)malloc(sizeof(ucanvas_2d_obj));
    rect->x1         = height;
    rect->y1         = width;
    rect->pos_x      = pos_x;
    rect->pos_y      = pos_y;
    rect->ucanvas_2d_objype = RECTANGLE;
    rect->fill       = fill;
    rect->visibality = true;
    if(add_into_elements_list(rect))return NULL; //Push into elements array.
    return rect;
}

ucanvas_2d_obj* create_circle(int pos_x, int pos_y, int radius, int fill){
    ucanvas_2d_obj* circ = (ucanvas_2d_obj*)malloc(sizeof(ucanvas_2d_obj));
    circ->pos_x      = pos_x;
    circ->pos_y      = pos_y;
    circ->r1         = radius;
    circ->ucanvas_2d_objype = CIRCLE;
    circ->fill       = fill;
    circ->visibality = true;
    if(add_into_elements_list(circ))return NULL; //Push into elements array.
    return circ;
}


ucanvas_2d_obj*  create_line(int x1, int y1, int x2,int y2){
    ucanvas_2d_obj* line = (ucanvas_2d_obj*)malloc(sizeof(ucanvas_2d_obj));
    line->x1      = x1;
    line->y1      = y1;
    line->x2      = x2;
    line->y2      = y2;
    line->ucanvas_2d_objype = LINE;
    line->visibality = true;
    if(add_into_elements_list(line))return NULL; //Push into elements array.
    return line;
}

void render_current_element(int i){
    switch (current_page_ptr->elements[i]->ucanvas_2d_objype)
    {
    case RECTANGLE:
        if(current_page_ptr->elements[i]->fill == NOFILL){
        
        SSD1306_DrawRectangle(current_page_ptr->elements[i]->pos_x,current_page_ptr->elements[i]->pos_y,
                            current_page_ptr->elements[i]->y1,current_page_ptr->elements[i]->x1,1);
        }
        if(current_page_ptr->elements[i]->fill == FILL){
        SSD1306_DrawFilledRectangle(current_page_ptr->elements[i]->pos_x,current_page_ptr->elements[i]->pos_y,
                            current_page_ptr->elements[i]->y1,current_page_ptr->elements[i]->x1,1);
        }
        break;

    case CIRCLE :
        if(current_page_ptr->elements[i]->fill == NOFILL){
        SSD1306_DrawCircle(current_page_ptr->elements[i]->pos_x,current_page_ptr->elements[i]->pos_y,
                            current_page_ptr->elements[i]->r1, 1);
        }
        if(current_page_ptr->elements[i]->fill == FILL){
        SSD1306_DrawFilledCircle(current_page_ptr->elements[i]->pos_x,current_page_ptr->elements[i]->pos_y,
                            current_page_ptr->elements[i]->r1, 1);
        }
        break;

    case TEXT_BOX :
        SSD1306_GotoXY(current_page_ptr->elements[i]->pos_x,current_page_ptr->elements[i]->pos_y);
        SSD1306_Puts(current_page_ptr->elements[i]->str,current_page_ptr->elements[i]->font_type,1);
        break;

    case LINE:
        SSD1306_DrawLine(current_page_ptr->elements[i]->x1,
                         current_page_ptr->elements[i]->y1,
                         current_page_ptr->elements[i]->x2,
                         current_page_ptr->elements[i]->y2,
                         1
                        );
        break;
    default:
        break;
    }
}

void render_elements(void){
    while (1)
    {
        if (xSemaphoreTake(cur_page_smphr, portMAX_DELAY) == pdTRUE){
            SSD1306_Clear();      
            for(int i = 0; i <= current_page_ptr->elements_ptr; i++){  
                if(current_page_ptr->elements[i] != NULL && 
                current_page_ptr->elements[i]->visibality==true){ 
                        render_current_element(i);
                }
            }
            SSD1306_UpdateScreen();
            xSemaphoreGive(cur_page_smphr);
            // vTaskDelay(1);     
        }
        vTaskDelay(1);  
    }
}

void destroy_shape_obj(ucanvas_2d_obj* obj){
    if(obj != NULL){
        int idx = obj->index;
        current_page_ptr->elements[idx] = NULL;
        obj = NULL;
    }
}


void display_render_task (void){
    while (1)
    {
        render_elements();
        vTaskDelay(1);
    }  
}

void start_geomatery_engine(void){
    SSD1306_Init();
    cur_page_smphr = xSemaphoreCreateBinary();
    xSemaphoreGive(cur_page_smphr);
    xTaskCreate(&display_render_task,"Task",4096,NULL,1,&render_task_handle);
}

void stop_geomatery_engine(void){
    vTaskDelete(render_task_handle);
}


void set_line_p1(ucanvas_2d_obj* line,int x1,int y1){
    line->x1 = x1;
    line->y1 = y1;
}

void set_line_p2(ucanvas_2d_obj* line,int x2,int y2){
    line->x2 = x2;
    line->y2 = y2;
}

void shape_reposition(ucanvas_2d_obj* shape, int x, int y){
    shape->pos_x = x;
    shape->pos_y = y;
}

void set_textbox(ucanvas_2d_obj* textbox, char* str){
    sprintf(textbox->str,"%s", str);
}

void set_visiblity(ucanvas_2d_obj* shape, visibility_ctrl_t vc){
    shape->visibality = vc;
}

Vector2D_t get_distance(ucanvas_2d_obj* shape1, ucanvas_2d_obj* shape2 ){
    Vector2D_t vec;
    vec.x = shape1->pos_x - shape2->pos_x;
    vec.y = shape1->pos_y - shape2->pos_y;
    return vec;
}

rectangle_t create_rectangle_obj(int xpos, int ypos, int height, int width){
   rectangle_t rect;
   rect.shape_properties.fill = FILL;
   rect.shape_properties.position.x = xpos;
   rect.shape_properties.position.y = ypos;
   rect.shape_properties.visibality = VISIBLE;
   rect.width_height_xy.x = width;
   rect.width_height_xy.y = height;
   return rect;
}