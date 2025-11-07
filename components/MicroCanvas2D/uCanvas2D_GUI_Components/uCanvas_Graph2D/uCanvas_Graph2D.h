#include "uCanvas_api.h"

#define MAX_GRAPH_LINES 200
typedef enum{
   GRAPH2D_LINE,
   GRAPH2D_DOTTTED,
}uCanvas_Graph_type_t;

typedef struct{
    color_t background_color[8];
    color_t forground_color[8];
    color_t border_color[8];
    color_t main_content_color[8];
}uCanvasGUI_Color_Style_t;

typedef struct uCanvas_Graph2D_Instance
{
    uCanvas_universal_obj_t* canvas[8];
    uCanvas_universal_obj_t* graph_line[MAX_GRAPH_LINES];
    int datapoints_x[MAX_GRAPH_LINES+2];
    int datapoints_y[MAX_GRAPH_LINES+2];
    uCanvas_Graph_type_t GraphType;
    int data_x_min;
    int data_x_max;
    int data_y_min;
    int data_y_max;
    int data_ptr_x;
    int data_ptr_y;
    int16_t x_pos;
    int16_t y_pos;
    int16_t title_pos_x;
    int16_t title_pos_y;
    int16_t height;
    int16_t width;
    int max_datapoints;
    
    uCanvasGUI_Color_Style_t Color_Style;
}uCanvas_Graph2D_Instance_t;


int uCanvasGUI_Create_Graph2D(uCanvas_Graph2D_Instance_t* Graph2D_Instance,int16_t x_pos, int16_t y_pos, int16_t width, int16_t height, int max_datapoints);
int uCanvasGui_Graph2D_Push_DataPoints(uCanvas_Graph2D_Instance_t* Graph2D_Instance, int datapoint_x, int datapoint_y);
int uCanvasGUI_Graph2D_Update(uCanvas_Graph2D_Instance_t* Graph2D_Instance);

int uCanvasGUI_Graph2D_Set_BackGround_Color(uCanvas_Graph2D_Instance_t* Graph2D_Instance, uint16_t r, uint16_t g, uint16_t b);
int uCanvasGUI_Graph2D_Set_Border_Color(uCanvas_Graph2D_Instance_t* Graph2D_Instance, uint16_t r, uint16_t g, uint16_t b);
int uCanvasGUI_Graph2D_Set_MainContent_Color(uCanvas_Graph2D_Instance_t* Graph2D_Instance, uint16_t r, uint16_t g, uint16_t b);
int uCanvasGui_Graph2D_Push_DataPoints_Y(uCanvas_Graph2D_Instance_t* Graph2D_Instance, int datapoint_y);
int uCanvasGui_Graph2D_Push_DataPoints_X(uCanvas_Graph2D_Instance_t* Graph2D_Instance, int datapoint_x);
int uCanvasGUI_Graph2D_Set_Title_Text(uCanvas_Graph2D_Instance_t* Graph2D_Instance, char* text);