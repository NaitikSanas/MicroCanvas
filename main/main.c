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


// extern u8g2_t u8g2;

void app_main(){
    start_uCanvas_engine();

    uCanvas_Scene_t* main_scene = create_scene();
    uCanvas_set_active_scene(main_scene);
    
    uCanvas_universal_obj_t* bars[4] = {0};
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
            vTaskDelay(100);
        }
        for (int i = 3; i >= 0; i--)
        {
            char level[32];
            sprintf(level,"%d",i);
            uCanvas_Set_Text(current_level,level);
            uCanvas_Set_Fill(bars[i],NOFILL);
            vTaskDelay(100);
        }
    }    
}
