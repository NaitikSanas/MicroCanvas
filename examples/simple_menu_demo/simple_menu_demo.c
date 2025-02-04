#include "simple_menu_demo.h"
static uCanvas_Scene_t* scene1;
static uCanvas_Scene_t* scene2;
#include "ucanvas_button.h"

uCanvas_obj_t* selector1;
uCanvas_obj_t* selector2;
static uint16_t selector_icon[9*9] = {   \
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
#define MENU_POSITION_Y 18
#define TEXT_OFFSET_X 2
#define GAP_X 5
#define GAP_Y  15

static selection_menu_obj_t menu_1;
static selection_menu_obj_t menu_2;

prompt_t prompt_1;

void onContentClicked_Menu_2(void);
void onItemClicked_Menu_1(void);

void onScrollUp_Menu_1(void){
    printf("[Event]:Scroll-Up : idx %d\r\n",menu_get_current_index(&menu_1));
}
void onScrollDown_Menu_1(void){
    printf("[Event]:Scroll-Down : idx %d\r\n",menu_get_current_index(&menu_1));  
}

void create_menu_1_instace(void){
    uCanvas_set_active_scene(scene1);
    
    menu_1.menu_position_x = MENU_POSITION_X;
    menu_1.menu_position_y = MENU_POSITION_Y;
    menu_1.gap_x = GAP_X;
    menu_1.gap_y = GAP_Y;
    menu_1.text_offset_x = 20;

    menu_1.click_handler        = onItemClicked_Menu_1; //This function will be called when select button is pressed
    menu_1.scroll_up_handler    = onScrollUp_Menu_1;
    menu_1.scroll_down_handler  = onScrollDown_Menu_1;
    menu_1.select_btn_wait_to_release = true;
    menu_1.menu_content_font = FONT_16M;

    menu_set_active_elements(&menu_1,8); //Set Active Members of menu object
    menu_set_enable_cursor_index_text(&menu_1,true);//shows cursor position in corner
    menu_add_gpio_control(&menu_1,47,48,45); //Add GPIO to control UP/DOWN/SELECT navigation
    
    /*
        The selector or cursor can be any object like sprite, rectangle, circle etc.
    */
    static sprite2D_t selector_sprite;
    uCanvas_Compose_2DSprite_Obj(&selector_sprite,selector_icon,9,9);
    selector1 = New_uCanvas_2DSprite(&selector_sprite, MENU_POSITION_X,MENU_POSITION_Y);  

    /*Instantiate Menu Object*/
    create_menu(&menu_1,selector1);
    
    menu_set_title(&menu_1,"< Menu_1-Demo >"   ,20,0);
    
    New_uCanvas_2DLine(0,MENU_POSITION_Y-8,128,MENU_POSITION_Y-8);
    New_uCanvas_2DLine(0,MENU_POSITION_Y-7,128,MENU_POSITION_Y-7);

    /* Add Content in your menu object */
    menu_set_content(&menu_1,"Brightness",0);
    menu_set_content(&menu_1,"Contrast"  ,1);
    menu_set_content(&menu_1,"Volume"    ,2);
    menu_set_content(&menu_1,"Source"    ,3);
    menu_set_content(&menu_1,"Next-Page" ,4);
    
    
    prompt_1.box_h = 32;
    prompt_1.box_w = 100;
    prompt_1.prompt_position_x = 10;
    prompt_1.prompt_position_y = 20;
    prompt_1.relative_text_position_x = 5;
    prompt_1.relative_text_position_y = 5;

    create_prompt(&prompt_1);

}


void create_menu_2_instace(void){
    uCanvas_set_active_scene(scene2);
    menu_2.menu_position_x = MENU_POSITION_X;
    menu_2.menu_position_y = MENU_POSITION_Y;
    menu_2.gap_x = GAP_X;
    menu_2.gap_y = GAP_Y;
    menu_2.text_offset_x = 20;
    menu_2.click_handler = onContentClicked_Menu_2;
    menu_2.select_btn_wait_to_release = false;

    menu_set_active_elements(&menu_2,10);
    menu_set_enable_cursor_index_text(&menu_2,true);
    menu_add_gpio_control(&menu_2,47,48,45);
    
    sprite2D_t selector_sprite;
    uCanvas_Compose_2DSprite_Obj(&selector_sprite,selector_icon,9,9);
    selector2 = New_uCanvas_2DSprite(&selector_sprite, MENU_POSITION_X,MENU_POSITION_Y);  

    create_menu(&menu_2,selector2);
    menu_set_title(&menu_2,"< Menu_2-Demo >"   ,20,0);
    menu_set_content(&menu_2,"GOTO MENU-1",9);
    New_uCanvas_2DLine(0,MENU_POSITION_Y-8,128,MENU_POSITION_Y-8);
    New_uCanvas_2DLine(0,MENU_POSITION_Y-7,128,MENU_POSITION_Y-7);
    // New_uCanvas_2DLine(85,MENU_POSITION_Y-3,85,64);
}

void simple_menu_demo_setup(void){
    start_uCanvas_engine();
    scene1 = New_uCanvas_Scene(); //This holds Menu_1 Instance
    scene2 = New_uCanvas_Scene(); //This holds Menu_2 Instance
    if(scene2==NULL){
        printf("why null?");
    }
    //GPIOs for Rotory encoder
    uCanvas_Init_PushButton(47);
    uCanvas_Init_PushButton(48);
    uCanvas_Init_PushButton(45);
    
    create_menu_1_instace(); //creates menu_1 page_1
    create_menu_2_instace();//create menu_2 page 2
    uCanvas_set_active_scene(scene1);

    /*
        setting is_Active flag of menu instace to false allows disabling
        user input handling when the menu is no displayed on screen.
    */
    menu_set_active_state(&menu_1,true);
    menu_set_active_state(&menu_2,false);    
}
extern int64_t elapsed_time;
extern int64_t time_to_draw_element;
extern int64_t time_to_draw_frame_buf;
extern int64_t on_screen_draw_time;
   
void simple_menu_demo_App_Main(void){
    //  printf("--------------------------------\r\n");
    // printf("FPS: %lld \r\n", 1000000/elapsed_time );
    // printf("Frame Time: %.2f ms \r\n", (float)elapsed_time/1000.0 );
    // printf("On Screen Draw Time: %.2f ms \r\n", (float)on_screen_draw_time/1000.0 );
    // printf("OSDPS : %.2f\r\n", (float)1000000.0/on_screen_draw_time );
    // printf("Element Draw time: %.2f ms\r\n", (float)time_to_draw_element/1000.0 );
    // printf("Frame Buf Prepare: %f ms\r\n", (float)time_to_draw_frame_buf/1000.0 );
    // printf("FBPS: %f \r\n", (float)1000000.0/time_to_draw_frame_buf);
    // printf("--------------------------------\r\n\r\n");
    // printf("Index = %d\r\n",menu_get_current_index(&menu_1));
    uCanvas_Delay(10);
}

/*
    Handle user input events on Menu Instance 1
*/
void onItemClicked_Menu_1(){
    char content[32];
    int cursor_position = menu_get_current_index(&menu_1) ;
    /*
        Increment user_data field at cursor postion where click event was triggered. 
        We are not limited to this You can do anything else here. this example showcases usage of
        built in user_Data field that can be used to hold the state of UI, Other processes etc.
    */
    switch (cursor_position)
    {
    case 0:
        menu_1.user_data[cursor_position]++;
        sprintf(content,"Brightness %d",menu_1.user_data[cursor_position]);
        menu_set_content(&menu_1,content,cursor_position);
        break;
    
    case 1:
        menu_1.user_data[cursor_position]++;
        sprintf(content,"Contrast   %d",menu_1.user_data[cursor_position]);
        menu_set_content(&menu_1,content,cursor_position);
        break;
    case 2:
        menu_1.user_data[cursor_position]++;
        sprintf(content,"Volume     %d",menu_1.user_data[cursor_position]);
        menu_set_content(&menu_1,content,cursor_position);
        break;
    
    case 3:
        if(menu_1.user_data[cursor_position] ){
            show_prompt(&prompt_1);
        }else {
            hide_prompt(&prompt_1);
        }
        menu_1.user_data[cursor_position] = ! menu_1.user_data[cursor_position];
        sprintf(content,"Source     %d",menu_1.user_data[cursor_position]);
        menu_set_content(&menu_1,content,cursor_position);
        break;
    
    default:
        break;
    }
    /* When cursor is on position 4 that is memeber "Next-Page" We transition to Scene-2 and disabling this event handler 
        untill menu_1 is active again.
     */
    if(cursor_position == 4){
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
/*
    Handle user input events on Menu Instance 2
*/
void onContentClicked_Menu_2(void){
    printf("EVENT onContentClicked_Menu_2 \r\n");
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
        /*
            This block was my attempt to create transition animation between scene switch up and I think
            it does look cool.
        */
        uint16_t initial_pos = menu_1.content[0]->properties.position.x;

        for (int i = 0; i < menu_1.active_elements; i++)
        {
            menu_1.content[i]->properties.position.x = 130;
        }
        // uCanvas_Delay(50);
        uCanvas_set_active_scene(scene1);

        while (menu_1.content[0]->properties.position.x != initial_pos)
        {        
            for (int i = 0; i < menu_1.active_elements; i++)
            {
                menu_1.content[i]->properties.position.x--;
            }
            ets_delay_us(1000);
            
            // uCanvas_Delay(1);
        }  
        uCanvas_Delay(50);
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
