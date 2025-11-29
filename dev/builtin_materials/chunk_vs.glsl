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
out vec3 frag_pos;
out vec2 out_uv;
out vec3 normal;
flat out uvec2 tex_uv;
const vec3 face_offsets[6 * 6] = vec3[](
    // ============================================================
    // +X (RIGHT)
    // ============================================================
    vec3(1,0,0), vec3(1,1,0), vec3(1,1,1),
    vec3(1,1,1), vec3(1,0,1), vec3(1,0,0),

    // ============================================================
    // -X (LEFT)
    // ============================================================
    vec3(0,0,0), vec3(0,0,1), vec3(0,1,1),
    vec3(0,1,1), vec3(0,1,0), vec3(0,0,0),


    // ============================================================
    // +Y (TOP)
    // ============================================================
    vec3(0,1,1), vec3(1,1,1), vec3(1,1,0),
    vec3(1,1,0), vec3(0,1,0), vec3(0,1,1),

    // ============================================================
    // -Y (BOTTOM)
    // ============================================================
    vec3(0,0,0), vec3(1,0,0), vec3(1,0,1),
    vec3(1,0,1), vec3(0,0,1), vec3(0,0,0),

    // ============================================================
    // +Z (FRONT)
    // ============================================================
    vec3(0,0,1), vec3(1,0,1), vec3(1,1,1),
    vec3(1,1,1), vec3(0,1,1), vec3(0,0,1),

    // ============================================================
    // -Z (BACK)
    // ============================================================
    vec3(0,0,0), vec3(0,1,0), vec3(1,1,0),
    vec3(1,1,0), vec3(1,0,0), vec3(0,0,0)
);
const vec2 faces_uvs[6] = vec2[6](
    vec2(0,0),
    vec2(1,0),
    vec2(1,1),
    vec2(1,1),
    vec2(0,1),
    vec2(0,0)
);
const vec3 face_normals[6] = vec3[6](
    vec3(1,0,0),
    vec3(-1,0,0),
    vec3(0,1,0),
    vec3(0,-1,0),
    vec3(0,0,1),
    vec3(0,0,-1)
);

void main()
{
    // gl_VertexID goes 0..5*6*? depending on draw
    uint cube_index = gl_VertexID / 24;    // 24 vertices per cube (6 faces × 4)
    uint face_index_uniform = (gl_VertexID / 6);
    uint face_in_uniform = FACES.f[face_index_uniform];
    uint x = face_in_uniform & 0x7;
    uint y = (face_in_uniform >> 3) & 0x7;
    uint z = (face_in_uniform >> 6) & 0x7;
    uint face_dir = (face_in_uniform >> 13) & 0x7;
    uint corner_index = gl_VertexID % 6;
    uint tex_x = (face_in_uniform >> 24) & 0xf;
    uint tex_y = (face_in_uniform >> 28) & 0xf;
    tex_uv.x = tex_x;
    tex_uv.y = tex_y;


    vec3 local_vertex = face_offsets[face_dir * 6 + corner_index];

    local_vertex.x += x;
    local_vertex.y += y;
    local_vertex.z += z;
    vec3 world_pos = (MODEL.mod * vec4(local_vertex,1.0)).xyz;
    frag_pos = world_pos;
    out_uv = faces_uvs[corner_index];

    normal = face_normals[face_dir];

    gl_Position =  UBO.proj * UBO.view * vec4(world_pos.xyz, 1.0);
}
