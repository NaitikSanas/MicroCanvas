// =========================================================================
// This library is placed under the MIT License
//
// C port
// Copyright 2020 Maxime Clement
//
// Original Author esp32-MPU-driver C++ libraries
// Copyright 2017-2018 Natanael Josue Rabello 
//
// For the license information refer to LICENSE file in root directory
// =========================================================================

#ifndef __mpu60x0_h__
#define __mpu60x0_h__

#include "esp_err.h"
#include "mpu60x0_math.h"
#include "mpu60x0_types.h"

esp_err_t mpu_write_fifo(mpu_handle_t *mpu,size_t length,const uint8_t *data);
esp_err_t mpu_read_fifo(mpu_handle_t *mpu,size_t length,uint8_t *data);
uint16_t mpu_get_fifo_count(mpu_handle_t *mpu);
esp_err_t mpu_reset_fifo(mpu_handle_t *mpu);
bool mpu_get_fifo_enabled(mpu_handle_t *mpu);
esp_err_t mpu_set_fifo_enabled(mpu_handle_t *mpu,bool enable);
fifo_config_t mpu_get_fifo_config(mpu_handle_t *mpu);
esp_err_t mpu_set_fifo_config(mpu_handle_t *mpu,fifo_config_t config);
fifo_mode_t mpu_get_fifo_mode(mpu_handle_t *mpu);
esp_err_t mpu_set_fifo_mode(mpu_handle_t *mpu,fifo_mode_t mode);
int_stat_t mpu_get_interrupt_status(mpu_handle_t *mpu);
int_en_t mpu_get_interrupt_enabled(mpu_handle_t *mpu);
esp_err_t mpu_set_interrupt_enabled(mpu_handle_t *mpu,int_en_t mask);
int_config_t mpu_get_interrupt_config(mpu_handle_t *mpu);
esp_err_t mpu_setInterruptConfig(mpu_handle_t *mpu,int_config_t config);
#if defined CONFIG_MPU_6050
mot_stat_t mpu_get_motion_detect_status(mpu_handle_t *mpu);
ff_config_t mpu_get_free_fall_config(mpu_handle_t *mpu);
esp_err_t mpu_set_free_fall_config(mpu_handle_t *mpu,ff_config_t *config);
zrmot_config_t mpu_get_zero_motion_config(mpu_handle_t *mpu);
esp_err_t mpu_set_zero_motion_config(mpu_handle_t *mpu,zrmot_config_t *config);
#endif
mot_config_t mpu_get_motion_detect_config(mpu_handle_t *mpu);
esp_err_t mpu_set_motion_detect_config(mpu_handle_t *mpu,mot_config_t *config);
bool mpu_get_motion_feature_enabled(mpu_handle_t *mpu);
esp_err_t mpu_set_motion_feature_enabled(mpu_handle_t *mpu,bool enable);
esp_err_t mpu_set_standby_mode(mpu_handle_t *mpu,stby_en_t mask);
lp_accel_rate_t mpu_get_low_power_accel_rate(mpu_handle_t *mpu);
esp_err_t mpu_set_low_power_accel_rate(mpu_handle_t *mpu,lp_accel_rate_t rate);
bool mpu_get_low_power_accel_mode(mpu_handle_t *mpu);
esp_err_t mpu_set_low_power_accel_mode(mpu_handle_t *mpu,bool enable);
esp_err_t mpu_reset_signal_path(mpu_handle_t *mpu);
esp_err_t mpu_motion(mpu_handle_t *mpu,raw_axes_t *accel,raw_axes_t *gyro);
esp_err_t mpu_temperature(mpu_handle_t *mpu,int16_t *temp);
esp_err_t mpu_rotation(mpu_handle_t *mpu,raw_axes_t *gyro);
esp_err_t mpu_acceleration(mpu_handle_t *mpu,raw_axes_t *accel);
esp_err_t mpu_test_connection(mpu_handle_t *mpu);
bool mpu_get_sleep(mpu_handle_t *mpu);
esp_err_t mpu_set_sleep(mpu_handle_t *mpu,bool enable);
esp_err_t mpu_reset(mpu_handle_t *mpu);
esp_err_t mpu_initialize_chip(mpu_handle_t *mpu);
esp_err_t mpu_initialize_peripheral(mpu_handle_t *mpu);
void mpu_print_something(void);



#endif // ifndef __mpu60x0_h__