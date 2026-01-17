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

out vec3 out_n;
out vec4 out_col;
float round_away_from_zero(float x)
{
    return sign(x) * ceil(abs(x));
}
int iround_away_from_zero(float x)
{
    return int(sign(x) * ceil(abs(x)));
}
vec4 sampleHeightDbg(vec2 w)
{
  float ch = float(MODEL.chunk_sz);

  vec2 cellf = floor(w / ch);
  ivec2 wi = ivec2(cellf);

  int layer = texelFetch(pageTable, wi, 0).r;
  vec2 local_uv = (w / ch) - cellf;
  //vec4 ret = vec4(layer / 512.0, local_uv, 1.0);
  float val =layer / 512.0;
  vec4 ret = vec4(local_uv, 0.0, 1.0);
  return ret;
  if (layer < 0) return vec4(0.0);


}
float sampleHeight(vec2 w)
{
  float ch = float(MODEL.chunk_sz);

  vec2 cellf = floor(w / ch);
  ivec2 wi = ivec2(cellf);

  int layer = texelFetch(pageTable, wi, 0).r;
  if (layer < 0) return 0.0;

  vec2 local_uv = (w / ch) - cellf;

  return texture(terrainTex, vec3(local_uv, layer)).r * 1000.0;
}
vec3 terrainNormal(vec2 w, float e )
{
    float hL = sampleHeight(w + vec2(-e, 0));
    float hR = sampleHeight(w + vec2( e, 0));
    float hD = sampleHeight(w + vec2(0, -e));
    float hU = sampleHeight(w + vec2(0,  e));

    return normalize(vec3(
        hL - hR,
        2.0 * e,
        hD - hU
    ));
}
void main()
{
  vec4 pos = vec4(vpos.xyz, 1.0);
  vec4 p = MODEL.mod * pos;
  vec4 sampl = vec4(UBO.snap_pos.xyz, 0.0) + p;
  float val = sampleHeight(sampl.xz);
  p.y += val;

  float step = float(MODEL.chunk_sz) / textureSize(terrainTex, 0).x;
  out_n = terrainNormal(sampl.xz, step);

  out_col = sampleHeightDbg(sampl.xz);

  gl_Position =  UBO.proj *  UBO.view * p;
}
