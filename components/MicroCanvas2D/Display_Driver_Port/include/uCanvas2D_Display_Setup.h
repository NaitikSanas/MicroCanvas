#pragma once
#include "stdio.h"
#include "stdint.h"
#include "driver/ppa.h"
typedef enum {
    OR_LANDSCAPE,
    OR_LANDSCAPE_FLIPPED,
    OR_PORTRAIT,
    OR_PORTRAIT_FLIPPED,
}uCanvas2D_Display_Orientation_t;
typedef struct {  
    int width;
    int height;
    int pitch;
    int offset_x;
    int offset_y;
    uint16_t* pixels;
    int use_ppa;

} uCanvas2D_RenderBuffer_t;
typedef struct {
    void (*init)(uCanvas2D_Display_Orientation_t orientation);
    void (*push_render_buffer)(int x, int y, uCanvas2D_RenderBuffer_t* buf);
    void (*set_backlight)(int percent);
    uCanvas2D_Display_Orientation_t orientation;
    uint16_t width;
    uint16_t height; 
} uCanvas2D_Display_Panel_t;
uCanvas2D_Display_Panel_t* uCanvas2D_Get_Panel_Driver_EK79007(void);

uCanvas2D_Display_Panel_t* uCanvas2D_Get_Panel_Driver_ST7789(void);
void uCanvas2D_GetPanel_Driver_ST7789(uCanvas2D_Display_Panel_t* driver);