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

typedef struct input_t input_t;
struct input_t {
    struct {
        b8_t new;
        b8_t pressed;
    } keyboard[GLFW_KEY_LAST];
};
static input_t _input = {0};

// This should be called before polling for input
// because if it's called after the new flag
// will always be false thus never letting
// 'key_down()' return true.
static void input_reset(void) {
    for (u32_t i = 0; i < GLFW_KEY_LAST; i++) {
        _input.keyboard[i].new = false;
    }
}

b8_t key_down(u32_t key) {
    return _input.keyboard[key].pressed && _input.keyboard[key].new;
}
b8_t key_press(u32_t key) {
    return _input.keyboard[key].pressed;
}
b8_t key_up(u32_t key) {
    return !_input.keyboard[key].pressed && _input.keyboard[key].new;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void) window;

    switch (action) {
        case GLFW_PRESS:
            _input.keyboard[key].new = true;
            _input.keyboard[key].pressed = true;
            break;

        case GLFW_RELEASE:
            _input.keyboard[key].new = true;
            _input.keyboard[key].pressed = false;
            break;

        default:
            break;
    }
}

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

    glfwSetKeyCallback(state->window, key_callback);

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

    entity_t *player = entity_new(&state.iris);
    player->flags |= ENTITY_FLAG_RENDERABLE;
    player->renderer.color = re_vec4s(1.0f);

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

        if (key_down(GLFW_KEY_R)) {
            entity_destroy(&player);
            player = entity_new(&state.iris);
            player->flags |= ENTITY_FLAG_RENDERABLE;
            player->renderer.color = re_vec4_hex1(0xe67e22ff);
        }

        const f32_t speed = 10.0f;

        re_vec2_t vel = {0};
        vel.x = key_press(GLFW_KEY_D) - key_press(GLFW_KEY_A);
        vel.y = key_press(GLFW_KEY_W) - key_press(GLFW_KEY_S);
        vel = re_vec2_normalize(vel);
        vel = re_vec2_muls(vel, speed * state.iris.dt);

        player->position = re_vec2_add(player->position, vel);

        render(&state);

        input_reset();
        glfwSwapBuffers(state.window);
        glfwPollEvents();
    }

    state_terminate(&state);

    re_terminate();
    return 0;
}
