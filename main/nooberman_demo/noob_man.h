#include "uCanvas_api.h"

#define GRAVITY -3.8          // Acceleration due to gravity (negative since it acts downward)
#define JUMP_VELOCITY 3.0     // Initial velocity when jumping
#define JUMP_HEIGHT 18.0 // Adjust jump height as needed

typedef struct main_char{
    uCanvas_universal_obj_t* head;
    uCanvas_universal_obj_t* eye_left;
    uCanvas_universal_obj_t* eye_right;
    uCanvas_universal_obj_t* mouth;
}noob_man_t;

noob_man_t* New_Noob_Man_Instance (void);
void Set_Noob_Man_Position (noob_man_t*noob_man, int xpos, int ypos);
void character_blink_animation (noob_man_t*noob_man);
Coordinate2D_t Get_Noob_Man_Position (noob_man_t*noob_man);
void character_hit_animation(noob_man_t*noob_man);

void noober_man_demo_loop();