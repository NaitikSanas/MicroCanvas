#ifndef __mpu60x0_i2c_h__
#define __mpu60x0_i2c_h__

#include <stdint.h>
#include "esp_err.h"
#include "mpu60x0_types.h"

/*******************************************************************************
 * SETUP
 ******************************************************************************/

esp_err_t mpu_i2c_close(mpu_handle_t *mpu);

esp_err_t mpu_i2c_begin(mpu_handle_t *mpu);

esp_err_t mpu_i2c_close(mpu_handle_t* mpu);

/*******************************************************************************
 * WRITING
 ******************************************************************************/

esp_err_t mpu_i2c_read_bytes(mpu_handle_t *mpu, uint8_t regAddr, size_t length,
                             uint8_t *data);

esp_err_t mpu_i2c_read_bits(mpu_handle_t *mpu, uint8_t regAddr,
                            uint8_t bitStart, uint8_t length, uint8_t *data);

esp_err_t mpu_i2c_read_bit(mpu_handle_t *mpu, uint8_t regAddr, uint8_t bitNum,
                           uint8_t *data);

esp_err_t mpu_i2c_read_byte(mpu_handle_t *mpu, uint8_t regAddr, uint8_t *data);

/*******************************************************************************
 * READING
 ******************************************************************************/

esp_err_t mpu_i2c_write_bytes(mpu_handle_t *mpu, uint8_t regAddr, size_t length,
                              const uint8_t *data);

esp_err_t mpu_i2c_write_bits(mpu_handle_t *mpu, uint8_t regAddr,
                             uint8_t bitStart, uint8_t length, uint8_t data);

esp_err_t mpu_i2c_write_byte(mpu_handle_t *mpu, uint8_t regAddr, uint8_t data);

esp_err_t mpu_i2c_write_bit(mpu_handle_t *mpu, uint8_t regAddr, uint8_t bitNum,
                            uint8_t data);


#endif