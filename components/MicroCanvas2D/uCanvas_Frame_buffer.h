#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "fontx.h"

typedef struct { 
    uint16_t height;
    uint16_t width;
    uint16_t* buffer;
} Framebuffer_t;

int16_t uCanvas_framebuffer_init(Framebuffer_t *fb, uint16_t height, uint16_t width);
void uCanvas_framebuffer_clear(Framebuffer_t *fb, uint16_t color);
void uCanvas_framebuffer_draw_pixel(Framebuffer_t *fb, int16_t x, int16_t y, uint16_t color);

void uCanvas_framebuffer_draw_rectangle(Framebuffer_t *fb, int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color);
void uCanvas_framebuffer_draw_filled_rectangle(Framebuffer_t *fb, int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color);

void uCanvas_framebuffer_draw_circle(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t radius, uint16_t color);
void uCanvas_framebuffer_draw_filled_circle(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t radius, uint16_t color);

void uCanvas_framebuffer_draw_ellipse(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color);
void uCanvas_framebuffer_draw_filled_ellipse(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color);

void uCanvas_framebuffer_draw_triangle(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void uCanvas_framebuffer_draw_filled_triangle(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

void uCanvas_framebuffer_draw_line(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
int uCanvas_framebuffer_Draw_Char(Framebuffer_t *fb, int x, int y, char ascii, FontxFile *fx, uint16_t color1,uint16_t color2, uint16_t font_direction, uint8_t ul_en);
int uCanvas_Framebuffer_Draw_Text(Framebuffer_t *fb, int x, int y, const char *text, FontxFile *fx, uint16_t color1, uint16_t color2, uint16_t font_direction, uint8_t ul_en);
#endif // FRAMEBUFFER_H