#include "uCanvas_Slide_Switch.h"

void uCanvasGUI_Create_SlideSwitch(uCanvas_Slide_Switch_t* SlideSwitch_Obj,uint16_t position_x, uint16_t position_y, uint16_t x_size, uint16_t y_size, uint16_t notch_margin){
    SlideSwitch_Obj->notch_margin = notch_margin; 
    
    SlideSwitch_Obj->layout [0] = New_uCanvas_2DRectangle(position_x,position_y,y_size,x_size);
    SlideSwitch_Obj->layout [3] = New_uCanvas_2DRectangle(position_x,position_y,y_size,x_size);
    SlideSwitch_Obj->layout [1] = New_uCanvas_2DRectangle(position_x+SlideSwitch_Obj->notch_margin ,position_y+SlideSwitch_Obj->notch_margin ,y_size-(2*SlideSwitch_Obj->notch_margin ),x_size/2);
    
    uCanvas_Set_Color(SlideSwitch_Obj->layout [0],100,100,100);
    uCanvas_Set_Color(SlideSwitch_Obj->layout [3],255,255,255);
    uCanvas_Set_Color(SlideSwitch_Obj->layout [1],0,255,0);

    SlideSwitch_Obj->layout [0]->properties.fill = NOFILL;
    SlideSwitch_Obj->layout [3]->properties.fill = NOFILL;
    SlideSwitch_Obj->layout [1]->properties.fill = FILL;

    SlideSwitch_Obj->x_size = x_size;
    SlideSwitch_Obj->y_size = y_size;
}

void uCanvasGUI_Set_SlideSwitch_State(uCanvas_Slide_Switch_t* SlideSwitch_Obj,switch_state_t state){
    int td = (SlideSwitch_Obj->x_size - (SlideSwitch_Obj->notch_margin  + SlideSwitch_Obj->x_size/2) - SlideSwitch_Obj->notch_margin);
    if(SlideSwitch_Obj->state != state){
        if(state == UCANVAS_SLIDE_SWITCH_STATE_ENABLED){
            printf("sw-on\r\n");
            for (int i = 0; i <=td; i++)
            {
                if(SlideSwitch_Obj->layout [1]->properties.position.x < SlideSwitch_Obj->layout [0]->properties.position.x + SlideSwitch_Obj->x_size)
                    SlideSwitch_Obj->layout [1]->properties.position.x++;
                uCanvas_Delay(2);
            }
        }
        if(state==UCANVAS_SLIDE_SWITCH_STATE_DISABLED){
            printf("sw-off\r\n");
            for (int i = 0; i <=td ; i++)
            {
                if(SlideSwitch_Obj->layout [1]->properties.position.x > SlideSwitch_Obj->layout [0]->properties.position.x)
                    SlideSwitch_Obj->layout [1]->properties.position.x--;
                uCanvas_Delay(2);
            }
        }
        SlideSwitch_Obj->state=state;
    }
}

switch_state_t uCanvasGUI_Get_SlideSwitch_State(uCanvas_Slide_Switch_t* SlideSwitch_Obj){
    return SlideSwitch_Obj->state;
}

void uCanvasGUI_Set_SlideSwitch_Notch_Color(uCanvas_Slide_Switch_t* SlideSwitch_Obj, uint16_t r, uint16_t g, uint16_t b, fill_t fill_type){
    SlideSwitch_Obj->layout [1]->properties.color.red   = r;
    SlideSwitch_Obj->layout [1]->properties.color.green = g;
    SlideSwitch_Obj->layout [1]->properties.color.blue  = b;
    SlideSwitch_Obj->layout [1]->properties.fill  = fill_type;
}

void uCanvasGUI_SlideSwitch_Set_Background_Color(uCanvas_Slide_Switch_t* SlideSwitch_Obj, uint16_t r, uint16_t g, uint16_t b, fill_t fill_type){
    SlideSwitch_Obj->layout [0]->properties.color.red   = r;
    SlideSwitch_Obj->layout [0]->properties.color.green = g;
    SlideSwitch_Obj->layout [0]->properties.color.blue  = b;
    SlideSwitch_Obj->layout [0]->properties.fill  = fill_type;
}

void uCanvasGUI_SlideSwitch_Set_Border_Color(uCanvas_Slide_Switch_t* SlideSwitch_Obj, uint16_t r, uint16_t g, uint16_t b){
    SlideSwitch_Obj->layout [3]->properties.color.red   = r;
    SlideSwitch_Obj->layout [3]->properties.color.green = g;
    SlideSwitch_Obj->layout [3]->properties.color.blue  = b;
}