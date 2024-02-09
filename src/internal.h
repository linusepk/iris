#pragma once

#include "iris.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "batch_renderer.h"

typedef struct state_t state_t;
struct state_t {
    GLFWwindow *window;
    batch_renderer_t *br;

    iris_state_t iris;
};

