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


#define PB0_PIN 20

typedef struct spikes
{
    uCanvas_universal_obj_t* spikes[4];
    int distance_x;
}spikes_t;

typedef enum{
    IDLE = 0,
    GAME_STARTED,
    GAME_STOPPED
}game_steate_t;

typedef enum{
    PLAYER_IDLE=0,
    PLAYER_ALIVE,
    PLAYER_DEAD
}player_state_t;

player_state_t player_state = IDLE;
game_steate_t game_state = IDLE;

void noob_man1_controller();
Coordinate2D_t get_xy_cordinates(int angle, int rx, int ry, int xoffset, int yoffset);
int get_random_number(int min, int max);