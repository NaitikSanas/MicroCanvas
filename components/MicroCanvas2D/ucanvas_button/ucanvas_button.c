#include "ucanvas_button.h"


void New_canvas_switch_instance(ucanvas_switch_t* btn_obj,uint16_t position_x, uint16_t position_y, uint16_t x_size, uint16_t y_size){
    btn_obj->layout [0] = New_uCanvas_2DRectangle(position_x,position_y,10,20);
    btn_obj->layout [1] = New_uCanvas_2DRectangle(position_x+2,position_y+2,10-4,20/4);
    
    btn_obj->layout [0]->properties.fill = NOFILL;
    btn_obj->layout [1]->properties.fill = FILL;
}

void ucanvas_switch_update_state(ucanvas_switch_t* btn_obj,switch_state_t state){
    if(btn_obj->state != state){
        if(state == SW_ON){
            printf("sw-on\r\n");
            for (int i = 0; i <= 10; i++)
            {
                btn_obj->layout [1]->properties.position.x++;
                uCanvas_Delay(1);
            }
        }
        if(state==SW_OFF){
            printf("sw-off\r\n");
            for (int i = 0; i <= 10; i++)
            {
                btn_obj->layout [1]->properties.position.x--;
                uCanvas_Delay(1);
            }
        }
        btn_obj->state=state;
    }
}

switch_state_t ucanvas_get_switch_state(ucanvas_switch_t* btn_obj){
    return btn_obj->state;
}
