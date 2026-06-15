/**
 * @file uCanvasServer_Core.c
 * @brief Core dispatcher implementation
 */

#include <string.h>
#include "uCanvasServer_Core.h"
#include "esp_log.h"

static const char* TAG = "uCanvasServer_Core";

/* ------------------------------------------------------------------ */
/* Handle / Scene tables                                                */
/* ------------------------------------------------------------------ */
static uCanvas_universal_obj_t* s_handle_table[UCS_MAX_HANDLES];
static uCanvas_Scene_t*         s_scene_table[UCS_MAX_SCENES];

/* Static instance storage - mirrors `static uCanvas2D_Instance_t g_canvas;`
   from the example. Index by instance_id (0 or 1). */
static uCanvas2D_Instance_t s_instances[UCS_MAX_INSTANCES];
static bool                 s_instance_in_use[UCS_MAX_INSTANCES];
static bool                 s_instance_has_panel[UCS_MAX_INSTANCES];
/* Store requested panel type for instances created by host until renderbuffer is attached */
static uint8_t              s_instance_panel_type[UCS_MAX_INSTANCES];
static uint16_t             s_instance_off_x[UCS_MAX_INSTANCES];
static uint16_t             s_instance_off_y[UCS_MAX_INSTANCES];

/* Maps protocol panel type byte -> uCanvas2D_Display_Panel_t* via
   uCanvas_Get_Panel_Handle(). Returns NULL on unknown type. */
static uCanvas2D_Display_Panel_t* panel_handle_from_type(uint8_t panel_type)
{
    switch (panel_type) {
        case UCS_PANEL_DUMMY:   return uCanvas_Get_Panel_Handle(PANEL_DUMMY);
        case UCS_PANEL_ST7789:  return uCanvas_Get_Panel_Handle(PANEL_ST7789);
        case UCS_PANEL_EK79007: return uCanvas_Get_Panel_Handle(PANEL_EK79007);
        default: return NULL;
    }
}

/* Sprite-upload working state (single in-flight upload for POC) */
typedef struct {
    bool      in_use;
    uint16_t  width;
    uint16_t  height;
    uint8_t   color_format;
    uint32_t  total_bytes;
    uint8_t*  buf;
} sprite_upload_t;

#define UCS_MAX_SPRITE_UPLOADS 4
static sprite_upload_t s_sprite_uploads[UCS_MAX_SPRITE_UPLOADS];

static uCanvasServer_SendFunc_t s_send_func = NULL;

/* ------------------------------------------------------------------ */
/* Little-endian helpers (UART/SPI frames use LE on the wire)          */
/* ------------------------------------------------------------------ */
static inline uint16_t rd_u16(const uint8_t* p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static inline uint32_t rd_u32(const uint8_t* p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static inline void wr_u16(uint8_t* p, uint16_t v) { p[0] = v & 0xFF; p[1] = (v >> 8) & 0xFF; }
static inline void wr_u32(uint8_t* p, uint32_t v) {
    p[0] = v & 0xFF; p[1] = (v >> 8) & 0xFF; p[2] = (v >> 16) & 0xFF; p[3] = (v >> 24) & 0xFF;
}

/* ------------------------------------------------------------------ */
/* Response helpers                                                     */
/* ------------------------------------------------------------------ */
static void send_ack(void)
{
    uint8_t payload[1] = { 0x00 };
    uCanvasServer_SendFrame(RSP_ACK, payload, sizeof(payload));
}

static void send_error(uint8_t err_code)
{
    uint8_t payload[2] = { 0x01, err_code };
    uCanvasServer_SendFrame(RSP_ERROR, payload, sizeof(payload));
    ESP_LOGW(TAG, "RSP_ERROR: 0x%02X", err_code);
}

static void send_handle(uint16_t handle)
{
    uint8_t payload[2];
    wr_u16(payload, handle);
    uCanvasServer_SendFrame(RSP_HANDLE, payload, sizeof(payload));
}

static void send_scene_id(uint16_t scene_id)
{
    uint8_t payload[2];
    wr_u16(payload, scene_id);
    uCanvasServer_SendFrame(RSP_SCENE_ID, payload, sizeof(payload));
}

static void send_sprite_id(uint16_t sprite_id)
{
    uint8_t payload[2];
    wr_u16(payload, sprite_id);
    uCanvasServer_SendFrame(RSP_SPRITE_ID, payload, sizeof(payload));
}

/* ------------------------------------------------------------------ */
/* Handle / scene table management                                     */
/* ------------------------------------------------------------------ */
uint16_t uCanvasServer_RegisterHandle(uCanvas_universal_obj_t* obj)
{
    for (uint16_t i = 0; i < UCS_MAX_HANDLES; i++) {
        if (s_handle_table[i] == NULL) {
            s_handle_table[i] = obj;
            return i;
        }
    }
    return 0xFFFF; /* table full */
}

uCanvas_universal_obj_t* uCanvasServer_GetObj(uint16_t handle)
{
    if (handle >= UCS_MAX_HANDLES) return NULL;
    return s_handle_table[handle];
}

bool uCanvasServer_FreeHandle(uint16_t handle)
{
    if (handle >= UCS_MAX_HANDLES || s_handle_table[handle] == NULL) return false;
    s_handle_table[handle] = NULL;
    return true;
}

uint16_t uCanvasServer_RegisterScene(uCanvas_Scene_t* scene)
{
    for (uint16_t i = 0; i < UCS_MAX_SCENES; i++) {
        if (s_scene_table[i] == NULL) {
            s_scene_table[i] = scene;
            return i;
        }
    }
    return 0xFFFF;
}

uCanvas_Scene_t* uCanvasServer_GetScene(uint16_t scene_id)
{
    if (scene_id >= UCS_MAX_SCENES) return NULL;
    return s_scene_table[scene_id];
}

bool uCanvasServer_FreeScene(uint16_t scene_id)
{
    if (scene_id >= UCS_MAX_SCENES || s_scene_table[scene_id] == NULL) return false;
    s_scene_table[scene_id] = NULL;
    return true;
}

/* ------------------------------------------------------------------ */
/* Init / send func registration                                       */
/* ------------------------------------------------------------------ */
void uCanvasServer_Core_Init(void)
{
    memset(s_handle_table, 0, sizeof(s_handle_table));
    memset(s_scene_table, 0, sizeof(s_scene_table));
    memset(s_sprite_uploads, 0, sizeof(s_sprite_uploads));
    memset(s_instances, 0, sizeof(s_instances));
    memset(s_instance_in_use, 0, sizeof(s_instance_in_use));
    memset(s_instance_has_panel, 0, sizeof(s_instance_has_panel));
    for (int i = 0; i < UCS_MAX_INSTANCES; i++) {
        s_instance_panel_type[i] = 0xFF;
        s_instance_off_x[i] = 0;
        s_instance_off_y[i] = 0;
    }
    ESP_LOGI(TAG, "uCanvas Server core initialized (max handles=%d, max scenes=%d)",
             UCS_MAX_HANDLES, UCS_MAX_SCENES);
}

void uCanvasServer_SetSendFunc(uCanvasServer_SendFunc_t fn)
{
    s_send_func = fn;
}

void uCanvasServer_SendFrame(uint8_t cmd, const uint8_t* payload, uint16_t len)
{
    if (s_send_func != NULL) {
        s_send_func(cmd, payload, len);
    } else {
        ESP_LOGW(TAG, "SendFrame called but no transport registered (cmd=0x%02X)", cmd);
    }
}

/* ------------------------------------------------------------------ */
/* Argument validation helper                                          */
/* ------------------------------------------------------------------ */
#define REQUIRE_LEN(expected) \
    do { if (len != (expected)) { send_error(UCS_ERR_BAD_PAYLOAD_LEN); return; } } while (0)

#define REQUIRE_MIN_LEN(minlen) \
    do { if (len < (minlen)) { send_error(UCS_ERR_BAD_PAYLOAD_LEN); return; } } while (0)

#define GET_OBJ_OR_FAIL(var, handle) \
    uCanvas_universal_obj_t* var = uCanvasServer_GetObj(handle); \
    do { if ((var) == NULL) { send_error(UCS_ERR_BAD_HANDLE); return; } } while (0)

/* ------------------------------------------------------------------ */
/* Frame dispatcher                                                     */
/* ------------------------------------------------------------------ */
void uCanvasServer_HandleFrame(uint8_t cmd, const uint8_t* payload, uint16_t len)
{
    switch (cmd) {

    /* ============================================================ */
    /* Instance management                                            */
    /* ============================================================ */
     /* CMD_NEW_INSTANCE handled below (deferred panel attach) */

    case CMD_ATTACH_RENDERBUFFER: {
        REQUIRE_LEN(5); /* instance_id(1) width(2) height(2) */
        uint8_t instance_id = payload[0];
        if (instance_id >= UCS_MAX_INSTANCES || !s_instance_in_use[instance_id]) {
            send_error(UCS_ERR_BAD_INSTANCE_ID); break;
        }
        uint16_t width  = rd_u16(payload + 1);
        uint16_t height = rd_u16(payload + 3);

        int ok = uCanvas_Attach_RenderBuffer(&s_instances[instance_id], width, height);
        if (!ok) { send_error(UCS_ERR_RENDERBUFFER_FAIL); break; }

        ESP_LOGI(TAG, "CMD_ATTACH_RENDERBUFFER id=%d -> render_buffer=%p", instance_id, (void*)s_instances[instance_id].render_buffer);

        /* If host previously requested a panel type for this instance, init
           and attach the panel now (mirrors firmware: attach renderbuffer
           before attaching panel). */
        uint8_t ptype = s_instance_panel_type[instance_id];
        if (ptype != 0xFF) {
            uCanvas2D_Display_Panel_t* panel = panel_handle_from_type(ptype);
            if (panel) {
                panel->init(1);
                uCanvas_Attach_Panel(&s_instances[instance_id], panel);
                s_instance_has_panel[instance_id] = true;
                ESP_LOGI(TAG, "Attached panel for instance %d -> panel=%p", instance_id, (void*)panel);
            } else {
                ESP_LOGW(TAG, "Requested panel_type %d unknown at attach time", ptype);
            }
        }

        /* Apply any stored viewport offset now that renderbuffer exists */
        if (s_instance_off_x[instance_id] || s_instance_off_y[instance_id]) {
            uCanvas_Set_ViewPort_Position(&s_instances[instance_id], s_instance_off_x[instance_id], s_instance_off_y[instance_id]);
            ESP_LOGI(TAG, "Applied stored viewport offset for instance %d -> %d,%d", instance_id, s_instance_off_x[instance_id], s_instance_off_y[instance_id]);
        }

        send_ack();
        break;
    }

    case CMD_SET_VIEWPORT_POS: {
        REQUIRE_LEN(5); /* instance_id(1) x(2) y(2) */
        uint8_t instance_id = payload[0];
        if (instance_id >= UCS_MAX_INSTANCES || !s_instance_in_use[instance_id]) {
            send_error(UCS_ERR_BAD_INSTANCE_ID); break;
        }
        uint16_t x = rd_u16(payload + 1);
        uint16_t y = rd_u16(payload + 3);
        if (s_instances[instance_id].render_buffer == NULL) {
            s_instance_off_x[instance_id] = x;
            s_instance_off_y[instance_id] = y;
            ESP_LOGI(TAG, "Stored viewport offset for instance %d -> %d,%d", instance_id, x, y);
        } else {
            uCanvas_Set_ViewPort_Position(&s_instances[instance_id], x, y);
        }
        send_ack();
        break;
    }

    case CMD_ATTACH_SCENE: {
        REQUIRE_LEN(3); /* instance_id(1) scene_id(2) */
        uint8_t  instance_id = payload[0];
        uint16_t scene_id    = rd_u16(payload + 1);

        if (instance_id >= UCS_MAX_INSTANCES || !s_instance_in_use[instance_id]) {
            send_error(UCS_ERR_BAD_INSTANCE_ID); break;
        }
        uCanvas_Scene_t* scene = uCanvasServer_GetScene(scene_id);
        if (scene == NULL) { send_error(UCS_ERR_BAD_SCENE_ID); break; }

        uCanvas_Attach_Scene(&s_instances[instance_id], scene);
        ESP_LOGI(TAG, "CMD_ATTACH_SCENE id=%d -> active_scene=%p", instance_id, (void*)s_instances[instance_id].active_scene);
        send_ack();
        break;
    }

    case CMD_ATTACH_RENDERER: {
        REQUIRE_LEN(2); /* instance_id(1) core_id(1) */
        uint8_t instance_id = payload[0];
        uint8_t core_id     = payload[1];

        if (instance_id >= UCS_MAX_INSTANCES || !s_instance_in_use[instance_id]) { send_error(UCS_ERR_BAD_INSTANCE_ID); break; }

        /* Ensure renderbuffer and panel are attached before starting renderer */
        if (s_instances[instance_id].render_buffer == NULL || !s_instance_has_panel[instance_id]) {
            ESP_LOGW(TAG, "CMD_ATTACH_RENDERER id=%d -> missing renderbuffer/panel", instance_id);
            send_error(UCS_ERR_RENDERBUFFER_FAIL); break;
        }
        uCanvas_Set_Render_Mode(&s_instances[instance_id],AUTO_REFRESH);
        uCanvas_Attach_Renderer(&s_instances[instance_id], core_id,UCANVAS_SKETCH);
        ESP_LOGI(TAG, "CMD_ATTACH_RENDERER id=%d -> renderer task handle=%p", instance_id, (void*)s_instances[instance_id].render_task_handle);
        send_ack();
        break;
    }

     case CMD_NEW_INSTANCE: {
          REQUIRE_LEN(10); /* instance_id(1) panel_type(1) width(2) height(2) offset_x(2) offset_y(2) */
          uint8_t  instance_id = payload[0];
          uint8_t  panel_type  = payload[1];
          uint16_t width   = rd_u16(payload + 2);
          uint16_t height  = rd_u16(payload + 4);
          uint16_t off_x   = rd_u16(payload + 6);
          uint16_t off_y   = rd_u16(payload + 8);

          if (instance_id >= UCS_MAX_INSTANCES) { send_error(UCS_ERR_BAD_INSTANCE_ID); break; }

          /* Validate panel type now but defer calling panel->init / Attach_Panel
              until after the render buffer is created (matches firmware order). */
          uCanvas2D_Display_Panel_t* panel = panel_handle_from_type(panel_type);
          if (panel == NULL) { send_error(UCS_ERR_BAD_PANEL_TYPE); break; }

          uCanvas2D_Instance_t* inst = &s_instances[instance_id];
          memset(inst, 0, sizeof(*inst));

          inst->scale_output = false;
          inst->scale_x = 1.0f;
          inst->scale_y = 1.0f;

          /* store the requested panel type until ATTACH_RENDERBUFFER; store
              the viewport offset to be applied once the renderbuffer exists */
          s_instance_off_x[instance_id] = off_x;
          s_instance_off_y[instance_id] = off_y;
          s_instance_panel_type[instance_id] = panel_type;
          s_instance_in_use[instance_id] = true;

          ESP_LOGI(TAG, "CMD_NEW_INSTANCE id=%d panel_type=%d width=%d height=%d off=%d,%d -> inst=%p", 
                      instance_id, panel_type, width, height, off_x, off_y, (void*)inst);

          /* width/height not yet applied to a render buffer here - host must
              follow up with CMD_ATTACH_RENDERBUFFER using the same dimensions */
          (void)width; (void)height;

        send_ack();
        break;
    }

    case CMD_INSTANCE_PAUSE: {
        REQUIRE_LEN(1);
        uint8_t instance_id = payload[0];
        if (instance_id >= UCS_MAX_INSTANCES || !s_instance_in_use[instance_id]) {
            send_error(UCS_ERR_BAD_INSTANCE_ID); break;
        }
        uCanvas_Pause_Instance(&s_instances[instance_id]);
        send_ack();
        break;
    }

    case CMD_INSTANCE_RESUME: {
        REQUIRE_LEN(1);
        uint8_t instance_id = payload[0];
        if (instance_id >= UCS_MAX_INSTANCES || !s_instance_in_use[instance_id]) {
            send_error(UCS_ERR_BAD_INSTANCE_ID); break;
        }
        uCanvas_Resume_Instance(&s_instances[instance_id]);
        send_ack();
        break;
    }

    case CMD_INSTANCE_DESTROY: {
        REQUIRE_LEN(1);
        uint8_t instance_id = payload[0];
        if (instance_id >= UCS_MAX_INSTANCES || !s_instance_in_use[instance_id]) {
            send_error(UCS_ERR_BAD_INSTANCE_ID); break;
        }
        uCanvas_Destroy_Instance(&s_instances[instance_id]);
        s_instance_in_use[instance_id] = false;
        s_instance_has_panel[instance_id] = false;
        send_ack();
        break;
    }

    /* ============================================================ */
    /* Object creation                                                */
    /* ============================================================ */
    case CMD_CREATE_RECT: {
        REQUIRE_LEN(8);
        uint16_t x = rd_u16(payload + 0);
        uint16_t y = rd_u16(payload + 2);
        uint16_t h = rd_u16(payload + 4);
        uint16_t w = rd_u16(payload + 6);
        uCanvas_universal_obj_t* obj = New_uCanvas_2DRectangle(x, y, h, w);
        uint16_t handle = uCanvasServer_RegisterHandle(obj);
        if (handle == 0xFFFF) { send_error(UCS_ERR_HANDLE_TABLE_FULL); break; }
        send_handle(handle);
        break;
    }

    case CMD_CREATE_CIRCLE: {
        REQUIRE_LEN(6);
        uint16_t x = rd_u16(payload + 0);
        uint16_t y = rd_u16(payload + 2);
        uint16_t r = rd_u16(payload + 4);
        uCanvas_universal_obj_t* obj = New_uCanvas_2DCircle(x, y, r);
        uint16_t handle = uCanvasServer_RegisterHandle(obj);
        if (handle == 0xFFFF) { send_error(UCS_ERR_HANDLE_TABLE_FULL); break; }
        send_handle(handle);
        break;
    }

    case CMD_CREATE_LINE: {
        REQUIRE_LEN(8);
        uint16_t x1 = rd_u16(payload + 0);
        uint16_t y1 = rd_u16(payload + 2);
        uint16_t x2 = rd_u16(payload + 4);
        uint16_t y2 = rd_u16(payload + 6);
        uCanvas_universal_obj_t* obj = New_uCanvas_2DLine(x1, y1, x2, y2);
        uint16_t handle = uCanvasServer_RegisterHandle(obj);
        if (handle == 0xFFFF) { send_error(UCS_ERR_HANDLE_TABLE_FULL); break; }
        send_handle(handle);
        break;
    }

    case CMD_CREATE_TRIANGLE: {
        REQUIRE_LEN(12);
        Coordinate2D_t p1 = { rd_u16(payload + 0),  rd_u16(payload + 2) };
        Coordinate2D_t p2 = { rd_u16(payload + 4),  rd_u16(payload + 6) };
        Coordinate2D_t p3 = { rd_u16(payload + 8),  rd_u16(payload + 10) };
        uCanvas_universal_obj_t* obj = New_uCanvas_2DTriangle(p1, p2, p3);
        uint16_t handle = uCanvasServer_RegisterHandle(obj);
        if (handle == 0xFFFF) { send_error(UCS_ERR_HANDLE_TABLE_FULL); break; }
        send_handle(handle);
        break;
    }

    case CMD_CREATE_ELLIPSE: {
        REQUIRE_LEN(8);
        uint16_t x  = rd_u16(payload + 0);
        uint16_t y  = rd_u16(payload + 2);
        uint16_t rx = rd_u16(payload + 4);
        uint16_t ry = rd_u16(payload + 6);
        uCanvas_universal_obj_t* obj = New_uCanvas_2DEllipse(x, y, rx, ry);
        uint16_t handle = uCanvasServer_RegisterHandle(obj);
        if (handle == 0xFFFF) { send_error(UCS_ERR_HANDLE_TABLE_FULL); break; }
        send_handle(handle);
        break;
    }

    case CMD_CREATE_TEXTBOX: {
        REQUIRE_MIN_LEN(5); /* x(2) y(2) len(1) */
        uint16_t x = rd_u16(payload + 0);
        uint16_t y = rd_u16(payload + 2);
        uint8_t  text_len = payload[4];
        if (len != (uint16_t)(5 + text_len)) { send_error(UCS_ERR_BAD_PAYLOAD_LEN); break; }

        char text[256];
        memcpy(text, payload + 5, text_len);
        text[text_len] = '\0';

        uCanvas_universal_obj_t* obj = New_uCanvas_2DTextbox(text, x, y);
        uint16_t handle = uCanvasServer_RegisterHandle(obj);
        if (handle == 0xFFFF) { send_error(UCS_ERR_HANDLE_TABLE_FULL); break; }
        send_handle(handle);
        break;
    }

    case CMD_CREATE_ADV_TEXTBOX: {
        REQUIRE_MIN_LEN(9); /* x(2) y(2) w(2) h(2) len(1) */
        uint16_t x = rd_u16(payload + 0);
        uint16_t y = rd_u16(payload + 2);
        uint16_t w = rd_u16(payload + 4);
        uint16_t h = rd_u16(payload + 6);
        uint8_t  text_len = payload[8];
        if (len != (uint16_t)(9 + text_len)) { send_error(UCS_ERR_BAD_PAYLOAD_LEN); break; }

        char text[256];
        memcpy(text, payload + 9, text_len);
        text[text_len] = '\0';

        uCanvas_universal_obj_t* obj = New_uCanvas_2DAdvancedTextbox(text, x, y, w, h);
        uint16_t handle = uCanvasServer_RegisterHandle(obj);
        if (handle == 0xFFFF) { send_error(UCS_ERR_HANDLE_TABLE_FULL); break; }
        send_handle(handle);
        break;
    }

    /* ============================================================ */
    /* Sprite upload sequence                                         */
    /* ============================================================ */
    case CMD_SPRITE_BEGIN: {
        REQUIRE_LEN(5); /* width(2) height(2) color_format(1) */
        uint16_t width  = rd_u16(payload + 0);
        uint16_t height = rd_u16(payload + 2);
        uint8_t  fmt    = payload[4];

        uint16_t slot = 0xFFFF;
        for (uint16_t i = 0; i < UCS_MAX_SPRITE_UPLOADS; i++) {
            if (!s_sprite_uploads[i].in_use) { slot = i; break; }
        }
        if (slot == 0xFFFF) { send_error(UCS_ERR_SPRITE_ALLOC); break; }

        /* bytes per pixel: ARGB8888=4, RGB565=2 - color_fmt enum is project-defined,
           assume 0 = RGB565 (2 bytes), 1 = ARGB8888 (4 bytes) for POC */
        uint32_t bpp = (fmt == 1) ? 4 : 2;
        uint32_t total = (uint32_t)width * height * bpp;

        uint8_t* buf = (uint8_t*)malloc(total);
        if (buf == NULL) { send_error(UCS_ERR_SPRITE_ALLOC); break; }

        s_sprite_uploads[slot].in_use      = true;
        s_sprite_uploads[slot].width       = width;
        s_sprite_uploads[slot].height      = height;
        s_sprite_uploads[slot].color_format = fmt;
        s_sprite_uploads[slot].total_bytes = total;
        s_sprite_uploads[slot].buf         = buf;

        send_sprite_id(slot);
        break;
    }

    case CMD_SPRITE_CHUNK: {
        REQUIRE_MIN_LEN(6); /* sprite_id(2) offset(2) chunk_len(2) */
        uint16_t sprite_id = rd_u16(payload + 0);
        uint16_t offset    = rd_u16(payload + 2);
        uint16_t chunk_len = rd_u16(payload + 4);

        if (sprite_id >= UCS_MAX_SPRITE_UPLOADS || !s_sprite_uploads[sprite_id].in_use) {
            send_error(UCS_ERR_BAD_SPRITE_ID); break;
        }
        if (len != (uint16_t)(6 + chunk_len)) { send_error(UCS_ERR_BAD_PAYLOAD_LEN); break; }

        sprite_upload_t* su = &s_sprite_uploads[sprite_id];
        if ((uint32_t)offset + chunk_len > su->total_bytes) {
            send_error(UCS_ERR_BAD_PAYLOAD_LEN); break;
        }

        memcpy(su->buf + offset, payload + 6, chunk_len);
        send_ack();
        break;
    }

    case CMD_SPRITE_COMMIT: {
        REQUIRE_LEN(6); /* sprite_id(2) x(2) y(2) */
        uint16_t sprite_id = rd_u16(payload + 0);
        uint16_t x = rd_u16(payload + 2);
        uint16_t y = rd_u16(payload + 4);

        if (sprite_id >= UCS_MAX_SPRITE_UPLOADS || !s_sprite_uploads[sprite_id].in_use) {
            send_error(UCS_ERR_BAD_SPRITE_ID); break;
        }

        sprite_upload_t* su = &s_sprite_uploads[sprite_id];
        uCanvas_color_format_t color_fmt = (su->color_format == 1) ? COLOR_ARGB8888 : COLOR_RGB565;

        sprite2D_t sprite;
        uCanvas_Compose_2DSprite_Obj(&sprite, su->buf, su->width, su->height, color_fmt);

        uCanvas_universal_obj_t* obj = New_uCanvas_2DSprite(&sprite, x, y);

        /* Note: uCanvas_Compose_2DSprite_Obj is expected to copy/own pixel data
           into its own buffer as needed by the renderer; free our staging buffer. */
        free(su->buf);
        su->buf = NULL;
        su->in_use = false;

        uint16_t handle = uCanvasServer_RegisterHandle(obj);
        if (handle == 0xFFFF) { send_error(UCS_ERR_HANDLE_TABLE_FULL); break; }
        send_handle(handle);
        break;
    }

    /* ============================================================ */
    /* Property setters                                               */
    /* ============================================================ */
    case CMD_SET_POSITION: {
        REQUIRE_LEN(6);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Position(obj, rd_u16(payload + 2), rd_u16(payload + 4));
        send_ack();
        break;
    }

    case CMD_SET_POSITION_X: {
        REQUIRE_LEN(4);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Position_X(obj, rd_u16(payload + 2));
        send_ack();
        break;
    }

    case CMD_SET_POSITION_Y: {
        REQUIRE_LEN(4);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Position_Y(obj, rd_u16(payload + 2));
        send_ack();
        break;
    }

    case CMD_SET_COLOR: {
        REQUIRE_LEN(5);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Color(obj, payload[2], payload[3], payload[4]);
        send_ack();
        break;
    }

    case CMD_SET_FILL: {
        REQUIRE_LEN(3);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Fill(obj, (fill_t)payload[2]);
        send_ack();
        break;
    }

    case CMD_SET_VISIBILITY: {
        REQUIRE_LEN(3);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Visiblity(obj, (visibility_ctrl_t)payload[2]);
        send_ack();
        break;
    }

    case CMD_SET_WIDTH_HEIGHT: {
        REQUIRE_LEN(6);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Width_Height(obj, rd_u16(payload + 2), rd_u16(payload + 4));
        send_ack();
        break;
    }

    case CMD_SET_RADIUS1: {
        REQUIRE_LEN(4);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Radius1(obj, rd_u16(payload + 2));
        send_ack();
        break;
    }

    case CMD_SET_RADIUS2: {
        REQUIRE_LEN(4);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Radius2(obj, rd_u16(payload + 2));
        send_ack();
        break;
    }

    case CMD_SET_LINE_COORDS: {
        REQUIRE_LEN(10);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Line_Coordinates(obj, rd_u16(payload + 2), rd_u16(payload + 4),
                                      rd_u16(payload + 6), rd_u16(payload + 8));
        send_ack();
        break;
    }

    case CMD_SET_LINE_END: {
        REQUIRE_LEN(6);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Line_End(obj, rd_u16(payload + 2), rd_u16(payload + 4));
        send_ack();
        break;
    }

    case CMD_SET_TEXT: {
        REQUIRE_MIN_LEN(3); /* handle(2) len(1) */
        uint16_t handle = rd_u16(payload + 0);
        uint8_t text_len = payload[2];
        if (len != (uint16_t)(3 + text_len)) { send_error(UCS_ERR_BAD_PAYLOAD_LEN); break; }

        GET_OBJ_OR_FAIL(obj, handle);
        char text[256];
        memcpy(text, payload + 3, text_len);
        text[text_len] = '\0';
        uCanvas_Set_Text(obj, text);
        send_ack();
        break;
    }

    case CMD_SET_TRIANGLE_PTS: {
        REQUIRE_LEN(14);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        Coordinate2D_t p1 = { rd_u16(payload + 2),  rd_u16(payload + 4) };
        Coordinate2D_t p2 = { rd_u16(payload + 6),  rd_u16(payload + 8) };
        Coordinate2D_t p3 = { rd_u16(payload + 10), rd_u16(payload + 12) };
        uCanvas_Set_Triangle_Points(obj, p1, p2, p3);
        send_ack();
        break;
    }

    case CMD_SET_OBJ_TYPE: {
        REQUIRE_LEN(3);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Obj_Type(obj, (uCanvas_element_type_t)payload[2]);
        send_ack();
        break;
    }

    /* ============================================================ */
    /* Textbox-specific setters                                       */
    /* ============================================================ */
    case CMD_SET_TB_ALIGNMENT: {
        REQUIRE_LEN(3);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Textbox_Alignment(obj, (uCanvas_Text_Alignment_t)payload[2]);
        send_ack();
        break;
    }

    case CMD_SET_TB_WRAP: {
        REQUIRE_LEN(4);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_Textbox_Wrap_Style(obj, (uCanvas_Text_Wrap_t)payload[2], payload[3]);
        send_ack();
        break;
    }

    case CMD_SET_TB_MARGIN: {
        REQUIRE_LEN(6);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_TextBox_Margin(obj, rd_u16(payload + 2), rd_u16(payload + 4));
        send_ack();
        break;
    }

    case CMD_SET_TB_FILL_BG: {
        REQUIRE_LEN(6);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_TextBox_Fill_Background(obj, (fill_t)payload[2], payload[3], payload[4], payload[5]);
        send_ack();
        break;
    }

    case CMD_SET_TB_FONT: {
        REQUIRE_LEN(3);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_TextBox_FontType(obj, (FontType_t)payload[2]);
        send_ack();
        break;
    }

    case CMD_SET_TB_SIZE: {
        REQUIRE_LEN(6);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_TextBox_Size(obj, rd_u16(payload + 2), rd_u16(payload + 4));
        send_ack();
        break;
    }

    case CMD_TB_CURSOR_ENABLE: {
        REQUIRE_LEN(7); /* handle(2) blink_rate(4) cursor_type(1) */
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uint32_t blink_rate = rd_u32(payload + 2);
        uCanvas_Enable_TextBox_Cursor(obj, blink_rate, (uCanvas_TextBox_Cursor_t)payload[6]);
        send_ack();
        break;
    }

    case CMD_TB_CURSOR_DISABLE: {
        REQUIRE_LEN(2);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Disable_TextBox_Cursor(obj);
        send_ack();
        break;
    }

    case CMD_SET_TB_BORDER: {
        REQUIRE_LEN(6);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Set_TextBox_Border_Properties(obj, payload[2], payload[3], payload[4], payload[5]);
        send_ack();
        break;
    }

    case CMD_ANIMATE_TEXT_REVEAL: {
        REQUIRE_MIN_LEN(5); /* handle(2) delay(2) len(1) */
        uint16_t handle = rd_u16(payload + 0);
        uint16_t delay  = rd_u16(payload + 2);
        uint8_t  text_len = payload[4];
        if (len != (uint16_t)(5 + text_len)) { send_error(UCS_ERR_BAD_PAYLOAD_LEN); break; }

        GET_OBJ_OR_FAIL(obj, handle);
        char text[256];
        memcpy(text, payload + 5, text_len);
        text[text_len] = '\0';
        uCanvas_Animate_Text_Reveal(obj, text, delay);
        send_ack();
        break;
    }

    /* ============================================================ */
    /* Scene & instance management                                   */
    /* ============================================================ */
    case CMD_NEW_SCENE: {
        REQUIRE_LEN(0);
        uCanvas_Scene_t* scene = New_uCanvas_Scene();
        uint16_t scene_id = uCanvasServer_RegisterScene(scene);
        if (scene_id == 0xFFFF) { send_error(UCS_ERR_SCENE_TABLE_FULL); break; }
        send_scene_id(scene_id);
        break;
    }

    case CMD_SET_ACTIVE_SCENE: {
        REQUIRE_LEN(2);
        uint16_t scene_id = rd_u16(payload + 0);
        uCanvas_Scene_t* scene = uCanvasServer_GetScene(scene_id);
        if (scene == NULL) { send_error(UCS_ERR_BAD_SCENE_ID); break; }
        uCanvas_set_active_scene(scene);
        ESP_LOGI(TAG, "CMD_SET_ACTIVE_SCENE scene_id=%d -> global active_scene=%p", scene_id, (void*)scene);
        send_ack();
        break;
    }

    case CMD_DELETE_OBJECT: {
        REQUIRE_LEN(2);
        uint16_t handle = rd_u16(payload + 0);
        GET_OBJ_OR_FAIL(obj, handle);
        uCanvas_Delete_obj_from_scene(obj);
        uCanvasServer_FreeHandle(handle);
        send_ack();
        break;
    }

    case CMD_DELETE_SCENE: {
        REQUIRE_LEN(2);
        uint16_t scene_id = rd_u16(payload + 0);
        uCanvas_Scene_t* scene = uCanvasServer_GetScene(scene_id);
        if (scene == NULL) { send_error(UCS_ERR_BAD_SCENE_ID); break; }
        uCanvas_Delete_Scene(scene);
        uCanvasServer_FreeScene(scene_id);
        send_ack();
        break;
    }

    case CMD_SET_TARGET_PANEL: {
        REQUIRE_LEN(1);
        /* TODO: hook up to uCanvas_Attach_Panel / multi-panel instance switching
           once the multi-instance routing design is finalized. */
        ESP_LOGI(TAG, "CMD_SET_TARGET_PANEL: panel_id=%d (not yet implemented)", payload[0]);
        send_ack();
        break;
    }

    /* ============================================================ */
    /* System / misc                                                  */
    /* ============================================================ */
    case CMD_PING: {
        REQUIRE_LEN(0);
        uCanvasServer_SendFrame(RSP_PONG, NULL, 0);
        break;
    }

    case CMD_GET_FPS: {
        REQUIRE_LEN(0);
        /* TODO: pass actual uCanvas2D_Instance_t* once core init stores it */
        uint8_t resp[4];
        wr_u32(resp, 0);
        uCanvasServer_SendFrame(RSP_FPS, resp, sizeof(resp));
        break;
    }

    case CMD_SET_REFRESH_DELAY: {
        REQUIRE_LEN(2);
        /* TODO: pass actual uCanvas2D_Instance_t* once core init stores it */
        ESP_LOGI(TAG, "CMD_SET_REFRESH_DELAY: %d ms (not yet wired to instance)",
                 rd_u16(payload + 0));
        send_ack();
        break;
    }

    /* ============================================================ */
    default:
        send_error(UCS_ERR_UNKNOWN_CMD);
        break;
    }
}