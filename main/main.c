#include "main.h"
#include "app.h"
#include "simple_menu_demo.h"
#include "design_game_map.h"
#include "uCanvas_Physix.h"
#include "fonts_demo.h"
#include "slider_example.h"
#include "GUI_Demo.h"
#include "Game_of_life.h"
#include "Sprite_Animation_With_Keyframe.h"
#include "L_System_Demo.h"
#include "3D_Cube.h"
#include "uCanvas_IO_Usage_Demo.h"
#include "space_explorer_game.h"
    
// #define RUN_APP_C 1

// #define RUN_DESIGN_GAME_MAP_DEMO 1
// #define RUN_SIMPLE_MENU_DEMO 1
// #define RUN_FONTS_TESTING 1
// #define RUN_SLIDER_EXAMPLE_TESTING 1
// #define RUN_GUI_DEMO_TESTING 1
// #define RUN_GAME_OF_LIFE_DEMO 1

// #define RUN_SPRITE_ANIMATION_WITH_KEYFRAME 1
// #define RUN_L_SYSTEM_DEMO 1
// #define RUN_3D_CUBE_DEMO 1
// #define RUN_IO_USAGE_DEMO 1
#define RUN_SPACE_EXPLORER_DEMO 1
void app_main(){

    #if RUN_APP_C
    uCanvas_Setup();
    while (1)
    {
        uCanvas_App_Main();
        // uCanvas_Delay(1);
    }
    #elif RUN_GAME_OF_LIFE_DEMO
        start_game_of_life_demo();  
    #elif RUN_SIMPLE_MENU_DEMO 

    simple_menu_demo_setup();
    while (1)
    {
        simple_menu_demo_App_Main();
    }

    #elif RUN_DESIGN_GAME_MAP_DEMO
    
        uCanvas_Game_Design_Demo_Setup();
        while (1)
        {
            uCanvas_Game_Design_Demo_Loop();
            uCanvas_Delay(1);
        }
    
    #elif RUN_FONTS_TESTING
        uCanvas_Run_Fonts_demo();
    
    #elif RUN_SLIDER_EXAMPLE_TESTING
        uCanvas_slider_demo_Setup();
        uCanvas_slider_demo_Main();
    #elif RUN_GUI_DEMO_TESTING
        uCanvas_GUI_Demo_Setup();
        uCanvas_GUI_Demo_Main();

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
    #endif
}




