#include "app.h"
#include "uCanvas_api.h"
#include "uCanvas_display_port.h"
#include "sprite_collection.h"

int rpg_map[10][10] = {
    {1, 1, 1, 2, 2, 1, 1,1 },
    {1, 1, 1, 2, 2, 1, 1,1 },
    {1, 1, 1, 2, 2, 1, 1,1 },
    {2, 2, 2, 2, 2, 1, 1,1 },
    {2, 2, 2, 2, 2, 1, 1,1 },
    {2, 2, 2, 2, 2, 1, 1,1 },
    {1, 1, 1, 2, 2, 1, 1,1 },
    {1, 1, 1, 2, 2, 1, 1,1 },
    {1, 1, 1, 2, 2, 1, 1,1 },
    {1, 1, 1, 2, 2, 1, 1,1 },
};
uCanvas_universal_obj_t* map_objects[20][20] = {NULL};

sprite2D_t house_sprite_obj;
sprite2D_t tree_sprite_obj;
sprite2D_t grass_sprite_obj;
sprite2D_t grass_sprite_obj2;
sprite2D_t grass_sprite_obj3;
sprite2D_t character_sprite_obj1;
sprite2D_t character_sprite_obj2;
sprite2D_t character_sprite_obj3;
sprite2D_t character_sprite_obj4;

sprite2D_t character_run_sprite_obj1;
sprite2D_t character_run_sprite_obj2;
sprite2D_t character_run_sprite_obj3;

uCanvas_universal_obj_t* character;
void build_rpg_map() {
    for (int row = 0; row < 10; row++) {
        for (int col = 0; col < 8; col++) {
            int type = rpg_map[row][col];
            if (type == 1) {  // Only create objects for visible cells
                map_objects[row][col] = New_uCanvas_2DSprite(&grass_sprite_obj3, col * 34, row * 34);
            }
            else if(type == 2){
                // if(get_random_number(1,2)==1)
                    map_objects[row][col] = New_uCanvas_2DSprite(&grass_sprite_obj, col * 34, row * 34);
                // else 
                    // map_objects[row][col] = New_uCanvas_2DSprite(&grass_sprite_obj, col * 34, row * 34);
            }
            else {
                // if(get_random_number(1,2)==1) map_objects[row][col] = New_uCanvas_2DSprite(floor_sprite_obj, col * 10, row * 10);
                // else  map_objects[row][col] = New_uCanvas_2DSprite(floor_sprite_obj2, col * 10, row * 10);
            }
        }
    }
}

uCanvas_universal_obj_t* fps_counter;
void uCanvas_Game_Design_Demo_Setup(){
    start_uCanvas_engine();
    uCanvas_Scene_t *scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    // New_uCanvas_2DRectangle(0,0,319,239);
    
    uCanvas_Compose_2DSprite_Obj(&character_sprite_obj1,tile000,32,32);
    uCanvas_Compose_2DSprite_Obj(&character_sprite_obj2,tile001,32,32);
    uCanvas_Compose_2DSprite_Obj(&character_sprite_obj3,tile002,32,32);
    uCanvas_Compose_2DSprite_Obj(&character_sprite_obj4,tile003,32,32);

    uCanvas_Compose_2DSprite_Obj(&character_run_sprite_obj1,run_0,RUN_0_WIDTH,RUN_0_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&character_run_sprite_obj2,run_1,RUN_0_WIDTH,RUN_0_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&character_run_sprite_obj3,run_2,RUN_0_WIDTH,RUN_0_HEIGHT);

    uCanvas_Compose_2DSprite_Obj(&grass_sprite_obj3,Grass,34,34);
    uCanvas_Compose_2DSprite_Obj(&grass_sprite_obj,TX_Tileset_Grass,TX_TILESET_GRASS_WIDTH,TX_TILESET_GRASS_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&grass_sprite_obj2,TX_Tileset_Grass2,TX_TILESET_GRASS_WIDTH,TX_TILESET_GRASS_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&tree_sprite_obj,tree,48,64);

    uCanvas_Sprite_Adjust_Contrast(&grass_sprite_obj3,500);
    uCanvas_Sprite_Adjust_Contrast(&grass_sprite_obj,500);

    build_rpg_map();
    character = New_uCanvas_2DSprite(&character_sprite_obj2,50,150);

    uCanvas_universal_obj_t* tree1 = New_uCanvas_2DSprite(&tree_sprite_obj,60,180);
    
    uCanvas_universal_obj_t* tree3 = New_uCanvas_2DSprite(&tree_sprite_obj,-30,180);
    uCanvas_universal_obj_t* tree2 = New_uCanvas_2DSprite(&tree_sprite_obj,20,180);
    fps_counter = New_uCanvas_2DTextbox("",10,20);
    uCanvas_Delay(1);
    uCanvas_Init_PushButton(45);
    uCanvas_Init_PushButton(48);
    uCanvas_Init_PushButton(47);
}
int dir = 0;
extern int64_t elapsed_time;
extern int64_t time_to_draw_element;
extern int64_t time_to_draw_frame_buf;
extern int64_t on_screen_draw_time;
void uCanvas_Game_Design_Demo_Loop(void){
    printf("--------------------------------\r\n");
    printf("FPS: %lld \r\n", 1000000/elapsed_time );
    printf("Frame Time: %.2f ms \r\n", (float)elapsed_time/1000.0 );
    printf("On Screen Draw Time: %.2f ms \r\n", (float)on_screen_draw_time/1000.0 );
    printf("OSDPS : %.2f\r\n", (float)1000000.0/on_screen_draw_time );
    printf("Element Draw time: %.2f ms\r\n", (float)time_to_draw_element/1000.0 );
    printf("Frame Buf Prepare: %f ms\r\n", (float)time_to_draw_frame_buf/1000.0 );
    printf("FBPS: %f \r\n", (float)1000000.0/time_to_draw_frame_buf);
    // printf("x_flip: %d \r\n", character->properties.flip_x );
    printf("--------------------------------\r\n\r\n");
    
    sprintf(fps_counter->text,"Frame Time: %.2f ms", (float)elapsed_time/1000.0 );

    // if(!uCanvas_Get_PushbuttonState(45)){
    //     dir = !dir;
    // }

    if(!uCanvas_Get_PushbuttonState(48)){
        character->properties.flip_x = false;
        character->properties.position.x+= 5;
        uCanvas_Change_Sprite_Source(character,&character_run_sprite_obj1);
        character->properties.position.x+= 5;
        uCanvas_Delay(5);
        uCanvas_Change_Sprite_Source(character,&character_run_sprite_obj2);
        character->properties.position.x+= 5;
        uCanvas_Delay(5);
        uCanvas_Change_Sprite_Source(character,&character_run_sprite_obj3);
        uCanvas_Delay(5);
    }
    else if(!uCanvas_Get_PushbuttonState(47)){
        character->properties.flip_x= true;
        character->properties.position.x-= 5;
        uCanvas_Change_Sprite_Source(character,&character_run_sprite_obj1);
        uCanvas_Delay(5);
        character->properties.position.x-= 5;
        uCanvas_Change_Sprite_Source(character,&character_run_sprite_obj2);
        uCanvas_Delay(5);
        character->properties.position.x-= 5;
        uCanvas_Change_Sprite_Source(character,&character_run_sprite_obj3);
        uCanvas_Delay(5);
    }
    else {
        uCanvas_Change_Sprite_Source(character,&character_sprite_obj1);
        uCanvas_Delay(5);
        uCanvas_Change_Sprite_Source(character,&character_sprite_obj2);
        uCanvas_Delay(8);
        uCanvas_Change_Sprite_Source(character,&character_sprite_obj3);
        uCanvas_Delay(7);
        uCanvas_Change_Sprite_Source(character,&character_sprite_obj4);
        uCanvas_Delay(5);
    }
}