#include "esp_err.h"
#include "esp_log.h"
#include "uCanvas2D_Display_Setup.h"
#include "ssd1306.h"

#define USE_DITHERED_FB 1
#define RGB565_INT_THRESHOLD 128

void SSD1306_DrawRenderBufferDithered(uCanvas2D_RenderBuffer_t* rb, int dst_x, int dst_y);
void SSD1306_DrawRenderBuffer(uCanvas2D_RenderBuffer_t* rb, int dst_x, int dst_y, uint8_t threshold);
