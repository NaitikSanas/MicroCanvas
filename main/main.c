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

int solar_system_center_x = 60, 
    solar_system_center_y = 40;

void uCanvas_Animation_task_Planet_1(void*arg){
    printf("Animation_Task1\r\n");
    uCanvas_universal_obj_t* character = create_circle(solar_system_center_x,solar_system_center_y,4);    
    while (1)
    {
        for (int i = 0; i < 360; ++i) {
            double theta = 2.0 * PI * i / 360;
            double x = 40 * cos(theta) + solar_system_center_x;
            double y = 20 * sin(theta) + solar_system_center_y;
            uCanvas_Set_Position(character,x,y);
            vTaskDelay(40/portTICK_PERIOD_MS);
        }     
    }
}

void uCanvas_Animation_task_Planet_2(void*arg){
    printf("Animation_Task2\r\n");
    uCanvas_universal_obj_t* character = create_circle(64,40,2);
    while (1)
    {
        for (int i = 360; i > 0; i--) {
            double theta = 2.0 * PI * i / 360;
            double x = 10 * cos(theta) + solar_system_center_x;
            double y = 21 * sin(theta) + solar_system_center_y;
            uCanvas_Set_Position(character,x,y);
            vTaskDelay(10/portTICK_PERIOD_MS);
        }     
    }
}

void uCanvas_Animation_task_Planet_3(void*arg){
    printf("Animation_Task2\r\n");
    uCanvas_universal_obj_t* character = create_circle(64,40,2);
    while (1)
    {
        for (int i = 360; i > 0; i--) {
            double theta = 2.0 * PI * i / 360;
            double x = 50 * cos(theta) + solar_system_center_x;
            double y = 10 * sin(theta) + solar_system_center_y;
            uCanvas_Set_Position(character,x,y);
            vTaskDelay(10/portTICK_PERIOD_MS);
        }     
    }
}

void Control_Level_bar_task(void*arg){
    uCanvas_universal_obj_t* bars[4] = {0}; //Array 4 Bars.
    uint8_t xpos = 45, ypos = 0, width = 5, height = 10;
    bars[0] = create_rectangle(xpos+0, ypos,height,width);   
    bars[1] = create_rectangle(xpos+8, ypos,height,width); 
    bars[2] = create_rectangle(xpos+16,ypos,height,width); 
    bars[3] = create_rectangle(xpos+24,ypos,height,width); 

    create_text_box("LEVEL:",0,0);
    uCanvas_universal_obj_t* current_level = create_text_box(".",xpos+24+10,0);
    while (1)
    {
        for (int i = 0; i < 4; i++)
        {
            char level[32];
            sprintf(level,"%d",i);
            uCanvas_Set_Text(current_level,level);
            uCanvas_Set_Fill(bars[i],FILL);
            vTaskDelay(20);
        }
        for (int i = 3; i >= 0; i--)
        {
            char level[32];
            sprintf(level,"%d",i);
            uCanvas_Set_Text(current_level,level);
            uCanvas_Set_Fill(bars[i],NOFILL);
            vTaskDelay(20);
        }
    }
}

void app_main(){
    start_uCanvas_engine();

    uCanvas_Scene_t* main_scene = create_scene();
    uCanvas_set_active_scene(main_scene);
    
    create_circle(solar_system_center_x,solar_system_center_y,8); //Create static Sun
    uCanvas_Add_Task(uCanvas_Animation_task_Planet_1); //Create Planet Animation Loop thread
    uCanvas_Add_Task(uCanvas_Animation_task_Planet_2); //Create Planet Animation Loop thread
    uCanvas_Add_Task(uCanvas_Animation_task_Planet_3); //Create Planet Animation Loop thread  

    uCanvas_Add_Task(Control_Level_bar_task); //Just animate Level bar for example.  
}