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

void uCanvas_Setup(void);
void uCanvas_App_Main(void);


#define EZGIF_FRAME_016_HEIGHT 64
#define EZGIF_FRAME_016_WIDTH 64

#endif