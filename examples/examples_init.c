#include "examples_init.h"

static uCanvas2D_Instance_t  g_canvas;

void Example_uCanvas_Instance_Setup(void){
    /*
    Initialize Display Driver of your choice.
    Options of Currently supported Panels : 
        1. PANEL_ST7789,
        2. PANEL_EK79007
    */ 
    uCanvas2D_Display_Panel_t *panel = uCanvas_Get_Panel_Handle(PANEL_TYPE);

    // panel->set_backlight(0xff);

    /* 
        Create a new 2D Scene. A scene object holds all 2D Permitives Created by app.
        It is used by renderer to render the Scene on the frame buffer then push to the display.   
    */
    uCanvas_Scene_t *scene = New_uCanvas_Scene();

    /*
        uCanvas_Attach_RenderBuffer Creates Frame buffer of specified size. it can be set to lower than resolution
        of the display as well which you can use to create a dedicated viewport area on display and share free region of display with 
        other uCanvas Instance.
    */
    #if USE_CUSTOM_CANVAS_SIZE
    if (uCanvas_Attach_RenderBuffer(&g_canvas, UCANVAS_INST_WIDTH, UCANVAS_INST_HEIGHT)) {
    #else 
    if (uCanvas_Attach_RenderBuffer(&g_canvas, panel->width, panel->height)) {
    #endif
        uCanvas_Set_Panel_RefreshDelay(&g_canvas, REFRESH_DELAY); //Set Refresh interval to updade content on display
        uCanvas_Attach_Panel(&g_canvas, panel); // Attach Previously created display panel to uCanvas Instance
        uCanvas_Set_ViewPort_Position(&g_canvas, VIEWPORT_POSITION_XY); //If this uCanvas Instance is Viewport you can offset its position on screen
        uCanvas_Attach_Scene(&g_canvas, scene); // Attach Scene object to the instance 
        uCanvas_Attach_Renderer(&g_canvas,RENDERER_TYPE,RENDERER_TASK_CORE_ID); 
        uCanvas_Set_Render_Mode(&g_canvas, AUTO_REFRESH);
        uCanvas_Set_Canvas_Clear_Color(&g_canvas,CANVAS_CLEAR_COLOR);
    }

    /*
        uCanvas_set_active_scene makes lets ucanvas api to access the scene objects and create/remove or read/write 
        properties of different universal object containing in passed scene instance.
    */
    uCanvas_set_active_scene(g_canvas.active_scene);
}

/*
    @brief Returns reference of uCanvas Instance 
*/
uCanvas2D_Instance_t* Get_Example_uCanvas_Instance(void){
    return &g_canvas;
}
