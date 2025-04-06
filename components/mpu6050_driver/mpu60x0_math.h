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

#ifndef __mpu60x0_math_h__
#define __mpu60x0_math_h__

#include <math.h>
#include <stdint.h>
#include "mpu60x0_types.h"
#include "sdkconfig.h"

#define k_room_temp_offset     (int16_t)    -521   // LSB
#define k_celsius_offset        (float)     35.f   // ºC
#define k_temp_sensitivity      (float)     340.f  // LSB/ºC
#define k_temp_resolution       (float)     (98.67f / INT16_MAX)
#define k_fahrenheit_offset     (float)     (k_celsius_offset * 1.8f + 32)  // ºF

/*******************************************************************************
 * @brief
 * @param
 */
inline uint8_t mpu_math_accel_fsr_value(const accel_fs_t fs)
{
    return 2 << fs;
}

/*******************************************************************************
 * @brief
 * @param
 */
inline uint16_t mpu_math_gyro_fsr_value(const gyro_fs_t fs)
{
    return 250 << fs;
}

/*******************************************************************************
 * @brief
 * @param
 */
inline uint16_t mpu_math_accel_sensitivity(const accel_fs_t fs)
{
    return 16384 >> fs;
}

/*******************************************************************************
 * @brief
 * @param
 */
inline float mpu_math_gyro_sensitivity(const gyro_fs_t fs)
{
    return 131.f / (1 << fs);
}

/*******************************************************************************
 * @brief
 * @param
 */
inline float mpu_math_accel_resolution(const accel_fs_t fs)
{
    return (float) (mpu_math_accel_fsr_value(fs)) / INT16_MAX;
}

/*******************************************************************************
 * @brief
 * @param
 */
inline float mpu_math_gyro_resolution(const gyro_fs_t fs)
{
    return (float) (mpu_math_gyro_fsr_value(fs)) / INT16_MAX;
}

/*******************************************************************************
 * @brief
 * @param
 */
inline float_axes_t mpu_math_accel_gravity(const raw_axes_t* raw_axes,
        const accel_fs_t fs)
{
    float_axes_t axes;
    axes.x = raw_axes->x * mpu_math_accel_resolution(fs);
    axes.y = raw_axes->y * mpu_math_accel_resolution(fs);
    axes.z = raw_axes->z * mpu_math_accel_resolution(fs);
    return axes;
}

/*******************************************************************************
 * @brief
 * @param
 */
inline float mpu_math_gyro_deg_per_sec_single(const int16_t axis, const gyro_fs_t fs)
{
    return axis * mpu_math_gyro_resolution(fs);
}

/*******************************************************************************
 * @brief
 * @param
 */
inline float_axes_t mpu_math_gyro_deg_per_sec(const raw_axes_t* raw_axes, const gyro_fs_t fs)
{
    float_axes_t axes;
    axes.x = raw_axes->x * mpu_math_gyro_resolution(fs);
    axes.y = raw_axes->y * mpu_math_gyro_resolution(fs);
    axes.z = raw_axes->z * mpu_math_gyro_resolution(fs);
    return axes;
}

/*******************************************************************************
 * @brief
 * @param
 */
inline float mpu_math_gyro_rad_per_sec_single(const int16_t axis, const gyro_fs_t fs)
{
    return (M_PI / 180) * mpu_math_gyro_deg_per_sec_single(axis, fs);
}

/*******************************************************************************
 * @brief
 * @param
 */
inline float_axes_t mpu_math_gyro_rad_per_sec(const raw_axes_t* raw_axes, const gyro_fs_t fs)
{
    float_axes_t axes;
    axes.x = (M_PI / 180) * mpu_math_gyro_deg_per_sec_single(raw_axes->x, fs);
    axes.y = (M_PI / 180) * mpu_math_gyro_deg_per_sec_single(raw_axes->y, fs);
    axes.z = (M_PI / 180) * mpu_math_gyro_deg_per_sec_single(raw_axes->z, fs);
    return axes;
}

/*******************************************************************************
 * @brief
 * @param
 */
inline float mpu_math_temp_celsius(const int16_t temp)
{
    // temp_deg_c = ( ( temp_out – room_temp_offset ) / temp_sensitivity ) + degrees_celsius_offset
    return (temp - k_room_temp_offset) * k_temp_resolution + k_celsius_offset;
}

/*******************************************************************************
 * @brief
 * @param
 */
inline float mpu_math_temp_fahrenheit(const int16_t temp)
{
    return (temp - k_room_temp_offset) * k_temp_resolution * 1.8f +
           k_fahrenheit_offset;
}

#endif // ifndef __mpu60x0_math_h__
