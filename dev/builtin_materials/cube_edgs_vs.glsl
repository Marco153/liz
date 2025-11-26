#version 430 core


layout(std140) uniform ubo {
    mat4 view;
    mat4 proj;
} UBO;

layout(std140) uniform _model {
    mat4 mod;
} MODEL;

out vec3 frag_pos;

// =============================================================
// 24 hardcoded edge vertices (12 line segments × 2 endpoints)
// Cube spans [0,1] in X,Y,Z
// =============================================================
const vec3 cubeEdgeVerts[24] = vec3[24](
    // Bottom edges (y = 0)
    vec3(0,0,0), vec3(1,0,0),
    vec3(1,0,0), vec3(1,0,1),
    vec3(1,0,1), vec3(0,0,1),
    vec3(0,0,1), vec3(0,0,0),

    // Top edges (y = 1)
    vec3(0,1,0), vec3(1,1,0),
    vec3(1,1,0), vec3(1,1,1),
    vec3(1,1,1), vec3(0,1,1),
    vec3(0,1,1), vec3(0,1,0),

    // Vertical edges (connect bottom → top)
    vec3(0,0,0), vec3(0,1,0),
    vec3(1,0,0), vec3(1,1,0),
    vec3(1,0,1), vec3(1,1,1),
    vec3(0,0,1), vec3(0,1,1)
);

void main()
{
    vec3 local = cubeEdgeVerts[gl_VertexID];
    vec4 world = MODEL.mod * vec4(local, 1.0);
    frag_pos = world.xyz;

    gl_Position = UBO.proj * UBO.view * world;
}

