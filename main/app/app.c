#include "app.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "uCanvas_Viewport.h"
#include "uCanvas_display_port.h"
uint16_t window_object_buf1[100*80]={0x00FF00};
uint16_t window_object_buf2[128*50]={0x00FF00};

void uCanvas_Setup() {
    start_uCanvas_engine();
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    uCanvas_universal_obj_t* r = New_uCanvas_2DRectangle(0,0,320,240);
    r->properties.fill = FILL;
    uCanvas_Set_Color(r,255,255,255);

    uCanvas_Viewport_Instance window_1 = {
        .canvas_height = 80,
        .canvas_width = 100,
        .canvas_position_x = 20,
        .canvas_position_y = 20
    };

    uCanvas_universal_obj_t* window_obj_handle = uCanvas_Create_Viewport(&window_1,window_object_buf1,100,80);
    window_obj_handle->properties.use_rgba_to_rgb_conv = 1;
    
    color_t c;
    c.red   = 250;
    c.green = 250;
    c.blue  = 0;

    uCanvas_Viewport_DrawRect(&window_1,1,1,50,50,convertToRGB565(c),1);

    c.red   = 250;
    c.green = 0;
    c.blue  = 0;
    uCanvas_Viewport_DrawLine(&window_1,0,0,100,80,convertToRGB565(c));

    uCanvas_Viewport_Instance window_2 = {
        .canvas_position_x = 0,
        .canvas_position_y = 20
    };
    uCanvas_universal_obj_t* text = New_uCanvas_2DTextbox("Animated Viewport",10,170);
    uCanvas_Set_Color(text,0,0,0);
    text->font_properties.font_type = FONT_24G;

    uCanvas_universal_obj_t* window_obj_handle2 = uCanvas_Create_Viewport(&window_2,window_object_buf2,128,50);
    window_obj_handle2->properties.use_rgba_to_rgb_conv = 1;
    window_obj_handle2->properties.position.x = 20;
    window_obj_handle2->properties.position.y = 200; 

    int pos_x = 0;
    while (1)
    {
        if(pos_x < window_2.canvas_width){
            pos_x++;
        }
        else pos_x = -50;

        uCanvas_Viewport_Fill(&window_2,0);
        uCanvas_Viewport_DrawRect(&window_2,pos_x,0,50,50,convertToRGB565(c),1);
        uCanvas_Delay(2);
        
    }
    
   
}

void uCanvas_App_Main(void) {
    uCanvas_Delay(1);
}