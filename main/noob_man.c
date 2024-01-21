#include "noob_man.h"

int get_random_number(int min, int max) {
    return (esp_random() % (max - min + 1)) + min;
}

extern bool collided;

noob_man_t* New_Noob_Man_Instance(void){
    noob_man_t* noob_man = (noob_man_t*)malloc(sizeof(noob_man_t));
    
    noob_man->head = New_uCanvas_2DCircle(0,0,10);
    noob_man->head->properties.fill = FILL;

    noob_man->eye_left = New_uCanvas_2DCircle(0,0,2);
    noob_man->eye_left->properties.color.monochrome_pixel = 0;
    noob_man->eye_left->properties.fill = FILL;
    noob_man->eye_left->height = 1;
    noob_man->eye_left->width = 2;

    noob_man->eye_right = New_uCanvas_2DCircle(0,0,2); 
    noob_man->eye_right->properties.color.monochrome_pixel = 0;
    noob_man->eye_right->properties.fill = FILL;
    noob_man->eye_right->height = 1;
    noob_man->eye_right->width = 2;

    noob_man->mouth = New_uCanvas_2DRectangle(0,0,0,0);
    noob_man->mouth->properties.color.monochrome_pixel = 0;
    noob_man->mouth->properties.fill = FILL;
    noob_man->mouth->height = 3;
    noob_man->mouth->width  = 8;
    return noob_man;
}

void Set_Noob_Man_Position(noob_man_t*noob_man, int xpos, int ypos){
    noob_man->head->properties.position.x      = xpos;
    noob_man->head->properties.position.y      = ypos;
    //Eyes and Mouth moves relative to positions derived from head position
    noob_man->eye_left->properties.position.x  = xpos - 5;
    noob_man->eye_left->properties.position.y  = ypos - 2;
    noob_man->eye_right->properties.position.x = xpos + 5;
    noob_man->eye_right->properties.position.y = ypos - 2;
    noob_man->mouth->properties.position.x     = xpos - 4;
    noob_man->mouth->properties.position.y     = ypos + 3;
}




Coordinate2D_t Get_Noob_Man_Position (noob_man_t*noob_man){
    Coordinate2D_t pos;
    pos.x = noob_man->head->properties.position.x;
    pos.y = noob_man->head->properties.position.y;
    return pos;
}


void character_hit_animation(noob_man_t*noob_man){
    noob_man->eye_left->properties.type = RECTANGLE;
    noob_man->eye_right->properties.type = RECTANGLE; 
    noob_man->eye_left->properties.color.monochrome_pixel = 1;
    noob_man->eye_right->properties.color.monochrome_pixel = 1;
    noob_man->mouth->properties.color.monochrome_pixel = 1;
    noob_man->head->properties.fill = NOFILL;

    uCanvas_Delay(100);
    noob_man->eye_left->properties.type = CIRCLE;
    noob_man->eye_right->properties.type = CIRCLE;
    noob_man->head->properties.fill = FILL;
    noob_man->eye_left->properties.color.monochrome_pixel  = 0;
    noob_man->eye_right->properties.color.monochrome_pixel = 0;
    noob_man->mouth->properties.color.monochrome_pixel     = 0;
    uCanvas_Delay(100);
}