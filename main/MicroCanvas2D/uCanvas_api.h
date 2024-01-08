#ifndef uCanvas_api_H_ 
    #define uCanvas_api_H_
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/semphr.h"
    #include "uCanvasDataTypes.h"

    SemaphoreHandle_t active_scene_mutex;
    #define uCanvas_Universal_Object (uCanvas_universal_obj_t*)malloc(sizeof(uCanvas_universal_obj_t))
    #define uCanvas_Scene_Object  (uCanvas_Scene_t*)malloc(sizeof(uCanvas_Scene_t)) 
    #define LOCK_ACTIVE_SCENEB_BUF xSemaphoreTake(active_scene_mutex,portMAX_DELAY)
    #define UNLOCK_ACTIVE_SCENEB_BUF xSemaphoreGive(active_scene_mutex);

    void start_uCanvas_engine(void);
    
    /**
     * APIs for controlling Scenes
    */
    uCanvas_Scene_t* create_scene(void);
    void uCanvas_set_active_scene(uCanvas_Scene_t* scene);
    
    /**
     * API for creating 2D Shapes and Objects
    */
    uCanvas_universal_obj_t* create_rectangle(uint16_t xpos, uint16_t ypos, uint16_t h, uint16_t w);
    uCanvas_universal_obj_t* create_text_box(char* text, uint16_t xpos, uint16_t ypos);
    
    /**
     * Properties Control API
    */
    void uCanvas_Set_Text(uCanvas_universal_obj_t*obj,char*text);
    void uCanvas_Set_Obj_Type(uCanvas_universal_obj_t*obj,uCanvas_element_type_t type);
    void uCanvas_Set_Color(uCanvas_universal_obj_t* obj, uint16_t r,uint16_t g, uint16_t b);
    void uCanvas_Set_Monochrome_Color(uCanvas_universal_obj_t* obj, uint16_t color );
    void uCanvas_Set_Position(uCanvas_universal_obj_t* obj, uint16_t xpos,uint16_t ypos);
    void uCanvas_Set_Width_Height(uCanvas_universal_obj_t* obj, uint16_t width,uint16_t height);
    void uCanvas_Set_Fill(uCanvas_universal_obj_t* obj, fill_t fill);
    void uCanvas_Set_Visiblity(uCanvas_universal_obj_t* obj, visibility_ctrl_t vctrl);
#endif