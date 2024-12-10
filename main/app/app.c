#include "app.h"
#include "uCanvas_api.h"
#include "uCanvas_display_port.h"

// Map layout: 1 = Wall, 0 = Floor, 2 = Door
int rpg_map[20][20] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

uint8_t brick_sprite[9*9] = {
    1,1,1,1,1,1,1,1,1,
    1,0,3,0,0,0,3,0,1,
    1,2,2,2,2,2,2,2,1,
    1,0,0,0,3,0,0,0,1,
    1,2,2,2,2,2,2,2,1,
    1,0,0,3,0,0,3,0,1,
    1,2,2,2,2,2,2,2,1,
    1,0,3,0,0,3,0,0,1,
    1,1,1,1,1,1,1,1,1,
};

#define CHARACTER_SKELETON_HEIGHT 64
#define CHARACTER_SKELETON_WIDTH 64


static uint8_t character_sprite_buf[9 * 9] = {
    3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 3, 3, 0, 3, 3, 0, 3,
    3, 0, 3, 3, 0, 3, 3, 0, 3,
    3, 0, 3, 3, 0, 3, 3, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 3, 3, 3, 3, 3, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3
};

// 2D array for map objects
uCanvas_universal_obj_t* map_objects[20][20] = {NULL};
uCanvas_universal_obj_t* character;  // Character object

// Character position
int char_x = 1, char_y = 1;
int camera_x = 0, camera_y = 0;

// Sprites
sprite2D_t wall_sprite, floor_sprite, door_sprite, character_sprite;

// Build the RPG map based on the layout
void build_rpg_map() {
    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 20; col++) {
            switch (rpg_map[row][col]) {
                case 1:  // Wall
                    map_objects[row][col] = New_uCanvas_2DSprite(wall_sprite, col * 10, row * 10);
                    break;

                // case 0:  // Floor
                //     map_objects[row][col] = New_uCanvas_2DSprite(floor_sprite, col * 10, row * 10);
                //     break;
                // case 2:  // Door
                //     map_objects[row][col] = New_uCanvas_2DSprite(door_sprite, col * 10, row * 10);
                    break;
                default:
                    map_objects[row][col] = NULL;
                    break;
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
// Move the character if the new position is valid
void move_character(int new_x, int new_y) {
    if (new_x < 0 || new_y < 0 || new_x >= 20 || new_y >= 20) {
        printf("Out of bounds\r\n");
        return;  // Out of bounds
    }
    if (rpg_map[new_y][new_x] == 1) {
        printf("Collision\r\n");
        return;  // Collision with wall
    }
    
    // if(character->properties.position.x > 120) {
    //     pan_through_wall_map(map_objects,20,20,40,PAN_DIR_RIGHT,10000);
    //      character->properties.position.x -= 40;
    // }
    // if(character->properties.position.y > 60) {
    //     pan_through_wall_map(map_objects,20,20,40,PAN_DIR_DOWN,10000);
    //     character->properties.position.y -= 40;
    // }

    char_x = new_x;
    char_y = new_y;
    character->properties.position.x = char_x * 10;
    character->properties.position.y = char_y * 10;
    
}

// Handle input to move the character
void handle_input(char input) {
    switch (input) {
        case 'w': move_character(char_x, char_y - 1); break;  // Up
        case 's': move_character(char_x, char_y + 1); break;  // Down
        case 'a': move_character(char_x - 1, char_y); break;  // Left
        case 'd': move_character(char_x + 1, char_y); break;  // Right
    }
}
uint8_t dir = 0;
char get_input(){
    if(!uCanvas_Get_PushbuttonState_WTR(45)){
        dir = !dir;
    }
    if(!uCanvas_Get_PushbuttonState_WTR(47)){
        if(dir) return 'w';
        else return 'a';
    }
    if(!uCanvas_Get_PushbuttonState_WTR(48)){
        if(dir) return 's';
        else return 'd';
    }
    return 'x';
}
// Setup and render the RPG map and character
void uCanvas_Setup() {
    start_uCanvas_engine();
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    uCanvas_Init_PushButton(47);
    uCanvas_Init_PushButton(48);
    uCanvas_Init_PushButton(45);
    // Create sprites
    uCanvas_Compose_2DSprite_Obj(&wall_sprite, brick_sprite, 10, 10);
    // uCanvas_Compose_2DSprite_Obj(&floor_sprite, floor_sprite_buf, 10, 10);
    // uCanvas_Compose_2DSprite_Obj(&door_sprite, door_sprite_buf, 10, 10);
    uCanvas_Compose_2DSprite_Obj(&character_sprite, character_sprite_buf, 9, 9);

    // Build the map
    build_rpg_map();

    // Create the character
    character = New_uCanvas_2DSprite(character_sprite, char_x * 9, char_y * 9);

    // Main game loop
    while (1) {
        char input = get_input();  // Get user input
        handle_input(input);      // Move the character
        uCanvas_Delay(1);        // Delay for smoother rendering
    }
}

void uCanvas_App_Main(void) {
}