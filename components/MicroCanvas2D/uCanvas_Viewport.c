#include "uCanvas_Viewport.h"
#include "uCanvas_display_port.h"
/**
 * @brief Creates A Canvas of specified size as a 2D sprite in a backend
 * @returns Pointer to sprite object holding Canvas
 */
uCanvas_universal_obj_t* uCanvas_Create_Viewport(uCanvas_Viewport_Instance_t* viewport_instance,uint16_t* canvas_buf, uint16_t viewport_width, uint16_t viewport_height){
    viewport_instance->frame_buf = canvas_buf;
    viewport_instance->viewport_width = viewport_width;
    viewport_instance->viewport_height = viewport_height;
    uCanvas_Compose_2DSprite_Obj(&viewport_instance->sprite_obj,canvas_buf,viewport_width,viewport_height);
    return New_uCanvas_2DSprite(&viewport_instance->sprite_obj,viewport_instance->viewport_position_x,viewport_instance->viewport_position_y);
}

void uCanvas_Viewport_DrawLine(uCanvas_Viewport_Instance_t* viewport_instance, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    if (!viewport_instance || !viewport_instance->frame_buf) {
        printf("Error: Null canvas or frame buffer!\n");
        return;
    }
    
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    
    while (1) {
        if (x0 >= 0 && x0 < viewport_instance->viewport_width && y0 >= 0 && y0 < viewport_instance->viewport_height) {
            int index = y0 * viewport_instance->viewport_width + x0;
            viewport_instance->frame_buf[index] = color;
        }
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void uCanvas_Viewport_DrawRect(uCanvas_Viewport_Instance_t* viewport_instance, int16_t pos_x, int16_t pos_y, int16_t width, int16_t height, uint16_t color, bool fill) {
    if (!viewport_instance || !viewport_instance->frame_buf) {
        printf("Error: Null canvas or frame buffer!\n");
        return;
    }
    
    if (fill) {
        for (int16_t y = pos_y; y < pos_y + height; y++) {
            for (int16_t x = pos_x; x < pos_x + width; x++) {
                if (x >= 0 && x < viewport_instance->viewport_width && y >= 0 && y < viewport_instance->viewport_height) {
                    int index = y * viewport_instance->viewport_width + x;
                    viewport_instance->frame_buf[index] = color;
                }
            }
        }
    } else {
        uCanvas_Viewport_DrawLine(viewport_instance, pos_x, pos_y, pos_x + width - 1, pos_y, color); // Top
        uCanvas_Viewport_DrawLine(viewport_instance, pos_x, pos_y + height - 1, pos_x + width - 1, pos_y + height - 1, color); // Bottom
        uCanvas_Viewport_DrawLine(viewport_instance, pos_x, pos_y, pos_x, pos_y + height - 1, color); // Left
        uCanvas_Viewport_DrawLine(viewport_instance, pos_x + width - 1, pos_y, pos_x + width - 1, pos_y + height - 1, color); // Right
    }
}

void uCanvas_Viewport_Fill(uCanvas_Viewport_Instance_t* viewport_instance,uint16_t color){
    uCanvas_Viewport_DrawRect(viewport_instance,0,0,viewport_instance->viewport_width,viewport_instance->viewport_height,color,1);
}

void uCanvas_Viewport_DrawCircle(uCanvas_Viewport_Instance_t* viewport_instance, int16_t x0, int16_t y0, int16_t radius, uint16_t color, bool fill){
    if (!viewport_instance || !viewport_instance->frame_buf) {
        printf("Error: Null canvas or frame buffer!\n");
        return;
    }
    
    int x = radius, y = 0;
    int err = 1 - x;
    int width = viewport_instance->viewport_width;
    int height = viewport_instance->viewport_height;
    
    while (x >= y) {
        if (fill) {
            for (int i = x0 - x; i <= x0 + x; i++) {
                if (i >= 0 && i < width && (y0 + y) >= 0 && (y0 + y) < height)
                    viewport_instance->frame_buf[(y0 + y) * width + i] = color;
                if (i >= 0 && i < width && (y0 - y) >= 0 && (y0 - y) < height)
                    viewport_instance->frame_buf[(y0 - y) * width + i] = color;
            }
            for (int i = x0 - y; i <= x0 + y; i++) {
                if (i >= 0 && i < width && (y0 + x) >= 0 && (y0 + x) < height)
                    viewport_instance->frame_buf[(y0 + x) * width + i] = color;
                if (i >= 0 && i < width && (y0 - x) >= 0 && (y0 - x) < height)
                    viewport_instance->frame_buf[(y0 - x) * width + i] = color;
            }
        } else {
            if ((y0 + y) >= 0 && (y0 + y) < height) {
                if ((x0 + x) >= 0 && (x0 + x) < width)
                    viewport_instance->frame_buf[(y0 + y) * width + (x0 + x)] = color;
                if ((x0 - x) >= 0 && (x0 - x) < width)
                    viewport_instance->frame_buf[(y0 + y) * width + (x0 - x)] = color;
            }
            if ((y0 - y) >= 0 && (y0 - y) < height) {
                if ((x0 + x) >= 0 && (x0 + x) < width)
                    viewport_instance->frame_buf[(y0 - y) * width + (x0 + x)] = color;
                if ((x0 - x) >= 0 && (x0 - x) < width)
                    viewport_instance->frame_buf[(y0 - y) * width + (x0 - x)] = color;
            }
            if ((y0 + x) >= 0 && (y0 + x) < height) {
                if ((x0 + y) >= 0 && (x0 + y) < width)
                    viewport_instance->frame_buf[(y0 + x) * width + (x0 + y)] = color;
                if ((x0 - y) >= 0 && (x0 - y) < width)
                    viewport_instance->frame_buf[(y0 + x) * width + (x0 - y)] = color;
            }
            if ((y0 - x) >= 0 && (y0 - x) < height) {
                if ((x0 + y) >= 0 && (x0 + y) < width)
                    viewport_instance->frame_buf[(y0 - x) * width + (x0 + y)] = color;
                if ((x0 - y) >= 0 && (x0 - y) < width)
                    viewport_instance->frame_buf[(y0 - x) * width + (x0 - y)] = color;
            }
        }
        
        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}