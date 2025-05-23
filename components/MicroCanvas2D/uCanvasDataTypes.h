#ifndef __uCanvasDataTypes_H__
    #define __uCanvasDataTypes_H__
    #include <stdio.h>
    #include <string.h>
    #include "uCanvas_Settings.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/semphr.h"
    #define UCANVAS_TEXTBOX_MAX_CONTNENT_SIZE       (256)

    typedef TaskFunction_t uCanvas_Animation_task_t;
    typedef TaskHandle_t uCanvas_Animation_task_handle_t;
    
    typedef enum{
        FONT_16G,
        FONT_24G,
        FONT_32G,
        FONT_32L,
        FONT_16M,
        FONT_24M,
        FONT_10M,
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
        SPRITE2D
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
     *      > Sprite_buffer - Pointes to 1D buffer storing the pixel values
     *      > Sprite_Height - Actual Height of sprite buffer (CHANGING THIS DOES NOT SCALES THE SPRITES)
     *      > Sprite_Width  - Actual Width  of sprite buffer (CHANGING THIS DOES NOT SCALES THE SPRITES)
     *      > Sprite_Orientation
     */
    typedef struct sprite2D
    {
        uint16_t* sprite_buf;
        uint16_t height;
        uint16_t width;
        uint8_t orientation;    
    }sprite2D_t;
    
    typedef enum {uCanvas_Font_Dir_0, uCanvas_Font_Dir_90, uCanvas_Font_Dir_180, uCanvas_Font_Dir_270} font_draw_direction_t;
    
    typedef struct uCanvas_font_properties
    {
        FontType_t font_type;
        font_draw_direction_t Font_Draw_Direction;
    }uCanvas_font_properties_t;
    
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
        char* text;
        uCanvas_font_properties_t font_properties;
        
        
        uint16_t* sprite_buffer;
        // Coordinate2D_t sprite_resolution;
        
        // sprite2D_t sprite_obj;
        // sprite2D_t* sprite2D_obj;
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
#endif