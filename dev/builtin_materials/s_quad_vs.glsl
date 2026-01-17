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

out vec2 uv;

void main()
{
    // 2 triangles = 6 vertices
    const vec2 positions[6] = vec2[](
        vec2(-0.5, -0.5),
        vec2( 0.5, -0.5),
        vec2( 0.5,  0.5),

        vec2(-0.5, -0.5),
        vec2( 0.5,  0.5),
        vec2(-0.5,  0.5)
    );

    const vec2 uvs[6] = vec2[](
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(1.0, 1.0),

        vec2(0.0, 0.0),
        vec2(1.0, 1.0),
        vec2(0.0, 1.0)
    );

    vec2 pos = positions[gl_VertexID];
    uv = uvs[gl_VertexID];
    gl_Position = MODEL.mod * vec4(pos, 0.0, 1.0);
}
