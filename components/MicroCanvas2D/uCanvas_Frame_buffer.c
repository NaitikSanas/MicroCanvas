#include "uCanvas_Frame_buffer.h"
#include <stdlib.h>

void framebuffer_init(Framebuffer_t *fb) {
    framebuffer_clear(fb, 0x0000);
}

void framebuffer_clear(Framebuffer_t *fb, uint16_t color) {
    for (int y = 0; y < FRAMEBUFFER_HEIGHT; y++) {
        for (int x = 0; x < FRAMEBUFFER_WIDTH; x++) {
            fb->buffer[y][x] = color;
        }
    }
}

void framebuffer_draw_pixel(Framebuffer_t *fb, int x, int y, uint16_t color) {
    if (x >= 0 && x < FRAMEBUFFER_WIDTH && y >= 0 && y < FRAMEBUFFER_HEIGHT) {
        fb->buffer[y][x] = color;
    }
}

void framebuffer_draw_line(Framebuffer_t *fb, int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        framebuffer_draw_pixel(fb, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void framebuffer_draw_rectangle(Framebuffer_t *fb, int x, int y, int width, int height, uint16_t color) {
    framebuffer_draw_line(fb, x, y, x + width, y, color);
    framebuffer_draw_line(fb, x, y, x, y + height, color);
    framebuffer_draw_line(fb, x + width, y, x + width, y + height, color);
    framebuffer_draw_line(fb, x, y + height, x + width, y + height, color);
}

void framebuffer_draw_filled_rectangle(Framebuffer_t *fb, int x, int y, int width, int height, uint16_t color) {
    for (int i = 0; i < height; i++) {
        framebuffer_draw_line(fb, x, y + i, x + width, y + i, color);
    }
}

void framebuffer_draw_circle(Framebuffer_t *fb, int x0, int y0, int radius, uint16_t color) {
    int x = radius, y = 0;
    int err = 1 - x;
    while (x >= y) {
        framebuffer_draw_pixel(fb, x0 + x, y0 + y, color);
        framebuffer_draw_pixel(fb, x0 - x, y0 + y, color);
        framebuffer_draw_pixel(fb, x0 + x, y0 - y, color);
        framebuffer_draw_pixel(fb, x0 - x, y0 - y, color);
        framebuffer_draw_pixel(fb, x0 + y, y0 + x, color);
        framebuffer_draw_pixel(fb, x0 - y, y0 + x, color);
        framebuffer_draw_pixel(fb, x0 + y, y0 - x, color);
        framebuffer_draw_pixel(fb, x0 - y, y0 - x, color);
        y++;
        if (err <= 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

void framebuffer_draw_filled_circle(Framebuffer_t *fb, int x0, int y0, int radius, uint16_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                framebuffer_draw_pixel(fb, x0 + x, y0 + y, color);
            }
        }
    }
}

void framebuffer_draw_ellipse(Framebuffer_t *fb, int x0, int y0, int rx, int ry, uint16_t color) {
    int x, y;
    for (y = -ry; y <= ry; y++) {
        for (x = -rx; x <= rx; x++) {
            if (((x * x) * (ry * ry) + (y * y) * (rx * rx)) <= (rx * rx * ry * ry)) {
                framebuffer_draw_pixel(fb, x0 + x, y0 + y, color);
            }
        }
    }
}

void framebuffer_draw_filled_ellipse(Framebuffer_t *fb, int x0, int y0, int rx, int ry, uint16_t color) {
    framebuffer_draw_ellipse(fb, x0, y0, rx, ry, color);
}

void framebuffer_draw_triangle(Framebuffer_t *fb, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    framebuffer_draw_line(fb, x0, y0, x1, y1, color);
    framebuffer_draw_line(fb, x1, y1, x2, y2, color);
    framebuffer_draw_line(fb, x2, y2, x0, y0, color);
}

void framebuffer_draw_filled_triangle(Framebuffer_t *fb, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    // Simple scanline filling (not optimized)
    for (int y = y0; y <= y2; y++) {
        for (int x = x0; x <= x2; x++) {
            framebuffer_draw_pixel(fb, x, y, color);
        }
    }
}
