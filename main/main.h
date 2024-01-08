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
#define PI 3.14159265358979323846

void uCanvas_Animation_task_Planet_1(void*arg);
void uCanvas_Animation_task_Planet_2(void*arg);
void uCanvas_Animation_task_Planet_3(void*arg);
void Control_Level_bar_task(void*arg);
_2dPoint_t get_xy_cordinates(int angle, int rx, int ry, int xoffset, int yoffset);
