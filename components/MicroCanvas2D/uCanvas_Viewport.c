#include "uCanvas_Viewport.h"
#include "uCanvas_display_port.h"
/**
 * @brief Creates A Canvas of specified size as a 2D sprite in a backend
 * @returns Pointer to sprite object holding Canvas
 */
uCanvas_universal_obj_t* uCanvas_Create_Viewport(uCanvas_Draw_Canvas_t* canvas_obj,uint16_t* canvas_buf, uint16_t canvas_width, uint16_t canvas_height){
    canvas_obj->frame_buf = canvas_buf;
    uCanvas_Compose_2DSprite_Obj(&canvas_obj->sprite_obj,canvas_buf,canvas_width,canvas_height);
    return New_uCanvas_2DSprite(&canvas_obj->sprite_obj,canvas_obj->canvas_position_x,canvas_obj->canvas_position_y);
}

void uCanvas_Viewport_DrawLine(uCanvas_Draw_Canvas_t* canvas_obj, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    if (!canvas_obj || !canvas_obj->frame_buf) {
        printf("Error: Null canvas or frame buffer!\n");
        return;
    }
    
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    
    while (1) {
        if (x0 >= 0 && x0 < canvas_obj->sprite_obj.width && y0 >= 0 && y0 < canvas_obj->sprite_obj.height) {
            int index = y0 * canvas_obj->sprite_obj.width + x0;
            canvas_obj->frame_buf[index] = color;
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void uCanvas_Viewport_DrawRect(uCanvas_Draw_Canvas_t* canvas_obj, int16_t pos_x, int16_t pos_y, int16_t width, int16_t height, uint16_t color, bool fill) {
    if (!canvas_obj || !canvas_obj->frame_buf) {
        printf("Error: Null canvas or frame buffer!\n");
        return;
    }
    
    if (fill) {
        for (int16_t y = pos_y; y < pos_y + height; y++) {
            for (int16_t x = pos_x; x < pos_x + width; x++) {
                if (x >= 0 && x < canvas_obj->sprite_obj.width && y >= 0 && y < canvas_obj->sprite_obj.height) {
                    int index = y * canvas_obj->sprite_obj.width + x;
                    canvas_obj->frame_buf[index] = color;
                }
            }
        }
    } else {
        uCanvas_Viewport_DrawLine(canvas_obj, pos_x, pos_y, pos_x + width - 1, pos_y, color); // Top
        uCanvas_Viewport_DrawLine(canvas_obj, pos_x, pos_y + height - 1, pos_x + width - 1, pos_y + height - 1, color); // Bottom
        uCanvas_Viewport_DrawLine(canvas_obj, pos_x, pos_y, pos_x, pos_y + height - 1, color); // Left
        uCanvas_Viewport_DrawLine(canvas_obj, pos_x + width - 1, pos_y, pos_x + width - 1, pos_y + height - 1, color); // Right
    }
}