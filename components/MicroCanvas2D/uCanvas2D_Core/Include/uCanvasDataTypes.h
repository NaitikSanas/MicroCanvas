#ifndef __uCanvasDataTypes_H__
    #define __uCanvasDataTypes_H__
    #include <stdio.h>
    #include <string.h>
    #include "uCanvas_Settings.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/semphr.h"
    #include "uCanvas2D_Display_Setup.h"
   

    typedef TaskFunction_t uCanvas_Animation_task_t;
    typedef TaskHandle_t uCanvas_Animation_task_handle_t;

    typedef enum {uCanvas_Font_Dir_0, uCanvas_Font_Dir_90, uCanvas_Font_Dir_180, uCanvas_Font_Dir_270} font_draw_direction_t;
    
    typedef enum{
        FONTX_16G,
        FONTX_24G,
        FONTX_32G,
        FONTX_32L,
        FONTX_16M,
        FONTX_24M,
        FONTX_10M,

        SFONT_8,
        SFONT_12,
        SFONT_16,
        SFONT_20,
        SFONT_24,
        SFONT_ROBOTO_ITALIC_32,
        SFONT_SIXTYFOUR_32,
        SFONT_BITCOUNT_32,

        OLED_7x10,
        OLED_11x18,
        OLED_16x26,
    }FontType_t;


  
    typedef enum{
        RECTANGLE = 0,
        CIRCLE,
        TEXTBOX,
        LINE,
        ELLIPSE,
        TRIANGLE,
        SPRITE2D,
        WINDOW,
        RICH_TEXBOX
    }uCanvas_element_type_t;

    typedef struct _point
    {
        int x;
        int y;
    }Coordinate2D_t;

    typedef enum {
        NOFILL = 0,
        FILL = 1
    } fill_t;

    typedef struct color_value {
        uint8_t red; 
        uint8_t green;
        uint8_t blue;
        uint8_t monochrome_pixel;
    } color_t;

    typedef enum visibility_ctrl{
        INVISIBLE = 0,
        VISIBLE = 1
    }visibility_ctrl_t;

    typedef enum {
        TEXT_LEFT_ALIGNED = 0,
        TEXT_CENTER_ALIGNED,
        TEXT_RIGHT_ALIGNED
    }uCanvas_Text_Alignment_t;

    typedef enum {
        TEXT_NO_WRAP = 0,
        TEXT_WRAP_PER_NCHARACTER,
        TEXT_WRAP_PER_NWORDS,
        TEXT_WRAP_STRECH
    }uCanvas_Text_Wrap_t;

    typedef struct uCanvas_font_properties
    {
        FontType_t font_type;
        font_draw_direction_t Font_Draw_Direction;
    }uCanvas_font_properties_t;

    typedef struct 
    {
        int textbox_height;
        int textbox_width;
        int margin_x;
        int margin_y;
        int wrap_index;
        fill_t fill_background;
        uCanvas_Text_Alignment_t text_alignment;
        uCanvas_Text_Wrap_t text_wrap_mode; 
        color_t background_color;
        FontType_t font_type;
        font_draw_direction_t Font_Draw_Direction;
        char* textbox_content;
    }uCanvas_TextBox_Properties_t;

    

    typedef struct uCanvas_base 
    { 
        visibility_ctrl_t visiblity;
        uCanvas_element_type_t type;
        uint16_t index;
        Coordinate2D_t position;
        fill_t fill;
        color_t color;
        
        uint8_t collision_detection;
        uint8_t flip_x;
        uint8_t flip_y;

       
    }uCanvas_base_t;




    /**
     * Create 2D Sprite Object based on raw Sprite buffer to use it easily in application
     *  sprite2D_t (properties) -
     *      > pixel_data - Pointes to 1D buffer storing the pixel values
     *      > Sprite_Height - Actual Height of sprite buffer (CHANGING THIS DOES NOT SCALES THE SPRITES)
     *      > Sprite_Width  - Actual Width  of sprite buffer (CHANGING THIS DOES NOT SCALES THE SPRITES)
     *      > Sprite_Orientation
     */
    typedef enum {
        COLOR_RGBA565 = 0,
        COLOR_RGB565,
        COLOR_ARGB8888,
        COLOR_MONOCHROME
    }uCanvas_color_format_t;
    typedef struct sprite2D
    {
        void* sprite_buf;
        uint16_t height;
        uint16_t width;
        uint8_t orientation;  
        uCanvas_color_format_t color_format;  
    }sprite2D_t;
    
    
   
    
    typedef struct uCanvas_universal_obj
    {
        uCanvas_base_t properties; 
        uint16_t index;
        
        
        uint16_t width;
        uint16_t height;

        uint16_t r1;
        uint16_t r2;

        Coordinate2D_t point1;
        Coordinate2D_t point2; 
        Coordinate2D_t point3;
        Coordinate2D_t point4;

        uint8_t invert_sprite_pixels;
        uint8_t state;
        
        uCanvas_font_properties_t font_properties;
        uCanvas_color_format_t color_format;
        uint8_t requires_update;
        void* ctx_data;
        uint16_t* pixel_data;
        char* text;
        uCanvas_TextBox_Properties_t* textbox_properties;
    } uCanvas_universal_obj_t;

    typedef uCanvas_universal_obj_t uCanvas_obj_t;
    typedef struct uCanvas_rectangle
    {
        uCanvas_base_t properties; 
        uint16_t width;
        uint16_t height;
    } uCanvas_rectangle_t;

    /**
     * Scene object is composed of array of 2D Elements. 
     * Any Element created by application will be appended to 
     * current active scene which then forwarded to Render engine
     * To read out element properties and draw on display accordingly.
     * 
     * having multiple scene object can allow application to switch between different
     * 2D Secenes without recreating all the time allowing fast transitions. 
     *  uCanvas_Scene_t ->
     *      > Array : uCanvas_universal_obj_t //List of objects created in scene
     *      > int   : Write Pointer //When object is appended in scene write pointer 
     *                is incremented by +1
     *      > int   : Scene Index (Can be used as a ID)
    */
    typedef struct MicroCanvas2D_Scene
    {
        uCanvas_universal_obj_t* _2D_Objects[MAX_ELEMENTS_NUM];
        int16_t _2D_Object_Ptr;
        uint16_t idx;
    }uCanvas_Scene_t;

    typedef void (*FunctionPointer)(void);
    typedef struct controller_properties
    {
        float jump_velocity;
        float jump_height;
        float gravity;
        float floor_level;
        float position_y;
        float position_x;
        uint32_t control_gpio;
        uCanvas_universal_obj_t* control_object;
        FunctionPointer user_callback_post_jump;
        FunctionPointer user_callback_pre_jump;
    }controller_properties_t;

/**
 *  [delay(keyframe_time[0])] [sprite_source(FRAME_1)]
 *  [delay(keyframe_time[1])] [sprite_source(FRAME_2)]
 *  [delay(keyframe_time[2])] [sprite_source(FRAME_3)]
 *  [delay(keyframe_time[3])] [sprite_source(FRAME_4)]
 *  [delay(keyframe_time[..])] [sprite_source(FRAME_...)]
 *  [delay(keyframe_time[n])] [sprite_source(FRAME_n)]
 */
#define MAX_KEY_FRAMES 32

typedef enum {
    SMOOTH_LINEAR_POS_TRANSFORM = 0,
    HARD_LINEAR_POS_TRANSFORM
}pos_tranforsm_type_t;

typedef struct uCanvas_Loop_Points
{
    int8_t loop_start_frame;
    int8_t loop_end_frame;
    uint8_t loop_for;
}uCanvas_Loop_Points_t;

typedef struct uCanvas_KeyFrame_Parameters
{
    int32_t                 keyframe_time       [MAX_KEY_FRAMES]; 
    
    int                     sprite_postion_x    [MAX_KEY_FRAMES];
    int                     sprite_postion_y    [MAX_KEY_FRAMES];

    int16_t                 sprite_flip_x       [MAX_KEY_FRAMES];
    int16_t                 sprite_flip_y       [MAX_KEY_FRAMES];
    
    uint8_t                 active_keyframes; 
    uint32_t                 frame_time; 
}uCanvas_KeyFrame_Parameters_t;

typedef struct uCanvas_Sprite_KeyFrames  {
   uCanvas_universal_obj_t*  main_sprite;
   uCanvas_KeyFrame_Parameters_t KeyFrame_Parameters;
}uCanvas_Sprite_KeyFrames_t;

typedef enum {ENCODER_IDLE=0, ENCODER_CW, ENCODER_CCW} encoder_state_t;

typedef struct rotary_encoder_obj
{
    uint32_t pin_A;
    uint32_t pin_B;
    uint32_t pin_SW;
    encoder_state_t state;
    uint8_t sw_state;
    uint8_t current_state;
    uint8_t last_state;
}rotary_encoder_t;

typedef enum {
    NO_REFRESH,
    AUTO_REFRESH,
    REFRESH_ON_SIGNAL
}uCanvas2D_Render_Mode_t;

typedef struct uCanvas2D_Instance
{
    uCanvas_Scene_t* active_scene;
    uCanvas2D_Display_Panel_t* panel_1;
    uCanvas2D_Display_Panel_t* panel_2;
    uint8_t instance_dirty;
    uCanvas2D_RenderBuffer_t* render_buffer;
    uCanvas2D_RenderBuffer_t* render_buffer_aux;
    uCanvas2D_RenderBuffer_t* post_processing_frame_buf;
    float scale_x;
    float scale_y;
    bool scale_output;
    TaskHandle_t render_task_handle;
    SemaphoreHandle_t render_buffer_lock;
    int pin_to_core;
    uCanvas2D_Render_Mode_t refresh_mode;
    uint8_t signal_scene_refresh;
    uint64_t fps;
    uint8_t blend;
    uint8_t Clear_On_Refresh;
    uint32_t refresh_delay;
    void* window_instance;
    int synchronize;
    int direct_draw_bitmap;
}uCanvas2D_Instance_t;



typedef struct{
    uCanvas_Scene_t* active_scene;
    uCanvas2D_RenderBuffer_t* render_buffer;
    SemaphoreHandle_t render_buffer_lock; 
    uint8_t signal_scene_refresh;
    uCanvas2D_Render_Mode_t refresh_mode;
    TaskHandle_t layer_render_taskhandle;
    uint8_t layer_dirty;
    uint8_t active;
    int position_x;
    int position_y;
}uCanvas_Layer_t;

typedef struct {
    uCanvas_Layer_t* Layers[10];
    int Active_Layers;
    TaskHandle_t blend_task_handle;
    int pin_to_core;
}uCanvas_Layer_Stack_t;

typedef struct {
    uCanvas_Layer_Stack_t* LayerStack;
    uCanvas2D_Display_Panel_t* Panel;
    TaskHandle_t DirectBlend_Handle;
}uCanvas_DirectBlend_t;

typedef enum{
    INPUT_DEVICE_NONE = 0,
    INPUT_DEVICE_KEYBOARD = 1,
    INPUT_DEVICE_MOUSE = 1 << 2,
    INPUT_DEVICE_TOUCHPOINT = 1 << 3
}uCanvasInputDeviceType_t;
typedef struct {
    enum key_state {
        KEY_STATE_PRESSED = 0x00,
        KEY_STATE_RELEASED = 0x01
    } state;
    uint8_t modifier;
    uint8_t key_code;
    uint8_t key_char;
} key_event_t;

// typedef void (*EventListenerCallback)(uCanvasInputDeviceType_t device_type, int event_code, void* user_data); 
typedef void (*uCanvas_Input_HID_Device_Init)(void);
#define MAX_EVENT_LISTENER_CLIENTS 32
typedef struct {
    void (*EventListenerCallback)(uCanvasInputDeviceType_t device_type, int event_code, void* user_data);
    uint8_t EventListenerState; //Active or Inactive
    uCanvasInputDeviceType_t inputdevices;
}uCanvas_Indev_Listener_Client_t;

typedef struct {
    uCanvas_Indev_Listener_Client_t* in_dev_listener_client[MAX_EVENT_LISTENER_CLIENTS];
    uint8_t listener_idx;
    void (*uCanvas_Input_HID_Device_Init)(void);
    void (*uCanvas_Input_HID_Device_DeInit)(void);
    void (*EventListenerCallback)(uCanvasInputDeviceType_t device_type, key_event_t* key_event, void* user_data) ; 
    void* arg;
}uCanvas_Input_HID_Device_t;

#endif