#include "uCanvas2D_Instance.h"
#include "uCanvas_Render_Common.h"
#include "uCanvas_Render_Classic.h"
#include "uCanvas_Render_Sketch.h"

/**
 * Renderer Manager - Handles renderer initialization, attachment, and mode management
 * 
 * This file contains:
 * - Renderer attachment and initialization
 * - Render mode configuration
 * - FPS monitoring
 * - Refresh signal handling
 */

/**
 * Get the current FPS from a renderer instance
 */
int64_t uCanvas_Get_FPS(uCanvas2D_Instance_t* instance) {
    if(instance == NULL) return 0;
    if(instance->fps_smoothed > 0.0f) {
        return (int64_t)(instance->fps_smoothed + 0.5f);
    }
    // Fallback if no frames presented yet
    if(instance->last_frame_time_us > 0) {
        return (int64_t)(1000000.0f / (float)instance->last_frame_time_us);
    }
    return 0;
}

/**
 * Set the render mode for manual refresh control
 */
void uCanvas_Set_Render_Mode(uCanvas2D_Instance_t* instance, uCanvas2D_Render_Mode_t mode) {
    instance->Render_Mode = mode;
    instance->signal_scene_refresh = xSemaphoreCreateBinary();
    instance->scene_refresh_complete = xSemaphoreCreateBinary();
}

/**
 * Send a refresh signal to the renderer
 * Behavior depends on Render_Mode:
 * - AUTO_REFRESH: returns immediately
 * - ASYNC_FRAME_QUEUED: signals refresh and continues
 * - ASYNC_FRAME_COMMIT: signals refresh and waits for completion
 */
void uCanvas_Send_Refresh_Signal_To_Renderer(uCanvas2D_Instance_t* instance) {
    if(instance->Render_Mode == AUTO_REFRESH) return;
    
    if((instance->Render_Mode == ASYNC_FRAME_QUEUED || instance->Render_Mode == ASYNC_FRAME_COMMIT) && instance->signal_scene_refresh != NULL) {
        xSemaphoreGive(instance->signal_scene_refresh);
    }

    if(instance->Render_Mode == ASYNC_FRAME_COMMIT) {
        xSemaphoreTake(instance->scene_refresh_complete, portMAX_DELAY);
    }
}

/**
 * Wait for and handle refresh signals
 * Internal function used by renderer tasks
 */
void wait_on_referesh_signal(uCanvas2D_Instance_t* instance) {
    xSemaphoreGive(instance->scene_refresh_complete);
    if(instance->Render_Mode == ASYNC_FRAME_COMMIT && instance->signal_scene_refresh != NULL) {
        xSemaphoreTake(instance->signal_scene_refresh, portMAX_DELAY); 
    }
}

/**
 * Attach a renderer to an instance
 * 
 * @param instance - Target uCanvas2D instance
 * @param RendererType - Renderer type (UCANVAS_CLASSIC or UCANVAS_SKETCH)
 * @param core_id - ESP32 core to pin the renderer task to
 */
void uCanvas_Attach_Renderer(uCanvas2D_Instance_t* instance, uCanvas_Rendere_type_t RendererType, int core_id) {
    instance->Render_Mode = AUTO_REFRESH;
    instance->Clear_On_Refresh = true;
    instance->refresh_delay = 2;
    instance->pin_to_core = core_id;
    instance->upscale_instance_output = false;
    switch(RendererType) {
        case UCANVAS_CLASSIC:
            xTaskCreatePinnedToCore(
                &uCanvas_renderer_task_classic,
                "uCanvas_Render_Task",
                UCANVAS_RENDER_TASK_STACK_SIZE,
                instance,
                UCANVAS_RENDER_TASK_PRIORITY,
                &instance->render_task_handle,
                instance->pin_to_core
            );
            printf("[INFO] Classic renderer attached\r\n");
            break;
            
        case UCANVAS_SKETCH:
            xTaskCreatePinnedToCore(
                &uCanvas_renderer_task_dirtyrect,
                "uCanvas_Render_Task",
                UCANVAS_RENDER_TASK_STACK_SIZE,
                instance,
                UCANVAS_RENDER_TASK_PRIORITY,
                &instance->render_task_handle,
                instance->pin_to_core
            );
            printf("[INFO] Sketch (dirty-rect) renderer attached\r\n");
            break;
            
        default:
            printf("[ERROR] Invalid Renderer type\r\n");
            break;
    }
}

/**
 * Set the refresh delay for the renderer
 * This controls the frame update rate (in milliseconds)
 */
void uCanvas_Set_Panel_RefreshDelay(uCanvas2D_Instance_t* instance, uint16_t RefreshDelay) {
    if(instance) instance->refresh_delay = RefreshDelay;
}

/**
 * Pause the renderer task
 */
void uCanvas_Pause_Instance(uCanvas2D_Instance_t* instance) {
    if(instance == NULL || instance->render_task_handle == NULL) {
        printf("Invalid uCanvas2D_Instance_t or render task handle\r\n");
        return;
    }
    vTaskSuspend(instance->render_task_handle);
}

/**
 * Resume the renderer task
 */
void uCanvas_Resume_Instance(uCanvas2D_Instance_t* instance) {
    if(instance == NULL || instance->render_task_handle == NULL) {
        printf("Invalid uCanvas2D_Instance_t or render task handle\r\n");
        return;
    }
    vTaskResume(instance->render_task_handle);
}

/**
 * Global pause/resume functions for global engine control
 */
extern TaskHandle_t uCanvas_taskhandle;

void pause_uCanvas_engine(void) {
    vTaskSuspend(uCanvas_taskhandle);
}

void resume_uCanvas_engine(void) {
    vTaskResume(uCanvas_taskhandle);
}
