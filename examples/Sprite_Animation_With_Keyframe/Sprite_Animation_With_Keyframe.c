#include "Sprite_Animation_With_Keyframe.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "sprite_collection.h"
#include "websocket_server.h"
#include "uCanvas_Physix.h"
static uCanvas_Sprite_KeyFrames_t character_run_animation_keyframes = {
    .KeyFrame_Parameters.active_keyframes  = 6,
    .KeyFrame_Parameters.keyframe_time      = {70000, 70000, 70000, 70000, 70000, 70000},
    .KeyFrame_Parameters.sprite_postion_y   = {-5, +5, -5, +5, 0},  
    .KeyFrame_Parameters.sprite_postion_x   = {0, 0, 0, 0, 0, 0},
    .KeyFrame_Parameters.sprite_flip_x      = {0, 0, 0, 0, 0, 0},
    .KeyFrame_Parameters.sprite_flip_y      = {0, 0, 0, 0, 0, 0},  
};

static uCanvas_Sprite_KeyFrames_t grass_run_animation_keyframes = {
    .KeyFrame_Parameters.active_keyframes  = 3,
    .KeyFrame_Parameters.keyframe_time      = {90000, 90000, 90000, 90000, 90000, 90000},
    .KeyFrame_Parameters.sprite_postion_y   = {0,0,0,0,0,0},  
    .KeyFrame_Parameters.sprite_postion_x   = {0, 0, 0, 0, 0, 0},
    .KeyFrame_Parameters.sprite_flip_x      = {0, 0, 0, 0, 0, 0},
    .KeyFrame_Parameters.sprite_flip_y      = {0, 0, 0, 0, 0, 0},  
    .KeyFrame_Parameters.frame_time = 150000
};

sprite2D_t character_run_animation_sprite_set[6];
sprite2D_t grass_animation_sprite_set[6];

int char_pos_x = 50;
int char_pos_y = 320-32-45;
uCanvas_universal_obj_t* platform_grid[34][2]={0};

void wrapped_platform_render(int dir){
    int offset_X = 0;
    for (int i = 0; i < 10; i++)
    {
        platform_grid[i][0]->properties.position.x += dir;
        int th = offset_X - platform_grid[i][0]->width;
        if(platform_grid[i][0]->properties.position.x < th){
            platform_grid[i][0]->properties.position.x = 9*GRASS_PLATFORM3_WIDTH-1;
        }
    }
}

void platform(){
    while(1){
        wrapped_platform_render(-1);
        ets_delay_us(2400);
        // uCanvas_Delay(1);
    }
}
int animate = 1;
void pre_jump(){
    animate = 0;
}
void post_jump(){
    animate = 1;
}
void task(){
    uCanvas_Compose_2DSprite_Obj(&character_run_animation_sprite_set[0],run_0,RUN_0_WIDTH,RUN_0_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&character_run_animation_sprite_set[1],run_1,RUN_1_WIDTH,RUN_1_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&character_run_animation_sprite_set[2],run_2,RUN_2_WIDTH,RUN_2_HEIGHT);

    uCanvas_Compose_2DSprite_Obj(&character_run_animation_sprite_set[3],run_0,RUN_0_WIDTH,RUN_0_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&character_run_animation_sprite_set[4],run_1,RUN_1_WIDTH,RUN_1_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&character_run_animation_sprite_set[5],run_2,RUN_2_WIDTH,RUN_2_HEIGHT);

    uCanvas_universal_obj_t* b1 = New_uCanvas_2DRectangle(39+10,40,33,33);
    uCanvas_universal_obj_t* b2 = New_uCanvas_2DRectangle(39+32+10,40,33,33);
    uCanvas_universal_obj_t* b3 = New_uCanvas_2DRectangle(39+32+32+10,40,33,33);
    uCanvas_Set_Color(b1,255,255,0);
    uCanvas_Set_Color(b2,255,255,0);
    uCanvas_Set_Color(b3,255,255,0);

    New_uCanvas_2DSprite(&character_run_animation_sprite_set[0],40+10,40);
    New_uCanvas_2DSprite(&character_run_animation_sprite_set[1],40+32+10,40);
    New_uCanvas_2DSprite(&character_run_animation_sprite_set[2],40+32+32+10,40);

    character_run_animation_keyframes.main_sprite = New_uCanvas_2DSprite(&character_run_animation_sprite_set[0],char_pos_x,char_pos_y);
    uCanvas_Sprite_Adjust_Contrast(&character_run_animation_sprite_set[0],512);
    uCanvas_Sprite_Adjust_Contrast(&character_run_animation_sprite_set[1],512);
    uCanvas_Sprite_Adjust_Contrast(&character_run_animation_sprite_set[2],512);

    uCanvas_universal_obj_t* textbox = New_uCanvas_2DTextbox("Frames :",40,30);
    uCanvas_Set_Color(textbox,255,255,0);
    textbox->font_properties.font_type = FONT_16G;
    

    printf("done\r\n");
    int dir = -1;

    controller_properties_t controller;
    controller.control_gpio = 10;
    controller.jump_height = 50;
    controller.floor_level = char_pos_y;
    controller.gravity = -9.8;
    controller.jump_velocity = 5;
    controller.user_callback_pre_jump = pre_jump;
    controller.user_callback_post_jump = post_jump;

    uCanvas_attach_type1_controller_script(character_run_animation_keyframes.main_sprite,&controller);

    printf("Current free heap size: %d bytes\n", esp_get_free_heap_size());
    while(1){
        if(animate)
            uCanvas_Play_Sprite_Animation(&character_run_animation_keyframes, character_run_animation_sprite_set);
        else uCanvas_Delay(1);
    }
}

void Run_Sprite_Animation_With_Keyframe() {
    
    start_uCanvas_engine();
    printf("size of obj %d\r\n",sizeof(uCanvas_universal_obj_t));
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    
    uCanvas_set_active_scene(scene);
    
    uCanvas_universal_obj_t* bg = New_uCanvas_2DRectangle(0,0,320,240);
    uCanvas_Set_Color(bg,116, 160, 214);
    bg->properties.fill = FILL;

    sprite2D_t platform1, platform2;
    uCanvas_Compose_2DSprite_Obj(&platform1,Grass_Platform3,GRASS_PLATFORM3_WIDTH,GRASS_PLATFORM3_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&platform2,Grass_Platform_2,GRASS_PLATFORM_2_WIDTH,GRASS_PLATFORM_2_HEIGHT);
    
    uCanvas_Sprite_Adjust_Contrast(&platform1,500);
    uCanvas_Sprite_Adjust_Contrast(&platform2,500);

    uCanvas_universal_obj_t* sun = New_uCanvas_2DCircle(240-50,320-120,20);
    uCanvas_Set_Color(sun,255,255,0);
    sun->properties.fill = FILL;

    for (int i = 0; i <=10; i++)
    {
        platform_grid[i][0] =  New_uCanvas_2DSprite(&platform1,i*GRASS_PLATFORM3_WIDTH,char_pos_y+32);
    }
       
    uCanvas_Add_Task(platform,NULL,0);
    
    task();
    
}
