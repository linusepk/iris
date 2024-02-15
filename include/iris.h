#pragma once

#include <rebound.h>
#include <threads.h>

typedef struct iris_state_t iris_state_t;

typedef struct imod_state_t imod_state_t;
struct imod_state_t {
};

typedef struct imod_desc_t imod_desc_t;
struct imod_desc_t {
    const char *name;
};

// Module entry point.
// This will be called for every module loaded.
// If the entry point is not found, an error is printed and module is discarded.
typedef imod_desc_t (*imod_init_func_t)(void);

/*=========================*/
// Entity system
/*=========================*/

#define ENTITY_MAX 512

typedef enum {
    ENTITY_FLAG_RENDERABLE = 1 << 0,
} entity_flag_t;

typedef struct entity_t entity_t;
struct entity_t {
    entity_flag_t flags;
    b8_t alive;

    re_vec2_t position;
    re_vec2_t size;

    struct {
        re_vec4_t color;
    } renderer;
};

extern entity_t *entity_new(iris_state_t *state);
extern void entity_destroy(entity_t **ent);

/*=========================*/
// Input
/*=========================*/

// Stolen from GLFW.
typedef enum {
    KEY_SPACE = 32,
    KEY_APOSTROPHE = 39,
    KEY_COMMA = 44,
    KEY_MINUS = 45,
    KEY_PERIOD = 46,
    KEY_SLASH = 47,
    KEY_0 = 48,
    KEY_1 = 49,
    KEY_2 = 50,
    KEY_3 = 51,
    KEY_4 = 52,
    KEY_5 = 53,
    KEY_6 = 54,
    KEY_7 = 55,
    KEY_8 = 56,
    KEY_9 = 57,
    KEY_SEMICOLON = 59,
    KEY_EQUAL = 61,
    KEY_A = 65,
    KEY_B = 66,
    KEY_C = 67,
    KEY_D = 68,
    KEY_E = 69,
    KEY_F = 70,
    KEY_G = 71,
    KEY_H = 72,
    KEY_I = 73,
    KEY_J = 74,
    KEY_K = 75,
    KEY_L = 76,
    KEY_M = 77,
    KEY_N = 78,
    KEY_O = 79,
    KEY_P = 80,
    KEY_Q = 81,
    KEY_R = 82,
    KEY_S = 83,
    KEY_T = 84,
    KEY_U = 85,
    KEY_V = 86,
    KEY_W = 87,
    KEY_X = 88,
    KEY_Y = 89,
    KEY_Z = 90,
    KEY_LEFT_BRACKET = 91,
    KEY_BACKSLASH = 92,
    KEY_RIGHT_BRACKET = 93,
    KEY_GRAVE_ACCENT = 96,
    KEY_WORLD_1 = 161,
    KEY_WORLD_2 = 162,

    KEY_ESCAPE = 256,
    KEY_ENTER = 257,
    KEY_TAB = 258,
    KEY_BACKSPACE = 259,
    KEY_INSERT = 260,
    KEY_DELETE = 261,
    KEY_RIGHT = 262,
    KEY_LEFT = 263,
    KEY_DOWN = 264,
    KEY_UP = 265,
    KEY_PAGE_UP = 266,
    KEY_PAGE_DOWN = 267,
    KEY_HOME = 268,
    KEY_END = 269,
    KEY_CAPS_LOCK = 280,
    KEY_SCROLL_LOCK = 281,
    KEY_NUM_LOCK = 282,
    KEY_PRINT_SCREEN = 283,
    KEY_PAUSE = 284,
    KEY_F1 = 290,
    KEY_F2 = 291,
    KEY_F3 = 292,
    KEY_F4 = 293,
    KEY_F5 = 294,
    KEY_F6 = 295,
    KEY_F7 = 296,
    KEY_F8 = 297,
    KEY_F9 = 298,
    KEY_F10 = 299,
    KEY_F11 = 300,
    KEY_F12 = 301,
    KEY_F13 = 302,
    KEY_F14 = 303,
    KEY_F15 = 304,
    KEY_F16 = 305,
    KEY_F17 = 306,
    KEY_F18 = 307,
    KEY_F19 = 308,
    KEY_F20 = 309,
    KEY_F21 = 310,
    KEY_F22 = 311,
    KEY_F23 = 312,
    KEY_F24 = 313,
    KEY_F25 = 314,
    KEY_KP_0 = 320,
    KEY_KP_1 = 321,
    KEY_KP_2 = 322,
    KEY_KP_3 = 323,
    KEY_KP_4 = 324,
    KEY_KP_5 = 325,
    KEY_KP_6 = 326,
    KEY_KP_7 = 327,
    KEY_KP_8 = 328,
    KEY_KP_9 = 329,
    KEY_KP_DECIMAL = 330,
    KEY_KP_DIVIDE = 331,
    KEY_KP_MULTIPLY = 332,
    KEY_KP_SUBTRACT = 333,
    KEY_KP_ADD = 334,
    KEY_KP_ENTER = 335,
    KEY_KP_EQUAL = 336,
    KEY_LEFT_SHIFT = 340,
    KEY_LEFT_CONTROL = 341,
    KEY_LEFT_ALT = 342,
    KEY_LEFT_SUPER = 343,
    KEY_RIGHT_SHIFT = 344,
    KEY_RIGHT_CONTROL = 345,
    KEY_RIGHT_ALT = 346,
    KEY_RIGHT_SUPER = 347,
    KEY_MENU = 348,

    KEY_LAST = KEY_MENU,
} ikey_t;

extern b8_t key_down(u32_t key);
extern b8_t key_press(ikey_t key);
extern b8_t key_up(ikey_t key);

typedef struct input_t input_t;
struct input_t {
    struct {
        b8_t new;
        b8_t pressed;
    } keyboard[KEY_LAST];
};
extern input_t _iris_input;

/*=========================*/
// Debugging
/*=========================*/

typedef struct iris_debug_draw_call_t iris_debug_draw_call_t;
struct iris_debug_draw_call_t {
    iris_debug_draw_call_t *next;
    iris_debug_draw_call_t *last;

    re_vec2_t origin;
    re_vec2_t pos;
    re_vec2_t size;
    f32_t rotation;
    re_vec4_t color;
};

extern void iris_debug_draw_line_points(iris_state_t *state, re_vec2_t a, re_vec2_t b, re_vec4_t color);
extern void iris_debug_draw_line_length(iris_state_t *state, re_vec2_t pos, f32_t length, f32_t angle, re_vec4_t color);
extern void iris_debug_draw_quad(iris_state_t *state, re_vec2_t pos, re_vec2_t size, f32_t rotation, re_vec4_t color);
extern void iris_debug_draw_quad_wireframe(iris_state_t *state, re_vec2_t pos, re_vec2_t size, f32_t rotation, re_vec4_t color);

/*=========================*/
// State
/*=========================*/

struct iris_state_t {
    re_arena_t *permanent_arena;
    re_arena_t *frame_arena;

    entity_t ents[ENTITY_MAX];
    entity_t *player;

    f32_t dt;

    iris_debug_draw_call_t *_debug_draw_calls;
};
