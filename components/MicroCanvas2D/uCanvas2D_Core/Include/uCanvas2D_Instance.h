#ifndef UCANVAS2D_INSTANCE_H
#define UCANVAS2D_INSTANCE_H

#include "uCanvasDataTypes.h"
#include "uCanvas2D_Display_Setup.h"

/**
 * uCanvas2D Instance Lifecycle Management
 * 
 * This header defines the core instance management functions for the rendering engine.
 * Instance creation, attachment, and destruction are handled through this API.
 */

// Instance creation and destruction
uCanvas2D_Instance_t* New_uCanvas_Instance(uCanvas_Scene_t* scene, uCanvas2D_Display_Panel_t* panel_1, uCanvas2D_Display_Panel_t* panel_2, int width, int height, int offset_x, int offset_y);
uCanvas2D_Instance_t* New_uCanvas_Window_Instance(uCanvas_Scene_t* scene, int width, int height);
void uCanvas_Destroy_Instance(uCanvas2D_Instance_t* instance);

// Attachment functions
int uCanvas_Attach_RenderBuffer(uCanvas2D_Instance_t* instance, int width, int height);
void uCanvas_Attach_Scene(uCanvas2D_Instance_t* instance, uCanvas_Scene_t* scene);
void uCanvas_Attach_Panel(uCanvas2D_Instance_t* instance, uCanvas2D_Display_Panel_t* panel);
void uCanvas_Set_ViewPort_Position(uCanvas2D_Instance_t* instance, int x, int y);
void uCanvas_Set_Canvas_Clear_Color(uCanvas2D_Instance_t* Instance,uint8_t r, uint8_t g, uint8_t b);
#endif // UCANVAS2D_INSTANCE_H
