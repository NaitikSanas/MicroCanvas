#include "mpu6050_read.h"
#include "esp_log.h"
#include "esp_timer.h"
mpu_handle_t mpu;
float TILT_TRIGGER     = 20; // deg to trigger gesture
float TILT_RETURN_ZONE = 10;  // deg to reset gesture 

typedef struct {
    float ax, ay, az;
    float gx, gy, gz;
} mpu_offset_t;
mpu_offset_t offset = {0};

void mpu_set_tilt_detection_parameters(float tilt_trigger, float tilt_return_zone){
    TILT_TRIGGER     = tilt_trigger;
    TILT_RETURN_ZONE = tilt_return_zone;
}

void mpu_calibrate(int samples)
{   
    raw_axes_t accel_raw, gyro_raw;
    printf("calibrating\r\n");
    for (int i = 0; i < samples; i++) {
        mpu_motion(&mpu, &accel_raw, &gyro_raw);
        offset.ax += accel_raw.x;
        offset.ay += accel_raw.y;
        offset.az += accel_raw.z;
        offset.gx += gyro_raw.x;
        offset.gy += gyro_raw.y;
        offset.gz += gyro_raw.z;
    }

    offset.ax /= samples;
    offset.ay /= samples;
    offset.az /= samples;
    offset.gx /= samples;
    offset.gy /= samples;
    offset.gz /= samples;

    // Optional: compensate for gravity along Z
    offset.az -= 16384;  // assuming +/-2g and 16-bit output
}

int mpu_6050_init(uint32_t sda, uint32_t scl)
{
    esp_err_t err;
    mpu.addr = 104;
    mpu.bus.num = 0;
    mpu.bus.timeout = 50;
    mpu.init.clk_speed = 400000;
    mpu.init.sda_io_num = sda;
    mpu.init.scl_io_num = scl;
    mpu.init.sda_pullup_en = false;
    mpu.init.scl_pullup_en = false;
    init : err = mpu_initialize_peripheral(&mpu);
    printf("Returned with %x", (int)err);
    if(err != ESP_OK)goto init;
    ESP_LOGI("TAG", "MPU connection successful!");

    err = mpu_initialize_chip(&mpu);
    ESP_LOGI("TAG", "i2c_slave_init");
    mpu_calibrate(100);
    return 0;
}

void mpu6050_calibrate_error(void){

}

float rad_to_deg = 180.0f / 3.14159265f;
float alpha = 0.50f;  // complementary filter coefficient


tilt_angle_t get_tilt_angles_from_accel()
{
    raw_axes_t accel_raw;
    float_axes_t accel_g;
    tilt_angle_t angle;

    // Read raw data
    mpu_motion(&mpu, &accel_raw, NULL); // no need for gyro here

    // Convert to g's
    accel_g = mpu_math_accel_gravity(&accel_raw, ACCEL_FS_4G);

    // Calculate pitch and roll (in degrees)
    // pitch: rotation around Y-axis
    // roll:  rotation around X-axis

    float x = accel_g.x;
    float y = accel_g.y;
    float z = accel_g.z;

    angle.pitch = atan2f(x, sqrtf(y * y + z * z)) * rad_to_deg;
    angle.roll  = atan2f(y, sqrtf(x * x + z * z)) * rad_to_deg;

    return angle;
}

tilt_angle_t get_tilt_angles_complementary()
{
    static float angle_pitch = 0.0f;
    static float angle_roll = 0.0f;
    static int64_t last_time_us = 0;

    raw_axes_t accel_raw, gyro_raw;
    float_axes_t accel_g, gyro_dps;
    tilt_angle_t angle;

    // Time delta
    int64_t now_us = esp_timer_get_time();  // microseconds
    float dt = (last_time_us == 0) ? 0.01f : (now_us - last_time_us) / 1000000.0f;
    last_time_us = now_us;

    // Read sensor
    mpu_motion(&mpu, &accel_raw, &gyro_raw);

    // Convert
    accel_g = mpu_math_accel_gravity(&accel_raw, ACCEL_FS_4G);
    gyro_dps = mpu_math_gyro_deg_per_sec(&gyro_raw, GYRO_FS_500DPS);

    accel_raw.x -= offset.ax;
    accel_raw.y -= offset.ay;
    accel_raw.z -= offset.az;
    gyro_raw.x  -= offset.gx;
    gyro_raw.y  -= offset.gy;
    gyro_raw.z  -= offset.gz;

    // Calculate accel-based angles
    float pitch_acc = atan2f(accel_g.x, sqrtf(accel_g.y * accel_g.y + accel_g.z * accel_g.z)) * rad_to_deg;
    float roll_acc  = atan2f(accel_g.y, sqrtf(accel_g.x * accel_g.x + accel_g.z * accel_g.z)) * rad_to_deg;

    // Integrate gyro rate to angle
    angle_pitch = alpha * (angle_pitch + gyro_dps.y * dt) + (1.0f - alpha) * pitch_acc;
    angle_roll  = alpha * (angle_roll  + gyro_dps.x * dt) + (1.0f - alpha) * roll_acc;

    // Store result
    angle.pitch = angle_pitch;
    angle.roll = angle_roll;

    return angle;
}

void mpu6050_read(float_axes_t* data){
    raw_axes_t gyro_raw;   
    raw_axes_t accel_raw; 
    float_axes_t accel_g;   // accel axes in (g) gravity format
    float_axes_t gyro_dps;  // gyro axes in (DPS) º/s format
    // Read
    mpu_acceleration(&mpu, &accel_raw);  // fetch raw data from the registers
    mpu_rotation(&mpu, &gyro_raw);       // fetch raw data from the registers
    mpu_motion(&mpu, &accel_raw, &gyro_raw);  // read both in one shot
    // Convert
    accel_g = mpu_math_accel_gravity(&accel_raw, ACCEL_FS_4G);
    gyro_dps = mpu_math_gyro_deg_per_sec(&gyro_raw, GYRO_FS_500DPS);

    printf("%d,%d,%d |  %d, %d, %d\r\n",accel_raw.x,accel_raw.y,accel_raw.z,gyro_raw.x,gyro_raw.y,gyro_raw.z);
    // data->x = Acc_angle_x;
    // data->y = Acc_angle_y;
    // data->z = Acc_angle_z;
}

// Thresholds (degrees)

tilt_dir_t detect_2d_tilt(float pitch_deg, float roll_deg) {
    static tilt_dir_t current_state = NO_TILT;

    switch (current_state) {
        case NO_TILT:
            if (roll_deg < -TILT_TRIGGER)
                current_state = TILT_LEFT;
            else if (roll_deg > TILT_TRIGGER)
                current_state = TILT_RIGHT;
            // else if (pitch_deg < -TILT_TRIGGER)
            //     current_state = TILT_UP;
            // else if (pitch_deg > TILT_TRIGGER)
            //     current_state = TILT_DOWN;
            break;

        case TILT_LEFT:
            if (roll_deg > -TILT_RETURN_ZONE)
                current_state = NO_TILT;
            break;

        case TILT_RIGHT:
            if (roll_deg < TILT_RETURN_ZONE)
                current_state = NO_TILT;
            break;

        case TILT_UP:
            if (pitch_deg > -TILT_RETURN_ZONE)
                current_state = NO_TILT;
            break;

        case TILT_DOWN:
            if (pitch_deg < TILT_RETURN_ZONE)
                current_state = NO_TILT;
            break;
        default :
            break;
    }

    return current_state;
}


tilt_dir_t detect_2d_tilt_LR(float refrence_axis) {
    static tilt_dir_t current_state = NO_TILT;

    switch (current_state) {
        case NO_TILT:
            if (refrence_axis < -TILT_TRIGGER)
                current_state = TILT_LEFT;
            else if (refrence_axis > TILT_TRIGGER)
                current_state = TILT_RIGHT;
            break;

        case TILT_LEFT:
            if (refrence_axis > -TILT_RETURN_ZONE)
                current_state = NO_TILT;
            break;

        case TILT_RIGHT:
            if (refrence_axis < TILT_RETURN_ZONE)
                current_state = NO_TILT;
            break;

        default :
            break;
    }
    return current_state;
}

tilt_dir_t detect_2d_tilt_UD(float refrence_axis) {
    static tilt_dir_t current_state = NO_TILT;

    switch (current_state) {
        case NO_TILT:
            if (refrence_axis < -TILT_TRIGGER)
                current_state = TILT_UP;
            else if (refrence_axis > TILT_TRIGGER)
                current_state = TILT_DOWN;
            break;

        case TILT_UP:
            if (refrence_axis > -TILT_RETURN_ZONE)
                current_state = NO_TILT;
            break;

        case TILT_DOWN:
            if (refrence_axis < TILT_RETURN_ZONE)
                current_state = NO_TILT;
            break;
        default :
            break;
    }

    return current_state;
}


tilt_dir_t detect_8way_tilt(float pitch_deg, float roll_deg) {
    static tilt_dir_t current_state = NO_TILT;

    // Exit condition: close to neutral
    if (fabs(pitch_deg) < TILT_RETURN_ZONE && fabs(roll_deg) < TILT_RETURN_ZONE) {
        current_state = NO_TILT;
    }

    // Only allow new direction if we're in NO_TILT
    if (current_state == NO_TILT) {
        bool pitch_up = pitch_deg < -TILT_TRIGGER;
        bool pitch_down = pitch_deg > TILT_TRIGGER;
        bool roll_left = roll_deg < -TILT_TRIGGER;
        bool roll_right = roll_deg > TILT_TRIGGER;

        if (pitch_up && roll_left) current_state = TILT_UP_LEFT;
        else if (pitch_up && roll_right) current_state = TILT_UP_RIGHT;
        else if (pitch_down && roll_left) current_state = TILT_DOWN_LEFT;
        else if (pitch_down && roll_right) current_state = TILT_DOWN_RIGHT;
        else if (pitch_up) current_state = TILT_UP;
        else if (pitch_down) current_state = TILT_DOWN;
        else if (roll_left) current_state = TILT_LEFT;
        else if (roll_right) current_state = TILT_RIGHT;
    }

    return current_state;
}