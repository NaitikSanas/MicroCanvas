#include "app.h"
#include "ucanvas_slider.h"

uCanvas_Scene_t* scene_1; //Create A Scene Object to hold 2D Elemenets
uCanvas_Scene_t* scene_2; //Create A Scene Object to hold 2D Elemenets

slider_t slider_1;

void slider_eventhandler(void){
    printf("[Event] : slider_1 %f\r\n",slider_1.slider_value);
}
void uCanvas_Setup(void)
{
    start_uCanvas_engine();
    scene_1 = New_uCanvas_Scene();
    scene_2 = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene_1);
    uCanvas_Init_PushButton(48);
    uCanvas_Init_PushButton(47);
    slider_1.slider_gpio_1 = 47;
    slider_1.slider_gpio_2 = 48;
    slider_1.position_x = 20;
    slider_1.position_y = 25;
    slider_1.slider_length = 100; // Length of slider in pixels
    slider_1.slider_notch_radius = 5;
    slider_1.slider_thickness = 5;
    slider_1.relative_label_pos_x = 10;
    slider_1.relative_label_pos_y = -15;
    slider_1.min_value = 0.0;    // Minimum slider value
    slider_1.max_value = 20.0; // Maximum slider value
    slider_1.slider_step = 1.0; // Step size for each input
    slider_1.slider_event_handler = slider_eventhandler;

    create_slider(&slider_1);
    slider_task(&slider_1);
}
void uCanvas_App_Main(void){
    // if(rectangle->properties.position.x < 128)
    //     rectangle->properties.position.x++;
    // else{ 
    //     rectangle->properties.position.x = 0;
    // }
}