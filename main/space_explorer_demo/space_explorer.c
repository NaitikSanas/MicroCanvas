#include "space_explorer.h"
#include "noob_man.h"

fighter_jet_t* player_fighter_jet;

fighter_jet_t* create_fighter_jet_instance(int x, int y, int notch){
    fighter_jet_t* obj = (fighter_jet_t*)malloc(sizeof(fighter_jet_t));
    obj->notch_face = notch;
    obj->position.x = x;
    obj->position.y = y;
    obj->P1.x = obj->position.x;
    obj->P1.y = obj->position.y - 5;
    obj->P2.x = obj->position.x;
    obj->P2.y = obj->position.y + 5;
    obj->P3.x = notch;
    obj->P3.y = obj->position.y;
    obj->fighter_jet = New_uCanvas_2DTriangle(obj->P1,obj->P2,obj->P3);
    obj->fighter_jet->properties.fill = FILL;
    return obj;
}

stars_t* create_stars(int instances){
    stars_t* stars = (stars_t*)malloc(sizeof(stars_t));
    stars->stars_instances = instances; 
    for (int i = 0; i < instances; i++)
    {
        int xpos = get_random_number(128,550);
        int ypos = get_random_number(16,64);
        int r1  =  get_random_number(0,1);
        stars->stars[i] = New_uCanvas_2DCircle(xpos,ypos,r1);
        stars->stars[i]->properties.fill = FILL;
    }
    return stars;
}
int speed = 5;
void move_stars(stars_t* stars){
    while (1)
    {
        for (int  i = 0; i < stars->stars_instances; i++)
        {
            int blink = get_random_number(0,100);

            if(stars->stars[i]->properties.position.x > 0){
                stars->stars[i]->properties.position.x--;
            }
            if((stars->stars[i]->properties.position.x > 0 && stars->stars[i]->properties.position.x < 128)&&
                (stars->stars[i]->properties.position.x > stars->stars[i]->r1)
            ){
                
                if((blink > 15  && blink < 20) && (stars->stars[i]->r1 == 0)) stars->stars[i]->properties.visiblity = INVISIBLE;
                else stars->stars[i]->properties.visiblity = VISIBLE;
            }
            else {
                stars->stars[i]->properties.visiblity = INVISIBLE;
                stars->stars[i]->properties.position.x = get_random_number(128,550);
                stars->stars[i]->properties.position.y = get_random_number(16,64);
                
            }
            
        }
        if(!uCanvas_Get_PushbuttonState(5)){
        while (!uCanvas_Get_PushbuttonState(5))
        {
            uCanvas_Delay(1);
        }
            if(speed<7)speed++;
            else speed = 1;
        }
        uCanvas_Delay(speed);
    }
    
}


fighter_jet_army_t* spawn_army(int instances){
    fighter_jet_army_t* enemy_army = (fighter_jet_army_t*)malloc(sizeof(fighter_jet_army_t));
    enemy_army->spawned_instances = instances; 
    for (int i = 0; i < instances; i++)
    {
        //Spawn Jets on Randomize XY Coordinates for each instances
        int xpos = get_random_number(80,120);
        int ypos = get_random_number(20,56);
        enemy_army->enemy_jets[i] = create_fighter_jet_instance(xpos,ypos,-10);
        

        //Randomize Fill/NoFill for each jet instances
        int fill_rnd = get_random_number(0,100);
        if(fill_rnd > 15  && fill_rnd < 30) enemy_army->enemy_jets[i]->fighter_jet->properties.fill  = NOFILL;
        else enemy_army->enemy_jets[i]->fighter_jet->properties.fill  = FILL;
    }
    return enemy_army;
}

void randomize_positions(fighter_jet_army_t* enemy_army,int i){
    int xpos = get_random_number(50,300);
    int ypos = get_random_number(22,56);
    int fill_rnd = get_random_number(0,100);
    if(fill_rnd > 15  && fill_rnd < 50) enemy_army->enemy_jets[i]->fighter_jet->properties.fill  = NOFILL;
    else enemy_army->enemy_jets[i]->fighter_jet->properties.fill  = FILL;
    enemy_army->enemy_jets[i]->position.x = 128+xpos;
    enemy_army->enemy_jets[i]->position.y = ypos;
}

void redraw_fighter_jet(fighter_jet_t* player_fighter_jet){
    player_fighter_jet->fighter_jet->point1.x = player_fighter_jet->position.x;
    player_fighter_jet->fighter_jet->point1.y = player_fighter_jet->position.y - 5;

    player_fighter_jet->fighter_jet->point2.x = player_fighter_jet->position.x;
    player_fighter_jet->fighter_jet->point2.y = player_fighter_jet->position.y + 5;
    
    player_fighter_jet->fighter_jet->point3.x = player_fighter_jet->position.x+player_fighter_jet->notch_face;
    player_fighter_jet->fighter_jet->point3.y = player_fighter_jet->position.y;
}


void attack_player_through_army_instance(fighter_jet_army_t* army){
    for (int i = 0; i < army->spawned_instances; i++)
    {
        /**
         * Take steps to travel
        */
        if(army->enemy_jets[i]->position.x > 10)army->enemy_jets[i]->position.x--;
        else {
            randomize_positions(army,i); //Respawn enemy that reached out of bounds at random location at right side.
        }
        
        /**
         * Redraw jets with new positions on screen
        */
        if(army->enemy_jets[i]->position.x <128){
            army->enemy_jets[i]->fighter_jet->properties.visiblity = VISIBLE;
            redraw_fighter_jet(army->enemy_jets[i]);
        }
        else army->enemy_jets[i]->fighter_jet->properties.visiblity = INVISIBLE;    
    }
}


void update_player_jet_position_on_user_input(){
    if(!uCanvas_Get_PushbuttonState(5)){
            while (!uCanvas_Get_PushbuttonState(5))
            {
                uCanvas_Delay(1);
            }
            if(player_fighter_jet->position.y < 64)player_fighter_jet->position.y++;
            else player_fighter_jet->position.y = 16;
            redraw_fighter_jet(player_fighter_jet);
    }
}


void space_explorer_demo_loop(void){
        
    player_fighter_jet = create_fighter_jet_instance(3,32,10);
    
    uCanvas_Init_PushButton(5);

    fighter_jet_army_t* enemy_army = spawn_army(4);
    
    New_uCanvas_2DTextbox("Space Explorer >",10,0);
    
    stars_t* stars = create_stars(50);
    uCanvas_Add_Task(move_stars,(stars_t*)stars);
    while (1)
    {

        attack_player_through_army_instance(enemy_army);
        update_player_jet_position_on_user_input();
        uCanvas_Delay(2);
    }
}