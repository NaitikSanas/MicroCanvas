#include "uCanvas_api.h"


typedef struct icon
{
    /* data */
    uCanvas_universal_obj_t* icon[50];
    color_t color[50];
    int active_obj;
}icon_t;


typedef struct tiles
{
    int absolute_pos_x;
    int absolute_pos_y;
    int row;
    int col;
    int tiles_per_row;
    color_t tiles_color;
    int margin;
    int tile_width;
    int tile_height;
    uCanvas_universal_obj_t* selector;
    uCanvas_universal_obj_t* tiles[50];
    uCanvas_universal_obj_t* content[50];
    icon_t* icon;
}tiles_t;

void generate_grid_menu(tiles_t* tiles_menu, int n, int m, int tile_width, int tile_height, int margin, int r, int g, int b);
void show_tiles_with_fade_in_effect(tiles_t*tiles_menu,color_t color);
void show_tiles_with_fade_in_effect_at_index(tiles_t*tiles_menu,color_t color,int index);
void update_tile_with_fio_effect(tiles_t* tiles_menu,char* msg,int index);

void apply_fade_in_effect_to_obj(uCanvas_universal_obj_t* obj,color_t color);
void apply_fade_out_effect_to_obj(uCanvas_universal_obj_t* obj);
void pan_menu(tiles_t* tiles_menu, float pan_by_npixels, float steps,int dir);
int push_obj_as_tile_icon_element(tiles_t* tiles_menu, uCanvas_universal_obj_t* obj);
uCanvas_universal_obj_t* get_icon_obj_by_id(tiles_t* tiles_menu, int icon_element_id);
_2dPoint_t get_tile_positon(tiles_t*tiles_menu,  int index);

void add_tiles_selector(tiles_t* tiles_menu);
void select_tiles_by_index(tiles_t* tiles_menu,int tile_index);
void show_icons_by_fade_in_effect(tiles_t* tiles_menu);
void set_color_for_icon_element(tiles_t* tiles_menu,int icon_index,color_t c);
