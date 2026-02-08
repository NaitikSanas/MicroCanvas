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


#define GRAPH_POINTS     150
#define SCROLL_SPEED     0.4f
static uCanvas_Graph2D_Instance_t Graph1;
static uCanvas_Graph2D_Instance_t Graph2;


void demo_graph_sine_scroll(void)
{
    // init graph
    Graph1.data_x_min = 0;
    Graph1.data_x_max = GRAPH_POINTS;
    Graph1.data_y_min = 0;
    Graph1.data_y_max = 20;

    Graph2.data_x_min = 0;
    Graph2.data_x_max = 20;
    Graph2.data_y_min = 0;
    Graph2.data_y_max = GRAPH_POINTS;

    if (uCanvasGUI_Create_Graph2D(&Graph1, 25, 25, 300, 150, GRAPH_POINTS))
    {
        uCanvasGUI_Graph2D_Set_BackGround_Color(&Graph1, 0, 20, 0);
        uCanvasGUI_Graph2D_Set_Border_Color(&Graph1, 0, 130, 0);
        uCanvasGUI_Graph2D_Set_MainContent_Color(&Graph1, 50, 255, 0);
    }

    if (uCanvasGUI_Create_Graph2D(&Graph2, 10, 300, 200, 100, GRAPH_POINTS))
    {
        uCanvasGUI_Graph2D_Set_BackGround_Color(&Graph2, 0, 20, 0);
        uCanvasGUI_Graph2D_Set_Border_Color(&Graph2, 0, 130, 0);
        uCanvasGUI_Graph2D_Set_MainContent_Color(&Graph2, 50, 255, 0);
    }

    float phase = 0.0f;

    // initial fill
    for (int i = 0; i < GRAPH_POINTS-2; i++)
    {
        float y = (sinf(phase) * 0.5f + 0.5f) * (Graph1.data_y_max - Graph1.data_y_min) + Graph1.data_y_min;
        float y2 = (sinf(phase) * 0.5f + 0.5f) * (Graph2.data_y_max - Graph2.data_y_min) + Graph2.data_y_min;
        uCanvasGui_Graph2D_Push_DataPoints(&Graph1, i, (int)y);
        uCanvasGui_Graph2D_Push_DataPoints(&Graph2, (int)y2,i);
        phase += SCROLL_SPEED;
    }
    int x = 0;
    for (int i = 0; i < GRAPH_POINTS; i++)
    {
        uCanvasGui_Graph2D_Push_DataPoints_Y(&Graph1,i);
        uCanvasGui_Graph2D_Push_DataPoints_X(&Graph2,i);
    }

    char buf[32]={0};
    Graph1.GraphType = GRAPH2D_LINE;
    Graph2.GraphType = GRAPH2D_LINE;
    // scrolling loop
    while (1)
    {
        float y = (sinf(phase) * 0.5f + 0.5f) * (Graph1.data_y_max - Graph1.data_y_min) + Graph1.data_y_min;
        float y2 = (sinf(phase) * 0.5f + 0.5f) * (Graph2.data_x_max - Graph2.data_x_min) + Graph2.data_x_min;
        uCanvasGui_Graph2D_Push_DataPoints_Y(&Graph1,(int)y);
        uCanvasGui_Graph2D_Push_DataPoints_X(&Graph2,(int)y2);
        sprintf(buf,"Sine Wave Graph - y = %lld",uCanvas_Get_FPS(&uCanvas_Instance_1));
        uCanvasGUI_Graph2D_Set_Title_Text(&Graph1,buf);
        uCanvasGUI_Graph2D_Update(&Graph1);
        uCanvasGUI_Graph2D_Update(&Graph2);
        phase += SCROLL_SPEED;
        // uCanvas_Send_Refresh_Signal_To_Renderer(&uCanvas_Instance_1);
        vTaskDelay(pdMS_TO_TICKS(12)); // FreeRTOS style delay
    }
}
void uCanvas_Setup() {
    scene_1 = New_uCanvas_Scene();
    scene_2 = New_uCanvas_Scene();

    uCanvas2D_Display_Panel_t* panel = uCanvas2D_Get_Panel_Driver_ST7789();
    panel->init(1);
    panel->set_backlight(3000);
    
    //-- Set up ucanvas insance
    uCanvas_Attach_RenderBuffer(&uCanvas_Instance_1, 512, 512);
    uCanvas_Set_Panel_RefreshDelay(&uCanvas_Instance_1, 2);
    uCanvas_Attach_Panel(&uCanvas_Instance_1, panel);
    uCanvas_Set_ViewPort_Position(&uCanvas_Instance_1, 0, 0);
    uCanvas_Attach_Scene(&uCanvas_Instance_1, scene_1);
    uCanvas_Attach_Renderer(&uCanvas_Instance_1, 1);
    uCanvas_Set_Render_Mode(&uCanvas_Instance_1,AUTO_REFRESH);
    uCanvas_set_active_scene(scene_1);
    demo_graph_sine_scroll();
}

void uCanvas_App_Main(void) {

     uCanvas_Delay(4000);
}