#include "uCanvas2D_SSD1306.h"

static void _SSD1306_init(uCanvas2D_Display_Orientation_t orientation){
    printf("--SSD1306 Initialization\r\n");
    SSD1306_Init();
    SSD1306_SetBrightness(0xff);
    
    char str[16];
    SSD1306_GotoXY(0,16);
    sprintf(str,"DRIVER TEST OK");
    SSD1306_Puts(str, &Font_7x10, 1);
    SSD1306_UpdateScreen();

    vTaskDelay(pdMS_TO_TICKS(1000));

    SSD1306_Clear();
    SSD1306_UpdateScreen();
}
/**
 * @brief  Extract 0-255 luminance from an RGB565 pixel (Rec.601 weights).
 */
static inline uint8_t rgb565_to_luminance(uint16_t px) {
    uint8_t r5 = (px >> 11) & 0x1F;
    uint8_t g6 = (px >> 5)  & 0x3F;
    uint8_t b5 =  px        & 0x1F;

    /* Expand to 8-bit (bit-replication, not just <<3) */
    uint8_t r8 = (r5 << 3) | (r5 >> 2);
    uint8_t g8 = (g6 << 2) | (g6 >> 4);
    uint8_t b8 = (b5 << 3) | (b5 >> 2);

    return (uint8_t)((r8 * 77 + g8 * 150 + b8 * 29) >> 8); /* 0.299/0.587/0.114 */
}
static inline uint8_t rgb565_to_intensity(uint16_t px) {
    uint8_t r5 = (px >> 11) & 0x1F;
    uint8_t g6 = (px >> 5)  & 0x3F;
    uint8_t b5 =  px        & 0x1F;

    uint8_t r8 = (r5 << 3) | (r5 >> 2);
    uint8_t g8 = (g6 << 2) | (g6 >> 4);
    uint8_t b8 = (b5 << 3) | (b5 >> 2);

    uint8_t maxc = r8 > g8 ? r8 : g8;
    maxc = maxc > b8 ? maxc : b8;
    return maxc; /* pure red/green/blue at full saturation -> 255 */
}

/**
 * @brief  Blit a uCanvas2D_RenderBuffer_t (RGB565) into the SSD1306 buffer,
 *         converting to monochrome via a fixed threshold.
 * @param  rb:        source render buffer
 * @param  dst_x/y:   top-left position on the OLED to draw at
 * @param  threshold: 0-255 luminance cutoff; >= threshold -> white pixel
 */
void SSD1306_DrawRenderBuffer(uCanvas2D_RenderBuffer_t* rb, int dst_x, int dst_y, uint8_t threshold) {
    if (!rb || !rb->pixels) return;

    for (int y = 0; y < rb->height; y++) {
        int sy = dst_y + y;
        if (sy < 0 || sy >= SSD1306_HEIGHT) continue;

        uint16_t* row = rb->pixels + (rb->offset_y + y) * rb->width + rb->offset_x;

        for (int x = 0; x < rb->width; x++) {
            int sx = dst_x + x;
            if (sx < 0 || sx >= SSD1306_WIDTH) continue;

            uint8_t val = rgb565_to_intensity(row[x]);
            SSD1306_DrawPixel(sx, sy, (val >= threshold) ? SSD1306_COLOR_WHITE : SSD1306_COLOR_BLACK);
        }
    }
}

static const uint8_t bayer4x4[4][4] = {
    {  0, 128,  32, 160 },
    { 192,  64, 224,  96 },
    {  48, 176,  16, 144 },
    { 240, 112, 208,  80 }
};

void SSD1306_DrawRenderBufferDithered(uCanvas2D_RenderBuffer_t* rb, int dst_x, int dst_y) {
    if (!rb || !rb->pixels) return;

    for (int y = 0; y < rb->height; y++) {
        int sy = dst_y + y;
        if (sy < 0 || sy >= SSD1306_HEIGHT) continue;

        uint16_t* row = rb->pixels + y * rb->width;

        for (int x = 0; x < rb->width; x++) {
            int sx = dst_x + x;
            if (sx < 0 || sx >= SSD1306_WIDTH) continue;

            uint8_t val = rgb565_to_intensity(row[x]);
            uint8_t map_val = bayer4x4[sy & 3][sx & 3];
            SSD1306_DrawPixel(sx, sy, (val > map_val) ? SSD1306_COLOR_WHITE : SSD1306_COLOR_BLACK);
        }
    }
}



void SSD1306_DrawRenderBufferDotMatrix(uCanvas2D_RenderBuffer_t* rb, int dst_x, int dst_y, uint8_t threshold, uint8_t dot_radius) {
    if (!rb || !rb->pixels) return;

    for (int y = 0; y < rb->height; y++) {
        int sy = dst_y + y;
        uint16_t* row = rb->pixels + y * rb->width;

        for (int x = 0; x < rb->width; x++) {
            int sx = dst_x + x;
            uint8_t val = rgb565_to_intensity(row[x]);
            if (val >= threshold) {
                if (dot_radius == 0) {
                    if (sx >= 0 && sx < SSD1306_WIDTH && sy >= 0 && sy < SSD1306_HEIGHT)
                        SSD1306_DrawPixel(sx, sy, SSD1306_COLOR_WHITE);
                } else {
                    SSD1306_DrawFilledCircle(sx, sy, dot_radius, SSD1306_COLOR_WHITE);
                }
            }
        }
    }
}

static void SSD1306_PushTile(int x, int y, uCanvas2D_RenderBuffer_t* buf){
    if (!buf || !buf->pixels) return;
    // printf("SSD1306_DrawRenderBuffer\r\n");

    SSD1306_DrawRenderBufferDithered(buf,x,y);
    SSD1306_UpdateScreen();
}

static void SSD1306_Adjust_Brightness(int value){
    printf("SSD1306_Adjust_Brightness prototype\r\n");
    SSD1306_SetBrightness(value);
}

uCanvas2D_Display_Panel_t* uCanvas2D_Get_Panel_Driver_SSD1306(void){
    uCanvas2D_Display_Panel_t* driver = (uCanvas2D_Display_Panel_t*) malloc(sizeof(uCanvas2D_Display_Panel_t));
    driver->height = SSD1306_HEIGHT;
    driver->width = SSD1306_WIDTH;
    driver->init = _SSD1306_init;
    driver->push_render_buffer = SSD1306_PushTile;
    driver->set_backlight = SSD1306_Adjust_Brightness;
    return driver;
}
