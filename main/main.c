#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "ssd1306.h"
#include "fonts.h"
#include "esp_log.h"

#include "MicroCanvas2D.h"
#include <unistd.h>
#include <time.h>
#include "math.h"

#define PI 3.14159265358979323846
int naresh  = 64; 
int center_x  = 38; 
int center_y  = 38; 
int radius    = 15; 
int numPoints = 360;

int random_int(int min, int max) {
    if (min > max) {
        // Ensure that min is not greater than max
        int temp = min;
        min = max;
        max = temp;
    }
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

/**
 * Bar graph is collection of ucanvas_2d_obj. 
 * which has its own x,y position,
 * number of bars, 
 * datapoints which adjusts hight of each bars respect to its value.
*/
typedef struct bargraph{
    int bars_to_render;
    int x_pos;
    int y_pos;
    ucanvas_2d_obj* bar[32];
    int datapoints[32];
}BarGraph_Obj_t;

void create_bargraph(BarGraph_Obj_t* obj){
    for(int i = 0; i<obj->bars_to_render;i++){
        obj->bar[i] = create_rectangle(5 ,1, i*obj->x_pos,obj->y_pos-1,NOFILL);      
    }
}

void add_datapoints(BarGraph_Obj_t* obj, int* buf){
    for(int i = 0; i<obj->bars_to_render;i++){
        obj->bar[i]->x1 = buf[i];
        obj->bar[i]->pos_y = obj->y_pos-buf[i]; 
    }
}

void app_main(){
    start_geomatery_engine();
    srand(xTaskGetTickCount());

    // page_t* page1 = (page_t*)malloc(sizeof(page_t));
    // page_init(page1);
    // set_current_page(page1);

    // BarGraph_Obj_t* bar_graph = (BarGraph_Obj_t*)malloc(sizeof(BarGraph_Obj_t));
    // bar_graph->bars_to_render = 10;
    // bar_graph->x_pos = 10;
    // bar_graph->y_pos = 64;
    // create_bargraph(bar_graph);

    // int data_points[10];
    // int offset = 0;
    // while (1)
    // {
    //     int dptr = 0;
    //     for (int i = offset; i < 10+offset; i+=1) {
    //         double theta = 2 * PI * i / 10;
    //         data_points[dptr] = 10 + (int)(radius * cos(theta));
    //         dptr++;
            
    //     }
    //     offset++;
    //     add_datapoints(bar_graph,data_points);
    //     vTaskDelay(9);
    // }
    
    srand(xTaskGetTickCount());

    page_t* page1 = (page_t*)malloc(sizeof(page_t));
    page_init(page1);
    set_current_page(page1);

    ucanvas_2d_obj* pointer_1 = create_line(center_x,center_y,center_x,radius);
    ucanvas_2d_obj* pointer_2 = create_line(center_x,center_y,center_x,radius-5);
    create_circle(center_x,center_y,radius,NOFILL);  
    
    ucanvas_2d_obj* circ  = create_circle(center_x,center_y,radius-5,NOFILL);
    ucanvas_2d_obj* label = create_textbox("",&Font_7x10,0,0);
    while (1)
    {
        
        for (int i = 0; i < numPoints; i++) {
            double theta = 2 * PI * i / numPoints;
            int x1 = center_x + (int)(radius * cos(theta));
            int y1 = center_y + (int)(radius * sin(theta));
            pointer_1->x2 = x1;
            pointer_1->y2 = y1;

            int x2 = x1 + (int)((radius-5) * cos(theta - 80));
            int y2 = y1 + (int)((radius-5) * sin(theta - 80));

            circ->pos_x = x1;
            circ->pos_y = y1;

            pointer_2->x1 = x1;
            pointer_2->y1 = y1;
            pointer_2->x2 = x2;
            pointer_2->y2 = y2;

            sprintf(label->str,"P1=%d:%d|P2=%d:%d", x1,y1,x2,y2);
            Vector2D_t vec = get_distance(pointer_1,pointer_2);
            
            vTaskDelay(400 / numPoints);           
        }
    }
}