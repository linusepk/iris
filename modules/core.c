#include <iris.h>

imod_desc_t iris_module_entry(void) {
    return (imod_desc_t) {
        .name = "Core"
    };
}

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
