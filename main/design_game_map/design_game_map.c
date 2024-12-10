#include "app.h"
#include "uCanvas_api.h"
#include "uCanvas_display_port.h"

// Map layout: 1 = Wall, 0 = Floor
int rpg_map[20][20] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

// Sprite buffer for walls (1) and floors (0)
static uint8_t wall_sprite_buf[10 * 10] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static uint8_t floor_sprite_buf[10 * 10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
    0, 0, 0, 0, 1, 0, 0, 1, 0, 0,
    0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
static uint8_t floor_sprite_buf2[10 * 10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
sprite2D_t wall_sprite_obj;
sprite2D_t floor_sprite_obj;
sprite2D_t floor_sprite_obj2;
uCanvas_universal_obj_t* map_objects[20][20] = {NULL};

// Build the map based on rpg_map
void build_rpg_map() {
    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 20; col++) {
            int type = rpg_map[row][col];
            if (type == 1) {  // Only create objects for visible cells
                map_objects[row][col] = New_uCanvas_2DSprite(wall_sprite_obj, col * 10, row * 10);
            }
            else {
                if(get_random_number(1,2)==1) map_objects[row][col] = New_uCanvas_2DSprite(floor_sprite_obj, col * 10, row * 10);
                else  map_objects[row][col] = New_uCanvas_2DSprite(floor_sprite_obj2, col * 10, row * 10);
            }
        }
    }
}

typedef enum {
    PAN_DIR_LEFT = 0,
    PAN_DIR_RIGHT,
    PAN_DIR_UP,
    PAN_DIR_DOWN,
}pan_direction_t;
    
void pan_through_wall_map(
    uCanvas_universal_obj_t* wall[][20],  // 2D array of wall objects (e.g., 20x20 maze)
    uint16_t rows,                       // Number of rows in the wall array
    uint16_t cols,                       // Number of columns in the wall array
    uint16_t steps,                      // Number of panning steps
    pan_direction_t direction,           // Panning direction
    uint16_t delay_us                    // Delay between steps
) {
    for (int p = 0; p < steps; p++) {
        // Iterate through all objects in the 2D wall array
        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < cols; col++) {
                // Check if the current cell contains an object
                if (wall[row][col] != NULL) {
                    switch (direction) {
                        case PAN_DIR_LEFT:
                            wall[row][col]->properties.position.x++;   
                            break;
                        case PAN_DIR_RIGHT:
                            wall[row][col]->properties.position.x--;   
                            break;
                        case PAN_DIR_UP:
                            wall[row][col]->properties.position.y++;   
                            break;
                        case PAN_DIR_DOWN:
                            wall[row][col]->properties.position.y--;   
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        // Add delay between steps
        ets_delay_us(delay_us);
    }
}

// Setup the map and render
void uCanvas_Setup1() {
    start_uCanvas_engine();
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    // Create sprite for walls and floors
    uCanvas_Compose_2DSprite_Obj(&wall_sprite_obj, wall_sprite_buf, 10, 10);
    uCanvas_Compose_2DSprite_Obj(&floor_sprite_obj, floor_sprite_buf, 10, 10);
    uCanvas_Compose_2DSprite_Obj(&floor_sprite_obj2, floor_sprite_buf2, 10, 10);

    // Build the map
    build_rpg_map();

}
void uCanvas_App_Main1(void) {
    pan_direction_t dir = 0;
    uint16_t steps = 10;
    uint32_t slide_animation_delay = 1000000;
    uCanvas_Init_PushButton(47);
    uCanvas_Init_PushButton(48);
    uCanvas_Init_PushButton(45);
    while (1) {
        if(!uCanvas_Get_PushbuttonState_WTR(45)){
            dir = !dir;
            if(dir)printf("RIGHT-LEFT\r\n");
            else printf("UP-DOWN\r\n");
        }

        if(!uCanvas_Get_PushbuttonState(47)){
            if(dir == 1){
                printf("PAN_DIR_RIGHT\r\n");
                pan_through_wall_map(map_objects,20,20,steps,PAN_DIR_RIGHT,slide_animation_delay);
            }
            else {
                printf("PAN_DIR_DOWN\r\n");
                pan_through_wall_map(map_objects,20,20,steps,PAN_DIR_DOWN,slide_animation_delay);
            }
            printf("anim_done\r\n");
        }
        if(!uCanvas_Get_PushbuttonState(48)){
            if(dir == 1){
                printf("PAN_DIR_LEFT\r\n");
                pan_through_wall_map(map_objects,20,20,steps,PAN_DIR_LEFT,slide_animation_delay);
            }
            else {
                printf("PAN_DIR_UP\r\n");
                pan_through_wall_map(map_objects,20,20,steps,PAN_DIR_UP ,slide_animation_delay);
            }
            printf("anim_done\r\n");
        }
        
        uCanvas_Delay(1);
    }
}