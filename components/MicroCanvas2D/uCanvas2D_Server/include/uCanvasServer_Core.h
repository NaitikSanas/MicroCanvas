/**
 * @file uCanvasServer_Core.h
 * @brief Core dispatcher: handle table, scene table, frame -> uCanvas API mapping.
 *
 * Transport-agnostic. UART/SPI modules call uCanvasServer_HandleFrame() with
 * a decoded (cmd, payload, len) and this module does the rest, including
 * sending responses back via uCanvasServer_SendFrame() (currently routed
 * to the active transport's send function).
 */

#ifndef UCANVAS_SERVER_CORE_H_
#define UCANVAS_SERVER_CORE_H_

#include <stdint.h>
#include "uCanvasServer_Protocol.h"
#include "uCanvas_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the core dispatcher (handle table, scene table).
 *        Call after uCanvas rendering engine is already running
 *        (i.e. after New_uCanvas_Instance / Attach_Renderer).
 */
void uCanvasServer_Core_Init(void);

/**
 * @brief Entry point called by the transport layer for every validated frame.
 * @param cmd     Command opcode (see uCanvasServer_Protocol.h)
 * @param payload Pointer to payload bytes (may be NULL if len==0)
 * @param len     Payload length in bytes
 */
void uCanvasServer_HandleFrame(uint8_t cmd, const uint8_t* payload, uint16_t len);

/**
 * @brief Sends a response/event frame back to host.
 *        Implemented by binding to the active transport (UART/SPI) at link time
 *        or via a function pointer set with uCanvasServer_SetSendFunc().
 */
void uCanvasServer_SendFrame(uint8_t cmd, const uint8_t* payload, uint16_t len);

/**
 * @brief Registers the transport's send function. Call once during init,
 *        e.g. uCanvasServer_SetSendFunc(uCanvasServer_UART_SendFrame);
 */
typedef void (*uCanvasServer_SendFunc_t)(uint8_t cmd, const uint8_t* payload, uint16_t len);
void uCanvasServer_SetSendFunc(uCanvasServer_SendFunc_t fn);

/* ------------------------------------------------------------------ */
/* Handle table helpers (exposed in case FSM module needs lookups)     */
/* ------------------------------------------------------------------ */
uint16_t uCanvasServer_RegisterHandle(uCanvas_universal_obj_t* obj);
uCanvas_universal_obj_t* uCanvasServer_GetObj(uint16_t handle);
bool uCanvasServer_FreeHandle(uint16_t handle);

uint16_t uCanvasServer_RegisterScene(uCanvas_Scene_t* scene);
uCanvas_Scene_t* uCanvasServer_GetScene(uint16_t scene_id);
bool uCanvasServer_FreeScene(uint16_t scene_id);

#ifdef __cplusplus
}
#endif

#endif /* UCANVAS_SERVER_CORE_H_ */
