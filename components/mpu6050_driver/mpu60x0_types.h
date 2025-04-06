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

#ifndef __mpu60x0_types_h__
#define __mpu60x0_types_h__

#include "driver/gpio.h"
#include "mpu60x0_registers.h"


// ##     ## ########  ##     ## 
// ###   ### ##     ## ##     ## 
// #### #### ##     ## ##     ## 
// ## ### ## ########  ##     ## 
// ##     ## ##        ##     ## 
// ##     ## ##        ##     ## 
// ##     ## ##         #######   

/*******************************************************************************
 * @brief Port number of the I2C peripheral */
typedef int mpu_periph_num_t;

/*******************************************************************************
 * @brief Variables for initialization of the I2C */
typedef struct mpu_initializer_struct
{
    gpio_num_t sda_io_num;
    gpio_num_t scl_io_num;
    gpio_pullup_t sda_pullup_en;
    gpio_pullup_t scl_pullup_en;
    uint32_t clk_speed;
} mpu_initializer_t;


/*******************************************************************************
 * @brief Port number of the SPI peripheral */
typedef int mpu_periph_num_t;




/*******************************************************************************
 * @brief I2C bus address */
typedef uint8_t mpu_addr_t;

/*******************************************************************************
 * @brief I2C/SPI bus metadata handler */
typedef struct mpu_bus_struct
{
    mpu_periph_num_t num;
    int timeout; // timeout for read/write operations
} mpu_bus_t;

/*******************************************************************************
 * @brief main MPU data/metadata handler */
typedef struct mpu_handle_struct
{
    mpu_initializer_t init;
    mpu_bus_t bus;      // Communication bus pointer, I2C / SPI
    mpu_addr_t addr;    // I2C address / SPI device handle
    uint8_t buffer[16]; // Common buffer for temporary data
    esp_err_t last_err; // Holds last error code
} mpu_handle_t;

//  ######   ##    ## ########                   ###     ######   ######  
// ##    ##   ##  ##  ##     ##                 ## ##   ##    ## ##    ## 
// ##          ####   ##     ##                ##   ##  ##       ##       
// ##   ####    ##    ########     #######    ##     ## ##       ##       
// ##    ##     ##    ##   ##                 ######### ##       ##       
// ##    ##     ##    ##    ##                ##     ## ##    ## ##    ## 
//  ######      ##    ##     ##               ##     ##  ######   ######

/*******************************************************************************
 * @brief data structure (INT) for Acceleration/Gyro measurements */
typedef struct raw_axes_struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} raw_axes_t;

/*******************************************************************************
 * @brief data structure (FLOAT) for Acceleration/Gyro measurements */
typedef struct float_axes_struct
{
    float x;
    float y;
    float z;
} float_axes_t;


// ##     ##  #######  ######## ####  #######  ##    ## 
// ###   ### ##     ##    ##     ##  ##     ## ###   ## 
// #### #### ##     ##    ##     ##  ##     ## ####  ## 
// ## ### ## ##     ##    ##     ##  ##     ## ## ## ## 
// ##     ## ##     ##    ##     ##  ##     ## ##  #### 
// ##     ## ##     ##    ##     ##  ##     ## ##   ### 
// ##     ##  #######     ##    ####  #######  ##    ##

/*! Zero-motion configuration */
typedef struct zrmot_config_struct
{
    uint8_t threshold;  // Motion threshold in LSB. 1LSB = 1mg, 255LSB = 1020mg
    uint8_t time;       /** Duration in milliseconds that the accel data must exceed
                         * the threshold before motion is reported. MAX = 255ms. */
} zrmot_config_t;

/*******************************************************************************
 * @brief Motion Detection counter decrement rate (Motion and FreeFall */
typedef enum mot_counter_struct
{
    MOT_COUNTER_RESET = 0,  // When set, any non-qualifying sample will reset the corresponding counter to 0
    MOT_COUNTER_DEC_1 = 1,  // Decrement counter in 1
    MOT_COUNTER_DEC_2 = 2,  // Decrement counter in 2
    MOT_COUNTER_DEC_4 = 3   // Decrement counter in 4
} mot_counter_t;

/*******************************************************************************
 * @brief Motion Detection configuration */
typedef struct mot_config_struct
{
    /** Motion threshold in LSB.
    * For MPU6000 / MPU6050 / MPU9150: 1LSB = 32mg, 255LSB = 8160mg.
    * For MPU6500 / MPU9250: 1LSB = 4mg, 255LSB = 1020mg. */
    uint8_t threshold;

    /** Duration in milliseconds that the accel data must exceed
    * the threshold before motion is reported. MAX = 255ms. */
    uint8_t time;

    /** Specifies in milliseconds the additional power-on delay applied to accelerometer
    * data path modules. MAX = 3ms.
    * More: The signal path contains filters which must be flushed on wake-up with new
    * samples before the detection modules begin operations.
    * There is already a default built-in 4ms delay. */
    uint8_t accel_on_delay : 2;

    // Configures the detection counter decrement rate.
    mot_counter_t counter : 2;
} mot_config_t;

/*******************************************************************************
 * @brief Free Fall configuration */
typedef struct ff_config_struct
{
    uint8_t threshold;          //!< Motion threshold in LSB. 1LSB = 1mg, 255LSB = 1020mg
    uint8_t time;               /*!< Duration in milliseconds that the accel data must exceed
                                 * the threshold before motion is reported. MAX = 255ms. */
    uint8_t accel_on_delay : 2; /**< Specifies in milliseconds the additional power-on delay applied to accelerometer
                                 * data path modules. MAX = 3ms.
                                 * More: The signal path contains filters which must be flushed on wake-up with new
                                 * samples before the detection modules begin operations.
                                 * There is already a default built-in 4ms delay. */
    mot_counter_t counter : 2;  //!< Configures the detection counter decrement rate.
} ff_config_t;

typedef uint8_t mot_stat_t;

#define MOT_STAT_XNEG       (mot_stat_t) (1 << MPU_R_MOT_STATUS_X_NEG_BIT)
#define MOT_STAT_XPOS       (mot_stat_t) (1 << MPU_R_MOT_STATUS_X_POS_BIT)
#define MOT_STAT_YNEG       (mot_stat_t) (1 << MPU_R_MOT_STATUS_Y_NEG_BIT)
#define MOT_STAT_YPOS       (mot_stat_t) (1 << MPU_R_MOT_STATUS_Y_POS_BIT)
#define MOT_STAT_ZNEG       (mot_stat_t) (1 << MPU_R_MOT_STATUS_Z_NEG_BIT)
#define MOT_STAT_ZPOS       (mot_stat_t) (1 << MPU_R_MOT_STATUS_Z_POS_BIT)
#define MOT_STAT_ZEROMOTION (mot_stat_t) (1 << MPU_R_MOT_STATUS_ZRMOT_BIT)

// ######## #### ##       ######## ######## ########  
// ##        ##  ##          ##    ##       ##     ## 
// ##        ##  ##          ##    ##       ##     ## 
// ######    ##  ##          ##    ######   ########  
// ##        ##  ##          ##    ##       ##   ##   
// ##        ##  ##          ##    ##       ##    ##  
// ##       #### ########    ##    ######## ##     ##

/*******************************************************************************
 * @brief Accelerometer Digital High Pass Filter (only for motion detection modules */
typedef enum accel_dhpf_enum
{
    ACCEL_DHPF_RESET = 0,   /** This effectively disables the high pass filter. This mode may be toggled to quickly
                             * settle the filter. */
    ACCEL_DHPF_5HZ    = 1,  // ON state, the high pass filter will pass signals above the cut off frequency.
    ACCEL_DHPF_2_5HZ  = 2,  // ON state, the high pass filter will pass signals above the cut off frequency.
    ACCEL_DHPF_1_25HZ = 3,  // ON state, the high pass filter will pass signals above the cut off frequency.
    ACCEL_DHPF_0_63HZ = 4,  // ON state, the high pass filter will pass signals above the cut off frequency.
    ACCEL_DHPF_HOLD   = 7,  /** The filter holds the present sample. The output will be the difference between the
                             * input sample and the held sample. */
} accel_dhpf_t;

/*******************************************************************************
 * @brief Digital low-pass filter (based on gyro bandwidth */
typedef enum dlpf_enum
{
    DLPF_256HZ_NOLPF = 0,
    DLPF_188HZ       = 1,
    DLPF_98HZ        = 2,
    DLPF_42HZ        = 3,
    DLPF_20HZ        = 4,
    DLPF_10HZ        = 5,
    DLPF_5HZ         = 6,
#ifdef CONFIG_MPU_6050
    DLPF_2100HZ_NOLPF = 7
#endif
} dlpf_t;

// ######## #### ########  #######  
// ##        ##  ##       ##     ## 
// ##        ##  ##       ##     ## 
// ######    ##  ######   ##     ## 
// ##        ##  ##       ##     ## 
// ##        ##  ##       ##     ## 
// ##       #### ##        ####### 

typedef uint16_t fifo_config_t;

/*******************************************************************************
 * @brief FIFO mode */
typedef enum fifo_mode_enum
{
    FIFO_MODE_OVERWRITE = 0,  // when fifo full, additional writes will be written to fifo, replacing the oldest data.
    FIFO_MODE_STOP_FULL = 1   // when fifo full, additional writes will not be written to fifo.
} fifo_mode_t;

/*******************************************************************************
 * @brief FIFO configuration, enable sensors to be written to FIFO */
#define FIFO_CFG_NONE (fifo_config_t) 0x0
#define FIFO_CFG_GYRO (fifo_config_t)  (  1 << regs::FIFO_XGYRO_EN_BIT  \
                                        | 1 << regs::FIFO_YGYRO_EN_BIT  \
                                        | 1 << regs::FIFO_ZGYRO_EN_BIT )
#define FIFO_CFG_ACCEL       (fifo_config_t) (1 << regs::FIFO_ACCEL_EN_BIT)
#define FIFO_CFG_TEMPERATURE (fifo_config_t) (1 << regs::FIFO_TEMP_EN_BIT)
#define FIFO_CFG_SLAVE0      (fifo_config_t) (1 << regs::FIFO_SLV_0_EN_BIT)
#define FIFO_CFG_SLAVE1      (fifo_config_t) (1 << regs::FIFO_SLV_1_EN_BIT)
#define FIFO_CFG_SLAVE2      (fifo_config_t) (1 << regs::FIFO_SLV_2_EN_BIT)
#define FIFO_CFG_SLAVE3      (fifo_config_t) (1 << 8)

// #### ##    ## ######## 
//  ##  ###   ##    ##    
//  ##  ####  ##    ##    
//  ##  ## ## ##    ##    
//  ##  ##  ####    ##    
//  ##  ##   ###    ##    
// #### ##    ##    ## 

typedef uint8_t int_en_t;
typedef uint8_t int_stat_t;

/*******************************************************************************
 * @brief DMP Interrupt mode */
typedef enum dmp_int_mode_enum
{
    DMP_INT_MODE_PACKET = 0,
    DMP_INT_MODE_GESTURE = 1
} dmp_int_mode_t;

/*******************************************************************************
 *  @brief Interrupt active level */
typedef enum int_lvl_enum
{
    INT_LVL_ACTIVE_HIGH = 0,
    INT_LVL_ACTIVE_LOW = 1
} int_lvl_t;

/*******************************************************************************
 *  @brief Interrupt drive state */
typedef enum int_drive_enum
{
    INT_DRV_PUSHPULL = 0,
    INT_DRV_OPENDRAIN = 1
} int_drive_t;

/*******************************************************************************
 *  @brief Interrupt mode */
typedef enum int_mode_enum
{
    INT_MODE_PULSE50US = 0,
    INT_MODE_LATCH = 1
} int_mode_t;

/*******************************************************************************
 *  @brief Interrupt clear mode */
typedef enum int_clear_enum
{
    INT_CLEAR_STATUS_REG = 0,
    INT_CLEAR_ANYREAD = 1
} int_clear_t;

/*******************************************************************************
 *  @brief Interrupt configuration struct */
typedef struct int_config_struct
{
    int_lvl_t level : 1;
    int_drive_t drive : 1;
    int_mode_t mode : 1;
    int_clear_t clear : 1;
} int_config_t;

/*******************************************************************************
 * @brief Enable features to generate signal at Interrupt pin
 * @note Freefall and zero motion only available to MPU6000 / MPU6050 / MPU9150
 * */

#define INT_EN_NONE             (int_en_t) (0x0)
#define INT_EN_MOTION_DETECT    (int_en_t) (1 << MPU_R_INT_ENABLE_MOTION_BIT)
#define INT_EN_FIFO_OVERFLOW    (int_en_t) (1 << MPU_R_INT_ENABLE_FIFO_OFLOW_BIT)
#define INT_EN_I2C_MST_FSYNC    (int_en_t) (1 << MPU_R_INT_ENABLE_I2C_MST_FSYNC_BIT)  // int from I2C_MST_STATUS
#define INT_EN_PLL_READY        (int_en_t) (1 << MPU_R_INT_ENABLE_PLL_RDY_BIT)
#define INT_EN_DMP_READY        (int_en_t) (1 << MPU_R_INT_ENABLE_DMP_RDY_BIT)
#define INT_EN_RAWDATA_READY    (int_en_t) (1 << MPU_R_INT_ENABLE_RAW_DATA_RDY_BIT)
#if defined CONFIG_MPU_6000 || defined CONFIG_MPU_6050
#define INT_EN_FREE_FALL        (int_en_t) (1 << MPU_R_INT_ENABLE_FREEFALL_BIT)
#define INT_EN_ZERO_MOTION      (int_en_t) (1 << MPU_R_INT_ENABLE_ZEROMOT_BIT)
#endif

/*******************************************************************************
 * @brief Interrupt Status
 * @note Freefall and zero motion only available to MPU6000 / MPU6050 / MPU9150
 * */

#define INT_STAT_MOTION_DETECT      (int_en_t) (1 << MPU_R_INT_STATUS_MOTION_BIT)
#define INT_STAT_FIFO_OVERFLOW      (int_en_t) (1 << MPU_R_INT_STATUS_FIFO_OFLOW_BIT)
#define INT_STAT_I2C_MST_FSYNC      (int_en_t) (1 << MPU_R_INT_STATUS_I2C_MST_BIT)  // int from I2C_MST_STATUS
#define INT_STAT_PLL_READY          (int_en_t) (1 << MPU_R_INT_STATUS_PLL_RDY_BIT)
#define INT_STAT_DMP_READY          (int_en_t) (1 << MPU_R_INT_STATUS_DMP_RDY_BIT)
#define INT_STAT_RAWDATA_READY      (int_en_t) (1 << MPU_R_INT_STATUS_RAW_DATA_RDY_BIT)
#if defined CONFIG_MPU_6000 || defined CONFIG_MPU_6050
#define INT_STAT_FREE_FALL          (int_en_t) (1 << MPU_R_INT_STATUS_FREEFALL_BIT)
#define INT_STAT_ZERO_MOTION        (int_en_t) (1 << MPU_R_INT_STATUS_ZEROMOT_BIT)
#endif

// ##        #######  ##      ##    ########   #######  ##      ##
// ##       ##     ## ##  ##  ##    ##     ## ##     ## ##  ##  ##
// ##       ##     ## ##  ##  ##    ##     ## ##     ## ##  ##  ##
// ##       ##     ## ##  ##  ##    ########  ##     ## ##  ##  ##
// ##       ##     ## ##  ##  ##    ##        ##     ## ##  ##  ##
// ##       ##     ## ##  ##  ##    ##        ##     ## ##  ##  ##
// ########  #######   ###  ###     ##         #######   ###  ###

/*******************************************************************************
 * @brief Standby Mode */
typedef uint8_t stby_en_t;
#define  STBY_EN_NONE    (stby_en_t) 0x0
#define  STBY_EN_ACCEL_X (stby_en_t) (1 << regs::PWR2_STBY_XA_BIT)
#define  STBY_EN_ACCEL_Y (stby_en_t) (1 << regs::PWR2_STBY_YA_BIT)
#define  STBY_EN_ACCEL_Z (stby_en_t) (1 << regs::PWR2_STBY_ZA_BIT)
#define  STBY_EN_ACCEL   (stby_en_t) (STBY_EN_ACCEL_X | STBY_EN_ACCEL_Y | STBY_EN_ACCEL_Z)
#define  STBY_EN_GYRO_X  (stby_en_t) (1 << regs::PWR2_STBY_XG_BIT)
#define  STBY_EN_GYRO_Y  (stby_en_t) (1 << regs::PWR2_STBY_YG_BIT)
#define  STBY_EN_GYRO_Z  (stby_en_t) (1 << regs::PWR2_STBY_ZG_BIT)
#define  STBY_EN_GYRO    (stby_en_t) (STBY_EN_GYRO_X | STBY_EN_GYRO_Y | STBY_EN_GYRO_Z)
#define  STBY_EN_TEMP    (stby_en_t) (1 << 6)
/*! This is a low power mode that allows quick enabling of the gyros.
 * \note: When set, the gyro drive and pll circuitry are enabled, but the sense paths are disabled. */
#define  STBY_EN_LOWPWR_GYRO_PLL_ON (stby_en_t) (1 << 7)

// ##     ## ####  ######   ######  
// ###   ###  ##  ##    ## ##    ## 
// #### ####  ##  ##       ##       
// ## ### ##  ##   ######  ##       
// ##     ##  ##        ## ##       
// ##     ##  ##  ##    ## ##    ## 
// ##     ## ####  ######   ######  

/*******************************************************************************
 * @brief Low-Power Accelerometer wake-up rates */
typedef enum lp_accel_rate_enum
{
    LP_ACCEL_RATE_1_25HZ = 0,
    LP_ACCEL_RATE_5HZ    = 1,
    LP_ACCEL_RATE_20HZ   = 2,
    LP_ACCEL_RATE_40HZ   = 3
} lp_accel_rate_t;

/*******************************************************************************
 * @brief Clock Source */
typedef enum mpu_clock_src_enum
{
    CLOCK_INTERNAL = 0,  // Internal oscillator: 20MHz for MPU6500 and 8MHz for MPU6050
    CLOCK_PLL      = 3,  // Selects automatically best pll source (recommended)
#if defined CONFIG_MPU_6050
    CLOCK_EXT32KHZ = 4,  // PLL with external 32.768kHz reference
    CLOCK_EXT19MHZ = 5,  // PLL with external 19.2MHz reference
#endif
    CLOCK_KEEP_RESET = 7  // Stops the clock and keeps timing generator in reset
} mpu_clock_src_t;
  

/*******************************************************************************
 * @brief Gyroscope full-scale range */
typedef enum gyro_fs_enum
{
    GYRO_FS_250DPS  = 0,  // +/- 250 º/s  -> 131 LSB/(º/s)
    GYRO_FS_500DPS  = 1,  // +/- 500 º/s  -> 65.5 LSB/(º/s)
    GYRO_FS_1000DPS = 2,  // +/- 1000 º/s -> 32.8 LSB/(º/s)
    GYRO_FS_2000DPS = 3   // +/- 2000 º/s -> 16.4 LSB/(º/s)
} gyro_fs_t;

/*******************************************************************************
 * @brief Accel full-scale range */
typedef enum accel_fs_enum
{
    ACCEL_FS_2G  = 0,  // +/- 2 g  -> 16.384 LSB/g
    ACCEL_FS_4G  = 1,  // +/- 4 g  -> 8.192 LSB/g
    ACCEL_FS_8G  = 2,  // +/- 8 g  -> 4.096 LSB/g
    ACCEL_FS_16G = 3   // +/- 16 g -> 2.048 LSB/g
} accel_fs_t;

#endif // ifndef __mpu60x0_types_h__