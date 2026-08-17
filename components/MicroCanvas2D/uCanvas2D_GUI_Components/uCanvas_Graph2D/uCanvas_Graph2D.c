/*
 * uCanvas_Graph2D.c
 *
 * NOTE: Header additions required in uCanvas_Graph2D.h (not included here,
 * since it wasn't provided):
 *
 *   #define NUM_AXIS_TICKS 5
 *
 *   // Inside uCanvas_Graph2D_Instance_t:
 *   uCanvas_Element_t* x_axis_label[NUM_AXIS_TICKS];  // match your canvas[] element type
 *   uCanvas_Element_t* y_axis_label[NUM_AXIS_TICKS];
 */

#include "uCanvas_Graph2D.h"
#include <stdio.h>
#include <string.h>

static void _format_axis_value(char* buf, size_t buf_size, float value)
{
    // Use integer formatting if the value is a whole number, else 1 decimal
    if (value == (int)value)
        snprintf(buf, buf_size, "%d", (int)value);
    else
        snprintf(buf, buf_size, "%.1f", value);
}

int uCanvasGUI_Create_Graph2D(uCanvas_Graph2D_Instance_t* Graph2D_Instance,int16_t x_pos, int16_t y_pos, int16_t width, int16_t height, int max_datapoints){
    if(Graph2D_Instance){
        Graph2D_Instance->width = width;
        Graph2D_Instance->height = height;
        Graph2D_Instance->x_pos = x_pos;
        Graph2D_Instance->y_pos = y_pos;
        //Create Background
        Graph2D_Instance->canvas[0] = New_uCanvas_2DRectangle(x_pos,y_pos,height,width);
        Graph2D_Instance->canvas[0]->properties.color = Graph2D_Instance->Color_Style.background_color[0];
        uCanvas_Set_Fill(Graph2D_Instance->canvas[0],FILL);
        // Graph2D_Instance->canvas[0]->properties.visiblity = INVISIBLE;
        

        //Create Title TextBox
        Graph2D_Instance->canvas[2] = New_uCanvas_2DTextbox("2D Line Graph",x_pos+10,y_pos-20);
        Graph2D_Instance->canvas[2]->properties.color = Graph2D_Instance->Color_Style.main_content_color[0];

        //Create Axis Tick Labels
        for (int i = 0; i < NUM_AXIS_TICKS; i++)
        {
            Graph2D_Instance->x_axis_label[i] = New_uCanvas_2DTextbox("0", x_pos, y_pos + height + 5);
            uCanvas_Set_Color(Graph2D_Instance->x_axis_label[i], Graph2D_Instance->Color_Style.border_color[0].red,Graph2D_Instance->Color_Style.border_color[0].green,Graph2D_Instance->Color_Style.border_color[0].blue);
            
            Graph2D_Instance->y_axis_label[i] = New_uCanvas_2DTextbox("0", x_pos - 25, y_pos);
            uCanvas_Set_Color(Graph2D_Instance->y_axis_label[i], Graph2D_Instance->Color_Style.border_color[0].red,Graph2D_Instance->Color_Style.border_color[0].green,Graph2D_Instance->Color_Style.border_color[0].blue);
            
            uCanvas_Set_TextBox_FontType( Graph2D_Instance->x_axis_label[i],SFONT_12);
            uCanvas_Set_TextBox_FontType( Graph2D_Instance->y_axis_label[i],SFONT_12);
        }

        //Initialize Lines 
        for (int i = 0; i < MAX_GRAPH_LINES; i++)
        {
            Graph2D_Instance->datapoints_x[i] = 0;
            Graph2D_Instance->datapoints_y[i] = 0;
            
            Graph2D_Instance->graph_line[i] = New_uCanvas_2DLine(0,0,0,0);
            Graph2D_Instance->graph_line[i]->properties.color = Graph2D_Instance->Color_Style.main_content_color[0];
            Graph2D_Instance->graph_line[i]->properties.visiblity = INVISIBLE;
            uCanvas_Set_Thickness(Graph2D_Instance->graph_line[i],Graph2D_Instance->graph_line_thickness);
        }
        
        //Create Border 
        Graph2D_Instance->canvas[1] = New_uCanvas_2DRectangle(x_pos,y_pos,height,width);
        Graph2D_Instance->canvas[1]->properties.color = Graph2D_Instance->Color_Style.border_color[0];
        uCanvas_Set_Thickness(Graph2D_Instance->canvas[1],Graph2D_Instance->graph_line_thickness);
        uCanvas_Set_Fill(Graph2D_Instance->canvas[1],NOFILL);

        if(max_datapoints < MAX_GRAPH_LINES){
            Graph2D_Instance->data_ptr_x = 0;
            Graph2D_Instance->data_ptr_y = 0;
            Graph2D_Instance->max_datapoints = max_datapoints;
            return 1;
        }
        else {
            return 0;
        }
    }
    return 0;
}

int uCanvasGui_Graph2D_Push_DataPoints(uCanvas_Graph2D_Instance_t* Graph2D_Instance, int datapoint_x, int datapoint_y)
{
    if (!Graph2D_Instance)
        return 0;
        
    if (Graph2D_Instance->data_ptr_x < MAX_GRAPH_LINES)
    {
        Graph2D_Instance->datapoints_x[Graph2D_Instance->data_ptr_x] = datapoint_x;
        Graph2D_Instance->datapoints_y[Graph2D_Instance->data_ptr_y] = datapoint_y;
        Graph2D_Instance->data_ptr_x++;
        Graph2D_Instance->data_ptr_y++;
    }
    else
    {
        for (int i = 1; i < MAX_GRAPH_LINES; i++)
        {
            Graph2D_Instance->datapoints_x[i - 1] = Graph2D_Instance->datapoints_x[i];
            Graph2D_Instance->datapoints_y[i - 1] = Graph2D_Instance->datapoints_y[i];
        }

        Graph2D_Instance->datapoints_x[MAX_GRAPH_LINES - 1] = datapoint_x;
        Graph2D_Instance->datapoints_y[MAX_GRAPH_LINES - 1] = datapoint_y;
    }

    return 1;
}

int uCanvasGui_Graph2D_Push_DataPoints_X(uCanvas_Graph2D_Instance_t* Graph2D_Instance, int datapoint_x)
{
    if (!Graph2D_Instance)
        return 0;

    if (Graph2D_Instance->data_ptr_x < MAX_GRAPH_LINES)
    {
        Graph2D_Instance->datapoints_x[Graph2D_Instance->data_ptr_x] = datapoint_x;
        Graph2D_Instance->data_ptr_x++;
    }
    else
    {
        for (int i = 1; i < MAX_GRAPH_LINES; i++)
        {
            Graph2D_Instance->datapoints_x[i - 1] = Graph2D_Instance->datapoints_x[i];
        }
        Graph2D_Instance->datapoints_x[MAX_GRAPH_LINES - 1] = datapoint_x;
    }

    return 1;
}

int uCanvasGui_Graph2D_Push_DataPoints_Y(uCanvas_Graph2D_Instance_t* Graph2D_Instance, int datapoint_y)
{
    if (!Graph2D_Instance)
        return 0;
    if (Graph2D_Instance->data_ptr_y < MAX_GRAPH_LINES)
    {
        Graph2D_Instance->datapoints_y[Graph2D_Instance->data_ptr_y] = datapoint_y;
        Graph2D_Instance->data_ptr_y++;
    }
    else
    {
        for (int i = 1; i < MAX_GRAPH_LINES; i++)
        {
            Graph2D_Instance->datapoints_y[i - 1] = Graph2D_Instance->datapoints_y[i];
        }
        Graph2D_Instance->datapoints_y[MAX_GRAPH_LINES - 1] = datapoint_y;
    }
    return 1;
}

int _map_int(int value, int in_min, int in_max, int out_min, int out_max) {
    if (in_max == in_min) return out_min;
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

Coordinate2D_t _normalize_datapoint_xy(uCanvas_Graph2D_Instance_t* Graph2D_Instance,
                                      int16_t index)
{
    int graph_w = Graph2D_Instance->width;
    int graph_h = Graph2D_Instance->height-10;
    int graph_pos_x = Graph2D_Instance->x_pos;
    int graph_pos_y = Graph2D_Instance->y_pos;

    float data_x_min = Graph2D_Instance->data_x_min;
    float data_x_max = Graph2D_Instance->data_x_max;
    float data_y_min = Graph2D_Instance->data_y_min;
    float data_y_max = Graph2D_Instance->data_y_max;

    int16_t datapoint_x = Graph2D_Instance->datapoints_x[index];
    int16_t datapoint_y = Graph2D_Instance->datapoints_y[index];

    Coordinate2D_t Point = {0, 0};

    float x_range = data_x_max - data_x_min;
    float y_range = data_y_max - data_y_min;

    if (x_range == 0 || y_range == 0) {
        printf("Invalid data range: x_range=%.2f y_range=%.2f\n", x_range, y_range);
        return Point;
    }

    // Normalize and clamp to [0,1]
    float x_norm = (datapoint_x - data_x_min) / x_range;
    if (x_norm < 0.0f) x_norm = 0.0f;
    if (x_norm > 1.0f) x_norm = 1.0f;

    float y_norm = (datapoint_y - data_y_min) / y_range;
    if (y_norm < 0.0f) y_norm = 0.0f;
    if (y_norm > 1.0f) y_norm = 1.0f;

    // Convert normalized → pixel
    Point.x = graph_pos_x + (int)(x_norm * graph_w);
    Point.y = graph_pos_y + graph_h - (int)(y_norm * graph_h); // invert Y

    // printf("Raw(%d,%d) → Norm(%.2f,%.2f) → Pixel(%d,%d)\r\n",
        //    datapoint_x, datapoint_y, x_norm, y_norm, Point.x, Point.y);

    return Point;
}

int uCanvasGUI_Graph2D_Update_Axis_Labels(uCanvas_Graph2D_Instance_t* Graph2D_Instance)
{
    if (!Graph2D_Instance)
        return 0;

    int graph_w = Graph2D_Instance->width;
    int graph_h = Graph2D_Instance->height - 10;
    int graph_pos_x = Graph2D_Instance->x_pos;
    int graph_pos_y = Graph2D_Instance->y_pos;

    float x_min = Graph2D_Instance->data_x_min;
    float x_max = Graph2D_Instance->data_x_max;
    float y_min = Graph2D_Instance->data_y_min;
    float y_max = Graph2D_Instance->data_y_max;

    char label_buf[16];

    for (int i = 0; i < NUM_AXIS_TICKS; i++)
    {
        float t = (float)i / (NUM_AXIS_TICKS - 1);

        // X axis: left -> right, min -> max
        float x_value = x_min + t * (x_max - x_min);
        int x_px = graph_pos_x + (int)(t * graph_w);
        int x_py = graph_pos_y + graph_h + 5;

        _format_axis_value(label_buf, sizeof(label_buf), x_value);
        uCanvas_Set_Text(Graph2D_Instance->x_axis_label[i], label_buf);
        uCanvas_Set_Position(Graph2D_Instance->x_axis_label[i], x_px, x_py);
        Graph2D_Instance->x_axis_label[i]->properties.visiblity = VISIBLE;

        // Y axis: bottom -> top, min -> max (inverted, since screen Y grows downward)
        float y_value = y_min + t * (y_max - y_min);
        int y_px = graph_pos_x - 25;
        int y_py = graph_pos_y + graph_h - (int)(t * graph_h);

        _format_axis_value(label_buf, sizeof(label_buf), y_value);
        uCanvas_Set_Text(Graph2D_Instance->y_axis_label[i], label_buf);
        uCanvas_Set_Position(Graph2D_Instance->y_axis_label[i], y_px, y_py);
        Graph2D_Instance->y_axis_label[i]->properties.visiblity = VISIBLE;
    }

    return 1;
}

int uCanvasGUI_Graph2D_Update(uCanvas_Graph2D_Instance_t* Graph2D_Instance)
{
    if (!Graph2D_Instance || Graph2D_Instance->data_ptr_x < 2 || Graph2D_Instance->data_ptr_y < 2 )
        return 0; // no data or invalid

    int count = Graph2D_Instance->data_ptr_y;
    Graph2D_Instance->graph_line[MAX_GRAPH_LINES - 1]->properties.visiblity = INVISIBLE;
    // printf("data_ptr_y %d\r\n",count);
    for (int i = 0; i < count-1; i++)
    {
        Coordinate2D_t p1 = _normalize_datapoint_xy(Graph2D_Instance, i);
        Coordinate2D_t p2 = _normalize_datapoint_xy(Graph2D_Instance, i + 1);
        if (Graph2D_Instance->datapoints_x[i+1] < Graph2D_Instance->datapoints_x[i])
        continue; 

        if(Graph2D_Instance->GraphType == GRAPH2D_LINE){
            Graph2D_Instance->graph_line[i]->properties.type = LINE;
            Graph2D_Instance->graph_line[i]->properties.visiblity = VISIBLE;
            Graph2D_Instance->graph_line[i]->point1 = p1;
            Graph2D_Instance->graph_line[i]->point2 = p2;
            Graph2D_Instance->graph_line[i]->properties.color = Graph2D_Instance->Color_Style.main_content_color[0];
        }
        if(Graph2D_Instance->GraphType == GRAPH2D_DOTTTED){
            Graph2D_Instance->graph_line[i]->properties.type = CIRCLE;
            uCanvas_Set_Radius1(Graph2D_Instance->graph_line[i],2);
            uCanvas_Set_Position(Graph2D_Instance->graph_line[i],p1.x,p1.y);

            Graph2D_Instance->graph_line[i+1]->properties.type = CIRCLE;
            uCanvas_Set_Radius1(Graph2D_Instance->graph_line[i+1],2);
            uCanvas_Set_Position(Graph2D_Instance->graph_line[i+1],p1.x,p2.y);

            Graph2D_Instance->graph_line[i]->properties.visiblity = VISIBLE;
            Graph2D_Instance->graph_line[i]->point1 = p1;
            Graph2D_Instance->graph_line[i]->point2 = p2;
            Graph2D_Instance->graph_line[i]->properties.color = Graph2D_Instance->Color_Style.main_content_color[0];
        }
        if ((p1.x < Graph2D_Instance->x_pos || p1.x > Graph2D_Instance->x_pos + Graph2D_Instance->width) ||
            (p2.x < Graph2D_Instance->x_pos || p2.x > Graph2D_Instance->x_pos + Graph2D_Instance->width))
            Graph2D_Instance->graph_line[i]->properties.visiblity = INVISIBLE;

        // uCanvas_DrawLine(Graph2D_Instance->canvas, p1.x, p1.y, p2.x, p2.y, Graph2D_Instance->line_color);
    }

    return 1;
}

int uCanvasGUI_Graph2D_Set_BackGround_Color(uCanvas_Graph2D_Instance_t* Graph2D_Instance, uint16_t r, uint16_t g, uint16_t b){
    if(Graph2D_Instance){
        Graph2D_Instance->Color_Style.background_color[0].red   = r;
        Graph2D_Instance->Color_Style.background_color[0].green = g;
        Graph2D_Instance->Color_Style.background_color[0].blue  = b;
        Graph2D_Instance->canvas[0]->properties.color = Graph2D_Instance->Color_Style.background_color[0];
        return 1;
    }
    

    return 0;
}

int uCanvasGUI_Graph2D_Set_Border_Color(uCanvas_Graph2D_Instance_t* Graph2D_Instance, uint16_t r, uint16_t g, uint16_t b){
    if(Graph2D_Instance){
        Graph2D_Instance->Color_Style.border_color[0].red   = r;
        Graph2D_Instance->Color_Style.border_color[0].green = g;
        Graph2D_Instance->Color_Style.border_color[0].blue  = b;
        Graph2D_Instance->canvas[1]->properties.color = Graph2D_Instance->Color_Style.border_color[0];
        return 1;
    }
    return 0;
}

int uCanvasGUI_Graph2D_Set_MainContent_Color(uCanvas_Graph2D_Instance_t* Graph2D_Instance, uint16_t r, uint16_t g, uint16_t b){
    if(Graph2D_Instance){
        Graph2D_Instance->Color_Style.main_content_color[0].red   = r;
        Graph2D_Instance->Color_Style.main_content_color[0].green = g;
        Graph2D_Instance->Color_Style.main_content_color[0].blue  = b;
        Graph2D_Instance->canvas[2]->properties.color = Graph2D_Instance->Color_Style.main_content_color[0];
        return 1;
    }
    return 0;
}

int uCanvasGUI_Graph2D_Set_Title_Text(uCanvas_Graph2D_Instance_t* Graph2D_Instance, char* text){
    if(Graph2D_Instance){
        uCanvas_Set_Text(Graph2D_Instance->canvas[2],text);
        return 1;
    }
    return 0;
}