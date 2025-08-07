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
static uCanvas2D_Instance_t uCanvas_Instance_1;
static uCanvas2D_Instance_t uCanvas_Instance_2;
static uCanvas2D_Instance_t uCanvas_Instance_3;
static uCanvas2D_Instance_t uCanvas_Instance_4;
#include "uCanvas_HID.h"
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

void uCanvas_Setup() {
    
    KeyBoardDevice.EventListenerCallback = MyKeyboardListener;
    uCanvas_Get_HID_Device(&KeyBoardDevice);
    KeyBoardDevice.uCanvas_Input_HID_Device_Init();

    // uCanvas_Scene_t* scene_1 = New_uCanvas_Scene();
    // uCanvas2D_Display_Panel_t* panel = uCanvas2D_Get_Panel_Driver_EK79007();
    // panel->init(1);
    // panel->set_backlight(3000);

    // if(uCanvas_Attach_RenderBuffer(&uCanvas_Instance_1,CANVAS_WIDTH,CANVAS_HEIGHT)){
    //     uCanvas_Set_Panel_RefreshDelay(&uCanvas_Instance_1,2);
    //     uCanvas_Attach_Panel(&uCanvas_Instance_1,panel);
    //     uCanvas_Set_ViewPort_Position(&uCanvas_Instance_1,20,20);
    //     uCanvas_Attach_Scene(&uCanvas_Instance_1,scene_1);
    //     uCanvas_Attach_Renderer(&uCanvas_Instance_1,1);
    // }  
}   

void uCanvas_App_Main(void) {
    uCanvas_Delay(1);
   
}