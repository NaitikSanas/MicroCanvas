#ifndef uCanvas_api_H_ 
    #define uCanvas_api_H_
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/semphr.h"
    #include "uCanvasDataTypes.h"
    #include "sdkconfig.h"
    #include "uCanvas2D_Display_Setup.h"
    #include "uCanvas2D_EK79007Port.h"
    #include "uCanvas2D_ST7789_Port.h"   
    #include "Panel_Selector.h"
    #define uCanvas_Universal_Object    (uCanvas_universal_obj_t*)malloc(sizeof(uCanvas_universal_obj_t))
    #define uCanvas_Scene_Object        (uCanvas_Scene_t*)malloc(sizeof(uCanvas_Scene_t)) 

    #define LOCK_RESOURCE(x)      xSemaphoreTake(x,portMAX_DELAY)
    #define UNLOCK_RESOURCE(x)    xSemaphoreGive(x);

    void uCanvas_lock_scene();
    void uCanvas_unlock_scene();
    // #define uCANVAS2D_EK79007_SETUP(x) New_uCanvas_Instance(x, uCanvas2D_Get_Panel_Driver_ST7789(),NULL)
    // #define uCANVAS2D_ST7789_SETUP(x) New_uCanvas_Instance(x, uCanvas2D_Get_Panel_Driver_ST7789(),NULL)
    
    /*Starts Rendering Engine and Initializes Display*/
    uCanvas2D_Instance_t* New_uCanvas_Instance(uCanvas_Scene_t* scene, uCanvas2D_Display_Panel_t* panel_1,uCanvas2D_Display_Panel_t* panel_2, int width, int height, int offset_x, int offset_y);
    uCanvas2D_Instance_t* New_uCanvas_Window_Instance(uCanvas_Scene_t* scene,int width, int height);
    void uCanvas_Attach_Renderer(uCanvas2D_Instance_t* instance, int core_id);
    void uCanvas_Attach_Panel(uCanvas2D_Instance_t* instance, uCanvas2D_Display_Panel_t* panel);
    int uCanvas_Attach_RenderBuffer(uCanvas2D_Instance_t* instance, int width, int height);
    void uCanvas_Set_ViewPort_Position(uCanvas2D_Instance_t* instance, int x, int y);
    void uCanvas_Attach_Scene(uCanvas2D_Instance_t* instance, uCanvas_Scene_t* scene);
    void uCanvas_Set_Panel_RefreshDelay(uCanvas2D_Instance_t* instance, uint16_t RefreshDelay);

    /**
     * @brief This API Sets the Render Mode of Target instance. 
     * There are 3 types of Render Modes :
     * 1. AUTO_REFRESH : This mode Refreshes Region of Display Assigned to Target Instance at set interval of time. 
     * 
     * 2. ASYNC_FRAME_QUEUED : When this mode is set, Renderer waits for refresh signal from the user application. 
     * In Queud Mode uCanvas_Send_Refresh_Signal_To_Renderer API becomes Non-Blocking. Suitable for Panels that uses DMA to push the rederbuffer.
     * 
     * 3. ASYNC_FRAME_COMMIT : Works the same way as ASYNC_FRAME_QUEUED but uCanvas_Send_Refresh_Signal_To_Renderer in this mode
     * waits for Display Refresh to complete. Suitable for Panels when DMA is not used for pushing the Render Buffer. 
     */
    void uCanvas_Set_Render_Mode(uCanvas2D_Instance_t* instance,uCanvas2D_Render_Mode_t mode);

    /**
     * @brief Sends Signal to renerer to update display region allocated to passed uCanvas Instance.
     */
    void uCanvas_Send_Refresh_Signal_To_Renderer(uCanvas2D_Instance_t* instance);

    /* uCanvas Instance Management */
    void uCanvas_Pause_Instance(uCanvas2D_Instance_t* instance);
    void uCanvas_Resume_Instance(uCanvas2D_Instance_t* instance);
    void uCanvas_Destroy_Instance(uCanvas2D_Instance_t* instance);
    
    int64_t uCanvas_Get_FPS(uCanvas2D_Instance_t* instance);
    /**

    void pause_uCanvas_engine(void);
    void resume_uCanvas_engine(void);

    
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
    void uCanvas_Change_Active_Instance(uCanvas2D_Instance_t* instance);
    /**
     * API for creating 2D Shapes and Objects
    */
    uCanvas_universal_obj_t* New_uCanvas_2DRectangle(uint16_t xpos, uint16_t ypos, uint16_t h, uint16_t w);
    uCanvas_universal_obj_t* New_uCanvas_2DTextbox(char* text, uint16_t xpos, uint16_t ypos);
    uCanvas_universal_obj_t* New_uCanvas_2DAdvancedTextbox(char* text, uint16_t xpos, uint16_t ypos, int width, int height);
    uCanvas_universal_obj_t* New_uCanvas_2DCircle(uint16_t xpos, uint16_t ypos,uint16_t radius);
    uCanvas_universal_obj_t* New_uCanvas_2DLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    uCanvas_universal_obj_t* New_uCanvas_2DTriangle(Coordinate2D_t Point1, Coordinate2D_t Point2, Coordinate2D_t Point3);
    uCanvas_universal_obj_t* New_uCanvas_2DEllipse(uint16_t xpos, uint16_t ypos,uint16_t radius_x, uint16_t radius_y);
    uCanvas_universal_obj_t* New_uCanvas_ViewPort(uCanvas2D_Instance_t* Window_Instance,uint16_t pos_x, uint16_t pos_y);
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
    uCanvas_universal_obj_t* New_uCanvas_2DSprite(sprite2D_t* sprite2D_obj,uint16_t pos_x, uint16_t pos_y);
    void uCanvas_Convert_RGB565A_to_RGBA8888(uint16_t* src, uint32_t* dest, int width, int height);
    void uCanvas_Convert_RGB565A_to_ARGB8888(uint16_t* src, uint32_t* dest, int width, int height);
    /**
     * @brief This API let's user scale up their custom 2D sprite Buffers. 
     * @param src           : sprite to be scaled
     * @param des           : location where to store scaled sprite
     * @param src_width     : actual width
     * @param src_height    : actual height
     * @param scale_factor  : Scale factor to scale up the sprite
     */
    void uCanvas_ScaleUp_SpriteBuf(uint16_t* src, uint16_t* dest, int src_width, int src_height, int scale_factor);
    void uCanvas_ScaleUp_Sprite2D(sprite2D_t* sprite_obj,uint16_t* reference,uint16_t* buffer, int h, int w, int scale_factor);
    void uCanvas_Sprite_Adjust_Contrast(sprite2D_t *sprite, int contrast);
    /**
     * @brief This API dynamically changes the 2D sprite Object source to render on scene.
     * @param obj           : reference to sprite element
     * @param sprite_obj    : reference to new sprite object to replace with
     */
    void uCanvas_Change_Sprite_Source(uCanvas_universal_obj_t* obj, sprite2D_t* sprite_obj);
    
    /**
     * @paragraph
     * This API Creates 2D Sprite Object that packages the sprite buffer, width, height and orientation
     * Create 2D Sprite Object based on raw Sprite buffer so we can use it easily in application
     *  sprite2D_t (properties) -
     *      > pixel_data - Pointes to 1D buffer storing the pixel values
     *      > Sprite_Height - Actual Height of sprite buffer (CHANGING THIS DOES NOT SCALES THE SPRITES)
     *      > Sprite_Width  - Actual Width  of sprite buffer (CHANGING THIS DOES NOT SCALES THE SPRITES)
     *      > Sprite_Orientation
     * 
     * @param  obj : reference to object variable
     * @param  pixel_data : reference to pixel_data
     * @param  width : actual width of pixel_data content
     * @param  height : actual width of height content
     */
    void uCanvas_Compose_2DSprite_Obj(sprite2D_t* obj, void* pixel_data,uint16_t width, uint16_t height, uCanvas_color_format_t color_format);
    
    /**
     * Properties Control API
    */
    /* Animates TextBox with Reaveal style animation */
    void uCanvas_Animate_Text_Reveal(uCanvas_universal_obj_t*obj, char* text, uint16_t delay);
    
    /* Sets content of Passed Textbox Object */
    void uCanvas_Set_Text(uCanvas_universal_obj_t*obj,char*text);

    /* Adjust Textbox properties */
    uint16_t uCanvas_Get_Font_Width(FontType_t FontType);
    uint16_t uCanvas_Get_Font_Height(FontType_t FontType);
    void uCanvas_Set_Textbox_Alignment(uCanvas_universal_obj_t* obj,uCanvas_Text_Alignment_t align_type);
    void uCanvas_Set_Textbox_Wrap_Style(uCanvas_universal_obj_t* obj,uCanvas_Text_Wrap_t wrap_type, uint8_t Wrap_Index);
    void uCanvas_Set_TextBox_Margin(uCanvas_universal_obj_t* obj,int margin_x, int margin_y);
    void uCanvas_Set_TextBox_Fill_Background(uCanvas_universal_obj_t* obj,fill_t fill_state, uint8_t r, uint8_t g, uint8_t b);
    void uCanvas_Set_TextBox_FontType(uCanvas_universal_obj_t* obj,FontType_t FontType);
    void uCanvas_Set_TextBox_Size(uCanvas_universal_obj_t* obj,int width, int height);
    void uCanvas_Enable_TextBox_Cursor(uCanvas_universal_obj_t* obj, uint32_t cursor_blink_rate,uCanvas_TextBox_Cursor_t cursor_type);
    void uCanvas_Disable_TextBox_Cursor(uCanvas_universal_obj_t* obj);
    void uCanvas_Set_TextBox_Border_Properties(uCanvas_universal_obj_t* obj,uint8_t border_thickness, uint8_t r, uint8_t g, uint8_t b);
    /* Sets the type of passed 2D Object */
    void uCanvas_Set_Obj_Type(uCanvas_universal_obj_t*obj,uCanvas_element_type_t type);
    
    /* Sets color of Passed 2D Object */
    void uCanvas_Set_Color(uCanvas_universal_obj_t* obj, uint16_t r,uint16_t g, uint16_t b);
    
    /* Sets Monochrome pixels values of Passed 2D Object */
    void uCanvas_Set_Monochrome_Color(uCanvas_universal_obj_t* obj, uint16_t color );
    
    /* Sets absolute position of Passed 2D Object */
    int uCanvas_Set_Triangle_Points(uCanvas_universal_obj_t* triangle, Coordinate2D_t Point1, Coordinate2D_t Point2, Coordinate2D_t Point3);
    void uCanvas_Set_Position(uCanvas_universal_obj_t* obj, uint16_t xpos,uint16_t ypos);
    void uCanvas_Set_Position_X(uCanvas_universal_obj_t* obj, uint16_t xpos);
    void uCanvas_Set_Position_Y(uCanvas_universal_obj_t* obj, uint16_t ypos);
    
    /* Sets cordinates of 2D Line Object */
    void uCanvas_Set_Line_Coordinates(uCanvas_universal_obj_t*line,uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void uCanvas_Set_Line_End(uCanvas_universal_obj_t*line, uint16_t x2, uint16_t y2);
    /* Sets radius value of elliptical type of 2D Object */
    void uCanvas_Set_Radius1(uCanvas_universal_obj_t* obj, uint16_t radius);
    void uCanvas_Set_Radius2(uCanvas_universal_obj_t* obj, uint16_t radius);
    
    /* Sets Height and Width of rectangular 2D Object */
    void uCanvas_Set_Width_Height(uCanvas_universal_obj_t* obj, uint16_t width,uint16_t height);
    
    /* Allows to Indicate Fill/NoFill insides of 2D  Object */
    void uCanvas_Set_Fill(uCanvas_universal_obj_t* obj, fill_t fill);
    
    /* Allows to Control Visiblity of Passed 2D  Object */
    void uCanvas_Set_Visiblity(uCanvas_universal_obj_t* obj, visibility_ctrl_t vctrl);   

    int get_random_number(int min, int max);
    
    void uCanvas_Delete_obj_from_scene(uCanvas_universal_obj_t* obj);
    void uCanvas_Delete_Scene(uCanvas_Scene_t* scene_obj);
    void uCanvas_Play_Sprite_Animation(uCanvas_Sprite_KeyFrames_t* obj, sprite2D_t* sprite_set);

    void uCanvas2D_Create_RenderBuffer(uCanvas2D_RenderBuffer_t* render_buffer, int width, int height);
    void draw_universal_object_to_target_render_buffer(uCanvas_universal_obj_t* obj, uCanvas2D_RenderBuffer_t* framebuffer, uCanvas2D_Display_Panel_t* panel);
#endif