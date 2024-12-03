#include "main.h"
#include "app.h"
#include "simple_menu_demo.h"

#define RUN_APP_C
// #define RUN_SIMPLE_MENU_DEMO
// #define RUN_2D_SPRITE_DEMO 1
void app_main(){
    #ifdef RUN_APP_C
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

    #endif
    
    
    //
    
    
    
}




