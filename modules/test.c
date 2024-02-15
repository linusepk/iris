#include "rebound.h"
#include <iris.h>

imod_desc_t init(iris_state_t *state) {
    return (imod_desc_t) {
        .name = "Test"
    };
}

void terminate(iris_state_t *state) {}

void update(iris_state_t *state) {
    static entity_t *ent = NULL;

    if (key_down(KEY_T)) {
        if (ent == NULL) {
            ent = entity_new(state);
            ent->flags |= ENTITY_FLAG_RENDERABLE;
            ent->renderer.color = re_vec4_hex1(0x212121ff);
            ent->position.x = 1;
            ent->position.y = 3;
        } else {
            entity_destroy(&ent);
        }
    }

    iris_debug_draw_quad(state, re_vec2s(0.0f), re_vec2s(0.75f), 22.5f, re_vec4s(0.0f));
}

void fixed_update(iris_state_t *state) {}
