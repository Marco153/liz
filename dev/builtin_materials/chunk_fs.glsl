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

layout(std430, binding = 0) buffer faces {
  uint f [6 * 8 * 8 * 8];
} FACES;
out vec4 FragColor;
out vec4 out_normal;
in vec2 frag_pos;
in vec3 normal;
in vec2 out_uv;
uniform sampler2D tex;
void main(){
  ivec2 uv = ivec2(out_uv.x * 16.0, out_uv.y * 16.0 + 32.0);

  FragColor =  texelFetch(tex, uv, 0);
  vec3 aux = normal;
  aux.x = (aux.x + 1.0) * 0.5;
  aux.y = (aux.y + 1.0) * 0.5;
  aux.z = (aux.z + 1.0) * 0.5;
  out_normal = vec4(normal, 1.0);
}
