#define MAX_ELEMENTS_NUM                        (4048)

#define USE_PPA_FOR_RENDERING                   (1)
#define UCANVAS_USE_SPIRAM                      (1)

#define UCANVAS_TASK_STACK_SIZE                 (4046)
#define UCANVAS_RENDER_TASK_STACK_SIZE          (4096)
#define UCANVAS_RENDER_TASK_PRIORITY            (1)

 #define UCANVAS_TEXTBOX_MAX_CONTNENT_SIZE       (512)

#define UCANVAS_DEFAULT_RED                     (0)
#define UCANVAS_DEFAULT_GREEN                   (0)
#define UCANVAS_DEFAULT_BLUE                    (0)
#define UCANVAS_DISPLAY_ORIENTATION             1

#define UCANVAS_USE_DOUBLE_BUFFERING            (1)

/**
 * Optional renderer that redraws only dirty rectangles (regions).
 * 0 = use existing full-scene renderer (uCanvas_renderer_task)
 * 1 = use dirty-rect renderer (uCanvas_renderer_task_dirtyrect)
 */
#define UCANVAS_USE_DIRTY_RECT_RENDERER         (0)

/**
 * Dirty-rect renderer tuning:
 * If the computed dirty rectangle area exceeds this percent of the framebuffer,
 * fall back to full redraw + full push for that frame (usually faster than
 * packing/pushing a near-fullscreen tile).
 */
#define UCANVAS_DIRTY_RECT_FALLBACK_FULL_PCT    (10)

enum {
    UNDEFINED,
    ESP32,
    STM32
};
#define UCANVAS_TARGET ESP32