#include "uCanvas_Frame_buffer.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

int uCanvas_framebuffer_init(Framebuffer_t *fb, uint16_t height, uint16_t width) {
    printf("free %d",heap_caps_get_free_size(MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL));
    fb->height = height;
    fb->width = width;
    fb->buffer = heap_caps_malloc(sizeof(uint16_t)*fb->width*fb->height, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
	if (fb->buffer == NULL) {
		ESP_LOGE("uCanvas_FB", "heap_caps_malloc fail");
        return 0;
	} else {
		ESP_LOGI("uCanvas_FB", "heap_caps_malloc success");
        return 1;
	}
    uCanvas_framebuffer_clear(fb, 0x0000);
}


void uCanvas_framebuffer_clear(Framebuffer_t *fb, uint16_t color) {
    for (int i = 0; i < fb->height * fb->width; i++) {
        fb->buffer[i] = color;
    }
    printf("clr\r\n");
}

void uCanvas_framebuffer_draw_pixel(Framebuffer_t *fb, int x, int y, uint16_t color) {
    if (x >= 0 && x < fb->width && y >= 0 && y < fb->height ) {
        fb->buffer[y * fb->width + x] = color;
    }
}
void uCanvas_framebuffer_draw_line(Framebuffer_t *fb, int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        uCanvas_framebuffer_draw_pixel(fb, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void uCanvas_framebuffer_draw_rectangle(Framebuffer_t *fb, int x, int y, int width, int height, uint16_t color) {
    uCanvas_framebuffer_draw_line(fb, x, y, x + width, y, color);
    uCanvas_framebuffer_draw_line(fb, x, y, x, y + height, color);
    uCanvas_framebuffer_draw_line(fb, x + width, y, x + width, y + height, color);
    uCanvas_framebuffer_draw_line(fb, x, y + height, x + width, y + height, color);
}

void uCanvas_framebuffer_draw_filled_rectangle(Framebuffer_t *fb, int x, int y, int width, int height, uint16_t color) {
    for (int i = 0; i < height; i++) {
        uCanvas_framebuffer_draw_line(fb, x, y + i, x + width, y + i, color);
    }
}

void uCanvas_framebuffer_draw_circle(Framebuffer_t *fb, int x0, int y0, int radius, uint16_t color) {
    int x = radius, y = 0;
    int err = 1 - x;
    while (x >= y) {
        uCanvas_framebuffer_draw_pixel(fb, x0 + x, y0 + y, color);
        uCanvas_framebuffer_draw_pixel(fb, x0 - x, y0 + y, color);
        uCanvas_framebuffer_draw_pixel(fb, x0 + x, y0 - y, color);
        uCanvas_framebuffer_draw_pixel(fb, x0 - x, y0 - y, color);
        uCanvas_framebuffer_draw_pixel(fb, x0 + y, y0 + x, color);
        uCanvas_framebuffer_draw_pixel(fb, x0 - y, y0 + x, color);
        uCanvas_framebuffer_draw_pixel(fb, x0 + y, y0 - x, color);
        uCanvas_framebuffer_draw_pixel(fb, x0 - y, y0 - x, color);
        y++;
        if (err <= 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

void uCanvas_framebuffer_draw_filled_circle(Framebuffer_t *fb, int x0, int y0, int radius, uint16_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                uCanvas_framebuffer_draw_pixel(fb, x0 + x, y0 + y, color);
            }
        }
    }
}

void uCanvas_framebuffer_draw_ellipse(Framebuffer_t *fb, int x0, int y0, int rx, int ry, uint16_t color) {
    int x, y;
    for (y = -ry; y <= ry; y++) {
        for (x = -rx; x <= rx; x++) {
            if (((x * x) * (ry * ry) + (y * y) * (rx * rx)) <= (rx * rx * ry * ry)) {
                uCanvas_framebuffer_draw_pixel(fb, x0 + x, y0 + y, color);
            }
        }
    }
}

void uCanvas_framebuffer_draw_filled_ellipse(Framebuffer_t *fb, int x0, int y0, int rx, int ry, uint16_t color) {
    uCanvas_framebuffer_draw_ellipse(fb, x0, y0, rx, ry, color);
}

void uCanvas_framebuffer_draw_triangle(Framebuffer_t *fb, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    uCanvas_framebuffer_draw_line(fb, x0, y0, x1, y1, color);
    uCanvas_framebuffer_draw_line(fb, x1, y1, x2, y2, color);
    uCanvas_framebuffer_draw_line(fb, x2, y2, x0, y0, color);
}

void uCanvas_framebuffer_draw_filled_triangle(Framebuffer_t *fb, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    // Simple scanline filling (not optimized)
    for (int y = y0; y <= y2; y++) {
        for (int x = x0; x <= x2; x++) {
            uCanvas_framebuffer_draw_pixel(fb, x, y, color);
        }
    }
}

