#include "main.h"
#include "app.h"


#include "Game_of_life.h"
#include "Sprite_Animation_With_Keyframe.h"
#include "L_System_Demo.h"
#include "3D_Cube.h"
#include "uCanvas_IO_Usage_Demo.h"
#include "space_explorer_game.h"
#include "Adv_Textbox_with_HID_Keyboard.h"
#include "pcm5102a.h"
#include "uCanvas_Synth.h"
// #define RUN_APP_C 1
// #define RUN_UCANVAS_STRESS_TEST 1

// #define RUN_GAME_OF_LIFE_DEMO 1
// #define RUN_ADV_TEXTBOX_WITH_HID_KEYBOARD 1
// #define RUN_SPRITE_ANIMATION_WITH_KEYFRAME 1
// #define RUN_L_SYSTEM_DEMO 1
// #define RUN_3D_CUBE_DEMO 1
// #define RUN_IO_USAGE_DEMO 1
// #define RUN_SPACE_EXPLORER_DEMO 1
// #define RUN_UCANVAS_SYNTH 1

#include "uCanvasServer_Core.h"
#include "uCanvasServer_UART.h"
static uCanvas2D_Instance_t  g_canvas;
void uCanvasServer_Start(void)
{
    /* 1. Init core (handle table, scene table) */
    uCanvasServer_Core_Init();

    //  uCanvas2D_Display_Panel_t *panel = uCanvas_Get_Panel_Handle(PANEL_EK79007);
    // panel->init(1);

    // uCanvas_Scene_t *scene = New_uCanvas_Scene();

    // g_canvas.scale_output = false;
    // g_canvas.scale_x      = 2.0f;
    // g_canvas.scale_y      = 2.0f;

    // if (uCanvas_Attach_RenderBuffer(&g_canvas, 1024, 600)) {
    //     uCanvas_Set_Panel_RefreshDelay(&g_canvas, 2);
    //     uCanvas_Attach_Panel(&g_canvas, panel);
    //     uCanvas_Set_ViewPort_Position(&g_canvas, 0, 0);
    //     uCanvas_Attach_Scene(&g_canvas, scene);
    //     uCanvas_Attach_Renderer(&g_canvas, 1);
    //     uCanvas_Set_Render_Mode(&g_canvas, AUTO_REFRESH);
    // }
    
    // uCanvas_set_active_scene(g_canvas.active_scene);

    // uCanvas_universal_obj_t* bg = New_uCanvas_2DRectangle(0,0,800,1024);
    // uCanvas_Set_Color(bg,255,0,0);
    // uCanvas_Set_Fill(bg,NOFILL);
    // /* 2. Bring up UART transport */
   
    uCanvasServer_UART_Config_t uart_cfg = uCanvasServer_UART_DefaultConfig();
    /* Override pins/baud here if needed, e.g.: */
    // uart_cfg.tx_pin = 17;
    // uart_cfg.rx_pin = 18;
    // uart_cfg.baud_rate = 921600;

    uCanvasServer_UART_Init(&uart_cfg);

    // /* 3. Bind core's response/event output to UART send */
    uCanvasServer_SetSendFunc(uCanvasServer_UART_SendFrame);

    /* From here on, the UART RX task parses incoming frames and
       dispatches them via uCanvasServer_HandleFrame() automatically. */
}


void app_main(){

    
    
    uCanvasServer_Start();
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