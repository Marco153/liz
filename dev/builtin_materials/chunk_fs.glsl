
#version 430 core
layout(std140) uniform ubo {
    mat4 view;
    mat4 proj;
} UBO;

layout(std140) uniform _model {
    mat4 mod;
} MODEL;

layout(std140, binding = 0) buffer faces {
  uint f [6 * 8 * 8 * 8];
} FACES;
out vec4 FragColor;
in vec2 frag_pos;
in vec2 out_uv;
uniform sampler2D tex;
void main(){
  FragColor =  texture(tex, out_uv);
}
