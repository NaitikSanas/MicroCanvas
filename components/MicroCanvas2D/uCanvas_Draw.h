#include "uCanvas_api.h"

typedef struct uCanvas_Draw_Canvas
{
    uint16_t* frame_buf;
    sprite2D_t sprite_obj;
    uint16_t  canvas_width;
    uint16_t  canvas_height;

    int16_t canvas_position_x;
    int16_t canvas_position_y;
}uCanvas_Draw_Canvas_t;

uCanvas_universal_obj_t* uCanvas_Create_Viewport(uCanvas_Draw_Canvas_t* canvas_obj,uint16_t* canvas_buf, uint16_t canvas_width, uint16_t canvas_height);
void uCanvas_Viewport_DrawRect    (uCanvas_Draw_Canvas_t* canvas_obj, int16_t pos_x, int16_t pos_y, int16_t width, int16_t height, uint16_t color, bool fill);
void uCanvas_Viewport_DrawLine(uCanvas_Draw_Canvas_t* canvas_obj, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) ;

