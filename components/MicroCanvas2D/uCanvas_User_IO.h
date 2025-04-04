#include "driver/gpio.h"
#include "uCanvasDataTypes.h"

void uCanvas_Init_PushButton(gpio_num_t gpio_num);
uint8_t uCanvas_Get_PushbuttonState(gpio_num_t gpio_num);
uint8_t uCanvas_Get_PushbuttonState_WTR(gpio_num_t gpio_num);
void uCanvas_rotary_encoder_init(rotary_encoder_t* obj,gpio_num_t pinA, gpio_num_t pinB, gpio_num_t pinSW);
void uCanvas_rotary_encoder_read(rotary_encoder_t* obj);
encoder_state_t uCanvas_rotary_encoder_get_state(rotary_encoder_t* obj);