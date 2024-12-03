#include "uCanvas_api.h"

typedef struct slider
{
    uCanvas_universal_obj_t* obj[5];
    uint32_t slider_gpio_1;
    uint32_t slider_gpio_2;
    uint16_t position_x;
    uint16_t position_y;
    uint16_t show_label;
    uint16_t slider_step; // Step size for slider

    int relative_label_pos_x;
    int relative_label_pos_y;

    uint16_t slider_notch_radius;
    uint16_t slider_length;
    uint16_t slider_thickness;
    float slider_value; // Current value
    float min_value;    // Minimum value
    float max_value;    // Maximum value
    FunctionPointer slider_event_handler;
} slider_t;

void create_slider(slider_t* slider_obj);
void slider_task(slider_t* slider);