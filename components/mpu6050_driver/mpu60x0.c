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

#include <stdint.h>
#include <stdbool.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mpu60x0.h"
#include "mpu60x0_i2c.h"

static const char *TAG = "MPU"; // needs to be included before mpu60x0_log.h
#include "mpu60x0_log.h"


// Static declarations
static esp_err_t mpu_set_clock_src(mpu_handle_t * mpu,
                                   mpu_clock_src_t clockSrc);
static mpu_clock_src_t mpu_get_clock_src(mpu_handle_t * mpu);
static esp_err_t mpu_set_gyro_full_scale(mpu_handle_t * mpu, gyro_fs_t fsr);
static gyro_fs_t mpu_get_gyro_full_scale(mpu_handle_t * mpu);
static esp_err_t mpu_set_accel_full_scale(mpu_handle_t * mpu, accel_fs_t fsr);
static accel_fs_t mpu_get_accel_full_scale(mpu_handle_t * mpu);
static esp_err_t mpu_set_dlp_filter(mpu_handle_t * mpu, dlpf_t dlpf);
static dlpf_t mpu_get_dlp_filter(mpu_handle_t * mpu);
static esp_err_t mpu_set_sample_rate(mpu_handle_t * mpu, uint16_t rate);
static uint16_t mpu_get_sample_rate(mpu_handle_t * mpu);
static esp_err_t mpu_set_aux_i2c_enabled(mpu_handle_t * mpu, bool enable);
static bool mpu_get_aux_i2c_enabled(mpu_handle_t * mpu);

/*******************************************************************************
 * @brief Purely for debug
 */
void mpu_print_something(void)
{
    ESP_LOGI(TAG, "Log from mpu6050");
}

// #### ##    ## #### ########
//  ##  ###   ##  ##     ##
//  ##  ####  ##  ##     ##
//  ##  ## ## ##  ##     ##
//  ##  ##  ####  ##     ##
//  ##  ##   ###  ##     ##
// #### ##    ## ####    ##

/*******************************************************************************
 * @brief initialize the I2C/SPI peripheral
 * @details wrapper arount mpu_i2c_begin/mpu_spi_begin
 */
esp_err_t mpu_initialize_peripheral(mpu_handle_t * mpu)
{
    esp_err_t err = mpu_i2c_begin(mpu);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Initialize MPU device and set basic configurations.
 * @details
 *  Init configuration:
 *  - Accel FSR: 4G
 *  - Gyro FSR: 500DPS
 *  - Sample rate: 100Hz
 *  - DLPF: 42Hz
 *  - INT pin: disabled
 *  - FIFO: disabled
 *  - Clock source: gyro PLL \n
 *
 * @note
 *  - A soft reset is performed first, which takes 100-200ms.
 *  - When using SPI, the primary I2C Slave module is disabled right away.
 */
esp_err_t mpu_initialize_chip(mpu_handle_t * mpu)
{
    esp_err_t err;

    // reset device (wait a little to clear all registers)
    err = mpu_reset(mpu);
    if(MPU_ERR_CHECK(err)) return err;

    // wake-up the device (power on-reset state is asleep for some models)
    err = mpu_set_sleep(mpu, false);
    if(MPU_ERR_CHECK(err)) return err;

#ifdef CONFIG_MPU_R_SPI
    err = mpu_i2c_write_bit(mpu, MPU_R_USER_CTRL,
                            MPU_R_USERCTRL_I2C_IF_DIS_BIT, 1);
    if(MPU_ERR_CHECK(err)) return err;
#endif

    // disable MPU's I2C slave module when using SPI
    err = mpu_set_clock_src(mpu, (uint8_t) CLOCK_PLL);
    if(MPU_ERR_CHECK(err)) return err;

    // set Full Scale range
    err = mpu_set_gyro_full_scale(mpu, GYRO_FS_500DPS);
    if(MPU_ERR_CHECK(err)) return err;

    err = mpu_set_accel_full_scale(mpu, ACCEL_FS_4G);
    if(MPU_ERR_CHECK(err)) return err;

    // set Digital Low Pass Filter to get smoother data
    err = mpu_set_dlp_filter(mpu, DLPF_42HZ);
    if(MPU_ERR_CHECK(err)) return err;

    // set sample rate to 100Hz
    err = mpu_set_sample_rate(mpu, 100);
    if(MPU_ERR_CHECK(err)) return err;

    MPU_LOGI("Initialization complete");
    return err;
}

/*******************************************************************************
 * @brief Reset internal registers and restore to default start-up state.
 * @note
 *  - This function delays 100ms when using I2C and 200ms when using SPI.
 *  - It does not initialize the MPU again, just call initialize() instead.
 */
esp_err_t mpu_reset(mpu_handle_t * mpu)
{
    esp_err_t err = mpu_i2c_write_bit(mpu, MPU_R_PWR_MGMT1,
                                      MPU_R_PWR1_DEVICE_RESET_BIT,
                                      1);
    if(MPU_ERR_CHECK(err)) return err;
    vTaskDelay(100 / portTICK_PERIOD_MS);

#ifdef CONFIG_MPU_R_SPI
    err = reset_signal_path(mpu);
    if(MPU_ERR_CHECK(err)) return err;
#endif

    MPU_LOGI("Reset!");
    return err;
}

//  ######  ##       ######## ######## ########
// ##    ## ##       ##       ##       ##     ##
// ##       ##       ##       ##       ##     ##
//  ######  ##       ######   ######   ########
//       ## ##       ##       ##       ##
// ##    ## ##       ##       ##       ##
//  ######  ######## ######## ######## ##

/*******************************************************************************
 * @brief Enable / disable sleep mode
 * @param enable enable value
 */
esp_err_t mpu_set_sleep(mpu_handle_t * mpu, bool enable)
{
    esp_err_t err;
    err =  mpu_i2c_write_bit(mpu, MPU_R_PWR_MGMT1, MPU_R_PWR1_SLEEP_BIT,
                             (uint8_t) enable);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Get current sleep state.
 * @return
 *  - `true`: sleep enabled.
 *  - `false`: sleep disabled.
 */
bool mpu_get_sleep(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bit(mpu, MPU_R_PWR_MGMT1,
                                     MPU_R_PWR1_SLEEP_BIT,
                                     buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;
    return buffer[0];
}

// ######## ########  ######  ########
//    ##    ##       ##    ##    ##
//    ##    ##       ##          ##
//    ##    ######    ######     ##
//    ##    ##             ##    ##
//    ##    ##       ##    ##    ##
//    ##    ########  ######     ##

/*******************************************************************************
 * @brief Returns the value from WHO_AM_I register.
 */
static esp_err_t mpu_who_am_i(mpu_handle_t * mpu, uint8_t * buff)
{
    esp_err_t err = mpu_i2c_read_byte(mpu, MPU_R_WHO_AM_I, buff);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Test connection with MPU.
 * @details It reads the WHO_AM_IM register and check its value against
 *          the correct chip model.
 * @return
 *  - `ESP_OK`: The MPU is connected and matchs the model.
 *  - `ESP_ERR_NOT_FOUND`: A device is connect, but does not match the
 *                                                chip selected in _menuconfig_.
 *  - May return other communication bus errors. e.g:
 *                                      `ESP_FAIL`, `ESP_ERR_TIMEOUT`.
 */
esp_err_t mpu_test_connection(mpu_handle_t * mpu)
{
    uint8_t wai;
    esp_err_t err = mpu_who_am_i(mpu, &wai);
    if(MPU_ERR_CHECK(err)) return err;
    return (wai == 0x68) ? ESP_OK : ESP_ERR_NOT_FOUND;
}

//  ######  ##        #######   ######  ##    ##
// ##    ## ##       ##     ## ##    ## ##   ##
// ##       ##       ##     ## ##       ##  ##
// ##       ##       ##     ## ##       #####
// ##       ##       ##     ## ##       ##  ##
// ##    ## ##       ##     ## ##    ## ##   ##
//  ######  ########  #######   ######  ##    ##

/*******************************************************************************
 * @brief Select clock source.
 * @note The gyro PLL is better than internal clock.
 * @param clockSrc clock source
 */
static esp_err_t mpu_set_clock_src(mpu_handle_t * mpu, mpu_clock_src_t clockSrc)
{
    esp_err_t err = mpu_i2c_write_bits(mpu, MPU_R_PWR_MGMT1,
                                       MPU_R_PWR1_CLKSEL_BIT,
                                       MPU_R_PWR1_CLKSEL_LENGTH, clockSrc);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Return clock source.
 */
static mpu_clock_src_t mpu_get_clock_src(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bits(mpu, MPU_R_PWR_MGMT1,
                                      MPU_R_PWR1_CLKSEL_BIT,
                                      MPU_R_PWR1_CLKSEL_LENGTH, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;
    return (mpu_clock_src_t) buffer[0];
}

//  ######   ######     ###    ##       ########
// ##    ## ##    ##   ## ##   ##       ##
// ##       ##        ##   ##  ##       ##
//  ######  ##       ##     ## ##       ######
//       ## ##       ######### ##       ##
// ##    ## ##    ## ##     ## ##       ##
//  ######   ######  ##     ## ######## ########

/*******************************************************************************
 * @brief Select Gyroscope Full-scale range.
 */
static esp_err_t mpu_set_gyro_full_scale(mpu_handle_t * mpu, gyro_fs_t fsr)
{
    esp_err_t err = mpu_i2c_write_bits(mpu, MPU_R_GYRO_CONFIG,
                                       MPU_R_GCONFIG_FS_SEL_BIT,
                                       MPU_R_GCONFIG_FS_SEL_LENGTH, fsr);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Return Gyroscope Full-scale range.
 */
static gyro_fs_t mpu_get_gyro_full_scale(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bits(mpu, MPU_R_GYRO_CONFIG,
                                      MPU_R_GCONFIG_FS_SEL_BIT,
                                      MPU_R_GCONFIG_FS_SEL_LENGTH, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;
    return (gyro_fs_t) buffer[0];
}

/*******************************************************************************
 * @brief Select Accelerometer Full-scale range.
 * */
static esp_err_t mpu_set_accel_full_scale(mpu_handle_t * mpu, accel_fs_t fsr)
{
    esp_err_t err = mpu_i2c_write_bits(mpu, MPU_R_ACCEL_CONFIG,
                                       MPU_R_ACONFIG_FS_SEL_BIT,
                                       MPU_R_ACONFIG_FS_SEL_LENGTH, fsr);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Return Accelerometer Full-scale range.
 */
static accel_fs_t mpu_get_accel_full_scale(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bits(mpu, MPU_R_ACCEL_CONFIG,
                                      MPU_R_ACONFIG_FS_SEL_BIT,
                                      MPU_R_ACONFIG_FS_SEL_LENGTH, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;
    return (accel_fs_t) buffer[0];
}

// ########  ##       ########  ########
// ##     ## ##       ##     ## ##
// ##     ## ##       ##     ## ##
// ##     ## ##       ########  ######
// ##     ## ##       ##        ##
// ##     ## ##       ##        ##
// ########  ######## ##        ##        Digital Low Pass Filter

/*******************************************************************************
 * @brief Configures Digital Low Pass Filter (DLPF) setting for both the
 *          gyroscope and accelerometer.
 * @param dlpf digital low-pass filter value
 */
static esp_err_t mpu_set_dlp_filter(mpu_handle_t * mpu,
                                    dlpf_t dlpf)
{
    esp_err_t err = mpu_i2c_write_bits(mpu, MPU_R_CONFIG,
                                       MPU_R_CONFIG_DLPF_CFG_BIT,
                                       MPU_R_CONFIG_DLPF_CFG_LENGTH, dlpf);
    if(MPU_ERR_CHECK(err)) return err;

    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Return Digital Low Pass Filter configuration
 */
static dlpf_t mpu_get_dlp_filter(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bits(mpu, MPU_R_CONFIG,
                                      MPU_R_CONFIG_DLPF_CFG_BIT,
                                      MPU_R_CONFIG_DLPF_CFG_LENGTH, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;
    return (dlpf_t) buffer[0];
}

//  ######     ###    ##     ## ########  ##       ########       ########
// ##    ##   ## ##   ###   ### ##     ## ##       ##             ##     ##
// ##        ##   ##  #### #### ##     ## ##       ##             ##     ##
//  ######  ##     ## ## ### ## ########  ##       ######         ########
//       ## ######### ##     ## ##        ##       ##             ##   ##
// ##    ## ##     ## ##     ## ##        ##       ##             ##    ##  ###
//  ######  ##     ## ##     ## ##        ######## ########       ##     ## ###

/*******************************************************************************
 * @brief Set sample rate of data output.
 *
 * @details Sample rate controls sensor data output rate and FIFO sample rate.
 * This is the update rate of sensor register. \n
 * Formula: Sample Rate = Internal Output Rate / (1 + SMPLRT_DIV)
 *
 * @param rate 4Hz ~ 1KHz
 *  - For sample rate 8KHz: set digital low pass filter to DLPF_256HZ_NOLPF.
 *  - For sample rate 32KHZ [MPU6500 / MPU9250]: set fchoice to FCHOICE_0, see setFchoice().
 */
static esp_err_t mpu_set_sample_rate(mpu_handle_t * mpu, uint16_t rate)
{
    // Check value range
    if (rate < 4)
    {
        MPU_LOGWMSG(MPU_MSG_INVALID_SAMPLE_RATE, " %d, minimum rate is 4",
                    rate);
        rate = 4;
    }
    else if (rate > 1000)
    {
        MPU_LOGWMSG(MPU_MSG_INVALID_SAMPLE_RATE, " %d, maximum rate is 1000",
                    rate);
        rate = 1000;
    }

#if CONFIG_MPU_LOG_LEVEL >= ESP_LOG_WARN
    // Check dlpf configuration
    dlpf_t dlpf = mpu_get_dlp_filter(mpu);
    esp_err_t err = mpu->last_err;
    if (MPU_ERR_CHECK(err)) return err;
    if (dlpf == 0 || dlpf == 7)
        MPU_LOGWMSG(MPU_MSG_INVALID_STATE,
                    ", sample rate divider is not effective when DLPF is (0 or 7)");
#endif

    uint16_t internal_sample_rate = 1000;
    uint16_t divider              = internal_sample_rate / rate - 1;
    // Check for rate match
    uint16_t finalRate = (internal_sample_rate / (1 + divider));
    if (finalRate != rate)
    {
        MPU_LOGW("Sample rate constrained to %d Hz", finalRate);
    }
    else
    {
        MPU_LOGI("Sample rate set to %d Hz", finalRate);
    }
    // Write divider to register
    err = mpu_i2c_write_byte(mpu, MPU_R_SMPLRT_DIV, (uint8_t) divider);
    if(MPU_ERR_CHECK(err)) return err;

    return err;
}

/*******************************************************************************
 * @brief Retrieve sample rate divider and calculate the actual rate.
 */
static uint16_t mpu_get_sample_rate(mpu_handle_t * mpu)
{
    uint16_t sample_rate_max_nolpf = 8000;

    dlpf_t dlpf                    = mpu_get_dlp_filter(mpu);
    esp_err_t err = mpu->last_err;
    if (MPU_ERR_CHECK(err)) return err;

    if (dlpf == 0 || dlpf == 7) return sample_rate_max_nolpf;

    uint16_t internal_sample_rate = 1000;
    uint8_t* buffer = mpu->buffer;
    err = mpu_i2c_read_byte(mpu, MPU_R_SMPLRT_DIV, buffer);
    if(MPU_ERR_CHECK(err)) return err;

    uint16_t rate = internal_sample_rate / (1 + buffer[0]);
    return rate;
}

// ########  ########    ###    ########  #### ##    ##  ######    ######
// ##     ## ##         ## ##   ##     ##  ##  ###   ## ##    ##  ##    ##
// ##     ## ##        ##   ##  ##     ##  ##  ####  ## ##        ##
// ########  ######   ##     ## ##     ##  ##  ## ## ## ##   ####  ######
// ##   ##   ##       ######### ##     ##  ##  ##  #### ##    ##        ##
// ##    ##  ##       ##     ## ##     ##  ##  ##   ### ##    ##  ##    ##
// ##     ## ######## ##     ## ########  #### ##    ##  ######    ######

/*******************************************************************************
 * @brief Read accelerometer raw data.
 * */
esp_err_t mpu_acceleration(mpu_handle_t * mpu, raw_axes_t* accel)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bytes(mpu, MPU_R_ACCEL_XOUT_H, 6, buffer);
    if (MPU_ERR_CHECK(err)) return err;

    accel->x = buffer[0] << 8 | buffer[1];
    accel->y = buffer[2] << 8 | buffer[3];
    accel->z = buffer[4] << 8 | buffer[5];

    return err;
}

/*******************************************************************************
 * @brief Read gyroscope raw data.
 * */
esp_err_t mpu_rotation(mpu_handle_t * mpu, raw_axes_t* gyro)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bytes(mpu, MPU_R_GYRO_XOUT_H, 6, buffer);
    if (MPU_ERR_CHECK(err)) return err;

    gyro->x = buffer[0] << 8 | buffer[1];
    gyro->y = buffer[2] << 8 | buffer[3];
    gyro->z = buffer[4] << 8 | buffer[5];

    return err;
}

/*******************************************************************************
 * @brief Read temperature raw data.
 * */
esp_err_t mpu_temperature(mpu_handle_t * mpu, int16_t* temp)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bytes(mpu, MPU_R_TEMP_OUT_H, 2, buffer);
    if (MPU_ERR_CHECK(err)) return err;
    *temp = buffer[0] << 8 | buffer[1];
    return err;
}

/*******************************************************************************
 * @brief Read accelerometer and gyroscope data at once.
 * */
esp_err_t mpu_motion(mpu_handle_t * mpu, raw_axes_t* accel, raw_axes_t* gyro)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bytes(mpu, MPU_R_ACCEL_XOUT_H, 14, buffer);
    if (MPU_ERR_CHECK(err)) return err;
    if(accel){
        accel->x = buffer[0] << 8 | buffer[1];
        accel->y = buffer[2] << 8 | buffer[3];
        accel->z = buffer[4] << 8 | buffer[5];
    }
    if(gyro){
        gyro->x  = buffer[8] << 8 | buffer[9];
        gyro->y  = buffer[10] << 8 | buffer[11];
        gyro->z  = buffer[12] << 8 | buffer[13];
    }
    return err;
}

/*******************************************************************************
 * @brief Reset sensors signal path.
 *
 * @details Reset all gyro digital signal path, accel digital signal path, and temp
 * digital signal path. This also clears all the sensor registers.
 *
 * @note This function delays 100 ms, needed for reset to complete.
 * */
esp_err_t mpu_reset_signal_path(mpu_handle_t * mpu)
{
    esp_err_t err = mpu_i2c_write_bit(mpu, MPU_R_USER_CTRL,
                                      MPU_R_USERCTRL_SIG_COND_RESET_BIT, 1);
    if (MPU_ERR_CHECK(err)) return err;
    vTaskDelay(100 / portTICK_PERIOD_MS);
    return err;
}

// ##        #######  ##      ##    ########   #######  ##      ##
// ##       ##     ## ##  ##  ##    ##     ## ##     ## ##  ##  ##
// ##       ##     ## ##  ##  ##    ##     ## ##     ## ##  ##  ##
// ##       ##     ## ##  ##  ##    ########  ##     ## ##  ##  ##
// ##       ##     ## ##  ##  ##    ##        ##     ## ##  ##  ##
// ##       ##     ## ##  ##  ##    ##        ##     ## ##  ##  ##
// ########  #######   ###  ###     ##         #######   ###  ###

/*******************************************************************************
 * @brief Enter Low Power Accelerometer mode.
 *
 * @details In low-power accel mode, the chip goes to sleep and only wakes up to sample
 * the accelerometer at a certain frequency.
 * See setLowPowerAccelRate() to set the frequency.
 *
 * @param enable value
 *  + This function does the following when enable == true :
 *   - Set CYCLE bit to 1
 *   - Set SLEEP bit to 0
 *   - Set TEMP_DIS bit to 1
 *   - Set STBY_XG, STBY_YG, STBY_ZG bits to 1
 *   - Set STBY_XA, STBY_YA, STBY_ZA bits to 0
 *   - Set FCHOICE to 0 (ACCEL_FCHOICE_B bit to 1) [MPU6500 / MPU9250 only]
 *   - Disable Auxiliary I2C Master I/F
 *
 *  + This function does the following when enable == false :
 *   - Set CYCLE bit to 0
 *   - Set TEMP_DIS bit to 0
 *   - Set STBY_XG, STBY_YG, STBY_ZG bits to 0
 *   - Set STBY_XA, STBY_YA, STBY_ZA bits to 0
 *   - Set FCHOICE to 3 (ACCEL_FCHOICE_B bit to 0) [MPU6500 / MPU9250 only]
 *   - Enable Auxiliary I2C Master I/F
 * */
esp_err_t mpu_set_low_power_accel_mode(mpu_handle_t * mpu, bool enable)
{
    // read PWR_MGMT1 and PWR_MGMT2 at once
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bytes(mpu, MPU_R_PWR_MGMT1, 2, buffer);
    if (MPU_ERR_CHECK(err)) return err;

    if (enable)
    {
        // set CYCLE bit to 1 and SLEEP bit to 0 and TEMP_DIS bit to 1
        buffer[0] |= 1 << MPU_R_PWR1_CYCLE_BIT;
        buffer[0] &= ~(1 << MPU_R_PWR1_SLEEP_BIT);
        buffer[0] |= 1 << MPU_R_PWR1_TEMP_DIS_BIT;
        // set STBY_XG, STBY_YG, STBY_ZG bits to 1
        buffer[1] |= MPU_R_PWR2_STBY_XYZG_BITS;
    }
    else    // disable
    {
        // set CYCLE bit to 0 and TEMP_DIS bit to 0
        buffer[0] &= ~(1 << MPU_R_PWR1_CYCLE_BIT);
        buffer[0] &= ~(1 << MPU_R_PWR1_TEMP_DIS_BIT);
        // set STBY_XG, STBY_YG, STBY_ZG bits to 0
        buffer[1] &= ~(MPU_R_PWR2_STBY_XYZG_BITS);
    }
    // set STBY_XA, STBY_YA, STBY_ZA bits to 0
    buffer[1] &= ~(MPU_R_PWR2_STBY_XYZA_BITS);

    // write back PWR_MGMT1 and PWR_MGMT2 at once
    err = mpu_i2c_write_bytes(mpu, MPU_R_PWR_MGMT1, 2, buffer);
    if (MPU_ERR_CHECK(err)) return err;

    // disable Auxiliary I2C Master I/F in case it was active
    err = mpu_set_aux_i2c_enabled(mpu, !enable);
    if (MPU_ERR_CHECK(err)) return err;

    return err;
}

/*******************************************************************************
 * @brief Return Low Power Accelerometer state.
 *
 * @note Condition to return true:
 *  - CYCLE bit is 1
 *  - SLEEP bit is 0
 *  - TEMP_DIS bit is 1
 *  - STBY_XG, STBY_YG, STBY_ZG bits are 1
 *  - STBY_XA, STBY_YA, STBY_ZA bits are 0
 *  - FCHOICE is 0 (ACCEL_FCHOICE_B bit is 1) [MPU6500 / MPU9250 only]
 *
 * */
bool mpu_get_low_power_accel_mode(mpu_handle_t * mpu)
{
    // read PWR_MGMT1 and PWR_MGMT2 at once
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bytes(mpu, MPU_R_PWR_MGMT1, 2, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    // define configuration bits
    uint8_t LPACCEL_CONFIG_BITMASK[2] =
    {
        (1 << MPU_R_PWR1_SLEEP_BIT) | (1 << MPU_R_PWR1_CYCLE_BIT) | (1 << MPU_R_PWR1_TEMP_DIS_BIT),
        MPU_R_PWR2_STBY_XYZA_BITS | MPU_R_PWR2_STBY_XYZG_BITS
    };
    uint8_t LPACCEL_ENABLED_VALUE[2] =
    {
        (1 << MPU_R_PWR1_CYCLE_BIT) | (1 << MPU_R_PWR1_TEMP_DIS_BIT),
        MPU_R_PWR2_STBY_XYZG_BITS
    };
    // get just the configuration bits
    buffer[0] &= LPACCEL_CONFIG_BITMASK[0];
    buffer[1] &= LPACCEL_CONFIG_BITMASK[1];
    // check pattern
    return buffer[0] == LPACCEL_ENABLED_VALUE[0]
           && buffer[1] == LPACCEL_ENABLED_VALUE[1];
}

/*******************************************************************************
 * @brief Set Low Power Accelerometer frequency of wake-up.
 * */
esp_err_t mpu_set_low_power_accel_rate(mpu_handle_t * mpu, lp_accel_rate_t rate)
{
    esp_err_t err = mpu_i2c_write_bits(mpu, MPU_R_PWR_MGMT2,
                                       MPU_R_PWR2_LP_WAKE_CTRL_BIT,
                                       MPU_R_PWR2_LP_WAKE_CTRL_LENGTH, rate);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Get Low Power Accelerometer frequency of wake-up.
 */
lp_accel_rate_t mpu_get_low_power_accel_rate(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bits(mpu, MPU_R_PWR_MGMT2,
                                      MPU_R_PWR2_LP_WAKE_CTRL_BIT,
                                      MPU_R_PWR2_LP_WAKE_CTRL_LENGTH, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    return (lp_accel_rate_t) buffer[0];
}

/*******************************************************************************
 * @brief Configure sensors' standby mode.
 * */
esp_err_t mpu_set_standby_mode(mpu_handle_t * mpu, stby_en_t mask)
{
    const uint8_t k_pwr1_stby_bits = mask >> 6;
    esp_err_t err = mpu_i2c_write_bits(mpu, MPU_R_PWR_MGMT1,
                                       MPU_R_PWR1_GYRO_STANDBY_BIT,
                                       2, k_pwr1_stby_bits);
    if (MPU_ERR_CHECK(err)) return err;

    err = mpu_i2c_write_bits(mpu, MPU_R_PWR_MGMT2, MPU_R_PWR2_STBY_XA_BIT, 6, mask);
    return MPU_ERR_CHECK(err);
}

// /*******************************************************************************
//  * @brief Return Standby configuration.
//  * */
// stby_en_t mpu_get_standby_mode(mpu_handle_t * mpu)
// {
//     MPU_ERR_CHECK(readBytes(MPU_R_PWR_MGMT1, 2, buffer));
//     constexpr uint8_t kStbyTempAndGyroPLLBits = STBY_EN_TEMP | STBY_EN_LOWPWR_GYRO_PLL_ON;
//     stby_en_t mask                            = buffer[0] << 3 & kStbyTempAndGyroPLLBits;
//     constexpr uint8_t kStbyAccelAndGyroBits   = STBY_EN_ACCEL | STBY_EN_GYRO;
//     mask |= buffer[1] & kStbyAccelAndGyroBits;
//     return mask;
// }

//    ###    ##     ## ##     ##    ####  #######   ######
//   ## ##   ##     ##  ##   ##      ##  ##     ## ##    ##
//  ##   ##  ##     ##   ## ##       ##         ## ##
// ##     ## ##     ##    ###        ##   #######  ##
// ######### ##     ##   ## ##       ##  ##        ##
// ##     ## ##     ##  ##   ##      ##  ##        ##    ##
// ##     ##  #######  ##     ##    #### #########  ######

/*******************************************************************************
 * @brief Enable / disable Auxiliary I2C Master module.
 * */
static esp_err_t mpu_set_aux_i2c_enabled(mpu_handle_t * mpu, bool enable)
{
    esp_err_t err = mpu_i2c_write_bit(mpu, MPU_R_USER_CTRL,
                                      MPU_R_USERCTRL_I2C_MST_EN_BIT, (uint8_t) enable);
    if (MPU_ERR_CHECK(err)) return err;

    if (enable)
    {
        err = mpu_i2c_write_bit(mpu, MPU_R_INT_PIN_CONFIG,
                                MPU_R_INT_CFG_I2C_BYPASS_EN_BIT, 0);
        return (MPU_ERR_CHECK(err));
    }
    return err;
}

/*******************************************************************************
 * @brief Return Auxiliary I2C Master state.
 */
static bool mpu_get_aux_i2c_enabled(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;

    esp_err_t err = mpu_i2c_read_bit(mpu, MPU_R_USER_CTRL,
                                     MPU_R_USERCTRL_I2C_MST_EN_BIT, buffer);
    if (MPU_ERR_CHECK(err)) return err;

    err = mpu_i2c_read_bit(mpu, MPU_R_INT_PIN_CONFIG,
                           MPU_R_INT_CFG_I2C_BYPASS_EN_BIT, buffer + 1);
    if (MPU_ERR_CHECK(err)) return err;

    return buffer[0] && (!buffer[1]);
}

// ##     ##  #######  ######## ####  #######  ##    ##
// ###   ### ##     ##    ##     ##  ##     ## ###   ##
// #### #### ##     ##    ##     ##  ##     ## ####  ##
// ## ### ## ##     ##    ##     ##  ##     ## ## ## ##
// ##     ## ##     ##    ##     ##  ##     ## ##  ####
// ##     ## ##     ##    ##     ##  ##     ## ##   ###
// ##     ##  #######     ##    ####  #######  ##    ##

/*******************************************************************************
 * @brief Enable/disable Motion modules (Motion detect, Zero-motion, Free-Fall).
 *
 * @attention
 *  The configurations must've already been set with setMotionDetectConfig() before
 *  enabling the module!
 * @note
 *  - Call getMotionDetectStatus() to find out which axis generated motion interrupt.
 *    [MPU6000, MPU6050, MPU9150].
 *  - It is recommended to set the Motion Interrupt to propagate to the INT pin.
 *    To do that, use setInterruptEnabled().
 * @param enable
 *  - On _true_, this function modifies the DLPF, put gyro and temperature in standby,
 *    and disable Auxiliary I2C Master I/F.
 *  - On _false_, this function sets DLPF to 42Hz and enables Auxiliary I2C master I/F.
 * */
esp_err_t mpu_set_motion_feature_enabled(mpu_handle_t * mpu, bool enable)
{
    esp_err_t err;
    if (enable)
    {
        dlpf_t kDLPF = DLPF_256HZ_NOLPF;
        err = mpu_set_dlp_filter(mpu, kDLPF);
        if (MPU_ERR_CHECK(err)) return err;

        // give a time for accumulation of samples
        vTaskDelay(10 / portTICK_PERIOD_MS);
        err = mpu_i2c_write_bits(mpu, MPU_R_ACCEL_CONFIG, MPU_R_ACONFIG_HPF_BIT,
                                 MPU_R_ACONFIG_HPF_LENGTH, ACCEL_DHPF_HOLD);
        if (MPU_ERR_CHECK(err)) return err;
    }
    else
    {
        dlpf_t kDLPF = DLPF_42HZ;
        err = mpu_set_dlp_filter(mpu, kDLPF);
        if (MPU_ERR_CHECK(err)) return err;
    }
    // disable Auxiliary I2C Master I/F in case it was active
    err = mpu_set_aux_i2c_enabled(mpu, !enable);
    if (MPU_ERR_CHECK(err)) return err;
    return err;
}

/*******************************************************************************
 * @brief Return true if a Motion Dectection module is enabled.
 */
bool mpu_get_motion_feature_enabled(mpu_handle_t * mpu)
{
    uint8_t data;
    esp_err_t err = mpu_i2c_read_bits(mpu, MPU_R_ACCEL_CONFIG,
                                      MPU_R_ACONFIG_HPF_BIT,
                                      MPU_R_ACONFIG_HPF_LENGTH, &data);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    if (data != ACCEL_DHPF_HOLD) return false;
    dlpf_t kDLPF = DLPF_256HZ_NOLPF;
    dlpf_t dlpf = mpu_get_dlp_filter(mpu);
    if (MPU_ERR_CHECK(mpu->last_err)) (void) 0;

    if (dlpf != kDLPF) return false;
    return true;
}

/*******************************************************************************
 * @brief Configure Motion-Detect or Wake-on-motion feature.
 *
 * @details The behaviour of this feature is very different between the MPU6050 (MPU9150) and the
 * MPU6500 (MPU9250). Each chip's version of this feature is explained below.
 *
 * [MPU6050, MPU6000, MPU9150]:
 * Accelerometer measurements are passed through a configurable digital high pass filter (DHPF)
 * in order to eliminate bias due to gravity. A qualifying motion sample is one where the high
 * passed sample from any axis has an absolute value exceeding a user-programmable threshold. A
 * counter increments for each qualifying sample, and decrements for each non-qualifying sample.
 * Once the counter reaches a user-programmable counter threshold, a motion interrupt is triggered.
 * The axis and polarity which caused the interrupt to be triggered is flagged in the
 * MOT_DETECT_STATUS register.
 *
 * [MPU6500, MPU9250]:
 * Unlike the MPU6050 version, the hardware does not "lock in" a reference sample.
 * The hardware monitors the accel data and detects any large change over a short period of time.
 * A qualifying motion sample is one where the high passed sample from any axis has
 * an absolute value exceeding the threshold.
 * The hardware motion threshold can be between 4mg and 1020mg in 4mg increments.
 *
 * @note
 * It is possible to enable **wake-on-motion** mode by doing the following:
 *  1. Enter Low Power Accelerometer mode with setLowPowerAccelMode();
 *  2. Select the wake-up rate with setLowPowerAccelRate();
 *  3. Configure motion-detect interrupt with setMotionDetectConfig();
 *  4. Enable the motion detection module with setMotionFeatureEnabled();
 * */
esp_err_t mpu_set_motion_detect_config(mpu_handle_t * mpu, mot_config_t* config)
{
    esp_err_t err;
#if defined CONFIG_MPU_6050
    err = mpu_i2c_write_byte(mpu, MPU_R_MOTION_DUR, config->time);
    if (MPU_ERR_CHECK(err)) return err;

    err = mpu_i2c_write_bits(mpu, MPU_R_MOTION_DETECT_CTRL,
                             MPU_R_MOTCTRL_ACCEL_ON_DELAY_BIT,
                             MPU_R_MOTCTRL_ACCEL_ON_DELAY_LENGTH, config->accel_on_delay);
    if (MPU_ERR_CHECK(err)) return err;

    err = mpu_i2c_write_bits(mpu, MPU_R_MOTION_DETECT_CTRL,
                             MPU_R_MOTCTRL_MOT_COUNT_BIT, MPU_R_MOTCTRL_MOT_COUNT_LENGTH,
                             config->counter);
    if (MPU_ERR_CHECK(err)) return err;
#endif
    err = mpu_i2c_write_byte(mpu, MPU_R_MOTION_THR, config->threshold);
    if (MPU_ERR_CHECK(err)) return err;

    return err;
}

/*******************************************************************************
 * @brief Return Motion Detection Configuration.
 */
mot_config_t mpu_get_motion_detect_config(mpu_handle_t * mpu)
{
    mot_config_t config = {0};
    esp_err_t err;
    uint8_t* buffer = mpu->buffer;

#if defined CONFIG_MPU_6050
    err = mpu_i2c_read_byte(mpu, MPU_R_MOTION_DUR, &config.time);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;
    err = mpu_i2c_read_byte(mpu, MPU_R_MOTION_DETECT_CTRL, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    config.accel_on_delay =
        (buffer[0] >> (MPU_R_MOTCTRL_ACCEL_ON_DELAY_BIT -
                       MPU_R_MOTCTRL_ACCEL_ON_DELAY_LENGTH + 1)) & 0x3;
    config.counter =
        (mot_counter_t)((buffer[0] >> (MPU_R_MOTCTRL_MOT_COUNT_BIT -
                                       MPU_R_MOTCTRL_MOT_COUNT_LENGTH + 1)) & 0x3);
#endif

    err = mpu_i2c_read_byte(mpu, MPU_R_MOTION_THR, &config.threshold);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    return config;
}

#if defined CONFIG_MPU_6050
/*******************************************************************************
 * @brief Configure Zero-Motion.
 *
 * @details The Zero Motion detection capability uses the digital high pass filter (DHPF) and a similar
 * threshold scheme to that of Free Fall detection. Each axis of the high passed accelerometer
 * measurement must have an absolute value less than a threshold specified in the ZRMOT_THR
 * register, which can be increased in 1 mg increments. Each time a motion sample meets this
 * condition, a counter increments. When this counter reaches a threshold specified in ZRMOT_DUR, an
 * interrupt is generated.
 *
 * Unlike Free Fall or Motion detection, Zero Motion detection triggers an interrupt both when Zero
 * Motion is first detected and when Zero Motion is no longer detected. While Free Fall and Motion
 * are indicated with a flag which clears after being read, reading the state of the Zero Motion
 * detected from the MOT_DETECT_STATUS register does not clear its status.
 *
 * @note Enable by calling setMotionFeatureEnabled();
 * */
esp_err_t mpu_set_zero_motion_config(mpu_handle_t * mpu, zrmot_config_t* config)
{
    uint8_t* buffer = mpu->buffer;
    buffer[0] = config->threshold;
    buffer[1] = config->time;

    esp_err_t err = mpu_i2c_write_bytes(mpu, MPU_R_ZRMOTION_THR, 2, buffer);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Return Zero-Motion configuration.
 */
zrmot_config_t mpu_get_zero_motion_config(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bytes(mpu, MPU_R_ZRMOTION_THR, 2, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    zrmot_config_t config = {0};
    config.threshold = buffer[0];
    config.time      = buffer[1];

    return config;
}

/*******************************************************************************
 * @brief Configure Free-Fall.
 *
 * @details Free fall is detected by checking if the accelerometer measurements from all 3 axes have an
 * absolute value below a user-programmable threshold (acceleration threshold). For each sample
 * where this condition is true (a qualifying sample), a counter is incremented. For each sample
 * where this condition is false (a non- qualifying sample), the counter is decremented. Once the
 * counter reaches a user-programmable threshold (the counter threshold), the Free Fall interrupt is
 * triggered and a flag is set. The flag is cleared once the counter has decremented to zero. The
 * counter does not increment above the counter threshold or decrement below zero.
 *
 * @note Enable by calling setMotionFeatureEnabled().
 * */
esp_err_t mpu_set_free_fall_config(mpu_handle_t * mpu, ff_config_t* config)
{
    uint8_t* buffer = mpu->buffer;
    buffer[0] = config->threshold;
    buffer[1] = config->time;

    esp_err_t err = mpu_i2c_write_bytes(mpu, MPU_R_FF_THR, 2, buffer);
    if (MPU_ERR_CHECK(err)) return err;

    err = mpu_i2c_write_bits(mpu, MPU_R_MOTION_DETECT_CTRL,
                             MPU_R_MOTCTRL_ACCEL_ON_DELAY_BIT,
                             MPU_R_MOTCTRL_ACCEL_ON_DELAY_LENGTH, config->accel_on_delay);
    if (MPU_ERR_CHECK(err)) return err;

    err = mpu_i2c_write_bits(mpu, MPU_R_MOTION_DETECT_CTRL,
                             MPU_R_MOTCTRL_MOT_COUNT_BIT,
                             MPU_R_MOTCTRL_MOT_COUNT_LENGTH, config->counter);
    if (MPU_ERR_CHECK(err)) return err;

    return err;
}

/*******************************************************************************
 * @brief Return Free-Fall Configuration.
 */
ff_config_t mpu_get_free_fall_config(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    ff_config_t config = {0};

    esp_err_t err = mpu_i2c_read_bytes(mpu, MPU_R_FF_THR, 2, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    config.threshold = buffer[0];
    config.time      = buffer[1];

    err = mpu_i2c_read_byte(mpu, MPU_R_MOTION_DETECT_CTRL, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    config.accel_on_delay =
        (buffer[0] >> (MPU_R_MOTCTRL_ACCEL_ON_DELAY_BIT -
                       MPU_R_MOTCTRL_ACCEL_ON_DELAY_LENGTH + 1)) & 0x3;
    config.counter =
        (mot_counter_t)((buffer[0] >> (MPU_R_MOTCTRL_MOT_COUNT_BIT -
                                       MPU_R_MOTCTRL_MOT_COUNT_LENGTH + 1)) & 0x3);
    return config;
}

/*******************************************************************************
 * @brief Return Motion Detection Status.
 * @note Reading this register clears all motion detection status bits.
 * */
mot_stat_t mpu_get_motion_detect_status(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_byte(mpu, MPU_R_MOTION_DETECT_STATUS, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    return (mot_stat_t) buffer[0];
}

#endif  // MPU6050's stuff

// #### ##    ## ######## ######## ########
//  ##  ###   ##    ##    ##       ##     ##
//  ##  ####  ##    ##    ##       ##     ##
//  ##  ## ## ##    ##    ######   ########
//  ##  ##  ####    ##    ##       ##   ##
//  ##  ##   ###    ##    ##       ##    ##
// #### ##    ##    ##    ######## ##     ##  Interrupt


/*******************************************************************************
 * @brief Configure the Interrupt pin (INT).
 * @param config configuration desired.
 */
esp_err_t mpu_setInterruptConfig(mpu_handle_t * mpu, int_config_t config)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_byte(mpu, MPU_R_INT_PIN_CONFIG, buffer);
    if (MPU_ERR_CHECK(err)) return err;

    // zero the bits we're setting, but keep the others we're not setting as they are;
    uint8_t INT_PIN_CONFIG_BITMASK = (1 << MPU_R_INT_CFG_LEVEL_BIT) |
                                     (1 << MPU_R_INT_CFG_OPEN_BIT) |
                                     (1 << MPU_R_INT_CFG_LATCH_EN_BIT) |
                                     (1 << MPU_R_INT_CFG_ANYRD_2CLEAR_BIT);
    buffer[0] &= ~INT_PIN_CONFIG_BITMASK;
    // set the configurations
    buffer[0] |= config.level << MPU_R_INT_CFG_LEVEL_BIT;
    buffer[0] |= config.drive << MPU_R_INT_CFG_OPEN_BIT;
    buffer[0] |= config.mode << MPU_R_INT_CFG_LATCH_EN_BIT;
    buffer[0] |= config.clear << MPU_R_INT_CFG_ANYRD_2CLEAR_BIT;
    err = mpu_i2c_write_byte(mpu, MPU_R_INT_PIN_CONFIG, buffer[0]);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Return Interrupt pin (INT) configuration.
 */
int_config_t mpu_get_interrupt_config(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_byte(mpu, MPU_R_INT_PIN_CONFIG, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    int_config_t config = {0};
    config.level = (int_lvl_t)((buffer[0] >> MPU_R_INT_CFG_LEVEL_BIT) & 0x1);
    config.drive = (int_drive_t)((buffer[0] >> MPU_R_INT_CFG_OPEN_BIT) & 0x1);
    config.mode  = (int_mode_t)((buffer[0] >> MPU_R_INT_CFG_LATCH_EN_BIT) & 0x1);
    config.clear = (int_clear_t)((buffer[0] >> MPU_R_INT_CFG_ANYRD_2CLEAR_BIT) &
                                 0x1);
    return config;
}

/*******************************************************************************
 * @brief Enable features to generate signal at Interrupt pin
 * @param mask ORed features.
 */
esp_err_t mpu_set_interrupt_enabled(mpu_handle_t * mpu, int_en_t mask)
{
    esp_err_t err = mpu_i2c_write_byte(mpu, MPU_R_INT_ENABLE, mask);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Return enabled features configured to generate signal at Interrupt pin.
 */
int_en_t mpu_get_interrupt_enabled(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_byte(mpu, MPU_R_INT_ENABLE, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    return (int_en_t) buffer[0];
}

/*******************************************************************************
 * @brief Return the Interrupt status from INT_STATUS register.
 * @note Reading this register, clear all bits.
 */
int_stat_t mpu_get_interrupt_status(mpu_handle_t * mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_byte(mpu, MPU_R_INT_STATUS, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;

    return (int_stat_t) buffer[0];
}

// ######## #### ########  #######
// ##        ##  ##       ##     ##
// ##        ##  ##       ##     ##
// ######    ##  ######   ##     ##
// ##        ##  ##       ##     ##
// ##        ##  ##       ##     ##
// ##       #### ##        #######

/*******************************************************************************
 * @brief Change FIFO mode.
 *
 * Options:
 * `FIFO_MODE_OVERWRITE`: When the fifo is full, additional writes will be
 *  written to the fifo,replacing the oldest data.
 * `FIFO_MODE_STOP_FULL`: When the fifo is full, additional writes will not be written to fifo.
 * */
esp_err_t mpu_set_fifo_mode(mpu_handle_t* mpu, fifo_mode_t mode)
{
    esp_err_t err = mpu_i2c_write_bit(mpu, MPU_R_CONFIG,
                                      MPU_R_CONFIG_FIFO_MODE_BIT, mode);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Return FIFO mode.
 */
fifo_mode_t mpu_get_fifo_mode(mpu_handle_t* mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bit(mpu, MPU_R_CONFIG, MPU_R_CONFIG_FIFO_MODE_BIT,
                                     buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;
    return (fifo_mode_t) buffer[0];
}

/*******************************************************************************
 * @brief Configure the sensors that will be written to the FIFO.
 * */
esp_err_t mpu_set_fifo_config(mpu_handle_t* mpu, fifo_config_t config)
{
    esp_err_t err = mpu_i2c_write_byte(mpu, MPU_R_FIFO_EN, (uint8_t) config);
    if (MPU_ERR_CHECK(err)) return err;

    err = mpu_i2c_write_bit(mpu, MPU_R_I2C_MST_CTRL,
                            MPU_R_I2CMST_CTRL_SLV_3_FIFO_EN_BIT, config >> 8);

    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Return FIFO configuration.
 */
fifo_config_t mpu_get_fifo_config(mpu_handle_t* mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bytes(mpu, MPU_R_FIFO_EN, 2, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;
    fifo_config_t config = buffer[0];
    config |= (buffer[1] & (1 << MPU_R_I2CMST_CTRL_SLV_3_FIFO_EN_BIT)) << 3;
    return config;
}

/*******************************************************************************
 * @brief Enabled / disable FIFO module.
 * */
esp_err_t mpu_set_fifo_enabled(mpu_handle_t* mpu, bool enable)
{
    esp_err_t err = mpu_i2c_write_bit(mpu, MPU_R_USER_CTRL,
                                      MPU_R_USERCTRL_FIFO_EN_BIT, (uint8_t) enable);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Return FIFO module state.
 */
bool mpu_get_fifo_enabled(mpu_handle_t* mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bit(mpu, MPU_R_USER_CTRL,
                                     MPU_R_USERCTRL_FIFO_EN_BIT,
                                     buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;
    return buffer[0];
}

/*******************************************************************************
 * @brief Reset FIFO module.
 *
 * Zero FIFO count, reset is asynchronous. \n
 * The bit auto clears after one clock cycle.
 * */
esp_err_t mpu_reset_fifo(mpu_handle_t* mpu)
{
    esp_err_t err = mpu_i2c_write_bit(mpu, MPU_R_USER_CTRL,
                                      MPU_R_USERCTRL_FIFO_RESET_BIT, 1);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Return number of written bytes in the FIFO.
 * @note FIFO overflow generates an interrupt which can be check with getInterruptStatus().
 * */
uint16_t mpu_get_fifo_count(mpu_handle_t* mpu)
{
    uint8_t* buffer = mpu->buffer;
    esp_err_t err = mpu_i2c_read_bytes(mpu, MPU_R_FIFO_COUNT_H, 2, buffer);
    if (MPU_ERR_CHECK(err)) mpu->last_err = err;
    uint16_t count = buffer[0] << 8 | buffer[1];
    return count;
}

/*******************************************************************************
 * @brief Read data contained in FIFO buffer.
 * */
esp_err_t mpu_read_fifo(mpu_handle_t* mpu, size_t length, uint8_t* data)
{
    esp_err_t err = mpu_i2c_read_bytes(mpu, MPU_R_FIFO_R_W, length, data);
    return MPU_ERR_CHECK(err);
}

/*******************************************************************************
 * @brief Write data to FIFO buffer.
 * */
esp_err_t mpu_write_fifo(mpu_handle_t* mpu, size_t length, const uint8_t* data)
{
    esp_err_t err = mpu_i2c_write_bytes(mpu, MPU_R_FIFO_R_W, length, data);
    return MPU_ERR_CHECK(err);
}


//  ######         ######## ########  ######  ########
// ##    ##           ##    ##       ##    ##    ##
// ##                 ##    ##       ##          ##
//  ######            ##    ######    ######     ##
//       ##           ##    ##             ##    ##
// ##    ## ###       ##    ##       ##    ##    ##
//  ######  ###       ##    ########  ######     ##

// /**
//  * @brief Trigger gyro and accel hardware self-test.
//  * @attention when calling this function, the MPU must remain as horizontal as possible (0 degrees), facing up.
//  * @param result Should be ZERO if gyro and accel passed.
//  * @todo Elaborate doc.
//  * */
// esp_err_t MPU::selfTest(selftest_t* result)
// {
// #ifdef CONFIG_MPU6050
//     constexpr accel_fs_t kAccelFS = ACCEL_FS_16G;
//     constexpr gyro_fs_t kGyroFS   = GYRO_FS_250DPS;
// #elif CONFIG_MPU6500
//     constexpr accel_fs_t kAccelFS = ACCEL_FS_2G;
//     constexpr gyro_fs_t kGyroFS   = GYRO_FS_250DPS;
// #endif
//     raw_axes_t gyroRegBias, accelRegBias;
//     raw_axes_t gyroSTBias, accelSTBias;
//     // get regular biases
//     if (MPU_ERR_CHECK(getBiases(kAccelFS, kGyroFS, &accelRegBias, &gyroRegBias, false))) return err;
//     // get self-test biases
//     if (MPU_ERR_CHECK(getBiases(kAccelFS, kGyroFS, &accelSTBias, &gyroSTBias, true))) return err;
//     // perform self-tests
//     uint8_t accelST, gyroST;
//     if (MPU_ERR_CHECK(accelSelfTest(accelRegBias, accelSTBias, &accelST))) return err;
//     if (MPU_ERR_CHECK(gyroSelfTest(gyroRegBias, gyroSTBias, &gyroST))) return err;
//     // check results
//     *result = 0;
//     if (accelST != 0) *result |= SELF_TEST_ACCEL_FAIL;
//     if (gyroST != 0) *result |= SELF_TEST_GYRO_FAIL;
//     return err;
// }

// #if defined CONFIG_MPU6500
// // Production Self-Test table for MPU6500 based models,
// // used in accel and gyro self-test code below.
// static constexpr uint16_t kSelfTestTable[256] = {
//     2620,  2646,  2672,  2699,  2726,  2753,  2781,  2808,   // 7
//     2837,  2865,  2894,  2923,  2952,  2981,  3011,  3041,   // 15
//     3072,  3102,  3133,  3165,  3196,  3228,  3261,  3293,   // 23
//     3326,  3359,  3393,  3427,  3461,  3496,  3531,  3566,   // 31
//     3602,  3638,  3674,  3711,  3748,  3786,  3823,  3862,   // 39
//     3900,  3939,  3979,  4019,  4059,  4099,  4140,  4182,   // 47
//     4224,  4266,  4308,  4352,  4395,  4439,  4483,  4528,   // 55
//     4574,  4619,  4665,  4712,  4759,  4807,  4855,  4903,   // 63
//     4953,  5002,  5052,  5103,  5154,  5205,  5257,  5310,   // 71
//     5363,  5417,  5471,  5525,  5581,  5636,  5693,  5750,   // 79
//     5807,  5865,  5924,  5983,  6043,  6104,  6165,  6226,   // 87
//     6289,  6351,  6415,  6479,  6544,  6609,  6675,  6742,   // 95
//     6810,  6878,  6946,  7016,  7086,  7157,  7229,  7301,   // 103
//     7374,  7448,  7522,  7597,  7673,  7750,  7828,  7906,   // 111
//     7985,  8065,  8145,  8227,  8309,  8392,  8476,  8561,   // 119
//     8647,  8733,  8820,  8909,  8998,  9088,  9178,  9270,   //
//     9363,  9457,  9551,  9647,  9743,  9841,  9939,  10038,  //
//     10139, 10240, 10343, 10446, 10550, 10656, 10763, 10870,  //
//     10979, 11089, 11200, 11312, 11425, 11539, 11654, 11771,  //
//     11889, 12008, 12128, 12249, 12371, 12495, 12620, 12746,  //
//     12874, 13002, 13132, 13264, 13396, 13530, 13666, 13802,  //
//     13940, 14080, 14221, 14363, 14506, 14652, 14798, 14946,  //
//     15096, 15247, 15399, 15553, 15709, 15866, 16024, 16184,  //
//     16346, 16510, 16675, 16842, 17010, 17180, 17352, 17526,  //
//     17701, 17878, 18057, 18237, 18420, 18604, 18790, 18978,  //
//     19167, 19359, 19553, 19748, 19946, 20145, 20347, 20550,  //
//     20756, 20963, 21173, 21385, 21598, 21814, 22033, 22253,  //
//     22475, 22700, 22927, 23156, 23388, 23622, 23858, 24097,  //
//     24338, 24581, 24827, 25075, 25326, 25579, 25835, 26093,  //
//     26354, 26618, 26884, 27153, 27424, 27699, 27976, 28255,  //
//     28538, 28823, 29112, 29403, 29697, 29994, 30294, 30597,  //
//     30903, 31212, 31524, 31839, 32157, 32479, 32804, 33132   //
// };
// #endif

// /**
//  * @brief Accel Self-test.
//  * @param result self-test error for each axis (X=bit0, Y=bit1, Z=bit2). Zero is a pass.
//  * @note Bias should be in 16G format for MPU6050 and 2G for MPU6500 based models.
//  * */
// esp_err_t MPU::accelSelfTest(raw_axes_t& regularBias, raw_axes_t& selfTestBias, uint8_t* result)
// {
// #if defined CONFIG_MPU6050
//     constexpr accel_fs_t kAccelFS = ACCEL_FS_16G;
//     // Criteria A: must be within 14% variation
//     constexpr float kMaxVariation = .14f;
//     // Criteria B: must be between 300 mg and 950 mg
//     constexpr float kMinGravity = .3f, kMaxGravity = .95f;

// #elif defined CONFIG_MPU6500
//     constexpr accel_fs_t kAccelFS = ACCEL_FS_2G;
//     // Criteria A: must be within 50% variation
//     constexpr float kMaxVariation = .5f;
//     // Criteria B: must be between 255 mg and 675 mg
//     constexpr float kMinGravity = .225f, kMaxGravity = .675f;
//     // Criteria C: 500 mg for accel
//     constexpr float kMaxGravityOffset = .5f;
// #endif

//     /* Convert biases */
//     float_axes_t regularBiasGravity  = math::accelGravity(regularBias, kAccelFS);
//     float_axes_t selfTestBiasGravity = math::accelGravity(selfTestBias, kAccelFS);
//     MPU_LOGVMSG(msgs::EMPTY, "regularBias: %+d %+d %+d | regularBiasGravity: %+.2f %+.2f %+.2f", regularBias.x,
//                 regularBias.y, regularBias.z, regularBiasGravity.x, regularBiasGravity.y, regularBiasGravity.z);
//     MPU_LOGVMSG(msgs::EMPTY, "selfTestBias: %+d %+d %+d | selfTestBiasGravity: %+.2f %+.2f %+.2f", selfTestBias.x,
//                 selfTestBias.y, selfTestBias.z, selfTestBiasGravity.x, selfTestBiasGravity.y, selfTestBiasGravity.z);

//     /* Get OTP production shift code */
//     uint8_t shiftCode[3];
// #if defined CONFIG_MPU6050
//     if (MPU_ERR_CHECK(readBytes(MPU_R_SELF_TEST_X, 4, buffer))) return err;
//     shiftCode[0] = ((buffer[0] & 0xE0) >> 3) | ((buffer[3] & 0x30) >> 4);
//     shiftCode[1] = ((buffer[1] & 0xE0) >> 3) | ((buffer[3] & 0x0C) >> 2);
//     shiftCode[2] = ((buffer[2] & 0xE0) >> 3) | (buffer[3] & 0x03);

// #elif defined CONFIG_MPU6500
//     if (MPU_ERR_CHECK(readBytes(MPU_R_SELF_TEST_X_ACCEL, 3, shiftCode))) return err;
// #endif
//     MPU_LOGVMSG(msgs::EMPTY, "shiftCode: %+d %+d %+d", shiftCode[0], shiftCode[1], shiftCode[2]);

//     /* Calulate production shift value */
//     float shiftProduction[3] = {0};
//     for (int i = 0; i < 3; i++) {
//         if (shiftCode[i] != 0) {
// #if defined CONFIG_MPU6050
//             // Equivalent to.. shiftProduction[i] = 0.34f * powf(0.92f/0.34f, (shiftCode[i]-1)
//             // / 30.f)
//             shiftProduction[i] = 0.34f;
//             while (--shiftCode[i]) shiftProduction[i] *= 1.034f;

// #elif defined CONFIG_MPU6500
//             shiftProduction[i] = kSelfTestTable[shiftCode[i] - 1];
//             shiftProduction[i] /= math::accelSensitivity(ACCEL_FS_2G);
// #endif
//         }
//     }
//     MPU_LOGVMSG(msgs::EMPTY, "shiftProduction: %+.2f %+.2f %+.2f", shiftProduction[0], shiftProduction[1],
//                 shiftProduction[2]);

//     /* Evaluate criterias */
//     *result                 = 0;
//     float shiftResponse[3]  = {0};
//     float shiftVariation[3] = {0};
//     for (int i = 0; i < 3; i++) {
//         shiftResponse[i] = fabs(selfTestBiasGravity[i] - regularBiasGravity[i]);
//         // Criteria A
//         if (shiftProduction[i] != 0) {
//             shiftVariation[i] = shiftResponse[i] / shiftProduction[i] - 1;
//             if (fabs(shiftVariation[i]) > kMaxVariation) *result |= 1 << i;
//             // Criteria B
//         }
//         else if (shiftResponse[i] < kMinGravity || shiftResponse[i] > kMaxGravity) {
//             *result |= 1 << i;
//         }
// // Criteria C
// #if defined CONFIG_MPU6050
//             // no criteria C
// #elif defined CONFIG_MPU6500
//         if (fabs(regularBiasGravity[i] > kMaxGravityOffset)) *result |= 1 << i;
// #endif
//     }
//     MPU_LOGVMSG(msgs::EMPTY, "shiftResponse: %+.2f %+.2f %+.2f", shiftResponse[0], shiftResponse[1], shiftResponse[2]);
//     MPU_LOGVMSG(msgs::EMPTY, "shiftVariation: %+.2f %+.2f %+.2f", shiftVariation[0], shiftVariation[1],
//                 shiftVariation[2]);

//     MPU_LOGD("Accel self-test: [X=%s] [Y=%s] [Z=%s]", ((*result & 0x1) ? "FAIL" : "OK"),
//              ((*result & 0x2) ? "FAIL" : "OK"), ((*result & 0x4) ? "FAIL" : "OK"));
//     return err;
// }

// /**
//  * @brief Gyro Self-test.
//  * @param result Self-test error for each axis (X=bit0, Y=bit1, Z=bit2). Zero is a pass.
//  * @note Bias should be in 250DPS format for both MPU6050 and MPU6500 based models.
//  * */
// esp_err_t MPU::gyroSelfTest(raw_axes_t& regularBias, raw_axes_t& selfTestBias, uint8_t* result)
// {
//     constexpr gyro_fs_t kGyroFS = GYRO_FS_250DPS;

// #if defined CONFIG_MPU6050
//     // Criteria A: must not exceed +14% variation
//     constexpr float kMaxVariation = .14f;
//     // Criteria B: must be between 10 dps and 105 dps
//     constexpr float kMinDPS = 10.f, kMaxDPS = 105.f;

// #elif defined CONFIG_MPU6500
//     // Criteria A: must be within 50% variation
//     constexpr float kMaxVariation = .5f;
//     // Criteria B: must be between 20 dps and 60 dps
//     constexpr float kMinDPS = 20.f, kMaxDPS = 60.f;
// #endif

//     /* Convert biases */
//     float_axes_t regularBiasDPS  = math::gyroDegPerSec(regularBias, kGyroFS);
//     float_axes_t selfTestBiasDPS = math::gyroDegPerSec(selfTestBias, kGyroFS);
//     MPU_LOGVMSG(msgs::EMPTY, "regularBias: %+d %+d %+d | regularBiasDPS: %+.2f %+.2f %+.2f", regularBias.x,
//                 regularBias.y, regularBias.z, regularBiasDPS.x, regularBiasDPS.y, regularBiasDPS.z);
//     MPU_LOGVMSG(msgs::EMPTY, "selfTestBias: %+d %+d %+d | selfTestBiasDPS: %+.2f %+.2f %+.2f", selfTestBias.x,
//                 selfTestBias.y, selfTestBias.z, selfTestBiasDPS.x, selfTestBiasDPS.y, selfTestBiasDPS.z);

//     /* Get OTP production shift code */
//     uint8_t shiftCode[3];
// #if defined CONFIG_MPU6050
//     if (MPU_ERR_CHECK(readBytes(MPU_R_SELF_TEST_X, 3, buffer))) return err;
//     shiftCode[0] = buffer[0] & 0x1F;
//     shiftCode[1] = buffer[1] & 0x1F;
//     shiftCode[2] = buffer[2] & 0x1F;

// #elif defined CONFIG_MPU6500
//     if (MPU_ERR_CHECK(readBytes(MPU_R_SELF_TEST_X_GYRO, 3, shiftCode))) return err;
// #endif
//     MPU_LOGVMSG(msgs::EMPTY, "shiftCode: %+d %+d %+d", shiftCode[0], shiftCode[1], shiftCode[2]);

//     /* Calulate production shift value */
//     float shiftProduction[3] = {0};
//     for (int i = 0; i < 3; i++) {
//         if (shiftCode[i] != 0) {
// #if defined CONFIG_MPU6050
//             shiftProduction[i] = 3275.f / math::gyroSensitivity(kGyroFS);  // should yield 25
//             while (--shiftCode[i]) shiftProduction[i] *= 1.046f;

// #elif defined CONFIG_MPU6500
//             shiftProduction[i] = kSelfTestTable[shiftCode[i] - 1];
//             shiftProduction[i] /= math::gyroSensitivity(kGyroFS);
// #endif
//         }
//     }
//     MPU_LOGVMSG(msgs::EMPTY, "shiftProduction: %+.2f %+.2f %+.2f", shiftProduction[0], shiftProduction[1],
//                 shiftProduction[2]);

//     /* Evaluate criterias */
//     *result                 = 0;
//     float shiftResponse[3]  = {0};
//     float shiftVariation[3] = {0};
//     for (int i = 0; i < 3; i++) {
//         shiftResponse[i] = fabs(selfTestBiasDPS[i] - regularBiasDPS[i]);
//         // Criteria A
//         if (shiftProduction[i] != 0) {
//             shiftVariation[i] = shiftResponse[i] / shiftProduction[i] - 1;
//             if (fabs(shiftVariation[i]) > kMaxVariation) *result |= 1 << i;
//             // Criteria B
//         }
//         else if (shiftResponse[i] < kMinDPS || shiftResponse[i] > kMaxDPS) {
//             *result |= 1 << i;
//         }
//     }
//     MPU_LOGVMSG(msgs::EMPTY, "shiftResponse: %+.2f %+.2f %+.2f", shiftResponse[0], shiftResponse[1], shiftResponse[2]);
//     MPU_LOGVMSG(msgs::EMPTY, "shiftVariation: %+.2f %+.2f %+.2f", shiftVariation[0], shiftVariation[1],
//                 shiftVariation[2]);

//     MPU_LOGD("Gyro self-test: [X=%s] [Y=%s] [Z=%s]", ((*result & 0x1) ? "FAIL" : "OK"),
//              ((*result & 0x2) ? "FAIL" : "OK"), ((*result & 0x4) ? "FAIL" : "OK"));
//     return err;
// }

// /**
//  * @brief Compute the Biases in regular mode and self-test mode.
//  * @attention When calculating the biases the MPU must remain as horizontal as possible (0 degrees), facing up.
//  * This algorithm takes about ~400ms to compute offsets.
//  * */
// esp_err_t MPU::getBiases(accel_fs_t accelFS, gyro_fs_t gyroFS, raw_axes_t* accelBias, raw_axes_t* gyroBias,
//                          bool selftest)
// {
//     // configurations to compute biases
//     constexpr uint16_t kSampleRate      = 1000;
//     constexpr dlpf_t kDLPF              = DLPF_188HZ;
//     constexpr fifo_config_t kFIFOConfig = FIFO_CFG_ACCEL | FIFO_CFG_GYRO;
//     constexpr size_t kPacketSize        = 12;
//     // backup previous configuration
//     const uint16_t prevSampleRate      = getSampleRate();
//     const dlpf_t prevDLPF              = getDigitalLowPassFilter();
//     const accel_fs_t prevAccelFS       = getAccelFullScale();
//     const gyro_fs_t prevGyroFS         = getGyroFullScale();
//     const fifo_config_t prevFIFOConfig = getFIFOConfig();
//     const bool prevFIFOState           = getFIFOEnabled();
//     // setup
//     if (MPU_ERR_CHECK(setSampleRate(kSampleRate))) return err;
//     if (MPU_ERR_CHECK(setDigitalLowPassFilter(kDLPF))) return err;
//     if (MPU_ERR_CHECK(setAccelFullScale(accelFS))) return err;
//     if (MPU_ERR_CHECK(setGyroFullScale(gyroFS))) return err;
//     if (MPU_ERR_CHECK(setFIFOConfig(kFIFOConfig))) return err;
//     if (MPU_ERR_CHECK(setFIFOEnabled(true))) return err;
//     if (selftest) {
//         if (MPU_ERR_CHECK(writeBits(MPU_R_ACCEL_CONFIG, MPU_R_ACONFIG_XA_ST_BIT, 3, 0x7))) {
//             return err;
//         }
//         if (MPU_ERR_CHECK(writeBits(MPU_R_GYRO_CONFIG, MPU_R_GCONFIG_XG_ST_BIT, 3, 0x7))) {
//             return err;
//         }
//     }
//     // wait for 200ms for sensors to stabilize
//     vTaskDelay(200 / portTICK_PERIOD_MS);
//     // fill FIFO for 100ms
//     if (MPU_ERR_CHECK(resetFIFO())) return err;
//     vTaskDelay(100 / portTICK_PERIOD_MS);
//     if (MPU_ERR_CHECK(setFIFOConfig(FIFO_CFG_NONE))) return err;
//     // get FIFO count
//     const uint16_t fifoCount = getFIFOCount();
//     if (MPU_ERR_CHECK(lastError())) return err;
//     const int packetCount = fifoCount / kPacketSize;
//     // read overrun bytes, if any
//     const int overrunCount      = fifoCount - (packetCount * kPacketSize);
//     uint8_t buffer[kPacketSize] = {0};
//     if (overrunCount > 0) {
//         if (MPU_ERR_CHECK(readFIFO(overrunCount, buffer))) return err;
//     }
//     // fetch data and add up
//     axes_t<int> accelAvg, gyroAvg;
//     for (int i = 0; i < packetCount; i++) {
//         if (MPU_ERR_CHECK(readFIFO(kPacketSize, buffer))) return err;
//         // retrieve data
//         raw_axes_t accelCur, gyroCur;
//         accelCur.x = (buffer[0] << 8) | buffer[1];
//         accelCur.y = (buffer[2] << 8) | buffer[3];
//         accelCur.z = (buffer[4] << 8) | buffer[5];
//         gyroCur.x  = (buffer[6] << 8) | buffer[7];
//         gyroCur.y  = (buffer[8] << 8) | buffer[9];
//         gyroCur.z  = (buffer[10] << 8) | buffer[11];
//         // add up
//         accelAvg.x += accelCur.x;
//         accelAvg.y += accelCur.y;
//         accelAvg.z += accelCur.z;
//         gyroAvg.x += gyroCur.x;
//         gyroAvg.y += gyroCur.y;
//         gyroAvg.z += gyroCur.z;
//     }
//     // calculate average
//     accelAvg.x /= packetCount;
//     accelAvg.y /= packetCount;
//     accelAvg.z /= packetCount;
//     gyroAvg.x /= packetCount;
//     gyroAvg.y /= packetCount;
//     gyroAvg.z /= packetCount;
//     // remove gravity from Accel Z axis
//     const uint16_t gravityLSB = INT16_MAX >> (accelFS + 1);
//     accelAvg.z -= gravityLSB;
//     // save biases
//     for (int i = 0; i < 3; i++) {
//         (*accelBias)[i] = (int16_t) accelAvg[i];
//         (*gyroBias)[i]  = (int16_t) gyroAvg[i];
//     }
//     // set back previous configs
//     if (MPU_ERR_CHECK(setSampleRate(prevSampleRate))) return err;
//     if (MPU_ERR_CHECK(setDigitalLowPassFilter(prevDLPF))) return err;
//     if (MPU_ERR_CHECK(setAccelFullScale(prevAccelFS))) return err;
//     if (MPU_ERR_CHECK(setGyroFullScale(prevGyroFS))) return err;
//     if (MPU_ERR_CHECK(setFIFOConfig(prevFIFOConfig))) return err;
//     if (MPU_ERR_CHECK(setFIFOEnabled(prevFIFOState))) return err;
//     return err;
// }