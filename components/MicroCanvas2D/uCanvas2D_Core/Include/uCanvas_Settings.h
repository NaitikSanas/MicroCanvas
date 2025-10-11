#define MAX_ELEMENTS_NUM                        (4096)

#define USE_PPA_FOR_RENDERING                (1)

#define UCANVAS_TASK_STACK_SIZE                 (4046)
#define UCANVAS_RENDER_TASK_STACK_SIZE          (4096)
#define UCANVAS_RENDER_TASK_PRIORITY            (1)

 #define UCANVAS_TEXTBOX_MAX_CONTNENT_SIZE       (512)

#define UCANVAS_DEFAULT_RED                     (0)
#define UCANVAS_DEFAULT_GREEN                   (0)
#define UCANVAS_DEFAULT_BLUE                    (0)
#define UCANVAS_DISPLAY_ORIENTATION             1

#define UCANVAS_USE_DOUBLE_BUFFERING            (2)

enum {
    UNDEFINED,
    ESP32,
    STM32
};
#define UCANVAS_TARGET ESP32