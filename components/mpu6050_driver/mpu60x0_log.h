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

#ifndef __mpu60x0_log_h__
#define __mpu60x0_log_h__

#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

// Note: declare TAG before include this header
// Note: include only in .cpp files from this library

#define MPU_LOGE(format, ...) ESP_LOGE(TAG, format, ##__VA_ARGS__);
#define MPU_LOGW(format, ...) ESP_LOGW(TAG, format, ##__VA_ARGS__); 
#define MPU_LOGI(format, ...) //ESP_LOGI(TAG, format, ##__VA_ARGS__); 
#define MPU_LOGD(format, ...) ESP_LOGD(TAG, format, ##__VA_ARGS__); 

#define MPU_LOGEMSG(msg, format, ...) MPU_LOGE("%s()-> %s" format, __FUNCTION__, msg, ##__VA_ARGS__)
#define MPU_LOGWMSG(msg, format, ...) MPU_LOGW("%s()-> %s" format, __FUNCTION__, msg, ##__VA_ARGS__)
#define MPU_LOGIMSG(msg, format, ...) MPU_LOGI("%s()-> %s" format, __FUNCTION__, msg, ##__VA_ARGS__)
#define MPU_LOGDMSG(msg, format, ...) MPU_LOGD("%s()-> %s" format, __FUNCTION__, msg, ##__VA_ARGS__)

#ifndef MPU_LOG_ON_ERROR
#define MPU_ERR_CHECK(x) (x)
#else
#define MPU_ERR_CHECK(x) error_check_logger(x, __ASSERT_FUNC, __LINE__, #x)
#endif

static inline esp_err_t error_check_logger(esp_err_t x, const char* func, const int line, const char* expr)
{
    if (x) MPU_LOGE("func:%s @ line:%d, expr:\"%s\", error:0x%X ", func, line, expr, x);
    return x;
}

static const char MPU_MSG_INVALID_ARG[]           = "Invalid Argument";
static const char MPU_MSG_INVALID_STATE[]         = "Invalid State";
static const char MPU_MSG_INVALID_LENGTH[]        = "Invalid length";
static const char MPU_MSG_INVALID_FIFO_RATE[]     = "Invalid FIFO rate";
static const char MPU_MSG_INVALID_SAMPLE_RATE[]   = "Invalid Sample rate";
static const char MPU_MSG_INVALID_TAP_THRESH[]    = "Invalid Tap threshold";
static const char MPU_MSG_DMP_LOAD_FAIL[]         = "Failed to load DMP firmware";
static const char MPU_MSG_DMP_NOT_LOADED[]        = "DMP firmware has not been loaded";
static const char MPU_MSG_UNKNOWN_DMP_CFG_STATE[] = "Unknown DMP config state";
static const char MPU_MSG_NO_AXIS_PASSED[]        = "No Axis passed";
static const char MPU_MSG_BANK_BOUNDARIES[]       = "Bank boundaries overpass";
static const char MPU_MSG_FIFO_CORRUPTION[]       = "FIFO Corruption. Quaternion data outside of the acceptable threshold";
static const char MPU_MSG_AUX_I2C_DISABLED[]      = "Auxiliary I2C is disabled";
static const char MPU_MSG_AUX_I2C_SLAVE_NACK[]    = "Auxiliary I2C Slave NACK";
static const char MPU_MSG_AUX_I2C_LOST_ARB[]      = "Auxiliary I2C Master loose abitraion of the bus";
static const char MPU_MSG_COMPASS_DISABLED[]      = "Compass is disabled";
static const char MPU_MSG_NOT_SUPPORTED[]         = "Not supported";
static const char MPU_MSG_TIMEOUT[]               = "Timeout";
static const char MPU_MSG_EMPTY[]                 = "";

#endif // ifndef __mpu60x0_log_h__