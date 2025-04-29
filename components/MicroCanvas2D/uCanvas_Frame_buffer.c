#include "uCanvas_Frame_buffer.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"
#include "uCanvasDataTypes.h"
int16_t uCanvas_framebuffer_init(Framebuffer_t *fb, uint16_t height, uint16_t width) {
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
    uCanvas_framebuffer_clear(fb, 0xFFFF);
}


void uCanvas_framebuffer_clear(Framebuffer_t *fb, uint16_t color) {
    for (uint32_t i = 0; i < fb->height * fb->width; i++) {
        fb->buffer[i] = color;
    }
}

void uCanvas_framebuffer_draw_pixel(Framebuffer_t *fb, int16_t x, int16_t y, uint16_t color) {
    // if (x >= 0 && x < fb->width && y >= 0 && y < fb->height ) {
        fb->buffer[y * fb->width + x] = color;
    // }
}
void uCanvas_framebuffer_draw_line(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy, e2;
    while (1) {
        uCanvas_framebuffer_draw_pixel(fb, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
    
}

void uCanvas_framebuffer_draw_rectangle(Framebuffer_t *fb, int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color) {
    uCanvas_framebuffer_draw_line(fb, x, y, x + width, y, color);
    uCanvas_framebuffer_draw_line(fb, x, y, x, y + height, color);
    uCanvas_framebuffer_draw_line(fb, x + width, y, x + width, y + height, color);
    uCanvas_framebuffer_draw_line(fb, x, y + height, x + width, y + height, color);
}

void uCanvas_framebuffer_draw_filled_rectangle(Framebuffer_t *fb, int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color) {
    for (int16_t i = 0; i < height; i++) {
        uCanvas_framebuffer_draw_line(fb, x, y + i, x + width, y + i, color);
    }
}

void uCanvas_framebuffer_draw_circle(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t radius, uint16_t color) {
    int16_t x = radius, y = 0;
    int16_t err = 1 - x;
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

void uCanvas_framebuffer_draw_filled_circle(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t radius, uint16_t color) {
    for (int16_t y = -radius; y <= radius; y++) {
        for (int16_t x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                uCanvas_framebuffer_draw_pixel(fb, x0 + x, y0 + y, color);
            }
        }
    }
}

void uCanvas_framebuffer_draw_ellipse(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color) {
    int16_t x, y;
    for (y = -ry; y <= ry; y++) {
        for (x = -rx; x <= rx; x++) {
            if (((x * x) * (ry * ry) + (y * y) * (rx * rx)) <= (rx * rx * ry * ry)) {
                uCanvas_framebuffer_draw_pixel(fb, x0 + x, y0 + y, color);
            }
        }
    }
}

void uCanvas_framebuffer_draw_filled_ellipse(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color) {
    uCanvas_framebuffer_draw_ellipse(fb, x0, y0, rx, ry, color);
}

void uCanvas_framebuffer_draw_triangle(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    uCanvas_framebuffer_draw_line(fb, x0, y0, x1, y1, color);
    uCanvas_framebuffer_draw_line(fb, x1, y1, x2, y2, color);
    uCanvas_framebuffer_draw_line(fb, x2, y2, x0, y0, color);
}

void uCanvas_framebuffer_draw_filled_triangle(Framebuffer_t *fb, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    // Simple scanline filling (not optimized)
    for (int16_t y = y0; y <= y2; y++) {
        for (int16_t x = x0; x <= x2; x++) {
            uCanvas_framebuffer_draw_pixel(fb, x, y, color);
        }
    }
}
inline uint16_t ucanvas_conv_rgba565_to_rgb565(uint16_t rgba565) {
    // Mask to extract RGB components
    uint16_t r = (rgba565 & 0xF800); // Red (5 bits)
    uint16_t g = (rgba565 & 0x07E0); // Green (6 bits)
    uint16_t b = (rgba565 & 0x003E) >> 1; // Blue (5 bits)
    
    // Combine RGB without the alpha channel
    uint16_t rgb565 = r | g | b;
    return rgb565;
}

void uCanvas_framebuffer_draw_bitmap(Framebuffer_t *fb, uCanvas_universal_obj_t *obj) { 
    int offset_x = obj->properties.position.x;
    int offset_y = obj->properties.position.y;
    uint16_t sprite_width =  obj->width ;
    uint16_t sprite_height = obj->height;
    if (obj->properties.visiblity == INVISIBLE) {	
        return;
    }
    for (int row = 0; row < sprite_height; row++) {
        int pos_y = row + offset_y;
        if (pos_y < 0 || pos_y >= fb->height) {
            continue;
        }
        uint16_t *src_row = &obj->sprite_buffer[row * sprite_width];
        uint16_t *dest_row = &fb->buffer[pos_y * fb->width];
        int start_col = (offset_x < 0) ? -offset_x : 0;  // First visible column
        int end_col = (offset_x + sprite_width >  fb->width) ?  fb->width - offset_x : sprite_width;
        // Iterate over visible columns
        for (int col = start_col; col < end_col; col++) {
            int pos_x = col + offset_x;
            if ((src_row[col] & 0x01)) { 
                dest_row[pos_x] = ucanvas_conv_rgba565_to_rgb565(src_row[col]);
            }
        }
    }
}


int uCanvas_framebuffer_Draw_Char(Framebuffer_t *fb, int x, int y, char ascii, FontxFile *fx, uint16_t color1,uint16_t color2, uint16_t font_direction, uint8_t ul_en) {
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
                    uCanvas_framebuffer_draw_pixel(fb, xx, yy, color1);
				} 
                if (h == (ph-2) && ul_en)
					uCanvas_framebuffer_draw_pixel(fb, xx, yy, color2);
				if (h == (ph-1) && ul_en)
                    uCanvas_framebuffer_draw_pixel(fb, xx, yy, color2);
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


int uCanvas_Framebuffer_Draw_Text(Framebuffer_t *fb, int x, int y, const char *text, FontxFile *fx, uint16_t color1, uint16_t color2, uint16_t font_direction, uint8_t ul_en) {
    int length = strlen((char *)text);
	for(int i=0;i<length;i++) {
		if (font_direction == 0)
			x = uCanvas_framebuffer_Draw_Char(fb, x, y, text[i], fx,color1,color2,font_direction,ul_en);
		if (font_direction == 1)
			y = uCanvas_framebuffer_Draw_Char(fb, x, y, text[i], fx,color1,color2,font_direction,ul_en);
		if (font_direction == 2)
			x = uCanvas_framebuffer_Draw_Char(fb, x, y, text[i], fx,color1,color2,font_direction,ul_en);
		if (font_direction == 3)
			y = uCanvas_framebuffer_Draw_Char(fb, x, y, text[i], fx,color1,color2,font_direction,ul_en);
	}
	if (font_direction == 0) return x;
	if (font_direction == 2) return x;
	if (font_direction == 1) return y;
	if (font_direction == 3) return y;
	return 0;
}
