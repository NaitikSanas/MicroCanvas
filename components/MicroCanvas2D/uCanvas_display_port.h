#include "stdio.h"
#include "uCanvasDataTypes.h"
// #define USE_SSD1306
#define USE_ST7789
typedef struct uCanvas_display_port
{
    uint16_t xPos;
    uint16_t yPos;

    uint16_t h1;
    uint16_t w1;

    uint16_t h2;
    uint16_t w2;

    uint16_t r1;
    uint16_t r2;

    uint16_t fill;
    uint16_t color;

}uCanvas_Disp_Ctx_t;

void uCanvas_Display_init               (void);
void uCanvas_Display_clear_buffer       (void);
void uCanvas_Update_Display             (void);

void uCanvas_Draw_Rectangle             (int x, int y, int h, int w, color_t color, fill_t fill);
void uCanvas_Draw_Circle                (int x, int y, int r, color_t color, fill_t fill);
void uCanvas_Draw_Line                  (Coordinate2D_t point1, Coordinate2D_t point2, color_t color);
void uCanvas_Draw_Text                  (char* text, int x, int y, color_t color);

void uCanvas_Draw_Triangle              (Coordinate2D_t point1, Coordinate2D_t point2, Coordinate2D_t point3, color_t color, fill_t fill);
void uCanvas_DrawPixel                  (Coordinate2D_t pos,color_t color);
void IRAM_ATTR uCanvas_DrawPixel565                  (Coordinate2D_t pos,uint16_t color);
void uCanvas_DrawPixel5652 (uint16_t x, uint16_t y,uint16_t color);
void st7789_draw_sprite_batch(uCanvas_universal_obj_t *obj);
uint16_t convertToRGB565(color_t color) ;