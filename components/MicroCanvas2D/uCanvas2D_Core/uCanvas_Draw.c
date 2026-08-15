#include "uCanvas_Draw.h"
#include "uCanvas2D_Display_Setup.h"
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "stdbool.h"
#include "uCanvas2D_Display_Setup.h"
#include "uCanvas2D_Acceleration.h"
#include "esp_timer.h"
#include "uCanvasDataTypes.h"
// Helper: Set pixel in RGB565

void IRAM_ATTR set_pixel(uCanvas2D_RenderBuffer_t* buf, int x, int y, uint16_t color) {
    if (x < 0 || y < 0 || x >= buf->width || y >= buf->height) return;
    buf->pixels[y * buf->width + x] = color;
}

// Draw line (Bresenham) with thickness
void IRAM_ATTR uCanvas2D_DrawLine(uCanvas2D_RenderBuffer_t* buf, int x0, int y0, int x1, int y1, uint16_t color, int thickness) {
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
void IRAM_ATTR uCanvas2D_DrawRect(uCanvas2D_RenderBuffer_t* buf, int x, int y, int w, int h, uint16_t color, int fill, int thickness) {
    if (fill) {
        
        if(buf->use_ppa){
            #if(CONFIG_IDF_TARGET_ESP32P4)
            
            ppa_helper_fill(
            buf->pixels,
            buf->width * buf->height * sizeof(uint16_t),
            buf->width,
            buf->height,x,y,w,h, color, 0
            );
            #endif
        }else{
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
                set_pixel(buf, x + j, y + i, color);
        }
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
void IRAM_ATTR uCanvas2D_DrawCircle(uCanvas2D_RenderBuffer_t* buf, int xc, int yc, int r, uint16_t color, int fill, int thickness) {
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
void IRAM_ATTR uCanvas2D_DrawTriangle(uCanvas2D_RenderBuffer_t* buf, int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color, int fill, int thickness) {
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
void IRAM_ATTR uCanvas2D_DrawEllipse(uCanvas2D_RenderBuffer_t* buf, int xc, int yc, int rx, int ry, uint16_t color, int fill, int thickness) {
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
void IRAM_ATTR uCanvas2D_DrawSprite(uCanvas2D_RenderBuffer_t* buf, int x, int y, const uint16_t* sprite, int w, int h,uCanvas_color_format_t color_format) {

    if(buf->use_ppa){
        // If using PPA, we need to handle the sprite differently
        #if(CONFIG_IDF_TARGET_ESP32P4)
        switch(color_format) {
            
            case COLOR_RGBA565:
                ppa_blend_bitmap((void*)sprite, w, h, 0, 0,PPA_RGB565, buf->pixels, buf->width, buf->height, x, y, buf->width * buf->height * sizeof(uint16_t),PPA_RGB565);
                return;
            case COLOR_ARGB8888:
                ppa_blend_bitmap((void*)sprite, w, h, 0, 0,PPA_ARGB8888, buf->pixels, buf->width, buf->height, x, y, buf->width * buf->height * sizeof(uint16_t),PPA_RGB565);
                return;
            case COLOR_RGB565:
                ppa_blend_bitmap((void*)sprite, w, h, 0, 0,PPA_RGB565, buf->pixels, buf->width, buf->height, x, y, buf->width * buf->height * sizeof(uint16_t),PPA_RGB565);
                return;
                break; // No need to convert
            default :
                printf ("Unsupported sprite color format: %d\n", color_format);
                return; // Unsupported color format, do nothing
            
        }
        #endif
        return;
    }
    else {
        switch(color_format) {
        case COLOR_RGBA565:
            for (int i = 0; i < h; ++i) {
                for (int j = 0; j < w; ++j) {
                    uint16_t pixel = sprite[i * w + j];
                    if (pixel & 0x01) { // Only draw if alpha bit (LSB) is set
                        set_pixel(buf, x + j, y + i, pixel & 0xFFFE); // Mask out alpha bit
                    }
                }
            }
            break;
        case COLOR_ARGB8888:
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

        case COLOR_RGB565:
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


#include "fonts.h"
void* get_font_by_name(FontType_t font_type){
    void* activefont = NULL;
    switch (font_type)
	{
	// case FONTX_16G:	 return fx16G; break;
	// case FONTX_24G:	 return fx24G; break;
	// case FONTX_32G:  return fx32G; break;
	// case FONTX_32L:  return fx32L; break;
	// case FONTX_16M:  return fx16M; break;
	// case FONTX_24M : return fx24M; break;
	// case FONTX_10M : return fx10M; break;

    case SFONT_8 :   return &Font8; break;
    case SFONT_12 :  return &Font12;break;
    case SFONT_16 :  return &Font16; break;
    case SFONT_20 :  return &Font20; break;
    case SFONT_24 :  return &Font24; break;
    case SFONT_ROBOTO_ITALIC_32 : return &Roboto_Italic_32; break;
    case SFONT_SIXTYFOUR_32     : return &Sixtyfour_32; break;
    case SFONT_BITCOUNT_32      : return &BitcountPropDouble32; break;
	default:
		activefont = &Font8;
		break;
	}
    return activefont;
}

uint16_t uCanvas_Get_Font_Height(FontType_t FontType){
    sFONT* font = get_font_by_name(FontType);
    return font->Height;
}

uint16_t uCanvas_Get_Font_Width(FontType_t FontType){
    sFONT* font = get_font_by_name(FontType);
    return font->Width;
}


#define DRAW_PIXEL(fb, x, y, color)  ((fb)->pixels[(y) * (fb)->width + (x)] = (color))

void IRAM_ATTR uCanvas_Draw_SFONT(uCanvas2D_RenderBuffer_t *fb, sFONT* Font, int x, int y, char ascii,
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
                set_pixel(fb, x + col, y + row, fg_color);
            } else {
                // DRAW_PIXEL(fb, x + col, y + row, bg_color);
            }
        }
    }
}

void IRAM_ATTR uCanvas_Draw_SFONT_Text(uCanvas2D_RenderBuffer_t *fb, int x, int y, const char *pString,
                         FontType_t font_type, uint16_t color1, uint16_t color2, int font_direction) {
    sFONT* Font = get_font_by_name(font_type);
    if (Font == NULL || pString == NULL) return;

    while (*pString != '\0') {
        uCanvas_Draw_SFONT(fb, Font, x, y, *pString, color1, color2, font_direction);
        pString++;
        x += Font->Width-4;
    }
}

void copy_fb_region_to_textbuf(uCanvas2D_RenderBuffer_t *fb,
                               uCanvas2D_RenderBuffer_t *text_buf,
                               int src_x, int src_y,
                               int width, int height)
{
    // Clip to framebuffer bounds
    if (src_x < 0) { width += src_x; src_x = 0; }
    if (src_y < 0) { height += src_y; src_y = 0; }
    if (src_x + width > fb->width)  width  = fb->width  - src_x;
    if (src_y + height > fb->height) height = fb->height - src_y;
    if (width <= 0 || height <= 0) return;

    // Assume both fb->pixels and text_buf->pixels are RGB565
    uint16_t *fb_pixels   = (uint16_t *)fb->pixels;
    uint16_t *buf_pixels  = (uint16_t *)text_buf->pixels;

    for (int row = 0; row < height; row++) {
        memcpy(buf_pixels + row * text_buf->width,
               fb_pixels + (src_y + row) * fb->width + src_x,
               width * sizeof(uint16_t));
    }
}

void IRAM_ATTR uCanvas_Draw_SFONT_Advanced_TextBox(uCanvas2D_RenderBuffer_t *fb, uCanvas_universal_obj_t* obj) {
    uCanvas_TextBox_Properties_t* tp = obj->textbox_properties;
    char* str = tp->textbox_content;
    sFONT* Font = get_font_by_name(tp->font_type);
    if (!Font || !str) return;
    int inner_pos_x = 0;
    int inner_pos_y = 0;

    int font_h = Font->Height;
    int font_w = Font->Width;
    int textbox_w = tp->textbox_width;
    int textbox_h = tp->textbox_height;
    int base_y = inner_pos_y + tp->margin_y;
    int inner_px = textbox_w - 2 * tp->margin_x;
    if (inner_px < font_w) inner_px = font_w;
    int chars_per_line = inner_px / font_w;
    if (chars_per_line < 1) chars_per_line = 1;
    int words_per_line = tp->wrap_index;
    int max_lines = textbox_h / font_h;
    int line_count = 0;
    
    int caret_x = inner_pos_x + tp->margin_x;
    int caret_y = base_y;
    int left_bound = inner_pos_x + tp->margin_x;
    int right_bound = left_bound + inner_px;

    uint16_t font_color = convertToRGB565(obj->properties.color);
    uint16_t background_color = convertToRGB565(tp->background_color);
    uint16_t border_color = convertToRGB565(tp->border_color);
    
    if(obj->textbox_properties->textbox_updated){
        obj->textbox_properties->textbox_updated = false;   
        if(tp->fill_background){
        #if(CONFIG_IDF_TARGET_ESP32P4)
        ppa_helper_fill(
                        tp->text_draw_buf->pixels,
                        tp->text_draw_buf->width * tp->text_draw_buf->height * sizeof(uint16_t),
                        tp->text_draw_buf->width,
                        tp->text_draw_buf->height,
                        0, 0, tp->text_draw_buf->width, tp->text_draw_buf->height, background_color,0
                        );
        #endif
        }
        else {
            copy_fb_region_to_textbuf(fb,
                          tp->text_draw_buf,
                          obj->properties.position.x,
                          obj->properties.position.y,
                          tp->textbox_width,
                          tp->textbox_height);
        }
        // memset(tp->text_draw_buf->pixels, convertToRGB565(tp->background_color), tp->text_draw_buf->width * tp->text_draw_buf->height * sizeof(uint16_t)); 

        if (tp->border_thickness > 0) {
            uCanvas2D_DrawRect(tp->text_draw_buf, inner_pos_x, inner_pos_y, tp->textbox_width, tp->textbox_height,font_color, 0,tp->border_thickness);
        }
        
        while (*str && line_count < max_lines) {
            int len = 0;

            if (tp->text_wrap_mode == TEXT_WRAP_PER_SET_WORD_LEN) {
                int words = 0;
                int pixel_width = 0;
                int i = 0;
                while (str[i] && str[i] != '\n') {
                    int word_len = 0;
                    while (str[i + word_len] && str[i + word_len] != ' ' && str[i + word_len] != '\n') {
                        word_len++;
                    }
                    int word_px = word_len * font_w;
                    int space_px = (words > 0) ? font_w : 0;
                    if ((pixel_width + space_px + word_px) > inner_px || (words >= words_per_line)) break;
                    pixel_width += space_px + word_px;
                    i += word_len;
                    if (str[i] == ' ') i++;
                    words++;
                }
                len = i;
                if (len == 0 && str[0] && str[0] != '\n') {
                    int remaining = 0;
                    while (str[remaining] && str[remaining] != ' ' && str[remaining] != '\n') remaining++;
                    int max_fit = chars_per_line;
                    if (max_fit < 1) max_fit = 1;
                    len = (remaining < max_fit) ? remaining : max_fit;
                }
            } else {
                int max_chars = (tp->text_wrap_mode == TEXT_WRAP_PER_SET_CHARACTER_LEN) ? tp->wrap_index / font_w : chars_per_line;
                if (max_chars < 1) max_chars = 1;
                while (str[len] && str[len] != '\n' && len < max_chars) len++;
            }

            int line_w_px = len * font_w;
            int start_x;
            switch (tp->text_alignment) {
                case TEXT_CENTER_ALIGNED:
                    start_x = inner_pos_x + (textbox_w - line_w_px) / 2;
                    break;
                case TEXT_RIGHT_ALIGNED:
                    start_x = inner_pos_x + textbox_w - line_w_px - tp->margin_x;
                    break;
                default:
                    start_x = left_bound;
                    break;
            }

            int draw_x = start_x;
            int drawn = 0;
            for (int i = 0; i < len; i++) {
                if (draw_x + font_w > right_bound) break;
                uCanvas_Draw_SFONT(tp->text_draw_buf, Font, draw_x, base_y, str[i],
                                font_color,
                                background_color,
                                tp->Font_Draw_Direction);
                draw_x += font_w;
                drawn++;
            }

            caret_x = draw_x;
            caret_y = base_y;
            if (caret_x > right_bound) caret_x = right_bound;

            str += (drawn > 0) ? drawn : len;
            if (*str == '\n') str++;
            else if (*str == ' ' && tp->text_wrap_mode == TEXT_WRAP_PER_SET_WORD_LEN) str++;

            base_y += font_h;
            line_count++;
        }
        tp->last_crate_pos_x = caret_x+obj->properties.position.x;
        tp->last_crate_pos_y = caret_y+obj->properties.position.y;
        
    }

    uCanvas2D_DrawSprite(fb,obj->properties.position.x,obj->properties.position.y,tp->text_draw_buf->pixels,tp->text_draw_buf->width,tp->text_draw_buf->height,COLOR_RGB565);


    if (tp->cursor_properties.enable_cursor == true) {
        uint64_t now = esp_timer_get_time();
        if (now - tp->cursor_properties.last_tick > tp->cursor_properties.blink_interval) {
            tp->cursor_properties.last_tick = now;
            tp->cursor_properties.cursor_visible = !tp->cursor_properties.cursor_visible;
        }
        if (tp->cursor_properties.cursor_visible) {
            switch (tp->cursor_properties.cursor_type) {
                case CARET_CURSOR:
                    uCanvas2D_DrawLine(fb, tp->last_crate_pos_x, tp->last_crate_pos_y, tp->last_crate_pos_x, tp->last_crate_pos_y + font_h - 1, font_color, 1);
                    break;
                case BLOCK_CURSOR:
                    uCanvas2D_DrawRect(fb, tp->last_crate_pos_x, tp->last_crate_pos_y, font_w, font_h, font_color, true, 1);
                    break;
                default:
                    uCanvas2D_DrawLine(fb, tp->last_crate_pos_x, tp->last_crate_pos_y, tp->last_crate_pos_x, tp->last_crate_pos_y + font_h - 1,font_color, 1);
                    break;
            }
        }
        else {

            switch (tp->cursor_properties.cursor_type) {
                case CARET_CURSOR:
                    uCanvas2D_DrawLine(fb, tp->last_crate_pos_x, tp->last_crate_pos_y, tp->last_crate_pos_x, tp->last_crate_pos_y + font_h - 1,background_color, 1);
                    break;
                case BLOCK_CURSOR:
                    uCanvas2D_DrawRect(fb, tp->last_crate_pos_x, tp->last_crate_pos_y, font_w, font_h, background_color, true, 1);
                    break;
                default:
                    uCanvas2D_DrawLine(fb, tp->last_crate_pos_x, tp->last_crate_pos_y, tp->last_crate_pos_x, tp->last_crate_pos_y + font_h - 1, background_color, 1);
                    break;
            }

        }
    }
}
    
   

void IRAM_ATTR uCanvas_Draw_SFONT_TextBox(uCanvas2D_RenderBuffer_t *fb, uCanvas_universal_obj_t* obj) {
    uCanvas_TextBox_Properties_t* tp = obj->textbox_properties;
    char* str = tp->textbox_content;
    sFONT* Font = get_font_by_name(tp->font_type);
    if (!Font || !str) return;

    int font_h = Font->Height;
    int font_w = Font->Width;
    int textbox_w = tp->textbox_width;
    int textbox_h = tp->textbox_height;
    int base_y = obj->properties.position.y + tp->margin_y;
    int inner_px = textbox_w - 2 * tp->margin_x;
    if (inner_px < font_w) inner_px = font_w;
    int chars_per_line = inner_px / font_w;
    if (chars_per_line < 1) chars_per_line = 1;
    int words_per_line = tp->wrap_index;
    int max_lines = textbox_h / font_h;
    int line_count = 0;
    int caret_x = obj->properties.position.x + tp->margin_x;
    int caret_y = base_y;
    int left_bound = obj->properties.position.x + tp->margin_x;
    int right_bound = left_bound + inner_px;
    uint16_t font_color = convertToRGB565(obj->properties.color);
    uint16_t background_color = convertToRGB565(tp->background_color);
    uint16_t border_color = convertToRGB565(tp->border_color);

    if (tp->fill_background) {
        
        uCanvas2D_DrawRect(fb, obj->properties.position.x, obj->properties.position.y, tp->textbox_width, tp->textbox_height, background_color, 1, 1);
        uCanvas2D_DrawRect(fb, obj->properties.position.x, obj->properties.position.y, tp->textbox_width, tp->textbox_height, border_color, 0, 1);
    }

    while (*str && line_count < max_lines) {
        int len = 0;

        if (tp->text_wrap_mode == TEXT_WRAP_PER_SET_WORD_LEN) {
            int words = 0;
            int pixel_width = 0;
            int i = 0;
            while (str[i] && str[i] != '\n') {
                int word_len = 0;
                while (str[i + word_len] && str[i + word_len] != ' ' && str[i + word_len] != '\n') {
                    word_len++;
                }
                int word_px = word_len * font_w;
                int space_px = (words > 0) ? font_w : 0;
                if ((pixel_width + space_px + word_px) > inner_px || (words >= words_per_line)) break;
                pixel_width += space_px + word_px;
                i += word_len;
                if (str[i] == ' ') i++;
                words++;
            }
            len = i;
            if (len == 0 && str[0] && str[0] != '\n') {
                int remaining = 0;
                while (str[remaining] && str[remaining] != ' ' && str[remaining] != '\n') remaining++;
                int max_fit = chars_per_line;
                if (max_fit < 1) max_fit = 1;
                len = (remaining < max_fit) ? remaining : max_fit;
            }
        } else {
            int max_chars = (tp->text_wrap_mode == TEXT_WRAP_PER_SET_CHARACTER_LEN) ? tp->wrap_index / font_w : chars_per_line;
            if (max_chars < 1) max_chars = 1;
            while (str[len] && str[len] != '\n' && len < max_chars) len++;
        }

        int line_w_px = len * font_w;
        int start_x;
        switch (tp->text_alignment) {
            case TEXT_CENTER_ALIGNED:
                start_x = obj->properties.position.x + (textbox_w - line_w_px) / 2;
                break;
            case TEXT_RIGHT_ALIGNED:
                start_x = obj->properties.position.x + textbox_w - line_w_px - tp->margin_x;
                break;
            default:
                start_x = left_bound;
                break;
        }

        int draw_x = start_x;
        int drawn = 0;
        for (int i = 0; i < len; i++) {
            if (draw_x + font_w > right_bound) break;
            uCanvas_Draw_SFONT(fb, Font, draw_x, base_y, str[i],
                              font_color,
                               background_color,
                               tp->Font_Draw_Direction);
            draw_x += font_w;
            drawn++;
        }

        caret_x = draw_x;
        caret_y = base_y;
        if (caret_x > right_bound) caret_x = right_bound;

        str += (drawn > 0) ? drawn : len;
        if (*str == '\n') str++;
        else if (*str == ' ' && tp->text_wrap_mode == TEXT_WRAP_PER_SET_WORD_LEN) str++;

        base_y += font_h;
        line_count++;
    }

    if (tp->cursor_properties.enable_cursor == true) {
        uint64_t now = esp_timer_get_time();
        if (now - tp->cursor_properties.last_tick > tp->cursor_properties.blink_interval) {
            tp->cursor_properties.last_tick = now;
            tp->cursor_properties.cursor_visible = !tp->cursor_properties.cursor_visible;
        }
        if (tp->cursor_properties.cursor_visible) {
            switch (tp->cursor_properties.cursor_type) {
                case CARET_CURSOR:
                    uCanvas2D_DrawLine(fb, caret_x, caret_y, caret_x, caret_y + font_h - 1, font_color, 1);
                    break;
                case BLOCK_CURSOR:
                    uCanvas2D_DrawRect(fb, caret_x, caret_y, font_w, font_h, font_color, true, 1);
                    break;
                default:
                    uCanvas2D_DrawLine(fb, caret_x, caret_y, caret_x, caret_y + font_h - 1,font_color, 1);
                    break;
            }
        }
    }
}






int IRAM_ATTR uCanvas_Draw_Text(uCanvas2D_RenderBuffer_t *fb, int x, int y, char* text, FontType_t font_type, uint16_t color1, uint16_t color2, uint8_t font_direction,uint8_t ul_en){
    // if(font_type < SFONT_8)uCanvas_Draw_FONTX_Text(fb,x,y,text,font_type,color1,color2,font_direction,ul_en);
    if(font_type >= SFONT_8)uCanvas_Draw_SFONT_Text(fb,x,y,text,font_type,color1,color2,font_direction);

    return 0;
}

