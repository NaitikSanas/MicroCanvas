#include "uCanvas_User_IO.h"

void uCanvas_PB_Input_ISR(void* arg){

}

void uCanvas_Init_PushButton(gpio_num_t gpio_num){
    gpio_set_direction(gpio_num,GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio_num,GPIO_PULLUP_ONLY);  
    gpio_set_intr_type(gpio_num,GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, uCanvas_PB_Input_ISR, (void*) gpio_num);
}

uint8_t uCanvas_Get_PushbuttonState(gpio_num_t gpio_num){
    return gpio_get_level(gpio_num);
}