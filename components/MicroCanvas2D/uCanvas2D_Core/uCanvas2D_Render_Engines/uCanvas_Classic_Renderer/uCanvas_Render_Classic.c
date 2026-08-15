#include "uCanvas2D_Instance.h"
#include "esp_timer.h"
#include "uCanvas_Render_Common.h"
#include "uCanvas_Draw.h"
#include "uCanvas2D_Display_Setup.h"
#include "uCanvas2D_Acceleration.h"

/**
 * Classic Renderer - Full framebuffer redraw every frame
 * 
 * Characteristics:
 * - Redraws entire scene every refresh
 * - No dirty-rect optimization
 * - Simpler logic, more predictable performance
 * - Best for scenes with frequent global changes
 */

void uCanvas_renderer_task_classic(void* arg) {
    uCanvas2D_Instance_t* instance = (uCanvas2D_Instance_t*)arg;
    uCanvas2D_RenderBuffer_t* current_bufffer = instance->render_buffer;
    uCanvas2D_RenderBuffer_t* draw_bufffer = instance->render_buffer;
    
    if(instance == NULL || instance->render_buffer == NULL) {
        printf("Invalid uCanvas2D_Instance_t\r\n");
        return;
    }

#if (UCANVAS_USE_DOUBLE_BUFFERING == ENABLED)
    int switch_buffer = 0; // toggled only after a successful present()
#endif
    
    printf("Classic Renderer task started\r\n");
    
    uint32_t obuf_size = 0;
    float scaled_x = 0;
    float scaled_y = 0;
    
    if(instance->upscale_instance_output) {
        scaled_x = instance->scale_x;
        scaled_y = instance->scale_y;
        obuf_size = instance->post_processing_frame_buf->width * instance->post_processing_frame_buf->height * sizeof(uint16_t);
    }
    
    while(1) { 
        int64_t start = esp_timer_get_time();
        uCanvas_Scene_t* _scene = instance->active_scene;
        vTaskDelay(pdMS_TO_TICKS(instance->refresh_delay));

        // Check if manual refresh on signal is enabled
        wait_on_referesh_signal(instance);

        if((_scene != NULL) && (_scene->_2D_Object_Ptr > 0)) {
            // Dirty fast-path: if nothing changed since last draw, skip render + panel push
            if(_scene->dirty_seq == instance->last_rendered_scene_seq) {
                continue;
            }

            // Pick target render buffer only when we will actually draw/present
#if (UCANVAS_USE_DOUBLE_BUFFERING == ENABLED)
            current_bufffer = switch_buffer ? instance->render_buffer_aux : instance->render_buffer;
#else
            current_bufffer = instance->render_buffer;
#endif
            
            if(current_bufffer == NULL) {
                printf("Null Render Buffer\r\n");
                return;
            }

            if(LOCK_RESOURCE(instance->render_buffer_lock)) { 
                // Clear Display
                if(instance->Clear_On_Refresh) {
                    if(current_bufffer->use_ppa) {
#if(CONFIG_IDF_TARGET_ESP32P4)
                        ppa_helper_fill(
                            current_bufffer->pixels,
                            current_bufffer->width * current_bufffer->height * sizeof(uint16_t),
                            current_bufffer->width,
                            current_bufffer->height,
                            0, 0, current_bufffer->width, current_bufffer->height, convertToRGB565(instance->canvas_clear_color), 0
                        );
#endif
                    } else {
                        memset(current_bufffer->pixels,convertToRGB565(instance->canvas_clear_color), current_bufffer->width * current_bufffer->height * sizeof(uint16_t)); 
                    }
                }
                
                // Push All elements to display 
                for(int i = 0; i < _scene->_2D_Object_Ptr; i++) {
                    if(_scene->_2D_Objects[i] == NULL) break;
                    if(_scene->_2D_Objects[i]->properties.visiblity == INVISIBLE) continue;
                    
                    uCanvas_universal_obj_t* obj = _scene->_2D_Objects[i];
                    draw_universal_object_to_target_render_buffer(obj, current_bufffer, instance->panel_1);
                    obj->dirty_flags = 0;
                }

                // Update display panel (This is non-blocking while not scaling output)
                draw_bufffer = current_bufffer;
                
                if(instance->upscale_instance_output) {
#if(CONFIG_IDF_TARGET_ESP32P4)
                    scale_buffer_with_factor(draw_bufffer->pixels, instance->post_processing_frame_buf->pixels, obuf_size, draw_bufffer->width, draw_bufffer->height, scaled_x, scaled_y);
                    if(instance->panel_1 != NULL) instance->panel_1->push_render_buffer(0, 0, instance->post_processing_frame_buf); 
#endif
                } else {
                    if(instance->panel_1 != NULL) instance->panel_1->push_render_buffer(draw_bufffer->offset_x, draw_bufffer->offset_y, draw_bufffer);
                }              
                
                instance->last_rendered_scene_seq = _scene->dirty_seq;

#if (UCANVAS_USE_DOUBLE_BUFFERING == ENABLED)
                // Only advance buffer after a complete frame has been presented
                switch_buffer = !switch_buffer;
#endif
                
                UNLOCK_RESOURCE(instance->render_buffer_lock);
                
                // Calculate and smooth FPS
                uint64_t now = esp_timer_get_time();
                uint32_t frame_us = (uint32_t)(now - start);
                instance->last_frame_time_us = frame_us;
                
                if(frame_us > 0) {
                    float inst_fps = 1000000.0f / (float)frame_us;
                    // EMA smoothing
                    const float alpha = 0.10f;
                    if(instance->fps_smoothed <= 0.0f) instance->fps_smoothed = inst_fps;
                    else instance->fps_smoothed = (instance->fps_smoothed * (1.0f - alpha)) + (inst_fps * alpha);
                }
            }
        } else {
            printf("No active scene to render\r\n");
        }
    }
}
