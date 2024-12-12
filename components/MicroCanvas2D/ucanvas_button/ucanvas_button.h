#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uCanvas_api.h"
typedef enum {
    SW_OFF = 1,
    SW_ON = 2,
    BTN_IDLE = 3
}switch_state_t;

typedef struct ucanvas_button
{
    uCanvas_universal_obj_t* layout[2];
    uint16_t x_size;
    uint16_t y_size;
    uint16_t position_x;
    uint16_t position_y;
    switch_state_t state;
}ucanvas_switch_t;

void New_canvas_switch_instance(ucanvas_switch_t* btn_obj,uint16_t position_x, uint16_t position_y, uint16_t x_size, uint16_t y_size);
void ucanvas_switch_update_state(ucanvas_switch_t* btn_obj,switch_state_t state);
switch_state_t ucanvas_get_switch_state(ucanvas_switch_t* btn_obj);