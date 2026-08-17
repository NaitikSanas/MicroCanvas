#include "uCanvas_api.h"

#define UCANVAS_INST_WIDTH      128
#define UCANVAS_INST_HEIGHT     64
#define REFRESH_DELAY           2
#define VIEWPORT_POSITION_XY    0,0
#define RENDERER_TASK_CORE_ID   1
#define RENDERER_TYPE           UCANVAS_SKETCH
#define RENDER_MODE             AUTO_REFRESH
#define PANEL_TYPE              PANEL_SSD1306_128_64
#define CANVAS_CLEAR_COLOR 0,0,0
void Example_uCanvas_Instance_Setup(void);
uCanvas2D_Instance_t* Get_Example_uCanvas_Instance(void);