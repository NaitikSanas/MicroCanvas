#include "uCanvas_display_port.h"
#include "ssd1306.h"
#ifdef USE_SSD1306
void uCanvas_Display_init(void){
    SSD1306_Init();
}

void uCanvas_Display_clear_buffer(void){
    SSD1306_Clear();
}

void uCanvas_Update_Display(void){
    SSD1306_UpdateScreen();
}

void uCanvaas_Display_2D_Element(uCanvas_element_type_t element,uCanvas_Disp_Ctx_t ctx ){
    switch (element)
    {
    case RECTANGLE:{
        if(ctx.fill)SSD1306_DrawFilledRectangle(ctx.xPos,ctx.yPos,ctx.w1,ctx.h1,ctx.color); 
        else SSD1306_DrawRectangle(ctx.xPos,ctx.yPos,ctx.w1,ctx.h1,ctx.color); 
        break;
    } 
    default:
        break;
    }
} 


void uCanvas_Draw_Rectangle(int x, int y, int h, int w, color_t color, fill_t fill){
    if(fill){
        SSD1306_DrawFilledRectangle(x,y,w,h,color.monochrome_pixel);
    }
    else {
        SSD1306_DrawRectangle2(x,y,w,h,color.monochrome_pixel);
    }
}

void uCanvas_Draw_Circle(int x, int y, int r, color_t color, fill_t fill){
    if(fill){
        SSD1306_DrawFilledCircle(x,y,r,color.monochrome_pixel);
    }
    else {     
        SSD1306_DrawCircle(x,y,r,color.monochrome_pixel);
    }
}


void uCanvas_Draw_Line(Coordinate2D_t point1, Coordinate2D_t point2, color_t color){
    // printf("line p1(%d, %d) , p2(%d, %d)\r\n",point1.x,point1.y,point2.x,point2.y);
        SSD1306_DrawLine(point1.x,point1.y,point2.x,point2.y,color.monochrome_pixel);
}

void uCanvas_Draw_Text(char* text, int x, int y, color_t color){
    SSD1306_GotoXY(x,y);
    SSD1306_Puts(text,&Font_7x10,color.monochrome_pixel);
}

void uCanvas_Draw_Triangle(Coordinate2D_t point1, Coordinate2D_t point2, Coordinate2D_t point3, color_t color, fill_t fill){
    if(fill)
        SSD1306_DrawFilledTriangle(point1.x,point1.y,point2.x,point2.y,point3.x, point3.y,color.monochrome_pixel);
    else
    SSD1306_DrawTriangle(point1.x,point1.y,point2.x,point2.y,point3.x, point3.y,color.monochrome_pixel);
}

void uCanvas_DrawPixel(Coordinate2D_t pos,color_t color){
    SSD1306_DrawPixel(pos.x,pos.y,color.monochrome_pixel);
}
#endif