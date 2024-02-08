#include "batch_renderer.h"

#include <errno.h>

#include <glad/gl.h>

static void read_file(const char *filepath, char *buffer, u32_t *size) {
    RE_ASSERT(size != NULL, "Can't provide a NULL size pointer.");

    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        re_log_error("Couldn't open file %s: %s", filepath, strerror(errno));
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp) + 1;
    fseek(fp, 0, SEEK_SET);

    if (buffer != NULL) {
        fread(buffer, sizeof(char), *size - 1, fp);
        buffer[*size - 1] = '\0';
    }
}

batch_renderer_t *batch_renderer_create(u32_t max_quad_count) {
    batch_renderer_t *renderer = re_malloc(sizeof(batch_renderer_t));

    renderer->quad_max = max_quad_count;
    renderer->quad_count = 0;

    batch_update(renderer, re_ivec2(1920, 1080), 16.0f);

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, &renderer->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ebo);
    u32_t indices_size = max_quad_count * 6 * sizeof(u32_t);
    renderer->indices= re_malloc(indices_size);
    u32_t j = 0;
    for (u32_t i = 0; i < max_quad_count * 6; i += 6) {
        renderer->indices[i + 0] = 0 + j;
        renderer->indices[i + 1] = 1 + j;
        renderer->indices[i + 2] = 2 + j;
    
        renderer->indices[i + 3] = 2 + j;
        renderer->indices[i + 4] = 3 + j;
        renderer->indices[i + 5] = 1 + j;
    
        j += 4;
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, renderer->indices, GL_STATIC_DRAW);

    glGenBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    u32_t vertices_size = max_quad_count * 4 * sizeof(vertex_t);
    renderer->vertices = re_malloc(vertices_size);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, NULL, GL_DYNAMIC_DRAW);

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(vertex_t), (const void *) re_offsetof(vertex_t, position));
    glEnableVertexAttribArray(0);
    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(vertex_t), (const void *) re_offsetof(vertex_t, uv));
    glEnableVertexAttribArray(1);
    // Color
    glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(vertex_t), (const void *) re_offsetof(vertex_t, color));
    glEnableVertexAttribArray(2);
    // Texture index
    glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(vertex_t), (const void *) re_offsetof(vertex_t, texture_index));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Shader
    i32_t success;
    char info_log[512];
    u32_t size;
    
    read_file("resources/shaders/vertex.glsl", NULL, &size);
    char *vertex_source = re_malloc(size);
    read_file("resources/shaders/vertex.glsl", vertex_source, &size);
    u32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const char **) &vertex_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        re_log_error("Vertex shader compilation error.\n%s", info_log);
    }
    re_free(vertex_source);

    read_file("resources/shaders/fragment.glsl", NULL, &size);
    char *fragment_source = re_malloc(size);
    read_file("resources/shaders/fragment.glsl", fragment_source, &size);
    u32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const char **) &fragment_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        re_log_error("Fragment shader compilation error.\n%s", info_log);
    }
    re_free(fragment_source);

    renderer->shader = glCreateProgram();
    glAttachShader(renderer->shader, vertex_shader);
    glAttachShader(renderer->shader, fragment_shader);
    glLinkProgram(renderer->shader);
    glGetProgramiv(renderer->shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(renderer->shader, 512, NULL, info_log);
        re_log_error("Shader linking error.\n%s", info_log);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glUseProgram(renderer->shader);
    i32_t samplers[MAX_TEXTURES] = {0};
    for (u32_t i = 0; i < MAX_TEXTURES; i++) {
        samplers[i] = i;
    }
    u32_t samplers_location = glGetUniformLocation(renderer->shader, "textures");
    glUniform1iv(samplers_location, MAX_TEXTURES, samplers);

    // Blank texture
    u8_t texture_data[3] = {255, 255, 255};
    renderer->textures[0] = texture_create(re_ivec2s(1), TEXTURE_FORMAT_RGB, texture_data, TEXTURE_SAMPLING_NEAREST);
    renderer->texture_count = 1;

    return renderer;
}

void batch_renderer_destroy(batch_renderer_t **renderer) {
    batch_renderer_t *r = *renderer;

    glDeleteVertexArrays(1, &r->vao);
    glDeleteBuffers(1, &r->ebo);
    glDeleteBuffers(1, &r->vbo);
    glDeleteProgram(r->shader);

    texture_destroy(&r->textures[0]);

    re_free(r->indices);
    re_free(r->vertices); 
    re_free(r);
    *renderer = NULL;
}

void batch_update(batch_renderer_t *renderer, re_ivec2_t screen_size, f32_t zoom) {
    renderer->screen_size = screen_size;
    renderer->zoom = zoom / 2.0f;
}

void batch_begin(batch_renderer_t *renderer) {
    renderer->quad_count = 0;
    renderer->texture_count = 1;
}

void batch_end(batch_renderer_t *renderer) {
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->quad_count * 4 * sizeof(vertex_t), renderer->vertices);
}

void batch_flush(batch_renderer_t *renderer) {
    glUseProgram(renderer->shader);

    for (u32_t i = 0; i < renderer->texture_count; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, renderer->textures[i]->handle);
    }

    re_ivec2_t screen_size = renderer->screen_size;
    f32_t aspect_ratio = (f32_t) screen_size.x / (f32_t) screen_size.y;
    re_mat4_t projection = re_mat4_orthographic_projection(
            -aspect_ratio * renderer->zoom, aspect_ratio * renderer->zoom,
            renderer->zoom, -renderer->zoom,
            -1.0f, 1.0f);
    u32_t projection_location = glGetUniformLocation(renderer->shader, "projection");
    glUniformMatrix4fv(projection_location, 1, false, (const f32_t *) &projection);

    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ebo);
    glDrawElements(GL_TRIANGLES, renderer->quad_count * 6, GL_UNSIGNED_INT, NULL);
}

extern void batch_draw_atlas(
        batch_renderer_t *renderer,
        re_vec2_t origin_offset,
        re_vec2_t position,
        f32_t rotation,
        re_vec2_t size,
        re_vec4_t color,
        texture_t *atlas,
        re_vec2_t uv_bottom_left,
        re_vec2_t uv_top_right) {
    if (renderer->quad_count == renderer->quad_max) {
        batch_end(renderer);
        batch_flush(renderer);
        batch_begin(renderer);
    }
    RE_ASSERT(renderer->quad_count < renderer->quad_max, "Too many quads.");
    if (renderer->texture_count == MAX_TEXTURES) { 
        batch_end(renderer);
        batch_flush(renderer);
        batch_begin(renderer);
    }
    RE_ASSERT(renderer->texture_count < MAX_TEXTURES, "Too many texutres.");

    u32_t texture_index = 0;
    b8_t found = false;
    for (u32_t i = 0; i < renderer->texture_count; i++) {
        if (renderer->textures[i]->handle == atlas->handle) {
            found = true;
            texture_index = i;
            break;
        }
    }
    if (!found) {
        renderer->textures[renderer->texture_count] = atlas;
        texture_index = renderer->texture_count;
        renderer->texture_count++;
    }

    const re_vec2_t vertex_offset[4] = {
        {-0.5f, -0.5f},
        { 0.5f, -0.5f},
        {-0.5f,  0.5f},
        { 0.5f,  0.5f}
    };

    re_vec2_t uv[4] = {
        {uv_bottom_left.x, uv_top_right.y}, // Top left
        uv_top_right, // Top right
        uv_bottom_left, // Bottom left
        {uv_top_right.x, uv_bottom_left.y}, // Bottom right
    };

    for (u32_t i = 0; i < 4; i++) {
        vertex_t *vert = &renderer->vertices[renderer->quad_count * 4 + i];

        // Scale
        vert->position = re_vec2_mul(vertex_offset[i], size);
        // Origin offset
        vert->position = re_vec2_add(vert->position, re_vec2_mul(origin_offset, size));
        // Rotate
        vert->position = re_vec2_rotate(vert->position, -rotation);
        // Position
        vert->position = re_vec2_add(vert->position, position);

        vert->uv = uv[i];
        vert->color = color;
        vert->texture_index = texture_index;
    }

    renderer->quad_count++;
}

void batch_draw(
        batch_renderer_t *renderer,
        re_vec2_t origin_offset,
        re_vec2_t position,
        f32_t rotation,
        re_vec2_t size,
        re_vec4_t color,
        texture_t *texture) {
    batch_draw_atlas(
            renderer,
            origin_offset,
            position,
            rotation,
            size,
            color,
            texture == NULL ? renderer->textures[0] : texture,
            re_vec2s(0.0f),
            re_vec2s(1.0f));
}

texture_t *texture_create(re_ivec2_t size, texture_format_t format, const u8_t *data, texture_sampling_t sampling) {
    texture_t *texture = re_malloc(sizeof(texture_t));
    texture->size = size;

    glGenTextures(1, &texture->handle);
    glBindTexture(GL_TEXTURE_2D, texture->handle);

    switch (sampling) {
        case TEXTURE_SAMPLING_LINEAR:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case TEXTURE_SAMPLING_NEAREST:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
    }


    u32_t gl_format;
    switch (format) {
        case TEXTURE_FORMAT_RGB:
            gl_format = GL_RGB;
            break;
        case TEXTURE_FORMAT_RGBA:
            gl_format = GL_RGBA;
            break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, gl_format, size.x, size.y, 0, gl_format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
}

void texture_destroy(texture_t **texture) {
    glDeleteTextures(1, &(*texture)->handle);
    re_free(*texture);
    *texture = NULL;
}
