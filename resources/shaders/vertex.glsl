#version 400 core
layout (location = 0) in vec2 v_pos;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec4 v_color;
layout (location = 3) in float v_texture_index;

out vec2 f_uv;
out vec4 f_color;
out float f_texture_index;

uniform mat4 projection;

void main() {
    f_uv = v_uv;
    f_color = v_color;
    f_texture_index = v_texture_index;

    gl_Position = projection * vec4(v_pos, 0.0, 1.0);
}
