#include "grid_menu.h"
#include "math.h"

void generate_grid_menu(tiles_t* tiles_menu, int n, int m, int tile_width, int tile_height, int margin, int r, int g, int b) {
    tiles_menu->icon = (icon_t*)malloc(sizeof(icon_t));
    tiles_menu->icon->active_obj = 0;
    tiles_menu->absolute_pos_x = 5;
    tiles_menu->absolute_pos_y = 15;
    tiles_menu->row = n;
    tiles_menu->col = m;
    tiles_menu->tiles_per_row = m;
    tiles_menu->margin = margin;
    tiles_menu->tile_width = tile_width;
    tiles_menu->tile_height = tile_height;
    tiles_menu->tiles_color.red   = r;
    tiles_menu->tiles_color.green = g;
    tiles_menu->tiles_color.blue  = b;
    char buf[32];
    
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < m; ++col) {
            int index = row * m + col;
            if (index < 256) {
                int x = margin + col * (tile_width + margin) +  tiles_menu->absolute_pos_x;
                int y = margin + row * (tile_height + margin) + tiles_menu->absolute_pos_y ;
                tiles_menu->tiles[index] = New_uCanvas_2DRectangle(x, y, tile_width, tile_height);
                uCanvas_Set_Color(tiles_menu->tiles[index], tiles_menu->tiles_color.red, tiles_menu->tiles_color.green, tiles_menu->tiles_color.blue);
                tiles_menu->tiles[index]->properties.fill = FILL;
                printf("added at %d\r\n",index);
                memset(buf,0,32);
                sprintf(buf,"TILE %d",index);
                tiles_menu->content[index] = New_uCanvas_2DTextbox(buf,x+tile_width/2-4, y+5);
                uCanvas_Set_Text(tiles_menu->content[index],buf);
                uCanvas_Set_Color(tiles_menu->content[index],255,255,255);
                tiles_menu->content[index]->properties.visiblity = INVISIBLE;
            }
        }                       
    }
}

void pan_menu(tiles_t* tiles_menu, float pan_by_npixels, float steps, int dir){
    for (float p = 0; p < pan_by_npixels; p+= steps)
    {
        for (int i = 0; i < tiles_menu->row * tiles_menu->col; i++)
        {
            
            if (dir == 0)  {
                tiles_menu->content[i]->properties.position.y -= floor(p);
                tiles_menu->tiles[i]->properties.position.y   -= floor(p);
                
            }
            else if(dir == 1) {
                tiles_menu->content[i]->properties.position.y += floor(p);
                tiles_menu->tiles[i]->properties.position.y   += floor(p);
               
            }
            else if(dir == 2) {
                tiles_menu->content[i]->properties.position.x += floor(p);
                tiles_menu->tiles[i]->properties.position.x   += floor(p);
               
            }

            else if(dir == 3) {
                tiles_menu->content[i]->properties.position.x -= floor(p);
                tiles_menu->tiles[i]->properties.position.x   -= floor(p);
               
            }
             

        }
        if(tiles_menu->icon != NULL){
            if(dir == 0){
                for (int icon_idx = 0; icon_idx < tiles_menu->icon->active_obj; icon_idx++)
                {
                    tiles_menu->icon->icon[icon_idx]->properties.position.y -= floor(p);
                }
            }
            else if(dir == 1){
                for (int icon_idx = 0; icon_idx < tiles_menu->icon->active_obj; icon_idx++)
                {
                    tiles_menu->icon->icon[icon_idx]->properties.position.y += floor(p);
                }         
            }

            else if(dir == 2){
                for (int icon_idx = 0; icon_idx < tiles_menu->icon->active_obj; icon_idx++)
                {
                    tiles_menu->icon->icon[icon_idx]->properties.position.x += floor(p);
                }         
            }
            else if(dir == 3){
                for (int icon_idx = 0; icon_idx < tiles_menu->icon->active_obj; icon_idx++)
                {
                    tiles_menu->icon->icon[icon_idx]->properties.position.x -= floor(p);
                }         
            }
        }
        uCanvas_Delay(5);
    }
}

void set_tiles_content(tiles_t*obj, char* text_content, int index){

}


void show_tiles_with_fade_in_effect(tiles_t*tiles_menu,color_t color){
    for (int i = 0; i < tiles_menu->row*tiles_menu->col; i++)
    {
        float tr = (float)color.red;
        float tg = (float)color.green;
        float tb = (float)color.blue;
        float tmpr=0.0, tmpb=0.0, tmpg=0.0;
        for (float b = 0.0; b < 1.0; b+=0.03)
        {
            tmpr = tr * b;
            tmpg = tg * b;
            tmpb = tb * b;
            tiles_menu->tiles[i]->properties.color.red   = floor(tmpr);
            tiles_menu->tiles[i]->properties.color.green = floor(tmpg);
            tiles_menu->tiles[i]->properties.color.blue  = floor(tmpb);
            uCanvas_Delay(1);  
        }  
        tiles_menu->content[i]->properties.visiblity = VISIBLE;
        // uCanvas_Delay(1);
    }
}


void show_tiles_with_fade_in_effect_at_index(tiles_t*tiles_menu,color_t color,int index){
    float tr = (float)color.red;
    float tg = (float)color.green;
    float tb = (float)color.blue;
    float tmpr=0.0, tmpb=0.0, tmpg=0.0;
    for (float b = 0.0; b < 1.0; b+=0.05)
    {
        tmpr = tr * b;
        tmpg = tg * b;
        tmpb = tb * b;
        tiles_menu->tiles[index]->properties.color.red   = floor(tmpr);
        tiles_menu->tiles[index]->properties.color.green = floor(tmpg);
        tiles_menu->tiles[index]->properties.color.blue  = floor(tmpb);
        uCanvas_Delay(1);  
    }  
    tiles_menu->content[index]->properties.visiblity = VISIBLE;
}

void update_tile_with_fio_effect(tiles_t* tiles_menu,char* msg,int index){
        color_t c;
        c.red   = tiles_menu->tiles[index]->properties.color.red;
        c.green = tiles_menu->tiles[index]->properties.color.green;
        c.blue  = tiles_menu->tiles[index]->properties.color.blue;

        float tr = (float)tiles_menu->tiles[index]->properties.color.red;
        float tg = (float)tiles_menu->tiles[index]->properties.color.green;
        float tb = (float)tiles_menu->tiles[index]->properties.color.blue;
        tiles_menu->content[index]->properties.visiblity = INVISIBLE;
        for (float b = 1.0; b >0; b-=0.02)
        {
            tr = tr * b;
            tg = tg * b;
            tb = tb * b;
            tiles_menu->tiles[index]->properties.color.red   = floor(tr);
            tiles_menu->tiles[index]->properties.color.green = floor(tg);
            tiles_menu->tiles[index]->properties.color.blue  = floor(tb);
            uCanvas_Delay(1);  
        }  
        uCanvas_Set_Text(tiles_menu->content[index],msg);
        show_tiles_with_fade_in_effect_at_index(tiles_menu,c,index);
        // uCanvas_Delay(1);
}

void apply_fade_in_effect_to_obj(uCanvas_universal_obj_t* obj,color_t color){
    obj->properties.visiblity = VISIBLE;
    float tr = (float)color.red;
    float tg = (float)color.green;
    float tb = (float)color.blue;
    float tmpr=0.0, tmpb=0.0, tmpg=0.0;
    for (float b = 0.0; b < 1.0; b+=0.05)
    {
        tmpr = tr * b;
        tmpg = tg * b;
        tmpb = tb * b;
        obj->properties.color.red   = floor(tmpr);
        obj->properties.color.green = floor(tmpg);
        obj->properties.color.blue  = floor(tmpb);
        uCanvas_Delay(1);  
    }  
}

void apply_fade_out_effect_to_obj(uCanvas_universal_obj_t* obj){
    obj->properties.visiblity = VISIBLE;
    float tr = (float)obj->properties.color.red;
    float tg = (float)obj->properties.color.green;
    float tb = (float)obj->properties.color.blue;
    for (float b = 1.0; b >0; b-=0.02)
    {
        tr = tr * b;
        tg = tg * b;
        tb = tb * b;
        obj->properties.color.red   = floor(tr);
        obj->properties.color.green = floor(tg);
        obj->properties.color.blue  = floor(tb);
        uCanvas_Delay(1);  
    }    
}


int push_obj_as_tile_icon_element(tiles_t* tiles_menu, uCanvas_universal_obj_t* obj){
    if(tiles_menu->icon == NULL){
        printf("icon space initialization\r\n");
        tiles_menu->icon = (icon_t*)malloc(sizeof(icon_t));
        tiles_menu->icon->active_obj = 0;
    }
    else {
        printf("icon space available to use\r\n");
    }
    if(obj != NULL){
        printf("icon element added\r\n");
        tiles_menu->icon->icon[tiles_menu->icon->active_obj] = obj;
        tiles_menu->icon->active_obj++;
    }
    else {
        printf("cannot add icon element\r\n");
    }
    return (tiles_menu->icon->active_obj-1);
}

uCanvas_universal_obj_t* get_icon_obj_by_id(tiles_t* tiles_menu, int icon_element_id){
    return tiles_menu->icon->icon[icon_element_id];
}

_2dPoint_t get_tile_positon(tiles_t*tiles_menu,  int index){
    return tiles_menu->tiles[index]->properties.position;
}

void add_tiles_selector(tiles_t* tiles_menu){
    int xs = get_tile_positon(tiles_menu,0).x;
    int ys = get_tile_positon(tiles_menu,0).y;
    tiles_menu->selector = New_uCanvas_2DRectangle(xs,ys,tiles_menu->tile_height,tiles_menu->tile_width);
    uCanvas_Set_Color(tiles_menu->selector,255,255,0);   
}
int last_index = -1;
void select_tiles_by_index(tiles_t* tiles_menu,int tile_index){

    color_t fc = {.red = 255, .green = 255, .blue = 0};
    apply_fade_out_effect_to_obj(tiles_menu->selector);
    tiles_menu->selector->width = tiles_menu->tiles[tile_index]->width ;
    tiles_menu->selector->height = tiles_menu->tiles[tile_index]->height ;
    tiles_menu->selector->properties.position.x = tiles_menu->tiles[tile_index]->properties.position.x;
    tiles_menu->selector->properties.position.y = tiles_menu->tiles[tile_index]->properties.position.y;
    apply_fade_in_effect_to_obj(tiles_menu->selector,fc);
}

void show_icons_by_fade_in_effect(tiles_t* tiles_menu){
    if(tiles_menu->icon->active_obj>0){
        for (int i = 0; i <tiles_menu->icon->active_obj ; i++)
        {
            apply_fade_in_effect_to_obj(tiles_menu->icon->icon[i],tiles_menu->icon->color[i]);
        }    
    }
}

void set_color_for_icon_element(tiles_t* tiles_menu,int icon_index,color_t c){
    tiles_menu->icon->color[icon_index] = c;
}