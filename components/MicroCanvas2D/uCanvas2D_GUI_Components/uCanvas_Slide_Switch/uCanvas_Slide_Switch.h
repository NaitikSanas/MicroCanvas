#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uCanvas_api.h"
typedef enum {
    UCANVAS_SLIDE_SWITCH_STATE_DISABLED = 1,
    UCANVAS_SLIDE_SWITCH_STATE_ENABLED = 2,
    UCANVAS_SLIDE_SWITCH_STATE_IDLE = 3
}switch_state_t;

typedef struct uCanvas_Slide_Switch
{
    uCanvas_universal_obj_t* layout[3];
    uint16_t x_size;
    uint16_t y_size;
    uint16_t position_x;
    uint16_t position_y;
    uint16_t notch_margin;
    uint16_t notch_size;
    switch_state_t state;
    uint8_t fill_insides;
    uint8_t fill_notch; 
}uCanvas_Slide_Switch_t;

void uCanvasGUI_Create_SlideSwitch(uCanvas_Slide_Switch_t* SlideSwitch_Obj,uint16_t position_x, uint16_t position_y, uint16_t x_size, uint16_t y_size, uint16_t notch_margin);
void uCanvasGUI_Set_SlideSwitch_State(uCanvas_Slide_Switch_t* SlideSwitch_Obj,switch_state_t state);
switch_state_t uCanvasGUI_Get_SlideSwitch_State(uCanvas_Slide_Switch_t* SlideSwitch_Obj);
void uCanvasGUI_Set_SlideSwitch_Notch_Color(uCanvas_Slide_Switch_t* SlideSwitch_Obj, uint16_t r, uint16_t g, uint16_t b, fill_t fill_type);
void uCanvasGUI_SlideSwitch_Set_Background_Color(uCanvas_Slide_Switch_t* SlideSwitch_Obj, uint16_t r, uint16_t g, uint16_t b, fill_t fill_type);
void uCanvasGUI_SlideSwitch_Set_Border_Color(uCanvas_Slide_Switch_t* SlideSwitch_Obj, uint16_t r, uint16_t g, uint16_t b);