#include "uCanvas_api.h"

typedef struct slider
{
    uCanvas_universal_obj_t* obj[6];
    uint32_t slider_gpio_1;
    uint32_t slider_gpio_2;
    uint32_t slider_gpio_3;
    uint16_t position_x;
    uint16_t position_y;
    uint16_t show_label;
    uint16_t slider_step; // Step size for slider

    int relative_label_pos_x;
    int relative_label_pos_y;
    
    uint16_t slider_notch_radius;
    uint16_t slider_length;
    uint16_t slider_thickness;
    uint8_t wait_to_release;
    float slider_value; // Current value
    float aux_slider_value; // Current value
    float min_value;    // Minimum value
    float max_value;    // Maximum value
    FunctionPointer slider_event_handler;
    FunctionPointer slider_pb_event_handler;
    uint8_t update_delay;
    uint8_t is_active;
    char slider_name[32];
} slider_t;

void uCanvas_Create_Slider(slider_t* slider_obj);
void uCanvas_Set_Slider_Bar_Color(slider_t* slider_obj,uint16_t r, uint16_t g, uint16_t b);
void uCanvas_Set_Slider_Notch_Color(slider_t* slider_obj,uint16_t r_outer, uint16_t g_outer, uint16_t b_outer,uint16_t r_inner, uint16_t g_inner, uint16_t b_inner);
void slider_task(slider_t* slider);
void set_slider_visiblity(slider_t* slider_obj,visibility_ctrl_t v);
void uCanvas_Set_Slider_Value(slider_t* slider, float value);