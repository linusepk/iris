#include <iris.h>

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

void iris_debug_draw_line_points(iris_state_t *state, re_vec2_t a, re_vec2_t b, re_vec4_t color) {
}

void iris_debug_draw_line_length(iris_state_t *state, re_vec2_t pos, f32_t length, f32_t angle, re_vec4_t color) {
}

void iris_debug_draw_quad(iris_state_t *state, re_vec2_t pos, re_vec2_t size, f32_t rotation, re_vec4_t color) {
    iris_debug_draw_call_t *draw_call = re_arena_push(state->frame_arena, sizeof(iris_debug_draw_call_t));
    *draw_call = (iris_debug_draw_call_t) {
        .pos = pos,
        .size = size,
        .rotation = rotation,
        .color = color,
    };

    if (state->_debug_draw_calls == NULL) {
        state->_debug_draw_calls = draw_call;
        state->_debug_draw_calls->last = draw_call;
        return;
    }

    state->_debug_draw_calls->last->next = draw_call;
    state->_debug_draw_calls->last = draw_call;
}

void iris_debug_draw_quad_wireframe(iris_state_t *state, re_vec2_t pos, re_vec2_t size, f32_t rotation, re_vec4_t color) {
}
