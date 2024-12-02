#include "simple_menu_demo.h"
uCanvas_Scene_t* scene1;
uCanvas_Scene_t* scene2;

#include "ssd1306.h"
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

#define MENU_POSITION_X 5
#define MENU_POSITION_Y 14
#define TEXT_OFFSET_X 2
#define SPAN_X 5
#define SPAN_Y 10

static selection_menu_obj_t menu_1;
static selection_menu_obj_t menu_2;

void onItemClicked(){
    int cursor_position = menu_get_current_index(&menu_1) ;
    if(cursor_position != 9){
        menu_1.user_data[cursor_position]++;
        printf("Click Evt at %d Events\r\n",cursor_position);    
        char content[32];
        sprintf(content,"Item %d     %d",cursor_position,menu_1.user_data[cursor_position]);
        menu_set_content(&menu_1,content,cursor_position);
    }

    if(cursor_position == 9){
        printf("Changing scene....\r\n");
        uCanvas_set_active_scene(scene2);
        uCanvas_Delay(100);
        menu_set_active_state(&menu_1,false);
        menu_set_active_state(&menu_2,true);
    }

    if(menu_get_active_state(&menu_1)){    
        menu_1.cursor->properties.visiblity = INVISIBLE;
        uCanvas_Delay(10);
        menu_1.cursor->properties.visiblity = VISIBLE;
        uCanvas_Delay(10);
    }
}

void onContentClicked(void){
    printf("EVENT onContentClicked \r\n");
    int cursor_position = menu_get_current_index(&menu_2) ;
    if(cursor_position != 9){
        menu_2.user_data[cursor_position]++;
        printf("Click Evt at %d %d Events\r\n",cursor_position,menu_2.user_data[cursor_position]);
        
        char content[32];
        sprintf(content,"Content %d  %d",cursor_position,menu_2.user_data[cursor_position]);
        menu_set_content(&menu_2,content,cursor_position);
    }

    if(cursor_position == 9){
        printf("Changing scene....\r\n");
        uCanvas_set_active_scene(scene1);
        uCanvas_Delay(100);
        menu_set_active_state(&menu_1,true);
        menu_set_active_state(&menu_2,false);
    }

    if(menu_get_active_state(&menu_2)){
        menu_2.cursor->properties.visiblity = INVISIBLE;
        uCanvas_Delay(10);
        menu_2.cursor->properties.visiblity = VISIBLE;
        uCanvas_Delay(10);
    }
}

void create_menu_1_instace(void){
    uCanvas_set_active_scene(scene1);
    menu_1.menu_position_x = MENU_POSITION_X;
    menu_1.menu_position_y = MENU_POSITION_Y;
    menu_1.span_x = SPAN_X;
    menu_1.span_y = SPAN_Y;
    menu_1.text_offset_x = 20;
    menu_1.click_handler = onItemClicked;
    menu_1.select_btn_wait_to_release = true;

    menu_set_active_elements(&menu_1,10);
    menu_set_enable_cursor_index_text(&menu_1,true);
    menu_add_gpio_control(&menu_1,47,48,45);
    
    sprite2D_t selector_sprite;
    uCanvas_Compose_2DSprite_Obj(&selector_sprite,selector_icon,9,9);
    selector = New_uCanvas_2DSprite(selector_sprite, MENU_POSITION_X,MENU_POSITION_Y);  

    create_menu(&menu_1,selector);
    menu_set_title(&menu_1,"< Menu_1-Demo >"   ,20,0);
    menu_set_content(&menu_1,"GOTO MENU-2",9);
    New_uCanvas_2DLine(0,MENU_POSITION_Y-4,128,MENU_POSITION_Y-4);
    New_uCanvas_2DLine(0,MENU_POSITION_Y-3,128,MENU_POSITION_Y-3);
    // New_uCanvas_2DLine(85,MENU_POSITION_Y-3,85,64);
}


void create_menu_2_instace(void){
    uCanvas_set_active_scene(scene2);
    menu_2.menu_position_x = MENU_POSITION_X;
    menu_2.menu_position_y = MENU_POSITION_Y;
    menu_2.span_x = SPAN_X;
    menu_2.span_y = SPAN_Y;
    menu_2.text_offset_x = 20;
    menu_2.click_handler = onContentClicked;
    menu_2.select_btn_wait_to_release = false;

    menu_set_active_elements(&menu_2,10);
    menu_set_enable_cursor_index_text(&menu_2,true);
    menu_add_gpio_control(&menu_2,47,48,45);
    
    sprite2D_t selector_sprite;
    uCanvas_Compose_2DSprite_Obj(&selector_sprite,selector_icon,9,9);
    selector = New_uCanvas_2DSprite(selector_sprite, MENU_POSITION_X,MENU_POSITION_Y);  

    create_menu(&menu_2,selector);
    menu_set_title(&menu_2,"< Menu_2-Demo >"   ,20,0);
    menu_set_content(&menu_2,"GOTO MENU-1",9);
    New_uCanvas_2DLine(0,MENU_POSITION_Y-4,128,MENU_POSITION_Y-4);
    New_uCanvas_2DLine(0,MENU_POSITION_Y-3,128,MENU_POSITION_Y-3);
    // New_uCanvas_2DLine(85,MENU_POSITION_Y-3,85,64);
}

void simple_menu_demo_setup(void){
    start_uCanvas_engine();
    scene1 = New_uCanvas_Scene();
    scene2 = New_uCanvas_Scene();
    

    uCanvas_Init_PushButton(47);
    uCanvas_Init_PushButton(48);
    uCanvas_Init_PushButton(45);
    
    create_menu_1_instace();
    create_menu_2_instace();
    uCanvas_set_active_scene(scene1); 
    
    menu_set_active_state(&menu_1,true);
    menu_set_active_state(&menu_2,false);
}

void simple_menu_demo_App_Main(void){
    // printf("Index = %d\r\n",menu_get_current_index(&menu_1));
    uCanvas_Delay(100);
}