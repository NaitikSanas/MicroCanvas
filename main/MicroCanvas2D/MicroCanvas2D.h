#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "ssd1306.h"
#include "fonts.h"

#define MAX_ELEMENTS 50

//Indicate Fill insides of shape or not
typedef enum fill_type{
    NOFILL = 0,
    FILL   = 1
}fill_t;

typedef enum visibility_ctrl{
    INVISIBLE = 0,
    VISIBLE = 1
}visibility_ctrl_t;

//Indicates type of shape. 
typedef enum ucanvas_2d_objypes{
    RECTANGLE   = 0,
    CIRCLE      = 1,
    LINE        = 2,
    TRIAGLE     = 3,
    TEXT_BOX    = 4
}ucanvas_2d_objypes_t;

/**
 * create struct that contains context for elements.
*/ 

typedef struct _2Dvec
{
    int x;
    int y;
}Vector2D_t;

typedef struct shape_g
{
    Vector2D_t position;
    visibility_ctrl_t visibality;
    fill_t fill;
    int index;
}shape_generic_t;

typedef struct rect 
{
    shape_generic_t shape_properties; 
    Vector2D_t width_height_xy;
}rectangle_t;

typedef struct textbox 
{
    char str[256];
    shape_generic_t shape_properties; 
}textbox_t;

typedef struct line 
{
    shape_generic_t shape_properties; 
    Vector2D_t point1;
    Vector2D_t point2;
}line_t;

typedef struct circle
{
    /* data */
    shape_generic_t shape_properties; 
    int radius;
};


typedef struct shape { 
    /*  object dimensional parameters : allows to set geomatery of objects. */
    char str [256];       // Textbox Content Data    
    int x1;               // alternative height 1  | line point 1
    int y1;               // alternative width 1  | line point 1
    int x2;               // alt. height 2 | line point 2
    int y2;               // alt. width 2  | line point 2
    int r1;               // radius 1 of shape like circle
    int r2;               // radius 1 of shape like circle,ellipse
    FontDef_t* font_type; // Font to be used for text box objects.

    /*  object control properties : allows to control object on display. */
    ucanvas_2d_objypes_t ucanvas_2d_objype; //Controls which shape to render using provided parameters
    fill_t fill;              // Fill insides of shape or leave it blank
    int visibality;           // Makes object visible, invisible on screen.
    int pos_x;                // x position of object on screen
    int pos_y;                // y position of object on screen
    int index;                // Index of shape on page. (for internal usage)
} ucanvas_2d_obj;

typedef struct page{
    ucanvas_2d_obj* elements[MAX_ELEMENTS]; //Array of shape objects.
    int      elements_ptr;           //element pointer. it is incremented when any new shape is created.
    int      page_idx;               //represents page index of self.
    void*    next_page;              //Points to next page pointer. 
}page_t;

void display_render_task (void);
void start_geomatery_engine(void);
void stop_geomatery_engine(void);
void set_current_page(page_t* page);
void page_init(page_t* page);
void destroy_page(page_t* page);

/**
 * All Api's below renders different shapes and returns Object pointer to user
 * which allows direct manipulation of on screen object in application. 
*/

ucanvas_2d_obj* create_line(int x1, int y1, int x2,int y2);
ucanvas_2d_obj* create_circle(int pos_x, int pos_y, int radius, 
                       int fill);
ucanvas_2d_obj* create_rectangle (int width, int height, int pos_x, 
                            int pos_y, fill_t fill);
ucanvas_2d_obj* create_textbox(char* str, FontDef_t* font_type, 
                         int pos_x, int pos_y);
                         
int add_into_elements_list(ucanvas_2d_obj* shape);
/**
 * for those who dont directly want to deal with object pointer can use this 
 * to control postion, textbox content, visibality, line points. 
*/
void set_textbox(ucanvas_2d_obj* textbox, char* str);
void shape_reposition(ucanvas_2d_obj* shape, int x, int y);
void set_line_p1(ucanvas_2d_obj* line,int x1,int y1);
void set_line_p2(ucanvas_2d_obj* line,int x2,int y2);
void set_visiblity(ucanvas_2d_obj* shape, visibility_ctrl_t vc);
Vector2D_t get_distance(ucanvas_2d_obj* shape1, ucanvas_2d_obj* shape2 );
void destroy_shape_obj(ucanvas_2d_obj* obj);