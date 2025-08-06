#include "ek79007_setup.h"


static char *TAG = "ek79007_test";
static esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
static esp_lcd_panel_io_handle_t mipi_dbi_io = NULL;
SemaphoreHandle_t refresh_finish = NULL;


IRAM_ATTR static bool notify_refresh_ready(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx)
{
    SemaphoreHandle_t refresh_finish = (SemaphoreHandle_t)user_ctx;
    BaseType_t need_yield = pdFALSE;

    xSemaphoreGiveFromISR(refresh_finish, &need_yield);

    return (need_yield == pdTRUE);
}

void ek79007_init(void)
{
#if PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT
    };
    gpio_config(&bk_gpio_config);
    gpio_set_level(PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);
#endif

    // Turn on the power for MIPI DSI PHY, so it can go from "No Power" state to "Shutdown" state
#ifdef MIPI_DSI_PHY_PWR_LDO_CHAN
    ESP_LOGI(TAG, "MIPI DSI PHY Powered on");
    esp_ldo_channel_config_t ldo_mipi_phy_config = {
        .chan_id = MIPI_DSI_PHY_PWR_LDO_CHAN,
        .voltage_mv = MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
    };
    esp_ldo_acquire_channel(&ldo_mipi_phy_config, &ldo_mipi_phy);
#endif

    ESP_LOGI(TAG, "Initialize MIPI DSI bus");
    esp_lcd_dsi_bus_config_t bus_config = EK79007_PANEL_BUS_DSI_2CH_CONFIG();
    esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus);

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_dbi_io_config_t dbi_config = EK79007_PANEL_IO_DBI_CONFIG();
    esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io);

    ESP_LOGI(TAG, "Install LCD driver of ek79007");
    esp_lcd_dpi_panel_config_t dpi_config = EK79007_1024_600_PANEL_60HZ_CONFIG(MIPI_DPI_PX_FORMAT);
    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
            .lane_num = MIPI_DSI_LANE_NUM,
        },
    };
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = LCD_BIT_PER_PIXEL,
        .vendor_config = &vendor_config,
    };
    esp_lcd_new_panel_ek79007(mipi_dbi_io, &panel_config, &panel_handle);
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);

    refresh_finish = xSemaphoreCreateBinary();
    xSemaphoreGive(refresh_finish);
    esp_lcd_dpi_panel_event_callbacks_t cbs = {
        .on_color_trans_done = notify_refresh_ready,
    };
    esp_lcd_dpi_panel_register_event_callbacks(panel_handle, &cbs, refresh_finish);
}

void ek79007_deinit(void)
{
    esp_lcd_panel_del(panel_handle);
    esp_lcd_panel_io_del(mipi_dbi_io);
    esp_lcd_del_dsi_bus(mipi_dsi_bus);
    panel_handle = NULL;
    mipi_dbi_io = NULL;
    mipi_dsi_bus = NULL;

    if (ldo_mipi_phy) {
        esp_ldo_release_channel(ldo_mipi_phy);
        ldo_mipi_phy = NULL;
    }

    vSemaphoreDelete(refresh_finish);
    refresh_finish = NULL;

#if PIN_NUM_BK_LIGHT >= 0
    gpio_reset_pin(PIN_NUM_BK_LIGHT);
#endif
}

void lcd_draw_rect_rgb888(esp_lcd_panel_handle_t panel, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    // Clamp to screen bounds
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if ((x + w) > LCD_H_RES) w = LCD_H_RES - x;
    if ((y + h) > LCD_V_RES) h = LCD_V_RES - y;
    if (w <= 0 || h <= 0) return;

    size_t buf_size = w * h * 3; // RGB888 = 3 bytes per pixel
    uint8_t *rect_buf = (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
    if (!rect_buf) {
        ESP_LOGE(TAG, "Failed to allocate rect buffer");
        return;
    }

    // Fill buffer with color
    for (int i = 0; i < w * h; i++) {
        rect_buf[i * 3 + 0] = r;
        rect_buf[i * 3 + 1] = g;
        rect_buf[i * 3 + 2] = b;
    }

    // Draw just the updated rectangle
    esp_lcd_panel_draw_bitmap(panel, x, y, x + w, y + h, rect_buf);

    heap_caps_free(rect_buf);
}

void ek79007_draw_tile(int x, int y, int tile_w, int tile_h, uint16_t* pixel_data){
    // printf("co-ord(%d,%d)-(%d,%d)\r\n", x, y, x + tile_w, y + tile_h);
    xSemaphoreTake(refresh_finish,portMAX_DELAY);
    esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + tile_w, y + tile_h, pixel_data);
}