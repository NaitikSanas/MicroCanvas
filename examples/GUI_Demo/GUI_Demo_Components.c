#include "GUI_Demo.h"

void color_wipe(uCanvas_universal_obj_t* obj, color_t from_color, color_t to_color, uint32_t delay_us, uint16_t steps) {
    int16_t red_diff = to_color.red - from_color.red;
    int16_t green_diff = to_color.green - from_color.green;
    int16_t blue_diff = to_color.blue - from_color.blue;

    for (uint16_t step = 0; step < steps; step++) {
        color_t current_color;
        current_color.red = from_color.red + (red_diff * step) / steps;
        current_color.green = from_color.green + (green_diff * step) / steps;
        current_color.blue = from_color.blue + (blue_diff * step) / steps;
        uCanvas_Set_Color(obj, current_color.red, current_color.green, current_color.blue);
        esp_rom_delay_us(delay_us);
    }
    uCanvas_Set_Color(obj, to_color.red, to_color.green, to_color.blue);
}


void create_tile_menu(tile_menu_t* tile_menu){
    tile_menu->selector_x = 0;
    tile_menu->selector_y = 0;
    for (int row = 0; row < tile_menu->tile_num_y; row++)
    {
        
        for (int col = 0; col < tile_menu->tile_num_x; col++)
        {  
            // printf("rc[%d][%d]\r\n",row,col);
            int tile_pos_x = tile_menu->tile_menu_pos_x + (tile_menu->tile_gap_x + tile_menu->tile_width)*col;
            int tile_pos_y = tile_menu->tile_menu_pos_y + (tile_menu->tile_gap_y + tile_menu->tile_width)*row;
            tile_menu->tiles[row][col] = New_uCanvas_2DRectangle(tile_pos_x,tile_pos_y,tile_menu->tile_height,tile_menu->tile_width);      
            tile_menu->tiles_label[row][col] = New_uCanvas_2DTextbox("NULL",tile_pos_x+tile_menu->tile_text_x,tile_pos_y+tile_menu->tile_text_y);
            tile_menu->tiles[row][col]->properties.fill = FILL; 
            color_wipe(tile_menu->tiles[row][col],tile_menu->color_1,tile_menu->color_2,1000,100);
            color_wipe(tile_menu->tiles_label[row][col],tile_menu->color_1,tile_menu->color_3,1000,100);
            tile_menu->tiles_content1[row][col] = New_uCanvas_2DRectangle(tile_pos_x,tile_pos_y,tile_menu->tile_height,tile_menu->tile_width);
            uCanvas_Set_Color(tile_menu->tiles_content1[row][col],tile_menu->color_2.red,tile_menu->color_2.green,tile_menu->color_2.blue);
        }    
    }
}

void set_tile_label(tile_menu_t* tile_menu, char* data, int row, int col){
    uCanvas_Set_Text(tile_menu->tiles_label[row][col],data);
}



void pan_tile_menu(tile_menu_t* tile_menu, char axis, int steps, int step, uint32_t delay_us){
    for (int i = 0; i < steps; i++)
    {
        for (int row = 0; row < tile_menu->tile_num_y; row++)
        {
            for (int col = 0; col < tile_menu->tile_num_x; col++)
            {  
                if(axis == 'x'){ 
                    tile_menu->tiles[row][col]->properties.position.x += step;
                    tile_menu->tiles_label[row][col]->properties.position.x += step;
                };
                if(axis == 'y'){ 
                    tile_menu->tiles[row][col]->properties.position.y += step;
                    tile_menu->tiles_label[row][col]->properties.position.y += step;
                };
            }    
        }
        esp_rom_delay_us(delay_us);
    }
}

void create_network_strength(status_bar_t* status_bar_obj){
    int height = status_bar_obj->network_strength_indicator.height_max;
    for (int i = 4; i >= 0; i--)
    {
        int pos_x = status_bar_obj->network_strength_indicator.position_x + (status_bar_obj->network_strength_indicator.gap_x + status_bar_obj->network_strength_indicator.width)  * i;
        int pos_y = status_bar_obj->network_strength_indicator.position_y - i*2;
        status_bar_obj->network_strength_indicator.obj[i] = New_uCanvas_2DRectangle(pos_x,pos_y, height,status_bar_obj->network_strength_indicator.width);
        uCanvas_Set_Color(status_bar_obj->network_strength_indicator.obj[i],status_bar_obj->network_strength_indicator.inactive_color.red,status_bar_obj->network_strength_indicator.inactive_color.green,status_bar_obj->network_strength_indicator.inactive_color.blue);
        uCanvas_Set_Fill(status_bar_obj->network_strength_indicator.obj[i],FILL);
        height -= status_bar_obj->network_strength_indicator.height_max/5;
    }
}

int compare_colors(color_t a, color_t b){
    if(a.red == b.red){
        if(a.green == b.green){
            if(a.blue == b.blue){
                return 1;
            }
        }
    }
    return 0;
}

void update_network_strength(status_bar_t* status_bar_obj, int level){
    color_t active_color   = status_bar_obj->network_strength_indicator.active_color;
    color_t inactive_color = status_bar_obj->network_strength_indicator.inactive_color;
    uint32_t delay_us = 1000;
    uint32_t steps = 80;
    switch (level)
    {
    case 0:
        if(compare_colors(status_bar_obj->network_strength_indicator.obj[4]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[4],active_color,inactive_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[3]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[3],active_color,inactive_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[2]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[2],active_color,inactive_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[1]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[1],active_color,inactive_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[0]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[0],active_color,inactive_color,delay_us,steps);

        break;
    case 1:
        if(compare_colors(status_bar_obj->network_strength_indicator.obj[0]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[0],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[1]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[1],active_color,inactive_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[2]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[2],active_color,inactive_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[3]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[3],active_color,inactive_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[4]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[4],active_color,inactive_color,delay_us,steps);

        break;
    case 2:
        if(compare_colors(status_bar_obj->network_strength_indicator.obj[0]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[0],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[1]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[1],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[2]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[2],active_color,inactive_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[3]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[3],active_color,inactive_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[4]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[4],active_color,inactive_color,delay_us,steps);
        break;
    case 3:
        if(compare_colors(status_bar_obj->network_strength_indicator.obj[0]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[0],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[1]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[1],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[2]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[2],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[3]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[3],active_color,inactive_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[4]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[4],active_color,inactive_color,delay_us,steps);
        break;
    case 4:
        if(compare_colors(status_bar_obj->network_strength_indicator.obj[0]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[0],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[1]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[1],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[2]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[2],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[3]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[3],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[4]->properties.color, active_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[4],active_color,inactive_color,delay_us,steps);
        break;
    case 5:
        if(compare_colors(status_bar_obj->network_strength_indicator.obj[0]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[0],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[1]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[1],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[2]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[2],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[3]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[3],inactive_color,active_color,delay_us,steps);

        if(compare_colors(status_bar_obj->network_strength_indicator.obj[4]->properties.color, inactive_color)) 
            color_wipe(status_bar_obj->network_strength_indicator.obj[4],inactive_color,active_color,delay_us,steps);
        break;
    default:
        break;
    }
}

void update_cursor(tile_menu_t* tile_menu_obj,int dir){
    uCanvas_universal_obj_t* obj = (uCanvas_universal_obj_t*)  (tile_menu_obj->tiles_content1[tile_menu_obj->selector_y][tile_menu_obj->selector_x]);
    color_wipe( obj,
                obj->properties.color, 
                tile_menu_obj->color_2,1000,100);
    if(tile_menu_obj->selector_x < tile_menu_obj->tile_num_x-1){
        tile_menu_obj->selector_x++;
    }
    else{
        tile_menu_obj->selector_x = 0;
        if(tile_menu_obj->selector_y < tile_menu_obj->tile_num_y-1){
            tile_menu_obj->selector_y++;
        }
    }
    obj = (uCanvas_universal_obj_t*)  (tile_menu_obj->tiles_content1[tile_menu_obj->selector_y][tile_menu_obj->selector_x]);
    color_wipe( obj,
                obj->properties.color, 
                tile_menu_obj->color_3,1000,100);
}