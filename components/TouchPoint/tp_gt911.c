#include "esp_lcd_touch_gt911.h"
#include "tp_gt911.h"
#include "driver/i2c_master.h"
static i2c_master_bus_handle_t i2c_handle = NULL;  // I2C Handle
static esp_lcd_panel_io_handle_t  tp_io_handle  = NULL;

esp_err_t bsp_touch_new(esp_lcd_touch_handle_t *ret_touch)
{
    /* Initilize I2C */
        i2c_master_bus_config_t i2c_bus_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = BSP_I2C_SDA,
        .scl_io_num = BSP_I2C_SCL,
        .i2c_port = 1,
    };
    i2c_new_master_bus(&i2c_bus_conf, &i2c_handle);


    /* Initialize touch */
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = BSP_LCD_H_RES,
        .y_max = BSP_LCD_V_RES,
        .rst_gpio_num = BSP_LCD_TOUCH_RST, // Shared with LCD reset
        .int_gpio_num = BSP_LCD_TOUCH_INT,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
#if CONFIG_BSP_LCD_TYPE_1024_600
            .mirror_x = 1,
            .mirror_y = 1,
#else
            .mirror_x = 0,
            .mirror_y = 0,
#endif
        },
    };
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
    tp_io_config.scl_speed_hz = 400000;
    esp_lcd_new_panel_io_i2c(i2c_handle, &tp_io_config, &tp_io_handle);
    return esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, ret_touch);
}
