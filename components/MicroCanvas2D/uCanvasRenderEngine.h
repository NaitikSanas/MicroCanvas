#include <stdio.h>
#include <string.h>

#include "uCanvas_api.h"

/**
 * APIs for setting up uCanvas
*/
void uCanvas_bg_render_engine_task(void*arg);
uCanvas2D_Instance_t* uCanvas_New_Instance(uCanvas_Scene_t* scene, uCanvas2D_Display_Panel_t* panel, uCanvas2D_RenderBuffer_t* render_buffer);
void uCanvas_Set_Active_Scene(uCanvas2D_Instance_t *instance, uCanvas_Scene_t* scene);
void uCanvas_Set_RenderBuffer(uCanvas2D_Instance_t *instance, uCanvas2D_RenderBuffer_t* render_buffer);
void uCanvas_Set_Panel(uCanvas2D_Instance_t *instance, uCanvas2D_Display_Panel_t* panel);
void uCanvas_Setup_Instance(uCanvas2D_Instance_t *instance);    
void uCanvas_Attach_Render_Task(uCanvas2D_Instance_t* instance);
void uCanvas_RenderBuffer_Init(uCanvas2D_RenderBuffer_t* buf, int width, int height);
uCanvas2D_Instance_t* New_uCanvas_Instance(uCanvas_Scene_t* scene, uCanvas2D_Display_Panel_t* panel_1,uCanvas2D_Display_Panel_t* panel_2);