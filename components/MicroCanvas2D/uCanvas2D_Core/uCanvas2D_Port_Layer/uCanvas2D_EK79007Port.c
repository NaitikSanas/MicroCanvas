#include "uCanvas2D_EK79007Port.h"
#if(CONFIG_IDF_TARGET_ESP32P4)

// Scratch buffer reused for packing strided tiles (dirty-rect pushes).
// This avoids malloc/free on every partial redraw.
static uint16_t* s_tile_scratch = NULL;
static size_t s_tile_scratch_pixels = 0;

void EK79007_Init(uCanvas2D_Display_Orientation_t orientation){
    printf("EK79007_Init prototype\r\n");
    ek79007_init();

}


void EK79007_PushTile(int x, int y, uCanvas2D_RenderBuffer_t* buf){
    if (!buf || !buf->pixels) {
        printf("NULL PIX BUF\r\n");
        return;
    }
    const uint16_t* pixels = (const uint16_t*) buf->pixels;
    int w = buf->width;
    int h = buf->height;
    int pitch = buf->pitch;
    if(pitch <= 0) pitch = w;
    // printf("dim(%d,%d)\r\n",w,h);
    if(pitch == w){
        ek79007_draw_tile(x,y,w,h,pixels);
    }else{
        // ek79007_draw_tile expects tightly packed pixels; copy row-by-row.
        size_t need_pixels = (size_t)w * (size_t)h;
        if(need_pixels > s_tile_scratch_pixels){
            uint16_t* new_buf = (uint16_t*)realloc(s_tile_scratch, need_pixels * sizeof(uint16_t));
            if(!new_buf){
                printf("EK79007_PushTile: scratch realloc failed\r\n");
                return;
            }
            s_tile_scratch = new_buf;
            s_tile_scratch_pixels = need_pixels;
        }
        if(!s_tile_scratch){
            printf("EK79007_PushTile: scratch alloc failed\r\n");
            return;
        }
        for(int row = 0; row < h; row++){
            memcpy(&s_tile_scratch[row * w], &pixels[row * pitch], (size_t)w * sizeof(uint16_t));
        }
        ek79007_draw_tile(x,y,w,h,s_tile_scratch);
    }
}

void EK79007_Adjust_Brightness(int percent){
    printf("EK79007_Adjust_Brightness prototype\r\n");
}

uCanvas2D_Display_Panel_t* uCanvas2D_Get_Panel_Driver_EK79007(void){
    uCanvas2D_Display_Panel_t* driver = (uCanvas2D_Display_Panel_t*) malloc(sizeof(uCanvas2D_Display_Panel_t));
    driver->init = EK79007_Init;
    driver->push_render_buffer = EK79007_PushTile;
    driver->set_backlight = EK79007_Adjust_Brightness;
    driver->height = LCD_V_RES;
    driver->width = LCD_H_RES;
    return driver;
}
#endif
