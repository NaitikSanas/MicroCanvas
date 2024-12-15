#include "app.h"
#include "uCanvas_api.h"
#include "uCanvas_display_port.h"

static sprite2D_t fences_sprite;
static sprite2D_t flooring_sprite;
static sprite2D_t terrain_1_sprite;
static sprite2D_t terrain_2_sprite;
static sprite2D_t terrain_3_sprite;
static sprite2D_t block_1_sprite;
static sprite2D_t green_bg_sprite;
typedef enum{
    EMPTY = 0,
    FLOOR = 1,
    TERRAIN = 2,
    BLOCK = 3
}sprite_type;


uint16_t game_map[64][64] = {
    {0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {2,2,2,2,2,2,2,2,0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0,2,2,2,2,2,2,2,2},
    {2,2,2,2,2,2,2,2,0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0,2,2,2,2,2,2,2,2},
};

static uCanvas_universal_obj_t* map_objects[32][10];
void build_game_map() {
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 32; col++) {
            
            int type = game_map[row][col];
            if (type == TERRAIN) {  // Only create objects for visible cells
                map_objects[row][col] = New_uCanvas_2DSprite(&terrain_2_sprite, col * terrain_2_sprite.width, row * terrain_2_sprite.width);
            }else 
            if(type == FLOOR){
                map_objects[row][col] = New_uCanvas_2DSprite(&terrain_3_sprite, col * terrain_3_sprite.width, row * terrain_3_sprite.height+120);
            }
            else if(type == BLOCK){
                map_objects[row][col] = New_uCanvas_2DSprite(&block_1_sprite, col * block_1_sprite.width, row * block_1_sprite.height+120);
            }
            else {
                map_objects[row][col] = 0;
            }
        }
    }
}

uCanvas_universal_obj_t* floor_component[32]={0};
uCanvas_universal_obj_t* blocks_component[32]={0};

void pan(int dir){
    int64_t start = esp_timer_get_time();
    uCanvas_lock_scene();
    for (int j = 0; j < 10; j++)
    {
        
        for (int i = 0; i < 32; i++) {
            if(floor_component[i] != NULL){
                if(dir) floor_component[i]->properties.position.x += 1;
                else floor_component[i]->properties.position.x -= 1;
            }
            if(blocks_component[i] != NULL){
                if(dir) blocks_component[i]->properties.position.x += 1;
                else blocks_component[i]->properties.position.x -= 1;
            }         
        }
       
        vTaskDelay(pdMS_TO_TICKS(9));
    }
     uCanvas_unlock_scene();
    printf("--time elapsed %.2f\r\n",1000000.0/(esp_timer_get_time()-start));
    
}
    // 

void uCanvas_Setup() {
    start_uCanvas_engine();
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    uCanvas_Init_PushButton(45);
    uCanvas_Init_PushButton(47);
    uCanvas_Compose_2DSprite_Obj(&terrain_1_sprite,Terrain_1,TERRAIN_1_WIDTH,TERRAIN_1_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&terrain_2_sprite,Terrain_2,TERRAIN_2_WIDTH,TERRAIN_2_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&terrain_3_sprite,Terrain_3,TERRAIN_3_WIDTH,TERRAIN_3_HEIGHT);
    uCanvas_Compose_2DSprite_Obj(&block_1_sprite,Block,BLOCK_WIDTH,BLOCK_WIDTH);
    uCanvas_Compose_2DSprite_Obj(&green_bg_sprite,Green_Background,GREEN_BG_WIDTH,GREEN_BG_HEIGHT);

    

    for (int i = 0; i <= 240/green_bg_sprite.width; i++)
    {
        New_uCanvas_2DSprite(&green_bg_sprite,i*green_bg_sprite.width,0);
    }
    for (int i = 0; i <= 240/green_bg_sprite.width; i++)
    {
        New_uCanvas_2DSprite(&green_bg_sprite,i*green_bg_sprite.width,64);
    }
    for (int i = 0; i <= 240/green_bg_sprite.width; i++)
    {
        New_uCanvas_2DSprite(&green_bg_sprite,i*green_bg_sprite.width,2*64);
    }
    blocks_component[0] = New_uCanvas_2DSprite(&block_1_sprite,290,90);
    blocks_component[1] = New_uCanvas_2DSprite(&block_1_sprite,80,40);

    // build_game_map(); 
    int prev_x = 0;
    for (int i = 0; i < 32; i++)
    {  
        int xpos = i*(TERRAIN_3_WIDTH);
        int ypos = 170;
        // printf("pos(%d,%d) - %d\r\n",xpos,ypos, (xpos- prev_x));
        floor_component[i] = New_uCanvas_2DSprite(&terrain_3_sprite,xpos,ypos);
        prev_x = xpos;
    } 
}

extern int64_t elapsed_time;
extern int64_t time_to_draw_element;
extern int64_t time_to_draw_frame_buf;
extern int64_t on_screen_draw_time;
   
void uCanvas_App_Main(void) {
    // printf("--------------------------------\r\n");
    // printf("FPS: %lld \r\n", 1000000/elapsed_time );
    // printf("Frame Time: %.2f ms \r\n", (float)elapsed_time/1000.0 );
    // printf("On Screen Draw Time: %.2f ms \r\n", (float)on_screen_draw_time/1000.0 );
    // printf("OSDPS : %.2f\r\n", (float)1000000.0/on_screen_draw_time );
    // printf("Element Draw time: %.2f ms\r\n", (float)time_to_draw_element/1000.0 );
    // printf("Frame Buf Prepare: %f ms\r\n", (float)time_to_draw_frame_buf/1000.0 );
    // printf("FBPS: %f \r\n", (float)1000000.0/time_to_draw_frame_buf);
    // printf("--------------------------------\r\n\r\n");
    if(!uCanvas_Get_PushbuttonState(45)){
        pan(0);
    }
    if(!uCanvas_Get_PushbuttonState(47)){
        pan(1);
    }
    // esp_rom_delay_us(100000);
    uCanvas_Delay(1);
}