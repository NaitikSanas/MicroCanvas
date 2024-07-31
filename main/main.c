#include "main.h"
#include "noob_man.h"
#include "space_explorer.h"
#include "uCanvas_Physix.h" 
    static uint8_t happy_face_sprite[9*9] = {   \
        0,1,1,1,1,1,1,1,0,                    \
        1,0,0,0,0,0,0,0,1,                    \
        1,0,1,1,0,1,1,0,1,                    \
        1,0,1,1,0,1,1,0,1,                    \
        1,0,0,0,0,0,0,0,1,                    \
        1,0,1,0,0,0,1,0,1,                    \
        1,0,0,1,1,1,0,0,1,                    \
        1,0,0,0,0,0,0,0,1,                    \
        0,1,1,1,1,1,1,1,0  
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




void app_main(){
    start_uCanvas_engine(); /* Start uCanvas engine */
    
    uCanvas_Scene_t* main_scene = New_uCanvas_Scene(); /* Create New Scene instance */
    uCanvas_set_active_scene(main_scene); /*Set Newly create scene as Active scene to render*/
    New_uCanvas_2DCircle(64,32,20);
    uCanvas_Delay(10);
    
    /**
     * Example of scaling up the 2D sprites.
     */
    #define scale 2 //sets scaling level of sprite
    static uint8_t happy_face_scaled_sprite     [(9* scale) * (9* scale)]; //buffer to store scaled up sprite
    static uint8_t surprised_face_scaled_sprite [(9* scale) * (9* scale)]; //buffer to store scaled up sprite
    static uint8_t neutral_face_scaled_sprite   [(9* scale) * (9* scale)]; //buffer to store scaled up sprite
    static uint8_t sad_face_scaled_sprite       [(9* scale) * (9* scale)]; //buffer to store scaled up sprite
    
    /**
     * Scales 9x9 sprite to 27 x 27 
     * */ 
    uCanvas_ScaleUp_Sprite2D(happy_face_sprite, happy_face_scaled_sprite        , 9, 9,scale);
    uCanvas_ScaleUp_Sprite2D(surprised_face_sprite, surprised_face_scaled_sprite, 9, 9,scale);
    uCanvas_ScaleUp_Sprite2D(neutral_face_sprite, neutral_face_scaled_sprite    , 9, 9,scale);
    uCanvas_ScaleUp_Sprite2D(sad_face_sprite, sad_face_scaled_sprite            , 9, 9,scale);

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
               neutral_face_scaled_obj;
    
    uCanvas_Compose_2DSprite_Obj(&happy_face_scaled_obj     ,happy_face_scaled_sprite    , 9*scale,9*scale);
    uCanvas_Compose_2DSprite_Obj(&surprised_face_scaled_obj ,surprised_face_scaled_sprite, 9*scale,9*scale);
    uCanvas_Compose_2DSprite_Obj(&neutral_face_scaled_obj   ,neutral_face_scaled_sprite  , 9*scale,9*scale);
    uCanvas_Compose_2DSprite_Obj(&sad_face_scaled_obj       ,sad_face_scaled_sprite      , 9*scale,9*scale);


    /**
     * This api creates Sprite Element on currently active scene and forwards to the render engine.
     *  sprite_element(properties) - 
     *       > sprite2D_t       - Sprite Object 
     *       > pos_x            - Absolute Sprite Position X      
     *       > pos_y            - Absolute Sprite Position Y
     */
    uCanvas_universal_obj_t* sprite1 = New_uCanvas_2DSprite(happy_face_scaled_obj     ,20,45);
    uCanvas_universal_obj_t* sprite2 = New_uCanvas_2DSprite(happy_face_scaled_obj     ,50,45);
    
    //Attach TYPE 1 controller to sprite 1
    static controller_properties_t player_controller_properties;
    uCanvas_add_control_gpio(&player_controller_properties,5);
    uCanvas_set_gravity     (&player_controller_properties  ,-3.2);
    uCanvas_set_jump_height (&player_controller_properties  ,18.6);
    uCanvas_set_floor_level (&player_controller_properties  ,45);
    uCanvas_attach_type1_controller_script(sprite1,&player_controller_properties);

    static controller_properties_t player_controller_properties2;
    uCanvas_add_control_gpio(&player_controller_properties2,4);
    uCanvas_set_gravity     (&player_controller_properties2  ,-3.2);
    uCanvas_set_jump_height (&player_controller_properties2  ,18.6);
    uCanvas_set_floor_level (&player_controller_properties2  ,45);
    uCanvas_attach_type1_controller_script(sprite2,&player_controller_properties2);


    
    
    // space_explorer_demo_loop();
    // noober_man_demo_loop();    
}