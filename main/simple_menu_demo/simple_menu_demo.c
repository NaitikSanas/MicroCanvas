#include "simple_menu_demo.h"
uCanvas_Scene_t* scene1;

uCanvas_obj_t* selector;
static uint8_t selector_icon[9*9] = {   \
    1,1,1,1,1,0,0,0,0,                    \
    1,1,1,1,1,1,0,0,0,                    \
    1,1,1,1,1,1,1,0,0,                    \
    1,1,1,1,1,1,1,1,0,                    \
    1,1,1,1,1,1,1,1,1,                    \
    1,1,1,1,1,1,1,1,0,                    \
    1,1,1,1,1,1,1,0,0,                    \
    1,1,1,1,1,1,0,0,0,                    \
    1,1,1,1,1,0,0,0,0  
};

#define MENU_POSITION_X 10
#define MENU_POSITION_Y 10
#define TEXT_OFFSET_X 10
#define SPAN_X 10
#define SPAN_Y 10

selection_menu_obj_t menu;
uint8_t state_counter[32]={0};

void onItemClicked(){
    char content[32];
    printf("Click Evt at %d\r\n",menu.cursor_index);
    state_counter[menu.cursor_index]++;
    sprintf(content,"Item %d > %d",menu.cursor_index,state_counter[menu.cursor_index]);
    menu_set_content(&menu,content,menu.cursor_index);
}

void simple_menu_demo_setup(void){
    start_uCanvas_engine();
    scene1 = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene1);

    uCanvas_Init_PushButton(47);
    uCanvas_Init_PushButton(48);
    uCanvas_Init_PushButton(45);
    
    menu.menu_position_x = MENU_POSITION_X;
    menu.menu_position_y = MENU_POSITION_Y;
    menu.span_x = SPAN_X;
    menu.span_y = SPAN_Y;
    menu.text_offset_x = 20;
    menu.click_handler = onItemClicked;

    menu_set_active_elements(&menu,32);
    menu_set_enable_cursor_index_text(&menu,true);
    menu_add_gpio_control(&menu,47,48,45);

    sprite2D_t selector_sprite;
    uCanvas_Compose_2DSprite_Obj(&selector_sprite,selector_icon,9,9);
    selector = New_uCanvas_2DSprite(selector_sprite, MENU_POSITION_X,MENU_POSITION_Y);    
    create_menu(&menu,selector);

    // selector = New_uCanvas_2DRectangle(0,28,10,128);
}

void simple_menu_demo_App_Main(void){
    printf("Index = %d\r\n",menu_get_current_index(&menu));
    uCanvas_Delay(100);
}