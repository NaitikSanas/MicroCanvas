#include "uCanvas_Render_Common.h"
#include "uCanvas_Draw.h"

/**
 * Universal Object Drawing Implementation
 * 
 * This file contains the core drawing logic that converts
 * universal scene objects into framebuffer pixels.
 * Used by all renderer implementations.
 */

uint16_t IRAM_ATTR convertToRGB565(color_t color) {
    // Combine into RGB565 format
    return (((color.red * 31) / 255) << 11) | (((color.green * 63) / 255) << 5) | ((color.blue * 31) / 255);
}

void IRAM_ATTR draw_universal_object_to_target_render_buffer(uCanvas_universal_obj_t* obj, uCanvas2D_RenderBuffer_t* framebuffer, uCanvas2D_Display_Panel_t* panel) {
    uint16_t color = convertToRGB565(obj->properties.color);
    switch (obj->properties.type) {
        case RECTANGLE: {
            uCanvas2D_DrawRect(
                framebuffer,
                obj->properties.position.x,
                obj->properties.position.y,
                obj->width,
                obj->height,
                color,
                obj->properties.fill, obj->properties.thickness
            );
            break;
        }

        case ADV_TEXTBOX: {
            uCanvas_Draw_SFONT_Advanced_TextBox(framebuffer, obj);
            break;
        }

        case TEXTBOX: {
            // uCanvas_Draw_Text(framebuffer,obj->properties.position.x,obj->properties.position.y,obj->text,obj->textbox_properties->font_type,color,0x0000,obj->font_properties.Font_Draw_Direction,0);
            uCanvas_Draw_SFONT_TextBox(framebuffer, obj);
            break;
        }

        case CIRCLE: {
            uCanvas2D_DrawCircle(
                framebuffer,
                obj->properties.position.x,
                obj->properties.position.y,
                obj->r1,
                color,
                obj->properties.fill, obj->properties.thickness
            );
            break;
        }

        case LINE: {
            uCanvas2D_DrawLine(
                framebuffer,
                obj->properties.position.x + obj->point1.x,
                obj->properties.position.y + obj->point1.y,
                obj->properties.position.x + obj->point2.x,
                obj->properties.position.y + obj->point2.y,
                color,
                obj->properties.thickness
            );
            break;
        }

        case TRIANGLE: {
            Coordinate2D_t p1, p2, p3;
            p1.x = obj->point1.x + obj->properties.position.x;
            p1.y = obj->point1.y + obj->properties.position.y;
            p2.x = obj->point2.x + obj->properties.position.x;
            p2.y = obj->point2.y + obj->properties.position.y;
            p3.x = obj->point3.x + obj->properties.position.x;
            p3.y = obj->point3.y + obj->properties.position.y;

            uCanvas2D_DrawTriangle(
                framebuffer,
                p1.x, p1.y,
                p2.x, p2.y,
                p3.x, p3.y,
                color,
                obj->properties.fill, obj->properties.thickness
            );
            break;
        }

        case SPRITE2D: {
            uCanvas2D_DrawSprite(
                framebuffer,
                obj->properties.position.x,
                obj->properties.position.y,
                (const uint16_t*)obj->pixel_data,
                obj->width,
                obj->height,
                obj->color_format
            );
            break;
        }

        case ELLIPSE: {
            uCanvas2D_DrawEllipse(
                framebuffer,
                obj->properties.position.x,
                obj->properties.position.y,
                obj->r1,
                obj->r2,
                color,
                obj->properties.fill, obj->properties.thickness
            );
            break;
        }

        case WINDOW: {
            uCanvas2D_Instance_t* Window_instance = (uCanvas2D_Instance_t*)obj->ctx_data;
            LOCK_RESOURCE(Window_instance->render_buffer_lock);
            uCanvas2D_DrawSprite(
                framebuffer,
                obj->properties.position.x,
                obj->properties.position.y,
                obj->pixel_data,
                obj->width,
                obj->height,
                obj->color_format
            );
            UNLOCK_RESOURCE(Window_instance->render_buffer_lock);
            break;
        }

        default:
            break;
    }
}
