#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "uCanvas_Physix.h"
typedef struct network_strenght_indicator
{
    uCanvas_universal_obj_t* obj[5];
    uint16_t gap_x;
    uint16_t height_max;
    uint16_t width;
    uint16_t position_x;
    uint16_t position_y;
    color_t  active_color;
    color_t  inactive_color;
}network_strenght_indicator_t;

typedef struct status_bar
{
   uCanvas_universal_obj_t* timestamp;
   uCanvas_universal_obj_t* status_icons[8];
   network_strenght_indicator_t network_strength_indicator;
   uint16_t pos_x; 
   uint16_t pos_y;
   color_t default_color_1;
   color_t default_color_2;
}status_bar_t;

typedef struct tile_menu
{
    uCanvas_universal_obj_t* tiles          [10][10];
    uCanvas_universal_obj_t* tiles_label    [10][10];
    uCanvas_universal_obj_t* tiles_content1  [10][10];
    uCanvas_universal_obj_t* tiles_content2  [10][10];
    int selector_x;
    int selector_y;
    int tile_menu_pos_x;
    int tile_menu_pos_y;

    int tile_width;
    int tile_height;
    
    int tile_gap_x;
    int tile_gap_y;
    
    int tile_num_x;
    int tile_num_y;

    int tile_text_x ;
    int tile_text_y ;
    color_t color_1;
    color_t color_2;
    color_t color_3;

}tile_menu_t;

void color_wipe(uCanvas_universal_obj_t* obj, color_t from_color, color_t to_color, uint32_t delay_us, uint16_t steps);
void create_tile_menu(tile_menu_t* tile_menu);
void set_tile_label(tile_menu_t* tile_menu, char* data, int row, int col);
void pan_tile_menu(tile_menu_t* tile_menu, char axis, int steps, int step, uint32_t delay_us);
void create_network_strength(status_bar_t* status_bar_obj);
int compare_colors(color_t a, color_t b);
void update_network_strength(status_bar_t* status_bar_obj, int level);
void update_cursor(tile_menu_t* tile_menu_obj,int dir);
void uCanvas_GUI_Demo_Setup() ;
void uCanvas_GUI_Demo_Main(void);
void rssi_monitor();
void clock_update_task();
