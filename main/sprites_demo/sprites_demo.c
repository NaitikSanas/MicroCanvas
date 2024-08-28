#include "sprites_demo.h"
#include "sprite_collection.h"

static uCanvas_Scene_t* sprite_demo_scene;

uCanvas_universal_obj_t* clouds1[6];
uCanvas_universal_obj_t* clouds2[6];

    
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
static uint8_t car_scaled_sprite            [(16* car_scale) * (8* car_scale)]; //buffer to store scaled up sprite
static uint8_t building_scaled_sprite       [(9* car_scale) * (16* car_scale)]; //buffer to store scaled up sprite



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

/**
 * task to generate clouds at random positions. 
 */            
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


/**
 * Scene elements holds randomly generated elments like trees, buildings, grass, mountains sprites. 
*/
uCanvas_universal_obj_t* scene_elements[13];
/**
 * Task to generate random stuff on the horizon.
 */
void generate_horizon(void){
    while (1)
    {
        for (int i = 0; i < 13; i++)
        {
            if(scene_elements[i]->properties.position.x != -9){
                scene_elements[i]->properties.position.x--;
            }
            else{ 
                int r = get_random_number(0,20);
                if(r>5 && r < 10){
                    uCanvas_Change_Sprite_Source(scene_elements[i],grass_obj);
                    scene_elements[i]->properties.position.x = get_random_number(128,700);
                    scene_elements[i]->properties.position.y = get_random_number(26,64);
                }
                else if(r>10 && r <20){
                    if(get_random_number(0,1)==1){
                        uCanvas_Change_Sprite_Source(scene_elements[i],house_obj);
                        scene_elements[i]->properties.position.x = get_random_number(128,700);
                        scene_elements[i]->properties.position.y = 23-8;
                    }
                    else {
                        uCanvas_Change_Sprite_Source(scene_elements[i],building_obj);
                        scene_elements[i]->properties.position.x = get_random_number(128,700);
                        scene_elements[i]->properties.position.y = 23-16;
                    }
                    
                }
                else {
                    uCanvas_Change_Sprite_Source(scene_elements[i],tree_obj);
                    scene_elements[i]->properties.position.x = get_random_number(128,700);
                    scene_elements[i]->properties.position.y = get_random_number(26,35);
                }
                
            }
            
        }     
        uCanvas_Delay(1);     
    }
}

/** 
 * Animates Car sprite by change in Y movement. 
 */
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


void sprite_demo(){
    start_uCanvas_engine();   
    sprite_demo_scene = New_uCanvas_Scene(); /* Create New Scene instance */
    uCanvas_set_active_scene(sprite_demo_scene); /*Set Newly create scene as Active scene to render*/

    /**
     * Scale UP 9x9 sprite to 27 x 27 
     * */ 
    uCanvas_ScaleUp_SpriteBuf(happy_face_sprite, happy_face_scaled_sprite        , 9, 9,scale);
    uCanvas_ScaleUp_SpriteBuf(surprised_face_sprite, surprised_face_scaled_sprite, 9, 9,scale);
    uCanvas_ScaleUp_SpriteBuf(neutral_face_sprite, neutral_face_scaled_sprite    , 9, 9,scale);
    uCanvas_ScaleUp_SpriteBuf(sad_face_sprite, sad_face_scaled_sprite            , 9, 9,scale);
    uCanvas_ScaleUp_SpriteBuf(tree_sprite, tree_scaled_sprite                    , 9, 9,scale2);
    uCanvas_ScaleUp_SpriteBuf(car_sprite, car_scaled_sprite                    , 16, 8,car_scale);
    uCanvas_ScaleUp_SpriteBuf(building_sprite, building_scaled_sprite          , 9, 16,car_scale);

    /**
     * This API creates sprite object from defined sprite buffers and stores context of hight, width, orientation etc. 
     * */ 
    uCanvas_Compose_2DSprite_Obj(&happy_face_scaled_obj     ,happy_face_scaled_sprite    , 9*scale,9*scale);
    uCanvas_Compose_2DSprite_Obj(&surprised_face_scaled_obj ,surprised_face_scaled_sprite, 9*scale,9*scale);
    uCanvas_Compose_2DSprite_Obj(&neutral_face_scaled_obj   ,neutral_face_scaled_sprite  , 9*scale,9*scale);
    uCanvas_Compose_2DSprite_Obj(&sad_face_scaled_obj       ,sad_face_scaled_sprite      , 9*scale,9*scale);
    uCanvas_Compose_2DSprite_Obj(&tree_scaled_obj           ,tree_scaled_sprite          , 9*scale2,9*scale2);
    uCanvas_Compose_2DSprite_Obj(&tree_obj                  ,tree_sprite                 , 9,9);
    uCanvas_Compose_2DSprite_Obj(&grid_obj                  ,grid_sprite                 , 9,9);
    uCanvas_Compose_2DSprite_Obj(&grid_obj2                 ,grid_sprite2                , 9,6);
    uCanvas_Compose_2DSprite_Obj(&car_obj1                  ,car_sprite                  , 16,8);
    uCanvas_Compose_2DSprite_Obj(&car_obj2                  ,car_scaled_sprite           , 16*car_scale,8*car_scale);
    uCanvas_Compose_2DSprite_Obj(&grass_obj                 ,grass_sprite                , 9,9);
    uCanvas_Compose_2DSprite_Obj(&mountain_obj              ,mountain_sprite             , 16,8);
    uCanvas_Compose_2DSprite_Obj(&building_obj              ,building_sprite             , 9,16);
    uCanvas_Compose_2DSprite_Obj(&house_obj                 ,house_sprite                , 16,8);


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

    static uCanvas_universal_obj_t* sprite1;
    static uCanvas_universal_obj_t* sprite2;

    sprite1 = New_uCanvas_2DSprite(car_obj2     ,4,12);

    //Attach TYPE 1 controller to sprite 1
    static controller_properties_t player_controller_properties;
    uCanvas_add_control_gpio(&player_controller_properties,5);
    uCanvas_set_gravity     (&player_controller_properties  ,-3.2);
    uCanvas_set_jump_height (&player_controller_properties  ,13.4);
    uCanvas_set_floor_level (&player_controller_properties  ,20);
    uCanvas_Animation_task_handle_t task_handler1 = uCanvas_attach_type1_controller_script(sprite1,&player_controller_properties);
    
    uCanvas_Add_Task(cloud_animation,NULL, NULL);
    uCanvas_Add_Task(car_animation,sprite1,NULL);
    int toggle = 0;

   
    for (int i = 0; i < 13; i++)
    {
        scene_elements[i] = New_uCanvas_2DSprite(tree_obj,i*10,32);
    }
    
    uCanvas_Add_Task(generate_horizon,NULL,0);
    
  
}