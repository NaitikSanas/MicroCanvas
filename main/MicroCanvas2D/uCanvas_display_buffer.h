#include "stdio.h"
#include "string.h"
#include "uCanvasDataTypes.h"
#define LCD_HEIGHT 100
#define LCD_WIDTH 100
static const int SPI_Data_Mode = 1;
typedef struct {
	uint16_t _model;
	uint16_t _width;
	uint16_t _height;
	uint16_t _offsetx;
	uint16_t _offsety;
	uint16_t _font_direction;
	uint16_t _font_fill;
	uint16_t _font_fill_color;
	uint16_t _font_underline;
	uint16_t _font_underline_color;
	int16_t _dc;
	int16_t _bl;
	int16_t _irq;
	spi_device_handle_t _TFT_Handle;
	bool _calibration;
	int16_t _min_xp; // Minimum xp calibration
	int16_t _min_yp; // Minimum yp calibration
	int16_t _max_xp; // Maximum xp calibration
	int16_t _max_yp; // Maximum yp calibration
	int16_t _min_xc; // Minimum x coordinate
	int16_t _min_yc; // Minimum y coordinate
	int16_t _max_xc; // Maximum x coordinate
	int16_t _max_yc; // Maximum y coordinate
} TFT_t;

void lcd_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, color_t c);
void lcd_Draw_pixel(uint16_t x, uint16_t y, color_t color);
