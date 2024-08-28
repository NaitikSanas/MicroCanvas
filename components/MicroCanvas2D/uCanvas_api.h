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

    /*Starts Rendering Engine and Initializes Display*/
    void start_uCanvas_engine(void);
    
    /**
     * APIs to creating uCanvas related Tasks to work with Scene objects, 
     * creating Animations, handling User Input etc.
    */

    /**
     * Creates Independent Custom thread of Passed Loop function
    */
    uCanvas_Animation_task_handle_t uCanvas_Add_Task(uCanvas_Animation_task_t animation_loop, void* arg, int core_id);
    
    /* Pauses uCanvas Custom User tasks */
    void uCanvas_Pause_Task(uCanvas_Animation_task_handle_t task_handle);
    void uCanvas_Resume_Task(uCanvas_Animation_task_handle_t task_handle);
    /* Adds Delay in MS */
    void uCanvas_Delay(uint16_t delay);

    /* Delets uCanvas Custom User Task */
    void uCanvas_Remove_Task(uCanvas_Animation_task_handle_t handle);

    /**
     * APIs for controlling Scenes
    */

    /* Creates New 2D Scene instance */
    uCanvas_Scene_t* New_uCanvas_Scene(void);
    /* Sets Passed 2D instance as Active Scene to Render */
    void uCanvas_set_active_scene(uCanvas_Scene_t* scene);
    
    /**
     * API for creating 2D Shapes and Objects
    */
    uCanvas_universal_obj_t* New_uCanvas_2DRectangle(uint16_t xpos, uint16_t ypos, uint16_t h, uint16_t w);
    uCanvas_universal_obj_t* New_uCanvas_2DTextbox(char* text, uint16_t xpos, uint16_t ypos);
    uCanvas_universal_obj_t* New_uCanvas_2DCircle(uint16_t xpos, uint16_t ypos,uint16_t radius);
    uCanvas_universal_obj_t* New_uCanvas_2DLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    uCanvas_universal_obj_t* New_uCanvas_2DTriangle(Coordinate2D_t Point1, Coordinate2D_t Point2, Coordinate2D_t Point3);
    /**
     * @brief api creates Sprite Element on currently active scene and forwards to the render engine.
     *  sprite_element(properties) - 
     *       > sprite2D_t       - Sprite Object 
     *       > pos_x            - Absolute Sprite Position X      
     *       > pos_y            - Absolute Sprite Position Y
     * @param sprite2D_obj : reference to 2D Sprite object to add into scene
     * @param pos_x : absolute x axis position on active scene
     * @param pos_y : absolute y axis position on active scene
     */
    uCanvas_universal_obj_t* New_uCanvas_2DSprite(sprite2D_t sprite2D_obj,uint16_t pos_x, uint16_t pos_y);
    
    /**
     * @brief This API let's user scale up their custom 2D sprite Buffers. 
     * @param src           : sprite to be scaled
     * @param des           : location where to store scaled sprite
     * @param src_width     : actual width
     * @param src_height    : actual height
     * @param scale_factor  : Scale factor to scale up the sprite
     */
    void uCanvas_ScaleUp_SpriteBuf(uint8_t* src, uint8_t* dest, int src_width, int src_height, int scale_factor);
    void uCanvas_ScaleUp_Sprite2D(sprite2D_t* sprite_obj,uint8_t* reference,uint8_t* buffer, int h, int w, int scale_factor);
    /**
     * @brief This API dynamically changes the 2D sprite Object source to render on scene.
     * @param obj           : reference to sprite element
     * @param sprite_obj    : reference to new sprite object to replace with
     */
    void uCanvas_Change_Sprite_Source(uCanvas_universal_obj_t* obj, sprite2D_t sprite_obj);
    
    /**
     * @paragraph
     * This API Creates 2D Sprite Object that packages the sprite buffer, width, height and orientation
     * Create 2D Sprite Object based on raw Sprite buffer so we can use it easily in application
     *  sprite2D_t (properties) -
     *      > Sprite_buffer - Pointes to 1D buffer storing the pixel values
     *      > Sprite_Height - Actual Height of sprite buffer (CHANGING THIS DOES NOT SCALES THE SPRITES)
     *      > Sprite_Width  - Actual Width  of sprite buffer (CHANGING THIS DOES NOT SCALES THE SPRITES)
     *      > Sprite_Orientation
     * 
     * @param  obj : reference to object variable
     * @param  sprite_buffer : reference to sprite_buffer
     * @param  width : actual width of sprite_buffer content
     * @param  height : actual width of height content
     */
    void uCanvas_Compose_2DSprite_Obj(sprite2D_t* obj, uint8_t* sprite_buffer,uint16_t width, uint16_t height);
    
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

    int get_random_number(int min, int max);
    
    void uCanvas_Delete_obj_from_scene(uCanvas_universal_obj_t* obj);
#endif