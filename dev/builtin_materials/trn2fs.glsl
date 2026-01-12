#version 430 core
layout(std140) uniform ubo {
    mat4 view;
    mat4 proj;
    vec4 sun_dir;
    vec4 sun_color;
    ivec3 snap_pos;
} UBO;

layout(std140) uniform _model {
    mat4 mod;
} MODEL;

out vec4 FragColor;
void main()
{
  FragColor = vec4(0.5, 0.4, 0.3, 1.0);
}
