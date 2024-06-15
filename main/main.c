// #include "main.h"
// #include "noob_man.h"
// #include "uCanvas_User_IO.h"

// noob_man_t* noob_man1;
// noob_man_t* noob_man2;
// spikes_t*   spikes;

// uCanvas_Animation_task_handle_t task_h;

// void New_Spikes_Instance(spikes_t* spikes){
//     spikes->spikes[0] = New_uCanvas_2DRectangle(0,0,0,0);
//     spikes->spikes[0]->properties.fill = FILL;

//     spikes->spikes[1] = New_uCanvas_2DRectangle(0,0,0,0);
//     spikes->spikes[1]->properties.fill = FILL;
// }

// void Create_Spikes(spikes_t* spikes){
//     uCanvas_universal_obj_t* total_points = New_uCanvas_2DTextbox("",60,0);
//     int points = 0;
//     while (1)
//     {
//         if(game_state == GAME_STARTED){
//             int height1 = get_random_number(7,30);
//             int width1 = get_random_number(4,10);

//             int height2 = get_random_number(10,30);
//             int width2 = get_random_number(4,10);
            
//             int distance = get_random_number(40,100);
//             if(player_state == PLAYER_ALIVE){
            
//                 spikes->spikes[0]->properties.position.x = 128;
//                 spikes->spikes[1]->properties.position.x = 128 + distance;
//                 points++;
//                 char stat[32]={0};
//                 sprintf(stat,"Points:%d",points);
//                 uCanvas_Set_Text(total_points,stat);
//             }
            
//             for (size_t i = 0; i < 128+distance; i++)
//             {
//                 if(player_state == PLAYER_ALIVE){
//                     spikes->spikes[0]->properties.position.x -= 1;
//                     spikes->spikes[0]->properties.position.y = 64-height1;
//                     spikes->spikes[0]->height = height1;
//                     spikes->spikes[0]->width = width1;

//                     spikes->spikes[1]->properties.position.x -= 1;
//                     spikes->spikes[1]->properties.position.y = 64-height2;
//                     spikes->spikes[1]->height = height2;
//                     spikes->spikes[1]->width  = width2;
//                     uCanvas_Delay(2);
//                 }
//                 else {
//                     uCanvas_Delay(10);
//                     spikes->spikes[0]->properties.position.x = 200;
//                     spikes->spikes[1]->properties.position.x = 200;
//                     break;
//                 }
//             } 
//         }
//         uCanvas_Delay(2);
//     }     
// }

// void detect_collision(){
//     uCanvas_universal_obj_t* hits_stat = New_uCanvas_2DTextbox("DTHS:0",0,0);
//     uCanvas_universal_obj_t* over_prompt = New_uCanvas_2DTextbox("You Died!",20,16);
//     over_prompt->properties.visiblity = INVISIBLE;
//     int hit = 0;
//     char stat[32]={0};
//     while (1)
//     {
//         if(game_state == GAME_STARTED){
//             _2dPoint_t noobman_pos = noob_man1->mouth->properties.position;
//             _2dPoint_t spikes_pos1 = spikes->spikes[0]->properties.position;
//             _2dPoint_t spikes_pos2 = spikes->spikes[1]->properties.position;

//             int collisionRange = 15;
//             if (
//                 (noobman_pos.x + collisionRange >= spikes_pos1.x) &&
//                 (noobman_pos.x - collisionRange <= spikes_pos1.x) &&
//                 (noobman_pos.y + collisionRange >= spikes_pos1.y) &&
//                 (noobman_pos.y - collisionRange <= spikes_pos1.y)
//             ) {        
//                 player_state = PLAYER_DEAD;
//                 game_state = GAME_STOPPED;
//                 hit++;
//                 over_prompt->properties.visiblity = VISIBLE;
//                 sprintf(stat,"DTHS:%d",hit);
//                 uCanvas_Set_Text(hits_stat,stat);
//                 character_hit_animation(noob_man1);
//                 uCanvas_Delay(200);
//                 over_prompt->properties.visiblity = INVISIBLE;
//                 game_state = IDLE;
//                 player_state = PLAYER_IDLE;
//                 continue;
//             }

//             if (
//                 (noobman_pos.x + collisionRange >= spikes_pos2.x) &&
//                 (noobman_pos.x - collisionRange <= spikes_pos2.x) &&
//                 (noobman_pos.y + collisionRange >= spikes_pos2.y) &&
//                 (noobman_pos.y - collisionRange <= spikes_pos2.y)
//             ){
//                 player_state = PLAYER_DEAD;
//                 game_state = GAME_STOPPED;
//                 hit++;
//                 over_prompt->properties.visiblity = VISIBLE;        
//                 sprintf(stat,"DTHS:%d",hit);
//                 uCanvas_Set_Text(hits_stat,stat);
//                 character_hit_animation(noob_man1);
//                 uCanvas_Delay(200);
//                 over_prompt->properties.visiblity = INVISIBLE;
//                 game_state = IDLE;
//                 player_state = PLAYER_IDLE;
//                 continue;
//             }    
//         }
//         uCanvas_Delay(1);
//     }
// }

// void game_idle_animations(void){
//     uCanvas_universal_obj_t* prompt = New_uCanvas_2DTextbox("",20,40);
//     while (1)
//     {
//         uCanvas_Delay(200); 
//         if((game_state == IDLE) || (player_state == PLAYER_IDLE)){
//             prompt->properties.visiblity = VISIBLE;
//             uCanvas_Animate_Text_Reveal(prompt,"Hi!",10);  
//             uCanvas_Delay(200);   
//         }
//         uCanvas_Delay(200);  

//         if((game_state == IDLE )||( player_state == PLAYER_IDLE)){                 
//             uCanvas_Animate_Text_Reveal(prompt,"Lets Play!",10);     
//             uCanvas_Delay(200);
//         }

//         if(game_state != IDLE || player_state == PLAYER_IDLE){
//             prompt->properties.visiblity = INVISIBLE;
//         }
//         uCanvas_Delay(1);
//     }
// }


// void app_main(){
//     start_uCanvas_engine(); /* Start uCanvas engine */
//     uCanvas_Scene_t* main_scene = New_uCanvas_Scene(); /* Create New Scene instance */
//     uCanvas_set_active_scene(main_scene); /*Set Newly create scene as Active scene to render*/
//     noob_man1 = New_Noob_Man_Instance(); /* Call create to custom character object */
    
//     /* uCanvas Thread to Control noob_man1 Jump Movements */
//     uCanvas_Add_Task(noob_man1_controller,NULL);
    
//     /**
//      * Generate Spikes of random heights and widths
//     */
//     spikes  = (spikes_t*)malloc(sizeof(spikes_t));
//     New_Spikes_Instance(spikes);

//     uCanvas_Add_Task(Create_Spikes,spikes); 
//     /* Collison Monitioring thread */
//     uCanvas_Add_Task(detect_collision,NULL); 
//     /* Idle state Animation thread */
//     uCanvas_Add_Task(game_idle_animations,NULL);
//     /* uCanvas Thread to Animate Eye blinks of Character */
//     uCanvas_Add_Task(character_blink_animation,noob_man1);  
// }    

// void noob_man1_controller(){
//     float velocity = 0;
//     float position = 53;
//     bool isJumping = false;
//     uCanvas_Init_PushButton(PB0_PIN);
//     while (1)
//     {
//         if(!isJumping){
//             if(!uCanvas_Get_PushbuttonState(PB0_PIN)){
//                 game_state = GAME_STARTED;
//                 player_state = PLAYER_ALIVE;
//                 isJumping = true;
//                 velocity = JUMP_HEIGHT; 
//             }
//         }
//         if (isJumping) {
//             velocity += GRAVITY * 0.1;
//             position -= velocity * 0.1;
//             if (position >= 53) {
//                 position = 53;
//                 velocity = 0; 
//                 isJumping = false;
//             }
//         }
//         if((player_state == PLAYER_ALIVE) || (player_state == PLAYER_IDLE))
//             Set_Noob_Man_Position(noob_man1,10,position);
//         uCanvas_Delay(1);
//     }
// }

// _2dPoint_t get_xy_cordinates(int angle, int rx, int ry, int xoffset, int yoffset){
//     _2dPoint_t coordinates;
//     double theta = 2.0 * PI * angle / 360;            
//     uint8_t x = rx * cos(theta) + xoffset;
//     uint8_t y = ry * sin(theta) + yoffset;
//     coordinates.x = x;
//     coordinates.y = y;
//     return coordinates;
// }

// void character_blink_animation(noob_man_t*noob_man){
//     while (1)
//     {
//         if(player_state == PLAYER_ALIVE || player_state == PLAYER_IDLE){
//             noob_man->eye_left->properties.type = RECTANGLE;
//             noob_man->eye_right->properties.type = RECTANGLE;     
//             uCanvas_Delay(get_random_number(20,50));
//         }
//         if(player_state == PLAYER_ALIVE || player_state == PLAYER_IDLE){
//             noob_man->eye_left->properties.type = CIRCLE;
//             noob_man->eye_right->properties.type = CIRCLE;
//             uCanvas_Delay(get_random_number(300,500));
//         }
//         uCanvas_Delay(1);
//     }
// }



#include "main.h"
#include "uCanvas_display_port.h"
#include "grid_menu.h"
#include "st7789.h"
int solar_system_center_x = 240/2+ 50, 
    solar_system_center_y = 320/2;



void HSVtoRGB(float h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b) {
    float p, q, t, ff;
    long i;
    float r_f, g_f, b_f;

    if(s <= 0.0) {       // < is bogus, just shuts up warnings
        *r = *g = *b = (uint8_t)(v * 255.0);
        return;
    }
    h /= 60.0;           // sector 0 to 5
    i = (long)h;
    ff = h - i;          // fractional part of h
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch(i) {
    case 0:
        r_f = v;
        g_f = t;
        b_f = p;
        break;
    case 1:
        r_f = q;
        g_f = v;
        b_f = p;
        break;
    case 2:
        r_f = p;
        g_f = v;
        b_f = t;
        break;
    case 3:
        r_f = p;
        g_f = q;
        b_f = v;
        break;
    case 4:
        r_f = t;
        g_f = p;
        b_f = v;
        break;
    case 5:
    default:
        r_f = v;
        g_f = p;
        b_f = q;
        break;
    }
    *r = (uint8_t)(r_f * 255.0);
    *g = (uint8_t)(g_f * 255.0);
    *b = (uint8_t)(b_f * 255.0);
}

void setRainbowColor(uCanvas_universal_obj_t* blob, float position) {
    // position should be in range [0, 1], mapping to [0, 360] degrees in hue
    float hue = position * 360.0;
    uint8_t r, g, b;
    HSVtoRGB(hue, 1.0, 1.0, &r, &g, &b);
    uCanvas_Set_Color(blob, r, g, b);
}


void rainbow_blob_task(void* obj){
    uCanvas_universal_obj_t* blob1 = New_uCanvas_2DCircle(150,24,8);
    uCanvas_universal_obj_t* blob2 = New_uCanvas_2DCircle(150,24+20,8);
    uCanvas_universal_obj_t* blob3 = New_uCanvas_2DCircle(150,24+20+20,8);
    uCanvas_universal_obj_t* blob4 = New_uCanvas_2DCircle(150,24+20+20+20,8);
    blob1->properties.fill = FILL;
    blob2->properties.fill = FILL;
    blob3->properties.fill = FILL;
    blob4->properties.fill = FILL;
    while (1)
    {
        for(float pos = 0; pos <= 1.0; pos += 0.01) {
            setRainbowColor(blob1, pos+0.1);
            setRainbowColor(blob2, pos+0.2);
            setRainbowColor(blob3, pos+0.6);
            setRainbowColor(blob4, pos+0.9);

            uCanvas_Delay(1);
        }
    } 
}

extern  TFT_t dev;


#define GRID_SIZE 20
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define GRID_SPACING 20

void draw_tilted_grid(uint16_t num_lines, float tilt_angle) {
    float cos_tilt = cos(tilt_angle); // tilt_angle is in radians
    float sin_tilt = sin(tilt_angle); // tilt_angle is in radians

    // Create horizontal lines
    for (uint16_t i = 0; i < num_lines; i++) {
        uint16_t y = SCREEN_HEIGHT - i * GRID_SPACING;
        uint16_t x_start = 0;
        uint16_t x_end = SCREEN_WIDTH;

        // Apply perspective transformation for tilting on Z-axis
        int16_t x_start_tilted = x_start + (y * sin_tilt);
        int16_t y_start_tilted = y * cos_tilt;
        int16_t x_end_tilted = x_end + (y * sin_tilt);
        int16_t y_end_tilted = y * cos_tilt;

        uCanvas_universal_obj_t* grid_line_h = New_uCanvas_2DLine(x_start_tilted, y_start_tilted, x_end_tilted, y_end_tilted);
        if (grid_line_h == NULL) {
            printf("Failed to create horizontal grid line %d\n", i);
        } else {
            uCanvas_Set_Color(grid_line_h, 255, 255, 255); // Deep pink grid lines
        }
    }

    // Create vertical lines
    for (uint16_t i = 0; i < num_lines; i++) {
        uint16_t x = i * GRID_SPACING;
        uint16_t y_start = SCREEN_HEIGHT;
        uint16_t y_end = SCREEN_HEIGHT / 2;

        // Apply perspective transformation for tilting on Z-axis
        int16_t x_start_tilted = x + (y_start * sin_tilt);
        int16_t y_start_tilted = y_start * cos_tilt;
        int16_t x_end_tilted = x + (y_end * sin_tilt);
        int16_t y_end_tilted = y_end * cos_tilt;

        uCanvas_universal_obj_t* grid_line_v = New_uCanvas_2DLine(x_start_tilted, y_start_tilted, x_end_tilted, y_end_tilted);
        if (grid_line_v == NULL) {
            printf("Failed to create vertical grid line %d\n", i);
        } else {
            uCanvas_Set_Color(grid_line_v, 255, 20, 147); // Deep pink grid lines
        }
    }
}


typedef struct grid
{
    uCanvas_universal_obj_t* H_Lines[120];
    uCanvas_universal_obj_t* V_Lines[120];
    int xstart;
    int ystart;
    int xend;
    int yend;
    int space;
}grid_t;

grid_t grid1 = {
    .xstart = 0,
    .ystart = 0,
    .space = 10,
    .xend = 240,
    .yend = 240
};

void create_grid(){
    // int x_start = 
    for (int i = 0; i < 20; i++)
    {
        grid1.H_Lines[i] = New_uCanvas_2DLine((grid1.xstart*i)+grid1.space,grid1.ystart,(grid1.xend*i)+grid1.space,grid1.yend);
        uCanvas_Set_Color(grid1.H_Lines[i],255,0,0);
    }
    
}

void app_main(){

    start_uCanvas_engine();
    color_t color;
    color.red = 0;
    color.green = 0;
    color.blue = 0;
    uCanvas_Scene_t* main_scene = New_uCanvas_Scene();
    
    uCanvas_set_active_scene(main_scene);
    uCanvas_Fill_screen(color);


    
    //creates grid menu.
    tiles_t* tiles_menu =(tiles_t*) malloc(sizeof(tiles_t));
    generate_grid_menu(tiles_menu,2,2,80,80,9,0,0,0);

    //Set title bar
    uCanvas_universal_obj_t* title_box = New_uCanvas_2DTextbox("uCanvas Tiles Menu Demo",220, 60);
    uCanvas_Set_Color(title_box,255,255,255);
    uCanvas_Animate_Text_Reveal(title_box,"uCanvas Tiles Menu Demo",5);

    //Show tiles menu by fade in effect
    color_t c= {.red = 255, .green=0  , .blue = 0};
    show_tiles_with_fade_in_effect(tiles_menu,c);

    //Update content of some tiles with fade in/out effects
    for (int i = 0; i < tiles_menu->row*tiles_menu->col; i+=2)
    {
        update_tile_with_fio_effect(tiles_menu,"Updated!",i);
        uCanvas_Delay(10);
    }
    
    //Create Icon embedded to specific tile
    int x = get_tile_positon(tiles_menu,2).x;
    int y = get_tile_positon(tiles_menu,2).y;
    int icon_idx1 = push_obj_as_tile_icon_element(tiles_menu,New_uCanvas_2DCircle(x+16,y+16,15));
    int icon_idx2 = push_obj_as_tile_icon_element(tiles_menu,New_uCanvas_2DTextbox("X",x,y+10));
    
    //Set properties of Icon elements.
    uCanvas_Set_Fill(get_icon_obj_by_id(tiles_menu,icon_idx1),FILL);
    uCanvas_Set_Color(get_icon_obj_by_id(tiles_menu,icon_idx2),255,255,255);

    //Show Icons with fade in effect.
    color_t c1 = {.red = 255, .green = 255, .blue = 255}; 
    color_t c2 = {.red = 0, .green = 0, .blue = 0};
    set_color_for_icon_element(tiles_menu,icon_idx1,c1);
    set_color_for_icon_element(tiles_menu,icon_idx2,c2);
    show_icons_by_fade_in_effect(tiles_menu);

    //Add tiles selector which acts as cursor by highlighting selected tiles by index.
    add_tiles_selector(tiles_menu);

    color_t fc = {.red = 255, .green = 255, .blue = 0};
    while (1)
    {
        for (int  i = 0; i < tiles_menu->row*tiles_menu->col; i++)
        {
            select_tiles_by_index(tiles_menu,i);
            uCanvas_Delay(120);
        }
    }
    

    // pan_menu(tiles_menu,10,1,1);
    // pan_menu(tiles_menu,4,1,0);
    // uCanvas_Delay(250);
    // pan_menu(tiles_menu,10,1,0);
    // pan_menu(tiles_menu,5,1,1);

    // uCanvas_Delay(140);
    // pan_menu(tiles_menu,20,1,3);
    // uCanvas_Delay(140);
    // pan_menu(tiles_menu,20,1,2);
    


 while (1)
 {
    /* code */
    uCanvas_Delay(10);
 }
 
    // while (1)
    // {
    //    for (int j = 0; j < 255; j++)
    //    {
    //     for (int i = 0; i < (tiles_menu.row*tiles_menu.col); i++)
    //     {
    //         sprintf(buf,"TILE %d",j);
    //         uCanvas_Set_Text(tiles_menu.content[i],buf);
    //         uCanvas_Delay(10);
    //     }
    //    }
    // }
    

    // uCanvas_universal_obj_t* text_box1 = New_uCanvas_2DTextbox("Part Num             : ST7789"  ,20         ,10  );
    // uCanvas_universal_obj_t* text_box2 = New_uCanvas_2DTextbox("Resolution           : 320x240" ,16+20      ,10  );
    // uCanvas_universal_obj_t* text_box3 = New_uCanvas_2DTextbox("Local Buffer Support : Yes"     ,16+20+20   ,10  );
    // uCanvas_universal_obj_t* text_box4 = New_uCanvas_2DTextbox("Uptime               : 0"       ,16+20+20+20,10  );
    // uCanvas_Set_Color(text_box1,255,255,255);
    // uCanvas_Set_Color(text_box2,255,255,255);
    // uCanvas_Set_Color(text_box3,255,255,255);
    // uCanvas_Set_Color(text_box4,255,255,255);
    
    // uCanvas_Add_Task(uCanvas_Animation_task_Planet_3,NULL); //Create Planet Animation Loop thread  
    // uCanvas_universal_obj_t* sun =  New_uCanvas_2DCircle(solar_system_center_x,solar_system_center_y,15); //Create static Sun
    // uCanvas_Set_Color(sun,255,255,0);
    // sun->properties.fill = FILL;
    // char buf[128];
    // uint8_t i = 0;
    // uCanvas_Add_Task(rainbow_blob_task,NULL);

    // while (1)
    // {
        
    //     sprintf(buf,"Uptime               : %d",i++);
    //     uCanvas_Set_Text(text_box4,buf);
    //     uCanvas_Delay(30);

    //     // uCanvas_Set_Color(blob,r,g,b);
    // }
    
    
    // ;

    // uCanvas_Add_Task(uCanvas_Animation_task_Planet_1,NULL); //Create Planet Animation Loop thread
    // uCanvas_Add_Task(uCanvas_Animation_task_Planet_2,NULL); //Create Planet Animation Loop thread
    
    // uCanvas_Add_Task(Control_Level_bar_task,NULL); //Just animate Level bar for example.  

    uCanvas_Delay(400);
    
}


void uCanvas_Animation_task_Planet_1(void*arg){
    printf("Animation_Task1\r\n");
    uCanvas_universal_obj_t* Planet_1 = New_uCanvas_2DCircle(solar_system_center_x,solar_system_center_y,15);   
    uCanvas_Set_Color(Planet_1,0,100,255);
    Planet_1->properties.fill = FILL;
    while (1)
    {
        for (int i = 0; i < 360; ++i) {
            _2dPoint_t coordinates = get_xy_cordinates(i,60,90,solar_system_center_x,solar_system_center_y);
            uCanvas_Set_Position(Planet_1,coordinates.x,coordinates.y);
            uCanvas_Delay(40/portTICK_PERIOD_MS);
        }     
    }
}

void uCanvas_Animation_task_Planet_2(void*arg){
    printf("Animation_Task2\r\n");
    uCanvas_universal_obj_t* Planet_2 = New_uCanvas_2DCircle(64,40,24);
    uCanvas_Set_Color(Planet_2,255,100,0);
    Planet_2->properties.fill = FILL;
    while (1)
    {
        for (int i = 360; i > 0; i--) {
            _2dPoint_t coordinates = get_xy_cordinates(i,60,90,solar_system_center_x,solar_system_center_y);
            uCanvas_Set_Position(Planet_2,coordinates.x,coordinates.y);
            uCanvas_Delay(10/portTICK_PERIOD_MS);
        }     
    }
}

void uCanvas_Animation_task_Planet_3(void*arg){
    printf("Animation_Task3\r\n");
    uCanvas_universal_obj_t* Planet_3 = New_uCanvas_2DCircle(100,40,10);
    uCanvas_universal_obj_t* Line =     New_uCanvas_2DLine(0,0,0,0);
    uCanvas_universal_obj_t* Floating_Txtbox = New_uCanvas_2DTextbox("NULL",0,0);
    uCanvas_Set_Color(Planet_3,0,100,255);
    Planet_3->properties.fill = FILL;
    Floating_Txtbox->properties.visiblity = VISIBLE;
    uCanvas_Set_Color(Floating_Txtbox,255,255,0);
    while (1)
    {
        for (int i = 360; i > 0; i--) {

            _2dPoint_t coordinates = get_xy_cordinates(i,40,90,solar_system_center_x,solar_system_center_y);
            char dist[16] = {0};
            sprintf(dist,"dst:%d",coordinates.x);
            uCanvas_Set_Text(Floating_Txtbox,dist);
            
            //Manipulating Properties of On Screen Object without API
            Floating_Txtbox->properties.position.x = coordinates.x-9;
            Floating_Txtbox->properties.position.y = coordinates.y-14;

            Planet_3->properties.position.x = coordinates.x;
            Planet_3->properties.position.y = coordinates.y;

            Line->point1.x = solar_system_center_x;
            Line->point1.y = solar_system_center_y;

            Line->point2.x = coordinates.x;
            Line->point2.y = coordinates.y;

            uCanvas_Delay(10/portTICK_PERIOD_MS);
        }     
    }
}

void Control_Level_bar_task(void*arg){
    uCanvas_universal_obj_t* bars[4] = {0}; //Array 4 Bars.
    uint8_t xpos = 45, ypos = 0, width = 5, height = 10;

    bars[0] = New_uCanvas_2DRectangle(xpos+0, ypos,height,width);   
    bars[1] = New_uCanvas_2DRectangle(xpos+8, ypos,height,width); 
    bars[2] = New_uCanvas_2DRectangle(xpos+16,ypos,height,width); 
    bars[3] = New_uCanvas_2DRectangle(xpos+24,ypos,height,width); 

    New_uCanvas_2DTextbox("LEVEL:",0,0);
    uCanvas_universal_obj_t* current_level = New_uCanvas_2DTextbox(".",xpos+24+10,0);
    while (1)
    {
        for (int i = 0; i < 4; i++)
        {
            char level[32];
            sprintf(level,"%d",i);
            uCanvas_Set_Text(current_level,level);
            uCanvas_Set_Fill(bars[i],FILL);
            uCanvas_Delay(20);
        }
        for (int i = 3; i >= 0; i--)
        {
            char level[32];
            sprintf(level,"%d",i);
            uCanvas_Set_Text(current_level,level);
            uCanvas_Set_Fill(bars[i],NOFILL);
            uCanvas_Delay(20);
        }
    }
}

_2dPoint_t get_xy_cordinates(int angle, int rx, int ry, int xoffset, int yoffset){
    _2dPoint_t coordinates;
    double theta = 2.0 * PI * angle / 360;            
    uint8_t x = rx * cos(theta) + xoffset;
    uint8_t y = ry * sin(theta) + yoffset;
    coordinates.x = x;
    coordinates.y = y;
    return coordinates;
}