#include "app.h"
#include "uCanvas_api.h"
#include "ucanvas_slider.h"
#include "uCanvas_User_IO.h"
#include "sprite_collection.h"
#include "websocket_server.h"
#include "uCanvas_api.h"
#include <math.h>
#include "3D_Cube.h"
#define CUBE_SIZE 20
#define SCREEN_CENTER_X 240/2
#define SCREEN_CENTER_Y 320/2
int DEPTH = 30;  // Adjust for perspective effect

uCanvas_universal_obj_t* line2D[12];

void Render_3D_Cube( float angle) {
    float vertices[8][3] = {
        {-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE},
        { CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE},
        { CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE},
        {-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE},
        {-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE},
        { CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE},
        { CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE},
        {-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE}
    };
    uCanvas_Init_PushButton(0);
    int projected[8][2];
    int state = 0;
    while(1){
        // if(angle < 1)
        // else angle = 0;
        if(!uCanvas_Get_PushbuttonState_WTR(0)){
            DEPTH+=5;
        }
 
        angle += 0.009;
        // printf("angle, depth %f, %d\r\n",angle,DEPTH);
        for (int i = 0; i < 8; i++) {
            // Rotate around Y-axis
            float y = vertices[i][0] * cos(angle) - vertices[i][2] * sin(angle)+ DEPTH; 
            float x = vertices[i][0] * sin(angle) + vertices[i][2] * cos(angle) ;
            float z = vertices[i][1];
            
            // Perspective projection
            projected[i][0] = SCREEN_CENTER_X + (int)(x * 40 / z);
            projected[i][1] = SCREEN_CENTER_Y + (int)(y * 40 / z);
        }
        
        // Connect edges
        int edges[12][2] = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}
        };
        
        int triangle_faces[12][3] = {
            {0, 1, 2}, {2, 3, 0},  // Front face
            {1, 5, 6}, {6, 2, 1},  // Right face
            {5, 4, 7}, {7, 6, 5},  // Back face
            {4, 0, 3}, {3, 7, 4},  // Left face
            {3, 2, 6}, {6, 7, 3},  // Top face
            {4, 5, 1}, {1, 0, 4}   // Bottom face
        };

        // for (int i = 0; i < 12; i++) {
        //     line2D[i]->point1.x =  projected[triangle_faces[i][0]][0];
        //     line2D[i]->point1.y =  projected[triangle_faces[i][0]][1];
        //     line2D[i]->point2.x =  projected[triangle_faces[i][1]][0]; 
        //     line2D[i]->point2.y =  projected[triangle_faces[i][1]][1];
        //     line2D[i]->point3.x =  projected[triangle_faces[i][2]][0]; 
        //     line2D[i]->point3.y =  projected[triangle_faces[i][2]][1];
        // }

        for (int i = 0; i < 12; i++) {
            uCanvas_Set_Line_Coordinates(line2D[i], projected[edges[i][0]][0], projected[edges[i][0]][1],
                                        projected[edges[i][1]][0], projected[edges[i][1]][1]);
        }

        uCanvas_Delay(4);
    }
}

void Run_3D_Cube_Demo() {
    
    uCanvas_Scene_t* scene;
    start_uCanvas_engine();
    scene = New_uCanvas_Scene();
    uCanvas_set_active_scene(scene);
    // uCanvas_universal_obj_t* bg = New_uCanvas_2DRectangle(0,0,320,240);
    // uCanvas_Set_Color(bg,255,255,255);
    // bg->properties.fill = FILL;
    for (int i = 0; i < 12; i++)
    {
        Coordinate2D_t c = {0,0};
        // line2D[i] = New_uCanvas_2DTriangle( c,c,c);
        line2D[i] = New_uCanvas_2DLine(0,0,0,0);
        uCanvas_Set_Color(line2D[i],255,255,0);
        line2D[i]->properties.fill = NOFILL;
        
    }

    uCanvas_Set_Color(line2D[0],255,0,0);
    uCanvas_Set_Color(line2D[1],0,255,0);
    uCanvas_Set_Color(line2D[2],0,0,255);
    uCanvas_Set_Color(line2D[3],255,255,0);
    uCanvas_Set_Color(line2D[4],255,0,255);
    uCanvas_Set_Color(line2D[6],255,255,255);


    
    Render_3D_Cube(0.5);
}
