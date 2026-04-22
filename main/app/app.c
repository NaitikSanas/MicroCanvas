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
#include "uCanvas_Graph2D.h"

uCanvas2D_Instance_t uCanvas_Instance_1;
uCanvas2D_Instance_t uCanvas_Instance_2;
#define CANVAS_HEIGHT       600
#define CANVAS_WIDTH        1024

uCanvas_Scene_t* scene_1 = NULL;
uCanvas_Scene_t* scene_2 = NULL;

void uCanvas_Setup() {
    scene_1 = New_uCanvas_Scene();

    uCanvas2D_Display_Panel_t* panel = uCanvas2D_Get_Panel_Driver_EK79007();
    panel->init(1);
    panel->set_backlight(3000);
    
    //-- Set up ucanvas insance
    uCanvas_Attach_RenderBuffer(&uCanvas_Instance_1, 1024, 800);
    uCanvas_Set_Panel_RefreshDelay(&uCanvas_Instance_1, 2);
    uCanvas_Attach_Panel(&uCanvas_Instance_1, panel);
    uCanvas_Set_ViewPort_Position(&uCanvas_Instance_1, 0, 0);
    uCanvas_Attach_Scene(&uCanvas_Instance_1, scene_1);
    uCanvas_Attach_Renderer(&uCanvas_Instance_1, 1);
    uCanvas_Set_Render_Mode(&uCanvas_Instance_1,AUTO_REFRESH);
    uCanvas_set_active_scene(scene_1);

    sprite2D_t bg;
    uCanvas_Compose_2DSprite_Obj(&bg,pexels_adonyi_foto_1400140,PEXELS_ADONYI_FOTO_1400140_WIDTH,PEXELS_ADONYI_FOTO_1400140_HEIGHT,COLOR_RGB565);

    uCanvas_universal_obj_t* bg_obj = New_uCanvas_2DSprite(&bg,0,0);
    uCanvas_Send_Refresh_Signal_To_Renderer(&uCanvas_Instance_1);
    while (1)
    {
        // if(bg_obj->properties.position.x + PEXELS_ADONYI_FOTO_1400140_WIDTH < 1024)bg_obj->properties.position.x++;
        // else bg_obj->properties.position.x = 0;
        // printf("FPS %lld\r\n",uCanvas_Get_FPS(&uCanvas_Instance_1));
        
        uCanvas_Delay(50);

    }
    

}

void uCanvas_App_Main(void) {

    uCanvas_Delay(50);
}