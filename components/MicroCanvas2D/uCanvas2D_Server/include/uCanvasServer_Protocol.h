/**
 * @file uCanvasServer_Protocol.h
 * @brief Command/response opcode definitions for uCanvas Server protocol v0 (POC)
 */

#ifndef UCANVAS_SERVER_PROTOCOL_H_
#define UCANVAS_SERVER_PROTOCOL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------- */
/* Frame constants                                                          */
/* ---------------------------------------------------------------------- */
#define UCS_SOF                 0xAA
#define UCS_MAX_PAYLOAD_LEN     512   /* generous bound for textbox/sprite chunk payloads */
#define UCS_MAX_HANDLES         256
#define UCS_MAX_SCENES          16

/* ---------------------------------------------------------------------- */
/* Instance management (0x05 - 0x0F)                                       */
/* Mirrors the manual setup sequence (static instances, no New_uCanvas_*    */
/* Instance/Window_Instance APIs). Max UCS_MAX_INSTANCES instances.        */
/* ---------------------------------------------------------------------- */
#define UCS_MAX_INSTANCES       2

/* Panel type enum - mirrors uCanvas2D_Display_Panel_t selectors */
#define UCS_PANEL_DUMMY         0x00
#define UCS_PANEL_ST7789        0x01
#define UCS_PANEL_EK79007       0x02  /* ESP32-P4 only */

/* Render mode enum - mirrors uCanvas2D_Render_Mode_t */
#define UCS_RENDER_AUTO_REFRESH         0x00
#define UCS_RENDER_ASYNC_FRAME_QUEUED   0x01
#define UCS_RENDER_ASYNC_FRAME_COMMIT   0x02

#define CMD_NEW_INSTANCE        0x06  /* instance_id:u8 panel_type:u8 width:u16 height:u16
                                          offset_x:u16 offset_y:u16 -> RSP_ACK/ERROR
                                          (host picks instance_id 0 or 1; panel->init() called here) */
#define CMD_ATTACH_RENDERBUFFER 0x07  /* instance_id:u8 width:u16 height:u16 -> RSP_ACK/ERROR */
#define CMD_SET_VIEWPORT_POS    0x08  /* instance_id:u8 x:u16 y:u16 */
#define CMD_ATTACH_SCENE        0x09  /* instance_id:u8 scene_id:u16 */
#define CMD_SET_PANEL_REFRESH   0x0A  /* instance_id:u8 delay:u16 */
#define CMD_SET_RENDER_MODE     0x0B  /* instance_id:u8 mode:u8 */
#define CMD_ATTACH_RENDERER     0x0C  /* instance_id:u8 core_id:u8 (starts renderer task) */
#define CMD_INSTANCE_PAUSE      0x0D  /* instance_id:u8 */
#define CMD_INSTANCE_RESUME     0x0E  /* instance_id:u8 */
#define CMD_INSTANCE_DESTROY    0x0F  /* instance_id:u8 */

/* ---------------------------------------------------------------------- */
/* Object creation commands (0x10 - 0x1F) -> respond with RSP_HANDLE       */
/* ---------------------------------------------------------------------- */
#define CMD_CREATE_RECT         0x10  /* x:u16 y:u16 h:u16 w:u16 */
#define CMD_CREATE_CIRCLE       0x11  /* x:u16 y:u16 radius:u16 */
#define CMD_CREATE_LINE         0x12  /* x1:u16 y1:u16 x2:u16 y2:u16 */
#define CMD_CREATE_TRIANGLE     0x13  /* p1x p1y p2x p2y p3x p3y : u16 each */
#define CMD_CREATE_ELLIPSE      0x14  /* x:u16 y:u16 rx:u16 ry:u16 */
#define CMD_CREATE_TEXTBOX      0x15  /* x:u16 y:u16 len:u8 text[len] */
#define CMD_CREATE_ADV_TEXTBOX  0x16  /* x:u16 y:u16 w:u16 h:u16 len:u8 text[len] */

/* Sprite upload sequence (0x17-0x19) */
#define CMD_SPRITE_BEGIN        0x17  /* width:u16 height:u16 color_fmt:u8 -> RSP_SPRITE_ID */
#define CMD_SPRITE_CHUNK        0x18  /* sprite_id:u16 offset:u16 chunk_len:u16 data[chunk_len] */
#define CMD_SPRITE_COMMIT       0x19  /* sprite_id:u16 x:u16 y:u16 -> RSP_HANDLE */

/* ---------------------------------------------------------------------- */
/* Property setter commands (0x20 - 0x3F) -> respond with RSP_ACK/RSP_ERROR*/
/* All take handle:u16 as first field unless noted                         */
/* ---------------------------------------------------------------------- */
#define CMD_SET_POSITION        0x20  /* handle:u16 x:u16 y:u16 */
#define CMD_SET_POSITION_X      0x21  /* handle:u16 x:u16 */
#define CMD_SET_POSITION_Y      0x22  /* handle:u16 y:u16 */
#define CMD_SET_COLOR           0x23  /* handle:u16 r:u8 g:u8 b:u8 */
#define CMD_SET_FILL            0x24  /* handle:u16 fill:u8 */
#define CMD_SET_VISIBILITY      0x25  /* handle:u16 vis:u8 */
#define CMD_SET_WIDTH_HEIGHT    0x26  /* handle:u16 w:u16 h:u16 */
#define CMD_SET_RADIUS1         0x27  /* handle:u16 radius:u16 */
#define CMD_SET_RADIUS2         0x28  /* handle:u16 radius:u16 */
#define CMD_SET_LINE_COORDS     0x29  /* handle:u16 x1:u16 y1:u16 x2:u16 y2:u16 */
#define CMD_SET_LINE_END        0x2A  /* handle:u16 x2:u16 y2:u16 */
#define CMD_SET_TEXT            0x2B  /* handle:u16 len:u8 text[len] */
#define CMD_SET_TRIANGLE_PTS    0x2C  /* handle:u16 p1x p1y p2x p2y p3x p3y : u16 each */
#define CMD_SET_OBJ_TYPE        0x2D  /* handle:u16 type:u8 */

/* Textbox-specific (0x30 - 0x3F) */
#define CMD_SET_TB_ALIGNMENT    0x30  /* handle:u16 align:u8 */
#define CMD_SET_TB_WRAP         0x31  /* handle:u16 wrap_type:u8 wrap_index:u8 */
#define CMD_SET_TB_MARGIN       0x32  /* handle:u16 margin_x:u16 margin_y:u16 */
#define CMD_SET_TB_FILL_BG      0x33  /* handle:u16 fill_state:u8 r:u8 g:u8 b:u8 */
#define CMD_SET_TB_FONT         0x34  /* handle:u16 font_type:u8 */
#define CMD_SET_TB_SIZE         0x35  /* handle:u16 w:u16 h:u16 */
#define CMD_TB_CURSOR_ENABLE    0x36  /* handle:u16 blink_rate:u32 cursor_type:u8 */
#define CMD_TB_CURSOR_DISABLE   0x37  /* handle:u16 */
#define CMD_SET_TB_BORDER       0x38  /* handle:u16 thickness:u8 r:u8 g:u8 b:u8 */
#define CMD_ANIMATE_TEXT_REVEAL 0x39  /* handle:u16 delay:u16 len:u8 text[len] */

/* ---------------------------------------------------------------------- */
/* Scene & instance management (0x40 - 0x4F)                               */
/* ---------------------------------------------------------------------- */
#define CMD_NEW_SCENE           0x40  /* (no payload) -> RSP_SCENE_ID */
#define CMD_SET_ACTIVE_SCENE    0x41  /* scene_id:u16 */
#define CMD_DELETE_OBJECT       0x42  /* handle:u16 */
#define CMD_DELETE_SCENE        0x43  /* scene_id:u16 */
#define CMD_SET_TARGET_PANEL    0x44  /* panel_id:u8  (selects active output panel) */

/* ---------------------------------------------------------------------- */
/* System / misc (0x50 - 0x5F)                                              */
/* ---------------------------------------------------------------------- */
#define CMD_PING                0x50  /* -> RSP_ACK (liveness check) */
#define CMD_GET_FPS             0x51  /* -> RSP_FPS */
#define CMD_SET_REFRESH_DELAY   0x52  /* delay_ms:u16 */

/* ---------------------------------------------------------------------- */
/* Future FSM block reserved (0x60 - 0x6F) - not implemented in POC        */
/* ---------------------------------------------------------------------- */

/* ---------------------------------------------------------------------- */
/* Responses / async events (0x80+) - server -> host                       */
/* ---------------------------------------------------------------------- */
#define RSP_ACK                 0x80  /* status:u8 (0=ok) */
#define RSP_ERROR               0x81  /* status:u8  err_code:u8 */
#define RSP_HANDLE              0x82  /* handle:u16 */
#define RSP_SPRITE_ID           0x83  /* sprite_id:u16 */
#define RSP_SCENE_ID            0x84  /* scene_id:u16 */
#define RSP_FPS                 0x85  /* fps:u32 */
#define RSP_PONG                0x86  /* (no payload) */

/* Reserved for FSM events in future version */
#define EVT_FSM_EMIT            0x90

/* ---------------------------------------------------------------------- */
/* Error codes (used in RSP_ERROR payload byte 1)                          */
/* ---------------------------------------------------------------------- */
#define UCS_ERR_NONE            0x00
#define UCS_ERR_UNKNOWN_CMD     0x01
#define UCS_ERR_BAD_PAYLOAD_LEN 0x02
#define UCS_ERR_BAD_HANDLE      0x03
#define UCS_ERR_HANDLE_TABLE_FULL 0x04
#define UCS_ERR_CRC             0x05
#define UCS_ERR_SCENE_TABLE_FULL 0x06
#define UCS_ERR_BAD_SCENE_ID    0x07
#define UCS_ERR_SPRITE_ALLOC    0x08
#define UCS_ERR_BAD_SPRITE_ID   0x09
#define UCS_ERR_BAD_INSTANCE_ID 0x0A
#define UCS_ERR_BAD_PANEL_TYPE  0x0B
#define UCS_ERR_RENDERBUFFER_FAIL 0x0C
#define UCS_ERR_INTERNAL        0xFF

#ifdef __cplusplus
}
#endif

#endif /* UCANVAS_SERVER_PROTOCOL_H_ */