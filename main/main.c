#include "main.h"
#include "app.h"
#include "simple_menu_demo.h"
void app_main(){
    // uCanvas_Setup();
    // while (1)
    // {
    //     uCanvas_App_Main();
    // }
    
    
    // sprite_demo(); // explore sprites_demo.c     
    simple_menu_demo_setup();
    while (1)
    {
        simple_menu_demo_App_Main();
    }
    
    
}




