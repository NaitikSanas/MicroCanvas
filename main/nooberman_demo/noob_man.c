#include "noob_man.h"
#define PI 3.14159265358979323846
#include "uCanvas_User_IO.h"
#include "math.h"
#define PB0_PIN 5

typedef struct spikes
{
    uCanvas_universal_obj_t* spikes[4];
    int distance_x;
}spikes_t;

typedef enum{
    IDLE = 0,
    GAME_STARTED,
    GAME_STOPPED
}game_steate_t;

typedef enum{
    PLAYER_IDLE=0,
    PLAYER_ALIVE,
    PLAYER_DEAD
}player_state_t;

player_state_t player_state = IDLE;
game_steate_t game_state = IDLE;


extern bool collided;

Coordinate2D_t get_xy_cordinates(int angle, int rx, int ry, int xoffset, int yoffset){
    Coordinate2D_t coordinates;
    double theta = 2.0 * PI * angle / 360;            
    uint8_t x = rx * cos(theta) + xoffset;
    uint8_t y = ry * sin(theta) + yoffset;
    coordinates.x = x;
    coordinates.y = y;
    return coordinates;
}

noob_man_t* New_Noob_Man_Instance(void){
    noob_man_t* noob_man = (noob_man_t*)malloc(sizeof(noob_man_t));
    
    noob_man->head = New_uCanvas_2DCircle(0,0,10);
    noob_man->head->properties.fill = FILL;

    noob_man->eye_left = New_uCanvas_2DCircle(0,0,2);
    noob_man->eye_left->properties.color.monochrome_pixel = 0;
    noob_man->eye_left->properties.fill = FILL;
    noob_man->eye_left->height = 1;
    noob_man->eye_left->width = 2;

    noob_man->eye_right = New_uCanvas_2DCircle(0,0,2); 
    noob_man->eye_right->properties.color.monochrome_pixel = 0;
    noob_man->eye_right->properties.fill = FILL;
    noob_man->eye_right->height = 1;
    noob_man->eye_right->width = 2;

    noob_man->mouth = New_uCanvas_2DRectangle(0,0,0,0);
    noob_man->mouth->properties.color.monochrome_pixel = 0;
    noob_man->mouth->properties.fill = FILL;
    noob_man->mouth->height = 3;
    noob_man->mouth->width  = 8;
    return noob_man;
}

void Set_Noob_Man_Position(noob_man_t*noob_man, int xpos, int ypos){
    noob_man->head->properties.position.x      = xpos;
    noob_man->head->properties.position.y      = ypos;
    //Eyes and Mouth moves relative to positions derived from head position
    noob_man->eye_left->properties.position.x  = xpos - 5;
    noob_man->eye_left->properties.position.y  = ypos - 2;
    noob_man->eye_right->properties.position.x = xpos + 5;
    noob_man->eye_right->properties.position.y = ypos - 2;
    noob_man->mouth->properties.position.x     = xpos - 4;
    noob_man->mouth->properties.position.y     = ypos + 3;
}




Coordinate2D_t Get_Noob_Man_Position (noob_man_t*noob_man){
    Coordinate2D_t pos;
    pos.x = noob_man->head->properties.position.x;
    pos.y = noob_man->head->properties.position.y;
    return pos;
}


void character_hit_animation(noob_man_t*noob_man){
    noob_man->eye_left->properties.type = RECTANGLE;
    noob_man->eye_right->properties.type = RECTANGLE; 
    noob_man->eye_left->properties.color.monochrome_pixel = 1;
    noob_man->eye_right->properties.color.monochrome_pixel = 1;
    noob_man->mouth->properties.color.monochrome_pixel = 1;
    noob_man->head->properties.fill = NOFILL;

    uCanvas_Delay(100);
    noob_man->eye_left->properties.type = CIRCLE;
    noob_man->eye_right->properties.type = CIRCLE;
    noob_man->head->properties.fill = FILL;
    noob_man->eye_left->properties.color.monochrome_pixel  = 0;
    noob_man->eye_right->properties.color.monochrome_pixel = 0;
    noob_man->mouth->properties.color.monochrome_pixel     = 0;
    uCanvas_Delay(100);
}



noob_man_t* noob_man1;
noob_man_t* noob_man2;
spikes_t*   spikes;

uCanvas_Animation_task_handle_t task_h;

void New_Spikes_Instance(spikes_t* spikes){
    spikes->spikes[0] = New_uCanvas_2DRectangle(0,0,0,0);
    spikes->spikes[0]->properties.fill = FILL;

    spikes->spikes[1] = New_uCanvas_2DRectangle(0,0,0,0);
    spikes->spikes[1]->properties.fill = FILL;
}

void Create_Spikes(spikes_t* spikes){
    uCanvas_universal_obj_t* total_points = New_uCanvas_2DTextbox("",60,0);
    int points = 0;
    while (1)
    {
        if(game_state == GAME_STARTED){
            int height1 = get_random_number(7,30);
            int width1 = get_random_number(4,10);

            int height2 = get_random_number(10,30);
            int width2 = get_random_number(4,10);
            
            int distance = get_random_number(40,100);
            if(player_state == PLAYER_ALIVE){
            
                spikes->spikes[0]->properties.position.x = 128;
                spikes->spikes[1]->properties.position.x = 128 + distance;
                points++;
                char stat[32]={0};
                sprintf(stat,"Points:%d",points);
                uCanvas_Set_Text(total_points,stat);
            }
            
            for (size_t i = 0; i < 128+distance; i++)
            {
                if(player_state == PLAYER_ALIVE){
                    spikes->spikes[0]->properties.position.x -= 1;
                    spikes->spikes[0]->properties.position.y = 64-height1;
                    spikes->spikes[0]->height = height1;
                    spikes->spikes[0]->width = width1;

                    spikes->spikes[1]->properties.position.x -= 1;
                    spikes->spikes[1]->properties.position.y = 64-height2;
                    spikes->spikes[1]->height = height2;
                    spikes->spikes[1]->width  = width2;
                    uCanvas_Delay(2);
                }
                else {
                    uCanvas_Delay(10);
                    spikes->spikes[0]->properties.position.x = 200;
                    spikes->spikes[1]->properties.position.x = 200;
                    break;
                }
            } 
        }
        uCanvas_Delay(2);
    }     
}

void detect_collision(){
    uCanvas_universal_obj_t* hits_stat = New_uCanvas_2DTextbox("DTHS:0",0,0);
    uCanvas_universal_obj_t* over_prompt = New_uCanvas_2DTextbox("You Died!",20,16);
    over_prompt->properties.visiblity = INVISIBLE;
    int hit = 0;
    char stat[32]={0};
    while (1)
    {
        if(game_state == GAME_STARTED){
            Coordinate2D_t noobman_pos = noob_man1->mouth->properties.position;
            Coordinate2D_t spikes_pos1 = spikes->spikes[0]->properties.position;
            Coordinate2D_t spikes_pos2 = spikes->spikes[1]->properties.position;

            int collisionRange = 15;
            if (
                (noobman_pos.x + collisionRange >= spikes_pos1.x) &&
                (noobman_pos.x - collisionRange <= spikes_pos1.x) &&
                (noobman_pos.y + collisionRange >= spikes_pos1.y) &&
                (noobman_pos.y - collisionRange <= spikes_pos1.y)
            ) {        
                player_state = PLAYER_DEAD;
                game_state = GAME_STOPPED;
                hit++;
                over_prompt->properties.visiblity = VISIBLE;
                sprintf(stat,"DTHS:%d",hit);
                uCanvas_Set_Text(hits_stat,stat);
                character_hit_animation(noob_man1);
                uCanvas_Delay(200);
                over_prompt->properties.visiblity = INVISIBLE;
                game_state = IDLE;
                player_state = PLAYER_IDLE;
                continue;
            }

            if (
                (noobman_pos.x + collisionRange >= spikes_pos2.x) &&
                (noobman_pos.x - collisionRange <= spikes_pos2.x) &&
                (noobman_pos.y + collisionRange >= spikes_pos2.y) &&
                (noobman_pos.y - collisionRange <= spikes_pos2.y)
            ){
                player_state = PLAYER_DEAD;
                game_state = GAME_STOPPED;
                hit++;
                over_prompt->properties.visiblity = VISIBLE;        
                sprintf(stat,"DTHS:%d",hit);
                uCanvas_Set_Text(hits_stat,stat);
                character_hit_animation(noob_man1);
                uCanvas_Delay(200);
                over_prompt->properties.visiblity = INVISIBLE;
                game_state = IDLE;
                player_state = PLAYER_IDLE;
                continue;
            }    
        }
        uCanvas_Delay(1);
    }
}

void game_idle_animations(void){
    uCanvas_universal_obj_t* prompt = New_uCanvas_2DTextbox("",20,40);
    while (1)
    {
        uCanvas_Delay(200); 
        if((game_state == IDLE) || (player_state == PLAYER_IDLE)){
            prompt->properties.visiblity = VISIBLE;
            uCanvas_Animate_Text_Reveal(prompt,"Hi!",10);  
            uCanvas_Delay(200);   
        }
        uCanvas_Delay(200);  

        if((game_state == IDLE )||( player_state == PLAYER_IDLE)){                 
            uCanvas_Animate_Text_Reveal(prompt,"Lets Play!",10);     
            uCanvas_Delay(200);
        }

        if(game_state != IDLE || player_state == PLAYER_IDLE){
            prompt->properties.visiblity = INVISIBLE;
        }
        uCanvas_Delay(1);
    }
}




void noob_man1_controller(){
    float velocity = 0;
    float position = 53;
    bool isJumping = false;
    uCanvas_Init_PushButton(PB0_PIN);
    while (1)
    {
        if(!isJumping){
            if(!uCanvas_Get_PushbuttonState(PB0_PIN)){
                game_state = GAME_STARTED;
                player_state = PLAYER_ALIVE;
                isJumping = true;
                velocity = JUMP_HEIGHT; 
            }
        }
        if (isJumping) {
            velocity += GRAVITY * 0.1;
            position -= velocity * 0.1;
            if (position >= 53) {
                position = 53;
                velocity = 0; 
                isJumping = false;
            }
        }
        if((player_state == PLAYER_ALIVE) || (player_state == PLAYER_IDLE))
            Set_Noob_Man_Position(noob_man1,10,position);
        uCanvas_Delay(1);
    }
}


void character_blink_animation(noob_man_t*noob_man){
    while (1)
    {
        if(player_state == PLAYER_ALIVE || player_state == PLAYER_IDLE){
            noob_man->eye_left->properties.type = RECTANGLE;
            noob_man->eye_right->properties.type = RECTANGLE;     
            uCanvas_Delay(get_random_number(20,50));
        }
        if(player_state == PLAYER_ALIVE || player_state == PLAYER_IDLE){
            noob_man->eye_left->properties.type = CIRCLE;
            noob_man->eye_right->properties.type = CIRCLE;
            uCanvas_Delay(get_random_number(300,500));
        }
        uCanvas_Delay(1);
    }
}

void noober_man_demo_loop(){
 noob_man1 = New_Noob_Man_Instance(); /* Call create to custom character object */
    
    /* uCanvas Thread to Control noob_man1 Jump Movements */
    uCanvas_Add_Task(noob_man1_controller,NULL);
      
    /**
     * Generate Spikes of random heights and widths
    */
    spikes  = (spikes_t*)malloc(sizeof(spikes_t));
    New_Spikes_Instance(spikes);

    uCanvas_Add_Task(Create_Spikes,spikes); 
    /* Collison Monitioring thread */
    uCanvas_Add_Task(detect_collision,NULL); 
    /* Idle state Animation thread */
    uCanvas_Add_Task(game_idle_animations,NULL);
    /* uCanvas Thread to Animate Eye blinks of Character */
    uCanvas_Add_Task(character_blink_animation,noob_man1);  
}