#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
void create_slider(slider_t* slider_obj){
    slider_obj->obj[0] = New_uCanvas_2DRectangle(slider_obj->position_x,slider_obj->position_y,slider_obj->slider_thickness,slider_obj->slider_length);
    slider_obj->obj[1] = New_uCanvas_2DCircle(slider_obj->position_x+5, slider_obj->position_y+ (slider_obj->slider_thickness/2),slider_obj->slider_notch_radius);
    slider_obj->obj[2] = New_uCanvas_2DTextbox("",slider_obj->position_x + slider_obj->relative_label_pos_x,slider_obj->position_y + slider_obj->relative_label_pos_y);
    slider_obj->obj[1]->properties.fill = FILL;
    
}

void slider_task(slider_t* slider)
{
    char buf[32] = {0};
    float total_range = slider->max_value - slider->min_value; 
    float step_size_pixels = slider->slider_length / (total_range / slider->slider_step); 

    while (1)
    {
        if (!uCanvas_Get_PushbuttonState_WTR(slider->slider_gpio_1))
        {
            // Increment slider value
            if (slider->slider_value + slider->slider_step <= slider->max_value)
            {
                slider->slider_value += slider->slider_step;
                slider->obj[1]->properties.position.x += step_size_pixels;
            }
            // Update label text
            sprintf(buf, "Value %.2f", slider->slider_value);
            uCanvas_Set_Text(slider->obj[2], buf);

            // Call the slider's event handler if defined
            if (slider->slider_event_handler)
            {
                slider->slider_event_handler();
            }
        }
        else if (!uCanvas_Get_PushbuttonState_WTR(slider->slider_gpio_2))
        {
            // Decrement slider value
            if (slider->slider_value - slider->slider_step >= slider->min_value)
            {
                slider->slider_value -= slider->slider_step;
                slider->obj[1]->properties.position.x -= step_size_pixels;
            }
            // Update label text
            sprintf(buf, "Value %.2f", slider->slider_value);
            uCanvas_Set_Text(slider->obj[2], buf);

            // Call the slider's event handler if defined
            if (slider->slider_event_handler)
            {
                slider->slider_event_handler();
            }
        }
        uCanvas_Delay(1);
    }
}