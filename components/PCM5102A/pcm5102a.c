// #include "pcm5102a.h"

// #define BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT

// #define TONE_FREQ       (60.0f)  // Hz
// #define PI              (3.14159265f)
// #define AMPLITUDE       (30000)   // peak amplitude for 16-bit

// static void i2s_init(void)
// {
//     i2s_config_t i2s_config = {
//         .mode = I2S_MODE_MASTER | I2S_MODE_TX,
//         .sample_rate = SAMPLE_RATE,
//         .bits_per_sample = BITS_PER_SAMPLE,
//         .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
//         .communication_format = I2S_COMM_FORMAT_I2S,
//         .dma_buf_count = 4,
//         .dma_buf_len = 256,
//         .use_apll = true,  // enable APLL for better MCLK stability
//         .intr_alloc_flags = 0
//     };

//     i2s_pin_config_t pin_config = {
//         .mck_io_num = I2S_MCK_IO,
//         .bck_io_num = I2S_BCK_IO,
//         .ws_io_num = I2S_WS_IO,
//         .data_out_num = I2S_DO_IO,
//         .data_in_num = I2S_PIN_NO_CHANGE
//     };

//     i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
//     i2s_set_pin(I2S_NUM, &pin_config);
//     i2s_zero_dma_buffer(I2S_NUM);
// }

// static void tone_task(void *arg)
// {
//     const int samples_per_cycle = SAMPLE_RATE / TONE_FREQ;
//     int16_t *buffer = malloc(samples_per_cycle * 4); // stereo, 2 bytes per channel
//     if (!buffer) vTaskDelete(NULL);

//     for (int i = 0; i < samples_per_cycle; i++) {
//         float theta = (2.0f * PI * i) / samples_per_cycle;
//         int16_t sample = (int16_t)(AMPLITUDE * sinf(theta));
//         buffer[2 * i]     = sample; // Left
//         buffer[2 * i + 1] = sample; // Right
//     }

//     while (1) {
//         size_t bytes_written;
//         i2s_write(I2S_NUM, buffer, samples_per_cycle * 4, &bytes_written, portMAX_DELAY);
//     }
// }

// void pcm5102_demo(void)
// {
//     i2s_init();
//     xTaskCreate(tone_task, "tone_task", 4096, NULL, 5, NULL);
// }
