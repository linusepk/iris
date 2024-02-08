#pragma once

#include <rebound.h>

#define MAX_TEXTURES 32

/*=========================*/
// Textures
/*=========================*/

typedef enum {
    TEXTURE_FORMAT_RGB = 3,
    TEXTURE_FORMAT_RGBA,
} texture_format_t;

typedef enum {
    TEXTURE_SAMPLING_NEAREST,
    TEXTURE_SAMPLING_LINEAR
} texture_sampling_t;

typedef struct texture_t texture_t;

extern texture_t *texture_create(re_ivec2_t size, texture_format_t format, const u8_t *data, texture_sampling_t sampling);
extern void texture_destroy(texture_t **texture);

/*=========================*/
// Rendering API
/*=========================*/

struct texture_t {
    u32_t handle;
    re_ivec2_t size;
};

typedef struct vertex_t vertex_t;
struct vertex_t {
    re_vec2_t position;
    re_vec2_t uv;
    re_vec4_t color;
    f32_t texture_index;
};

typedef struct batch_renderer_t batch_renderer_t;
struct batch_renderer_t {
    u32_t quad_max;
    u32_t quad_count;

    f32_t zoom;
    re_ivec2_t screen_size;

    u32_t vao;
    u32_t ebo;
    u32_t vbo;
    u32_t shader;

    vertex_t *vertices;
    u32_t *indices;

    texture_t *textures[MAX_TEXTURES];
    u32_t texture_count;
};

extern batch_renderer_t *batch_renderer_create(u32_t max_quad_count);
extern void batch_renderer_destroy(batch_renderer_t **renderer);
extern void batch_update(batch_renderer_t *renderer, re_ivec2_t screen_size, f32_t zoom);

extern void batch_begin(batch_renderer_t *renderer);
extern void batch_end(batch_renderer_t *renderer);
extern void batch_flush(batch_renderer_t *renderer);

extern void batch_draw(
        batch_renderer_t *renderer,
        re_vec2_t origin_offset,
        re_vec2_t position,
        f32_t rotation,
        re_vec2_t size,
        re_vec4_t color,
        texture_t *texture);

extern void batch_draw_atlas(
        batch_renderer_t *renderer,
        re_vec2_t origin_offset,
        re_vec2_t position,
        f32_t rotation,
        re_vec2_t size,
        re_vec4_t color,
        texture_t *atlas,
        re_vec2_t texture_top_left,
        re_vec2_t texture_size);
