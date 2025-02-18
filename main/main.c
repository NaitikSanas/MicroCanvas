#include "main.h"
#include "app.h"
#include "simple_menu_demo.h"
#include "design_game_map.h"
#include "uCanvas_Physix.h"
#include "fonts_demo.h"
#include "slider_example.h"
#include "GUI_Demo.h"
#include "Game_of_life.h"
// #define RUN_APP_C 1
// #define RUN_DESIGN_GAME_MAP_DEMO 1
// #define RUN_SIMPLE_MENU_DEMO 1
// #define RUN_FONTS_TESTING 1
// #define RUN_SLIDER_EXAMPLE_TESTING 1
// #define RUN_GUI_DEMO_TESTING 1
#define RUN_GAME_OF_LIFE_DEMO 1

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
    #endif
    


}




