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

#include <stdio.h>
#include <stdint.h>
#include "sdkconfig.h"

#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c.h"

#include "mpu60x0_i2c.h"

#define I2C_MASTER_ACK_EN   true    /*!< Enable ack check for master */
#define I2C_MASTER_ACK_DIS  false   /*!< Disable ack check for master */

static const char* TAG __attribute__((unused)) = "MPU_I2C";

//  ######  ######## ######## ##     ## ########
// ##    ## ##          ##    ##     ## ##     ##
// ##       ##          ##    ##     ## ##     ##
//  ######  ######      ##    ##     ## ########
//       ## ##          ##    ##     ## ##
// ##    ## ##          ##    ##     ## ##
//  ######  ########    ##     #######  ##

esp_err_t mpu_i2c_begin(mpu_handle_t* mpu)
{
    i2c_config_t conf;
    conf.clk_flags = 0;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = mpu->init.sda_io_num;
    conf.scl_io_num = mpu->init.scl_io_num;
    conf.sda_pullup_en = mpu->init.sda_pullup_en;
    conf.scl_pullup_en = mpu->init.scl_pullup_en;
    conf.master.clk_speed = mpu->init.clk_speed;

    esp_err_t err = i2c_param_config(mpu->bus.num, &conf);

    if(!err) err = i2c_driver_install(mpu->bus.num, conf.mode, 0, 0, 0);

    if(mpu->bus.timeout == 0)
        mpu->bus.timeout = 10; // set timeout to 10 ms

    return err;
}

esp_err_t mpu_i2c_close(mpu_handle_t* mpu)
{
    return i2c_driver_delete(mpu->bus.num);
}

// ##      ## ########  #### ######## ######## #### ##    ##  ######
// ##  ##  ## ##     ##  ##     ##       ##     ##  ###   ## ##    ##
// ##  ##  ## ##     ##  ##     ##       ##     ##  ####  ## ##
// ##  ##  ## ########   ##     ##       ##     ##  ## ## ## ##   ####
// ##  ##  ## ##   ##    ##     ##       ##     ##  ##  #### ##    ##
// ##  ##  ## ##    ##   ##     ##       ##     ##  ##   ### ##    ##
//  ###  ###  ##     ## ####    ##       ##    #### ##    ##  ######

esp_err_t mpu_i2c_write_bit(mpu_handle_t* mpu, uint8_t regAddr,
                            uint8_t bitNum, uint8_t data)
{
    uint8_t buffer;
    esp_err_t err = mpu_i2c_read_byte(mpu, regAddr, &buffer);
    if(err) return err;
    buffer = data ? (buffer | (1 << bitNum)) : (buffer & ~(1 << bitNum));
    return mpu_i2c_write_byte(mpu, regAddr, buffer);
}

esp_err_t mpu_i2c_write_bits(mpu_handle_t* mpu, uint8_t regAddr,
                             uint8_t bitStart, uint8_t length,
                             uint8_t data)
{
    uint8_t buffer;
    esp_err_t err = mpu_i2c_read_byte(mpu, regAddr, &buffer);
    if(err) return err;
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1);
    data &= mask;
    buffer &= ~mask;
    buffer |= data;
    return mpu_i2c_write_byte(mpu, regAddr, buffer);
}

esp_err_t mpu_i2c_write_byte(mpu_handle_t* mpu, uint8_t regAddr, uint8_t data)
{
    return mpu_i2c_write_bytes(mpu, regAddr, 1, &data);
}

esp_err_t mpu_i2c_write_bytes(mpu_handle_t* mpu,
                              uint8_t regAddr, size_t length,
                              const uint8_t *data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (mpu->addr << 1) | I2C_MASTER_WRITE,
                          I2C_MASTER_ACK_EN);
    i2c_master_write_byte(cmd, regAddr, I2C_MASTER_ACK_EN);
    i2c_master_write(cmd, (uint8_t*) data, length, I2C_MASTER_ACK_EN);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(mpu->bus.num, cmd, mpu->bus.timeout);
    i2c_cmd_link_delete(cmd);
    return err;
}

// ########  ########    ###    ########  #### ##    ##  ######
// ##     ## ##         ## ##   ##     ##  ##  ###   ## ##    ##
// ##     ## ##        ##   ##  ##     ##  ##  ####  ## ##
// ########  ######   ##     ## ##     ##  ##  ## ## ## ##   ####
// ##   ##   ##       ######### ##     ##  ##  ##  #### ##    ##
// ##    ##  ##       ##     ## ##     ##  ##  ##   ### ##    ##
// ##     ## ######## ##     ## ########  #### ##    ##  ######

esp_err_t mpu_i2c_read_bit(mpu_handle_t* mpu, uint8_t regAddr,
                           uint8_t bitNum, uint8_t *data)
{
    return mpu_i2c_read_bits(mpu, regAddr, bitNum, 1, data);
}

esp_err_t mpu_i2c_read_bits(mpu_handle_t* mpu, uint8_t regAddr,
                            uint8_t bitStart, uint8_t length,
                            uint8_t *data)
{
    uint8_t buffer;
    esp_err_t err = mpu_i2c_read_byte(mpu, regAddr, &buffer);
    if(!err)
    {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        buffer &= mask;
        buffer >>= (bitStart - length + 1);
        *data = buffer;
    }
    return err;
}

esp_err_t mpu_i2c_read_byte(mpu_handle_t* mpu, uint8_t regAddr, uint8_t *data)
{
    return mpu_i2c_read_bytes(mpu, regAddr, 1, data);
}

esp_err_t mpu_i2c_read_bytes(mpu_handle_t* mpu, uint8_t regAddr,
                             size_t length, uint8_t *data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (mpu->addr << 1) | I2C_MASTER_WRITE,
                          I2C_MASTER_ACK_EN);
    i2c_master_write_byte(cmd, regAddr, I2C_MASTER_ACK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (mpu->addr << 1) | I2C_MASTER_READ,
                          I2C_MASTER_ACK_EN);
    i2c_master_read(cmd, data, length, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(mpu->bus.num,
                                         cmd, mpu->bus.timeout);
    i2c_cmd_link_delete(cmd);
    return err;
}
