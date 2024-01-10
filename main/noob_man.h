#include "uCanvas_api.h"


typedef struct main_char{
    uCanvas_universal_obj_t* head;
    uCanvas_universal_obj_t* eye_left;
    uCanvas_universal_obj_t* eye_right;
    uCanvas_universal_obj_t* mouth;
}noob_man_t;

noob_man_t* New_Noob_Man_Instance (void);
void Set_Noob_Man_Position (noob_man_t*noob_man, int xpos, int ypos);
void character_blink_animation (noob_man_t*noob_man);