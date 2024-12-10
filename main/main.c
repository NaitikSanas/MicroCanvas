#include "main.h"
#include "app.h"
#include "simple_menu_demo.h"
#include "design_game_map.h"
#define RUN_APP_C 1
// #define RUN_DESIGN_GAME_MAP_DEMO 1
// #define RUN_SIMPLE_MENU_DEMO 1
// #define RUN_2D_SPRITE_DEMO 1
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
    
    
}




