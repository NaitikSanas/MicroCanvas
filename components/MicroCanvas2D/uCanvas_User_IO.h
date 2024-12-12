#include "driver/gpio.h"

void uCanvas_Init_PushButton(gpio_num_t gpio_num);
uint8_t uCanvas_Get_PushbuttonState(gpio_num_t gpio_num);
uint8_t uCanvas_Get_PushbuttonState_WTR(gpio_num_t gpio_num);