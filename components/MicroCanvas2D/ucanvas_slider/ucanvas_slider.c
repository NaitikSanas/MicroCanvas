#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"

void uCanvas_Create_Slider(slider_t* slider_obj){
    // Create Slider Bar
    slider_obj->obj[0] = New_uCanvas_2DRectangle(slider_obj->position_x,slider_obj->position_y,slider_obj->slider_thickness,slider_obj->slider_length);
    slider_obj->obj[1] = New_uCanvas_2DCircle(slider_obj->position_x,slider_obj->position_y+(slider_obj->slider_thickness/2),slider_obj->slider_thickness/2);
    slider_obj->obj[2] = New_uCanvas_2DCircle(slider_obj->position_x+slider_obj->slider_length,slider_obj->position_y+(slider_obj->slider_thickness/2),slider_obj->slider_thickness/2);
    slider_obj->obj[0]->properties.fill = FILL;
    slider_obj->obj[1]->properties.fill = FILL;
    slider_obj->obj[2]->properties.fill = FILL;
    uCanvas_Set_Color(slider_obj->obj[0],40,0,0);
    uCanvas_Set_Color(slider_obj->obj[1],40,0,0);
    uCanvas_Set_Color(slider_obj->obj[2],40,0,0);
    
    //Create Notch
    slider_obj->obj[3] = New_uCanvas_2DCircle(slider_obj->position_x+5, slider_obj->position_y+ (slider_obj->slider_thickness/2),slider_obj->slider_notch_radius);
    slider_obj->obj[4] = New_uCanvas_2DCircle(slider_obj->position_x+5, slider_obj->position_y+ (slider_obj->slider_thickness/2),slider_obj->slider_notch_radius-2);
    slider_obj->obj[3]->properties.fill = FILL;
    slider_obj->obj[4]->properties.fill = FILL;
    uCanvas_Set_Color(slider_obj->obj[3],255,0,0);
    uCanvas_Set_Color(slider_obj->obj[4],140,0,0);

    //Create A Label
    slider_obj->obj[5] = New_uCanvas_2DTextbox("",slider_obj->position_x + slider_obj->relative_label_pos_x,slider_obj->position_y + slider_obj->relative_label_pos_y);
    uCanvas_Set_Color(slider_obj->obj[5],250,0,0);
    slider_obj->obj[5]->font_properties.font_type = FONT_16G;
    
    //Create Event monitor task
    uCanvas_Add_Task((uCanvas_Animation_task_t)slider_task,slider_obj,1);
}

void uCanvas_Set_Slider_Bar_Color(slider_t* slider_obj,uint16_t r, uint16_t g, uint16_t b){
    slider_obj->obj[0]->properties.fill = FILL;
    slider_obj->obj[1]->properties.fill = FILL;
    slider_obj->obj[2]->properties.fill = FILL;
    uCanvas_Set_Color(slider_obj->obj[0],r,g,b);
    uCanvas_Set_Color(slider_obj->obj[1],r,g,b);
    uCanvas_Set_Color(slider_obj->obj[2],r,g,b);
}

void uCanvas_Set_Slider_Notch_Color(slider_t* slider_obj,uint16_t r_outer, uint16_t g_outer, uint16_t b_outer,uint16_t r_inner, uint16_t g_inner, uint16_t b_inner){
    slider_obj->obj[3]->properties.fill = FILL;
    slider_obj->obj[4]->properties.fill = FILL;
    uCanvas_Set_Color(slider_obj->obj[3],r_outer,g_outer,b_outer);
    uCanvas_Set_Color(slider_obj->obj[4],r_inner,g_inner,b_inner);
}


void set_slider_visiblity(slider_t* slider_obj,visibility_ctrl_t v){
    slider_obj->obj[0]->properties.visiblity = v;
    slider_obj->obj[1]->properties.visiblity = v;
    slider_obj->obj[2]->properties.visiblity = v;
    slider_obj->obj[3]->properties.visiblity = v;
    slider_obj->obj[4]->properties.visiblity = v;
    slider_obj->obj[5]->properties.visiblity = v;
}

void slider_task(slider_t* slider)
{
    char buf[64] = {0};
    //float total_range = slider->max_value - slider->min_value; 
    //float step_size_pixels = slider->slider_length / (total_range / slider->slider_step); 
    printf("slider_task_started\r\n");
    while (1)
    {
        if (slider->is_active)
        {
            uint16_t input1;
            if(slider->wait_to_release)
                input1 = !uCanvas_Get_PushbuttonState_WTR(slider->slider_gpio_1);
            else 
                input1 = !uCanvas_Get_PushbuttonState(slider->slider_gpio_1);

            if (input1)
            {
                // Increment slider value
                if (slider->slider_value + slider->slider_step <= slider->max_value)
                {
                    slider->slider_value += slider->slider_step;

                    // Update notch position
                    int pos = slider->position_x + ((slider->slider_value - slider->min_value) / (slider->max_value - slider->min_value)) * slider->slider_length;
                    slider->obj[3]->properties.position.x = pos;
                    slider->obj[4]->properties.position.x = pos;
                        
                }
                // Update label text
                sprintf(buf, "%s %.2f", slider->slider_name, slider->slider_value);
                uCanvas_Set_Text(slider->obj[5], buf);

                // Call the slider's event handler if defined
                if (slider->slider_event_handler)
                {
                    slider->slider_event_handler();
                }
            }
            uint16_t input2;
            if(slider->wait_to_release)
                input2 = !uCanvas_Get_PushbuttonState_WTR(slider->slider_gpio_2);
            else 
                input2 = !uCanvas_Get_PushbuttonState(slider->slider_gpio_2);

            if (input2)
            {
                // Decrement slider value
                if (slider->slider_value - slider->slider_step >= slider->min_value)
                {
                    slider->slider_value -= slider->slider_step;

                    // Update notch position
                    int pos = slider->position_x + ((slider->slider_value - slider->min_value) / (slider->max_value - slider->min_value)) * slider->slider_length;
                    slider->obj[3]->properties.position.x = pos;
                    slider->obj[4]->properties.position.x = pos;
                        
                }
                // Update label text
                sprintf(buf, "%s %.2f", slider->slider_name, slider->slider_value);
                uCanvas_Set_Text(slider->obj[5], buf);

                // Call the slider's event handler if defined
                if (slider->slider_event_handler)
                {
                    slider->slider_event_handler();
                }
            }
            else if (!uCanvas_Get_PushbuttonState_WTR(slider->slider_gpio_3))
            {
                if (slider->slider_pb_event_handler)
                {
                    slider->slider_pb_event_handler();
                }
            }
        }
        uCanvas_Delay(slider->update_delay);

    }
}

void uCanvas_Set_Slider_Value(slider_t* slider, float value)
{
    // Check if the value is within the slider's range
    if (value < slider->min_value || value > slider->max_value)
    {
        printf("Error: Value %.2f is out of range (%.2f - %.2f).\n", value, slider->min_value, slider->max_value);
        return;
    }

    // Update the slider's value
    slider->slider_value = value;

    // Calculate the new notch position
    int pos =  slider->position_x + ((slider->slider_value - slider->min_value) / (slider->max_value - slider->min_value)) * slider->slider_length;
    slider->obj[3]->properties.position.x = pos;       
    slider->obj[4]->properties.position.x = pos;
    // Update the label text
    char buf[64] = {0};
    sprintf(buf, "%s %.2f", slider->slider_name, slider->slider_value);
    uCanvas_Set_Text(slider->obj[5], buf);

    // Optionally call the slider event handler
    if (slider->slider_event_handler)
    {
        slider->slider_event_handler();
    }
}