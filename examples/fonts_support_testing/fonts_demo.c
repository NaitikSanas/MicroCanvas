#include "fonts_demo.h"
#include "uCanvas_api.h"
void uCanvas_Run_Fonts_demo(void){
    start_uCanvas_engine();
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    uCanvas_universal_obj_t* text = New_uCanvas_2DTextbox("Hello",20,20);
    text->font_properties.font_type = FONT_16G;
    text->font_properties.Font_Draw_Direction = uCanvas_Font_Dir_0;
    
    while (1)
    {
        if(text->font_properties.font_type < FONT_10M)text->font_properties.font_type++;
        else text->font_properties.font_type = FONT_16G;
        uCanvas_Delay(100);
    }
}