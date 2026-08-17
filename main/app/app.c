#include "app.h"
#include "uCanvas2D_Display_Setup.h"
#include "uCanvas_Menu.h"
#include "uCanvas_HID.h"
#include "examples_init.h"
#include "sprite_collection.h"

#define TOP_BAR_HEIGHT  30
#define TOP_BG_COLOR    255,0,0
#define MAIN_BG_COLOR   150,0,0
#define FONT_COLOR_1    255,255,255

void uCanvas_Setup(void)
{
    Example_uCanvas_Instance_Setup();
    uCanvas2D_Instance_t* Instance = Get_Example_uCanvas_Instance();
}

void uCanvas_Quick_Scroll_X(Universal_Obj_t* obj, int16_t reset_threshold,int16_t reset_value, int16_t offset){
    if(uCanvas_Get_Pos_X(obj) < reset_threshold){
        uCanvas_Inc_Pos_X(obj,offset);
    }
    else {
        uCanvas_Set_Position_X(obj,reset_value);
        if(obj->properties.thickness < 5){
            obj->properties.thickness += 1;
            uCanvas_Set_Thickness(obj,obj->properties.thickness);
        }
        else uCanvas_Set_Thickness(obj,1);
    }
}

void uCanvas_Quick_Scroll_Y(Universal_Obj_t* obj, int16_t reset_threshold,int16_t reset_value, int16_t offset){
    if(uCanvas_Get_Pos_Y(obj) < reset_threshold){
        uCanvas_Inc_Pos_Y(obj,offset);
    }
    else uCanvas_Set_Position_Y(obj,reset_value);
}


void uCanvas_App_Main(void)
{
    uCanvas_Setup();
    
    Universal_Obj_t* background_main = New_uCanvas_2DRectangle(0,TOP_BAR_HEIGHT,UCANVAS_INST_HEIGHT - TOP_BAR_HEIGHT,UCANVAS_INST_WIDTH);
    uCanvas_Set_Color(background_main,MAIN_BG_COLOR);
    uCanvas_Set_Fill(background_main,FILL);

    Universal_Obj_t* background_top = New_uCanvas_2DRectangle(0,0,TOP_BAR_HEIGHT,UCANVAS_INST_WIDTH);
    uCanvas_Set_Color(background_top,TOP_BG_COLOR);
    uCanvas_Set_Fill(background_top,FILL);

    Universal_Obj_t* textbox_1 = New_uCanvas_2DTextbox("MicroCanvas Demo",5,5);
    uCanvas_Set_Color(textbox_1,FONT_COLOR_1);
    uCanvas_Set_TextBox_FontType(textbox_1,SFONT_16);

    Universal_Obj_t* Circle = New_uCanvas_2DCircle(UCANVAS_INST_WIDTH/2, (UCANVAS_INST_HEIGHT/2),30);
    uCanvas_Set_Color(Circle,TOP_BG_COLOR);
    // uCanvas_Set_Fill(Circle,FILL);

    Universal_Obj_t* Box = New_uCanvas_2DRectangle(UCANVAS_INST_WIDTH/2, (UCANVAS_INST_HEIGHT/2)+20,20,20);
    uCanvas_Set_Color(Box,TOP_BG_COLOR);
    uCanvas_Set_Fill(Box,FILL);

    sprite2D_t Sprite_1;
    uCanvas_Compose_2DSprite_Obj(&Sprite_1,run_2,RUN_2_HEIGHT,RUN_2_WIDTH,COLOR_RGBA565);
    uCanvas_Sprite_Adjust_Contrast(&Sprite_1,512);
    Universal_Obj_t* Sprite_Obj = New_uCanvas_2DSprite(&Sprite_1,40,TOP_BAR_HEIGHT+20);

    while (1)
    {
        uCanvas_Quick_Scroll_X(Circle,UCANVAS_INST_WIDTH,0,1);
        
        uCanvas_Quick_Scroll_Y(Box,UCANVAS_INST_WIDTH,0,1);
        uCanvas_Quick_Scroll_X(Sprite_Obj,UCANVAS_INST_WIDTH,0,1); 
        uCanvas_Delay(16);
    }
    
}