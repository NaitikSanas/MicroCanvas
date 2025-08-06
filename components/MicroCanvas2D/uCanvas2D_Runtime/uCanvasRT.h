#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "uCanvasDataTypes.h"

typedef enum {
    VAR_TYPE_INT,
    VAR_TYPE_BOOL,
    VAR_TYPE_STRING,
    VAR_TYPE_FLOAT
} VarType;

typedef struct {
    char name[32];
    VarType type;
    union {
        int i_val;
        bool b_val;
        float f_val;
        char* s_val;
    };
} uCanvas_Variable_t;

typedef struct {
    uCanvas_Variable_t* vars;
    size_t count;
} uCanvas_VariableTable_t;

typedef void (*EventHandlerFn)(uCanvasRuntime_t* runtime, const char* event_id);

typedef struct {
    char event_id[32];
    EventHandlerFn handler;
} uCanvas_EventHandler_t;

typedef struct {
    uCanvas_EventHandler_t* handlers;
    size_t count;
} uCanvas_EventDispatcher_t;

typedef struct {
    char name[32];
    char type[16]; // e.g., "pushbutton", "encoder", "timer"
    char on_event[64]; // Optional event handler ID or name
} uCanvasScript_Object_t;

typedef struct {
    uCanvasScript_Object_t* objects;
    size_t object_count;
} uCanvasScript_t;

typedef enum {
    INPUT_TYPE_PUSHBUTTON,
    INPUT_TYPE_ENCODER,
    INPUT_TYPE_TOUCH,
    INPUT_TYPE_TIMER
} uCanvas_InputType_t;

typedef struct {
    char name[32];
    uCanvas_InputType_t type;
    union {
        struct {
            uint16_t pin;
            bool active_low;
        } pushbutton;

        struct {
            uint16_t pin_a;
            uint16_t pin_b;
        } encoder;

        struct {
            uint32_t interval_ms;
        } timer;
    };

    char event_on_trigger[32]; // e.g. "fire_weapon", "rotate_ship"
} uCanvas_InputDevice_t;

typedef struct {
    uCanvas_InputDevice_t* devices;
    size_t count;
} uCanvas_InputHandler_t;

typedef enum {
    OBJECT_TYPE_SPRITE,
    OBJECT_TYPE_TILEMAP,
    OBJECT_TYPE_TEXT,
    OBJECT_TYPE_LOGIC
} uCanvas_ObjectType_t;

typedef struct {
    char name[32];
    uCanvas_ObjectType_t type;
    void* object_ptr; // Pointer to the actual uCanvas struct (e.g. sprite2D_t*)
} uCanvas_ObjectEntry_t;

typedef struct {
    uCanvas_ObjectEntry_t* entries;
    size_t count;
} uCanvas_ObjectRegistry_t;

typedef struct {
    uCanvas_Scene_t* scene;
    uCanvas_Instance_t* instance;
    uCanvas_InputHandler_t input_handler;
    uCanvas_ObjectRegistry_t object_registry;
    uCanvas_VariableTable_t variables;
    uCanvas_EventDispatcher_t events;
} uCanvasRuntime_t;
