#ifndef UCANVAS_RENDER_COMMON_H
#define UCANVAS_RENDER_COMMON_H

#include "uCanvasRenderEngine.h"

/**
 * Rect utilities for dirty-rect and bounds calculations
 */
typedef struct {
    int16_t x0;
    int16_t y0;
    int16_t x1;
    int16_t y1;
} uCanvas_recti_t;

// Rect helper functions
static inline int uCanvas_rect_is_empty(const uCanvas_recti_t* r){
    return (r->x1 <= r->x0) || (r->y1 <= r->y0);
}

static inline void uCanvas_rect_union(uCanvas_recti_t* dst, const uCanvas_recti_t* a){
    if(uCanvas_rect_is_empty(a)) return;
    if(uCanvas_rect_is_empty(dst)){
        *dst = *a;
        return;
    }
    if(a->x0 < dst->x0) dst->x0 = a->x0;
    if(a->y0 < dst->y0) dst->y0 = a->y0;
    if(a->x1 > dst->x1) dst->x1 = a->x1;
    if(a->y1 > dst->y1) dst->y1 = a->y1;
}

static inline int uCanvas_rect_intersects(const uCanvas_recti_t* a, const uCanvas_recti_t* b){
    if(uCanvas_rect_is_empty(a) || uCanvas_rect_is_empty(b)) return 0;
    return !(a->x1 <= b->x0 || a->x0 >= b->x1 || a->y1 <= b->y0 || a->y0 >= b->y1);
}

static inline void uCanvas_rect_clip_to_buffer(uCanvas_recti_t* r, const uCanvas2D_RenderBuffer_t* fb){
    if(r->x0 < 0) r->x0 = 0;
    if(r->y0 < 0) r->y0 = 0;
    if(r->x1 > fb->width)  r->x1 = fb->width;
    if(r->y1 > fb->height) r->y1 = fb->height;
    if(r->x1 < r->x0) r->x1 = r->x0;
    if(r->y1 < r->y0) r->y1 = r->y0;
}

static inline uCanvas_recti_t uCanvas_bounds_from_xywh(int x, int y, int w, int h){
    uCanvas_recti_t r;
    r.x0 = (int16_t)x;
    r.y0 = (int16_t)y;
    r.x1 = (int16_t)(x + w);
    r.y1 = (int16_t)(y + h);
    return r;
}

// Bounds computation for all object types
static uCanvas_recti_t uCanvas_compute_object_bounds(const uCanvas_universal_obj_t* obj){
    if(!obj) return (uCanvas_recti_t){0,0,0,0};

    switch(obj->properties.type){
        case RECTANGLE:
            return uCanvas_bounds_from_xywh(obj->properties.position.x, obj->properties.position.y, obj->width, obj->height);
        case SPRITE2D:
        case WINDOW:
            return uCanvas_bounds_from_xywh(obj->properties.position.x, obj->properties.position.y, obj->width, obj->height);
        case CIRCLE:
            return uCanvas_bounds_from_xywh(obj->properties.position.x - obj->r1,
                                            obj->properties.position.y - obj->r1,
                                            obj->r1 * 2,
                                            obj->r1 * 2);
        case ELLIPSE:
            return uCanvas_bounds_from_xywh(obj->properties.position.x - obj->r1,
                                            obj->properties.position.y - obj->r2,
                                            obj->r1 * 2,
                                            obj->r2 * 2);
        case LINE: {
            int x0 = obj->point1.x < obj->point2.x ? obj->point1.x : obj->point2.x;
            int y0 = obj->point1.y < obj->point2.y ? obj->point1.y : obj->point2.y;
            int x1 = obj->point1.x > obj->point2.x ? obj->point1.x : obj->point2.x;
            int y1 = obj->point1.y > obj->point2.y ? obj->point1.y : obj->point2.y;
            // Thickness fudge factor used in draw (2)
            return (uCanvas_recti_t){ (int16_t)(x0 - 2), (int16_t)(y0 - 2), (int16_t)(x1 + 2), (int16_t)(y1 + 2) };
        }
        case TEXTBOX:
        case ADV_TEXTBOX: {
            int w = obj->textbox_properties ? obj->textbox_properties->textbox_width : (int)obj->width;
            int h = obj->textbox_properties ? obj->textbox_properties->textbox_height : (int)obj->height;
            if(w <= 0) w = 1;
            if(h <= 0) h = 1;
            return uCanvas_bounds_from_xywh(obj->properties.position.x, obj->properties.position.y, w, h);
        }
        default:
            // Conservative fallback: treat unknown objects as 1x1 at position
            return uCanvas_bounds_from_xywh(obj->properties.position.x, obj->properties.position.y, 1, 1);
    }
}

// Color conversion
uint16_t IRAM_ATTR convertToRGB565(color_t color);

// Universal object drawing
void IRAM_ATTR draw_universal_object_to_target_render_buffer(uCanvas_universal_obj_t* obj, uCanvas2D_RenderBuffer_t* framebuffer, uCanvas2D_Display_Panel_t* panel);

// Signal handling for refresh modes
void wait_on_referesh_signal(uCanvas2D_Instance_t* instance);

#endif // UCANVAS_RENDER_COMMON_H
