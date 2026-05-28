#include "uCanvas2D_ST7789_Port.h"
#include "driver/ledc.h"
static TFT_t st7789_dev_instance;

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (CONFIG_BL_GPIO) // Choose the GPIO pin connected to your LED
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // 13-bit resolution
#define LEDC_FREQUENCY          (5000) // Frequency in Hz

static void ST7789_Init(uCanvas2D_Display_Orientation_t orientation){
    printf("ST7789_Init prototype\r\n");
    // ledc_timer_config_t ledc_timer = {
    //     .speed_mode       = LEDC_MODE,
    //     .timer_num        = LEDC_TIMER,
    //     .duty_resolution  = LEDC_DUTY_RES,
    //     .freq_hz          = LEDC_FREQUENCY,
    //     .clk_cfg          = LEDC_AUTO_CLK
    // };
    // ledc_timer_config(&ledc_timer);

    // Configure PWM channel
    // ledc_channel_config_t ledc_channel = {
    //     .speed_mode     = LEDC_MODE,
    //     .channel        = LEDC_CHANNEL,
    //     .timer_sel      = LEDC_TIMER,
    //     .intr_type      = LEDC_INTR_DISABLE,
    //     .gpio_num       = LEDC_OUTPUT_IO,
    //     .duty           = 0, // Start with LED off
    //     .hpoint         = 0
    // };
    // ledc_channel_config(&ledc_channel);
	// ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
	// ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

    spi_master_init(&st7789_dev_instance, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, -1);   
	lcdInit(&st7789_dev_instance, CONFIG_WIDTH, CONFIG_HEIGHT, 0, 0);
    ST7789_Set_Orientation(&st7789_dev_instance,orientation);

	// //Ramp up brightness of backlight to max smoothly on start up.
	// for (int duty = 0; duty <= 8191; duty += 128) {
	// 	ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
	// 	ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
	// 	vTaskDelay(pdMS_TO_TICKS(15));
	// }
    
}

static void ST7789_SetAddressWindow(TFT_t *st7789_dev_instance, int x0, int y0, int x1, int y1) {
    // Column address set
    spi_master_write_command(st7789_dev_instance, 0x2A); // CASET
    spi_master_write_data_word(st7789_dev_instance, x0);
    spi_master_write_data_word(st7789_dev_instance, x1);

    // Row address set
    spi_master_write_command(st7789_dev_instance, 0x2B); // RASET
    spi_master_write_data_word(st7789_dev_instance, y0);
    spi_master_write_data_word(st7789_dev_instance, y1);

    // Write to RAM
    spi_master_write_command(st7789_dev_instance, 0x2C); // RAMWR
}

typedef struct {
    uint16_t* pixels;
    int w; 
    int h;
    int update_display
}st7789_async_write_t;
st7789_async_write_t async_write_handle;

static void ST7789_PushTile(int x, int y, uCanvas2D_RenderBuffer_t* buf){
    if (!buf || !buf->pixels) return;

    const uint16_t* pixels = (const uint16_t*) buf->pixels;
    int w = buf->width;
    int h = buf->height;
    int pitch = buf->pitch;
    if(pitch <= 0) pitch = w;

    ST7789_SetAddressWindow(&st7789_dev_instance, x, y, x + w - 1, y + h - 1);
    // Send pixels in safe chunks; handle pitch/stride if needed.
    const int maxPixelsPerChunk = 512;
    if(pitch == w){
        int totalPixels = w * h;
        int sent = 0;
        while (sent < totalPixels) {
            int chunkSize = (totalPixels - sent > maxPixelsPerChunk) ? maxPixelsPerChunk : (totalPixels - sent);
            spi_master_write_colors(&st7789_dev_instance, (uint16_t*)&pixels[sent], chunkSize);
            sent += chunkSize;
        }
    }else{
        for(int row = 0; row < h; row++){
            const uint16_t* row_pixels = &pixels[row * pitch];
            int sent = 0;
            while(sent < w){
                int chunkSize = (w - sent > maxPixelsPerChunk) ? maxPixelsPerChunk : (w - sent);
                spi_master_write_colors(&st7789_dev_instance, (uint16_t*)&row_pixels[sent], chunkSize);
                sent += chunkSize;
            }
        }
    }
}

static void ST7789_Adjust_Brightness(int percent){
    printf("ST7789_Adjust_Brightness prototype\r\n");
}

uCanvas2D_Display_Panel_t* uCanvas2D_Get_Panel_Driver_ST7789(void){
    uCanvas2D_Display_Panel_t* driver = (uCanvas2D_Display_Panel_t*) malloc(sizeof(uCanvas2D_Display_Panel_t));
    driver->height = CONFIG_HEIGHT;
    driver->width = CONFIG_WIDTH;
    driver->init = ST7789_Init;
    driver->push_render_buffer = ST7789_PushTile;
    driver->set_backlight = ST7789_Adjust_Brightness;
    return driver;
}

void uCanvas2D_GetPanel_Driver_ST7789(uCanvas2D_Display_Panel_t* driver){
    driver->height = CONFIG_HEIGHT;
    driver->width = CONFIG_WIDTH;
    driver->init = ST7789_Init;
    driver->push_render_buffer = ST7789_PushTile;
    driver->set_backlight = ST7789_Adjust_Brightness;
    // xTaskCreate(&Write_Pixels,"draw",1024*2,&async_write_handle,5,NULL);
}