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
in vec3 vpos;
uniform sampler2D heightmap[9];
float round_away_from_zero(float x)
{
    return sign(x) * ceil(abs(x));
}
int iround_away_from_zero(float x)
{
    return int(sign(x) * ceil(abs(x)));
}
float sampleHeight(vec2 p)
{
  float chunk_sz = 128.0;

  p /= chunk_sz;

  int x = 1 + iround_away_from_zero(p.x);
  int y = 1 + iround_away_from_zero(p.y);

  if(x < 0 || y < 0 || x > 2 || y > 2) return 0.0;

  float orig_x = float(x) - 0.5;
  float orig_y = float(y) - 0.5;
  vec2 localUV = vec2(p.x - orig_x, p.y - orig_y);

  return texture(heightmap[0], localUV).r;
}
void main()
{
  vec4 pos = vec4(vpos.xyz, 1.0);
  vec4 p = MODEL.mod * pos;
  float val = sampleHeight(p.xz);
  p.y += val;
  gl_Position =  UBO.proj *  UBO.view * p;
}
