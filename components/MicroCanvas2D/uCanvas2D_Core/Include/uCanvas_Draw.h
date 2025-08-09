#pragma once

#include <stdint.h>
#include "uCanvas2D_Display_Setup.h"
#include "uCanvasDataTypes.h"
#include "fontx.h"
// Draw a line in RGB565, with thickness
void uCanvas2D_DrawLine(uCanvas2D_RenderBuffer_t* buf, int x0, int y0, int x1, int y1, uint16_t color, int thickness);

// Draw a rectangle in RGB565, with fill and thickness
void uCanvas2D_DrawRect(uCanvas2D_RenderBuffer_t* buf, int x, int y, int w, int h, uint16_t color, int fill, int thickness);

// Draw a circle in RGB565, with fill and thickness
void uCanvas2D_DrawCircle(uCanvas2D_RenderBuffer_t* buf, int xc, int yc, int r, uint16_t color, int fill, int thickness);

// Draw a triangle in RGB565, with fill and thickness
void uCanvas2D_DrawTriangle(uCanvas2D_RenderBuffer_t* buf, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color, int fill, int thickness);

// Draw an ellipse in RGB565, with fill and thickness
void uCanvas2D_DrawEllipse(uCanvas2D_RenderBuffer_t* buf, int xc, int yc, int rx, int ry, uint16_t color, int fill, int thickness);

// Draw a sprite (RGB565, w x h), fill and thickness are ignored
void uCanvas2D_DrawSprite(uCanvas2D_RenderBuffer_t* buf, int x, int y, const uint16_t* sprite, int w, int h,uCanvas_color_format_t color_format);

uint16_t IRAM_ATTR convertToRGB565(color_t color);
void uCanvas_Load_FontX(void);
int uCanvas_Draw_FONTX(uCanvas2D_RenderBuffer_t *fb, int x, int y, char ascii, FontType_t font_type, uint16_t color1,uint16_t color2, uint16_t font_direction, uint8_t ul_en);
int uCanvas_Draw_Text(uCanvas2D_RenderBuffer_t *fb, int x, int y, char* text, FontType_t font_type, uint16_t color1, uint16_t color2, uint8_t font_direction,uint8_t ul_en);
void IRAM_ATTR uCanvas_Draw_SFONT_TextBox(uCanvas2D_RenderBuffer_t *fb, uCanvas_universal_obj_t* obj);