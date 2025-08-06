#include "app.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "sprite_collection.h"
#include "uCanvas2D_ST7789_Port.h"
#include "uCanvas2D_EK79007Port.h"
#include "uCanvasRenderEngine.h"
#define CANVAS_HEIGHT       600
#define CANVAS_WIDTH        1024
static uCanvas2D_Instance_t uCanvas_Instance_1;
static uCanvas2D_Instance_t uCanvas_Instance_2;
static uCanvas2D_Instance_t uCanvas_Instance_3;
static uCanvas2D_Instance_t uCanvas_Instance_4;



void uCanvas_Setup() {
    uCanvas_Scene_t* scene_1 = New_uCanvas_Scene();
    uCanvas2D_Display_Panel_t* panel = uCanvas2D_Get_Panel_Driver_EK79007();
    panel->init(1);
    panel->set_backlight(3000);

    if(uCanvas_Attach_RenderBuffer(&uCanvas_Instance_1,CANVAS_WIDTH,CANVAS_HEIGHT)){
        uCanvas_Set_Panel_RefreshDelay(&uCanvas_Instance_1,2);
        uCanvas_Attach_Panel(&uCanvas_Instance_1,panel);
        uCanvas_Set_ViewPort_Position(&uCanvas_Instance_1,20,20);
        uCanvas_Attach_Scene(&uCanvas_Instance_1,scene_1);
        uCanvas_Attach_Renderer(&uCanvas_Instance_1,1);
    }  
}   

void uCanvas_App_Main(void) {
    
   
}