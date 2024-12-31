#include "vector_graphics_app.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "uCanvas_Physix.h"
// #include "GUI_Demo.h"
int app_state = 0;
extern void color_wipe(uCanvas_universal_obj_t* obj, color_t from_color, color_t to_color, uint32_t delay_us, uint16_t steps) ;
uCanvas_universal_obj_t* l1;
uCanvas_universal_obj_t* l2;
uCanvas_universal_obj_t* l3;
uCanvas_universal_obj_t* c1;
uCanvas_universal_obj_t* c2;
uCanvas_universal_obj_t* c3;
uCanvas_universal_obj_t* text;
extern uCanvas_universal_obj_t* panel_1;

void vector_graphics_app_task(){
    int angle = 0;
    char buf[32]={0};
    while (1)
    {
        if(app_state){
            c1->properties.visiblity = VISIBLE;
            c2->properties.visiblity = VISIBLE;
            c3->properties.visiblity = VISIBLE;
            l1->properties.visiblity = VISIBLE;
            l2->properties.visiblity = VISIBLE;
            l3->properties.visiblity = VISIBLE;
            text->properties.visiblity = VISIBLE;
            while (app_state)
            {
                Coordinate2D_t cordinates1 = ucanvas_get_xy_circular_cordinates((angle+90)/2,60,50,240/2,320/2);
                Coordinate2D_t cordinates2 = ucanvas_get_xy_circular_cordinates(angle,50,30,cordinates1.x,cordinates1.y);
                uCanvas_Set_Position(c2,cordinates1.x,cordinates1.y);
                c3->properties.position = cordinates2;
                l1->point1 = cordinates1;
                l1->point2 = c1->properties.position;

                l2->point1 = c2->properties.position;
                l2->point2 = c3->properties.position;
                
                l3->point1 = c3->properties.position;
                l3->point2 = c1->properties.position;
                angle++;
                sprintf(buf,"xy = (%d,%d)",cordinates1.x,cordinates1.y);
                uCanvas_Set_Text(text,buf);
                uCanvas_Delay(1);
            }
            
        }else{
            c1->properties.visiblity = INVISIBLE;
            c2->properties.visiblity = INVISIBLE;
            c3->properties.visiblity = INVISIBLE;
            l1->properties.visiblity = INVISIBLE;
            l2->properties.visiblity = INVISIBLE;
            l3->properties.visiblity = INVISIBLE;
            text->properties.visiblity = INVISIBLE;
        }
        uCanvas_Delay(1);
    }
}

void start_vector_graphics_app(){
    //Create Demo App.
    l1 = New_uCanvas_2DLine(240/2,320/2,(240/2)+50,(320/2)+20);
    l2 = New_uCanvas_2DLine(240/2,320/2,(240/2)+50,(320/2)+20);
    l3 = New_uCanvas_2DLine(240/2,320/2,(240/2)+50,(320/2)+20);
    c1 = New_uCanvas_2DCircle(240/2,320/2,14);
    c2 = New_uCanvas_2DCircle((240/2)+50,(320/2)+20,10);
    c3 = New_uCanvas_2DCircle((240/2)+100,(320/2)+50,5);
   
    text = New_uCanvas_2DTextbox("",10,80);
    text->font_properties.font_type = FONT_32L;
    uCanvas_Set_Color(text,255,255,0);
    

    c1->properties.fill = FILL;
    c2->properties.fill = FILL;
    c3->properties.fill = FILL;

    uCanvas_Set_Color(c1,200,150,2);
    uCanvas_Set_Color(c2,0,255,0);
    uCanvas_Set_Color(c3,150,0,255);
    
    uCanvas_Set_Color(l1,255,0,0);
    uCanvas_Set_Color(l2,255,255,0);
    uCanvas_Set_Color(l3,255,255,255);
    
    text->properties.visiblity = INVISIBLE;
    c1->properties.visiblity = INVISIBLE;
    c2->properties.visiblity = INVISIBLE;
    c3->properties.visiblity = INVISIBLE;
    l1->properties.visiblity = INVISIBLE;
    l2->properties.visiblity = INVISIBLE;
    l3->properties.visiblity = INVISIBLE;
    uCanvas_Add_Task(vector_graphics_app_task,NULL,0);
    
}

void toggle_graphics_app_test(){
    if(panel_1->properties.visiblity==INVISIBLE){
        panel_1->properties.visiblity = VISIBLE;
        panel_1->properties.position.y = 300;
        while (panel_1->properties.position.y != 30)
        {
            panel_1->properties.position.y -= 1;
            ets_delay_us(800);
        }
        color_wipe(panel_1,panel_1->properties.color,(color_t){.red = 0, 0, 0},600,100);
        app_state = 1;
    }
    else
    if(panel_1->properties.visiblity==VISIBLE){
        app_state = 0;
        panel_1->properties.position.y = 30;
        color_wipe(panel_1,panel_1->properties.color,(color_t){.red = 50, 0, 0},600,100);
        while (panel_1->properties.position.y != 300)
        {
            panel_1->properties.position.y += 1;
            ets_delay_us(800);
        }
        
        panel_1->properties.visiblity = INVISIBLE;
        
    }
}