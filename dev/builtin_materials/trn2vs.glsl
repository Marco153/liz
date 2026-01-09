#version 430 core

layout(std140) uniform ubo {
    mat4 view;
    mat4 proj;
    vec4 sun_dir;
    vec4 sun_color;
} UBO;

layout(std140) uniform _model {
    mat4 mod;
} MODEL;
in vec3 vpos;
uniform sampler2D heightmap;
void main()
{
  gl_Position =  UBO.proj *  UBO.view * MODEL.mod * vec4(vpos.xyz, 1.0);
}
