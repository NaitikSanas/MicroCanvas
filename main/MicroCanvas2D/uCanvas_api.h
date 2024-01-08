#ifndef uCanvas_api_H_ 
    #define uCanvas_api_H_
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/semphr.h"
    #include "uCanvasDataTypes.h"

    SemaphoreHandle_t active_scene_mutex;
    #define uCanvas_Universal_Object    (uCanvas_universal_obj_t*)malloc(sizeof(uCanvas_universal_obj_t))
    #define uCanvas_Scene_Object        (uCanvas_Scene_t*)malloc(sizeof(uCanvas_Scene_t)) 
    #define LOCK_ACTIVE_SCENEB_BUF      xSemaphoreTake(active_scene_mutex,portMAX_DELAY)
    #define UNLOCK_ACTIVE_SCENEB_BUF    xSemaphoreGive(active_scene_mutex);

    void start_uCanvas_engine(void);
    
    /**
     * APIs to creating uCanvas related Tasks to work with Scene objects, 
     * creating Animations, handling User Input etc.
    */
    uCanvas_Animation_task_handle_t uCanvas_Add_Task(uCanvas_Animation_task_t animation_loop);
    void uCanvas_Pause_Task(uCanvas_Animation_task_handle_t task_handle);
    void uCanvas_Delay(uint16_t delay);
    void uCanvas_Remove_Task(uCanvas_Animation_task_handle_t handle);

    /**
     * APIs for controlling Scenes
    */
    uCanvas_Scene_t* New_uCanvas_Scene(void);
    void uCanvas_set_active_scene(uCanvas_Scene_t* scene);
    
    /**
     * API for creating 2D Shapes and Objects
    */
    uCanvas_universal_obj_t* New_uCanvas_2DRectangle(uint16_t xpos, uint16_t ypos, uint16_t h, uint16_t w);
    uCanvas_universal_obj_t* New_uCanvas_2DTextbox(char* text, uint16_t xpos, uint16_t ypos);
    uCanvas_universal_obj_t* New_uCanvas_2DCircle(uint16_t xpos, uint16_t ypos,uint16_t radius);
    uCanvas_universal_obj_t* New_uCanvas_2DLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    
    
    /**
     * Properties Control API
    */
    /* Animates TextBox with Reaveal style animation */
    void uCanvas_Animate_Text_Reveal(uCanvas_universal_obj_t*obj, char* text, uint16_t delay);
    
    /* Sets content of Passed Textbox Object */
    void uCanvas_Set_Text(uCanvas_universal_obj_t*obj,char*text);
    
    /* Sets the type of passed 2D Object */
    void uCanvas_Set_Obj_Type(uCanvas_universal_obj_t*obj,uCanvas_element_type_t type);
    
    /* Sets color of Passed 2D Object */
    void uCanvas_Set_Color(uCanvas_universal_obj_t* obj, uint16_t r,uint16_t g, uint16_t b);
    
    /* Sets Monochrome pixels values of Passed 2D Object */
    void uCanvas_Set_Monochrome_Color(uCanvas_universal_obj_t* obj, uint16_t color );
    
    /* Sets absolute position of Passed 2D Object */
    void uCanvas_Set_Position(uCanvas_universal_obj_t* obj, uint16_t xpos,uint16_t ypos);
    
    /* Sets cordinates of 2D Line Object */
    void uCanvas_Set_Line_Coordinates(uCanvas_universal_obj_t*line,uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    
    /* Sets radius value of elliptical type of 2D Object */
    void uCanvas_Set_Radius(uCanvas_universal_obj_t* obj, uint16_t radius);
    
    /* Sets Height and Width of rectangular 2D Object */
    void uCanvas_Set_Width_Height(uCanvas_universal_obj_t* obj, uint16_t width,uint16_t height);
    
    /* Allows to Indicate Fill/NoFill insides of 2D  Object */
    void uCanvas_Set_Fill(uCanvas_universal_obj_t* obj, fill_t fill);
    
    /* Allows to Control Visiblity of Passed 2D  Object */
    void uCanvas_Set_Visiblity(uCanvas_universal_obj_t* obj, visibility_ctrl_t vctrl);   
#endif