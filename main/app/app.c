#include "app.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "sprite_collection.h"
#include "uCanvas2D_ST7789_Port.h"
#include "uCanvas2D_EK79007Port.h"
#include "uCanvasRenderEngine.h"
#include "uCanvas_HID.h"
#include "uCanvas2D_Acceleration.h"
#include "esp_timer.h"

#include "esp_timer.h"
#include "FreeRTOSConfig.h"

uCanvas2D_Instance_t uCanvas_Instance_1;
#define CANVAS_HEIGHT       600
#define CANVAS_WIDTH        1024

void uCanvas_Setup() {
    Intialize_PPA();

    uCanvas_Scene_t* scene_1 = New_uCanvas_Scene();
    uCanvas2D_Display_Panel_t* panel = uCanvas2D_Get_Panel_Driver_EK79007();
    panel->init(1);
    panel->set_backlight(3000);
    
    
    if (uCanvas_Attach_RenderBuffer(&uCanvas_Instance_1, CANVAS_WIDTH, CANVAS_HEIGHT)) {
        uCanvas_Set_Panel_RefreshDelay(&uCanvas_Instance_1, 0);
        uCanvas_Attach_Panel(&uCanvas_Instance_1, panel);
        uCanvas_Set_ViewPort_Position(&uCanvas_Instance_1, 0, 0);
        uCanvas_Attach_Scene(&uCanvas_Instance_1, scene_1);
        uCanvas_Attach_Renderer(&uCanvas_Instance_1, 1);
        // uCanvas_Instance_1.Clear_On_Refresh = false;
    }
    
    uCanvas_set_active_scene(scene_1);
}

void uCanvas_App_Main(void) {
    uCanvas_Delay(1);
}