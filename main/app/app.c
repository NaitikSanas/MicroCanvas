#include "app.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "sprite_collection.h"
sprite2D_t logo;
#define CANVAS_HEIGHT       240
#define CANVAS_WIDTH        320
void uCanvas_Setup() {
    start_uCanvas_engine();   
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    // pause_uCanvas_engine();
}   

void uCanvas_App_Main(void) {
    uCanvas_universal_obj_t* c = New_uCanvas_2DCircle(0,240/2,10);
    uCanvas_Set_Color(c,255,0,0);
    c->properties.fill = FILL;
    int posx = 0;
    while (1)
    {
        if (posx < 320)posx += 1;
        else posx = 0;

        // c->properties.position.x = posx;

        uCanvas_Set_Position(c,posx,c->properties.position.y);
        // uCanvas_manually_render_scene();
        uCanvas_Delay(6);
    }
}