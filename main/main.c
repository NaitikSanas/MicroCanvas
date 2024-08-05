#include "main.h"
#include "uCanvas_Physix.h" 
#include "sprite_collection.h"

/**
 * Scene object is composed of array of 2D Elements ARA(uCanvas_universal_obj_t). 
 * Any Element created by application will be appended to 
 * current active scene which then forwarded to Render engine
 * To read out element properties and draw on display accordingly.
 * 
 * having multiple scene object can allow application to switch between different
 * 2D Secenes without recreating all the time allowing fast transitions. 
 *  uCanvas_Scene_t ->
 *      > Array : uCanvas_universal_obj_t   // List of objects created in scene
 *      > int   : Write Pointer             // When object is appended in scene write pointer 
 *                                             is incremented by +1
 *      > int   : Scene Index               // Can be used as a ID
*/
static uCanvas_Scene_t* main_scene;

void ucanvas_setup(){
    start_uCanvas_engine();                 /* Start uCanvas engine */
    main_scene = New_uCanvas_Scene();       /* Create New Scene instance */
    uCanvas_set_active_scene(main_scene);   /*Set Newly create scene as Active scene to render*/
}

void ucanvas_app_setup(){
    uCanvas_universal_obj_t* textbox = New_uCanvas_2DTextbox("",10,28);

    New_uCanvas_2DRectangle(0,0,64,128);
    while (1)
    {
        uCanvas_Animate_Text_Reveal(textbox,"MicroCanvas Demo",10);
        uCanvas_Delay(100);
        memset(textbox->text,NULL,256);
        uCanvas_Delay(10);
        printf("----\r\n");
    }    
}

void app_main(){
    ucanvas_setup();
    ucanvas_app_setup();
}



