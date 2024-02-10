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
