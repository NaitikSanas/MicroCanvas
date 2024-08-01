#include "main.h"
#include "noob_man.h"
#include "space_explorer.h"
#include "uCanvas_Physix.h" 


static uint8_t house_sprite[16*8] = {   \
    2,2,1,1,2,2,2,2,1,1,1,1,1,2,2,2,     \
    2,2,1,1,2,2,2,1,1,1,1,1,1,1,2,2,     \
    2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,     \
    1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,     \
    1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,     \
    1,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,      \
    1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,      \
    1,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1      \
};

static uint8_t building_sprite[9*16] = {   \
    1,1,1,1,1,1,1,1,1,                    \
    1,0,0,0,1,0,0,0,1,                    \
    1,0,0,0,1,0,0,0,1,                    \
    1,1,1,1,1,1,1,1,1,                    \
    1,0,0,0,1,0,0,0,1,                    \
    1,0,0,0,1,0,0,0,1,                    \
    1,1,1,1,1,1,1,1,1,                    \
    1,0,0,0,1,0,0,0,1,                    \
    1,0,0,0,1,0,0,0,1,  
    1,1,1,1,1,1,1,1,1,  
    1,0,0,0,1,0,0,0,1,  
    1,0,0,0,1,0,0,0,1,  
    1,1,1,1,1,1,1,1,1,  
    1,1,1,0,0,0,1,1,1,  
    1,1,1,0,0,0,1,1,1,  
    1,1,1,0,0,0,1,1,1,  
};

static uint8_t happy_face_sprite[9*9] = {   \
    1,1,1,1,1,1,1,1,1,                    \
    1,0,0,0,0,0,0,0,1,                    \
    1,0,1,1,0,1,1,0,1,                    \
    1,0,1,1,0,1,1,0,1,                    \
    1,0,0,0,0,0,0,0,1,                    \
    1,0,1,0,0,0,1,0,1,                    \
    1,0,0,1,1,1,0,0,1,                    \
    1,0,0,0,0,0,0,0,1,                    \
    1,1,1,1,1,1,1,1,1  
};

static uint8_t surprised_face_sprite[9*9] = {   \
    0,1,1,1,1,1,1,1,0,                    \
    1,0,0,0,0,0,0,0,1,                    \
    1,0,1,1,0,1,1,0,1,                    \
    1,0,1,1,0,1,1,0,1,                    \
    1,0,0,0,0,0,0,0,1,                    \
    1,0,0,1,1,1,0,0,1,                    \
    1,0,0,1,1,1,0,0,1,                    \
    1,0,0,0,0,0,0,0,1,                    \
    0,1,1,1,1,1,1,1,0  
};

static uint8_t neutral_face_sprite[9*9] = {   \
    0,1,1,1,1,1,1,1,0,                    \
    1,0,0,0,0,0,0,0,1,                    \
    1,0,1,1,0,1,1,0,1,                    \
    1,0,0,0,0,0,0,0,1,                    \
    1,0,0,0,0,0,0,0,1,                    \
    1,0,0,0,0,0,0,0,1,                    \
    1,0,1,1,1,1,1,0,1,                    \
    1,0,0,0,0,0,0,0,1,                    \
    0,1,1,1,1,1,1,1,0  
};

static uint8_t sad_face_sprite[9*9] = {   \
    0,1,1,1,1,1,1,1,0,                    \
    1,0,0,0,0,0,0,0,1,                    \
    1,0,1,1,0,1,1,0,1,                    \
    1,0,1,1,0,1,1,0,1,                    \
    1,0,0,0,0,0,0,0,1,                    \
    1,0,0,1,1,1,0,0,1,                    \
    1,0,1,0,0,0,1,0,1,                    \
    1,0,0,0,0,0,0,0,1,                    \
    0,1,1,1,1,1,1,1,0  
};


static uint8_t tree_sprite[9*9] = {   \
    0,0,1,0,1,0,1,0,0,                    \
    0,1,0,1,0,1,0,1,0,                    \
    1,0,1,0,1,0,1,0,1,                    \
    1,0,1,0,1,0,1,0,1,                    \
    0,1,0,1,0,1,0,1,0,                    \
    0,0,1,0,1,0,1,0,0,                    \
    0,0,0,1,1,1,0,0,0,                    \
    0,0,0,1,1,1,0,0,0,                    \
    0,1,1,1,1,1,1,1,0  
};

static uint8_t grass_sprite[9*9] = {   \
    2,2,1,2,2,2,2,2,2,                    \
    1,2,2,1,2,2,1,2,1,                    \
    2,1,2,1,2,1,1,2,1,                    \
    2,1,2,1,2,1,2,1,1,                    \
    2,1,2,1,2,1,1,1,2,                    \
    2,1,2,2,1,1,1,2,2,                    \
    2,1,1,1,1,1,1,2,2,                    \
    1,1,1,1,1,1,1,1,1,                    \
    2,1,1,2,1,1,1,2,2  
};



static uint8_t grid_sprite[9*9] = {   \
    1,1,1,1,1,1,1,1,1,                \
    0,1,0,1,0,1,0,1,0,                \
    1,1,1,1,1,1,1,1,1,                \
    0,1,0,1,0,1,0,1,0,                \
    1,1,1,1,1,1,1,1,1,                \
    0,1,0,1,0,1,0,1,0,                \
    1,1,1,1,1,1,1,1,1,                \
    0,1,0,1,0,1,0,1,0,                \
    1,1,1,1,1,1,1,1,1,                \
};




static uint8_t grid_sprite2[9*6] = {   \
    2,2,2,1,1,2,2,2,2,               \
    2,2,1,1,0,1,2,2,2,               \
    2,1,0,0,1,1,1,2,2,               \
    1,0,0,1,1,1,1,1,1,               \
    1,0,0,1,1,1,1,1,1,               \
    1,1,1,1,1,1,1,1,2,               \
    2,2,2,2,2,2,2,2,2,               \
    2,2,2,2,2,2,2,2,2,               \
    2,2,2,2,2,2,2,2,2,               \
};



static uint8_t car_sprite[16*8] = {   \
    2,2,1,1,1,1,1,1,1,1,1,2,2,2,2,2,     \
    2,1,0,0,0,0,1,0,0,0,0,1,2,2,2,2,     \
    2,1,0,0,0,0,1,0,0,0,0,0,1,1,1,1,     \
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,     \
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,     \
    1,1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,      \
    1,1,0,1,1,0,1,1,1,1,0,1,1,0,1,1,      \
    2,2,2,1,1,2,2,2,2,2,2,1,1,2,2,2      \
};


static uint8_t mountain_sprite[16*8] = {   \
    2,2,2,2,2,2,2,1,1,1,1,1,1,2,2,2,     \
    2,2,2,2,2,2,1,1,1,1,1,1,1,2,2,2,     \
    2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,     \
    2,2,2,1,1,1,1,1,1,1,1,1,1,1,2,2,     \
    2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,     \
    2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,      \
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,      \
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1      \
};

stars_t* create_stars(int instances){
    stars_t* stars = (stars_t*)malloc(sizeof(stars_t));
    stars->stars_instances = instances; 
    for (int i = 0; i < instances; i++)
    {
        int xpos = get_random_number(0,128);
        int ypos = get_random_number(0,20);
        int r1  =  get_random_number(0,1);
        stars->stars[i] = New_uCanvas_2DCircle(xpos,ypos,r1);
        stars->stars[i]->properties.fill = FILL;
    }
    return stars;
}
static uCanvas_Scene_t* main_scene;

uCanvas_universal_obj_t* clouds1[6];
    uCanvas_universal_obj_t* clouds2[6];
void cloud_animation(){
    while (1)
    {
        for (int i = 0; i < 6; i++)
        {
            if(clouds1[i]->properties.position.x != -9){
                clouds1[i]->properties.position.x-=1;
            }
            else {
                clouds1[i]->properties.position.x = get_random_number(140,200);
                clouds1[i]->properties.position.y = get_random_number(0,15);
            }
           
        }
         uCanvas_Delay(10);
    }
    
}

void car_animation(uCanvas_universal_obj_t* obj){
    int toggle = 1;
    while (1)
    {
        if(toggle)
        obj->properties.position.y += 2;
        else 
        obj->properties.position.y -= 2;
        toggle = !toggle;
        uCanvas_Delay(30);              
    }
    
    
}

void app_main(){
    start_uCanvas_engine(); /* Start uCanvas engine */
    
    main_scene = New_uCanvas_Scene(); /* Create New Scene instance */
    uCanvas_set_active_scene(main_scene); /*Set Newly create scene as Active scene to render*/
    
    /**
     * Example of scaling up the 2D sprites.
     */
    #define scale 2 //sets scaling level of sprite
    #define scale2 2 //sets scaling level of sprite
    #define car_scale 3 //sets scaling level of sprite
    static uint8_t happy_face_scaled_sprite     [(9* scale) * (9* scale)]; //buffer to store scaled up sprite
    static uint8_t surprised_face_scaled_sprite [(9* scale) * (9* scale)]; //buffer to store scaled up sprite
    static uint8_t neutral_face_scaled_sprite   [(9* scale) * (9* scale)]; //buffer to store scaled up sprite
    static uint8_t sad_face_scaled_sprite       [(9* scale) * (9* scale)]; //buffer to store scaled up sprite
    static uint8_t tree_scaled_sprite           [(9* scale2) * (9* scale2)]; //buffer to store scaled up sprite
    static uint8_t car_scaled_sprite           [(16* car_scale) * (8* car_scale)]; //buffer to store scaled up sprite
    static uint8_t building_scaled_sprite           [(9* car_scale) * (16* car_scale)]; //buffer to store scaled up sprite
    
    /**
     * Scales 9x9 sprite to 27 x 27 
     * */ 
    uCanvas_ScaleUp_Sprite2D(happy_face_sprite, happy_face_scaled_sprite        , 9, 9,scale);
    uCanvas_ScaleUp_Sprite2D(surprised_face_sprite, surprised_face_scaled_sprite, 9, 9,scale);
    uCanvas_ScaleUp_Sprite2D(neutral_face_sprite, neutral_face_scaled_sprite    , 9, 9,scale);
    uCanvas_ScaleUp_Sprite2D(sad_face_sprite, sad_face_scaled_sprite            , 9, 9,scale);
    uCanvas_ScaleUp_Sprite2D(tree_sprite, tree_scaled_sprite                    , 9, 9,scale2);
    uCanvas_ScaleUp_Sprite2D(car_sprite, car_scaled_sprite                    , 16, 8,car_scale);
    uCanvas_ScaleUp_Sprite2D(building_sprite, building_scaled_sprite          , 9, 16,car_scale);
    
    /**
     * Create 2D Sprite Object based on raw Sprite buffer so we can use it easily in application
     *  sprite2D_t (properties) -
     *      > Sprite_buffer - Pointes to 1D buffer storing the pixel values
     *      > Sprite_Height - Actual Height of sprite buffer (CHANGING THIS DOES NOT SCALES THE SPRITES)
     *      > Sprite_Width  - Actual Width  of sprite buffer (CHANGING THIS DOES NOT SCALES THE SPRITES)
     *      > Sprite_Orientation
     */
    sprite2D_t sad_face_scaled_obj,
               happy_face_scaled_obj,
               surprised_face_scaled_obj,
               tree_scaled_obj,
               tree_obj,
               neutral_face_scaled_obj,
               grid_obj,
               grid_obj2,
               car_obj1,
               car_obj2, 
               grass_obj,
               mountain_obj,
               building_obj,
               house_obj;
    
    uCanvas_Compose_2DSprite_Obj(&happy_face_scaled_obj     ,happy_face_scaled_sprite    , 9*scale,9*scale);
    uCanvas_Compose_2DSprite_Obj(&surprised_face_scaled_obj ,surprised_face_scaled_sprite, 9*scale,9*scale);
    uCanvas_Compose_2DSprite_Obj(&neutral_face_scaled_obj   ,neutral_face_scaled_sprite  , 9*scale,9*scale);
    uCanvas_Compose_2DSprite_Obj(&sad_face_scaled_obj       ,sad_face_scaled_sprite      , 9*scale,9*scale);
    uCanvas_Compose_2DSprite_Obj(&tree_scaled_obj           ,tree_scaled_sprite      , 9*scale2,9*scale2);
    uCanvas_Compose_2DSprite_Obj(&tree_obj                  ,tree_sprite      , 9,9);
    uCanvas_Compose_2DSprite_Obj(&grid_obj                  ,grid_sprite      , 9,9);
    uCanvas_Compose_2DSprite_Obj(&grid_obj2                  ,grid_sprite2      , 9,6);
    uCanvas_Compose_2DSprite_Obj(&car_obj1                  ,car_sprite      , 16,8);
    uCanvas_Compose_2DSprite_Obj(&car_obj2                  ,car_scaled_sprite      , 16*car_scale,8*car_scale);
    uCanvas_Compose_2DSprite_Obj(&grass_obj                  ,grass_sprite      , 9,9);
    uCanvas_Compose_2DSprite_Obj(&mountain_obj                  ,mountain_sprite      , 16,8);
    uCanvas_Compose_2DSprite_Obj(&building_obj                  ,building_sprite      , 9,16);
    uCanvas_Compose_2DSprite_Obj(&house_obj                  ,house_sprite      , 16,8);


    /**
     * This api creates Sprite Element on currently active scene and forwards to the render engine.
     *  sprite_element(properties) - 
     *       > sprite2D_t       - Sprite Object 
     *       > pos_x            - Absolute Sprite Position X      
     *       > pos_y            - Absolute Sprite Position Y
     */
    
    uCanvas_universal_obj_t* circle = New_uCanvas_2DCircle(64,10,10);
     circle->properties.fill = FILL;

    New_uCanvas_2DLine(0,38,127,23);
   
    
    for (int i = 0; i < 6; i++)
    {
        clouds1[i] = New_uCanvas_2DSprite(grid_obj2,i*10+get_random_number(2,100),get_random_number(2,12));
    }




     uCanvas_universal_obj_t* map_features[13];
    for (int i = 0; i < 13; i++)
    {
        map_features[i] =New_uCanvas_2DSprite(tree_obj,i*10,30);
    }

    
    // New_uCanvas_2DLine(0,38,127,40);
    // uCanvas_universal_obj_t* fences[15];
    //  for (int i = 0; i < 15; i++)
    // {
    //     fences[i] = New_uCanvas_2DSprite(grid_obj,i*9,37);
    // }



    static uCanvas_universal_obj_t* sprite1;
    static uCanvas_universal_obj_t* sprite2;

    sprite1 = New_uCanvas_2DSprite(car_obj2     ,4,12);
    // sprite2 = New_uCanvas_2DSprite(happy_face_scaled_obj     ,50,45);
    
    //Attach TYPE 1 controller to sprite 1
    static controller_properties_t player_controller_properties;
    uCanvas_add_control_gpio(&player_controller_properties,5);
    uCanvas_set_gravity     (&player_controller_properties  ,-3.2);
    uCanvas_set_jump_height (&player_controller_properties  ,13.4);
    uCanvas_set_floor_level (&player_controller_properties  ,20);
    uCanvas_Animation_task_handle_t task_handler1 = uCanvas_attach_type1_controller_script(sprite1,&player_controller_properties);

    // static controller_properties_t player_controller_properties2;
    // uCanvas_add_control_gpio(&player_controller_properties2,4);
    // uCanvas_set_gravity     (&player_controller_properties2  ,-3.2);
    // uCanvas_set_jump_height (&player_controller_properties2  ,13.4);
    // uCanvas_set_floor_level (&player_controller_properties2  ,40);
    // uCanvas_Animation_task_handle_t task_handler2 = uCanvas_attach_type1_controller_script(sprite2,&player_controller_properties2);

    // detach_type1_controller_script(task_handler1);
    // detach_type1_controller_script(task_handler2);
    // sprite2->properties.visiblity = INVISIBLE;
    // sprite1->properties.visiblity = INVISIBLE;
    uCanvas_Add_Task(cloud_animation,NULL);
    uCanvas_Add_Task(car_animation,sprite1);
    int toggle = 0;
     while (1)
    {
        // toggle ? uCanvas_Set_Visiblity(circle,INVISIBLE) :
        //        uCanvas_Set_Visiblity(circle,VISIBLE);
        
        for (int i = 0; i < 13; i++)
        {
            if(map_features[i]->properties.position.x != -9){
                map_features[i]->properties.position.x--;
            }
            else{ 
                int r = get_random_number(0,20);
                if(r>5 && r < 10){
                    uCanvas_Change_Sprite_Source(map_features[i],grass_obj);
                    map_features[i]->properties.position.x = get_random_number(128,700);
                    map_features[i]->properties.position.y = get_random_number(26,64);
                }
                else if(r>10 && r <20){
                    if(get_random_number(0,1)==1){
                        uCanvas_Change_Sprite_Source(map_features[i],house_obj);
                        map_features[i]->properties.position.x = get_random_number(128,700);
                        map_features[i]->properties.position.y = 23-8;
                    }
                    else {
                        uCanvas_Change_Sprite_Source(map_features[i],building_obj);
                        map_features[i]->properties.position.x = get_random_number(128,700);
                        map_features[i]->properties.position.y = 23-16;
                    }
                    
                }
                else {
                    uCanvas_Change_Sprite_Source(map_features[i],tree_obj);
                    map_features[i]->properties.position.x = get_random_number(128,700);
                    map_features[i]->properties.position.y = get_random_number(26,35);
                }
                
            }
            
        }

        // for (int i = 0; i < 15; i++)
        // {
        //     if(fences[i]->properties.position.x != -1){
        //         fences[i]->properties.position.x--;
        //     }
        //     else fences[i]->properties.position.x = (i*9)+128;
        // }
        

        
        uCanvas_Delay(5);
        
        
     }
    
    // space_explorer_demo_loop();
    // noober_man_demo_loop();    
}