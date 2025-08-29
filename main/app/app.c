#include "app.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "sprite_collection.h"
#include "uCanvas2D_ST7789_Port.h"
#include "uCanvas2D_EK79007Port.h"
#include "uCanvasRenderEngine.h"
#include "uCanvas_HID.h"
#include "uCanvas2D_Acceleration.h"
#include "esp_timer.h"

#include "esp_timer.h"
#include "FreeRTOSConfig.h"
uint32_t ulHighFrequencyTimerTicks(void) {
    return (uint32_t)(esp_timer_get_time() / 1000); // microseconds → milliseconds
}

static bool key_held = false;
static char held_char = 0;


#define CANVAS_HEIGHT       600
#define CANVAS_WIDTH        1024

static uCanvas_Input_HID_Device_t KeyBoardDevice;
static uCanvas_universal_obj_t* textbox = NULL;

// editable buffer for textbox text
static char text_buffer[512] = {0};
static size_t text_len = 0;

// simple helper to update the textbox's displayed text
static void update_textbox_display(void) {
    uCanvas_Set_Text(textbox, text_buffer);
}
#define MAX_HELD_KEYS 8

typedef struct {
    uint8_t key_code;
    char key_char;
} held_key_t;

static held_key_t held_keys[MAX_HELD_KEYS];
static int held_count = 0;

static int repeat_index = -1; 
static uint64_t last_repeat_time = 0;
static const uint64_t KEY_REPEAT_DELAY = 300 * 1000;  // µs before repeat starts
static const uint64_t KEY_REPEAT_RATE  = 50  * 1000;  // µs between repeats

static void add_char_to_buffer(char c) {
    if (c == '\b') {
        if (text_len > 0) {
            text_len--;
            text_buffer[text_len] = '\0';
        }
    } else if (c == '\r' || c == '\n') {
        if (text_len < sizeof(text_buffer) - 2) {
            text_buffer[text_len++] = '\n';
            text_buffer[text_len] = '\0';
        }
    } else if (c >= 32 && c <= 126) {
        if (text_len < sizeof(text_buffer) - 1) {
            text_buffer[text_len++] = c;
            text_buffer[text_len] = '\0';
        }
    }
    update_textbox_display();
}

static void start_repeat_for_key(int index) {
    repeat_index = index;
    last_repeat_time = esp_timer_get_time();
    add_char_to_buffer(held_keys[index].key_char); 
}

static void stop_repeat_if_key(int index) {
    if (repeat_index == index) {
        repeat_index = -1;
    }
}

static inline void MyKeyboardListener(uCanvasInputDeviceType_t device_type, key_event_t* key_event, void* user_data) {
    key_event_t* event = (key_event_t*)key_event;

    if (event->state == KEY_STATE_PRESSED) {
        for (int i = 0; i < held_count; i++) {
            if (held_keys[i].key_code == event->key_code) return;
        }
        if (held_count < MAX_HELD_KEYS) {
            held_keys[held_count].key_code = event->key_code;
            held_keys[held_count].key_char = event->key_char;
            held_count++;
            if (repeat_index == -1) {
                start_repeat_for_key(held_count - 1);
            }
        }
    } 
    else if (event->state == KEY_STATE_RELEASED) {
        for (int i = 0; i < held_count; i++) {
            if (held_keys[i].key_code == event->key_code) {
                stop_repeat_if_key(i);
                for (int j = i; j < held_count - 1; j++) {
                    held_keys[j] = held_keys[j + 1];
                }
                held_count--;
                if (repeat_index == -1 && held_count > 0) {
                    start_repeat_for_key(0);
                }
                break;
            }
        }
    }
}
void uCanvas_color_wipe(uCanvas_universal_obj_t* obj, color_t from_color, color_t to_color, uint32_t delay_us, uint16_t steps) {
    int16_t red_diff = to_color.red - from_color.red;
    int16_t green_diff = to_color.green - from_color.green;
    int16_t blue_diff = to_color.blue - from_color.blue;

    for (uint16_t step = 0; step < steps; step++) {
        color_t current_color;
        current_color.red = from_color.red + (red_diff * step) / steps;
        current_color.green = from_color.green + (green_diff * step) / steps;
        current_color.blue = from_color.blue + (blue_diff * step) / steps;
        obj->textbox_properties->background_color.red   = current_color.red;
        obj->textbox_properties->background_color.green = current_color.green; 
        obj->textbox_properties->background_color.blue  = current_color.blue;
        obj->textbox_properties->textbox_updated = true;
        vTaskDelay(pdMS_TO_TICKS (delay_us));
    }
    obj->textbox_properties->background_color.red   = to_color.red;
    obj->textbox_properties->background_color.green = to_color.green;
    obj->textbox_properties->background_color.blue  = to_color.blue;
     obj->textbox_properties->textbox_updated = true;
}

static uCanvas2D_Instance_t uCanvas_Instance_1;

void fps_monitor(void){
    char buf[32]={0};
    uCanvas_universal_obj_t* fps = New_uCanvas_2DTextbox(buf, 0,0);
    uCanvas_Set_Color(fps, 10, 147, 255);
    uCanvas_Set_Textbox_Alignment(fps, TEXT_LEFT_ALIGNED);
    uCanvas_Set_Textbox_Wrap_Style(fps, TEXT_WRAP_PER_SET_WORD_LEN, 100);
    uCanvas_Set_TextBox_Margin(fps, 10, 10);

    uCanvas_Set_TextBox_Fill_Background(fps, NOFILL, 168, 212, 247);
    uCanvas_Set_TextBox_Border_Properties(fps,5,81,164,232);
    uCanvas_Set_TextBox_FontType(fps, SFONT_24);

    while (1)
    {
        sprintf(buf,"FPS:%lld",uCanvas_Get_FPS(&uCanvas_Instance_1));
        uCanvas_Set_Text(fps,buf);
        // sprintf(buf2,"FPS:%lld",uCanvas_Get_FPS(&uCanvas_Instance_2));
        // uCanvas_Set_Text(fps_counter2,buf2);

        vTaskDelay(pdMS_TO_TICKS(200));
    } 
}

void uCanvas_Setup() {
    // init keyboard HID
    KeyBoardDevice.EventListenerCallback = MyKeyboardListener;
    uCanvas_Get_HID_Device(&KeyBoardDevice);
    KeyBoardDevice.uCanvas_Input_HID_Device_Init();
    
    Intialize_PPA();

    uCanvas_Scene_t* scene_1 = New_uCanvas_Scene();
    uCanvas2D_Display_Panel_t* panel = uCanvas2D_Get_Panel_Driver_EK79007();
    panel->init(1);
    panel->set_backlight(3000);
    
    
    if (uCanvas_Attach_RenderBuffer(&uCanvas_Instance_1, CANVAS_WIDTH, CANVAS_HEIGHT)) {
        uCanvas_Set_Panel_RefreshDelay(&uCanvas_Instance_1, 0);
        uCanvas_Attach_Panel(&uCanvas_Instance_1, panel);
        uCanvas_Set_ViewPort_Position(&uCanvas_Instance_1, 0, 0);
        uCanvas_Attach_Scene(&uCanvas_Instance_1, scene_1);
        uCanvas_Attach_Renderer(&uCanvas_Instance_1, 1);
        // uCanvas_Instance_1.Clear_On_Refresh = false;
    }
    
    uCanvas_set_active_scene(scene_1);

    // initial buffer text
    sprintf(text_buffer, "Type something...");
    text_len = strlen(text_buffer);

    uCanvas_universal_obj_t* c = New_uCanvas_2DCircle(220,100,50);
    uCanvas_Set_Color(c,255,255,0);
    c->properties.fill = FILL;

    textbox = New_uCanvas_2DAdvancedTextbox(text_buffer, 200, 50,400,300);
    uCanvas_Set_Color(textbox, 255, 0, 0);
    uCanvas_Set_Textbox_Alignment(textbox, TEXT_CENTER_ALIGNED);
    uCanvas_Set_Textbox_Wrap_Style(textbox, TEXT_WRAP_PER_SET_WORD_LEN, 30);
    uCanvas_Set_TextBox_Margin(textbox, 10, 10);
    uCanvas_Set_TextBox_Fill_Background(textbox, NOFILL, 0, 0, 0);
    uCanvas_Set_TextBox_FontType(textbox, SFONT_24);
    uCanvas_Enable_TextBox_Cursor(textbox,250,CARET_CURSOR);
    uCanvas_Set_TextBox_Border_Properties(textbox,3,255,0,0);

    uCanvas_Add_Task((void (*))fps_monitor,NULL,0);
    
    while (1)
    {
        
        // printf("%s\r\n",buf);
        uint64_t now = esp_timer_get_time();

        if (repeat_index != -1 && held_count > 0) {
            if (now - last_repeat_time >= KEY_REPEAT_DELAY) {
                static uint64_t last_rate_time = 0;
                if (now - last_rate_time >= KEY_REPEAT_RATE) {
                    add_char_to_buffer(held_keys[repeat_index].key_char);
                    last_rate_time = now;
                }
            }
        }

        uCanvas_Delay(1);
    }
}

void uCanvas_App_Main(void) {
    uCanvas_Delay(1);
}