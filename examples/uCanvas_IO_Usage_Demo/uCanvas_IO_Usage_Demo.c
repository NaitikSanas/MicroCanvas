#include "uCanvas_api.h"
#include "uCanvas_User_IO.h"
#include "uCanvas_IO_Usage_Demo.h"

#define ENC_A   39 
#define ENC_B   40 
#define ENC_SW  37

void Run_uCanvas_IO_Usage_Demo() {
    start_uCanvas_engine();

    //SetUp IMU Device
    uCanvas_Initialize_IMU_Device(42,41);
    uCanvas_IMU_Set_Tilt_Detection_Parameters(10,6);

    //SetUp Rotart Encoder Instance
    rotary_encoder_t rotary_encoder_1;
    uCanvas_rotary_encoder_init(&rotary_encoder_1,ENC_A,ENC_B,ENC_SW);

    //Create new 2D Scene
    uCanvas_Scene_t* scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);

    uCanvas_universal_obj_t* o = New_uCanvas_2DCircle(320/2, 240/2,30);
    o->properties.fill = FILL;
    uCanvas_Set_Color(o,255,255,0);

    uCanvas_universal_obj_t* textbox1 = New_uCanvas_2DTextbox("",30,30);
    uCanvas_Set_Color(textbox1,0,255,0);
    textbox1->font_properties.font_type = FONT_16M;

    uCanvas_universal_obj_t* textbox2 = New_uCanvas_2DTextbox("",30,60);
    uCanvas_Set_Color(textbox2,0,255,0);
    textbox2->font_properties.font_type = FONT_16M;
    
    uint8_t mode = 0;
    char buf[32];
    while (1)
    {
        sprintf(buf,"POS:(%d,%d)",o->properties.position.x,o->properties.position.y);
        uCanvas_Set_Text(textbox2,buf);

        // Control Circle through Encoder 
        uCanvas_rotary_encoder_read(&rotary_encoder_1);
        if(!rotary_encoder_1.sw_state){
            mode = !mode;
        }

        switch (rotary_encoder_1.state)
        {
        case ENCODER_CW:
            if(mode)
                o->properties.position.x += 1;
            else
                o->properties.position.y += 1;
            break;
        case ENCODER_CCW:
            if(mode)
                o->properties.position.x -= 1;
            else
                o->properties.position.y -= 1;
            break;
        default:
            break;
        }

        //Control Circle through IMU.
        tilt_dir_t a = uCanvas_Get_IMU_2D_Tilt();
        switch (a)
        {
            case TILT_LEFT:
                uCanvas_Set_Text(textbox1,"TILT_LEFT");
                if(o->properties.position.x < 320) 
                    o->properties.position.x += 1;
                break;

            case TILT_RIGHT:
                uCanvas_Set_Text(textbox1,"TILT_RIGHT");
                if(o->properties.position.x > 0) 
                    o->properties.position.x -= 1;
                break;

            case TILT_UP :
                uCanvas_Set_Text(textbox1,"TILT_UP");
                if(o->properties.position.y > 0)     
                    o->properties.position.y -= 1;
                break;

            case TILT_DOWN :
                uCanvas_Set_Text(textbox1,"TILT_DOWN");
                if(o->properties.position.y < 240) 
                    o->properties.position.y += 1;
                break;
                
            default:
                uCanvas_Set_Text(textbox1,"NO_TILT");
                break;
        
        }
        uCanvas_Delay(5);
    }   
}
