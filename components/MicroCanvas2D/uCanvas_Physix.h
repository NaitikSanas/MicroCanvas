#include "uCanvas_api.h"
#include "uCanvasDataTypes.h"


uCanvas_Animation_task_handle_t uCanvas_attach_type1_controller_script(uCanvas_universal_obj_t*obj,controller_properties_t* properties);
void detach_type1_controller_script         (uCanvas_Animation_task_handle_t taskhandle);
void uCanvas_add_control_gpio               (controller_properties_t* player_obj,uint32_t gpio_num);
void uCanvas_set_gravity                    (controller_properties_t* player_obj,float gravity);
void uCanvas_set_jump_height                (controller_properties_t* player_obj,float jump_height);
void uCanvas_set_floor_level                (controller_properties_t* player_obj,float floor_leve);
