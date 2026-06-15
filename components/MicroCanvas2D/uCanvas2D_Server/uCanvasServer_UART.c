/**
 * @file uCanvasServer_UART.c
 * @brief UART transport + frame parser implementation (ESP-IDF)
 */

#include <string.h>
#include "uCanvasServer_UART.h"
#include "uCanvasServer_Core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char* TAG = "uCanvasServer_UART";

/* Module-local state */
static uart_port_t s_uart_num;
static SemaphoreHandle_t s_tx_lock;

/* ------------------------------------------------------------------ */
/* CRC8 (poly 0x07, init 0x00) - matches dispatcher / host-side impl   */
/* ------------------------------------------------------------------ */
static uint8_t crc8(const uint8_t* data, uint16_t len)
{
    uint8_t crc = 0x00;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++) {
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x07) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}

/* ------------------------------------------------------------------ */
/* Parser state machine                                                 */
/* ------------------------------------------------------------------ */
typedef enum {
    PARSE_WAIT_SOF,
    PARSE_LEN_LO,
    PARSE_LEN_HI,
    PARSE_CMD,
    PARSE_PAYLOAD,
    PARSE_CRC
} parse_state_t;

typedef struct {
    parse_state_t state;
    uint16_t      payload_len;   /* expected payload length from frame */
    uint16_t      payload_idx;   /* bytes of payload received so far */
    uint8_t       cmd;
    uint8_t       payload[UCS_MAX_PAYLOAD_LEN];
} parser_t;

static void parser_reset(parser_t* p)
{
    p->state = PARSE_WAIT_SOF;
    p->payload_len = 0;
    p->payload_idx = 0;
    p->cmd = 0;
}

/**
 * @brief Feed one byte into the parser. When a complete, CRC-valid frame
 *        is assembled, dispatches it to uCanvasServer_HandleFrame().
 */
static void parser_feed(parser_t* p, uint8_t byte)
{
    switch (p->state) {
        case PARSE_WAIT_SOF:
            if (byte == UCS_SOF) {
                p->state = PARSE_LEN_LO;
            }
            /* else: stay in WAIT_SOF, discard noise byte */
            break;

        case PARSE_LEN_LO:
            p->payload_len = byte;
            p->state = PARSE_LEN_HI;
            break;

        case PARSE_LEN_HI:
            p->payload_len |= ((uint16_t)byte << 8);
            if (p->payload_len > UCS_MAX_PAYLOAD_LEN) {
                ESP_LOGW(TAG, "Frame payload len %u exceeds max, dropping frame", p->payload_len);
                parser_reset(p);
                break;
            }
            p->state = PARSE_CMD;
            break;

        case PARSE_CMD:
            p->cmd = byte;
            p->payload_idx = 0;
            p->state = (p->payload_len == 0) ? PARSE_CRC : PARSE_PAYLOAD;
            break;

        case PARSE_PAYLOAD:
            p->payload[p->payload_idx++] = byte;
            if (p->payload_idx >= p->payload_len) {
                p->state = PARSE_CRC;
            }
            break;

        case PARSE_CRC: {
            /* Verify CRC over CMD + PAYLOAD */
            uint8_t buf[1 + UCS_MAX_PAYLOAD_LEN];
            buf[0] = p->cmd;
            if (p->payload_len > 0) {
                memcpy(&buf[1], p->payload, p->payload_len);
            }
            uint8_t expected = crc8(buf, 1 + p->payload_len);

            if (expected == byte) {
                uCanvasServer_HandleFrame(p->cmd, p->payload, p->payload_len);
            } else {
                ESP_LOGW(TAG, "CRC mismatch (cmd=0x%02X, got=0x%02X, expected=0x%02X)",
                         p->cmd, byte, expected);
                uint8_t err_payload[2] = { 0x01 /* status: error */, UCS_ERR_CRC };
                uCanvasServer_UART_SendFrame(RSP_ERROR, err_payload, sizeof(err_payload));
            }
            parser_reset(p);
            break;
        }

        default:
            parser_reset(p);
            break;
    }
}

/* ------------------------------------------------------------------ */
/* RX task                                                              */
/* ------------------------------------------------------------------ */
static void uart_rx_task(void* arg)
{
    static parser_t parser;
    parser_reset(&parser);

    uint8_t rx_buf[128];

    while (1) {
        int len = uart_read_bytes(s_uart_num, rx_buf, sizeof(rx_buf), pdMS_TO_TICKS(20));
        for (int i = 0; i < len; i++) {
            parser_feed(&parser, rx_buf[i]);
        }
    }
}

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */
uCanvasServer_UART_Config_t uCanvasServer_UART_DefaultConfig(void)
{
    
    // Before uCanvasServer_UART_Init():

    uCanvasServer_UART_Config_t cfg = {
        
        .uart_num      = UART_NUM_1,
        .tx_pin        = 32,
        .rx_pin        = 36,
        .baud_rate     = 250000,
        .rx_buf_size   = 1024,
        .tx_buf_size   = 1024,
        .task_priority = 10,
        .task_core_id  = 0,
    };

    // cfg.uart_num  = UART_NUM_0;
    // cfg.tx_pin    = UART_PIN_NO_CHANGE; // keep default console pins
    // cfg.rx_pin    = UART_PIN_NO_CHANGE;
    // cfg.baud_rate = 115200; // match console baud unless you reconfigure
    return cfg;
}

bool uCanvasServer_UART_Init(const uCanvasServer_UART_Config_t* cfg)
{
    s_uart_num = cfg->uart_num;

    uart_config_t uart_cfg = {
        .baud_rate = cfg->baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    esp_err_t err;

    err = uart_driver_install(cfg->uart_num, cfg->rx_buf_size, cfg->tx_buf_size, 0, NULL, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "uart_driver_install failed: %d", err);
        return false;
    }

    err = uart_param_config(cfg->uart_num, &uart_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "uart_param_config failed: %d", err);
        return false;
    }

    err = uart_set_pin(cfg->uart_num, cfg->tx_pin, cfg->rx_pin,
                        UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "uart_set_pin failed: %d", err);
        return false;
    }

    s_tx_lock = xSemaphoreCreateMutex();
    if (s_tx_lock == NULL) {
        ESP_LOGE(TAG, "Failed to create TX mutex");
        return false;
    }

    BaseType_t ok = xTaskCreatePinnedToCore(
        uart_rx_task,
        "uCanvasServer_UART_RX",
        4096,
        NULL,
        cfg->task_priority,
        NULL,
        cfg->task_core_id
    );

    if (ok != pdPASS) {
        ESP_LOGE(TAG, "Failed to create UART RX task");
        return false;
    }

    ESP_LOGI(TAG, "uCanvas Server UART initialized: port=%d baud=%d tx=%d rx=%d",
             cfg->uart_num, cfg->baud_rate, cfg->tx_pin, cfg->rx_pin);

    return true;
}

void uCanvasServer_UART_SendFrame(uint8_t cmd, const uint8_t* payload, uint16_t len)
{
    if (len > UCS_MAX_PAYLOAD_LEN) {
        ESP_LOGW(TAG, "SendFrame: payload len %u exceeds max, truncating", len);
        len = UCS_MAX_PAYLOAD_LEN;
    }

    /* Build frame: SOF LEN_LO LEN_HI CMD PAYLOAD CRC */
    uint8_t frame[3 + 1 + UCS_MAX_PAYLOAD_LEN + 1];
    uint16_t idx = 0;

    frame[idx++] = UCS_SOF;
    frame[idx++] = (uint8_t)(len & 0xFF);
    frame[idx++] = (uint8_t)((len >> 8) & 0xFF);
    frame[idx++] = cmd;

    if (len > 0 && payload != NULL) {
        memcpy(&frame[idx], payload, len);
        idx += len;
    }

    /* CRC over CMD + PAYLOAD */
    uint8_t crc_buf[1 + UCS_MAX_PAYLOAD_LEN];
    crc_buf[0] = cmd;
    if (len > 0 && payload != NULL) {
        memcpy(&crc_buf[1], payload, len);
    }
    frame[idx++] = crc8(crc_buf, 1 + len);

    if (xSemaphoreTake(s_tx_lock, pdMS_TO_TICKS(100)) == pdTRUE) {
        uart_write_bytes(s_uart_num, (const char*)frame, idx);
        xSemaphoreGive(s_tx_lock);
    } else {
        ESP_LOGW(TAG, "SendFrame: TX lock timeout, dropping frame (cmd=0x%02X)", cmd);
    }
}
