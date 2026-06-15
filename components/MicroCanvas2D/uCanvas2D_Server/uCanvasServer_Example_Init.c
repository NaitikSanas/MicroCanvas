/**
 * @file uCanvasServer_Example_Init.c
 * @brief Example: wiring uCanvas Server (UART transport) into an ESP-IDF app
 *
 * Call this AFTER your uCanvas rendering engine is already running
 * (i.e. after New_uCanvas_Instance / Attach_Renderer / Attach_Scene etc.)
 */

#include "uCanvasServer_Core.h"
#include "uCanvasServer_UART.h"

void uCanvasServer_Start(void)
{
    /* 1. Init core (handle table, scene table) */
    uCanvasServer_Core_Init();

    /* 2. Bring up UART transport */
    uCanvasServer_UART_Config_t uart_cfg = uCanvasServer_UART_DefaultConfig();
    /* Override pins/baud here if needed, e.g.: */
    // uart_cfg.tx_pin = 17;
    // uart_cfg.rx_pin = 18;
    // uart_cfg.baud_rate = 921600;

    uCanvasServer_UART_Init(&uart_cfg);

    /* 3. Bind core's response/event output to UART send */
    uCanvasServer_SetSendFunc(uCanvasServer_UART_SendFrame);

    /* From here on, the UART RX task parses incoming frames and
       dispatches them via uCanvasServer_HandleFrame() automatically. */
}
