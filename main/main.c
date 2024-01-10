#include "main.h"
#include "noob_man.h"
#include "uCanvas_User_IO.h"

#define PB0_PIN 20
noob_man_t* noob_man1;

void app_main(){
    start_uCanvas_engine(); /* Start uCanvas engine */
    uCanvas_Scene_t* main_scene = New_uCanvas_Scene(); /* Create New Scene instance */
    uCanvas_set_active_scene(main_scene); /*Set Newly create scene as Active scene to render*/
    noob_man1 = New_Noob_Man_Instance(); /* Call create to custom character object */
    uCanvas_Add_Task(noob_man1_controller,NULL); /* uCanvas Thread to Control noob_man1 Movements */
    uCanvas_Add_Task(character_blink_animation,noob_man1);  /* uCanvas Thread to Animate  noob_man1 Character */
}   

void noob_man1_controller(){
    float velocity = 0;
    float position = 53;
    bool isJumping = false;
    uCanvas_Init_PushButton(PB0_PIN);
    while (1)
    {
        if(!isJumping){
            if(!uCanvas_Get_PushbuttonState(PB0_PIN)){
                isJumping = true;
                velocity = JUMP_HEIGHT; 
            }
        }
        if (isJumping) {
            velocity += GRAVITY * 0.1;
            position -= velocity * 0.1;
            if (position >= 53) {
                position = 53;
                velocity = 0; 
                isJumping = false;
            }
        }
        Set_Noob_Man_Position(noob_man1,10,position);
        uCanvas_Delay(1);
    }
}

_2dPoint_t get_xy_cordinates(int angle, int rx, int ry, int xoffset, int yoffset){
    _2dPoint_t coordinates;
    double theta = 2.0 * PI * angle / 360;            
    uint8_t x = rx * cos(theta) + xoffset;
    uint8_t y = ry * sin(theta) + yoffset;
    coordinates.x = x;
    coordinates.y = y;
    return coordinates;
}