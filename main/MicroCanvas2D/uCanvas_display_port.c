#include "uCanvas_display_port.h"


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
        SSD1306_DrawRectangle(x,y,w,h,color.monochrome_pixel);
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


void uCanvas_Draw_Line(_2dPoint_t point1, _2dPoint_t point2, color_t color){
        SSD1306_DrawLine(point1.x,point1.y,point2.x,point2.y,color.monochrome_pixel);
}

void uCanvas_Draw_Text(char* text, int x, int y, color_t color){
    SSD1306_GotoXY(x,y);
    SSD1306_Puts(text,&Font_7x10,color.monochrome_pixel);
}

void uCanvas_Draw_Triangle(_2dPoint_t point1, _2dPoint_t point2, _2dPoint_t point3, color_t color){
    SSD1306_DrawTriangle(point1.x,point1.y,point2.x,point2.y,point3.x, point3.y,color.monochrome_pixel);
}