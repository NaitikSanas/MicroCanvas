#include "main.h"
#include "app.h"
#include "Game_of_life.h"
#include "L_System_Demo.h"
#include "3D_Cube.h"
#include "space_explorer_game.h"



// #define RUN_APP_C 1
// #define RUN_GAME_OF_LIFE_DEMO 1
// #define RUN_L_SYSTEM_DEMO 1
// #define RUN_3D_CUBE_DEMO 1
#define RUN_SPACE_EXPLORER_DEMO 1

void app_main(){
    #if  RUN_APP_C
        uCanvas_App_Main();

    #elif RUN_GAME_OF_LIFE_DEMO
        start_game_of_life_demo();  

    #elif RUN_L_SYSTEM_DEMO
        L_System_Demo_Main();

    #elif RUN_3D_CUBE_DEMO
        Run_3D_Cube_Demo();

    #elif RUN_SPACE_EXPLORER_DEMO
        Run_Space_Explorer_Game();
    #endif
}
