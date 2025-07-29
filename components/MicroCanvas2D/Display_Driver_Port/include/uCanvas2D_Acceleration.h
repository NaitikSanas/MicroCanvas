#include "stdio.h"
#include "stdint.h"
#if CONFIG_IDF_TARGET_ESP32P4
#include "driver/ppa.h"

#define PPA_RGB565 PPA_SRM_COLOR_MODE_RGB565
#define PPA_ARGB8888 PPA_SRM_COLOR_MODE_ARGB8888
typedef ppa_srm_color_mode_t bitmap_color_format_t;

void ppa_srm_bitmap(
    void* in_buf, 
    int in_w, 
    int in_h, 
    int in_offset_x, 
    int in_offset_y,
    bitmap_color_format_t in_color_format,
    void* out_buf, 
    int out_w, 
    int out_h, 
    int out_offset_x, 
    int out_offset_y, 
    size_t buf_size,
    bitmap_color_format_t out_color_format
);

void ppa_blend_bitmap(
    void* in_buf, 
    int in_w, 
    int in_h, 
    int in_offset_x, 
    int in_offset_y,
    bitmap_color_format_t in_color_format,
    void* out_buf, 
    int out_w, 
    int out_h, 
    int out_offset_x, 
    int out_offset_y, 
    size_t buf_size,
    bitmap_color_format_t out_color_format
);

void ppa_helper_fill(
    void *out_buf,
    size_t buf_size,
    uint16_t buf_width,
    uint16_t buf_height,
    uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h,
    uint16_t color_rgb565
);

void Intialize_PPA(void);
#else
//show info during compilation
#pragma message("PPA is not supported on this target, using software rendering")

#endif