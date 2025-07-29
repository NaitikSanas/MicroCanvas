#include "app.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "sprite_collection.h"
#include "uCanvas2D_ST7789_Port.h"
#include "uCanvas2D_EK79007Port.h"
#include "uCanvasRenderEngine.h"
sprite2D_t logo;
#define CANVAS_HEIGHT       240
#define CANVAS_WIDTH        320
static uCanvas2D_Instance_t* uCanvas_Instance_1 = NULL;
static uCanvas2D_Instance_t* uCanvas_Instance_2 = NULL;
uCanvas_universal_obj_t* bg_spr_obj[CANVAS_WIDTH/TILE000_WIDTH * CANVAS_HEIGHT/TILE000_HEIGHT];
sprite2D_t bg_grass; //Creatte sprite background buffer based of canvas height and width
void uCanvas_Setup() {
  
    uCanvas_Scene_t* scene1 = New_uCanvas_Scene();
    uCanvas_Scene_t* scene2 = New_uCanvas_Scene();
    // uCanvas_Instance_1 = New_uCanvas_Instance(scene1, uCanvas2D_Get_Panel_Driver_EK79007(),NULL);
    uCanvas_Instance_1 = New_uCanvas_Instance(scene1, uCanvas2D_Get_Panel_Driver_ST7789(),uCanvas2D_Get_Panel_Driver_EK79007());
    uCanvas_Instance_1->render_buffer->use_ppa = false;
    uCanvas_Change_Active_Instance(uCanvas_Instance_1);
   

    
    // uCanvas_Change_Active_Instance(uCanvas_Instance_2);

    sprite2D_t bg_spr;
    sprite2D_t backgroud;

    backgroud.sprite_buf = (uint16_t*)malloc(CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(uint16_t));
    if (backgroud.sprite_buf == NULL) {
        printf("Failed to allocate memory for background sprite buffer\r\n");
        return;
    }
    backgroud.width = CANVAS_WIDTH;
    backgroud.height = CANVAS_HEIGHT;
    backgroud.color_format = SPRITE2D_COLOR_RGBA565;

    //create grass sprite backgroud of canvas height and width using Grass sprite
    uint16_t* bg_spr_ptr = backgroud.sprite_buf;
    for(int i = 0; i < CANVAS_WIDTH * CANVAS_HEIGHT; i++)
    {
        bg_spr_ptr[i]= Grass[i % (TX_TILESET_GRASS_WIDTH * TX_TILESET_GRASS_HEIGHT)];
    }

    uCanvas_universal_obj_t* main_bg_obj = New_uCanvas_2DSprite(&backgroud, 0, 0);
    

    uCanvas_universal_obj_t* c = New_uCanvas_2DCircle(0,240/2,10);
    uCanvas_universal_obj_t* line1 = New_uCanvas_2DLine(0,0,1024,0);
    uCanvas_universal_obj_t* line2 = New_uCanvas_2DLine(1024,0,1024,600);
    uCanvas_universal_obj_t* line3 = New_uCanvas_2DLine(1024,600,0,600);
    uCanvas_universal_obj_t* line4 = New_uCanvas_2DLine(0,600,0,0);

    uCanvas_Set_Position(line1,0,0);
    uCanvas_Set_Position(line2,0,0);
    uCanvas_Set_Position(line3,0,0);
    uCanvas_Set_Position(line4,0,0);
    uCanvas_Set_Color(line1,255,255,0);
    uCanvas_Set_Color(line2,255,255,0);
    uCanvas_Set_Color(line3,255,255,0);
    uCanvas_Set_Color(line4,255,255,0);

    uCanvas_universal_obj_t* r = New_uCanvas_2DRectangle(0,240/2,20,20);



    uCanvas_Set_Color(c,255,0,0);
    uCanvas_Set_Color(r,0,255,0);
    c->properties.fill = FILL;
    int posx = 0;
    while (1)
    {
        if (posx < CANVAS_WIDTH)posx += 1;
        else posx = 0;

        c->properties.position.x = posx;
        r->properties.position.x = posx;
        uCanvas_Delay(pdMS_TO_TICKS(20));
    }
    // pause_uCanvas_engine();
}   

void uCanvas_App_Main(void) {
    
   
}