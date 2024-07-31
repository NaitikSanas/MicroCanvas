#include "uCanvas_Physix.h"
#include "uCanvas_User_IO.h"


void controller_task(controller_properties_t* player_obj){
    float velocity = 0;       
    bool isJumping = false;
    uCanvas_Init_PushButton(player_obj->control_gpio);
    
    int floor_height = player_obj->floor_level;
    float position = floor_height;
    while (1)
    {
        if(!isJumping){
            if(!uCanvas_Get_PushbuttonState(player_obj->control_gpio)){
                isJumping = true;
                velocity = player_obj->jump_height; 
            }
        }
        if (isJumping) {
            velocity += player_obj->gravity * 0.1;
            position -= velocity * 0.1;
            if (position >= floor_height) {
                position = floor_height;
                velocity = 0; 
                isJumping = false;
            }
        }
        player_obj->control_object->properties.position.y = position;
        // player_obj->control_object->properties.position.y = (uint16_t)position;
        uCanvas_Delay(1);
    }
}

uCanvas_Animation_task_handle_t uCanvas_attach_type1_controller_script(uCanvas_universal_obj_t*obj,controller_properties_t* player_obj){
    player_obj->control_object = obj;
    return uCanvas_Add_Task(controller_task,player_obj);
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

