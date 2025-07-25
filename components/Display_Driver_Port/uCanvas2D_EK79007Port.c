#include "uCanvas2D_EK79007Port.h"


void EK79007_Init(uCanvas2D_Display_Orientation_t orientation){
    printf("EK79007_Init prototype\r\n");
    ek79007_init();
}

void EK79007_PushTile(int x, int y, uCanvas2D_RenderBuffer_t* buf){
    if (!buf || !buf->pixels) {
        printf("NULL PIX BUF\r\n");
        return;
    }
    const uint16_t* pixels = (const uint16_t*) buf->pixels;
    int w = buf->width;
    int h = buf->height;
    // printf("dim(%d,%d)\r\n",w,h);
    ek79007_draw_tile(x,y,w,h,pixels);
}

void EK79007_Adjust_Brightness(int percent){
    printf("EK79007_Adjust_Brightness prototype\r\n");
}

uCanvas2D_Display_Panel_t* uCanvas2D_Get_Panel_Driver_EK79007(void){
    uCanvas2D_Display_Panel_t* driver = (uCanvas2D_Display_Panel_t*) malloc(sizeof(uCanvas2D_Display_Panel_t));
    driver->init = EK79007_Init;
    driver->push_render_buffer = EK79007_PushTile;
    driver->set_backlight = EK79007_Adjust_Brightness;
    driver->height = LCD_V_RES;
    driver->width = LCD_H_RES;
    return driver;
}
