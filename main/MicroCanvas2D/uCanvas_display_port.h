#include "stdio.h"
#include "ssd1306.h"
#include "uCanvasDataTypes.h"

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
void uCanvas_Draw_Line                  (_2dPoint_t point1, _2dPoint_t point2, color_t color);
void uCanvas_Draw_Text                  (char* text, int x, int y, color_t color);


void uCanvas_Draw_Triangle(_2dPoint_t point1, _2dPoint_t point2, _2dPoint_t point3, color_t color);