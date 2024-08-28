// #include "stdint.h"

// /* Absolute value */
// #define ABS(x)   ((x) > 0 ? (x) : -(x))

// /* uCanvas_Draw_obj data buffer */
// static uint8_t uCanvas_Draw_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// /* Private uCanvas_Draw_obj structure */
// typedef struct {
// 	uint16_t CurrentX;
// 	uint16_t CurrentY;
// 	uint8_t Inverted;
// 	uint8_t Initialized;
// } uCanvas_draw_obj_t;

// /* Private variable */
// static uCanvas_draw_obj_t uCanvas_Draw_obj;




// void SSD1306_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color)
// {

//     int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
//     uint8_t byte = 0;

//     for(int16_t j=0; j<h; j++, y++)
//     {
//         for(int16_t i=0; i<w; i++)
//         {
//             if(i & 7)
//             {
//                byte <<= 1;
//             }
//             else
//             {
//                byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
//             }
// 			uint8_t tmp = byte&0x80;
//             if( tmp == 1) uCanvas_draw_pixel(x+i, y, (SSD1306_COLOR_t)color);
//         }
//     }
// }


// void SSD1306_ToggleInvert(void) {
// 	uint16_t i;
	
// 	/* Toggle invert */
// 	uCanvas_Draw_obj.Inverted = !uCanvas_Draw_obj.Inverted;
	
// 	/* Do memory toggle */
// 	for (i = 0; i < sizeof(uCanvas_Draw_Buffer); i++) {
// 		uCanvas_Draw_Buffer[i] = ~uCanvas_Draw_Buffer[i];
// 	}
// }

// void uCanvas_draw_fill(SSD1306_COLOR_t color) {
// 	/* Set memory */
// 	memset(uCanvas_Draw_Buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(uCanvas_Draw_Buffer));
// }

// void uCanvas_draw_pixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color) {
// 	if (
// 		x >= SSD1306_WIDTH ||
// 		y >= SSD1306_HEIGHT
// 	) {
// 		/* Error */
// 		return;
// 	}
	
// 	/* Check if pixels are inverted */
// 	if (uCanvas_Draw_obj.Inverted) {
// 		color = (SSD1306_COLOR_t)!color;
// 	}
	
// 	/* Set color */
// 	if (color == SSD1306_COLOR_WHITE) {
// 		uCanvas_Draw_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
// 	} else {
// 		uCanvas_Draw_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
// 	}
// }

// void SSD1306_GotoXY(uint16_t x, uint16_t y) {
// 	/* Set write pointers */
// 	uCanvas_Draw_obj.CurrentX = x;
// 	uCanvas_Draw_obj.CurrentY = y;
// }

// char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color) {
// 	uint32_t i, b, j;
	
// 	/* Check available space in LCD */
// 	if (
// 		SSD1306_WIDTH <= (uCanvas_Draw_obj.CurrentX + Font->FontWidth) ||
// 		SSD1306_HEIGHT <= (uCanvas_Draw_obj.CurrentY + Font->FontHeight)
// 	) {
// 		/* Error */
// 		return 0;
// 	}
	
// 	/* Go through font */
// 	for (i = 0; i < Font->FontHeight; i++) {
// 		b = Font->data[(ch - 32) * Font->FontHeight + i];
// 		for (j = 0; j < Font->FontWidth; j++) {
// 			if ((b << j) & 0x8000) {
// 				uCanvas_draw_pixel(uCanvas_Draw_obj.CurrentX + j, (uCanvas_Draw_obj.CurrentY + i), (SSD1306_COLOR_t) color);
// 			} else {
// 				uCanvas_draw_pixel(uCanvas_Draw_obj.CurrentX + j, (uCanvas_Draw_obj.CurrentY + i), (SSD1306_COLOR_t)!color);
// 			}
// 		}
// 	}
	
// 	/* Increase pointer */
// 	uCanvas_Draw_obj.CurrentX += Font->FontWidth;
	
// 	/* Return character written */
// 	return ch;
// }

// char SSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color) {
// 	/* Write characters */
// 	while (*str) {
// 		/* Write character by character */
// 		if (SSD1306_Putc(*str, Font, color) != *str) {
// 			/* Return error */
// 			return *str;
// 		}
		
// 		/* Increase string pointer */
// 		str++;
// 	}
	
// 	/* Everything OK, zero should be returned */
// 	return *str;
// }
 

// void uCanvas_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c) {
// 	int16_t dx, dy, sx, sy, err, e2, i, tmp; 
	
// 	/* Check for overflow */
// 	if (x0 >= SSD1306_WIDTH) {
// 		x0 = SSD1306_WIDTH - 1;
// 	}
// 	if (x1 >= SSD1306_WIDTH) {
// 		x1 = SSD1306_WIDTH - 1;
// 	}
// 	if (y0 >= SSD1306_HEIGHT) {
// 		y0 = SSD1306_HEIGHT - 1;
// 	}
// 	if (y1 >= SSD1306_HEIGHT) {
// 		y1 = SSD1306_HEIGHT - 1;
// 	}
	
// 	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
// 	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
// 	sx = (x0 < x1) ? 1 : -1; 
// 	sy = (y0 < y1) ? 1 : -1; 
// 	err = ((dx > dy) ? dx : -dy) / 2; 

// 	if (dx == 0) {
// 		if (y1 < y0) {
// 			tmp = y1;
// 			y1 = y0;
// 			y0 = tmp;
// 		}
		
// 		if (x1 < x0) {
// 			tmp = x1;
// 			x1 = x0;
// 			x0 = tmp;
// 		}
		
// 		/* Vertical line */
// 		for (i = y0; i <= y1; i++) {
// 			uCanvas_draw_pixel(x0, i, c);
// 		}
		
// 		/* Return from function */
// 		return;
// 	}
	
// 	if (dy == 0) {
// 		if (y1 < y0) {
// 			tmp = y1;
// 			y1 = y0;
// 			y0 = tmp;
// 		}
		
// 		if (x1 < x0) {
// 			tmp = x1;
// 			x1 = x0;
// 			x0 = tmp;
// 		}
		
// 		/* Horizontal line */
// 		for (i = x0; i <= x1; i++) {
// 			uCanvas_draw_pixel(i, y0, c);
// 		}
		
// 		/* Return from function */
// 		return;
// 	}
	
// 	while (1) {
// 		uCanvas_draw_pixel(x0, y0, c);
// 		if (x0 == x1 && y0 == y1) {
// 			break;
// 		}
// 		e2 = err; 
// 		if (e2 > -dx) {
// 			err -= dy;
// 			x0 += sx;
// 		} 
// 		if (e2 < dy) {
// 			err += dx;
// 			y0 += sy;
// 		} 
// 	}
// }

// void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
// 	/* Check input parameters */
// 	if (
// 		x >= SSD1306_WIDTH ||
// 		y >= SSD1306_HEIGHT
// 	) {
// 		/* Return error */
// 		return;
// 	}
	
// 	/* Check width and height */
// 	if ((x + w) >= SSD1306_WIDTH) {
// 		w = SSD1306_WIDTH - x;
// 	}
// 	if ((y + h) >= SSD1306_HEIGHT) {
// 		h = SSD1306_HEIGHT - y;
// 	}
	
// 	/* Draw 4 lines */
// 	uCanvas_draw_line(x, y, x + w, y, c);         /* Top line */
// 	uCanvas_draw_line(x, y+1, x + w, y, c);         /* Top line */
// 	uCanvas_draw_line(x, y + h, x + w, y + h, c); /* Bottom line */
// 	uCanvas_draw_line(x, y, x, y + h, c);         /* Left line */
// 	uCanvas_draw_line(x + w, y, x + w, y + h, c); /* Right line */
// }

// void SSD1306_DrawRectangle2(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
// 	/* Check input parameters */
// 	if (
// 		x >= SSD1306_WIDTH ||
// 		y >= SSD1306_HEIGHT
// 	) {
// 		/* Return error */
// 		return;
// 	}
	
// 	/* Check width and height */
// 	if ((x + w) >= SSD1306_WIDTH) {
// 		w = SSD1306_WIDTH - x;
// 	}
// 	if ((y + h) >= SSD1306_HEIGHT) {
// 		h = SSD1306_HEIGHT - y;
// 	}
	
// 	/* Draw 4 lines */
// 	uCanvas_draw_line(x, y, x + w, y, c);         /* Top line */
// 	uCanvas_draw_line(x, y + h, x + w, y + h, c); /* Bottom line */
// 	uCanvas_draw_line(x, y, x, y + h, c);         /* Left line */
// 	uCanvas_draw_line(x + w, y, x + w, y + h, c); /* Right line */
// }


// void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c) {
// 	uint8_t i;
	
// 	/* Check input parameters */
// 	if (
// 		x >= SSD1306_WIDTH ||
// 		y >= SSD1306_HEIGHT
// 	) {
// 		/* Return error */
// 		return;
// 	}
	
// 	/* Check width and height */
// 	if ((x + w) >= SSD1306_WIDTH) {
// 		w = SSD1306_WIDTH - x;
// 	}
// 	if ((y + h) >= SSD1306_HEIGHT) {
// 		h = SSD1306_HEIGHT - y;
// 	}
	
// 	/* Draw lines */
// 	for (i = 0; i <= h; i++) {
// 		/* Draw lines */
// 		uCanvas_draw_line(x, y + i, x + w, y + i, c);
// 	}
// }

// void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
// 	/* Draw lines */
// 	uCanvas_draw_line(x1, y1, x2, y2, color);
// 	uCanvas_draw_line(x2, y2, x3, y3, color);
// 	uCanvas_draw_line(x3, y3, x1, y1, color);
// }


// void SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
// 	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
// 	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
// 	curpixel = 0;
	
// 	deltax = ABS(x2 - x1);
// 	deltay = ABS(y2 - y1);
// 	x = x1;
// 	y = y1;

// 	if (x2 >= x1) {
// 		xinc1 = 1;
// 		xinc2 = 1;
// 	} else {
// 		xinc1 = -1;
// 		xinc2 = -1;
// 	}

// 	if (y2 >= y1) {
// 		yinc1 = 1;
// 		yinc2 = 1;
// 	} else {
// 		yinc1 = -1;
// 		yinc2 = -1;
// 	}

// 	if (deltax >= deltay){
// 		xinc1 = 0;
// 		yinc2 = 0;
// 		den = deltax;
// 		num = deltax / 2;
// 		numadd = deltay;
// 		numpixels = deltax;
// 	} else {
// 		xinc2 = 0;
// 		yinc1 = 0;
// 		den = deltay;
// 		num = deltay / 2;
// 		numadd = deltax;
// 		numpixels = deltay;
// 	}

// 	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
// 		uCanvas_draw_line(x, y, x3, y3, color);

// 		num += numadd;
// 		if (num >= den) {
// 			num -= den;
// 			x += xinc1;
// 			y += yinc1;
// 		}
// 		x += xinc2;
// 		y += yinc2;
// 	}
// }

// void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
// 	int16_t f = 1 - r;
// 	int16_t ddF_x = 1;
// 	int16_t ddF_y = -2 * r;
// 	int16_t x = 0;
// 	int16_t y = r;

//     uCanvas_draw_pixel(x0, y0 + r, c);
//     uCanvas_draw_pixel(x0, y0 - r, c);
//     uCanvas_draw_pixel(x0 + r, y0, c);
//     uCanvas_draw_pixel(x0 - r, y0, c);

//     while (x < y) {
//         if (f >= 0) {
//             y--;
//             ddF_y += 2;
//             f += ddF_y;
//         }
//         x++;
//         ddF_x += 2;
//         f += ddF_x;

//         uCanvas_draw_pixel(x0 + x, y0 + y, c);
//         uCanvas_draw_pixel(x0 - x, y0 + y, c);
//         uCanvas_draw_pixel(x0 + x, y0 - y, c);
//         uCanvas_draw_pixel(x0 - x, y0 - y, c);

//         uCanvas_draw_pixel(x0 + y, y0 + x, c);
//         uCanvas_draw_pixel(x0 - y, y0 + x, c);
//         uCanvas_draw_pixel(x0 + y, y0 - x, c);
//         uCanvas_draw_pixel(x0 - y, y0 - x, c);
//     }
// }

// void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
// 	int16_t f = 1 - r;
// 	int16_t ddF_x = 1;
// 	int16_t ddF_y = -2 * r;
// 	int16_t x = 0;
// 	int16_t y = r;

//     uCanvas_draw_pixel(x0, y0 + r, c);
//     uCanvas_draw_pixel(x0, y0 - r, c);
//     uCanvas_draw_pixel(x0 + r, y0, c);
//     uCanvas_draw_pixel(x0 - r, y0, c);
//     uCanvas_draw_line(x0 - r, y0, x0 + r, y0, c);

//     while (x < y) {
//         if (f >= 0) {
//             y--;
//             ddF_y += 2;
//             f += ddF_y;
//         }
//         x++;
//         ddF_x += 2;
//         f += ddF_x;

//         uCanvas_draw_line(x0 - x, y0 + y, x0 + x, y0 + y, c);
//         uCanvas_draw_line(x0 + x, y0 - y, x0 - x, y0 - y, c);

//         uCanvas_draw_line(x0 + y, y0 + x, x0 - y, y0 + x, c);
//         uCanvas_draw_line(x0 + y, y0 - x, x0 - y, y0 - x, c);
//     }
// }
 


// void SSD1306_Clear (void)
// {
// 	uCanvas_draw_fill ((SSD1306_COLOR_t)0);
//     //SSD1306_UpdateScreen();
// }