#ifndef UCANVAS_PANEL_SELECTOR_H_ 
    #define UCANVAS_PANEL_SELECTOR_H_

#include "uCanvas2D_Display_Setup.h"
#include "uCanvas_api.h"

typedef enum {
    PANEL_DUMMY,
    PANEL_ST7789,
    PANEL_EK79007,
    PANEL_SSD1306_128_64
}uCanvas_Panel_Selection_t;

uCanvas2D_Display_Panel_t* uCanvas_Get_Panel_Handle(uCanvas_Panel_Selection_t Panel_Name);

#endif