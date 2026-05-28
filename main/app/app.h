#ifndef __app_c__
#define __app_c__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <unistd.h>
#include <time.h>
#include "math.h"
#include "esp_log.h"
#include "uCanvas_api.h"
#include "uCanvas_Physix.h" 
#include "uCanvas_User_IO.h"
#define ENC_A       39
#define ENC_B       40
#define ENC_SW      37
#define PB1         36
#define PB2         35
#define USE_USB_KEYBOARD 1
void uCanvas_Setup(void);
void uCanvas_App_Main(void);

#endif