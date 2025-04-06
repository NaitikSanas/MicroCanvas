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

#ifndef __mpu60x0_registers_h__
#define __mpu60x0_registers_h__

#include "stdint.h"

/*******************************************************************************
 * MPU commom registers for all models
 ******************************************************************************/
#define MPU_R_XG_OFFSET_H  (uint8_t) 0x13
#define MPU_R_XG_OFFSET_L  (uint8_t) 0x14
#define MPU_R_YG_OFFSET_H  (uint8_t) 0x15
#define MPU_R_YG_OFFSET_L  (uint8_t) 0x16
#define MPU_R_ZG_OFFSET_H  (uint8_t) 0x17
#define MPU_R_ZG_OFFSET_L  (uint8_t) 0x18
#define MPU_R_SMPLRT_DIV   (uint8_t) 0x19  // [7:0]
//------------------------------------------------------------------------------
#define MPU_R_CONFIG                      (uint8_t) 0x1A
#define MPU_R_CONFIG_FIFO_MODE_BIT        (uint8_t) 6
#define MPU_R_CONFIG_EXT_SYNC_SET_BIT     (uint8_t) 5  // [5:3]
#define MPU_R_CONFIG_EXT_SYNC_SET_LENGTH  (uint8_t) 3
#define MPU_R_CONFIG_DLPF_CFG_BIT         (uint8_t) 2  // [2:0]
#define MPU_R_CONFIG_DLPF_CFG_LENGTH      (uint8_t) 3
//------------------------------------------------------------------------------
#define MPU_R_GYRO_CONFIG               (uint8_t) 0x1B
#define MPU_R_GCONFIG_XG_ST_BIT         (uint8_t) 7
#define MPU_R_GCONFIG_YG_ST_BIT         (uint8_t) 6
#define MPU_R_GCONFIG_ZG_ST_BIT         (uint8_t) 5
#define MPU_R_GCONFIG_FS_SEL_BIT        (uint8_t) 4  // [4:3]
#define MPU_R_GCONFIG_FS_SEL_LENGTH     (uint8_t) 2
#define MPU_R_GCONFIG_FCHOICE_B         (uint8_t) 1  // [1:0]
#define MPU_R_GCONFIG_FCHOICE_B_LENGTH  (uint8_t) 2
//------------------------------------------------------------------------------
#define MPU_R_ACCEL_CONFIG           (uint8_t) 0x1C
#define MPU_R_ACONFIG_XA_ST_BIT      (uint8_t) 7
#define MPU_R_ACONFIG_YA_ST_BIT      (uint8_t) 6
#define MPU_R_ACONFIG_ZA_ST_BIT      (uint8_t) 5
#define MPU_R_ACONFIG_FS_SEL_BIT     (uint8_t) 4  // [4:3]
#define MPU_R_ACONFIG_FS_SEL_LENGTH  (uint8_t) 2
#define MPU_R_ACONFIG_HPF_BIT        (uint8_t) 2  // [2:0]
#define MPU_R_ACONFIG_HPF_LENGTH     (uint8_t) 3
//------------------------------------------------------------------------------
#define MPU_R_FF_THR        (uint8_t) 0x1D
#define MPU_R_FF_DUR        (uint8_t) 0x1E
#define MPU_R_MOTION_THR    (uint8_t) 0x1F  // [7:0] // MPU9250_REG_WOM_THR
#define MPU_R_MOTION_DUR    (uint8_t) 0x20
#define MPU_R_ZRMOTION_THR  (uint8_t) 0x21
#define MPU_R_ZRMOTION_DUR  (uint8_t) 0x22
//------------------------------------------------------------------------------
#define MPU_R_FIFO_EN            (uint8_t) 0x23
#define MPU_R_FIFO_TEMP_EN_BIT   (uint8_t) 7
#define MPU_R_FIFO_XGYRO_EN_BIT  (uint8_t) 6
#define MPU_R_FIFO_YGYRO_EN_BIT  (uint8_t) 5
#define MPU_R_FIFO_ZGYRO_EN_BIT  (uint8_t) 4
#define MPU_R_FIFO_ACCEL_EN_BIT  (uint8_t) 3
#define MPU_R_FIFO_SLV_2_EN_BIT  (uint8_t) 2
#define MPU_R_FIFO_SLV_1_EN_BIT  (uint8_t) 1
#define MPU_R_FIFO_SLV_0_EN_BIT  (uint8_t) 0
//------------------------------------------------------------------------------
#define MPU_R_I2C_MST_CTRL                   (uint8_t) 0x24
#define MPU_R_I2CMST_CTRL_MULT_EN_BIT        (uint8_t) 7
#define MPU_R_I2CMST_CTRL_WAIT_FOR_ES_BIT    (uint8_t) 6
#define MPU_R_I2CMST_CTRL_SLV_3_FIFO_EN_BIT  (uint8_t) 5
#define MPU_R_I2CMST_CTRL_P_NSR_BIT          (uint8_t) 4
#define MPU_R_I2CMST_CTRL_CLOCK_BIT          (uint8_t) 3  // [3:0]
#define MPU_R_I2CMST_CTRL_CLOCK_LENGTH       (uint8_t) 4
//------------------------------------------------------------------------------
#define MPU_R_I2C_SLV0_ADDR      (uint8_t) 0x25
#define MPU_R_I2C_SLV_RNW_BIT    (uint8_t) 7  // same for all I2C_SLV registers
#define MPU_R_I2C_SLV_ID_BIT     (uint8_t) 6  // [6:0]
#define MPU_R_I2C_SLV_ID_LENGTH  (uint8_t) 7
//------------------------------------------------------------------------------
#define MPU_R_I2C_SLV0_REG  (uint8_t) 0x26  // [7:0]
//------------------------------------------------------------------------------
#define MPU_R_I2C_SLV0_CTRL        (uint8_t) 0x27
#define MPU_R_I2C_SLV_EN_BIT       (uint8_t) 7  // same for all I2C_SLV registers
#define MPU_R_I2C_SLV_BYTE_SW_BIT  (uint8_t) 6
#define MPU_R_I2C_SLV_REG_DIS_BIT  (uint8_t) 5
#define MPU_R_I2C_SLV_GRP_BIT      (uint8_t) 4
#define MPU_R_I2C_SLV_LEN_BIT      (uint8_t) 3  // [3:0]
#define MPU_R_I2C_SLV_LEN_LENGTH   (uint8_t) 4
//------------------------------------------------------------------------------
#define MPU_R_I2C_SLV1_ADDR  (uint8_t) 0x28  // see SLV0 for bit defines
#define MPU_R_I2C_SLV1_REG   (uint8_t) 0x29
#define MPU_R_I2C_SLV1_CTRL  (uint8_t) 0x2A
#define MPU_R_I2C_SLV2_ADDR  (uint8_t) 0x2B  // see SLV0 for bit defines
#define MPU_R_I2C_SLV2_REG   (uint8_t) 0x2C
#define MPU_R_I2C_SLV2_CTRL  (uint8_t) 0x2D
#define MPU_R_I2C_SLV3_ADDR  (uint8_t) 0x2E  // see SLV0 for bit defines
#define MPU_R_I2C_SLV3_REG   (uint8_t) 0x2F
#define MPU_R_I2C_SLV3_CTRL  (uint8_t) 0x30
#define MPU_R_I2C_SLV4_ADDR  (uint8_t) 0x31  // see SLV0 for bit defines
#define MPU_R_I2C_SLV4_REG   (uint8_t) 0x32
#define MPU_R_I2C_SLV4_DO    (uint8_t) 0x33  // [7:0]
//------------------------------------------------------------------------------
#define MPU_R_I2C_SLV4_CTRL              (uint8_t) 0x34
#define MPU_R_I2C_SLV4_EN_BIT            (uint8_t) 7
#define MPU_R_I2C_SLV4_DONE_INT_BIT      (uint8_t) 6
#define MPU_R_I2C_SLV4_REG_DIS_BIT       (uint8_t) 5
#define MPU_R_I2C_SLV4_MST_DELAY_BIT     (uint8_t) 4  // [4:0]
#define MPU_R_I2C_SLV4_MST_DELAY_LENGTH  (uint8_t) 5
//------------------------------------------------------------------------------
#define MPU_R_I2C_SLV4_DI  (uint8_t) 0x35  // [7:0]
//------------------------------------------------------------------------------
#define MPU_R_I2C_MST_STATUS                (uint8_t) 0x36
#define MPU_R_I2CMST_STAT_PASS_THROUGH_BIT  (uint8_t) 7
#define MPU_R_I2CMST_STAT_SLV4_DONE_BIT     (uint8_t) 6
#define MPU_R_I2CMST_STAT_LOST_ARB_BIT      (uint8_t) 5
#define MPU_R_I2CMST_STAT_SLV4_NACK_BIT     (uint8_t) 4
#define MPU_R_I2CMST_STAT_SLV3_NACK_BIT     (uint8_t) 3
#define MPU_R_I2CMST_STAT_SLV2_NACK_BIT     (uint8_t) 2
#define MPU_R_I2CMST_STAT_SLV1_NACK_BIT     (uint8_t) 1
#define MPU_R_I2CMST_STAT_SLV0_NACK_BIT     (uint8_t) 0
//------------------------------------------------------------------------------
#define MPU_R_INT_PIN_CONFIG                 (uint8_t) 0x37
#define MPU_R_INT_CFG_LEVEL_BIT              (uint8_t) 7
#define MPU_R_INT_CFG_OPEN_BIT               (uint8_t) 6
#define MPU_R_INT_CFG_LATCH_EN_BIT           (uint8_t) 5
#define MPU_R_INT_CFG_ANYRD_2CLEAR_BIT       (uint8_t) 4
#define MPU_R_INT_CFG_FSYNC_LEVEL_BIT        (uint8_t) 3
#define MPU_R_INT_CFG_FSYNC_INT_MODE_EN_BIT  (uint8_t) 2
#define MPU_R_INT_CFG_I2C_BYPASS_EN_BIT      (uint8_t) 1
#define MPU_R_INT_CFG_CLOCKOUT_EN_BIT        (uint8_t) 0
//------------------------------------------------------------------------------
#define MPU_R_INT_ENABLE                    (uint8_t) 0x38
#define MPU_R_INT_ENABLE_FREEFALL_BIT       (uint8_t) 7
#define MPU_R_INT_ENABLE_MOTION_BIT         (uint8_t) 6
#define MPU_R_INT_ENABLE_ZEROMOT_BIT        (uint8_t) 5
#define MPU_R_INT_ENABLE_FIFO_OFLOW_BIT     (uint8_t) 4
#define MPU_R_INT_ENABLE_I2C_MST_FSYNC_BIT  (uint8_t) 3
#define MPU_R_INT_ENABLE_PLL_RDY_BIT        (uint8_t) 2
#define MPU_R_INT_ENABLE_DMP_RDY_BIT        (uint8_t) 1
#define MPU_R_INT_ENABLE_RAW_DATA_RDY_BIT   (uint8_t) 0
//------------------------------------------------------------------------------
#define MPU_R_DMP_INT_STATUS    (uint8_t) 0x39
#define MPU_R_DMP_INT_STATUS_0  (uint8_t) 0
#define MPU_R_DMP_INT_STATUS_1  (uint8_t) 1
#define MPU_R_DMP_INT_STATUS_2  (uint8_t) 2
#define MPU_R_DMP_INT_STATUS_3  (uint8_t) 3
#define MPU_R_DMP_INT_STATUS_4  (uint8_t) 4
#define MPU_R_DMP_INT_STATUS_5  (uint8_t) 5
//------------------------------------------------------------------------------
#define MPU_R_INT_STATUS                   (uint8_t) 0x3A
#define MPU_R_INT_STATUS_FREEFALL_BIT      (uint8_t) 7
#define MPU_R_INT_STATUS_MOTION_BIT        (uint8_t) 6
#define MPU_R_INT_STATUS_ZEROMOT_BIT       (uint8_t) 5
#define MPU_R_INT_STATUS_FIFO_OFLOW_BIT    (uint8_t) 4
#define MPU_R_INT_STATUS_I2C_MST_BIT       (uint8_t) 3
#define MPU_R_INT_STATUS_PLL_RDY_BIT       (uint8_t) 2
#define MPU_R_INT_STATUS_DMP_RDY_BIT       (uint8_t) 1
#define MPU_R_INT_STATUS_RAW_DATA_RDY_BIT  (uint8_t) 0
//------------------------------------------------------------------------------
#define MPU_R_ACCEL_XOUT_H      (uint8_t) 0x3B  // [15:0]
#define MPU_R_ACCEL_XOUT_L      (uint8_t) 0x3C
#define MPU_R_ACCEL_YOUT_H      (uint8_t) 0x3D  // [15:0]
#define MPU_R_ACCEL_YOUT_L      (uint8_t) 0x3E
#define MPU_R_ACCEL_ZOUT_H      (uint8_t) 0x3F  // [15:0]
#define MPU_R_ACCEL_ZOUT_L      (uint8_t) 0x40
#define MPU_R_TEMP_OUT_H        (uint8_t) 0x41  // [15:0]
#define MPU_R_TEMP_OUT_L        (uint8_t) 0x42
#define MPU_R_GYRO_XOUT_H       (uint8_t) 0x43  // [15:0]
#define MPU_R_GYRO_XOUT_L       (uint8_t) 0x44
#define MPU_R_GYRO_YOUT_H       (uint8_t) 0x45  // [15:0]
#define MPU_R_GYRO_YOUT_L       (uint8_t) 0x46
#define MPU_R_GYRO_ZOUT_H       (uint8_t) 0x47  // [15:0]
#define MPU_R_GYRO_ZOUT_L       (uint8_t) 0x48
#define MPU_R_EXT_SENS_DATA_00  (uint8_t) 0x49  // Stores data read from Slave 0, 1, 2, and 3
#define MPU_R_EXT_SENS_DATA_01  (uint8_t) 0x4A
#define MPU_R_EXT_SENS_DATA_02  (uint8_t) 0x4B
#define MPU_R_EXT_SENS_DATA_03  (uint8_t) 0x4C
#define MPU_R_EXT_SENS_DATA_04  (uint8_t) 0x4D
#define MPU_R_EXT_SENS_DATA_05  (uint8_t) 0x4E
#define MPU_R_EXT_SENS_DATA_06  (uint8_t) 0x4F
#define MPU_R_EXT_SENS_DATA_07  (uint8_t) 0x50
#define MPU_R_EXT_SENS_DATA_08  (uint8_t) 0x51
#define MPU_R_EXT_SENS_DATA_09  (uint8_t) 0x52
#define MPU_R_EXT_SENS_DATA_10  (uint8_t) 0x53
#define MPU_R_EXT_SENS_DATA_11  (uint8_t) 0x54
#define MPU_R_EXT_SENS_DATA_12  (uint8_t) 0x55
#define MPU_R_EXT_SENS_DATA_13  (uint8_t) 0x56
#define MPU_R_EXT_SENS_DATA_14  (uint8_t) 0x57
#define MPU_R_EXT_SENS_DATA_15  (uint8_t) 0x58
#define MPU_R_EXT_SENS_DATA_16  (uint8_t) 0x59
#define MPU_R_EXT_SENS_DATA_17  (uint8_t) 0x5A
#define MPU_R_EXT_SENS_DATA_18  (uint8_t) 0x5B
#define MPU_R_EXT_SENS_DATA_19  (uint8_t) 0x5C
#define MPU_R_EXT_SENS_DATA_20  (uint8_t) 0x5D
#define MPU_R_EXT_SENS_DATA_21  (uint8_t) 0x5E
#define MPU_R_EXT_SENS_DATA_22  (uint8_t) 0x5F
#define MPU_R_EXT_SENS_DATA_23  (uint8_t) 0x60
#define MPU_R_I2C_SLV0_DO       (uint8_t) 0x63
#define MPU_R_I2C_SLV1_DO       (uint8_t) 0x64
#define MPU_R_I2C_SLV2_DO       (uint8_t) 0x65
#define MPU_R_I2C_SLV3_DO       (uint8_t) 0x66
//------------------------------------------------------------------------------
#define MPU_R_I2C_MST_DELAY_CRTL        (uint8_t) 0x67
#define MPU_R_I2CMST_DLY_ES_SHADOW_BIT  (uint8_t) 7
#define MPU_R_I2CMST_DLY_SLV4_EN_BIT    (uint8_t) 4
#define MPU_R_I2CMST_DLY_SLV3_EN_BIT    (uint8_t) 3
#define MPU_R_I2CMST_DLY_SLV2_EN_BIT    (uint8_t) 2
#define MPU_R_I2CMST_DLY_SLV1_EN_BIT    (uint8_t) 1
#define MPU_R_I2CMST_DLY_SLV0_EN_BIT    (uint8_t) 0
//------------------------------------------------------------------------------
#define MPU_R_SIGNAL_PATH_RESET    (uint8_t) 0x68
#define MPU_R_SPATH_GYRO_RST_BIT   (uint8_t) 2
#define MPU_R_SPATH_ACCEL_RST_BIT  (uint8_t) 1
#define MPU_R_SPATH_TEMP_RST_BIT   (uint8_t) 0
//------------------------------------------------------------------------------
#define MPU_R_USER_CTRL                    (uint8_t) 0x6A
#define MPU_R_USERCTRL_DMP_EN_BIT          (uint8_t) 7
#define MPU_R_USERCTRL_FIFO_EN_BIT         (uint8_t) 6
#define MPU_R_USERCTRL_I2C_MST_EN_BIT      (uint8_t) 5
#define MPU_R_USERCTRL_I2C_IF_DIS_BIT      (uint8_t) 4
#define MPU_R_USERCTRL_DMP_RESET_BIT       (uint8_t) 3
#define MPU_R_USERCTRL_FIFO_RESET_BIT      (uint8_t) 2
#define MPU_R_USERCTRL_I2C_MST_RESET_BIT   (uint8_t) 1
#define MPU_R_USERCTRL_SIG_COND_RESET_BIT  (uint8_t) 0
//------------------------------------------------------------------------------
#define MPU_R_PWR_MGMT1              (uint8_t) 0x6B
#define MPU_R_PWR1_DEVICE_RESET_BIT  (uint8_t) 7
#define MPU_R_PWR1_SLEEP_BIT         (uint8_t) 6
#define MPU_R_PWR1_CYCLE_BIT         (uint8_t) 5
#define MPU_R_PWR1_GYRO_STANDBY_BIT  (uint8_t) 4
#define MPU_R_PWR1_TEMP_DIS_BIT      (uint8_t) 3
#define MPU_R_PWR1_CLKSEL_BIT        (uint8_t) 2
#define MPU_R_PWR1_CLKSEL_LENGTH     (uint8_t) 3
//------------------------------------------------------------------------------
#define MPU_R_PWR_MGMT2                 (uint8_t) 0x6C
#define MPU_R_PWR2_LP_WAKE_CTRL_BIT     (uint8_t) 7
#define MPU_R_PWR2_LP_WAKE_CTRL_LENGTH  (uint8_t) 2
#define MPU_R_PWR2_STBY_XA_BIT          (uint8_t) 5
#define MPU_R_PWR2_STBY_YA_BIT          (uint8_t) 4
#define MPU_R_PWR2_STBY_ZA_BIT          (uint8_t) 3
#define MPU_R_PWR2_STBY_XG_BIT          (uint8_t) 2
#define MPU_R_PWR2_STBY_YG_BIT          (uint8_t) 1
#define MPU_R_PWR2_STBY_ZG_BIT          (uint8_t) 0
#define MPU_R_PWR2_STBY_XYZA_BITS       (uint8_t) ( 1<< MPU_R_PWR2_STBY_XA_BIT        \
                                        | 1 << MPU_R_PWR2_STBY_YA_BIT       \
                                        | 1 << MPU_R_PWR2_STBY_ZA_BIT )
#define MPU_R_PWR2_STBY_XYZG_BITS       (uint8_t) ( 1<< MPU_R_PWR2_STBY_XG_BIT        \
                                        | 1 << MPU_R_PWR2_STBY_YG_BIT       \
                                        | 1 << MPU_R_PWR2_STBY_ZG_BIT )
//------------------------------------------------------------------------------
#define MPU_R_BANK_SEL                   (uint8_t) 0x6D
#define MPU_R_BANKSEL_PRFTCH_EN_BIT      (uint8_t) 6
#define MPU_R_BANKSEL_CFG_USER_BANK_BIT  (uint8_t) 5
#define MPU_R_BANKSEL_MEM_SEL_BIT        (uint8_t) 4
#define MPU_R_BANKSEL_MEM_SEL_LENGTH     (uint8_t) 5
//------------------------------------------------------------------------------
#define MPU_R_MEM_START_ADDR  (uint8_t) 0x6E
#define MPU_R_MEM_R_W         (uint8_t) 0x6F
#define MPU_R_PRGM_START_H    (uint8_t) 0x70
#define MPU_R_PRGM_START_L    (uint8_t) 0x71
#define MPU_R_FIFO_COUNT_H    (uint8_t) 0x72  // [15:0]
#define MPU_R_FIFO_COUNT_L    (uint8_t) 0x73
#define MPU_R_FIFO_R_W        (uint8_t) 0x74
#define MPU_R_WHO_AM_I        (uint8_t) 0x75

/*******************************************************************************
 * MPU6000, MPU6050 and MPU9150 registers
 ******************************************************************************/
#if defined CONFIG_MPU_6050
#define MPU_R_XG_OTP_OFFSET_TC  (uint8_t) 0x00  // [7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
//------------------------------------------------------------------------------
#define MPU_R_YG_OTP_OFFSET_TC  (uint8_t) 0x01  // [7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU_R_TC_PWR_MODE_BIT   (uint8_t) 7     // note: TC = temperature compensation, i think
//------------------------------------------------------------------------------
#define MPU_R_ZG_OTP_OFFSET_TC  (uint8_t) 0x02  // [7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU_R_X_FINE_GAIN       (uint8_t) 0x03  // [7:0] X_FINE_GAIN
#define MPU_R_Y_FINE_GAIN       (uint8_t) 0x04  // [7:0] Y_FINE_GAIN
#define MPU_R_Z_FINE_GAIN       (uint8_t) 0x05  // [7:0] Z_FINE_GAIN
#define MPU_R_XA_OFFSET_H       (uint8_t) 0x06  // [15:1] XA_OFFS
#define MPU_R_XA_OFFSET_L       (uint8_t) 0x07  // note: TC: bit [0]
#define MPU_R_YA_OFFSET_H       (uint8_t) 0x08  // [15:1] YA_OFFS
#define MPU_R_YA_OFFSET_L       (uint8_t) 0x09  // note: TC: bit [0]
#define MPU_R_ZA_OFFSET_H       (uint8_t) 0x0A  // [15:1] ZA_OFFS
#define MPU_R_ZA_OFFSET_L       (uint8_t) 0x0B  // note: TC: bit [0]
#define MPU_R_SELF_TEST_X       (uint8_t) 0x0D
#define MPU_R_SELF_TEST_Y       (uint8_t) 0x0E
#define MPU_R_SELF_TEST_Z       (uint8_t) 0x0F
#define MPU_R_SELF_TEST_A       (uint8_t) 0x10
//------------------------------------------------------------------------------
#define MPU_R_MOTION_DETECT_STATUS  (uint8_t) 0x61
#define MPU_R_MOT_STATUS_X_NEG_BIT  (uint8_t) 7
#define MPU_R_MOT_STATUS_X_POS_BIT  (uint8_t) 6
#define MPU_R_MOT_STATUS_Y_NEG_BIT  (uint8_t) 5
#define MPU_R_MOT_STATUS_Y_POS_BIT  (uint8_t) 4
#define MPU_R_MOT_STATUS_Z_NEG_BIT  (uint8_t) 3
#define MPU_R_MOT_STATUS_Z_POS_BIT  (uint8_t) 2
#define MPU_R_MOT_STATUS_ZRMOT_BIT  (uint8_t) 0
//------------------------------------------------------------------------------
#define MPU_R_MOTION_DETECT_CTRL             (uint8_t) 0x69
#define MPU_R_MOTCTRL_ACCEL_ON_DELAY_BIT     (uint8_t) 5  // [5:4]
#define MPU_R_MOTCTRL_ACCEL_ON_DELAY_LENGTH  (uint8_t) 2
#define MPU_R_MOTCTRL_FF_COUNT_BIT           (uint8_t) 3  // [3:2]
#define MPU_R_MOTCTRL_FF_COUNT_LENGTH        (uint8_t) 2
#define MPU_R_MOTCTRL_MOT_COUNT_BIT          (uint8_t) 1  // [1:0]
#define MPU_R_MOTCTRL_MOT_COUNT_LENGTH       (uint8_t) 2
//------------------------------------------------------------------------------
#endif // defined CONFIG_MPU_R_6050

#endif // ifndef __mpu60x0_registers_h__