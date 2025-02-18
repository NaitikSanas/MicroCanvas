#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#define ESP_MAXIMUM_RETRY 10
void wifi_connect(const char *ssid, const char *password);
void wifi_init(void);