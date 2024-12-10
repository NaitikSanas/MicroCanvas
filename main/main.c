#include "main.h"
#include "app.h"
#include "simple_menu_demo.h"
#include "design_game_map.h"
#include "uCanvas_Physix.h"
#include "sprite_collection.h"
// #define RUN_APP_C 1
// #define RUN_DESIGN_GAME_MAP_DEMO 1
// #define RUN_SIMPLE_MENU_DEMO 1
// #define RUN_2D_SPRITE_DEMO 1
// array size is 8192

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

void build_rpg_map() {
    for (int row = 0; row < 10; row++) {
        for (int col = 0; col < 8; col++) {
            int type = rpg_map[row][col];
            if (type == 1) {  // Only create objects for visible cells
                map_objects[row][col] = New_uCanvas_2DSprite(grass_sprite_obj3, col * 34, row * 34);
            }
            else if(type == 2){
                if(get_random_number(1,2))
                    map_objects[row][col] = New_uCanvas_2DSprite(grass_sprite_obj, col * 34, row * 34);
                else 
                    map_objects[row][col] = New_uCanvas_2DSprite(grass_sprite_obj, col * 34, row * 34);
            }
            else {
                // if(get_random_number(1,2)==1) map_objects[row][col] = New_uCanvas_2DSprite(floor_sprite_obj, col * 10, row * 10);
                // else  map_objects[row][col] = New_uCanvas_2DSprite(floor_sprite_obj2, col * 10, row * 10);
            }
        }
    }
}


void app_main(){
    #if RUN_APP_C
    uCanvas_Setup();
    while (1)
    {
        uCanvas_App_Main();
        uCanvas_Delay(1);
    }

    #elif RUN_SIMPLE_MENU_DEMO 

    simple_menu_demo_setup();
    while (1)
    {
        simple_menu_demo_App_Main();
    }
    #elif RUN_2D_SPRITE_DEMO
    
    sprite_demo(); // explore sprites_demo.c     

    #elif RUN_DESIGN_GAME_MAP_DEMO
    
        uCanvas_Setup1();
        while (1)
        {
            uCanvas_App_Main1();
            // uCanvas_Delay(1);
        }
        
    #endif
    
    start_uCanvas_engine();
    uCanvas_Scene_t *scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    New_uCanvas_2DRectangle(0,0,319,239);
    sprite2D_t character_sprite_obj1;
    sprite2D_t character_sprite_obj2;
    sprite2D_t character_sprite_obj3;
    sprite2D_t character_sprite_obj4;

    sprite2D_t character_run_sprite_obj1;
    sprite2D_t character_run_sprite_obj2;
    sprite2D_t character_run_sprite_obj3;
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
    // uCanvas_Compose_2DSprite_Obj(&house_sprite_obj,character_skeleton,64,64);
    uCanvas_Compose_2DSprite_Obj(&tree_sprite_obj,tree,48,64);
    
    build_rpg_map();
    uCanvas_universal_obj_t* character = New_uCanvas_2DSprite(character_sprite_obj2,50,150);
    // uCanvas_universal_obj_t* sprite1 = New_uCanvas_2DSprite(house_sprite_obj,100,20);
    uCanvas_universal_obj_t* tree1 = New_uCanvas_2DSprite(tree_sprite_obj,60,180);
    
    uCanvas_universal_obj_t* tree3 = New_uCanvas_2DSprite(tree_sprite_obj,-30,180);
    uCanvas_universal_obj_t* tree2 = New_uCanvas_2DSprite(tree_sprite_obj,20,180);
    // for (int i = 0; i < count; i++)
    // {
    //     /* code */
    // }
        // uCanvas_Change_Sprite_Source(character,character_sprite_obj2);
    //  = New_uCanvas_2DSprite(tree_sprite_obj,0,120);
    uCanvas_Delay(1);
    uCanvas_Init_PushButton(45);
    uCanvas_Init_PushButton(48);
    uCanvas_Init_PushButton(47);

    int dir = 0;
    while (1)
    {
        // if(sprite2->properties.position.x < 200){
        //     sprite2->properties.position.x++;
        // }
        // else sprite2->properties.position.x = -64;
		if(!uCanvas_Get_PushbuttonState(45)){
			dir = !dir;
		}
		
        if(!uCanvas_Get_PushbuttonState(48)){
            character->properties.flip_x = false;
			character->properties.position.x+= 5;
            uCanvas_Change_Sprite_Source(character,character_run_sprite_obj1);
			character->properties.position.x+= 5;
            uCanvas_Delay(5);
            uCanvas_Change_Sprite_Source(character,character_run_sprite_obj2);
			character->properties.position.x+= 5;
            uCanvas_Delay(5);
            uCanvas_Change_Sprite_Source(character,character_run_sprite_obj3);
            uCanvas_Delay(5);
        }
        else if(!uCanvas_Get_PushbuttonState(47)){
            character->properties.flip_x= true;
			character->properties.position.x-= 5;
            uCanvas_Change_Sprite_Source(character,character_run_sprite_obj1);
            uCanvas_Delay(5);
			character->properties.position.x-= 5;
            uCanvas_Change_Sprite_Source(character,character_run_sprite_obj2);
            uCanvas_Delay(5);
			character->properties.position.x-= 5;
            uCanvas_Change_Sprite_Source(character,character_run_sprite_obj3);
            uCanvas_Delay(5);
        }
        else {
            uCanvas_Change_Sprite_Source(character,character_sprite_obj1);
            uCanvas_Delay(5);
            uCanvas_Change_Sprite_Source(character,character_sprite_obj2);
            uCanvas_Delay(8);
            uCanvas_Change_Sprite_Source(character,character_sprite_obj3);
            uCanvas_Delay(7);
            uCanvas_Change_Sprite_Source(character,character_sprite_obj4);
            uCanvas_Delay(5);
        }
    }
    

}




