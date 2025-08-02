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
static uCanvas2D_Instance_t* uCanvas_Instance_1 = NULL;
static uCanvas2D_Instance_t* uCanvas_Instance_2 = NULL;
static uCanvas2D_Instance_t* uCanvas_Instance_3 = NULL;
static uCanvas2D_Instance_t* uCanvas_Instance_4 = NULL;



void uCanvas_Setup() {
    uCanvas_Scene_t* scene_1 = New_uCanvas_Scene();
    uCanvas_Instance_1 = New_uCanvas_Instance(scene_1, uCanvas2D_Get_Panel_Driver_EK79007(),NULL);
    
}   

void uCanvas_App_Main(void) {
    
   
}