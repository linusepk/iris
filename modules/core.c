#include "rebound.h"
#include <iris.h>

#include <math.h>

imod_desc_t init(iris_state_t *state) {
    return (imod_desc_t) {
        .name = "Core"
    };
}

void terminate(iris_state_t *state) {}
void update(iris_state_t *state) {}
void fixed_update(iris_state_t *state) {}

/*=========================*/
// Entity system
/*=========================*/

entity_t *entity_new(iris_state_t *state) {
    for (u32_t i = 0; i < ENTITY_MAX; i++) {
        if (!state->ents[i].alive) {
            state->ents[i] = (entity_t) {
                .alive = true,
                .size = re_vec2s(1.0f),
            };
            return &state->ents[i];
        }
    }

    return NULL;
}

void entity_destroy(entity_t **ent) {
    (*ent)->alive = false;
    *ent = NULL;
}

/*=========================*/
// Input
/*=========================*/

input_t _iris_input = {0};

b8_t key_down(ikey_t key) {
    return _iris_input.keyboard[key].pressed && _iris_input.keyboard[key].new;
}
b8_t key_press(ikey_t key) {
    return _iris_input.keyboard[key].pressed;
}
b8_t key_up(ikey_t key) {
    return !_iris_input.keyboard[key].pressed && _iris_input.keyboard[key].new;
}

/*=========================*/
// Debugging
/*=========================*/

static void push_draw_call(iris_state_t *state, iris_debug_draw_call_t draw_call) {
    iris_debug_draw_call_t *_draw_call = re_arena_push(state->frame_arena, sizeof(iris_debug_draw_call_t));
    *_draw_call = draw_call;

    if (state->_debug_draw_calls == NULL) {
        state->_debug_draw_calls = _draw_call;
        state->_debug_draw_calls->last = _draw_call;
        return;
    }

    state->_debug_draw_calls->last->next = _draw_call;
    state->_debug_draw_calls->last = _draw_call;
}

void iris_debug_draw_line_points(iris_state_t *state, re_vec2_t a, re_vec2_t b, re_vec4_t color) {
    re_vec2_t diff = re_vec2_sub(b, a);
    f32_t length = re_vec2_magnitude(diff);
    f32_t angle = DEG(atan2f(diff.y, diff.x));

    push_draw_call(state, (iris_debug_draw_call_t) {
            .origin = re_vec2(0.5f, 0.0f),
            .pos = a,
            .size = re_vec2(length, 0.05f),
            .rotation = -angle,
            .color = color,
        });
}

void iris_debug_draw_line_length(iris_state_t *state, re_vec2_t pos, f32_t length, f32_t angle, re_vec4_t color) {
    push_draw_call(state, (iris_debug_draw_call_t) {
            .origin = re_vec2(0.5f, 0.0f),
            .pos = pos,
            .size = re_vec2(length, 0.05f),
            .rotation = -angle,
            .color = color,
        });
}

void iris_debug_draw_quad(iris_state_t *state, re_vec2_t pos, re_vec2_t size, f32_t rotation, re_vec4_t color) {
    push_draw_call(state, (iris_debug_draw_call_t) {
            .origin = re_vec2s(0.0f),
            .pos = pos,
            .size = size,
            .rotation = rotation,
            .color = color,
        });
}

void iris_debug_draw_quad_wireframe(iris_state_t *state, re_vec2_t pos, re_vec2_t size, f32_t rotation, re_vec4_t color) {
    // Bottom left
    re_vec2_t bl = re_vec2_divs(size, -2);
    bl = re_vec2_rotate(bl, rotation);
    bl = re_vec2_add(bl, pos);
    push_draw_call(state, (iris_debug_draw_call_t) {
            .origin = re_vec2(0.5f, 0.5f),
            .pos = bl,
            .size = re_vec2(size.x, 0.05f),
            .rotation = -rotation,
            .color = color,
        });

    // Top right
    re_vec2_t tr = re_vec2_divs(size, 2);
    tr = re_vec2_rotate(tr, rotation);
    tr = re_vec2_add(tr, pos);
    push_draw_call(state, (iris_debug_draw_call_t) {
            .origin = re_vec2(0.5f, 0.5f),
            .pos = tr,
            .size = re_vec2(size.x, 0.05f),
            .rotation = -rotation + 180,
            .color = color,
        });

    // Bottom right
    re_vec2_t br = re_vec2_divs(size, 2);
    br.y = -br.y;
    br = re_vec2_rotate(br, rotation);
    br = re_vec2_add(br, pos);
    push_draw_call(state, (iris_debug_draw_call_t) {
            .origin = re_vec2(0.5f, 0.5f),
            .pos = br,
            .size = re_vec2(size.x, 0.05f),
            .rotation = -rotation - 90,
            .color = color,
        });


    // Top left
    re_vec2_t tl = re_vec2_divs(size, 2);
    tl.x = -tl.x;
    tl = re_vec2_rotate(tl, rotation);
    tl = re_vec2_add(tl, pos);
    push_draw_call(state, (iris_debug_draw_call_t) {
            .origin = re_vec2(0.5f, 0.5f),
            .pos = tl,
            .size = re_vec2(size.x, 0.05f),
            .rotation = -rotation + 90,
            .color = color,
        });
}
