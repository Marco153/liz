#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uScene;
uniform sampler2D uDepth;
uniform sampler2D uNormal;
uniform float uNear;
uniform float uFar;
uniform vec4 sun_color;
uniform vec4 sun_dir;
float LinearizeDepth(float depth){
    // depth is [0..1] non-linear depth buffer value
    float z = depth * 2.0 - 1.0;           // Back to NDC
    return (2.0 * uNear * uFar) / (uFar + uNear - z * (uFar - uNear));
}

void main() {
  float depth = texture(uDepth, vUV).r;
  float linear = LinearizeDepth(depth);
  float viewZ01 = (linear - uNear) / (uFar - uNear);
  FragColor = vec4(vec3(viewZ01), 1.0);
  vec3 normal  = texture(uNormal, vUV).xyz;
  vec4 color  = texture(uScene, vUV);
  float d = dot(normal, sun_dir.xyz);
  d = clamp(d, 0.5, 1.0);
  FragColor = color * sun_color * d;
  FragColor.w = 1.0;
}
