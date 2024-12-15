#include "main.h"
#include "app.h"
#include "simple_menu_demo.h"
#include "design_game_map.h"
#include "uCanvas_Physix.h"
#include "fonts_demo.h"
// #define RUN_APP_C 1
#define RUN_DESIGN_GAME_MAP_DEMO 1
// #define RUN_SIMPLE_MENU_DEMO 1
// #define RUN_FONTS_TESTING 1


void app_main(){
    
    

    #if RUN_APP_C
    uCanvas_Setup();
    while (1)
    {
        uCanvas_App_Main();
        // uCanvas_Delay(1);
    }

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
    #endif
    


}




