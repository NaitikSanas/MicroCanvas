/**
 * @file  space_explorer_game.c
 * @brief Space Explorer – main game logic
 *
 * Structure
 * 
 *  1. Includes & Globals
 *  2. HUD helpers         (lives indicator, score, distance)
 *  3. Star field          (two-layer parallax)
 *  4. Enemy ships         (spawn, two-speed animation tasks)
 *  5. Player ship         (create, bullet system)
 *  6. Collision detection (bullet↔enemy, player↔enemy)
 *  7. Controller          (keyboard / encoder / IMU)
 *  8. Screens             (start screen, game-over overlay)
 *  9. Entry point         (Run_Space_Explorer_Game)
 */

#include "space_explorer_game.h"
#include "uCanvas_Draw.h"
#include "uCanvas2D_EK79007Port.h"
#include "uCanvas2D_ST7789_Port.h"
#include "uCanvas2D_Display_Setup.h"
#include "uCanvasRenderEngine.h"
#include "uCanvas2D_Acceleration.h"
#include "uCanvas_HID.h"

//  
// 1. Globals
//  

static uCanvas2D_Instance_t  g_canvas;

static spaceship_t           g_player;
static spaceship_t           g_enemies[MAX_ENEMIES];
static bullets_t             g_bullets;

static uCanvas_universal_obj_t *g_stars[MAX_STARS];
static uCanvas_universal_obj_t *g_lives[LIVES_MAX];

// HUD text objects
static uCanvas_universal_obj_t *g_hud_score;
static uCanvas_universal_obj_t *g_hud_distance;
static uCanvas_universal_obj_t *g_popup_plus;
static uCanvas_universal_obj_t *g_popup_value;
static uCanvas_universal_obj_t *g_overlay_text;   // "Game Over" / "Press SPACE"

// Game state
static int   g_current_score    = 0;
static int   g_last_score       = 0;
static int   g_lives_remaining  = LIVES_MAX;
static float g_distance_ly      = 0.0f;

// Input
static key_event_t                  g_key_event;
static uCanvas_Input_HID_Device_t   g_keyboard_device;
static rotary_encoder_t             g_encoder;

// Shared scratch buffer for sprintf
static char g_buf[64];

//  
// 2. HUD Helpers
//  

static void lives_indicator_create(void)
{
    for (int i = 0; i < LIVES_MAX; i++) {
        int x = (i * LIVES_DOT_GAP) + LIVES_DOT_ORIGIN_X;
        int y = LIVES_DOT_ORIGIN_Y;
        g_lives[i] = New_uCanvas_2DCircle(x, y, LIVES_DOT_RADIUS);
        uCanvas_Set_Color(g_lives[i], 255, 0, 0);
        uCanvas_Set_Fill(g_lives[i], FILL);
    }
}

static void lives_indicator_update(int count)
{
    for (int i = 0; i < LIVES_MAX; i++) {
        bool lit = (i < count);
        uint8_t r = lit ? 255 : 10;
        uCanvas_Set_Color(g_lives[i], r, 0, 0);
    }
}

static void hud_update_score(int score)
{
    snprintf(g_buf, sizeof(g_buf), "Score:%d", score);
    uCanvas_Set_Text(g_hud_score, g_buf);
}

static void hud_update_distance(float ly)
{
    snprintf(g_buf, sizeof(g_buf), "Travelled:%.1f(ly)  ", ly);
    uCanvas_Set_Text(g_hud_distance, g_buf);
}

static void hud_create(void)
{
    // Dark top bar background
    uCanvas_universal_obj_t *bg = New_uCanvas_2DRectangle(0, 0, 40, CANVAS_WIDTH);
    uCanvas_Set_Color(bg, 0, 0, 100);
    uCanvas_Set_Fill(bg, FILL);

    // Score (top-right)
    g_hud_score = New_uCanvas_2DTextbox(" ", CANVAS_WIDTH - 220, 5);
    uCanvas_Set_Color(g_hud_score, 255, 255, 0);
    uCanvas_Set_Textbox_Alignment(g_hud_score, TEXT_LEFT_ALIGNED);
    uCanvas_Set_Textbox_Wrap_Style(g_hud_score, TEXT_WRAP_PER_SET_WORD_LEN, 20);
    uCanvas_Set_TextBox_Margin(g_hud_score, 0, 0);
    uCanvas_Set_TextBox_Fill_Background(g_hud_score, NOFILL, 0, 0, 0);
    uCanvas_Set_TextBox_FontType(g_hud_score, SFONT_16);
    uCanvas_Set_TextBox_Size(g_hud_score, 420, 200);

    // Distance (top-center)
    g_hud_distance = New_uCanvas_2DTextbox("", CANVAS_WIDTH / 2 - 140, 5);
    uCanvas_Set_Color(g_hud_distance, 255, 255, 0);
    uCanvas_Set_Textbox_Alignment(g_hud_distance, TEXT_LEFT_ALIGNED);
    uCanvas_Set_Textbox_Wrap_Style(g_hud_distance, TEXT_WRAP_PER_SET_WORD_LEN, 20);
    uCanvas_Set_TextBox_Margin(g_hud_distance, 0, 0);
    uCanvas_Set_TextBox_Fill_Background(g_hud_distance, NOFILL, 0, 0, 0);
    uCanvas_Set_TextBox_FontType(g_hud_distance, SFONT_16);
    uCanvas_Set_TextBox_Size(g_hud_distance, 420, 200);

    // "+1" hit popup (appears at enemy position)
    g_popup_plus = New_uCanvas_2DTextbox("+1", 0, 0);
    uCanvas_Set_Color(g_popup_plus, 255, 255, 255);
    uCanvas_Set_Textbox_Alignment(g_popup_plus, TEXT_LEFT_ALIGNED);
    uCanvas_Set_Textbox_Wrap_Style(g_popup_plus, TEXT_WRAP_PER_SET_WORD_LEN, 20);
    uCanvas_Set_TextBox_Margin(g_popup_plus, 0, 0);
    uCanvas_Set_TextBox_Fill_Background(g_popup_plus, NOFILL, 0, 0, 0);
    uCanvas_Set_TextBox_FontType(g_popup_plus, SFONT_16);
    uCanvas_Set_TextBox_Size(g_popup_plus, 420, 200);

    // Current score value popup
    g_popup_value = New_uCanvas_2DTextbox("+1", 0, 0);
    uCanvas_Set_Color(g_popup_value, 0, 255, 0);
    uCanvas_Set_Textbox_Alignment(g_popup_value, TEXT_LEFT_ALIGNED);
    uCanvas_Set_Textbox_Wrap_Style(g_popup_value, TEXT_WRAP_PER_SET_WORD_LEN, 20);
    uCanvas_Set_TextBox_Margin(g_popup_value, 0, 0);
    uCanvas_Set_TextBox_Fill_Background(g_popup_value, NOFILL, 0, 0, 0);
    uCanvas_Set_TextBox_FontType(g_popup_value, SFONT_16);
    uCanvas_Set_TextBox_Size(g_popup_value, 420, 200);
}

//  
// 3. Star Field
//  

static void stars_init(void)
{
    for (int i = 0; i < MAX_STARS; i++) {
        int x = get_random_number(0, CANVAS_WIDTH);
        int y = get_random_number(0, CANVAS_HEIGHT);
        int r = get_random_number(0, 1);
        g_stars[i] = New_uCanvas_2DCircle(x, y, r);
        uCanvas_Set_Color(g_stars[i], 255, 255, get_random_number(0, 255));
        uCanvas_Set_Fill(g_stars[i], FILL);
    }
}

/** Scroll a contiguous range of stars downward and respawn above canvas. */
static void stars_scroll_range(int first, int last, int blink_chance)
{
    for (int i = first; i < last; i++) {
        if (g_stars[i]->properties.position.y < CANVAS_HEIGHT) {
            uCanvas_Set_Position_Y(g_stars[i], g_stars[i]->properties.position.y + 1);
            if (get_random_number(0, 10) == blink_chance) {
                uCanvas_Set_Visiblity(g_stars[i], INVISIBLE);
                uCanvas_Delay(1);
            }
            uCanvas_Set_Visiblity(g_stars[i], VISIBLE);
        } else {
            uCanvas_Set_Position(g_stars[i],
                                 get_random_number(0, CANVAS_WIDTH),
                                 get_random_number(-CANVAS_HEIGHT, 0));
            uCanvas_Set_Color(g_stars[i], 255, 255, get_random_number(100, 255));
            uCanvas_Set_Visiblity(g_stars[i], INVISIBLE);
        }
    }
}

/** Task: fast foreground stars (indices 0 … MAX_STARS/2-1). */
static void task_stars_foreground(void *arg)
{
    while (1) {
        stars_scroll_range(0, MAX_STARS / 2, 5);
        g_distance_ly += 0.005f;
        hud_update_distance(g_distance_ly);
        uCanvas_Delay(STARS_SLOW_SCROLL_RATE);
    }
}

/** Task: slow background stars (indices MAX_STARS/2 … MAX_STARS-1). */
static void task_stars_background(void *arg)
{
    while (1) {
        stars_scroll_range(MAX_STARS / 2, MAX_STARS, 5);
        uCanvas_Delay(STARS_FAST_SCROLL_RATE);
    }
}

//  
// 4. Enemy Ships
//  

static void enemies_randomize_positions(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        uCanvas_Set_Position(g_enemies[i].obj,
                             get_random_number(0, CANVAS_WIDTH),
                             get_random_number(-CANVAS_HEIGHT, 0));
        uCanvas_Set_Visiblity(g_enemies[i].obj, INVISIBLE);
    }
}

static void enemies_spawn(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        uint32_t *rgba = (uint32_t *)malloc(SHIP_ENEMY_WIDTH * SHIP_ENEMY_HEIGHT * sizeof(uint32_t));
        if (!rgba) {
            printf("[ERROR] enemy sprite malloc failed (index %d)\n", i);
            return;
        }

        uCanvas_Convert_RGB565A_to_ARGB8888((uint16_t *)ship_enemy, rgba,
                                             SHIP_ENEMY_WIDTH, SHIP_ENEMY_HEIGHT);

        sprite2D_t sprite;
        uCanvas_Compose_2DSprite_Obj(&sprite, rgba, SHIP_ENEMY_WIDTH, SHIP_ENEMY_HEIGHT,
                                     COLOR_ARGB8888);

        g_enemies[i].obj = New_uCanvas_2DSprite(&sprite, 0, 0);
        g_enemies[i].obj->point1 = (Coordinate2D_t){10,  0};
        g_enemies[i].obj->point2 = (Coordinate2D_t){ 0, 10};
        g_enemies[i].obj->point3 = (Coordinate2D_t){20, 10};
        uCanvas_Set_Position(g_enemies[i].obj,
                             get_random_number(0, CANVAS_WIDTH),
                             get_random_number(-CANVAS_HEIGHT, 0));
        uCanvas_Set_Fill(g_enemies[i].obj, FILL);

        uCanvas_Set_Color(g_enemies[i].obj,
                          get_random_number(150, 255),
                          get_random_number(150, 255),
                          get_random_number(0,   255));
    }
}

/** Scroll a range of enemies downward; re-spawn off-screen when they leave. */
static void enemies_scroll_range(int first, int last, int delay_ms)
{
    while (1) {
        for (int i = first; i < last; i++) {
            if (g_enemies[i].obj->properties.position.y < CANVAS_HEIGHT) {
                uCanvas_Set_Position_Y(g_enemies[i].obj, g_enemies[i].obj->properties.position.y + 1);
                uCanvas_Set_Visiblity(g_enemies[i].obj, VISIBLE);
            } else {
                uCanvas_Set_Position(g_enemies[i].obj,
                                     get_random_number(0, CANVAS_WIDTH),
                                     get_random_number(-CANVAS_HEIGHT, 0));
                uCanvas_Set_Visiblity(g_enemies[i].obj, INVISIBLE);
            }
        }
        uCanvas_Delay(delay_ms);
    }
}

static void task_enemies_group_a(void *arg) { enemies_scroll_range(0,               MAX_ENEMIES / 2,     4); }
static void task_enemies_group_b(void *arg) { enemies_scroll_range(MAX_ENEMIES / 2, MAX_ENEMIES / 2 + 1, 20); }

//  
// 5. Player Ship & Bullets
//  

static void player_ship_create(void)
{
    uint32_t *rgba = (uint32_t *)malloc(SHIP_WIDTH * SHIP_HEIGHT * sizeof(uint32_t));
    if (!rgba) {
        printf("[ERROR] player sprite malloc failed\n");
        return;
    }

    uCanvas_Convert_RGB565A_to_ARGB8888((uint16_t *)ship, rgba, SHIP_WIDTH, SHIP_HEIGHT);

    sprite2D_t sprite;
    uCanvas_Compose_2DSprite_Obj(&sprite, rgba, 40, 40, COLOR_ARGB8888);
    uCanvas_Sprite_Adjust_Contrast(&sprite, 400);

    g_player.obj         = New_uCanvas_2DSprite(&sprite, 0, 0);
    g_player.obj->point1 = (Coordinate2D_t){10,  0};
    g_player.obj->point2 = (Coordinate2D_t){ 0, 10};
    g_player.obj->point3 = (Coordinate2D_t){20, 10};
    uCanvas_Set_Fill(g_player.obj, FILL);
    g_player.state = 0;

    uCanvas_Set_Color(g_player.obj, 0, 150, 255);
    uCanvas_Set_Position(g_player.obj, CANVAS_WIDTH / 2, CANVAS_HEIGHT - SHIP_HEIGHT);
}

static void bullets_init_system(uint8_t bullets_per_trigger)
{
    g_bullets.bullets_per_trigger = bullets_per_trigger;
    g_bullets.active_count        = 0;

    for (int i = 0; i < bullets_per_trigger; i++) {
        g_bullets.obj[i] = New_uCanvas_2DRectangle(0, 0, 5, 3);
        uCanvas_Set_Visiblity(g_bullets.obj[i], INVISIBLE);
        uCanvas_Set_Fill(g_bullets.obj[i], FILL);
        uCanvas_Set_Color(g_bullets.obj[i], 255, 0, 0);
    }
}

static void bullets_fire(void)
{
    if (g_bullets.active_count != 0) return;

    for (int i = 0; i < g_bullets.bullets_per_trigger; i++) {
        uCanvas_Set_Position(g_bullets.obj[i],
                             g_player.obj->properties.position.x + 20,
                             g_player.obj->properties.position.y + (8 * i));
        uCanvas_Set_Visiblity(g_bullets.obj[i], VISIBLE);
        g_bullets.active_count++;
        uCanvas_Delay(1);
    }
}

/** Compute approximate centroid (world-space) of a sprite/triangle object. */
static inline void object_centroid(const uCanvas_universal_obj_t *o, int *cx, int *cy)
{
    *cx = (o->point1.x + o->point2.x + o->point3.x) / 3 + o->properties.position.x;
    *cy = (o->point1.y + o->point2.y + o->point3.y) / 3 + o->properties.position.y;
}

/** Check AABB overlap within COLLISION_THRESHOLD. */
static inline bool centroids_collide(int ax, int ay, int bx, int by)
{
    return (abs(ax - bx) < COLLISION_THRESHOLD) &&
           (abs(ay - by) < COLLISION_THRESHOLD);
}

static void enemy_explode_and_respawn(int enemy_idx, int bullet_idx)
{
    spaceship_t *e = &g_enemies[enemy_idx];

    // Advance sprite to center of hitbox for explosion effect
    uCanvas_Set_Position(e->obj,
                         e->obj->properties.position.x + (SHIP_ENEMY_WIDTH  / 2),
                         e->obj->properties.position.y + (SHIP_ENEMY_HEIGHT / 2));

    // Position popups near explosion
    uCanvas_Set_Position(g_popup_plus,
                         e->obj->properties.position.x + 15,
                         e->obj->properties.position.y + 5);
    uCanvas_Set_Position(g_popup_value,
                         e->obj->properties.position.x + 25,
                         e->obj->properties.position.y + 20);

    snprintf(g_buf, sizeof(g_buf), "%d", g_current_score);
    uCanvas_Set_Text(g_popup_value, g_buf);

    // Expanding circle explosion
    uCanvas_Set_Radius1(e->obj, 2);
    uCanvas_Set_Obj_Type(e->obj, CIRCLE);
    uCanvas_Set_Visiblity(g_popup_plus, VISIBLE);
    uCanvas_Set_Visiblity(g_popup_value, VISIBLE);
    uCanvas_Set_Color(e->obj, 255, 125, 0);

    for (int r = 0; r < SHIP_ENEMY_WIDTH / 2; r++) {
        uCanvas_Set_Radius1(e->obj, e->obj->r1 + 1);
        uCanvas_Delay(5);
    }

    // Reset enemy
    uCanvas_Set_Obj_Type(e->obj, SPRITE2D);
    uCanvas_Set_Visiblity(e->obj, INVISIBLE);
    uCanvas_Set_Color(e->obj,
                      get_random_number(150, 255),
                      get_random_number(150, 255),
                      get_random_number(0,   255));
    uCanvas_Set_Position(e->obj,
                         get_random_number(0, CANVAS_WIDTH),
                         get_random_number(-CANVAS_HEIGHT, 0));

    // Hide popups & bullet
    uCanvas_Set_Visiblity(g_popup_plus, INVISIBLE);
    uCanvas_Set_Visiblity(g_popup_value, INVISIBLE);
    uCanvas_Set_Visiblity(g_bullets.obj[bullet_idx], INVISIBLE);
    g_bullets.active_count--;
}

static void task_bullets_animate(void *arg)
{
    while (1) {
        for (int i = 0; i < g_bullets.bullets_per_trigger; i++) {
            if (g_bullets.obj[i]->properties.visiblity != VISIBLE) continue;

            // Move bullet upward
            if (g_bullets.obj[i]->properties.position.y > 0) {
                uCanvas_Set_Position_Y(g_bullets.obj[i], g_bullets.obj[i]->properties.position.y - 1);
            } else {
                uCanvas_Set_Visiblity(g_bullets.obj[i], INVISIBLE);
                g_bullets.active_count--;
                continue;
            }

            // Check bullet-vs-enemy collision
            int bx = g_bullets.obj[i]->properties.position.x;
            int by = g_bullets.obj[i]->properties.position.y;

            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (g_enemies[j].obj->properties.visiblity != VISIBLE) continue;

                int ex, ey;
                object_centroid(g_enemies[j].obj, &ex, &ey);

                if (centroids_collide(bx, by, ex, ey)) {
                    g_current_score++;
                    hud_update_score(g_current_score);
                    enemy_explode_and_respawn(j, i);
                    break;   // one bullet, one kill
                }
            }
        }
        uCanvas_Delay(pdMS_TO_TICKS(1));
    }
}

//  
// 6. Player–Enemy Collision (runs in its own task)
//  

/** Flash player red/blue five times to signal a hit. */
static void player_hit_flash(void)
{
    for (int i = 0; i < 5; i++) {
        uCanvas_Set_Visiblity(g_player.obj, INVISIBLE);
        uCanvas_Set_Color(g_player.obj, 255, 0, 0);
        uCanvas_Delay(70);
        uCanvas_Set_Visiblity(g_player.obj, VISIBLE);
        uCanvas_Set_Color(g_player.obj, 0, 0, 255);
        uCanvas_Delay(70);
    }
}

static void game_reset(void)
{
    g_distance_ly   = 0.0f;
    g_last_score    = g_current_score;
    g_current_score = 0;
    g_lives_remaining = LIVES_MAX;

    lives_indicator_update(g_lives_remaining);
    enemies_randomize_positions();
}

static void show_game_over_screen(void)
{
    uCanvas_Set_Position_X(g_overlay_text, 150);
    uCanvas_Set_Visiblity(g_overlay_text, VISIBLE);
    uCanvas_Set_Text(g_overlay_text, "");
    uCanvas_Animate_Text_Reveal(g_overlay_text, "Game Over", 50);
    uCanvas_Delay(2500);

    // Wait for SPACE to restart
    while (1) {
        enemies_randomize_positions();
        uCanvas_Set_Text(g_overlay_text, "Press SPACE for Restart");
        g_distance_ly = 0.0f;
        if (g_key_event.key_char == ' ') {
            uCanvas_Set_Visiblity(g_overlay_text, INVISIBLE);
            break;
        }
        uCanvas_Delay(1);
    }
}

static void task_player_enemy_collision(void *arg)
{
    while (1) {
        int px, py;
        object_centroid(g_player.obj, &px, &py);

        for (int i = 0; i < MAX_ENEMIES; i++) {
            int ex, ey;
            object_centroid(g_enemies[i].obj, &ex, &ey);

            if (!centroids_collide(px, py, ex, ey)) continue;

            // Hit!
            if (g_lives_remaining > 0) {
                g_lives_remaining--;
                lives_indicator_update(g_lives_remaining);
                printf("[GAME] Lives remaining: %d\n", g_lives_remaining);
            } else {
                game_reset();
                show_game_over_screen();
            }

            player_hit_flash();
            enemies_randomize_positions();
            break;
        }

        uCanvas_Send_Refresh_Signal_To_Renderer(&g_canvas);
        uCanvas_Delay(1);
    }
}

//  
// 7. Controller
//  

static void keyboard_event_cb(uCanvasInputDeviceType_t type,
                               key_event_t *ev,
                               void *user_data)
{
    g_key_event = *ev;

    if (ev->state == KEY_STATE_PRESSED)
        printf("[INPUT] Key pressed:  '%c'\n", ev->key_char);
    else
        printf("[INPUT] Key released: '%c'\n", ev->key_char);
}

static void task_controller(void *arg)
{
    while (1) {
#if USE_USB_KEYBOARD
        if (g_key_event.state == KEY_STATE_PRESSED) {
            switch (g_key_event.key_char) {
                case 'd':
                    if (g_player.obj->properties.position.x < CANVAS_WIDTH)
                        uCanvas_Set_Position_X(g_player.obj, g_player.obj->properties.position.x + 1);
                    break;
                case 'a':
                    if (g_player.obj->properties.position.x > 0)
                        uCanvas_Set_Position_X(g_player.obj, g_player.obj->properties.position.x - 1);
                    break;
                case ' ':
                    bullets_fire();
                    break;
                default:
                    break;
            }
        }
#endif

#if USE_ROTORY_ENCODER
        uCanvas_rotary_encoder_read(&g_encoder);

        if (!g_encoder.sw_state)
            bullets_fire();

        encoder_state_t enc = uCanvas_rotary_encoder_get_state(&g_encoder);

        if (uCanvas_Get_PushbuttonState(PB1) || enc == ENCODER_CW) {
            if (g_player.obj->properties.position.x < CANVAS_WIDTH)
                uCanvas_Set_Position_X(g_player.obj, g_player.obj->properties.position.x + 1);
        }
        if (uCanvas_Get_PushbuttonState(PB2) || enc == ENCODER_CCW) {
            if (g_player.obj->properties.position.x > 0)
                uCanvas_Set_Position_X(g_player.obj, g_player.obj->properties.position.x - 1);
        }
#endif
        uCanvas_Delay(1);
    }
}

//  
// 8. Screens
//  

static uCanvas_universal_obj_t *textbox_create(const char *text,
                                                int x, int y,
                                                uint8_t r, uint8_t g, uint8_t b,
                                                int w, int h)
{
    uCanvas_universal_obj_t *tb = New_uCanvas_2DTextbox(text, x, y);
    uCanvas_Set_Color(tb, r, g, b);
    uCanvas_Set_Textbox_Alignment(tb, TEXT_LEFT_ALIGNED);
    uCanvas_Set_Textbox_Wrap_Style(tb, TEXT_WRAP_PER_SET_WORD_LEN, 20);
    uCanvas_Set_TextBox_Margin(tb, 0, 0);
    uCanvas_Set_TextBox_Fill_Background(tb, NOFILL, 0, 0, 0);
    uCanvas_Set_TextBox_FontType(tb, SFONT_16);
    uCanvas_Set_TextBox_Size(tb, w, h);
    return tb;
}

static void show_start_screen(void)
{
    uCanvas_universal_obj_t *title_space = textbox_create(
        "Space",    CANVAS_WIDTH / 4,       0, 255, 255, 0, 500, 200);
    uCanvas_universal_obj_t *title_exp   = textbox_create(
        "Explorer", CANVAS_WIDTH / 5 + 100, 0, 255, 255, 0, 420, 200);

    // Overlay text reused later for "Game Over"
    g_overlay_text = textbox_create("", CANVAS_WIDTH / 4 - 100, 0, 255, 255, 255, 420, 200);
    uCanvas_Set_Position_Y(g_overlay_text, 70);
    uCanvas_Set_Visiblity(g_overlay_text, INVISIBLE);

    // Animate title sliding down from above
    uCanvas_Set_Position_Y(title_space, -32);
    uCanvas_Set_Position_Y(title_exp, -32);

    for (int i = 0; i < CANVAS_HEIGHT / 2; i++) {
        uCanvas_Set_Position_Y(title_space, title_space->properties.position.y + 1);
        uCanvas_Set_Position_Y(title_exp, title_exp->properties.position.y + 1);
        uCanvas_Delay(10);
    }

    uCanvas_Set_Visiblity(g_overlay_text, VISIBLE);
    uCanvas_Animate_Text_Reveal(g_overlay_text, "Press SPACE to Play", 50);

    // Wait for SPACE
    while (1) {
#if USE_USB_KEYBOARD
        if (g_key_event.key_char == ' ') break;
#elif USE_ROTORY_ENCODER
        uCanvas_rotary_encoder_read(&g_encoder);
        if (!g_encoder.sw_state) break;
#endif
        uCanvas_Delay(10);
    }

    uCanvas_Set_Visiblity(title_space, INVISIBLE);
    uCanvas_Set_Visiblity(title_exp, INVISIBLE);
    uCanvas_Set_Visiblity(g_overlay_text, INVISIBLE);
}

static void task_fps_monitor(void *arg)
{
    uCanvas_universal_obj_t *fps_tb = textbox_create(
        "", CANVAS_WIDTH - 150, CANVAS_HEIGHT - 32, 255, 255, 255, 420, 200);

    while (1) {
        snprintf(g_buf, sizeof(g_buf), "FPS:%lld", uCanvas_Get_FPS(&g_canvas));
        uCanvas_Set_Text(fps_tb, g_buf);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

//  
// 9. Entry Point
//  

void Run_Space_Explorer_Game(void)
{
    //  Input setup 
#if USE_USB_KEYBOARD
    g_keyboard_device.EventListenerCallback = keyboard_event_cb;
    uCanvas_Get_HID_Device(&g_keyboard_device);
    g_keyboard_device.uCanvas_Input_HID_Device_Init();
#endif
    Example_uCanvas_Instance_Setup();

    //  Peripheral setup 
#if USE_IMU_DIR_CONTROL
    uCanvas_Initialize_IMU_Device(42, 41);
    uCanvas_IMU_Set_Tilt_Detection_Parameters(7, 2);
#endif
    uCanvas_Init_PushButton(PB1);
    uCanvas_Init_PushButton(PB2);
    uCanvas_rotary_encoder_init(&g_encoder, ENC_A, ENC_B, ENC_SW);

    //  Game object init 
    stars_init();
    bullets_init_system(10);
    player_ship_create();

    //  Launch background tasks 
    uCanvas_Add_Task(task_stars_foreground, NULL, 0);
    uCanvas_Add_Task(task_stars_background, NULL, 0);
    uCanvas_Add_Task((void (*))task_fps_monitor, NULL, 0);

    //  Start screen (blocks until SPACE) 
    show_start_screen();

    //  Spawn enemies & launch gameplay tasks 
    enemies_spawn();

    uCanvas_Add_Task(task_enemies_group_a,           NULL,      1);
    uCanvas_Add_Task(task_enemies_group_b,           NULL,      1);
    uCanvas_Add_Task((void (*))task_bullets_animate, &g_bullets, 0);
    uCanvas_Add_Task((void (*))task_controller,      NULL,      0);
    uCanvas_Add_Task((void (*))task_player_enemy_collision, NULL, 0);

    //  HUD & lives display 
    hud_create();
    lives_indicator_create();
    lives_indicator_update(LIVES_MAX);

}

