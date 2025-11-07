#include "esp_lcd_touch_gt911.h"
#define BSP_I2C_SCL           (GPIO_NUM_8)
#define BSP_I2C_SDA           (GPIO_NUM_7)
#define BSP_LCD_H_RES              (1024)
#define BSP_LCD_V_RES              (600)
#define BSP_LCD_BACKLIGHT     (GPIO_NUM_23)
#define BSP_LCD_RST           (GPIO_NUM_NC)
#define BSP_LCD_TOUCH_RST     (GPIO_NUM_NC)
#define BSP_LCD_TOUCH_INT     (GPIO_NUM_NC)
esp_err_t bsp_touch_new(esp_lcd_touch_handle_t *ret_touch);