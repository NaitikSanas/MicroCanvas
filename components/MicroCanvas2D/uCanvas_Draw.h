#pragma once

#include <stdint.h>
#include "uCanvas2D_Display_Setup.h"

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
void uCanvas2D_DrawSprite(uCanvas2D_RenderBuffer_t* buf, int x, int y, const uint16_t* sprite, int w, int h, int fill, int thickness);