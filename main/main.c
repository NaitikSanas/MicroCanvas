#include "main.h"
#include "app.h"

#include "uCanvas_Physix.h"

#include "Game_of_life.h"
#include "Sprite_Animation_With_Keyframe.h"
#include "L_System_Demo.h"
#include "3D_Cube.h"
#include "uCanvas_IO_Usage_Demo.h"
#include "space_explorer_game.h"
#include "Adv_Textbox_with_HID_Keyboard.h"
#include "pcm5102a.h"
#include "uCanvas_Synth.h"
#define RUN_APP_C 1
// #define RUN_UCANVAS_STRESS_TEST 1

// #define RUN_GAME_OF_LIFE_DEMO 1
// #define RUN_ADV_TEXTBOX_WITH_HID_KEYBOARD 1
// #define RUN_SPRITE_ANIMATION_WITH_KEYFRAME 1
// #define RUN_L_SYSTEM_DEMO 1
// #define RUN_3D_CUBE_DEMO 1
// #define RUN_IO_USAGE_DEMO 1
// #define RUN_SPACE_EXPLORER_DEMO 1
// #define RUN_UCANVAS_SYNTH 1

void app_main(){
    

    #if  RUN_APP_C
        uCanvas_App_Main();
    #elif RUN_GAME_OF_LIFE_DEMO
        start_game_of_life_demo();  
    #elif RUN_SIMPLE_MENU_DEMO 

    simple_menu_demo_setup();
    while (1)
    {
        simple_menu_demo_App_Main();
    }

    #elif RUN_SPRITE_ANIMATION_WITH_KEYFRAME
        Run_Sprite_Animation_With_Keyframe();

    #elif RUN_L_SYSTEM_DEMO
        L_System_Demo_Main();

    #elif RUN_3D_CUBE_DEMO
        Run_3D_Cube_Demo();

    #elif RUN_IO_USAGE_DEMO
        Run_uCanvas_IO_Usage_Demo();
    #elif RUN_SPACE_EXPLORER_DEMO
        Run_Space_Explorer_Game();

    #elif RUN_ADV_TEXTBOX_WITH_HID_KEYBOARD
        Adv_TextBox_With_USB_HID_Keyboard();

    #elif RUN_UCANVAS_SYNTH
    uCanvas_Synth();
    #endif
}