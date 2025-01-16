#include "GUI_Demo.h"
#include "vector_graphics_app.h"
status_bar_t status_bar;
extern int app_state;
static tile_menu_t tile_menu;
uCanvas_universal_obj_t* panel_1;
uCanvas_Scene_t* scene;
void menu_task(void* arg);
static void create_game_app();
void show_hide_game(visibility_ctrl_t state);
void uCanvas_GUI_Demo_Setup() {
    start_uCanvas_engine();
    uCanvas_Init_PushButton(45);
    uCanvas_Init_PushButton(48);
    uCanvas_Init_PushButton(47);
    scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);

    // Create Background Fill
    uCanvas_universal_obj_t* bg = New_uCanvas_2DRectangle(0,0,320,240);
    uCanvas_Set_Color(bg,50,0,0);
    uCanvas_Set_Fill(bg,FILL);
    create_game_app();
    show_hide_game(INVISIBLE);  
    // Create area for status bar on top 
    uCanvas_universal_obj_t* status_bar_bg = New_uCanvas_2DRectangle(0,0,30,240);
    uCanvas_Set_Color(status_bar_bg,100,0,0);
    uCanvas_Set_Fill(status_bar_bg,FILL);

    //Create Panel_Object to create background for active application 
    panel_1 = New_uCanvas_2DRectangle(0,30,320-(32+20),240);
    panel_1->properties.visiblity = INVISIBLE;
    panel_1->properties.fill = FILL;
    panel_1->properties.color = tile_menu.color_1;

    //Create Area for task bar on bottom.
    uCanvas_universal_obj_t* task_bar = New_uCanvas_2DRectangle(0,320-(32+20),32+20,240);
    uCanvas_Set_Color(task_bar,100,0,0);
    uCanvas_Set_Fill(task_bar,FILL);
    
    //Create text box and Set properties for clock element
    status_bar.timestamp = New_uCanvas_2DTextbox("11:00PM",10,10+15);
    status_bar.timestamp->font_properties.font_type = FONT_16G;
    uCanvas_Set_Color(status_bar.timestamp,255,0,0);
    
    //Create network strenght indicator element
    status_bar.network_strength_indicator.position_x = 180;
    status_bar.network_strength_indicator.position_y = 20;
    status_bar.network_strength_indicator.gap_x = 5;
    status_bar.network_strength_indicator.height_max = 10;
    status_bar.network_strength_indicator.width = 2;
    status_bar.network_strength_indicator.active_color   = (color_t){.red = 255 , .green = 0, .blue = 0};
    status_bar.network_strength_indicator.inactive_color = (color_t){.red = 50  , .green = 0, .blue = 0};
    create_network_strength(&status_bar);
    uCanvas_Add_Task(rssi_monitor,NULL,0);
    uCanvas_Add_Task(clock_update_task,NULL,0);    

    // Create TaskBar Icon elements
    tile_menu.tile_menu_pos_x = 40;
    tile_menu.tile_menu_pos_y = 320-(32+10);
    tile_menu.tile_width =  32;
    tile_menu.tile_height = 32;
    tile_menu.tile_gap_x = 10;
    tile_menu.tile_gap_y = 10;
    tile_menu.tile_num_x = 4;
    tile_menu.tile_num_y = 1;
    tile_menu.tile_text_x = 5;
    tile_menu.tile_text_y = (tile_menu.tile_height/2);
    tile_menu.color_1 = (color_t){.red = 50, .green = 0,  .blue = 0};
    tile_menu.color_2 = (color_t){.red = 200,.green = 0,  .blue = 0};
    tile_menu.color_3 = (color_t){.red = 200,.green = 200,.blue = 0};
    create_tile_menu(&tile_menu);
    uCanvas_Add_Task(menu_task,NULL,0);

    start_vector_graphics_app(); 
    
    
    
}
uCanvas_universal_obj_t* title;
void expand_clock(){
    while (status_bar.timestamp->properties.position.y != 100)
    {
        status_bar.timestamp->properties.position.y++;
        ets_delay_us(1000);
    }
    title->properties.visiblity = VISIBLE;
    color_wipe(title,title->properties.color,(color_t){.red = 255,0,0},1000,50);
    uCanvas_Set_Color(status_bar.timestamp,50,0,0);
    status_bar.timestamp->font_properties.font_type = FONT_32L;
    color_wipe(status_bar.timestamp,status_bar.timestamp->properties.color,(color_t){.red = 255, 0, 0},600,100);  
}

void minimize_clock(){
    
    uCanvas_Set_Color(status_bar.timestamp,100,0,0);
    color_wipe(title,title->properties.color,(color_t){.red = 100,0,0},1000,90);
    
    status_bar.timestamp->font_properties.font_type = FONT_16G;
    
    while (status_bar.timestamp->properties.position.y != 25)
    {
        status_bar.timestamp->properties.position.y--;
        ets_delay_us(1000);
    }

    color_wipe(status_bar.timestamp,status_bar.timestamp->properties.color,(color_t){.red = 200, 0, 0},600,100);  
    title->properties.visiblity = INVISIBLE;  
}

int clock_state = 0;
int game_state = 0;

void menu_task(void* arg){
    title = New_uCanvas_2DTextbox("Clock App",10,30);
    title->font_properties.font_type = FONT_32L;
    title->properties.visiblity = INVISIBLE;
    uCanvas_Set_Color(title,100,0,0);

    uCanvas_Set_Text(tile_menu.tiles_label[0][0],"CLK");
    uCanvas_Set_Text(tile_menu.tiles_label[0][1],"WIFI");
    uCanvas_Set_Text(tile_menu.tiles_label[0][2],"GFX");
    uCanvas_Set_Text(tile_menu.tiles_label[0][3],"APP2");
    
    while (1)
    {
        if(!uCanvas_Get_PushbuttonState_WTR(47)){
            if(game_state == 0)
                update_cursor(&tile_menu,1);//pan_tile_menu(&tile_menu,'x',150,1,3000);
        }

        if(!uCanvas_Get_PushbuttonState_WTR(48)){
            printf("48_\r\n");
            color_wipe(tile_menu.tiles[tile_menu.selector_y][tile_menu.selector_x],tile_menu.tiles[tile_menu.selector_y][tile_menu.selector_x]->properties.color,(color_t){.red = 255,.green = 0,.blue = 0},1000,80);
            color_wipe(tile_menu.tiles[tile_menu.selector_y][tile_menu.selector_x],tile_menu.tiles[tile_menu.selector_y][tile_menu.selector_x]->properties.color,(color_t){.red = 200,.green = 0,.blue = 0},1000,80);   

            if(tile_menu.selector_x == 2 && tile_menu.selector_y == 0){
                show_hide_game(INVISIBLE);
                if(clock_state){
                    clock_state = 0;
                    minimize_clock();
                }
                printf("48_open\r\n");
                toggle_graphics_app_test();
                
                
            }
            
            if(tile_menu.selector_x == 0 && tile_menu.selector_y == 0){
                show_hide_game(INVISIBLE);
                if(app_state){
                    toggle_graphics_app_test();
                }
                if(clock_state==0){
                    clock_state = 1;
                    expand_clock();
                }
                else
                if(clock_state==1){
                    clock_state = 0;
                    minimize_clock();
                }

            }

            if(tile_menu.selector_x == 1 && tile_menu.selector_y == 0){
                if(app_state){
                    toggle_graphics_app_test();
                }
                if(clock_state==1){
                    clock_state = 0;
                    minimize_clock();
                }
                game_state = !game_state;
                show_hide_game(game_state);
            }  
            
        }
        uCanvas_Delay(1);
    }
}
void uCanvas_GUI_Demo_Main(void) {
    uCanvas_Delay(1000);
}


void rssi_monitor(){
    int strength = 0;
    while (1){
        update_network_strength(&status_bar,strength);
        if(strength < 5){
            strength++;
        }
        else strength = 0;
        uCanvas_Delay(50);
    }
}

void clock_update_task(){
    char buf[32]={0};
    uint8_t hour=0, minute=0, seconds=0;
   
    while (1)
    {

        if(seconds < 60){
            seconds++;
        }else {
            seconds=0;
            if(minute < 60)minute++;
            else{
                minute = 0;
                if(hour < 24)hour++;
                else hour = 0;
            }
        }
        
        sprintf(buf,"%02d:%02d:%02d",hour,minute,seconds);
        uCanvas_Set_Text(status_bar.timestamp,buf);
        uCanvas_Delay(100);
    }   
}
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
static uCanvas_universal_obj_t* map_objects[20][20] = {NULL};
 
static sprite2D_t house_sprite_obj;
static sprite2D_t tree_sprite_obj;
static sprite2D_t grass_sprite_obj;
static sprite2D_t grass_sprite_obj2;
static sprite2D_t grass_sprite_obj3;
static sprite2D_t character_sprite_obj1;
static sprite2D_t character_sprite_obj2;
static sprite2D_t character_sprite_obj3;
static sprite2D_t character_sprite_obj4;
 
static sprite2D_t character_run_sprite_obj1;
static sprite2D_t character_run_sprite_obj2;
static sprite2D_t character_run_sprite_obj3;
static uCanvas_universal_obj_t* character;
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

static uCanvas_universal_obj_t* fps_counter;
static uCanvas_universal_obj_t* tree1; 
static uCanvas_universal_obj_t* tree3;
static uCanvas_universal_obj_t* tree2;

static void game_animation(){
    printf("game_task_started \r\n");
    while (1)
    {
        if(game_state){
        if(!uCanvas_Get_PushbuttonState(45)){
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

        uCanvas_Delay(1);
    }
    
}

static void create_game_app(){
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
    uCanvas_Sprite_Adjust_Contrast(&tree_sprite_obj,500);
    uCanvas_Sprite_Adjust_Contrast(&character_run_sprite_obj1,500);
    uCanvas_Sprite_Adjust_Contrast(&character_run_sprite_obj2,500);
    uCanvas_Sprite_Adjust_Contrast(&character_run_sprite_obj3,500);

    uCanvas_Sprite_Adjust_Contrast(&character_sprite_obj1,500);
    uCanvas_Sprite_Adjust_Contrast(&character_sprite_obj2,500);
    uCanvas_Sprite_Adjust_Contrast(&character_sprite_obj3,500);
    uCanvas_Sprite_Adjust_Contrast(&character_sprite_obj4,500);

    build_rpg_map();
    character = New_uCanvas_2DSprite(&character_sprite_obj2,50,150);
    tree1 = New_uCanvas_2DSprite(&tree_sprite_obj,60,180);
    tree3 = New_uCanvas_2DSprite(&tree_sprite_obj,-30,180);
    tree2 = New_uCanvas_2DSprite(&tree_sprite_obj,20,180);
    fps_counter = New_uCanvas_2DTextbox("",10,20);
    xTaskCreatePinnedToCore(game_animation,"Task2",2048,NULL,1,NULL,1);
}

void show_hide_game(visibility_ctrl_t state){
    character->properties.visiblity = state;
    fps_counter->properties.visiblity = state;
    tree1->properties.visiblity = state; 
    tree3->properties.visiblity = state;
    tree2->properties.visiblity = state;
    for (int row = 0; row < 10; row++) {
        for (int col = 0; col < 8; col++) {
            int type = rpg_map[row][col];
            if(type == 2 || type == 1){
                // if(get_random_number(1,2)==1)
                    map_objects[row][col]->properties.visiblity = state;
            }
            else {
              
            }
        }
    }
}