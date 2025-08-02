#include "uCanvas_Draw.h"
#include "uCanvas2D_Display_Setup.h"
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "stdbool.h"
#include "uCanvas2D_Display_Setup.h"
#include "uCanvas2D_Acceleration.h"


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
void uCanvas2D_DrawSprite(uCanvas2D_RenderBuffer_t* buf, int x, int y, const uint16_t* sprite, int w, int h,sprite_color_format_t color_format) {

    if(buf->use_ppa){
        // If using PPA, we need to handle the sprite differently
        switch(color_format) {
            case SPRITE2D_COLOR_RGBA565:
                ppa_blend_bitmap((void*)sprite, w, h, 0, 0,PPA_RGB565, buf->pixels, buf->width, buf->height, x, y, buf->width * buf->height * sizeof(uint16_t),PPA_RGB565);
                return;
            case SPRITE2D_COLOR_ARGB8888:
                ppa_blend_bitmap((void*)sprite, w, h, 0, 0,PPA_ARGB8888, buf->pixels, buf->width, buf->height, x, y, buf->width * buf->height * sizeof(uint16_t),PPA_RGB565);
                return;
            case SPRITE2D_COLOR_RGB565:
                ppa_blend_bitmap((void*)sprite, w, h, 0, 0,PPA_RGB565, buf->pixels, buf->width, buf->height, x, y, buf->width * buf->height * sizeof(uint16_t),PPA_RGB565);
                return;
                break; // No need to convert
            default :
                printf ("Unsupported sprite color format: %d\n", color_format);
                return; // Unsupported color format, do nothing
        }
        return;
    }
    else {
        switch(color_format) {
        case SPRITE2D_COLOR_RGBA565:
            for (int i = 0; i < h; ++i) {
                for (int j = 0; j < w; ++j) {
                    uint16_t pixel = sprite[i * w + j];
                    if (pixel & 0x01) { // Only draw if alpha bit (LSB) is set
                        set_pixel(buf, x + j, y + i, pixel & 0xFFFE); // Mask out alpha bit
                    }
                }
            }
            break;
        case SPRITE2D_COLOR_ARGB8888:
            for (int i = 0; i < h; ++i) {
                for (int j = 0; j < w; ++j) {
                    uint32_t pixel = ((const uint32_t*)sprite)[i * w + j];
                    uint16_t rgba565 = ((pixel & 0xFF0000) >> 8) | ((pixel & 0x00FF00) >> 5) | ((pixel & 0x0000FF) >> 3);
                    if (rgba565 & 0x01) { // Only draw if alpha bit (LSB) is set
                        set_pixel(buf, x + j, y + i,rgba565); // Mask out alpha bit and convert to RGB565
                    }
                }
            }
            break;

        case SPRITE2D_COLOR_RGB565:
            for (int i = 0; i < h; ++i) {
                for (int j = 0; j < w; ++j) {
                    uint16_t pixel = sprite[i * w + j];
                    set_pixel(buf, x + j, y + i, pixel); // Directly draw RGB565
                }
            }
            break;
        default:
            // Unsupported color format, do nothing
            return;
        }
    }
}
#include "esp_spiffs.h"
#define TAG "uCanvas_Draw"
#include "esp_vfs.h"
#include "esp_log.h"
FontxFile fx16G[2];
FontxFile fx24G[2];
FontxFile fx32G[2];
FontxFile fx32L[2];
FontxFile fx16M[2];
FontxFile fx24M[2];
FontxFile fx10M[2];
static void SPIFFS_Directory(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(__FUNCTION__,"d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}


void uCanvas_Load_FontX(void){
    esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = NULL,
		.max_files = 12,
		.format_if_mount_failed =true
	};

	// Use settings defined above toinitialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is anall-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		}
		return;
	}

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(NULL, &total,&used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	SPIFFS_Directory("/spiffs/");
	InitFontx(fx16G,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
	InitFontx(fx24G,"/spiffs/ILGH24XB.FNT",""); // 12x24Dot Gothic
	InitFontx(fx32G,"/spiffs/ILGH32XB.FNT",""); // 16x32Dot Gothic
	InitFontx(fx32L,"/spiffs/LATIN32B.FNT",""); // 16x32Dot Latin

	
	InitFontx(fx16M,"/spiffs/ILMH16XB.FNT",""); // 8x16Dot Mincyo
	InitFontx(fx24M,"/spiffs/ILMH24XB.FNT",""); // 12x24Dot Mincyo
	InitFontx(fx10M,"/spiffs/FONT10X20.FNT",""); // 16x32Dot Mincyo
	
}
#include "fonts.h"
void* get_font_by_name(FontType_t font_type){
    void* activefont = NULL;
    switch (font_type)
	{
	case FONTX_16G:	 return fx16G; break;
	case FONTX_24G:	 return fx24G; break;
	case FONTX_32G:  return fx32G; break;
	case FONTX_32L:  return fx32L; break;
	case FONTX_16M:  return fx16M; break;
	case FONTX_24M : return fx24M; break;
	case FONTX_10M : return fx10M; break;

    case SFONT_8 :   return &Font8; break;
    case SFONT_12 :  return &Font12;break;
    case SFONT_16 :  return &Font16; break;
    case SFONT_20 :  return &Font20; break;
    case SFONT_24 :  return &Font24; break;
    case SFONT_ROBOTO_ITALIC_32 : return &Roboto_Italic_32; break;
    case SFONT_SIXTYFOUR_32     : return &Sixtyfour_32; break;
    case SFONT_BITCOUNT_32      : return &BitcountPropDouble32; break;
	default:
		activefont = fx10M;
		break;
	}
    return activefont;
}

int uCanvas_Draw_FONTX(uCanvas2D_RenderBuffer_t *fb, int x, int y, char ascii,
                      FontType_t font_type, uint16_t color1, uint16_t color2,
                      uint16_t font_direction, uint8_t ul_en) {
    unsigned char fonts[128]; // glyph bitmap
    uint8_t pw, ph;

    if (!GetFontx(get_font_by_name(font_type), ascii, fonts, &pw, &ph))
        return 0;

    int16_t xd1 = 0, yd1 = 0, xd2 = 0, yd2 = 0;
    int16_t xx = 0, yy = 0;
    int16_t next = 0;

    // Direction setup
    switch (font_direction) {
        case 0:
            xd1 = +1; yd1 = +1;
            xx = x; yy = y - (ph - 1);
            next = x + pw;
            break;
        case 1:
            xd2 = -1; yd2 = +1;
            xx = x + ph; yy = y;
            next = y + pw;
            break;
        case 2:
            xd1 = -1; yd1 = -1;
            xx = x; yy = y + ph + 1;
            next = x - pw;
            break;
        case 3:
            xd2 = +1; yd2 = -1;
            xx = x - (ph - 1); yy = y;
            next = y - pw;
            break;
        default:
            return 0;
    }

    int ofs = 0;
    for (int h = 0; h < ph; h++) {
        int bits_remaining = pw;
        int16_t xline = xx;
        int16_t yline = yy;

        for (int w = 0; w < ((pw + 7) >> 3); w++) {
            uint8_t b = fonts[ofs++];
            for (int bit = 0; bit < 8 && bits_remaining > 0; bit++, bits_remaining--) {
                uint8_t is_on = b & (0x80 >> bit);
                if (is_on) {
                    set_pixel(fb, xline, yline, color1);
                } else if (ul_en && (h == ph - 2 || h == ph - 1)) {
                    set_pixel(fb, xline, yline, color2);
                }

                xline += xd1;
                yline += yd2;
            }
        }

        xx += xd2;
        yy += yd1;
    }

    return (next < 0) ? 0 : next;
}


#define DRAW_PIXEL(fb, x, y, color)  ((fb)->pixels[(y) * (fb)->width + (x)] = (color))

void uCanvas_Draw_SFONT(uCanvas2D_RenderBuffer_t *fb, sFONT* Font, int x, int y, char ascii,
                        uint16_t fg_color, uint16_t bg_color, uint16_t font_direction)
{
    if (!Font || !fb || !fb->pixels || ascii < ' ') return;

    uint8_t char_width = Font->Width;
    uint8_t char_height = Font->Height;
    uint8_t bytes_per_row = (char_width + 7) / 8;

    const uint8_t* glyph_ptr = &Font->table[(ascii - ' ') * char_height * bytes_per_row];

    for (int row = 0; row < char_height; row++) {
        for (int col = 0; col < char_width; col++) {
            int byte_index = col / 8;
            int bit_index = 7 - (col % 8);  // MSB first
            uint8_t byte = glyph_ptr[row * bytes_per_row + byte_index];

            if (byte & (1 << bit_index)) {
                DRAW_PIXEL(fb, x + col, y + row, fg_color);
            } else {
                // DRAW_PIXEL(fb, x + col, y + row, bg_color);
            }
        }
    }
}


// int uCanvas_Draw_SFONT(uCanvas2D_RenderBuffer_t *fb, sFONT* Font, int x, int y, char ascii,
//                     uint16_t color1, uint16_t color2, uint16_t font_direction) {
//     int row, col;
//     int bytes_per_row = (Font->Width + 7) / 8;  // number of bytes per row
//     const uint8_t* bitmap = &Font->table[(ascii - ' ') * Font->Height * bytes_per_row];

//     for (row = 0; row < Font->Height; row++) {
//         for (col = 0; col < Font->Width; col++) {
//             int byte_index = row * bytes_per_row + (col / 8);
//             uint8_t byte = bitmap[byte_index];
//             if (byte & (0x80 >> (col % 8))) {
//                 set_pixel(fb, x + col, y + row, color1);
//             } else {
//                 set_pixel(fb, x + col, y + row, color2);
//             }
//         }
//     }

//     return 0;
// }

// int uCanvas_Draw_SFONT(uCanvas2D_RenderBuffer_t *fb, sFONT* Font, int x, int y, char ascii,
//                        uint16_t color1, uint16_t color2, uint16_t font_direction) {
//     uint8_t char_width = Font->Width;
//     uint8_t char_height = Font->Height;
//     uint8_t bytes_per_row = (char_width + 7) / 8;

//     uint32_t offset = (ascii - ' ') * char_height * bytes_per_row;
//     const uint8_t *ptr = &Font->table[offset];

//     for (int row = 0; row < char_height; row++) {
//         for (int col = 0; col < char_width; col++) {
//             int byte_index = col / 8;
//             int bit_index = 7 - (col % 8); // MSB first

//             uint8_t byte = ptr[byte_index];
//             uint8_t bit = (byte >> bit_index) & 0x01;

//             uint16_t color = bit ? color1 : color2;
//             set_pixel(fb, x + col, y + row, color);
//         }
//         ptr += bytes_per_row;
//     }

//     return 0;
// }

// int uCanvas_Draw_SFONT(uCanvas2D_RenderBuffer_t *fb, sFONT* Font, int x, int y, char ascii,
//                     uint16_t color1, uint16_t color2,
//                     uint16_t font_direction) {
//     uint8_t char_width = Font->Width;
//     uint8_t char_height = Font->Height;
//     uint8_t bytes_per_column = (char_height + 7) / 8;

//     uint32_t offset = (ascii - ' ') * char_width * bytes_per_column;
//     const uint8_t *ptr = &Font->table[offset];

//     for (int col = 0; col < char_width; col++) {
//         for (int byte = 0; byte < bytes_per_column; byte++) {
//             uint8_t data = *ptr++;
//             for (int bit = 0; bit < 8; bit++) {
//                 int row = byte * 8 + bit;
//                 if (row >= char_height) continue;

//                 uint16_t color = (data & (1 << bit)) ? color1 : color2;
//                 set_pixel(fb, x + col, y + row, color);
//             }
//         }
//     }

//     return 0;
// }
void uCanvas_Draw_SFONT_Text(uCanvas2D_RenderBuffer_t *fb, int x, int y, const char *pString,
                         FontType_t font_type, uint16_t color1, uint16_t color2, int font_direction) {
    sFONT* Font = get_font_by_name(font_type);
    if (Font == NULL || pString == NULL) return;

    while (*pString != '\0') {
        uCanvas_Draw_SFONT(fb, Font, x, y, *pString, color1, color2, font_direction);
        pString++;
        x += Font->Width;
    }
}
int uCanvas_Draw_FONTX_Text(uCanvas2D_RenderBuffer_t *fb, int x, int y, char* text, FontType_t font_type, uint16_t color1, uint16_t color2, uint8_t font_direction,uint8_t ul_en){
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        int pos = uCanvas_Draw_FONTX(fb, x, y, text[i], font_type, color1, color2, font_direction, ul_en);
        if (font_direction == 0 || font_direction == 2) x = pos;
        else y = pos;
    }
    return (font_direction == 0 || font_direction == 2) ? x : y;
}


int uCanvas_Draw_Text(uCanvas2D_RenderBuffer_t *fb, int x, int y, char* text, FontType_t font_type, uint16_t color1, uint16_t color2, uint8_t font_direction,uint8_t ul_en){
    if(font_type < SFONT_8)uCanvas_Draw_FONTX_Text(fb,x,y,text,font_type,color1,color2,font_direction,ul_en);
    if(font_type >= SFONT_8)uCanvas_Draw_SFONT_Text(fb,x,y,text,font_type,color1,color2,font_direction);

    return 0;
}

