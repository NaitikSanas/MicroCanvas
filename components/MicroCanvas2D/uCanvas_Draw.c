#include "uCanvas_Draw.h"
#include "uCanvas2D_Display_Setup.h"
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "fontx.h"
#include "stdbool.h"

// Helper: Set pixel in RGB565
static void set_pixel(uCanvas2D_RenderBuffer_t* buf, int x, int y, uint16_t color) {
    if (x < 0 || y < 0 || x >= buf->width || y >= buf->height) return;
    buf->pixels[y * buf->width + x] = color;
}

// Draw line (Bresenham) with thickness
void uCanvas2D_DrawLine(uCanvas2D_RenderBuffer_t* buf, int x0, int y0, int x1, int y1, uint16_t color, int thickness) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    int half = thickness / 2;
    if (thickness < 1) thickness = 1;
    // printf("Drawing line from (%d, %d) to (%d, %d) with color %04X and thickness %d\n", x0, y0, x1, y1, color, thickness);
    // Draw the line with thickness                                                         
    while (1) {
        for (int tx = -half; tx <= half; tx++)
            for (int ty = -half; ty <= half; ty++)
                set_pixel(buf, x0 + tx, y0 + ty, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// Draw rectangle with fill and thickness
void uCanvas2D_DrawRect(uCanvas2D_RenderBuffer_t* buf, int x, int y, int w, int h, uint16_t color, int fill, int thickness) {
    if (fill) {
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
                set_pixel(buf, x + j, y + i, color);
    } else {
        for (int t = 0; t < thickness; t++) {
            // Top
            for (int j = 0; j < w; ++j)
                set_pixel(buf, x + j, y + t, color);
            // Bottom
            for (int j = 0; j < w; ++j)
                set_pixel(buf, x + j, y + h - 1 - t, color);
            // Left
            for (int i = 0; i < h; ++i)
                set_pixel(buf, x + t, y + i, color);
            // Right
            for (int i = 0; i < h; ++i)
                set_pixel(buf, x + w - 1 - t, y + i, color);
        }
    }
}

// Draw circle with fill and thickness
void uCanvas2D_DrawCircle(uCanvas2D_RenderBuffer_t* buf, int xc, int yc, int r, uint16_t color, int fill, int thickness) {
    if (fill) {
        for (int y = -r; y <= r; y++) {
            for (int x = -r; x <= r; x++) {
                if (x * x + y * y <= r * r)
                    set_pixel(buf, xc + x, yc + y, color);
            }
        }
    } else {
        for (int t = 0; t < thickness; t++) {
            int rr = r - t;
            int x = 0, y = rr, d = 3 - 2 * rr;
            while (y >= x) {
                set_pixel(buf, xc + x, yc + y, color);
                set_pixel(buf, xc - x, yc + y, color);
                set_pixel(buf, xc + x, yc - y, color);
                set_pixel(buf, xc - x, yc - y, color);
                set_pixel(buf, xc + y, yc + x, color);
                set_pixel(buf, xc - y, yc + x, color);
                set_pixel(buf, xc + y, yc - x, color);
                set_pixel(buf, xc - y, yc - x, color);
                x++;
                if (d > 0) { y--; d = d + 4 * (x - y) + 10; }
                else { d = d + 4 * x + 6; }
            }
        }
    }
}

// Draw triangle with fill and thickness
void uCanvas2D_DrawTriangle(uCanvas2D_RenderBuffer_t* buf, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color, int fill, int thickness) {
    if (fill) {
        // Sort vertices by y
        if (y0 > y1) { int t; t = y0; y0 = y1; y1 = t; t = x0; x0 = x1; x1 = t; }
        if (y1 > y2) { int t; t = y1; y1 = y2; y2 = t; t = x1; x1 = x2; x2 = t; }
        if (y0 > y1) { int t; t = y0; y0 = y1; y1 = t; t = x0; x0 = x1; x1 = t; }
        int total_height = y2 - y0;
        for (int i = 0; i < total_height; i++) {
            int second_half = i > y1 - y0 || y1 == y0;
            int segment_height = second_half ? y2 - y1 : y1 - y0;
            float alpha = (float)i / total_height;
            float beta  = (float)(i - (second_half ? y1 - y0 : 0)) / segment_height;
            int ax = x0 + (x2 - x0) * alpha;
            int bx = second_half ? x1 + (x2 - x1) * beta : x0 + (x1 - x0) * beta;
            if (ax > bx) { int t = ax; ax = bx; bx = t; }
            for (int j = ax; j <= bx; j++)
                set_pixel(buf, j, y0 + i, color);
        }
    } else {
        uCanvas2D_DrawLine(buf, x0, y0, x1, y1, color, thickness);
        uCanvas2D_DrawLine(buf, x1, y1, x2, y2, color, thickness);
        uCanvas2D_DrawLine(buf, x2, y2, x0, y0, color, thickness);
    }
}

// Draw ellipse with fill and thickness
void uCanvas2D_DrawEllipse(uCanvas2D_RenderBuffer_t* buf, int xc, int yc, int rx, int ry, uint16_t color, int fill, int thickness) {
    if (fill) {
        for (int y = -ry; y <= ry; y++) {
            for (int x = -rx; x <= rx; x++) {
                if ((x * x) * (ry * ry) + (y * y) * (rx * rx) <= (rx * rx) * (ry * ry))
                    set_pixel(buf, xc + x, yc + y, color);
            }
        }
    } else {
        for (int t = 0; t < thickness; t++) {
            int rx_t = rx - t;
            int ry_t = ry - t;
            int x = 0, y = ry_t;
            int rx2 = rx_t * rx_t, ry2 = ry_t * ry_t;
            int tworx2 = 2 * rx2, twory2 = 2 * ry2;
            int px = 0, py = tworx2 * y;
            int p = round(ry2 - (rx2 * ry_t) + (0.25 * rx2));
            while (px < py) {
                set_pixel(buf, xc + x, yc + y, color);
                set_pixel(buf, xc - x, yc + y, color);
                set_pixel(buf, xc + x, yc - y, color);
                set_pixel(buf, xc - x, yc - y, color);
                x++;
                px += twory2;
                if (p < 0) p += ry2 + px;
                else { y--; py -= tworx2; p += ry2 + px - py; }
            }
            p = round(ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2);
            while (y >= 0) {
                set_pixel(buf, xc + x, yc + y, color);
                set_pixel(buf, xc - x, yc + y, color);
                set_pixel(buf, xc + x, yc - y, color);
                set_pixel(buf, xc - x, yc - y, color);
                y--;
                py -= tworx2;
                if (p > 0) p += rx2 - py;
                else { x++; px += twory2; p += rx2 - py + px; }
            }
        }
    }
}

// Draw sprite (assume src is RGBA565, w x h), draw only pixel with 0x01 alpha (LSB)
void uCanvas2D_DrawSprite(uCanvas2D_RenderBuffer_t* buf, int x, int y, const uint16_t* sprite, int w, int h, int fill, int thickness) {
    (void)fill; (void)thickness;

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            uint16_t pixel = sprite[i * w + j];
            if (pixel & 0x01) { // Only draw if alpha bit (LSB) is set
                set_pixel(buf, x + j, y + i, pixel & 0xFFFE); // Mask out alpha bit
            }
        }
    }
}


int uCanvas_Draw_Char(uCanvas2D_RenderBuffer_t *fb, int x, int y, char ascii, FontxFile *fx, uint16_t color1,uint16_t color2, uint16_t font_direction, uint8_t ul_en) {
    uint16_t xx,yy,bit,ofs;
	unsigned char fonts[128]; // font pattern
	unsigned char pw, ph;
	int h,w;
	uint16_t mask;
	bool rc;

	rc = GetFontx(fx, ascii, fonts, &pw, &ph);
	if (!rc) return 0;

	int16_t xd1 = 0;
	int16_t yd1 = 0;
	int16_t xd2 = 0;
	int16_t yd2 = 0;
	uint16_t xss = 0;
	uint16_t yss = 0;
	int16_t xsd = 0;
	int16_t ysd = 0;
	int16_t next = 0;
	uint16_t x0  = 0;
	uint16_t x1  = 0;
	uint16_t y0  = 0;
	uint16_t y1  = 0;
	if (font_direction == 0) {
		xd1 = +1;
		yd1 = +1; //-1;
		xd2 =  0;
		yd2 =  0;
		xss =  x;
		yss =  y - (ph - 1);
		xsd =  1;
		ysd =  0;
		next = x + pw;

		x0	= x;
		y0	= y - (ph-1);
		x1	= x + (pw-1);
		y1	= y;
	} else if (font_direction == 2) {
		xd1 = -1;
		yd1 = -1; //+1;
		xd2 =  0;
		yd2 =  0;
		xss =  x;
		yss =  y + ph + 1;
		xsd =  1;
		ysd =  0;
		next = x - pw;

		x0	= x - (pw-1);
		y0	= y;
		x1	= x;
		y1	= y + (ph-1);
	} else if (font_direction == 1) {
		xd1 =  0;
		yd1 =  0;
		xd2 = -1;
		yd2 = +1; //-1;
		xss =  x + ph;
		yss =  y;
		xsd =  0;
		ysd =  1;
		next = y + pw; //y - pw;

		x0	= x;
		y0	= y;
		x1	= x + (ph-1);
		y1	= y + (pw-1);
	} else if (font_direction == 3) {
		xd1 =  0;
		yd1 =  0;
		xd2 = +1;
		yd2 = -1; //+1;
		xss =  x - (ph - 1);
		yss =  y;
		xsd =  0;
		ysd =  1;
		next = y - pw; //y + pw;

		x0	= x - (ph-1);
		y0	= y - (pw-1);
		x1	= x;
		y1	= y;
	}


	int bits;
	ofs = 0;
	yy = yss;
	xx = xss;

	for(h=0;h<ph;h++) {
		if(xsd) xx = xss;
		if(ysd) yy = yss;
		//for(w=0;w<(pw/8);w++) {
		bits = pw;
		for(w=0;w<((pw+4)/8);w++) {
			mask = 0x80;
			for(bit=0;bit<8;bit++) {
				bits--;
				if (bits < 0) continue;
				if (fonts[ofs] & mask) {
                    set_pixel(fb, xx, yy, color1);
				} 
                if (h == (ph-2) && ul_en)
					set_pixel(fb, xx, yy, color2);
				if (h == (ph-1) && ul_en)
                    set_pixel(fb, xx, yy, color2);
				xx = xx + xd1;
				yy = yy + yd2;
				mask = mask >> 1;
			}
			ofs++;
		}
		yy = yy + yd1;
		xx = xx + xd2;
	}

	if (next < 0) next = 0;
	return next;
}
