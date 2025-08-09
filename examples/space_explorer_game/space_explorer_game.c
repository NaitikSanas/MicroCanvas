#include "space_explorer_game.h"
#include "uCanvas_api.h"
#include "uCanvas_User_IO.h"

#define CANVAS_HEIGHT       300*2
#define CANVAS_WIDTH        512*2

#define MAX_STARS           100 
#define STARS_SCROLLING_RATE 10  
#define MAX_ENEMIES         60
#define COLLISION_THRESHOLD 30  // adjust based on object size

#define ENC_A   39 
#define ENC_B   40 
#define ENC_SW  37

#define PB1 36
#define PB2 35
#define PIEZO_GPIO GPIO_NUM_14

typedef struct spaceship_obj
{
    uCanvas_universal_obj_t* obj;
    uint8_t state;
}spaceship_t;

typedef struct bullets
{
    uCanvas_universal_obj_t* obj[20];
    uint8_t emit_bullets_per_trigger;
    uint8_t active;
}bullets_t;

uCanvas_universal_obj_t* popup;
uCanvas_universal_obj_t* popup_score;
uCanvas_universal_obj_t* lives[4];
uCanvas_universal_obj_t* textbox1;
uCanvas_universal_obj_t* textbox2;
uCanvas_universal_obj_t* title_tb_3;

int current_score = 0;
int last_score = 0;
int cur_lives = 4;
float distance_travelled = 0;
    
spaceship_t enemy_spaceships [MAX_ENEMIES];
uCanvas_universal_obj_t* stars[MAX_STARS];
spaceship_t player_spaceship;
bullets_t player_bulletes_instance;
rotary_encoder_t rotary_encoder_1;
char buf[64] = {0};
#define LIVE_INDICATER_DOT_W 8
#define LIVE_INDICATER_DOT_GAP 25
#define LIVE_INDICATER_DOT_POS_X 20
#define LIVE_INDICATER_DOT_POS_Y 20
key_event_t event;
void create_lives_indicator(){
    for (int i = 0; i < 4; i++)
    {
        lives[i] = New_uCanvas_2DCircle((i*LIVE_INDICATER_DOT_GAP)+LIVE_INDICATER_DOT_POS_X,40/2,10);
        uCanvas_Set_Color(lives[i],255,0,0);
        lives[i]->properties.fill = FILL;
    }
}

void live_indicatior_set(int val){
    switch (val)
    {
    case 0:
        /**
         * thanks to u/dmitrygr for pointing out this stupid little 
         * bug where all indecies were set to zero XD. 
        */
        uCanvas_Set_Color(lives[0],10,0,0);
        uCanvas_Set_Color(lives[1],10,0,0);
        uCanvas_Set_Color(lives[2],10,0,0);
        uCanvas_Set_Color(lives[3],10,0,0);
        break;
    case 1:
        uCanvas_Set_Color(lives[0],255,0,0);
        uCanvas_Set_Color(lives[1],10,0,0);
        uCanvas_Set_Color(lives[2],10,0,0);
        uCanvas_Set_Color(lives[3],10,0,0);
        break;
    case 2:
        uCanvas_Set_Color(lives[0],255,0,0);
        uCanvas_Set_Color(lives[1],255,0,0);
        uCanvas_Set_Color(lives[2],10,0,0);
        uCanvas_Set_Color(lives[3],10,0,0);
        break;
    case 3:
        uCanvas_Set_Color(lives[0],255,0,0);
        uCanvas_Set_Color(lives[1],255,0,0);
        uCanvas_Set_Color(lives[2],255,0,0);
        uCanvas_Set_Color(lives[3],10,0,0);
        break;
    case 4:
        uCanvas_Set_Color(lives[0],255,0,0);
        uCanvas_Set_Color(lives[1],255,0,0);
        uCanvas_Set_Color(lives[2],255,0,0);
        uCanvas_Set_Color(lives[3],255,0,0);
        break;
    default:
        break;
    }
}
void randomize_all_enemiens(void){
    for (size_t i = 0; i < MAX_ENEMIES; i++)
    {
        enemy_spaceships[i].obj->properties.position.x = get_random_number(0,CANVAS_WIDTH);
        enemy_spaceships[i].obj->properties.position.y = get_random_number(-CANVAS_HEIGHT,0);
        enemy_spaceships[i].obj->properties.visiblity = INVISIBLE;
    }
}
void detect_spaceship_collision_with_enemyship(void){
    while (1)
    {
        bool collision_detected = false;
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            int enemy_x = (enemy_spaceships[i].obj->point1.x + enemy_spaceships[i].obj->point2.x + enemy_spaceships[i].obj->point3.x + 3 * enemy_spaceships[i].obj->properties.position.x) / 3;
            int enemy_y = (enemy_spaceships[i].obj->point1.y + enemy_spaceships[i].obj->point2.y + enemy_spaceships[i].obj->point3.y + 3 * enemy_spaceships[i].obj->properties.position.y) / 3;

            int player_x = (player_spaceship.obj->point1.x + player_spaceship.obj->point2.x + player_spaceship.obj->point3.x + 3 * player_spaceship.obj->properties.position.x) / 3;
            int player_y = (player_spaceship.obj->point1.y + player_spaceship.obj->point2.y + player_spaceship.obj->point3.y + 3 * player_spaceship.obj->properties.position.y) / 3;

            int dx = enemy_x - player_x;
            int dy = enemy_y - player_y;
            // 
            if (abs(dx) < COLLISION_THRESHOLD && abs(dy) < COLLISION_THRESHOLD) {
                // Collision!
                collision_detected = true;
                break;
            }
        } 
        if(collision_detected){
            if(cur_lives > 0){
                cur_lives--;
                live_indicatior_set(cur_lives);
                 printf("live : %d\r\n",cur_lives);
            }
            else {
                distance_travelled = 0.0;
                last_score = current_score; 
                current_score = 0;
                cur_lives = 4;
                live_indicatior_set(cur_lives);
                title_tb_3->properties.position.x = 150;
                title_tb_3->properties.visiblity = VISIBLE;
                uCanvas_Set_Text(title_tb_3,"");
                uCanvas_Animate_Text_Reveal(title_tb_3,"Game Over",50);
                uCanvas_Delay(2500);
                while (1)
                {
                    randomize_all_enemiens();
                    uCanvas_Set_Text(title_tb_3,"Press SPACE for Restart");
                    
                    distance_travelled = 0.0;
                    if(event.key_char==' '){
                        title_tb_3->properties.visiblity = INVISIBLE;
                        break;
                    }
                    uCanvas_Delay(1);     
                }
                
            }
            
            for (int blink = 0; blink < 5; blink++) {
                player_spaceship.obj->properties.visiblity = INVISIBLE;
                uCanvas_Set_Color(player_spaceship.obj,255,0,0);
                uCanvas_Delay(70);
                player_spaceship.obj->properties.visiblity = VISIBLE;
                uCanvas_Set_Color(player_spaceship.obj,0,0,255);
                uCanvas_Delay(70);
            }
            randomize_all_enemiens();
        }  
        uCanvas_Delay(1);  
    } 
}
void stars_array_init(){
    for (int i = 0; i < MAX_STARS; i++)
    {
        stars[i] = New_uCanvas_2DCircle(get_random_number(0,CANVAS_WIDTH),get_random_number(0,CANVAS_HEIGHT),get_random_number(0,1));
        uCanvas_Set_Color(stars[i],255,255,get_random_number(0,255));
        // uCanvas_Set_Color(stars[i],255,255,255);
        stars[i]->properties.fill = FILL;
    }   
}

void animate_stars1(void* arg){
    int blink = 0;
    while (1)
    {    
        for (int i = 0; i < MAX_STARS/2; i++)
        {
            if(stars[i]->properties.position.y < CANVAS_HEIGHT){
                stars[i]->properties.position.y += 1;
                blink = get_random_number(0,10);
                if(blink==5){
                    stars[i]->properties.visiblity = INVISIBLE;
                    uCanvas_Delay(1);
                }
                stars[i]->properties.visiblity = VISIBLE;
                
            }
            else {
                int px = get_random_number(0,CANVAS_WIDTH);
                int py = get_random_number(-CANVAS_HEIGHT,0);
                uCanvas_Set_Position(stars[i],px,py);
                uCanvas_Set_Color(stars[i],255,255,get_random_number(100,255));
                stars[i]->properties.visiblity = INVISIBLE;
            }
        }
        distance_travelled += 0.005;
        sprintf(buf, "Travelled:%.1f(ly)  ",distance_travelled);
        uCanvas_Set_Text(textbox2,buf);
        uCanvas_Delay(STARS_SCROLLING_RATE);
        
    }
}

void animate_stars2(void* arg){
    int blink = 0;
    while (1)
    {    
        for (int i = (MAX_STARS/2)-1; i < MAX_STARS; i++)
        {
            if(stars[i]->properties.position.y < CANVAS_HEIGHT){
                stars[i]->properties.position.y += 1;
                blink = get_random_number(0,10);
                if(blink==5){
                    stars[i]->properties.visiblity = INVISIBLE;
                    uCanvas_Delay(1);
                }
                stars[i]->properties.visiblity = VISIBLE;
                
            }
            else {
                int px = get_random_number(0,CANVAS_WIDTH);
                int py = get_random_number(-CANVAS_HEIGHT,0);
                uCanvas_Set_Position(stars[i],px,py);
                uCanvas_Set_Color(stars[i],255,255,get_random_number(100,255));
                stars[i]->properties.visiblity = INVISIBLE;
            }
        }
        uCanvas_Delay(STARS_SCROLLING_RATE*4);
        
    }
}

void animate_enemy_spaceships_1(void* arg){
    while (1)
    {    
        for (int i = 0; i < MAX_ENEMIES/2; i++)
        {
            if(enemy_spaceships[i].obj->properties.position.y < CANVAS_HEIGHT){
                enemy_spaceships[i].obj->properties.position.y += 1;
                enemy_spaceships[i].obj->properties.visiblity = VISIBLE;
            }
            else {
                int px = get_random_number(0,CANVAS_WIDTH);
                int py = get_random_number(-CANVAS_HEIGHT,0);
                uCanvas_Set_Position(enemy_spaceships[i].obj,px,py);
                enemy_spaceships[i].obj->properties.visiblity = INVISIBLE;
            }
        }
        uCanvas_Delay(4);  
    }
}


void animate_enemy_spaceships_2(void* arg){
    while (1)
    {    
        for (int i = (MAX_ENEMIES/2)-1; i < MAX_ENEMIES/2; i++)
        {
            if(enemy_spaceships[i].obj->properties.position.y < CANVAS_HEIGHT){
                enemy_spaceships[i].obj->properties.position.y += 1;
                enemy_spaceships[i].obj->properties.visiblity = VISIBLE;
            }
            else {
                int px = get_random_number(0,CANVAS_WIDTH);
                int py = get_random_number(-CANVAS_HEIGHT,0);
                uCanvas_Set_Position(enemy_spaceships[i].obj,px,py);
                enemy_spaceships[i].obj->properties.visiblity = INVISIBLE;
            }
        }
        
        uCanvas_Delay(20);  
    }
}

void bullets_init(bullets_t* obj, uint8_t emit_bullets_per_trigger){
    obj->emit_bullets_per_trigger = emit_bullets_per_trigger;
    obj->active = 0;
    for (int i = 0; i < emit_bullets_per_trigger; i++)
    {
        obj->obj[i] = New_uCanvas_2DRectangle(0,0,5,3);
        obj->obj[i]->properties.visiblity = INVISIBLE;
        obj->obj[i]->properties.fill = FILL;
        uCanvas_Set_Color(obj->obj[i],255,0,0);
    }    
}

void trigger_weapon(spaceship_t* spaceship,bullets_t* bullets){
    if(bullets->active == 0){
        for (int i = 0; i < bullets->emit_bullets_per_trigger; i++)
        {    
            bullets->obj[i]->properties.position.x = spaceship->obj->properties.position.x+20;
            bullets->obj[i]->properties.position.y = spaceship->obj->properties.position.y;
            bullets->obj[i]->properties.position.y += 8 * i;
            bullets->obj[i]->properties.visiblity = VISIBLE;
            bullets->active += 1;
            uCanvas_Delay(1);
        }
    }
}

void bullets_animation(bullets_t* bullets){
    while (1)
    {
        for (int i = 0; i <bullets->emit_bullets_per_trigger; i++)
        {  
            if(bullets->obj[i]->properties.visiblity == VISIBLE){
                if(bullets->obj[i]->properties.position.y > 0) bullets->obj[i]->properties.position.y -= 1;
                else {
                    bullets->obj[i]->properties.visiblity = INVISIBLE;
                    bullets->active--;
                }
            }

            if (bullets->obj[i]->properties.visiblity == VISIBLE) {
                int bullet_x = bullets->obj[i]->properties.position.x;
                int bullet_y = bullets->obj[i]->properties.position.y;
    
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (enemy_spaceships[j].obj->properties.visiblity == VISIBLE) {
                        // Calculate approximate center of enemy triangle
                        int ex = (enemy_spaceships[j].obj->point1.x + enemy_spaceships[j].obj->point2.x + enemy_spaceships[j].obj->point3.x) / 3 
                                 + enemy_spaceships[j].obj->properties.position.x;
                        int ey = (enemy_spaceships[j].obj->point1.y + enemy_spaceships[j].obj->point2.y + enemy_spaceships[j].obj->point3.y) / 3 
                                 + enemy_spaceships[j].obj->properties.position.y;
    
                        int dx = abs(bullet_x - ex);
                        int dy = abs(bullet_y - ey);
    
                        if (dx < COLLISION_THRESHOLD && dy < COLLISION_THRESHOLD) {
                            // Enemy got hit
                            // printf("currnet score %d\r\n",current_score);
                            current_score++;

                            sprintf(buf, "Score:%d",current_score);
                            uCanvas_Set_Text(textbox1,buf);
                            
                            enemy_spaceships[j].obj->properties.position.x += SHIP_ENEMY_WIDTH/2;
                            enemy_spaceships[j].obj->properties.position.y += SHIP_ENEMY_HEIGHT/2;

                            popup->properties.position.x = enemy_spaceships[j].obj->properties.position.x + 15;
                            popup->properties.position.y = enemy_spaceships[j].obj->properties.position.y + 5;

                            popup_score->properties.position.x = enemy_spaceships[j].obj->properties.position.x + 25;
                            popup_score->properties.position.y = enemy_spaceships[j].obj->properties.position.y + 20;

                            enemy_spaceships[j].obj->r1 = 2;
                            enemy_spaceships[j].obj->properties.type = CIRCLE;
                            char b[4]={0};
                            sprintf(b,"%d",current_score);
                            uCanvas_Set_Text(popup_score,b);
                            popup->properties.visiblity = VISIBLE;
                            popup_score->properties.visiblity = VISIBLE;
                            uCanvas_Set_Color(enemy_spaceships[j].obj,255,125,0);
                            for (int i = 0; i < SHIP_ENEMY_WIDTH/2; i++)
                            {
                                enemy_spaceships[j].obj->r1 += 1;
                                uCanvas_Delay(5);
                            }
                            enemy_spaceships[j].obj->properties.type = SPRITE2D;
                            uCanvas_Set_Color(enemy_spaceships[j].obj,get_random_number(150,255),get_random_number(150,255),get_random_number(0,255));
                            enemy_spaceships[j].obj->properties.visiblity = INVISIBLE;
                            
                            // Randomize offscreen spawn
                            int px = get_random_number(0, CANVAS_WIDTH); 
                            int py = get_random_number(-CANVAS_HEIGHT, 0);
                            uCanvas_Set_Position(enemy_spaceships[j].obj, px, py);
    
                            // Hide bullet
                            popup->properties.visiblity = INVISIBLE;
                            popup_score->properties.visiblity = INVISIBLE;
                            bullets->obj[i]->properties.visiblity = INVISIBLE;
                            bullets->active--;
                            break; // move to next bullet
                        }
                    }
                }
            }
        }
        // esp_rom_delay_us(100);
        uCanvas_Delay(pdMS_TO_TICKS(1));
    }
}

void create_spaceship(spaceship_t* obj){
    sprite2D_t spaceship_sprite_obj;
    uint32_t* ship_ARGB8888 = (uint32_t*)malloc(SHIP_WIDTH * SHIP_HEIGHT * sizeof(uint32_t));
    if (ship_ARGB8888 == NULL) {
        printf("Failed to allocate memory for spaceship sprite buffer\n");
        return;
    }
    uCanvas_Convert_RGB565A_to_ARGB8888(
        (uint16_t*)ship, 
        ship_ARGB8888, 
        SHIP_WIDTH, 
        SHIP_HEIGHT
    );

    uCanvas_Compose_2DSprite_Obj(&spaceship_sprite_obj,ship_ARGB8888,40,40,COLOR_ARGB8888);
    uCanvas_Sprite_Adjust_Contrast(&spaceship_sprite_obj,400);
    obj->obj = New_uCanvas_2DSprite(&spaceship_sprite_obj,0,0);
    obj->obj->point1 = (Coordinate2D_t){10,0};
    obj->obj->point2 = (Coordinate2D_t){0,10};
    obj->obj->point3 = (Coordinate2D_t){20,10};
    // obj->obj = New_uCanvas_2DTriangle((Coordinate2D_t){10,0},(Coordinate2D_t){0,10},(Coordinate2D_t){20,10});
    uCanvas_Set_Color(obj->obj,0,150,255);
    obj->obj->properties.fill = FILL;
    obj->state = 0;
    uCanvas_Set_Position(obj->obj,CANVAS_WIDTH/2,CANVAS_HEIGHT-SHIP_HEIGHT);
}




void spawn_enemines(){
    for (int i = 0; i < MAX_ENEMIES; i++)
    {  
        int px = get_random_number(0,CANVAS_WIDTH); 
        int py = get_random_number(-CANVAS_HEIGHT,0);
        sprite2D_t enemy_spaceship_sprite_obj;
        uint32_t* enemy_spaceship_rgba8888 = (uint32_t*)malloc(SHIP_ENEMY_WIDTH * SHIP_ENEMY_HEIGHT * sizeof(uint32_t));
        if (enemy_spaceship_rgba8888 == NULL) {
            printf("Failed to allocate memory for enemy spaceship sprite buffer\n");
            return;
        }

        uCanvas_Convert_RGB565A_to_ARGB8888(
            (uint16_t*)ship_enemy, 
            enemy_spaceship_rgba8888, 
            SHIP_ENEMY_WIDTH, 
            SHIP_ENEMY_HEIGHT
        );
        

        // uCanvas_Compose_2DSprite_Obj(&enemy_spaceship_sprite_obj,ship_enemy,SHIP_ENEMY_WIDTH,SHIP_ENEMY_HEIGHT,COLOR_RGBA565);
        uCanvas_Compose_2DSprite_Obj(&enemy_spaceship_sprite_obj,enemy_spaceship_rgba8888,SHIP_ENEMY_WIDTH,SHIP_ENEMY_HEIGHT,COLOR_ARGB8888);

        // uCanvas_Sprite_Adjust_Contrast(&enemy_spaceship_sprite_obj,200);
        

        enemy_spaceships[i].obj = New_uCanvas_2DSprite(&enemy_spaceship_sprite_obj,0,0);
        enemy_spaceships[i].obj->point1 = (Coordinate2D_t){10,0};
        enemy_spaceships[i].obj->point2 = (Coordinate2D_t){0,10};
        enemy_spaceships[i].obj->point3 = (Coordinate2D_t){20,10};
        // enemy_spaceships[i].obj = New_uCanvas_2DTriangle((Coordinate2D_t){0,0},(Coordinate2D_t){10,10},(Coordinate2D_t){20,0});
        enemy_spaceships[i].obj->properties.position.x = px;
        enemy_spaceships[i].obj->properties.position.y = py;
        uCanvas_Set_Color(enemy_spaceships[i].obj,get_random_number(150,255),get_random_number(150,255),get_random_number(0,255));
        // uCanvas_Set_Color(enemy_spaceships[i].obj,255,255,255);
        enemy_spaceships[i].obj->properties.fill = FILL;
    }   
}



void controller_task(void){
    while (1)
    {    
        switch (event.key_char)
        {
            case 'd': 
                if(event.state == KEY_STATE_PRESSED){
                    if(player_spaceship.obj->properties.position.x < CANVAS_WIDTH) {
                        player_spaceship.obj->properties.position.x  += 1;
                    }
                }
                break;

            case 'a':
                if(event.state == KEY_STATE_PRESSED){
                    if(player_spaceship.obj->properties.position.x > 0){
                        player_spaceship.obj->properties.position.x -= 1;
                    }
                }
                break;
            default : break;
        }

        switch (event.key_char)
        {
            case ' ':
                if(event.state == KEY_STATE_PRESSED){
                    trigger_weapon(&player_spaceship, &player_bulletes_instance);
                }
                break;
            default : break;
        }
        
        // uCanvas_rotary_encoder_read(&rotary_encoder_1);
        // if(!rotary_encoder_1.sw_state){
        //     trigger_weapon(&player_spaceship, &player_bulletes_instance);

        // }

        // if(uCanvas_Get_PushbuttonState(PB1) || uCanvas_rotary_encoder_get_state(&rotary_encoder_1) == ENCODER_CW){
        //     if(player_spaceship.obj->properties.position.x < CANVAS_WIDTH) 
        //             player_spaceship.obj->properties.position.x += 1;
        // }

        // if(uCanvas_Get_PushbuttonState(PB2) || uCanvas_rotary_encoder_get_state(&rotary_encoder_1) == ENCODER_CCW){
        //     if(player_spaceship.obj->properties.position.x > 0) 
        //             player_spaceship.obj->properties.position.x -= 1;
        // }

        uCanvas_Delay(1);
    }
}

void show_start_screen(){
    uCanvas_universal_obj_t* title_tb_1 = New_uCanvas_2DTextbox("Space",CANVAS_WIDTH/4 ,0);
    uCanvas_universal_obj_t* title_tb_2 = New_uCanvas_2DTextbox("Explorer",CANVAS_WIDTH/5 +100,0);
    title_tb_3 = New_uCanvas_2DTextbox("",CANVAS_WIDTH/4-100,0);
    uCanvas_Set_Color(title_tb_1,255,255,0);
    uCanvas_Set_Color(title_tb_2,255,255,0);
    uCanvas_Set_Color(title_tb_3,255,255,255);

    title_tb_1->font_properties.font_type = SFONT_SIXTYFOUR_32;
    title_tb_2->font_properties.font_type = SFONT_SIXTYFOUR_32;
    title_tb_3->font_properties.font_type = SFONT_SIXTYFOUR_32;
    title_tb_3->properties.visiblity = INVISIBLE;
    title_tb_1->properties.position.y = -32;
    title_tb_3->properties.position.y = +70;
    printf("anim start\r\n");
    for (int i = 0; i < (CANVAS_HEIGHT/2); i++)
    {
        title_tb_1->properties.position.y += 1;
        title_tb_2->properties.position.y += 1;
        // title_tb_3->properties.position.y += 1;
        uCanvas_Delay(10);
    }
    title_tb_3->properties.visiblity = VISIBLE;
    uCanvas_Animate_Text_Reveal(title_tb_3,"Press SPACE to Play",50);
    printf("anim end\r\n");
    printf("pos %d\r\n",title_tb_3->properties.position.y );
    
    while (1)
    {
        
        if(event.key_char == ' '){
            title_tb_1->properties.visiblity = INVISIBLE;
            title_tb_2->properties.visiblity = INVISIBLE;
            title_tb_3->properties.visiblity = INVISIBLE;
            break;
        }
        uCanvas_Delay(1);
    }
}
void create_hud(){
    uCanvas_universal_obj_t* bg = New_uCanvas_2DRectangle(0,0,40,CANVAS_WIDTH);
    uCanvas_Set_Color(bg,0,0,100);
    bg->properties.fill = FILL;

    textbox1 = New_uCanvas_2DTextbox(" ",CANVAS_WIDTH-220,  5);
    uCanvas_Set_Color(textbox1,255,255,0);
    textbox1->font_properties.font_type = SFONT_BITCOUNT_32;

    textbox2 = New_uCanvas_2DTextbox("",CANVAS_WIDTH/2-100, 5);
    uCanvas_Set_Color(textbox2,255,255,0);
    textbox2->font_properties.font_type = SFONT_24;

    popup = New_uCanvas_2DTextbox("+1",0,0);
    popup_score = New_uCanvas_2DTextbox("+1",0,0);
    uCanvas_Set_Color(popup,255,255,255);
    uCanvas_Set_Color(popup_score,0,255,0);
    popup->font_properties.font_type = SFONT_16;
    popup_score->font_properties.font_type = SFONT_16;
}
#include "uCanvas_Draw.h"
#include "uCanvas2D_EK79007Port.h"
#include "uCanvas2D_ST7789_Port.h"
#include "uCanvas2D_Display_Setup.h"
#include "uCanvasRenderEngine.h"
#include "uCanvas2D_Acceleration.h"

 uCanvas_Scene_t* scene = NULL;
uCanvas2D_Instance_t uCanvas_Instance_1;
uCanvas2D_Instance_t uCanvas_Instance_2;
uCanvas2D_Instance_t* game_window = NULL;
void fps_monitor(void){
    uCanvas_universal_obj_t* fps_counter = New_uCanvas_2DTextbox("",CANVAS_WIDTH-150,CANVAS_HEIGHT-32);
    fps_counter->font_properties.font_type = SFONT_24;
    uCanvas_Set_Color(fps_counter,255,255,255);

    // uCanvas_set_active_scene(uCanvas_Instance_2.active_scene);
    // uCanvas_universal_obj_t* fps_counter2 = New_uCanvas_2DTextbox("",128-60,128-16);
    // fps_counter2->font_properties.font_type = SFONT_12;
    // uCanvas_Set_Color(fps_counter2,255,255,0);


    char buf[32]={0};
    char buf2[32]={0};
    while (1)
    {
        sprintf(buf,"FPS:%lld",uCanvas_Get_FPS(&uCanvas_Instance_1));
        uCanvas_Set_Text(fps_counter,buf);

        // sprintf(buf2,"FPS:%lld",uCanvas_Get_FPS(&uCanvas_Instance_2));
        // uCanvas_Set_Text(fps_counter2,buf2);

        vTaskDelay(pdMS_TO_TICKS(200));
    } 
}
#include "uCanvas_HID.h"
static uCanvas_Input_HID_Device_t KeyBoardDevice;

static inline void MyKeyboardListener(uCanvasInputDeviceType_t device_type, key_event_t* key_event, void* user_data) {
    // printf("%c",(char)event_code);

        // if (!!event_code) {
        //     putchar(event_code);
        //     if ('\r' == event_code) {
        //         putchar('\n');
        //     }
        //     fflush(stdout);
        // }
    event.key_char = key_event->key_char;
    event.state = key_event->state;
    event.key_code = key_event->key_code;
    printf("key_event_callback\r\n");
    if (event.state == KEY_STATE_PRESSED) {
        printf("Key pressed: %c\n", event.key_char);

    } else if (event.state == KEY_STATE_RELEASED) {
        printf("Key released: %c\n", event.key_char);
    }
    return;
}



void Run_Space_Explorer_Game() {
    // uCanvas_Load_FontX();
    Intialize_PPA(); //Enable 2D Pixel Processing Accelerator
    KeyBoardDevice.EventListenerCallback = MyKeyboardListener;
    uCanvas_Get_HID_Device(&KeyBoardDevice);
    KeyBoardDevice.uCanvas_Input_HID_Device_Init();
    
    
    uCanvas2D_Display_Panel_t* panel = uCanvas2D_Get_Panel_Driver_EK79007();
    panel->init(1);
    // panel->set_backlight(3000);
    

    // uCanvas2D_Display_Panel_t* panel_st7789 = uCanvas2D_Get_Panel_Driver_ST7789();
    // panel_st7789->init(1);
    // // panel_st7789->set_backlight(3000);

    uCanvas_Scene_t*  scene = New_uCanvas_Scene();

    // uCanvas_set_active_scene(uCanvas_Instance_2.active_scene);
    // // Draws Everything within the Set Active scene
    // uCanvas_universal_obj_t* rect = New_uCanvas_2DRectangle(0,0,128,128);
    // uCanvas_Set_Color(rect,255,255,0);
    // uCanvas_universal_obj_t* text = New_uCanvas_2DTextbox("Window", 5,5);
    // text->font_properties.font_type = SFONT_12;
    // uCanvas_Set_Color(text,255,255,0);
    // uCanvas_universal_obj_t* line = New_uCanvas_2DLine(0,20,128,20);
    // uCanvas_Set_Color(line,255,255,0);
    
   
    uCanvas_Instance_1.scale_output = false;
    uCanvas_Instance_1.scale_x = 2.0;
    uCanvas_Instance_1.scale_y = 2.0;
    if(uCanvas_Attach_RenderBuffer(&uCanvas_Instance_1,CANVAS_WIDTH,CANVAS_HEIGHT)){
        uCanvas_Set_Panel_RefreshDelay(&uCanvas_Instance_1,2);
        uCanvas_Attach_Panel(&uCanvas_Instance_1,panel);
        uCanvas_Set_ViewPort_Position(&uCanvas_Instance_1,0,0);
        uCanvas_Attach_Scene(&uCanvas_Instance_1,scene);
        uCanvas_Attach_Renderer(&uCanvas_Instance_1,1);
    }
    // uCanvas2D_GetPanel_Driver_ST7789(&panel_st7789);
    // panel_st7789.init(1);
    // panel_st7789.set_backlight(3000);

    
    // uCanvas_Instance_1 = New_uCanvas_Instance(scene, uCanvas2D_Get_Panel_Driver_EK79007(),NULL,CANVAS_WIDTH,CANVAS_HEIGHT,100,100);
    // uCanvas2D_Instance_t* uCanvas_Instance_2 = New_uCanvas_Instance(hud_scene, uCanvas2D_Get_Panel_Driver_ST7789(),NULL,CANVAS_WIDTH,CANVAS_HEIGHT,0,0);
    // uCanvas_set_active_scene(uCanvas_Instance_2->active_scene);
    // uCanvas_universal_obj_t* rect = New_uCanvas_2DRectangle(0,0,200,200);
    // uCanvas_Set_Color(rect,255,255,0);

    uCanvas_set_active_scene(uCanvas_Instance_1.active_scene);
    // uCanvas_Initialize_IMU_Device(42,41);
    // uCanvas_IMU_Set_Tilt_Detection_Parameters(7,2);
    uCanvas_Init_PushButton(PB1);
    uCanvas_Init_PushButton(PB2);
    
    uCanvas_rotary_encoder_init(&rotary_encoder_1,ENC_A,ENC_B,ENC_SW);
    uCanvas_set_active_scene(uCanvas_Instance_1.active_scene);

    printf("---setup\r\n");
   

    stars_array_init();
    bullets_init(&player_bulletes_instance,10);
    create_spaceship(&player_spaceship);  
    uCanvas_Add_Task(animate_stars1,NULL,0);
    uCanvas_Add_Task(animate_stars2,NULL,0);
    uCanvas_Add_Task((void (*))fps_monitor,NULL,0);
    printf("---done\r\n");
    
    show_start_screen();
    spawn_enemines();

    uCanvas_Add_Task(animate_enemy_spaceships_1,NULL,1);
    uCanvas_Add_Task(animate_enemy_spaceships_2,NULL,1);
    uCanvas_Add_Task((void (*))bullets_animation,&player_bulletes_instance,0);
   
    uCanvas_Add_Task((void (*))controller_task,NULL,0);
    uCanvas_Add_Task((void (*))detect_spaceship_collision_with_enemyship,NULL,0);
    
    
    create_hud();
    create_lives_indicator();
    live_indicatior_set(4);
    printf("--here\r\n");
    // while(1){

    //     printf("fps: %lld\r\n",uCanvas_Get_FPS(uCanvas_Instance_1));
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);   
    // }
    // IMU_Monitor();
}

void IMU_Monitor(void) {
    
    while (1)
    {
        //Control Circle through IMU
        tilt_dir_t a = uCanvas_Get_IMU_2D_Tilt();
        switch (a)
        {
            case TILT_LEFT:  
                if(player_spaceship.obj->properties.position.x < CANVAS_WIDTH) {
                    player_spaceship.obj->properties.position.x  += 1;
                }
                break;

            case TILT_RIGHT:
                if(player_spaceship.obj->properties.position.x > 0){
                    player_spaceship.obj->properties.position.x -= 1;
                }
                break;

            case TILT_UP:
                
                
            default:
                break;
        
        }
        uCanvas_Delay(4);
    }  
    
}