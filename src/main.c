#include <rebound.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "iris.h"
#include "batch_renderer.h"

#include <dlfcn.h>

typedef struct state_t state_t;
struct state_t {
    GLFWwindow *window;
    batch_renderer_t *br;

    iris_state_t iris;
};

void state_init(state_t *state) {
    if (!glfwInit()) {
        re_log_error("Failed to load GLFW.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    state->window = glfwCreateWindow(800, 600, "Iris window", NULL, NULL);
    if (state->window == NULL) {
        re_log_error("Failed to create window.");
    }
    glfwMakeContextCurrent(state->window);
    glfwSwapInterval(0);

    if (!gladLoadGL(glfwGetProcAddress)) {
        re_log_error("Failed to load GL functions through GLAD.");
    }

    state->br = batch_renderer_create(1024);
}

void state_terminate(state_t *state) {
    batch_renderer_destroy(&state->br);
    glfwDestroyWindow(state->window);
    glfwTerminate();

    *state = (state_t) {0};
}

static void render(state_t *state) {
    glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    re_ivec2_t window_size;
    glfwGetWindowSize(state->window, &window_size.x, &window_size.y);
    batch_update(state->br, window_size, 16.0f);

    batch_begin(state->br);

    for (u32_t i = 0; i < ENTITY_MAX; i++) {
        if (!state->iris.ents[i].alive || (state->iris.ents[i].flags & ENTITY_FLAG_RENDERABLE) == 0) {
            continue;
        }

        batch_draw(
                state->br,
                re_vec2s(0.0f),
                state->iris.ents[i].position,
                0.0f,
                state->iris.ents[i].size,
                state->iris.ents[i].renderer.color,
                NULL);


    }

    batch_end(state->br);
    batch_flush(state->br);
}

static void calculate_delta_time(iris_state_t *state) {
    static f32_t curr = 0.0f, last = 0.0f;
    if (last == 0.0f) {
        last = re_os_get_time();
        return;
    }
    curr = re_os_get_time();
    state->dt = curr - last;
    last = curr;
}

i32_t main(i32_t argc, char **argv) {
    re_init();

    state_t state = {0};
    state_init(&state);

    entity_t *ent = entity_new(&state.iris);
    ent->flags |= ENTITY_FLAG_RENDERABLE;
    ent->renderer.color = re_vec4s(1.0f);

    f32_t timer = 0.0f;
    u32_t fps = 0;
    while (!glfwWindowShouldClose(state.window)) {
        calculate_delta_time(&state.iris);

        timer += state.iris.dt;
        if (timer >= 1.0f) {
            re_log_info("FPS: %u", fps);
            timer = 0.0f;
            fps = 0;
        }
        fps++;

        render(&state);
        ent->position.x += state.iris.dt * 2.0f;

        if (ent->position.x >= 4.0f) {
            entity_destroy(&ent);
            ent = entity_new(&state.iris);
            ent->flags |= ENTITY_FLAG_RENDERABLE;
            ent->renderer.color = re_vec4_hex1(0xe74c3cff);
        }

        glfwSwapBuffers(state.window);
        glfwPollEvents();
    }

    state_terminate(&state);

    re_terminate();
    return 0;
}
