#include "stdio.h"
#include "uCanvas_api.h"
#include "esp_random.h"
uCanvas_Scene_t* active_scene;
SemaphoreHandle_t active_scene_mutex;
extern TaskHandle_t uCanvas_taskhandle;
#define LOCK_ACTIVE_SCENEB_BUF      xSemaphoreTake(active_scene_mutex,portMAX_DELAY)
#define UNLOCK_ACTIVE_SCENEB_BUF    xSemaphoreGive(active_scene_mutex);

void uCanvas_lock_scene(){
    LOCK_ACTIVE_SCENEB_BUF;
}
void uCanvas_unlock_scene(){
    UNLOCK_ACTIVE_SCENEB_BUF;
}
void uCanvas_delete_object(uCanvas_universal_obj_t* obj){
    if(active_scene != NULL){
        if(LOCK_ACTIVE_SCENEB_BUF){
            free(active_scene->_2D_Objects[obj->index]);
            if(active_scene->_2D_Object_Ptr > obj->index){
                int objects_to_reallocate = active_scene->_2D_Object_Ptr - obj->index;
                for (int i = 0; i < objects_to_reallocate; i++)
                {
                    active_scene->_2D_Objects[obj->index+i] = active_scene->_2D_Objects[obj->index+1];  
                    active_scene->_2D_Objects[obj->index+1]->index = obj->index+i;
                }
            }
            active_scene->_2D_Object_Ptr--;
            UNLOCK_ACTIVE_SCENEB_BUF;
        }
    }
}

void uCanvas_push_object_to_activescene(uCanvas_universal_obj_t* obj){
    // 
    if(active_scene != NULL){
        // if(LOCK_ACTIVE_SCENEB_BUF){
            obj->index = active_scene->_2D_Object_Ptr;
            active_scene->_2D_Objects[active_scene->_2D_Object_Ptr] = obj;
            active_scene->_2D_Object_Ptr++;
            
            UNLOCK_ACTIVE_SCENEB_BUF;
        // }
    }
}

void uCanvas_Set_Text(uCanvas_universal_obj_t*obj,char*text){
    if(obj){
        memset(obj->text,0,256);
        sprintf(obj->text,"%s",text);
    }
    else{
        printf("err:uCanvas_Set_Text\r\n");
    }
}

void uCanvas_Set_Obj_Type(uCanvas_universal_obj_t*obj,uCanvas_element_type_t type){
    if(obj){
        obj->properties.type = type;
    }
}

void uCanvas_Set_Color(uCanvas_universal_obj_t* obj, uint16_t r,uint16_t g, uint16_t b ){
    if(obj){
        obj->properties.color.red = r;
        obj->properties.color.green = g;
        obj->properties.color.blue = b;
    }
}
void uCanvas_Set_Radius1(uCanvas_universal_obj_t* obj, uint16_t radius){
    if(obj){
        obj->r1 = radius;
    }
}

void uCanvas_Set_Radius2(uCanvas_universal_obj_t* obj, uint16_t radius){
    if(obj){
        obj->r2 = radius;
    }
}
void uCanvas_Set_Monochrome_Color(uCanvas_universal_obj_t* obj, uint16_t color ){
    if(obj){
        obj->properties.color.monochrome_pixel = color;
    }
}

void uCanvas_Set_Position(uCanvas_universal_obj_t* obj, uint16_t xpos,uint16_t ypos ){
    if(obj){
        obj->properties.position.x = xpos;
        obj->properties.position.y = ypos;
    }
}

void uCanvas_Set_Width_Height(uCanvas_universal_obj_t* obj, uint16_t width,uint16_t height ){
    if(obj){
        obj->width = width;
        obj->height = height;
    }
}

void uCanvas_Set_Fill(uCanvas_universal_obj_t* obj, fill_t fill){
    if(obj){
        obj->properties.fill = fill;
    }
}


void uCanvas_Set_Visiblity(uCanvas_universal_obj_t* obj, visibility_ctrl_t vctrl){
    if(obj){
        obj->properties.visiblity = vctrl;
    }
}

uCanvas_Scene_t* New_uCanvas_Scene(void){
    uCanvas_Scene_t* scene_object = uCanvas_Scene_Object;
    scene_object->_2D_Object_Ptr = 0;
    for (size_t i = 0; i < MAX_ELEMENTS_NUM; i++)
    {
        scene_object->_2D_Objects[i] = 0;
    }
    return scene_object;
}

void uCanvas_set_active_scene(uCanvas_Scene_t* scene){

    
    // if(LOCK_ACTIVE_SCENEB_BUF){
        // vTaskSuspend(uCanvas_taskhandle);
        active_scene = scene;
    //     vTaskResume(uCanvas_taskhandle);
    //     UNLOCK_ACTIVE_SCENEB_BUF;
    // }
}

uCanvas_universal_obj_t* New_uCanvas_2DRectangle(uint16_t xpos, uint16_t ypos, uint16_t h, uint16_t w){
    uCanvas_universal_obj_t* rect = uCanvas_Universal_Object;
    if(rect == NULL){
        printf("[uCanvas]:ERROR : Failed to Allocate Memory!\r\n");
        return NULL;
    }
    uCanvas_Set_Visiblity(rect,VISIBLE);
    uCanvas_Set_Obj_Type(rect, RECTANGLE);
    uCanvas_Set_Color(rect,UCANVAS_DEFAULT_RED,UCANVAS_DEFAULT_GREEN, UCANVAS_DEFAULT_BLUE);
    uCanvas_Set_Monochrome_Color(rect,1);
    uCanvas_Set_Position(rect,xpos,ypos);
    uCanvas_Set_Width_Height(rect,w,h);
    uCanvas_Set_Fill(rect,NOFILL);
    uCanvas_push_object_to_activescene(rect);
    return rect;
}

void uCanvas_Set_Line_Coordinates(uCanvas_universal_obj_t*line, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
    if(line){
        line->point1.x = x1;
        line->point1.y = y1;
        line->point2.x = x2;
        line->point2.y = y2;
    }
}

uCanvas_universal_obj_t* New_uCanvas_2DLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
    uCanvas_universal_obj_t* line = uCanvas_Universal_Object;
    if(line==NULL){
        printf("[uCanvas]:ERROR : Failed to Allocate Memory!\r\n");
        return NULL;
    }
    uCanvas_Set_Line_Coordinates(line,x1,y1, x2,y2);
    uCanvas_Set_Visiblity(line,VISIBLE);
    uCanvas_Set_Obj_Type(line, LINE);
    uCanvas_Set_Color(line,UCANVAS_DEFAULT_RED,UCANVAS_DEFAULT_GREEN, UCANVAS_DEFAULT_BLUE);
    uCanvas_Set_Monochrome_Color(line,1);
    uCanvas_Set_Fill(line,NOFILL);
    uCanvas_push_object_to_activescene(line);
    return line;
}

uCanvas_universal_obj_t* New_uCanvas_2DTextbox(char* text, uint16_t xpos, uint16_t ypos){
    uCanvas_universal_obj_t* textbox = uCanvas_Universal_Object;
    textbox->text = (char*) malloc(UCANVAS_TEXTBOX_MAX_CONTNENT_SIZE*sizeof(uint8_t));
    memset(textbox->text,0,256);
    sprintf(textbox->text,"%s",text);
    textbox->font_properties.Font_Draw_Direction = uCanvas_Font_Dir_0;
    textbox->font_properties.font_type = FONT_10M;
    uCanvas_Set_Visiblity(textbox,VISIBLE);
    uCanvas_Set_Obj_Type(textbox, TEXTBOX);
    uCanvas_Set_Color(textbox,UCANVAS_DEFAULT_RED,UCANVAS_DEFAULT_GREEN, UCANVAS_DEFAULT_BLUE);
    uCanvas_Set_Monochrome_Color(textbox,1);
    uCanvas_Set_Position(textbox,xpos,ypos);
    uCanvas_Set_Fill(textbox,NOFILL);
    uCanvas_push_object_to_activescene(textbox);

  return textbox;
}

uCanvas_universal_obj_t* New_uCanvas_2DCircle(uint16_t xpos, uint16_t ypos,uint16_t radius){
    uCanvas_universal_obj_t* circle = uCanvas_Universal_Object;
    if(circle==NULL){
        printf("failed to alloc mem\r\n");
        return NULL;
    }
    uCanvas_Set_Visiblity(circle,VISIBLE);
    uCanvas_Set_Obj_Type(circle, CIRCLE);
    uCanvas_Set_Radius1(circle,radius);
    uCanvas_Set_Color(circle,UCANVAS_DEFAULT_RED,UCANVAS_DEFAULT_GREEN, UCANVAS_DEFAULT_BLUE);
    uCanvas_Set_Monochrome_Color(circle,1);
    uCanvas_Set_Position(circle,xpos,ypos);
    uCanvas_Set_Fill(circle,NOFILL);
    uCanvas_push_object_to_activescene(circle);
  return circle;
}

uCanvas_universal_obj_t* New_uCanvas_2DEllipse(uint16_t xpos, uint16_t ypos,uint16_t radius_x, uint16_t radius_y){
    uCanvas_universal_obj_t* ellipse = uCanvas_Universal_Object;
    if(ellipse==NULL){
        printf("failed to alloc mem\r\n");
        return NULL;
    }
    uCanvas_Set_Visiblity(ellipse,VISIBLE);
    uCanvas_Set_Obj_Type(ellipse, ELLIPSE);
    uCanvas_Set_Radius1(ellipse,radius_x);
    uCanvas_Set_Radius2(ellipse,radius_y);
    uCanvas_Set_Color(ellipse,UCANVAS_DEFAULT_RED,UCANVAS_DEFAULT_GREEN, UCANVAS_DEFAULT_BLUE);
    uCanvas_Set_Monochrome_Color(ellipse,1);
    uCanvas_Set_Position(ellipse,xpos,ypos);
    uCanvas_Set_Fill(ellipse,NOFILL);
    uCanvas_push_object_to_activescene(ellipse);
  return ellipse;
}



uCanvas_universal_obj_t* New_uCanvas_2DTriangle(Coordinate2D_t Point1, Coordinate2D_t Point2, Coordinate2D_t Point3){
    uCanvas_universal_obj_t* triangle = uCanvas_Universal_Object;
    uCanvas_Set_Visiblity(triangle,VISIBLE);
    uCanvas_Set_Obj_Type(triangle, TRIANGLE);
    uCanvas_Set_Color(triangle,UCANVAS_DEFAULT_RED,UCANVAS_DEFAULT_GREEN, UCANVAS_DEFAULT_BLUE);
    uCanvas_Set_Monochrome_Color(triangle,1);
    uCanvas_Set_Fill(triangle,NOFILL);
    triangle->point1 = Point1;
    triangle->point2 = Point2;
    triangle->point3 = Point3;
    triangle->properties.position.x = 0;
    triangle->properties.position.y = 0;
    uCanvas_push_object_to_activescene(triangle);
    return triangle;
}

uCanvas_universal_obj_t* New_uCanvas_2DSprite(sprite2D_t* sprite2D_obj,uint16_t pos_x, uint16_t pos_y){
    uCanvas_universal_obj_t* uCanvas_Sprite = uCanvas_Universal_Object;
    uCanvas_Set_Visiblity(uCanvas_Sprite,VISIBLE);
    uCanvas_Sprite->invert_sprite_pixels = false;
    uCanvas_Sprite->properties.type = SPRITE2D;
    uCanvas_Set_Monochrome_Color(uCanvas_Sprite,1);
    uCanvas_Sprite->sprite_buffer = sprite2D_obj->sprite_buf;
    uCanvas_Sprite->width  = sprite2D_obj->width;
    uCanvas_Sprite->height = sprite2D_obj->height;

    // uCanvas_Sprite->sprite_resolution.x = sprite2D_obj->width;
    // uCanvas_Sprite->sprite_resolution.y = sprite2D_obj->height;

    uCanvas_Sprite->properties.position.x = pos_x;
    uCanvas_Sprite->properties.position.y = pos_y;
    uCanvas_Sprite->properties.flip_x = 0;
    uCanvas_Sprite->properties.flip_y = 0;
    uCanvas_push_object_to_activescene(uCanvas_Sprite);
    // printf("[uCanvas]uCanvas_push_object_to_activescene\r\n");
    return uCanvas_Sprite;
}

void uCanvas_Sprite_Adjust_Contrast(sprite2D_t *sprite, int contrast) {
    // Validate inputs
    if (!sprite || !sprite->sprite_buf || sprite->width <= 0 || sprite->height <= 0) {
        return; // Invalid sprite
    }

    // Iterate over the sprite buffer
    uint16_t *buffer = sprite->sprite_buf; // Assuming sprite->buffer is a pointer to uint16_t
    size_t num_pixels = sprite->width * sprite->height;

    for (size_t i = 0; i < num_pixels; i++) {
        uint16_t pixel = buffer[i];

        // Extract RGB components and alpha channel
        uint8_t red = (pixel >> 11) & 0x1F;  // 5 bits for red
        uint8_t green = (pixel >> 6) & 0x1F; // 5 bits for green
        uint8_t blue = (pixel >> 1) & 0x1F;  // 5 bits for blue
        uint8_t alpha = pixel & 0x01;        // 1 bit for alpha

        // Midpoints for each color component
        const int red_mid = 16;    // Midpoint for 5-bit red (0-31)
        const int green_mid = 16;  // Midpoint for 5-bit green (0-31)
        const int blue_mid = 16;   // Midpoint for 5-bit blue (0-31)

        // Adjust contrast
        int red_adjusted = red_mid + (((red - red_mid) * contrast) >> 8);
        int green_adjusted = green_mid + (((green - green_mid) * contrast) >> 8);
        int blue_adjusted = blue_mid + (((blue - blue_mid) * contrast) >> 8);

        // Inline clamp for each component
        red_adjusted = (red_adjusted < 0) ? 0 : (red_adjusted > 31) ? 31 : red_adjusted;
        green_adjusted = (green_adjusted < 0) ? 0 : (green_adjusted > 31) ? 31 : green_adjusted;
        blue_adjusted = (blue_adjusted < 0) ? 0 : (blue_adjusted > 31) ? 31 : blue_adjusted;

        // Recombine components into the RRRRR GGGGG BBBBB A format
        buffer[i] = (red_adjusted << 11) | (green_adjusted << 6) | (blue_adjusted << 1) | alpha;
    }
}

void uCanvas_Animate_Text_Reveal(uCanvas_universal_obj_t*obj, char* text, uint16_t delay){
    char tmp[64] = {0};
    for (int i = 0; i < strlen(text); i++)
    {
        tmp[i] = text[i];
        uCanvas_Set_Text(obj,tmp);
        uCanvas_Delay(delay);
    }
    uCanvas_Set_Text(obj,text);
}

uCanvas_Animation_task_handle_t uCanvas_Add_Task(uCanvas_Animation_task_t animation_loop, void *arg, int core_id){
    TaskHandle_t task_handle = NULL;
    xTaskCreatePinnedToCore(animation_loop,"Task2",UCANVAS_TASK_STACK_SIZE,arg,0,&task_handle,core_id);
    return task_handle;
}

void uCanvas_Remove_Task(uCanvas_Animation_task_handle_t handle){
    vTaskDelete(handle);
}   

void uCanvas_Pause_Task(uCanvas_Animation_task_handle_t task_handle){
    vTaskSuspend(task_handle); 
}

void uCanvas_Resume_Task(uCanvas_Animation_task_handle_t task_handle){
    vTaskResume(task_handle);
}

void uCanvas_Delay(uint16_t delay){
    vTaskDelay(delay);
}

int get_random_number(int min, int max) {
    return (esp_random() % (max - min + 1)) + min;
}

void uCanvas_ScaleUp_SpriteBuf(uint16_t* src, uint16_t* dest, int src_width, int src_height, int scale_factor){
    int dest_width = src_width * scale_factor;
    int dest_height = src_height * scale_factor;

    for (int y = 0; y < dest_height; y++) {
        for (int x = 0; x < dest_width; x++) {
            int src_x = x / scale_factor;
            int src_y = y / scale_factor;
            dest[y * dest_width + x] = src[src_y * src_width + src_x];
        }
    }
}



void uCanvas_ScaleUp_Sprite2D(sprite2D_t* sprite_obj,uint16_t* reference,uint16_t* buffer, int h, int w, int scale_factor){
    int new_h = h*scale_factor;
    int new_w = w*scale_factor;
    memset(buffer,0,(new_h * new_w));
    uCanvas_ScaleUp_SpriteBuf(reference,buffer,w,h,scale_factor);
    uCanvas_Compose_2DSprite_Obj(sprite_obj,buffer,new_w,new_h);
}

   
   
void uCanvas_Change_Sprite_Source(uCanvas_universal_obj_t* obj, sprite2D_t* sprite_obj){
        if(LOCK_ACTIVE_SCENEB_BUF){
            // obj->sprite_obj = sprite_obj;
            obj->sprite_buffer       = sprite_obj->sprite_buf;
            obj->width = sprite_obj->width;
            obj->height = sprite_obj->height;
            UNLOCK_ACTIVE_SCENEB_BUF;
        }
}

void uCanvas_Compose_2DSprite_Obj(sprite2D_t* obj, uint16_t* sprite_buffer,uint16_t width, uint16_t height){
    obj->sprite_buf = sprite_buffer;
    obj->height = height;
    obj->width = width;
    // printf("composed sprite of %dx%d\r\n",width,height);
}
/**
 * Deleting the object from scene requires to 
 *  1. Free the object from location where it stored.
 *  2. Remove from Active scene objects list
 *  3. Re-arrange the list by right shifting other objects in list to prevent
 *     empty object block inside the list. 
 * 
 * **********       Logic      **********************
 *            obj to delete
 *                |      
 *[0][1][2][3][4][5][6][7][8] <- Object list or Array
 *                |        |
 *                |   _2D_Object_Ptr
 *                |<-- 3 ->|   <- obj_to_delete - _2D_object_ptr
 *
 * --- iterations = 2 = (obj_to_delete - _2D_object_ptr) - 1
 *    i = obj_to_delete_idx = 5;
 * 
 *    0 = 5 <- 6 = i <- i+1
 *    1 = 6 <- 7 = i <- i+1
 *                 
 */
void uCanvas_Delete_obj_from_scene(uCanvas_universal_obj_t* obj) {
    if (!obj) return;
    uint16_t obj_idx = obj->index;
    
    free(obj->text);  // Free allocated text memory
    free(obj);
    
    for (int i = obj_idx; i < active_scene->_2D_Object_Ptr - 1; i++) {
        active_scene->_2D_Objects[i] = active_scene->_2D_Objects[i + 1];
        active_scene->_2D_Objects[i]->index = i;
    }
    active_scene->_2D_Object_Ptr--;  // Reduce object count
}

void uCanvas_Delete_Scene(uCanvas_Scene_t* scene_obj){
    //First free up memory occupied by every objects stored in current scene
    for (size_t i = 0; i < scene_obj->_2D_Object_Ptr; i++)
    {
        free(scene_obj->_2D_Objects[i]);
    }
    
    free(scene_obj);  
}

void uCanvas_Play_Sprite_Animation(uCanvas_Sprite_KeyFrames_t* obj, sprite2D_t* sprite_set){
    if(obj){
        for (int i = 0; i < obj->KeyFrame_Parameters.active_keyframes; i++)
        {
            //Pre-Delay
            if(obj->KeyFrame_Parameters.keyframe_time[i]){
                ets_delay_us(obj->KeyFrame_Parameters.keyframe_time[i]);
            }
            //Draw New Frame
            uCanvas_Change_Sprite_Source(obj->main_sprite,&sprite_set[i]);
            obj->main_sprite->properties.position.x += obj->KeyFrame_Parameters.sprite_postion_x[i];
            obj->main_sprite->properties.position.y += obj->KeyFrame_Parameters.sprite_postion_y[i];
            obj->main_sprite->properties.flip_x  = obj->KeyFrame_Parameters.sprite_flip_x[i];
            
            //Delay by defined Frame time to transition to next frame
            ets_delay_us(obj->KeyFrame_Parameters.frame_time);
            // printf("frame update done\r\n\r\n");
        }    
    } 
}