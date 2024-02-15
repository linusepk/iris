#pragma once

#include "iris.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "batch_renderer.h"

typedef struct module_t module_t;
struct module_t {
    module_t *next;

    imod_desc_t desc;
    imod_state_t state;

    imod_desc_t (*init)(iris_state_t *state);
    void (*terminate)(iris_state_t *state);
    void (*update)(iris_state_t *state);
    void (*fixed_update)(iris_state_t *state);

    void *dl_handle;
};

typedef struct state_t state_t;
struct state_t {
    re_arena_t *permanent_arena;

    re_arena_t *frame_arenas[2];
    u32_t frame_arena_current;

    re_str_t root_dir;

    GLFWwindow *window;
    batch_renderer_t *br;

    iris_state_t iris;

    module_t *module;
};

extern void modules_load(state_t *state);
extern void modules_unload(state_t *state);

extern void modules_init(state_t *state);
extern void modules_terminate(state_t *state);
extern void modules_update(state_t *state);
extern void modules_fixed_update(state_t *state);
