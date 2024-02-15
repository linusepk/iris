#include <rebound.h>

#include "batch_renderer.h"
#include "iris.h"
#include "internal.h"

#include <glad/gl.h>

// This should be called before polling for input
// because if it's called after the new flag
// will always be false thus never letting
// 'key_down()' return true.
static void input_reset(void) {
    for (u32_t i = 0; i < GLFW_KEY_LAST; i++) {
        _iris_input.keyboard[i].new = false;
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void) window;

    switch (action) {
        case GLFW_PRESS:
            _iris_input.keyboard[key].new = true;
            _iris_input.keyboard[key].pressed = true;
            break;

        case GLFW_RELEASE:
            _iris_input.keyboard[key].new = true;
            _iris_input.keyboard[key].pressed = false;
            break;

        default:
            break;
    }
}

void state_init(state_t *state, const char *argv0) {
    state->permanent_arena = re_arena_create(GB(4));

    for (u32_t i = 0; i < re_arr_len(state->frame_arenas); i++) {
        state->frame_arenas[i] = re_arena_create(GB(4));
    }
    state->frame_arena_current = 0;

    state->iris.permanent_arena = state->permanent_arena;
    state->iris.frame_arena = state->frame_arenas[0];

    state->root_dir = re_str_cstr(argv0);
    for (u32_t i = state->root_dir.len - 1; i > 0; i--) {
        if (state->root_dir.str[i] == '/') {
            state->root_dir = re_str_prefix(state->root_dir, i + 1);
            break;
        }
    }

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

static void render_entities(state_t *state) {
    const re_vec4_t color = re_vec4_hex1(0x212121ff);
    glClearColor(color.x, color.y, color.z, color.a);
    glClear(GL_COLOR_BUFFER_BIT);

    re_ivec2_t window_size;
    glfwGetWindowSize(state->window, &window_size.x, &window_size.y);
    batch_update(state->br, window_size, 16.0f);

    for (u32_t i = 0; i < ENTITY_MAX; i++) {
        if (!state->iris.ents[i].alive || !(state->iris.ents[i].flags & ENTITY_FLAG_RENDERABLE)) {
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

static void swap_frame_arena(state_t *state) {
    state->frame_arena_current = (state->frame_arena_current + 1) % re_arr_len(state->frame_arenas);
    re_arena_clear(state->frame_arenas[state->frame_arena_current]);
    state->iris.frame_arena = state->frame_arenas[state->frame_arena_current];
}

static void iris_reset_state(iris_state_t *state) {
    state->_debug_draw_calls = NULL;
}

static void process_debug_draw_calls(state_t *state) {
    for (iris_debug_draw_call_t *curr = state->iris._debug_draw_calls; curr != NULL; curr = curr->next) {
        batch_draw(
                state->br,
                curr->origin,
                curr->pos,
                curr->rotation,
                curr->size,
                curr->color,
                NULL);
    }
}

i32_t main(i32_t argc, char **argv) {
    re_init();

    state_t state = {0};
    state_init(&state, argv[0]);

    modules_load(&state);
    modules_init(&state);

    state.iris.player = entity_new(&state.iris);
    state.iris.player->flags |= ENTITY_FLAG_RENDERABLE;
    state.iris.player->renderer.color = re_vec4_hex1(0xebdbb2ff);

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

        modules_update(&state);

        // Rendering
        batch_begin(state.br);

        render_entities(&state);
        process_debug_draw_calls(&state);

        batch_end(state.br);
        batch_flush(state.br);



        input_reset();
        glfwSwapBuffers(state.window);
        glfwPollEvents();

        iris_reset_state(&state.iris);
        swap_frame_arena(&state);
    }

    modules_terminate(&state);

    modules_unload(&state);
    state_terminate(&state);

    re_terminate();
    return 0;
}
