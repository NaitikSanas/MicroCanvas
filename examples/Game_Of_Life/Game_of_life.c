#include "Game_of_life.h"
#include "uCanvas_api.h"

#define GRID_SIZE_X  28
#define GRID_SIZE_Y  28
#define CELL_SIZE   8
#define OFFSET_X 4
#define OFFSET_Y 90

static uCanvas_universal_obj_t* grid[GRID_SIZE_Y][GRID_SIZE_X]={{0},{0}};
static uCanvas_universal_obj_t* game_stats[5];
static fill_t next_state[GRID_SIZE_Y][GRID_SIZE_X] = {NOFILL};
static fill_t prev_state[GRID_SIZE_Y][GRID_SIZE_X] = {NOFILL};

void randomize_grid() {
    for (int i = 0; i < GRID_SIZE_Y; i++) {
        for (int j = 0; j < GRID_SIZE_X; j++) {
            if (rand() % 2 == 0) {
                grid[i][j]->properties.fill = FILL;
            } else {
                grid[i][j]->properties.fill = NOFILL;
            }
        }
    }
}

int count_live_neighbors(int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; // Skip the cell itself
            int nx = x + i;
            int ny = y + j;
            if (nx >= 0 && nx < GRID_SIZE_X && ny >= 0 && ny < GRID_SIZE_Y) {
                if (grid[ny][nx]->properties.fill == FILL) {
                    count++;
                }
            }
        }
    }
    return count;
}
int alive_cells = 0, dead_cells = 0, stable_cells = 0, oscillating_cells = 0;

void update_grid() {
    alive_cells = 0, dead_cells = 0, stable_cells = 0, oscillating_cells = 0;
    for (int i = 0; i < GRID_SIZE_Y; i++) {
        for (int j = 0; j < GRID_SIZE_X; j++) {
            int live_neighbors = count_live_neighbors(j, i);
            if (grid[i][j]->properties.fill == FILL) {
                next_state[i][j] = (live_neighbors == 2 || live_neighbors == 3) ? FILL : NOFILL;
            } else {
                next_state[i][j] = (live_neighbors == 3) ? FILL : NOFILL;
            }
            // Count statistics
            if (next_state[i][j] == FILL) alive_cells++;
            else dead_cells++;
            if (next_state[i][j] == prev_state[i][j]) stable_cells++;
            else oscillating_cells++;
        }
    }
    // Apply new states
    for (int i = 0; i < GRID_SIZE_Y; i++) {
        for (int j = 0; j < GRID_SIZE_X; j++) {
            prev_state[i][j] = grid[i][j]->properties.fill;  // Save previous state
            grid[i][j]->properties.fill = next_state[i][j];
            if(grid[i][j]->properties.fill == FILL){
                uCanvas_Set_Color(grid[i][j],255,255,0);  
            }
            else {
                uCanvas_Set_Color(grid[i][j],50,50,0); 
            }
        }
    }
    //Update stats on display
    char buf[64]={0};
    sprintf(buf,"Alive      : %d\n", alive_cells);
    uCanvas_Set_Text(game_stats[0],buf);
    sprintf(buf,"Dead       : %d\n", dead_cells);
    uCanvas_Set_Text(game_stats[1],buf);
    sprintf(buf,"Stable     : %d\n", stable_cells);
    uCanvas_Set_Text(game_stats[2],buf);
    sprintf(buf,"Oscillating: %d\n", oscillating_cells);
    uCanvas_Set_Text(game_stats[3],buf);
}

void set_cell(int x, int y, fill_t state) {
    if (x >= 0 && x < GRID_SIZE_X && y >= 0 && y < GRID_SIZE_Y) {
        grid[y][x]->properties.fill = state;
    }
}

void initialize_patterns() {
    // Blinker (oscillator)
    set_cell(5, 5, FILL);
    set_cell(5, 6, FILL);
    set_cell(5, 7, FILL);

    // Glider (moving diagonally)
    set_cell(1, 0, FILL);
    set_cell(2, 1, FILL);
    set_cell(0, 2, FILL);
    set_cell(1, 2, FILL);
    set_cell(2, 2, FILL);

    // Lightweight Spaceship (LWSS)
    set_cell(10, 5, FILL);
    set_cell(10, 6, FILL);
    set_cell(10, 7, FILL);
    set_cell(11, 4, FILL);
    set_cell(12, 4, FILL);
    set_cell(13, 7, FILL);
    set_cell(14, 4, FILL);
    set_cell(14, 6, FILL);
}

void create_grid(){
    for (int i = 0; i < GRID_SIZE_Y; i++){
        for (int j = 0; j < GRID_SIZE_X; j++){               
            grid[i][j] = New_uCanvas_2DRectangle(j*(CELL_SIZE+1)+OFFSET_X,i*(CELL_SIZE+1)+OFFSET_Y,CELL_SIZE,CELL_SIZE);
            if(grid[i][j]){
                grid[i][j]->properties.type = CIRCLE;
                grid[i][j]->r1 = 4;
                uCanvas_Set_Color(grid[i][j],255,255,0);
                grid[i][j]->properties.fill = NOFILL;
            }
            else {
                printf("NULL at idx %d, %d\r\n",i, j);
            }
        } 
    }
}

void create_game_stat_text_area(){
    game_stats[4] = New_uCanvas_2DRectangle(0,0,20*4,240);
    game_stats[4]->properties.fill = FILL;
    uCanvas_Set_Color(game_stats[4],50,50,0);
    for (int i = 0; i < 4; i++){
        game_stats[i] = New_uCanvas_2DTextbox("-",20,20+(i*20));
        game_stats[i]->font_properties.font_type = FONT_16G;
        uCanvas_Set_Color(game_stats[i],255,255,0);
    }
}

void setup(){
    start_uCanvas_engine();
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);

    create_grid();
    randomize_grid(); // Initialize with random state
    initialize_patterns();
    create_game_stat_text_area(); 
}

void start_game_of_life_demo(){   
    setup();
    // Game Loop
    uCanvas_universal_obj_t* prompt = New_uCanvas_2DTextbox("Randomizing Grid...",20,320/2);
    uCanvas_Set_Color(prompt,255,255,0);
    prompt->properties.visiblity = INVISIBLE;
    prompt->font_properties.font_type = FONT_24G;
    printf("Current free heap size: %d bytes\n", esp_get_free_heap_size());
    while (1) {
        update_grid();
        vTaskDelay(pdMS_TO_TICKS(60)); 
        if(oscillating_cells == 0){
            prompt->properties.visiblity = VISIBLE;
            vTaskDelay(pdMS_TO_TICKS(2000)); 
            randomize_grid();
            prompt->properties.visiblity = INVISIBLE;
        }
    }
}