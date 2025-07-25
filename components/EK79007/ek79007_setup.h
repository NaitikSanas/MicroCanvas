#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_ldo_regulator.h"
#include "esp_dma_utils.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_ek79007.h"
#include "string.h"

#define LCD_H_RES                  (1024)
#define LCD_V_RES                  (600)
#define LCD_BYTE_PER_PIXEL          (2)
#define LCD_BIT_PER_PIXEL          (16)
#define PIN_NUM_LCD_RST            (-1)
#define PIN_NUM_BK_LIGHT           (-1)    // set to -1 if not used
#define LCD_BK_LIGHT_ON_LEVEL      (1)
#define LCD_BK_LIGHT_OFF_LEVEL     !LCD_BK_LIGHT_ON_LEVEL
#define MIPI_DSI_LANE_NUM          (2)
#define PIN_NUM_VER_FLIP           (-1)
#define PIN_NUM_HOR_FLIP           (-1)
#define LCD_ROTATE_LEVEL           (1)
#define MIPI_DPI_PX_FORMAT         (LCD_COLOR_PIXEL_FORMAT_RGB565)
#define MIPI_DSI_PHY_PWR_LDO_CHAN          (3)
#define MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV    (2500)


void ek79007_deinit(void);
void ek79007_init(void);
void ek79007_draw_tile(int x, int y, int tile_w, int tile_h, uint16_t* pixel_data);
//need method to get refresh_finish semaphore