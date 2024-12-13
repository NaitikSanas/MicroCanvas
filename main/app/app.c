#include "app.h"
#include "uCanvas_api.h"
#include "uCanvas_display_port.h"

void uCanvas_Setup() {
    start_uCanvas_engine();
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);

}

void uCanvas_App_Main(void) {

    
    
}