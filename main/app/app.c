#include "app.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"

uCanvas_universal_obj_t* rect;
slider_t v_slider;
slider_t p_slider;

void uCanvas_Setup() {
    start_uCanvas_engine();
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    
    uCanvas_Init_PushButton(45);
    uCanvas_Init_PushButton(47);
    uCanvas_Init_PushButton(48);
    
    uCanvas_universal_obj_t* bg = New_uCanvas_2DRectangle(0,0,320,240);
    bg->properties.fill = FILL;
    uCanvas_Set_Color(bg,10,4,0);
    
    uCanvas_universal_obj_t* title = New_uCanvas_2DTextbox("DEMO",100,24);
    title->font_properties.font_type = FONT_24G;
    uCanvas_Set_Color(title,180,0,0);

    v_slider.is_active = true;
    v_slider.max_value = 100;
    v_slider.min_value = 0;
    v_slider.slider_step = 2;
    v_slider.position_x = 30;
    v_slider.position_y = 30+40;
    v_slider.relative_label_pos_x = 0;
    v_slider.relative_label_pos_y = -10;
    v_slider.show_label = true;
    v_slider.slider_length = 80;
    v_slider.slider_notch_radius = 10;
    v_slider.slider_thickness = 20;
    v_slider.update_delay = 1;
    v_slider.slider_gpio_1 = 45;
    v_slider.slider_gpio_2 = 47;
    v_slider.slider_gpio_3 = 48;
    uCanvas_Create_Slider(&v_slider);
    uCanvas_Set_Slider_Value(&v_slider,0);

    p_slider.is_active = true;
    p_slider.max_value = 200;
    p_slider.min_value = 0;
    p_slider.slider_step = 3;
    p_slider.position_x = 30;
    p_slider.position_y = 100 + 40;
    p_slider.relative_label_pos_x = 0;
    p_slider.relative_label_pos_y = -10;
    p_slider.show_label = true;
    p_slider.slider_length = 80;
    p_slider.slider_notch_radius = 10;
    p_slider.slider_thickness = 20;
    p_slider.update_delay = 1;
    p_slider.slider_gpio_1 = 45;
    p_slider.slider_gpio_2 = 47;
    p_slider.slider_gpio_3 = 48;
    uCanvas_Create_Slider(&p_slider);
    uCanvas_Set_Slider_Value(&p_slider,0);
    uCanvas_Set_Slider_Bar_Color(&p_slider,40,0,0);
    uCanvas_Set_Slider_Notch_Color(&p_slider,255,0,0,140,0,0);

    uCanvas_universal_obj_t* cursor =  New_uCanvas_2DCircle(10,70+p_slider.slider_thickness/2,4);
    cursor->properties.fill = FILL;
    uCanvas_Set_Color(cursor,255,0,0);
    int cursor_pos = 0;
    int last_pos = 0;
    while (1)
    {  
        if(!uCanvas_Get_PushbuttonState_WTR(48)){
            cursor_pos = !cursor_pos;
        }

        if(last_pos != cursor_pos){
            switch (cursor_pos)
            {
                case 0:
                    p_slider.is_active = false;
                    v_slider.is_active = false;
                    while (cursor->properties.position.y != 70 + p_slider.slider_thickness/2)
                    {
                        if(last_pos == 1){
                            cursor->properties.position.y--;
                        }
                        else cursor->properties.position.y++;
                        ets_delay_us(5000);
                    }
                    p_slider.is_active = false;
                    v_slider.is_active = true;
                    // printf("s0 selected\r\n");
                    break;
                case 1:
                    p_slider.is_active = false;
                    v_slider.is_active = false;
                    while (cursor->properties.position.y != 140 + p_slider.slider_thickness/2)
                    {
                        cursor->properties.position.y++;
                        ets_delay_us(5000);
                    }
                    // printf("s1 selected\r\n");
                    p_slider.is_active = true;
                    v_slider.is_active = false;
                    break;
                default:
                    break;
                }
            last_pos = cursor_pos;
        }
        uCanvas_Delay(1);
    }
    
    // rect = New_uCanvas_2DRectangle(10,10,40,40);
    // rect->properties.fill = FILL;
    // uCanvas_Set_Color(rect,255,150,00);
}

void uCanvas_App_Main(void) {
    // if(rect->properties.position.x < 240){
    //     rect->properties.position.x++;
    // }else {
    //     uCanvas_Set_Color(rect,get_random_number(100,200),get_random_number(100,200),0);
    //     rect->properties.position.x=0;
    // }
    uCanvas_Delay(1);
}