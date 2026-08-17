#include "uCanvas2D_Filters.h"
void uCanvas_DitherBuffer_RGB565(uCanvas2D_RenderBuffer_t* rb) {
    static const uint8_t bayer4x4[4][4] = {
        {  0, 128,  32, 160 },
        { 192,  64, 224,  96 },
        {  48, 176,  16, 144 },
        { 240, 112, 208,  80 }
    };

    if (!rb || !rb->pixels) return;

    for (int y = 0; y < rb->height; y++) {
        uint16_t* row = rb->pixels + (rb->offset_y + y) * rb->width + rb->offset_x;
        const uint8_t* map_row = bayer4x4[y & 3];

        for (int x = 0; x < rb->width; x++) {
            uint16_t px = row[x];

            uint8_t r5 = (px >> 11) & 0x1F;
            uint8_t g6 = (px >> 5)  & 0x3F;
            uint8_t b5 =  px        & 0x1F;

            uint8_t r8 = (r5 << 3) | (r5 >> 2);
            uint8_t g8 = (g6 << 2) | (g6 >> 4);
            uint8_t b8 = (b5 << 3) | (b5 >> 2);

            /* Perceptual luminance (Rec.601 weights) — works uniformly
               across hues, not just saturated colors */
            uint8_t lum = (uint8_t)((r8 * 77 + g8 * 150 + b8 * 29) >> 8);

            uint8_t map_val = map_row[x & 3];
            row[x] = (lum > map_val) ? 0xFFFF : 0x0000;
        }
    }
}