#include "uCanvas_api.h"
#include "uCanvas_User_IO.h"

/*
    Simple fighterJet structure created with 3 point vectors. 
    P1, P2, P3 Nodes are derived from X,Y location and Notch orientetion provided by user.


                            P1 -> (pos_y - 5)
                            *
                          * *
    P3(Notch_Point) ->  *   * <- Center point (pos_x,pos_y)
                          * *
                            * 
                            P2 -> (pos_y + 5)
                        
                        |<-->|
                  Distance = Notch_Point + pos_x  
                  if Notch_Point -x > Facing Left side
                  if Notch_Point +x > Facing Right side
*/
typedef struct fighter_jet
{
   uCanvas_universal_obj_t* fighter_jet;
   int notch_face;
    Coordinate2D_t P1;
    Coordinate2D_t P2;
    Coordinate2D_t P3;
    Coordinate2D_t position;
}fighter_jet_t;

typedef struct enemy_army
{
    fighter_jet_t* enemy_jets[10];
    int spawned_instances;
}fighter_jet_army_t;

typedef struct stars
{
    uCanvas_universal_obj_t* stars[100];
    int stars_instances;
}stars_t;



void space_explorer_demo_loop(void);