#include "uCanvas_Physix.h"
#include "uCanvas_User_IO.h"
#include "math.h"
extern uCanvas_Scene_t* active_scene;

void controller_task(controller_properties_t* player_obj){
    float velocity = 0;       
    bool isJumping = false;
    uCanvas_Init_PushButton(player_obj->control_gpio);
    
    float floor_height = player_obj->floor_level;
    float position = floor_height;
    printf("controller_task\r\n");
    while (1)
    {
        if(!isJumping){
            if(!uCanvas_Get_PushbuttonState(player_obj->control_gpio)){
                printf("is jumping\r\n");
                isJumping = true;
                player_obj->control_object->state = true;
                velocity = player_obj->jump_height; 
                if(player_obj->user_callback_pre_jump!=NULL)player_obj->user_callback_pre_jump();
            }
        }
        if (isJumping) {
            velocity += player_obj->gravity * 0.1;
            position -= velocity * 0.1;
            if (position >= floor_height) {
                position = floor_height;
                velocity = 0; 
                isJumping = false;
                player_obj->control_object->state = false;
                if(player_obj->user_callback_post_jump != NULL)player_obj->user_callback_post_jump();
            }
            player_obj->control_object->properties.position.y = position;
        }

        uCanvas_Delay(10);
    }
}

uCanvas_Animation_task_handle_t uCanvas_attach_type1_controller_script(uCanvas_universal_obj_t*obj,controller_properties_t* player_obj){
    player_obj->control_object = obj;
    return uCanvas_Add_Task((uCanvas_Animation_task_t)controller_task,player_obj,1);
}

void detach_type1_controller_script(uCanvas_Animation_task_handle_t taskhandle){
    uCanvas_Remove_Task(taskhandle);
}

void uCanvas_add_control_gpio(controller_properties_t* player_obj,uint32_t gpio_num){
    player_obj->control_gpio = gpio_num;
}

void uCanvas_set_gravity(controller_properties_t* player_obj,float gravity){
    player_obj->gravity = gravity;
}

void uCanvas_set_jump_height(controller_properties_t* player_obj,float jump_height){
    player_obj->jump_height = jump_height;
}

void uCanvas_set_floor_level(controller_properties_t* player_obj,float floor_leve){
    player_obj->floor_level = floor_leve;
}


void collison_dection_task(){
    while (1)
    {
        for (int i = 0; i < active_scene->_2D_Object_Ptr; i++)
        {
            uCanvas_universal_obj_t* obj = active_scene->_2D_Objects[i];
            if(obj->properties.collision_detection == true){

            }
        }
        
    }
}

Coordinate2D_t ucanvas_get_xy_circular_cordinates(int angle, int rx, int ry, int xoffset, int yoffset){
    Coordinate2D_t coordinates;
    double theta = 2.0 * 3.14 * angle / 360;            
    uint8_t x = rx * cos(theta) + xoffset;
    uint8_t y = ry * sin(theta) + yoffset;
    coordinates.x = x;
    coordinates.y = y;
    return coordinates;
}