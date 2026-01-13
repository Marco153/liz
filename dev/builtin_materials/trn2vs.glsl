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
in vec3 vpos;
layout(binding=0)uniform sampler2DArray terrainTex;
layout(binding=1)uniform isampler2D pageTable;

float round_away_from_zero(float x)
{
    return sign(x) * ceil(abs(x));
}
int iround_away_from_zero(float x)
{
    return int(sign(x) * ceil(abs(x)));
}
float sampleHeight(vec2 w)
{
  int trn_sz = MODEL.trn_sz;
  int ch_sz  = MODEL.chunk_sz;

  ivec2 wi = ivec2(w.xy) / ch_sz;

  int layer = texelFetch(pageTable, wi, 0).r;

  if(layer == -1) return 0.0;

  vec2 local_uv = fract(w / float(ch_sz));

  return texture(terrainTex, vec3(local_uv, layer)).r;
}
void main()
{
  vec4 pos = vec4(vpos.xyz, 1.0);
  vec4 p = MODEL.mod * pos;
  float val = sampleHeight(p.xz);
  p.y += val;
  gl_Position =  UBO.proj *  UBO.view * p;
}
