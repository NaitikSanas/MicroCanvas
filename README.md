## uCanvas 2D Graphics API

### Overview

The uCanvas Rendering Engine API provides a set of functions for initializing and controlling a rendering engine, creating tasks for animations and user interactions, managing scenes, and creating 2D shapes and objects. The API is designed to be simple and versatile for creating graphical applications.

### Initializing and Starting

#### `void start_uCanvas_engine(void);`

This function initiates the rendering engine and initializes the display.

### Task Management

#### `uCanvas_Animation_task_handle_t uCanvas_Add_Task(uCanvas_Animation_task_t animation_loop);`

Creates an independent custom thread for the passed loop function and returns a handle.

#### `void uCanvas_Pause_Task(uCanvas_Animation_task_handle_t task_handle);`

Pauses a custom user task identified by the task handle.

#### `void uCanvas_Delay(uint16_t delay);`

Adds a delay in milliseconds.

#### `void uCanvas_Remove_Task(uCanvas_Animation_task_handle_t handle);`

Deletes a custom user task identified by the task handle.

### Scene Management

#### `uCanvas_Scene_t* New_uCanvas_Scene(void);`

Creates a new 2D scene instance.

#### `void uCanvas_set_active_scene(uCanvas_Scene_t* scene);`

Sets the passed 2D instance as the active scene to render.

### Object Creation

#### `uCanvas_universal_obj_t* New_uCanvas_2DRectangle(uint16_t xpos, uint16_t ypos, uint16_t h, uint16_t w);`

#### `uCanvas_universal_obj_t* New_uCanvas_2DTextbox(char* text, uint16_t xpos, uint16_t ypos);`

#### `uCanvas_universal_obj_t* New_uCanvas_2DCircle(uint16_t xpos, uint16_t ypos, uint16_t radius);`

#### `uCanvas_universal_obj_t* New_uCanvas_2DLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);`

Creates various 2D objects such as rectangles, textboxes, circles, and lines.

### Properties Control

#### `void uCanvas_Animate_Text_Reveal(uCanvas_universal_obj_t* obj, char* text, uint16_t delay);`

Animates a textbox with a reveal-style animation.

#### `void uCanvas_Set_Text(uCanvas_universal_obj_t* obj, char* text);`

Sets the content of the passed textbox object.

#### `void uCanvas_Set_Obj_Type(uCanvas_universal_obj_t* obj, uCanvas_element_type_t type);`

Sets the type of the passed 2D object.

#### `void uCanvas_Set_Color(uCanvas_universal_obj_t* obj, uint16_t r, uint16_t g, uint16_t b);`

Sets the color of the passed 2D object.

#### `void uCanvas_Set_Monochrome_Color(uCanvas_universal_obj_t* obj, uint16_t color);`

Sets monochrome pixel values of the passed 2D object.

#### `void uCanvas_Set_Position(uCanvas_universal_obj_t* obj, uint16_t xpos, uint16_t ypos);`

Sets the absolute position of the passed 2D object.

#### `void uCanvas_Set_Line_Coordinates(uCanvas_universal_obj_t* line, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);`

Sets coordinates for a 2D line object.

#### `void uCanvas_Set_Radius(uCanvas_universal_obj_t* obj, uint16_t radius);`

Sets the radius value of an elliptical type of 2D object.

#### `void uCanvas_Set_Width_Height(uCanvas_universal_obj_t* obj, uint16_t width, uint16_t height);`

Sets the height and width of a rectangular 2D object.

#### `void uCanvas_Set_Fill(uCanvas_universal_obj_t* obj, fill_t fill);`

Allows indicating fill/no-fill insides of a 2D object.

#### `void uCanvas_Set_Visibility(uCanvas_universal_obj_t* obj, visibility_ctrl_t vctrl);`

Allows controlling the visibility of the passed 2D object.
