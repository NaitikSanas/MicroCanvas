// #include "uCanvas_Frame_buffer.h"
// #include "stdint.h"

// typedef struct uCanvas_Frame_buffer
// {
//     uint16_t* ucanvas_local_frame_buf;
//     uint16_t res_x;
//     uint16_t res_y;
//     uint16_t pos_x;
//     uint16_t pos_y;
// }uCanvas_Frame_buffer_t;


// int create_new_frame(uCanvas_Frame_buffer_t* frame_obj, uint16_t rx, uint16_t ry){
//     frame_obj = (uCanvas_Frame_buffer_t*)malloc(sizeof(uCanvas_Frame_buffer_t));
//     frame_obj->ucanvas_local_frame_buf = (uint8_t)malloc(rx*ry);
//     frame_obj->res_x = rx;
//     frame_obj->res_y = ry;
//     return 1;
// }

// int fill_fbuf(uCanvas_Frame_buffer_t* frame_obj,uint16_t c){
//     for (int i = 0; i < (frame_obj->res_x*frame_obj->res_y); i++)
//     {
//         frame_obj->ucanvas_local_frame_buf[i] = c;
//     }    
// }

// int draw_pixel(uCanvas_Frame_buffer_t* frame_obj,uint16_t px, uint16_t py, uint16_t c){
//     frame_obj->ucanvas_local_frame_buf[frame_obj->res_x * py + px] = c;
//     return 1;
// }


