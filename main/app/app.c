#include "app.h"
#include "uCanvas2D_Display_Setup.h"
#include "uCanvas_Menu.h"
#include "uCanvas_HID.h"
#include "Panel_Selector.h"
key_event_t g_key_event;
static uCanvas_Input_HID_Device_t   g_keyboard_device;
static uCanvas2D_Instance_t g_canvas;

static uCanvas_Menu_t Menu_1 = {
        
        .ColorTheme.MenuTitle       = {215, 230, 252},
        .ColorTheme.ItemName        = {85, 115, 161},
        .ColorTheme.Cursor          = {214, 230, 255},
        .ColorTheme.MenuBorder      = {66, 135, 245},
        .ColorTheme.MenuBackGround  = {138, 183, 255},
        .ColorTheme.MenuTitleBackground = {66, 135, 245},
        
        .spacing_y  = 15, 
        .height     = 350, 
        .width      = 520,
        .pos_x = 40,
        .pos_y = 40,

        .ExpandSelectedItem = false,
        .MenuItems = {
            {.ItemBackgroundEnabled=false},
            {.ItemBackgroundEnabled=false},
            {.ItemBackgroundEnabled=false},
            {.ItemBackgroundEnabled=false},
            {.ItemBackgroundEnabled=false},
            {.ItemBackgroundEnabled=false},
            {.ItemBackgroundEnabled=false},
        }
    };
static void keyboard_event_cb(uCanvasInputDeviceType_t type,
                               key_event_t *ev,
                               void *user_data)
{
    g_key_event = *ev;

    if (ev->state == KEY_STATE_PRESSED){
        printf("[INPUT] Key pressed:  '%c'\n", ev->key_char);
        if(ev->key_char == '<'){
            if(Menu_1.MenuCursor.CursorIndex_1<7)Menu_1.MenuCursor.CursorIndex_1++;
            else Menu_1.MenuCursor.CursorIndex_1 = 0;
            uCanvas_Menu_Update(&Menu_1);
        }
        if(ev->key_char == '>'){
            if(Menu_1.MenuCursor.CursorIndex_1>0)Menu_1.MenuCursor.CursorIndex_1--;
            else Menu_1.MenuCursor.CursorIndex_1 = 0;
            uCanvas_Menu_Update(&Menu_1);
        }
        if(ev->key_char == ' '){
            Menu_1.ExpandSelectedItem = !Menu_1.ExpandSelectedItem;
            uCanvas_Menu_Update(&Menu_1);
        }
    }
    else
        printf("[INPUT] Key released: '%c'\n", ev->key_char);
}

void uCanvas_Setup(void)
{
    g_keyboard_device.EventListenerCallback = keyboard_event_cb;
    uCanvas_Get_HID_Device(&g_keyboard_device);
    g_keyboard_device.uCanvas_Input_HID_Device_Init();

    uCanvas_Scene_t *scene = New_uCanvas_Scene();
    uCanvas2D_Display_Panel_t *panel = uCanvas_Get_Panel_Handle(PANEL_ST7789);

    panel->init(1);

    if (uCanvas_Attach_RenderBuffer(&g_canvas, 800, 600)) {
        uCanvas_Set_Panel_RefreshDelay(&g_canvas, 2);
        uCanvas_Attach_Panel(&g_canvas, panel);
        uCanvas_Set_ViewPort_Position(&g_canvas, 0, 0);
        uCanvas_Attach_Scene(&g_canvas, scene);
        uCanvas_Attach_Renderer(&g_canvas, 1);
        uCanvas_Set_Render_Mode(&g_canvas, AUTO_REFRESH);
    }

    uCanvas_set_active_scene(scene);
    // uCanvas_universal_obj_t* bg = New_uCanvas_2DRectangle(0,0,800,600);
    // uCanvas_Set_Color(bg,150,150,150);
    // uCanvas_Set_Fill(bg,FILL);

    // static uCanvas_Menu_t Menu_1 = {
    //     .ColorTheme.MenuTitle       = {255,0,0},
    //     .ColorTheme.ItemName        = {150,0,0},
    //     .ColorTheme.Cursor          = {255,0,0},
    //     .ColorTheme.MenuBorder      = {255,0,0},
    //     .ColorTheme.MenuBackGround  = {70,0,0},
    //     .ColorTheme.MenuTitleBackground = {150,0,0},
    //     .spacing_y  = 10, 
    //     .height     = 350, 
    //     .width      = 320
    // };

     
    
    FontType_t MenuFontType = SFONT_24;
    uCanvas_Menu_Set_Title(&Menu_1,"Settings",UCANVAS_ALIGN_CENTER, SFONT_SIXTYFOUR_32);

    uCanvas_Menu_Add_Item(&Menu_1,0,"WIFI",MenuFontType);
    uCanvas_Menu_Add_Item(&Menu_1,6,"LAYOUT PRESETS",MenuFontType);
    uCanvas_Menu_Add_Item(&Menu_1,2,"UPDATE",MenuFontType);
    uCanvas_Menu_Add_Item(&Menu_1,3,"IO MAPPING",MenuFontType);
    uCanvas_Menu_Add_Item(&Menu_1,4,"THEMES",MenuFontType);
    uCanvas_Menu_Add_Item(&Menu_1,5,"THEMES",MenuFontType);
    uCanvas_Menu_Add_Item(&Menu_1,1,"ITEM 6",MenuFontType);
    uCanvas_Menu_Add_Item(&Menu_1,7,"ITEM 7",MenuFontType);

    uCanvas_Menu_Add_Item_Values(&Menu_1,0,0,"Always ON",SFONT_16);
    uCanvas_Menu_Add_Item_Values(&Menu_1,1,0,"ON",SFONT_16);
    uCanvas_Menu_Add_Item_Values(&Menu_1,2,0,"OFF",SFONT_16);
    
    uCanvas_Menu_Update(&Menu_1);

    while (1)
    {
        // if(Menu_1.MenuCursor.CursorIndex_1<7)Menu_1.MenuCursor.CursorIndex_1++;
        // else Menu_1.MenuCursor.CursorIndex_1 = 0;
        // uCanvas_Menu_Update(&Menu_1);

        // if(Menu_1.pos_x < 600)Menu_1.pos_x++;
        // else Menu_1.pos_x = 0;
        uCanvas_Delay(20);
    }
    
    
}

void uCanvas_App_Main(void)
{
    uCanvas_Setup();
}
