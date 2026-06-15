/**
 * @file uCanvasServer_UART.h
 * @brief UART transport layer for uCanvas Server (ESP-IDF)
 *
 * Frame format:
 *   [SOF 0xAA] [LEN_LO] [LEN_HI] [CMD] [PAYLOAD...] [CRC8]
 *
 *   LEN = number of bytes in PAYLOAD only (does not include CMD or CRC)
 *   CRC8 is computed over CMD + PAYLOAD (poly 0x07, init 0x00)
 *
 * This module owns the UART driver instance and runs a background task
 * that parses incoming frames and dispatches them to uCanvasServer_Core.
 */

#ifndef UCANVAS_SERVER_UART_H_
#define UCANVAS_SERVER_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "driver/uart.h"
#include "uCanvasServer_Protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uart_port_t uart_num;
    int         tx_pin;
    int         rx_pin;
    int         baud_rate;
    int         rx_buf_size;
    int         tx_buf_size;
    int         task_priority;
    int         task_core_id;     /* core to pin the rx task to */
} uCanvasServer_UART_Config_t;

/**
 * @brief Initializes UART peripheral and starts the receiver task.
 *        Receiver task parses frames and calls uCanvasServer_HandleFrame().
 */
bool uCanvasServer_UART_Init(const uCanvasServer_UART_Config_t* cfg);

/**
 * @brief Sends a complete frame back to host (used for RSP_* / EVT_*).
 *        Builds SOF/LEN/CMD/PAYLOAD/CRC and writes it out over UART.
 */
void uCanvasServer_UART_SendFrame(uint8_t cmd, const uint8_t* payload, uint16_t len);

/* Default config helper - fills sensible defaults, caller can override fields */
uCanvasServer_UART_Config_t uCanvasServer_UART_DefaultConfig(void);

#ifdef __cplusplus
}
#endif

#endif /* UCANVAS_SERVER_UART_H_ */
