#ifndef __MPU_6050_HAL__
#define __MPU_6050_HAL__
#include "mpu60x0.h"
#include "mpu60x0_types.h"

typedef struct {
    float pitch; // x-axis tilt
    float roll;  // y-axis tilt
} tilt_angle_t;

typedef enum {
    NO_TILT,
    TILT_LEFT,
    TILT_RIGHT,
    TILT_UP,
    TILT_DOWN,
    TILT_UP_LEFT,
    TILT_UP_RIGHT,
    TILT_DOWN_LEFT,
    TILT_DOWN_RIGHT
} tilt_dir_t;

typedef enum{
    USE_PITCH = 0,
    USE_ROLL
} reference_axis_t;

void mpu6050_read(float_axes_t* data);
void mpu6050_calibrate_error(void);
int mpu_6050_init(uint32_t sda, uint32_t scl);
void mpu_set_tilt_detection_parameters(float tilt_trigger, float tilt_return_zone);
tilt_angle_t get_tilt_angles_from_accel();
tilt_angle_t get_tilt_angles_complementary();
tilt_dir_t detect_2d_tilt(float pitch_deg, float roll_deg);
tilt_dir_t detect_8way_tilt(float pitch_deg, float roll_deg);
tilt_dir_t detect_2d_tilt_UD(float refrence_axis);
tilt_dir_t detect_2d_tilt_LR(float refrence_axis);
#endif