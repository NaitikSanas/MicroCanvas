#include "driver/gpio.h"
#include "uCanvasDataTypes.h"
#include "mpu6050_read.h"

/*
    APIs to Get GPIO Input
*/
void uCanvas_Init_PushButton(gpio_num_t gpio_num);
uint8_t uCanvas_Get_PushbuttonState(gpio_num_t gpio_num);
uint8_t uCanvas_Get_PushbuttonState_WTR(gpio_num_t gpio_num);

/*
    APIs To Get Rotary Encoder Input
*/

void uCanvas_rotary_encoder_init(rotary_encoder_t* obj,gpio_num_t pinA, gpio_num_t pinB, gpio_num_t pinSW);
void uCanvas_rotary_encoder_read(rotary_encoder_t* obj);
encoder_state_t uCanvas_rotary_encoder_get_state(rotary_encoder_t* obj);
/*
    APIs To Get IMU Input
*/
void uCanvas_Initialize_IMU_Device(uint32_t sda, uint32_t scl);
tilt_dir_t uCanvas_Get_IMU_2D_Tilt(void);
tilt_angle_t uCanvas_IMU_Get_Tilt_Angles(void);
void uCanvas_IMU_Set_Tilt_Detection_Parameters(float tilt_trigger, float tilt_return_zone);
tilt_dir_t uCanvas_detect_2D_Tile_With_Reference_Axis(reference_axis_t reference_axis);

