#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define FRAMEBUFFER_WIDTH  240  // Adjust as needed
#define FRAMEBUFFER_HEIGHT 240  // Adjust as needed

typedef struct {
    uint16_t buffer[FRAMEBUFFER_HEIGHT][FRAMEBUFFER_WIDTH];
} Framebuffer_t;

void framebuffer_init(Framebuffer_t *fb);
void framebuffer_clear(Framebuffer_t *fb, uint16_t color);
void framebuffer_draw_pixel(Framebuffer_t *fb, int x, int y, uint16_t color);
void framebuffer_draw_rectangle(Framebuffer_t *fb, int x, int y, int width, int height, uint16_t color);
void framebuffer_draw_filled_rectangle(Framebuffer_t *fb, int x, int y, int width, int height, uint16_t color);
void framebuffer_draw_circle(Framebuffer_t *fb, int x0, int y0, int radius, uint16_t color);
void framebuffer_draw_filled_circle(Framebuffer_t *fb, int x0, int y0, int radius, uint16_t color);
void framebuffer_draw_ellipse(Framebuffer_t *fb, int x0, int y0, int rx, int ry, uint16_t color);
void framebuffer_draw_filled_ellipse(Framebuffer_t *fb, int x0, int y0, int rx, int ry, uint16_t color);
void framebuffer_draw_triangle(Framebuffer_t *fb, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);
void framebuffer_draw_filled_triangle(Framebuffer_t *fb, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);
void framebuffer_draw_line(Framebuffer_t *fb, int x0, int y0, int x1, int y1, uint16_t color);

#endif // FRAMEBUFFER_H
