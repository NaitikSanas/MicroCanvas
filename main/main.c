#include "main.h"

int solar_system_center_x = 60, 
    solar_system_center_y = 40;

void app_main(){
    start_uCanvas_engine();

    uCanvas_Scene_t* main_scene = New_uCanvas_Scene();
    uCanvas_Scene_t* splash = New_uCanvas_Scene();
    uCanvas_set_active_scene(splash);

    /**
     * Start up Splash screen Scene
    */
    uCanvas_universal_obj_t* wlcm_msg = New_uCanvas_2DTextbox("",0,32);
    uCanvas_Animate_Text_Reveal(wlcm_msg, "Welcome to uCanvas",6);
    uCanvas_Delay(200);

    /**
     * Main Animation Scene
    */
    uCanvas_set_active_scene(main_scene);

    New_uCanvas_2DCircle(solar_system_center_x,solar_system_center_y,8); //Create static Sun
    uCanvas_Add_Task(uCanvas_Animation_task_Planet_1); //Create Planet Animation Loop thread
    uCanvas_Add_Task(uCanvas_Animation_task_Planet_2); //Create Planet Animation Loop thread
    uCanvas_Add_Task(uCanvas_Animation_task_Planet_3); //Create Planet Animation Loop thread  
    uCanvas_Add_Task(Control_Level_bar_task); //Just animate Level bar for example.  
}


void uCanvas_Animation_task_Planet_1(void*arg){
    printf("Animation_Task1\r\n");
    uCanvas_universal_obj_t* Planet_1 = New_uCanvas_2DCircle(solar_system_center_x,solar_system_center_y,4);   

    while (1)
    {
        for (int i = 0; i < 360; ++i) {
            _2dPoint_t coordinates = get_xy_cordinates(i,40,20,solar_system_center_x,solar_system_center_y);
            uCanvas_Set_Position(Planet_1,coordinates.x,coordinates.y);
            uCanvas_Delay(40/portTICK_PERIOD_MS);
        }     
    }
}

void uCanvas_Animation_task_Planet_2(void*arg){
    printf("Animation_Task2\r\n");
    uCanvas_universal_obj_t* Planet_2 = New_uCanvas_2DCircle(64,40,2);
    while (1)
    {
        for (int i = 360; i > 0; i--) {
            _2dPoint_t coordinates = get_xy_cordinates(i,10,21,solar_system_center_x,solar_system_center_y);
            uCanvas_Set_Position(Planet_2,coordinates.x,coordinates.y);
            uCanvas_Delay(10/portTICK_PERIOD_MS);
        }     
    }
}

void uCanvas_Animation_task_Planet_3(void*arg){
    printf("Animation_Task3\r\n");
    uCanvas_universal_obj_t* Planet_3 = New_uCanvas_2DCircle(64,40,2);
    uCanvas_universal_obj_t* Line =     New_uCanvas_2DLine(0,0,0,0);
    uCanvas_universal_obj_t* distance = New_uCanvas_2DTextbox("",0,0);

    while (1)
    {
        for (int i = 360; i > 0; i--) {

            _2dPoint_t coordinates = get_xy_cordinates(i,50,10,solar_system_center_x,solar_system_center_y);
            char dist[16] = {0};
            sprintf(dist,"dst:%d",coordinates.x);
            uCanvas_Set_Text(distance,dist);
            uCanvas_Set_Position(distance,coordinates.x-9,coordinates.y-14);
            uCanvas_Set_Position(Planet_3,coordinates.x,coordinates.y);
            uCanvas_Set_Line_Coordinates(Line,solar_system_center_x,solar_system_center_y,coordinates.x,coordinates.y);
            uCanvas_Delay(10/portTICK_PERIOD_MS);
        }     
    }
}

void Control_Level_bar_task(void*arg){
    uCanvas_universal_obj_t* bars[4] = {0}; //Array 4 Bars.
    uint8_t xpos = 45, ypos = 0, width = 5, height = 10;
    bars[0] = New_uCanvas_2DRectangle(xpos+0, ypos,height,width);   
    bars[1] = New_uCanvas_2DRectangle(xpos+8, ypos,height,width); 
    bars[2] = New_uCanvas_2DRectangle(xpos+16,ypos,height,width); 
    bars[3] = New_uCanvas_2DRectangle(xpos+24,ypos,height,width); 
    New_uCanvas_2DTextbox("LEVEL:",0,0);
    uCanvas_universal_obj_t* current_level = New_uCanvas_2DTextbox(".",xpos+24+10,0);
    while (1)
    {
        for (int i = 0; i < 4; i++)
        {
            char level[32];
            sprintf(level,"%d",i);
            uCanvas_Set_Text(current_level,level);
            uCanvas_Set_Fill(bars[i],FILL);
            uCanvas_Delay(20);
        }
        for (int i = 3; i >= 0; i--)
        {
            char level[32];
            sprintf(level,"%d",i);
            uCanvas_Set_Text(current_level,level);
            uCanvas_Set_Fill(bars[i],NOFILL);
            uCanvas_Delay(20);
        }
    }
}

_2dPoint_t get_xy_cordinates(int angle, int rx, int ry, int xoffset, int yoffset){
    _2dPoint_t coordinates;
    double theta = 2.0 * PI * angle / 360;            
    uint8_t x = rx * cos(theta) + xoffset;
    uint8_t y = ry * sin(theta) + yoffset;
    coordinates.x = x;
    coordinates.y = y;
    return coordinates;
}