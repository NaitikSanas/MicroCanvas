#include "stdio.h"
#include "uCanvas_api.h"

uCanvas_Scene_t* active_scene;

void uCanvas_push_object_to_activescene(uCanvas_universal_obj_t* obj){
    if(active_scene != NULL){
        if(LOCK_ACTIVE_SCENEB_BUF){
            active_scene->_2D_Objects[active_scene->_2D_Object_Ptr] = obj;
            active_scene->_2D_Object_Ptr++;
            UNLOCK_ACTIVE_SCENEB_BUF;
        }
    }
}

void uCanvas_Set_Text(uCanvas_universal_obj_t*obj,char*text){
    obj->text = text;
}

void uCanvas_Set_Obj_Type(uCanvas_universal_obj_t*obj,uCanvas_element_type_t type){
    obj->properties.type = type;
}

void uCanvas_Set_Color(uCanvas_universal_obj_t* obj, uint16_t r,uint16_t g, uint16_t b ){
    obj->properties.color.red = r;
    obj->properties.color.green = g;
    obj->properties.color.blue = b;
}
void uCanvas_Set_Radius(uCanvas_universal_obj_t* obj, uint16_t radius){
    obj->r1 = radius;
}
void uCanvas_Set_Monochrome_Color(uCanvas_universal_obj_t* obj, uint16_t color ){
    obj->properties.color.monochrome_pixel = color;
}

void uCanvas_Set_Position(uCanvas_universal_obj_t* obj, uint16_t xpos,uint16_t ypos ){
    obj->properties.position.x = xpos;
    obj->properties.position.y = ypos;
}

void uCanvas_Set_Width_Height(uCanvas_universal_obj_t* obj, uint16_t width,uint16_t height ){
    obj->width = width;
    obj->height = height;
}

void uCanvas_Set_Fill(uCanvas_universal_obj_t* obj, fill_t fill){
    obj->properties.fill = fill;
}


void uCanvas_Set_Visiblity(uCanvas_universal_obj_t* obj, visibility_ctrl_t vctrl){
    obj->properties.visiblity = vctrl;
}

uCanvas_Scene_t* New_uCanvas_Scene(void){
    uCanvas_Scene_t* scene_object = uCanvas_Scene_Object;
    scene_object->_2D_Object_Ptr = 0;
    for (size_t i = 0; i < 128; i++)
    {
    scene_object->_2D_Objects[i] = 0;
    }
    return scene_object;
}

void uCanvas_set_active_scene(uCanvas_Scene_t* scene){
    active_scene = scene;
}

uCanvas_universal_obj_t* New_uCanvas_2DRectangle(uint16_t xpos, uint16_t ypos, uint16_t h, uint16_t w){
    uCanvas_universal_obj_t* rect = uCanvas_Universal_Object;
    uCanvas_Set_Visiblity(rect,VISIBLE);
    uCanvas_Set_Obj_Type(rect, RECTANGLE);
    uCanvas_Set_Color(rect,255,255,255);
    uCanvas_Set_Monochrome_Color(rect,1);
    uCanvas_Set_Position(rect,xpos,ypos);
    uCanvas_Set_Width_Height(rect,w,h);
    uCanvas_Set_Fill(rect,NOFILL);
    uCanvas_push_object_to_activescene(rect);
    return rect;
}

void uCanvas_Set_Line_Coordinates(uCanvas_universal_obj_t*line, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
    line->point1.x = x1;
    line->point1.y = y1;
    line->point2.x = x2;
    line->point2.y = y2;
}

uCanvas_universal_obj_t* New_uCanvas_2DLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
    uCanvas_universal_obj_t* line = uCanvas_Universal_Object;
    
    uCanvas_Set_Line_Coordinates(line,x1,y2, x2,y2);
    uCanvas_Set_Visiblity(line,VISIBLE);
    uCanvas_Set_Obj_Type(line, LINE);
    uCanvas_Set_Color(line,255,255,255);
    uCanvas_Set_Monochrome_Color(line,1);
    uCanvas_Set_Fill(line,NOFILL);
    uCanvas_push_object_to_activescene(line);
    return line;
}

uCanvas_universal_obj_t* New_uCanvas_2DTextbox(char* text, uint16_t xpos, uint16_t ypos){
    uCanvas_universal_obj_t* textbox = uCanvas_Universal_Object;
    textbox->text = (char*) malloc(256);
    textbox->text = text;
    uCanvas_Set_Visiblity(textbox,VISIBLE);
    uCanvas_Set_Obj_Type(textbox, TEXTBOX);
    uCanvas_Set_Color(textbox,255,255,255);
    uCanvas_Set_Monochrome_Color(textbox,1);
    uCanvas_Set_Position(textbox,xpos,ypos);
    uCanvas_Set_Fill(textbox,NOFILL);
    uCanvas_push_object_to_activescene(textbox);
  return textbox;
}

uCanvas_universal_obj_t* New_uCanvas_2DCircle(uint16_t xpos, uint16_t ypos,uint16_t radius){
    uCanvas_universal_obj_t* circle = uCanvas_Universal_Object;
    uCanvas_Set_Visiblity(circle,VISIBLE);
    uCanvas_Set_Obj_Type(circle, CIRCLE);
    uCanvas_Set_Radius(circle,radius);
    uCanvas_Set_Color(circle,255,255,255);
    uCanvas_Set_Monochrome_Color(circle,1);
    uCanvas_Set_Position(circle,xpos,ypos);
    uCanvas_Set_Fill(circle,NOFILL);
    uCanvas_push_object_to_activescene(circle);
  return circle;
}


uCanvas_universal_obj_t* New_uCanvas_2DTriangle(_2dPoint_t Point1, _2dPoint_t Point2, _2dPoint_t Point3){
    uCanvas_universal_obj_t* triangle = uCanvas_Universal_Object;
    uCanvas_Set_Visiblity(triangle,VISIBLE);
    uCanvas_Set_Obj_Type(triangle, TRIANGLE);
    uCanvas_Set_Color(triangle,255,255,255);
    uCanvas_Set_Monochrome_Color(triangle,1);
    uCanvas_Set_Fill(triangle,NOFILL);
    triangle->point1 = Point1;
    triangle->point2 = Point2;
    triangle->point3 = Point3;
    uCanvas_push_object_to_activescene(triangle);
    return triangle;
}

void uCanvas_Animate_Text_Reveal(uCanvas_universal_obj_t*obj, char* text, uint16_t delay){
    char tmp[512] = {0};
    for (int i = 0; i < strlen(text); i++)
    {
        tmp[i] = text[i];
        uCanvas_Set_Text(obj,tmp);
        uCanvas_Delay(delay);
    }
}

uCanvas_Animation_task_handle_t uCanvas_Add_Task(uCanvas_Animation_task_t animation_loop){
    uCanvas_Animation_task_handle_t task_handle = NULL;
    xTaskCreate(animation_loop,"Task2",4096,NULL,1,task_handle);
    return task_handle;
}

void uCanvas_Remove_Task(uCanvas_Animation_task_handle_t handle){
    vTaskDelete(handle);
}   

void uCanvas_Pause_Task(uCanvas_Animation_task_handle_t task_handle){
    vTaskSuspend(task_handle);
}

void uCanvas_Delay(uint16_t delay){
    vTaskDelay(delay);
}