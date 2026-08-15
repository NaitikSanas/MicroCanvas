#include "uCanvas2D_Instance.h"
#include "esp_timer.h"
#include "uCanvas_Render_Common.h"
#include "uCanvas_Draw.h"
#include "uCanvas2D_Display_Setup.h"
#include "uCanvas2D_Acceleration.h"

/**
 * Sketch/Dirty-Rect Renderer - Optimized dirty region rendering
 * 
 * Characteristics:
 * - Tracks dirty regions per object
 * - Only clears and redraws dirty areas
 * - Computes union of old + new bounds
 * - Fallback to full redraw if dirty region > threshold
 * - Best for scenes with localized changes
 * - Panel push still uses full-buffer (optimizes draw work, not IO)
 */

void uCanvas_renderer_task_dirtyrect(void* arg) {
    uCanvas2D_Instance_t* instance = (uCanvas2D_Instance_t*)arg;
    
    if(instance == NULL || instance->render_buffer == NULL) {
        printf("Invalid uCanvas2D_Instance_t\r\n");
        return;
    } 

    uCanvas2D_RenderBuffer_t* current_bufffer = instance->render_buffer;
    uCanvas2D_RenderBuffer_t* draw_bufffer = instance->render_buffer;

#if (UCANVAS_USE_DOUBLE_BUFFERING == ENABLED)
    int switch_buffer = 0;
#endif

    printf("Dirty-rect renderer task started\r\n");

    while(1) {
        int64_t start = esp_timer_get_time();
        uCanvas_Scene_t* _scene = instance->active_scene;
        vTaskDelay(pdMS_TO_TICKS(instance->refresh_delay));
        wait_on_referesh_signal(instance);

        if(_scene == NULL || _scene->_2D_Object_Ptr <= 0) {
            continue;
        }

        // If nothing changed since last present, skip
        if(_scene->dirty_seq == instance->last_rendered_scene_seq) {
            continue;
        }

#if (UCANVAS_USE_DOUBLE_BUFFERING == ENABLED)
        current_bufffer = switch_buffer ? instance->render_buffer_aux : instance->render_buffer;
#else
        current_bufffer = instance->render_buffer;
#endif
        
        if(current_bufffer == NULL) {
            printf("Null Render Buffer\r\n");
            return;
        }

        if(!LOCK_RESOURCE(instance->render_buffer_lock)) {
            continue;
        }

        // Initialize dirty region tracking
        uCanvas_recti_t dirty_union = (uCanvas_recti_t){0, 0, 0, 0};
        int any_dirty = 0;
        int force_full_redraw = 0;

        // Pass 1: compute dirty union (old + new bounds)
        for(int i = 0; i < _scene->_2D_Object_Ptr; i++) {
            uCanvas_universal_obj_t* obj = _scene->_2D_Objects[i];
            if(obj == NULL) break;

            // Force full redraw on scene-level dirty flag
            if(obj->dirty_flags & UCANVAS_DIRTY_SCENE) {
                force_full_redraw = 1;
            }

            // If object isn't flagged dirty, initialize prev bounds once
            if(!obj->prev_bounds_valid) {
                uCanvas_recti_t b = uCanvas_compute_object_bounds(obj);
                uCanvas_rect_clip_to_buffer(&b, current_bufffer);
                obj->prev_x0 = b.x0; obj->prev_y0 = b.y0; obj->prev_x1 = b.x1; obj->prev_y1 = b.y1;
                obj->prev_visible = (uint8_t)obj->properties.visiblity;
                obj->prev_bounds_valid = 1;
                continue;
            }

            // Check for visibility changes
            const int vis_now = (obj->properties.visiblity == VISIBLE);
            const int vis_prev = (obj->prev_visible != 0);
            const int vis_changed = (vis_now != vis_prev);

            // Compute dirty union from object changes
            if(obj->dirty_flags || vis_changed) {
                any_dirty = 1;

                uCanvas_recti_t oldb = (uCanvas_recti_t){ obj->prev_x0, obj->prev_y0, obj->prev_x1, obj->prev_y1 };
                uCanvas_recti_t newb = uCanvas_compute_object_bounds(obj);
                uCanvas_rect_clip_to_buffer(&oldb, current_bufffer);
                uCanvas_rect_clip_to_buffer(&newb, current_bufffer);

                uCanvas_rect_union(&dirty_union, &oldb);
                uCanvas_rect_union(&dirty_union, &newb);
            }
        }

        // If scene changed but no per-object dirty flags (delete/reorder), force full redraw
        if(!any_dirty) {
            force_full_redraw = 1;
        }

        // Expand dirty region to full screen if forced
        if(force_full_redraw) {
            dirty_union.x0 = 0;
            dirty_union.y0 = 0;
            dirty_union.x1 = current_bufffer->width;
            dirty_union.y1 = current_bufffer->height;
        }

        uCanvas_rect_clip_to_buffer(&dirty_union, current_bufffer);

        // If dirty rect is too large, a full redraw+push is typically faster
        if(!uCanvas_rect_is_empty(&dirty_union)) {
            int32_t dw = (int32_t)(dirty_union.x1 - dirty_union.x0);
            int32_t dh = (int32_t)(dirty_union.y1 - dirty_union.y0);
            int64_t dirty_area = (int64_t)dw * (int64_t)dh;
            int64_t full_area = (int64_t)current_bufffer->width * (int64_t)current_bufffer->height;
            
            if(full_area > 0) {
                int64_t dirty_pct_x100 = (dirty_area * 100) / full_area;
                if(dirty_pct_x100 >= (int64_t)UCANVAS_DIRTY_RECT_FALLBACK_FULL_PCT) {
                    force_full_redraw = 1;
                    dirty_union.x0 = 0;
                    dirty_union.y0 = 0;
                    dirty_union.x1 = current_bufffer->width;
                    dirty_union.y1 = current_bufffer->height;
                }
            }
        }

        // Clear and redraw only the dirty region
        if(!uCanvas_rect_is_empty(&dirty_union)) {
            // Clear only the dirty region
#if(CONFIG_IDF_TARGET_ESP32P4)
            if(current_bufffer->use_ppa) {
                ppa_helper_fill(
                    current_bufffer->pixels,
                    current_bufffer->width * current_bufffer->height * sizeof(uint16_t),
                    current_bufffer->width,
                    current_bufffer->height,
                    dirty_union.x0, dirty_union.y0,
                    (dirty_union.x1 - dirty_union.x0),
                    (dirty_union.y1 - dirty_union.y0),
                    convertToRGB565(instance->canvas_clear_color), 0
                );
            } else {
                for(int y = dirty_union.y0; y < dirty_union.y1; y++) {
                    uint16_t* row = &current_bufffer->pixels[y * current_bufffer->width + dirty_union.x0];
                    memset(row, convertToRGB565(instance->canvas_clear_color), (dirty_union.x1 - dirty_union.x0) * sizeof(uint16_t));
                }
            }
#else 
            for(int y = dirty_union.y0; y < dirty_union.y1; y++) {
                uint16_t* row = &current_bufffer->pixels[y * current_bufffer->width + dirty_union.x0];
                memset(row, 0, (dirty_union.x1 - dirty_union.x0) * sizeof(uint16_t));
            }
#endif

            // Pass 2: redraw any object intersecting the dirty union
            for(int i = 0; i < _scene->_2D_Object_Ptr; i++) {
                uCanvas_universal_obj_t* obj = _scene->_2D_Objects[i];
                if(obj == NULL) break;
                
                uCanvas_recti_t b = uCanvas_compute_object_bounds(obj);
                uCanvas_rect_clip_to_buffer(&b, current_bufffer);
                
                if(obj->properties.visiblity != INVISIBLE) {
                    if(uCanvas_rect_intersects(&b, &dirty_union)) {
                        draw_universal_object_to_target_render_buffer(obj, current_bufffer, instance->panel_1);
                    }
                }

                // Update prev bounds/visibility each frame we present
                obj->prev_x0 = b.x0; obj->prev_y0 = b.y0; obj->prev_x1 = b.x1; obj->prev_y1 = b.y1;
                obj->prev_visible = (uint8_t)obj->properties.visiblity;
                obj->prev_bounds_valid = 1;
                obj->dirty_flags = 0;
            }
        }

        // Present: push partial dirty region when beneficial, otherwise full frame
        if(instance->panel_1 != NULL) {
            if(force_full_redraw) {
                instance->panel_1->push_render_buffer(current_bufffer->offset_x, current_bufffer->offset_y, current_bufffer);
            } else if(!uCanvas_rect_is_empty(&dirty_union)) {
                uCanvas2D_RenderBuffer_t sub = *current_bufffer;
                sub.offset_x = current_bufffer->offset_x + dirty_union.x0;
                sub.offset_y = current_bufffer->offset_y + dirty_union.y0;
                sub.width = (dirty_union.x1 - dirty_union.x0);
                sub.height = (dirty_union.y1 - dirty_union.y0);
                sub.pitch = current_bufffer->width;
                sub.pixels = &current_bufffer->pixels[dirty_union.y0 * current_bufffer->width + dirty_union.x0];
                instance->panel_1->push_render_buffer(sub.offset_x, sub.offset_y, &sub);
            }
        }

        instance->last_rendered_scene_seq = _scene->dirty_seq;
#if (UCANVAS_USE_DOUBLE_BUFFERING == ENABLED)
        switch_buffer = !switch_buffer;
#endif

        UNLOCK_RESOURCE(instance->render_buffer_lock);
        
        // Calculate and smooth FPS
        uint64_t now = esp_timer_get_time();
        uint32_t frame_us = (uint32_t)(now - start);
        instance->last_frame_time_us = frame_us;
        
        if(frame_us > 0) {
            float inst_fps = 1000000.0f / (float)frame_us;
            const float alpha = 0.10f;
            if(instance->fps_smoothed <= 0.0f) instance->fps_smoothed = inst_fps;
            else instance->fps_smoothed = (instance->fps_smoothed * (1.0f - alpha)) + (inst_fps * alpha);
        }
    }
}
