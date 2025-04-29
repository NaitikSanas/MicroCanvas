#include "app.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "sprite_collection.h"
#include "uCanvas_display_port.h"

void uCanvas_Setup() {
    printf("--App.c\r\n");
    start_uCanvas_engine();
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    printf("--App.c\r\n");
    uCanvas_universal_obj_t* o = New_uCanvas_2DRectangle(0,240/2,20,20);
    uCanvas_Set_Color(o,255,0,0);
    o->properties.fill = FILL;
    printf("--App.c\r\n");
    sprite2D_t sp;
    uCanvas_Compose_2DSprite_Obj(&sp,run_2,32,32);
    uCanvas_universal_obj_t* spu = New_uCanvas_2DSprite(&sp,320/2, 240/2);
    printf("--App.c\r\n");
    
    printf("--App.c\r\n");
    uCanvas_universal_obj_t* txt = New_uCanvas_2DTextbox("test",0,240/2);
    uCanvas_Set_Color(txt,255,0,0);
    txt->font_properties.font_type = FONT_16G;
    printf("--App.c\r\n");
    while (1)
    {
        if(o->properties.position.x < 320)o->properties.position.x++;
        else o->properties.position.x = 0;
        uCanvas_Delay(20);
    }
    
}   
 
void uCanvas_App_Main(void) {

}