#ifndef UCANVAS_RENDER_MANAGER_H
#define UCANVAS_RENDER_MANAGER_H

#include "uCanvas2D_Instance.h"

/**
 * Renderer Manager - Public API for renderer control
 */

// FPS and performance monitoring
int64_t uCanvas_Get_FPS(uCanvas2D_Instance_t* instance);

// Render mode control
void uCanvas_Set_Render_Mode(uCanvas2D_Instance_t* instance, uCanvas2D_Render_Mode_t mode);
void uCanvas_Send_Refresh_Signal_To_Renderer(uCanvas2D_Instance_t* instance);

// Renderer attachment
void uCanvas_Attach_Renderer(uCanvas2D_Instance_t* instance, uCanvas_Rendere_type_t RendererType, int core_id);

// Refresh control
void uCanvas_Set_Panel_RefreshDelay(uCanvas2D_Instance_t* instance, uint16_t RefreshDelay);

// Instance pause/resume
void uCanvas_Pause_Instance(uCanvas2D_Instance_t* instance);
void uCanvas_Resume_Instance(uCanvas2D_Instance_t* instance);

// Global engine control
void pause_uCanvas_engine(void);
void resume_uCanvas_engine(void);

#endif // UCANVAS_RENDER_MANAGER_H
