#include "uCanvas_api.h"

//If set 0 Canvas size will be derived by the size of Panel Type
#define USE_CUSTOM_CANVAS_SIZE  0
#define UCANVAS_INST_WIDTH      320
#define UCANVAS_INST_HEIGHT     240

#define REFRESH_DELAY           2
#define VIEWPORT_POSITION_XY    0,0
#define RENDERER_TASK_CORE_ID   1
#define RENDERER_TYPE           UCANVAS_SKETCH
#define RENDER_MODE             AUTO_REFRESH
#define PANEL_TYPE              PANEL_ST7789
#define CANVAS_CLEAR_COLOR 0,0,0
void Example_uCanvas_Instance_Setup(void);
uCanvas2D_Instance_t* Get_Example_uCanvas_Instance(void);