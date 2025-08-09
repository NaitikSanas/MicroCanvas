#include "app.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "sprite_collection.h"
#include "uCanvas2D_ST7789_Port.h"
#include "uCanvas2D_EK79007Port.h"
#include "uCanvasRenderEngine.h"
#define CANVAS_HEIGHT       600
#define CANVAS_WIDTH        1024

static uCanvas2D_Instance_t uCanvas_Instance_2;
static uCanvas2D_Instance_t uCanvas_Instance_3;
static uCanvas2D_Instance_t uCanvas_Instance_4;
#include "uCanvas_HID.h"
uCanvas_universal_obj_t* textbox = NULL;

static inline void MyKeyboardListener(uCanvasInputDeviceType_t device_type, key_event_t* key_event, void* user_data) {
    // printf("%c",(char)event_code);

        // if (!!event_code) {
        //     putchar(event_code);
        //     if ('\r' == event_code) {
        //         putchar('\n');
        //     }
        //     fflush(stdout);
        // }
    key_event_t* event = (key_event_t*)key_event;
    printf("key_event_callback\r\n");
    if (event->state == KEY_STATE_PRESSED) {
        printf("Key pressed: %c\n", event->key_char);
    } else if (event->state == KEY_STATE_RELEASED) {
        printf("Key released: %c\n", event->key_char);
    }
    return;
}

static uCanvas_Input_HID_Device_t KeyBoardDevice;
#include "uCanvas2D_Acceleration.h"
void uCanvas_Setup() {
    static uCanvas2D_Instance_t uCanvas_Instance_1;
    Intialize_PPA();
    // KeyBoardDevice.EventListenerCallback = MyKeyboardListener;
    // uCanvas_Get_HID_Device(&KeyBoardDevice);
    // KeyBoardDevice.uCanvas_Input_HID_Device_Init();

    uCanvas_Scene_t* scene_1 = New_uCanvas_Scene();
    uCanvas2D_Display_Panel_t* panel = uCanvas2D_Get_Panel_Driver_EK79007();
    panel->init(1);
    panel->set_backlight(3000);

    if(uCanvas_Attach_RenderBuffer(&uCanvas_Instance_1,CANVAS_WIDTH,CANVAS_HEIGHT)){
        uCanvas_Set_Panel_RefreshDelay(&uCanvas_Instance_1,5);
        uCanvas_Attach_Panel(&uCanvas_Instance_1,panel);
        uCanvas_Set_ViewPort_Position(&uCanvas_Instance_1,0,0);
        uCanvas_Attach_Scene(&uCanvas_Instance_1,scene_1);
        uCanvas_Attach_Renderer(&uCanvas_Instance_1,1);
    }
    
    uCanvas_set_active_scene(scene_1);

    char buf [256]= {0};
    sprintf(buf, "%s","About MicroCanvas2D:\n\
        This is a Retained Mode Graphics Engine.It Allows Easy way to Create Fun 2D Graphics Applications for Embedded Systems");
    int h = 32;
    int w = 33*strlen(buf);
    int xstart = 50;

    

    textbox = New_uCanvas_2DTextbox(buf,50,50);

    uCanvas_Set_Color(textbox,255,0,0);
    uCanvas_Set_Textbox_Alignment(textbox,TEXT_LEFT_ALIGNED);
    uCanvas_Set_Textbox_Wrap_Style(textbox,TEXT_WRAP_PER_NWORDS,20);
    uCanvas_Set_TextBox_Margin(textbox,10,10);
    uCanvas_Set_TextBox_Fill_Background(textbox,NOFILL, 0,0,0);
    uCanvas_Set_TextBox_FontType(textbox,SFONT_16);
    uCanvas_Set_TextBox_Dimensions(textbox,420,200);

    // textbox->textbox_properties->text_alignment = TEXT_LEFT_ALIGNED;
    // textbox->textbox_properties->text_wrap_mode = TEXT_WRAP_PER_NWORDS;
    // textbox->textbox_properties->wrap_index = 20;
    // textbox->font_properties.font_type = SFONT_16;
    // textbox->textbox_properties->font_type = SFONT_16;
    // textbox->textbox_properties->margin_x = 10;
    // textbox->textbox_properties->textbox_width = 420;
    // textbox->textbox_properties->textbox_height = 200;

    uCanvas_universal_obj_t* textbg = New_uCanvas_2DRectangle(textbox->properties.position.x,textbox->properties.position.y,textbox->textbox_properties->textbox_height, textbox->textbox_properties->textbox_width);
    uCanvas_Set_Color(textbg,150,0,0);
    textbg->properties.fill = NOFILL;
    textbg->properties.visiblity = VISIBLE;
    // uCanvas_universal_obj_t* mask = New_uCanvas_2DRectangle(xstart,50,h,w);
    // uCanvas_Set_Color(mask,0,0,0);
    // mask->properties.fill = FILL;

    // EFFECT_START : 
    // textbg->properties.visiblity = INVISIBLE;
    // while (mask->width != 1 )
    // {
    //     mask->width--;
    //     mask->properties.position.x++;
    //     uCanvas_Delay(pdMS_TO_TICKS(15));
    // }
    
    
    // textbg->width = 1;
    // textbg->properties.visiblity = VISIBLE;
    // uCanvas_Delay(pdMS_TO_TICKS(1000));
    // textbg->properties.color.red = 0;
    // textbg->properties.color.green = 0;
    
    // while (textbg->width != w )
    // {
    //     textbg->width++;
    //     // textbg->properties.position.x++;
    //     if(textbg->properties.color.red < 255)textbg->properties.color.red++;
    //     if(textbg->properties.color.green < 120)textbg->properties.color.green++;
    //     uCanvas_Delay(pdMS_TO_TICKS(2));
    // }
    // uCanvas_Delay(pdMS_TO_TICKS(4000));

    // while (textbg->properties.color.red != 0 )
    // {
    //     textbg->properties.color.red--;
    //     if(textbg->properties.color.green > 0)textbg->properties.color.green--;
    //     uCanvas_Delay(pdMS_TO_TICKS(5));
    // }
    // textbg->properties.color.red = 0;
    // textbg->properties.color.green = 0;
    // textbg->properties.position.x = xstart;
    // textbg->width = w;
    // mask->properties.position.x = xstart;
    // mask->width = w;
    
    // printf("-redo %d,%d\r\n",mask->properties.position.x,mask->width);
    // goto EFFECT_START;
}   

void uCanvas_App_Main(void) {
    uCanvas_Delay(1);
   
}