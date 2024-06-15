#ifndef __uCanvasDataTypes_H__
    #define __uCanvasDataTypes_H__
    #include <stdio.h>
    #include <string.h>
    #include "uCanvas_Settings.h"
   
    #define UCANVAS_TEXTBOX_MAX_CONTNENT_SIZE       (256)

    typedef TaskFunction_t uCanvas_Animation_task_t;
    typedef TaskHandle_t uCanvas_Animation_task_handle_t;

    typedef enum{
        RECTANGLE = 0,
        CIRCLE,
        TEXTBOX,
        LINE,
        ELLIPSE,
        TRIANGLE
    }uCanvas_element_type_t;

    typedef struct _point
    {
        uint16_t x;
        uint16_t y;
    }_2dPoint_t;

    typedef enum {
        NOFILL = 0,
        FILL = 1
    } fill_t;

    typedef struct color_value {
        uint32_t red; 
        uint32_t green;
        uint32_t blue;
        uint8_t monochrome_pixel;
    } color_t;

    typedef enum visibility_ctrl{
        INVISIBLE = 0,
        VISIBLE = 1
    }visibility_ctrl_t;

    typedef struct uCanvas_base 
    { 
        uCanvas_element_type_t type;
        uint16_t index;
        _2dPoint_t position;
        fill_t fill;
        color_t color;
        visibility_ctrl_t visiblity;
    }uCanvas_base_t;

    typedef struct uCanvas_universal_obj
    {
        uCanvas_base_t properties; 
        uint16_t index;
        
        
        uint16_t width;
        uint16_t height;

        uint16_t r1;
        uint16_t r2;

        _2dPoint_t point1;
        _2dPoint_t point2; 
        _2dPoint_t point3;
        _2dPoint_t point4;  
        char text[512];
    } uCanvas_universal_obj_t;


    typedef struct uCanvas_rectangle
    {
        uCanvas_base_t properties; 
        uint16_t width;
        uint16_t height;
    } uCanvas_rectangle_t;

    typedef struct MicroCanvas2D_Scene
    {
        uCanvas_universal_obj_t* _2D_Objects[MAX_ELEMENTS_NUM];
        int16_t _2D_Object_Ptr;
        uint16_t idx;
    }uCanvas_Scene_t;
#endif