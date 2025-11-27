#version 430 core
layout(std140) uniform ubo {
    mat4 view;
    mat4 proj;
} UBO;

layout(std140) uniform _model {
    mat4 mod;
} MODEL;

out vec4 FragColor;
in vec2 frag_pos;
void main(){
  FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
