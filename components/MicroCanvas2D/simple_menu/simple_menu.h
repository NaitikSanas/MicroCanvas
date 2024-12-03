#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uCanvas_api.h"
typedef struct menu_obj
{
    uint16_t enable_index_disp;
    uint16_t is_active;
    int cursor_index;
    uint32_t up_btn;
    uint32_t down_btn;
    uint32_t select_btn;
    uint8_t select_btn_wait_to_release;
    uint16_t active_elements;
    uint16_t title_position_x;
    uint16_t title_position_y;
    uint16_t menu_position_x;
    uint16_t menu_position_y;
    uint16_t span_x;
    uint16_t span_y;
    uint16_t text_offset_x;
    FunctionPointer click_handler;
    FunctionPointer scroll_up_handler;
    FunctionPointer scroll_down_handler;
    uint16_t user_data[32];
    uCanvas_universal_obj_t* content[32];
    uCanvas_universal_obj_t* cursor;
    uCanvas_universal_obj_t* index_disp;
    uCanvas_universal_obj_t* title;
}selection_menu_obj_t;

typedef struct prompt
{
    uCanvas_universal_obj_t* obj[5];
    uint16_t box_h;
    uint16_t box_w;
    uint16_t prompt_position_x;
    uint16_t prompt_position_y;
    uint16_t relative_text_position_x;
    uint16_t relative_text_position_y;
    uint16_t state;
}prompt_t;


void create_menu(selection_menu_obj_t* menu_obj,uCanvas_universal_obj_t* cursor_object);
void menu_task(selection_menu_obj_t* menu_obj);
int menu_get_current_index(selection_menu_obj_t* menu_obj);
void menu_add_gpio_control(selection_menu_obj_t* menu_obj,uint32_t up_control,uint32_t down_control, uint32_t select_btn);
void menu_set_active_elements(selection_menu_obj_t* menu_obj,uint16_t active_elements);
void menu_set_enable_cursor_index_text(selection_menu_obj_t* menu_obj,uint16_t state);
void menu_set_content(selection_menu_obj_t* menu_obj,char* content, uint8_t index);
void menu_set_title(selection_menu_obj_t* menu_obj,char* content, uint16_t x, uint16_t y);
void menu_set_active_state(selection_menu_obj_t* menu_obj,uint16_t state);
uint16_t menu_get_active_state(selection_menu_obj_t* menu_obj);

void create_prompt(prompt_t* pObj);
void set_prompt_content(prompt_t* pObj,char* content_line_1, char* content_line_2);
void hide_prompt(prompt_t* pObj);
void show_prompt(prompt_t* pObj);