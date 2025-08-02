#include "uCanvasDataTypes.h"

typedef struct {
    void (*load_fonts);
    void (*draw_text)(int x, int y, const char* text, void* font_data, void* target);
    void* font_data;
}uCanvas_Fonts_t;

typedef struct {
    int (*get_glyph_bitmap)(char c, int* w, int* h, const uint8_t** data, void* font_data);
    int (*get_advance)(char c, void* font_data);
    int (*get_offset)(char c, int* xoff, int* yoff, void* font_data);
} FontGlyphOps_t;

void draw_text()