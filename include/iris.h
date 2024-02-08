#pragma once

#include <rebound.h>

// #define IRIS_API __attribute__((visibility("default")))
#define IRIS_API __attribute__((visibility("default")))

typedef struct imod_desc_t imod_desc_t;
struct imod_desc_t {
    const char *name;
};

// Module entry point.
// This will be called for every module loaded.
// If the entry point is not found, an error is printed and module is discarded.
typedef imod_desc_t (*imod_entry_func_t)(void);

typedef struct iris_state_t iris_state_t;

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

IRIS_API entity_t *entity_new(iris_state_t *state);
IRIS_API void entity_destroy(entity_t **ent);

struct iris_state_t {
    entity_t ents[ENTITY_MAX];
    f32_t dt;
};
