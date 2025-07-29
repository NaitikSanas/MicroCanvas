#include "uCanvas2D_Acceleration.h"

ppa_client_handle_t ppa_srm_handle = NULL;
ppa_client_handle_t ppa_blend_handle = NULL;
ppa_client_config_t ppa_blend_config = {
        .oper_type = PPA_OPERATION_BLEND,
        .max_pending_trans_num  = 10,
};


ppa_client_handle_t ppa_fill_handle = NULL;

ppa_client_config_t ppa_fill_config = {
        .oper_type = PPA_OPERATION_FILL,
        .max_pending_trans_num = 10,
};
ppa_client_config_t ppa_srm_config = {
        .oper_type = PPA_OPERATION_SRM,
        .max_pending_trans_num = 10,
};

void Intialize_PPA(void){
    ESP_ERROR_CHECK(ppa_register_client(&ppa_srm_config, &ppa_srm_handle));
    ESP_ERROR_CHECK(ppa_register_client(&ppa_fill_config, &ppa_fill_handle));
    ESP_ERROR_CHECK(ppa_register_client(&ppa_blend_config, &ppa_blend_handle));
}


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
)
{
    if (!out_buf || !ppa_fill_handle) return;

    ppa_fill_oper_config_t fill_config = {
        .out.buffer = out_buf,
        .out.buffer_size = buf_size,
        .out.pic_w = buf_width,
        .out.pic_h = buf_height,
        .out.block_offset_x = x,
        .out.block_offset_y = y,
        .out.fill_cm = PPA_FILL_COLOR_MODE_RGB565,
        .fill_block_w = w,
        .fill_block_h = h,
        .fill_argb_color = {
            .a = 0xFF, // fully opaque (not used in RGB565 anyway)
            .r = (color_rgb565 >> 11) << 3,
            .g = ((color_rgb565 >> 5) & 0x3F) << 2,
            .b = (color_rgb565 & 0x1F) << 3,
        },
        .mode = PPA_TRANS_MODE_BLOCKING,
    };
    ppa_do_fill(ppa_fill_handle, &fill_config);
}

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
)
{
    // Calculate how much of the sprite is actually visible onscreen
    int visible_w = in_w;
    int visible_h = in_h;

    if (out_offset_x < 0) {
        in_offset_x -= out_offset_x;
        visible_w += out_offset_x;  // reduce width
        out_offset_x = 0;
    }
    if (out_offset_y < 0) {
        in_offset_y -= out_offset_y;
        visible_h += out_offset_y;
        out_offset_y = 0;
    }

    if (out_offset_x + visible_w > out_w)
        visible_w = out_w - out_offset_x;

    if (out_offset_y + visible_h > out_h)
        visible_h = out_h - out_offset_y;

    // Guard: nothing to draw
    if (visible_w <= 0 || visible_h <= 0)
        return;

    ppa_srm_oper_config_t config = {
        .in.buffer = (void*)in_buf,
        .in.pic_w = in_w,
        .in.pic_h = in_h,
        .in.block_w = visible_w,
        .in.block_h = visible_h,
        .in.block_offset_x = in_offset_x,
        .in.block_offset_y = in_offset_y,
        .in.srm_cm = in_color_format,
        .in.blend_cm = in_color_format,

        .out.buffer = (uint16_t*)out_buf,
        .out.buffer_size = buf_size,
        .out.pic_w = out_w,
        .out.pic_h = out_h,
        .out.block_offset_x = out_offset_x,
        .out.block_offset_y = out_offset_y,
        .out.srm_cm = out_color_format,
        .out.blend_cm = out_color_format, 

        .rotation_angle = PPA_SRM_ROTATION_ANGLE_0,
        .scale_x = 1,
        .scale_y = 1,
        .rgb_swap = 0,
        .byte_swap = 0,
        .mirror_x = 0,
        .mirror_y = 0,
        .alpha_update_mode = PPA_ALPHA_NO_CHANGE,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    ESP_ERROR_CHECK(ppa_do_scale_rotate_mirror(ppa_srm_handle, &config));
}

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
) {
    int visible_w = in_w;
    int visible_h = in_h;

    if (out_offset_x < 0) {
        in_offset_x -= out_offset_x;
        visible_w += out_offset_x;
        out_offset_x = 0;
    }
    if (out_offset_y < 0) {
        in_offset_y -= out_offset_y;
        visible_h += out_offset_y;
        out_offset_y = 0;
    }
    if (out_offset_x + visible_w > out_w)
    visible_w = out_w - out_offset_x;
    if (in_offset_x + visible_w > in_w)
        visible_w = in_w - in_offset_x;

    if (out_offset_y + visible_h > out_h)
        visible_h = out_h - out_offset_y;
    if (in_offset_y + visible_h > in_h)
        visible_h = in_h - in_offset_y;

    // if (out_offset_x + visible_w > out_w)
    //     visible_w = out_w - out_offset_x;

    // if (out_offset_y + visible_h > out_h)
    //     visible_h = out_h - out_offset_y;

    if (visible_w <= 0 || visible_h <= 0)
        return;

    ppa_blend_oper_config_t blend_config = {
        .in_bg.buffer = out_buf,
        .in_bg.pic_w = out_w,
        .in_bg.pic_h = out_h,
        .in_bg.block_w = visible_w,
        .in_bg.block_h = visible_h,
        .in_bg.block_offset_x = out_offset_x,
        .in_bg.block_offset_y = out_offset_y,
        .in_bg.blend_cm = out_color_format,

        .in_fg.buffer = in_buf,
        .in_fg.pic_w = in_w,
        .in_fg.pic_h = in_h,
        .in_fg.block_w = visible_w,
        .in_fg.block_h = visible_h,
        .in_fg.block_offset_x = in_offset_x,
        .in_fg.block_offset_y = in_offset_y,
        .in_fg.blend_cm = in_color_format,

        .out.buffer = out_buf,
        .out.buffer_size = buf_size,
        .out.pic_w = out_w,
        .out.pic_h = out_h,
        .out.block_offset_x = out_offset_x,
        .out.block_offset_y = out_offset_y,
        .out.blend_cm = out_color_format,

        .bg_alpha_update_mode = PPA_ALPHA_NO_CHANGE,
        .fg_alpha_update_mode = PPA_ALPHA_NO_CHANGE,

        .bg_ck_en = false,
        .fg_ck_en = false,

        .mode = PPA_TRANS_MODE_BLOCKING,
    };
     ppa_do_blend(ppa_blend_handle, &blend_config);

}