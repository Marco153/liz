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
    int chunk_sz;
    int trn_sz;
} MODEL;

in vec3 out_n;
in vec4 out_col;
out vec4 FragColor;
void main()
{
  vec3 sun_dir = normalize(vec3(1.0, -0.6, 0.0));

  float d = -dot(sun_dir, out_n);
  FragColor = vec4(1.0) * d;
  //FragColor = vec4(out_n.xyz, 1.0);
  //FragColor = out_col;
}
