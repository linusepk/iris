#version 400 core
out vec4 frag_color;

in vec2 f_uv;
in vec4 f_color;
in float f_texture_index;

uniform sampler2D textures[32];

void main() {
    frag_color = texture(textures[int(f_texture_index)], f_uv) * f_color;
}
