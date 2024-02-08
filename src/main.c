#include <rebound.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "iris.h"
#include "batch_renderer.h"

i32_t main(i32_t argc, char **argv) {
    re_init();

    if (!glfwInit()) {
        re_log_error("Failed to load GLFW.");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Iris window", NULL, NULL);
    if (window == NULL) {
        re_log_error("Failed to create window.");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        re_log_error("Failed to load GL functions through GLAD.");
        glfwTerminate();
        return 1;
    }

    batch_renderer_t *br = batch_renderer_create(1024);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        re_ivec2_t window_size;
        glfwGetWindowSize(window, &window_size.x, &window_size.y);
        batch_update(br, window_size, 16.0f);

        batch_begin(br);

        batch_draw(
                br,
                re_vec2(0.0f, 0.0f),
                re_vec2(0.0f, 0.0f),
                0.0f,
                re_vec2s(1.0f),
                re_vec4s(1.0f),
                NULL);

        batch_end(br);
        batch_flush(br);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    batch_renderer_destroy(&br);

    glfwTerminate();

    re_terminate();
    return 0;
}
