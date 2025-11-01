// #define USE_TEXT_EDITOR
#include "include/vulkan_includes/vulkan/vulkan_core.h"
#include <assimp/material.h>
#define RAD_TO_DEG 57.29577
#define DEG_TO_RAD (3.14159265f / 180.0f)
#define LINUX
#define RENDERER_VULKAN

#ifdef LINUX
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h> //For PATH_MAX
#include <pthread.h>
#include <signal.h> //For PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vulkan/vulkan.h>
struct memory_watch {
  int address;
  int prev_val;
};

int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }
void TerminateProcess(int val, int val2) { kill(val, SIGKILL); }
void ExitProcess(int val) {
  *(int *)0 = 0;
  _exit(val);
}
#else
#include <dsound.h>
#include <sndfile.h> // Library for reading WAV files
#include <xaudio2.h>
#endif

int clamp(int v, int min, int max) {
  if (v <= min)
    return min;
  if (v >= max)
    return max;
  return v;
}
enum key_enum {
  _KEY_LEFT,
  _KEY_RIGHT,
  _KEY_DOWN,
  _KEY_UP,
  _KEY_ACT0,
  _KEY_ACT1,
  _KEY_ACT2,
  _KEY_ACT3,
  _KEY_JMP,
  _KEY_DEL,
  _KEY_SHIFT,
  _KEY_TAB,
  _KEY_ALT,
  _KEY_LCTRL,
  _KEY_A,
  _KEY_S,
  _KEY_D,
  _KEY_F,
  _KEY_Q,
  _KEY_E,
  _KEY_W,
  _KEY_I,
  _KEY_P,
  _KEY_Z,
  _KEY_X,
  _KEY_C,
  _KEY_V,
  _KEY_B,
  _KEY_N,
  _KEY_M,
  _KEY_ESCAPE,
  _KEY_SPACE,
  _KEY_F1,
  _KEY_F2,
  _KEY_F3,
  _KEY_F4,
  _KEY_F5,
  _KEY_F6,
  _KEY_F7,
  _KEY_F8,
  _KEY_F9,
  _KEY_F10,
  _KEY_F11,
  _KEY_F12,
  _KEY_ENTER,
  _KEY_K,
  _KEY_0,
  _KEY_1,
  _KEY_2,
  _KEY_3,
  _KEY_4,
  _KEY_5,
  _KEY_6,
  _KEY_7,
  _KEY_8,
  _KEY_9,
};

// #include <editor/TextEditor.cpp>
#define GLEW_STATIC
#include <GL/glew.h>
// #include <glad/glad.h>
// #include <glad/glad.c>
#include "../include/GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#ifdef LINUX
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#endif
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <portaudio/include/portaudio.h>

struct v4 {
  float x;
  float y;
  float z;
  float w;

  v4 mul(float m) {
    v4 ret;
    ret.x = x * m;
    ret.y = y * m;
    ret.z = z * m;
    return ret;
  }
  v4 operator*(float f) {
    v4 ret;
    ret.x = this->x * f;
    ret.y = this->y + f;
    return ret;
  }
  v4 operator-(v4 &other) {
    v4 ret;
    ret.x = this->x - other.x;
    ret.y = this->y - other.y;
    return ret;
  }
  v4 operator+(v4 &other) {
    v4 ret;
    ret.x = this->x + other.x;
    ret.y = this->y + other.y;
    return ret;
  }
  ImVec2 IM() {
    ImVec2 ret;
    ret.x = this->x;
    ret.y = this->y;
    return ret;
  }
  float dot(v4 &other) { return x * other.x + y * other.y + z * other.y; }
  float len(v4 &other) { return 1.0; }
};
struct v3 {
  float x;
  float y;
  float z;

  v3 mul(float m) {
    v3 ret;
    ret.x = x * m;
    ret.y = y * m;
    ret.z = z * m;
    return ret;
  }
  v3 operator*(float f) {
    v3 ret;
    ret.x = this->x * f;
    ret.y = this->y + f;
    return ret;
  }
  v3 operator-(v3 &other) {
    v3 ret;
    ret.x = this->x - other.x;
    ret.y = this->y - other.y;
    return ret;
  }
  v3 operator+(v3 &other) {
    v3 ret;
    ret.x = this->x + other.x;
    ret.y = this->y + other.y;
    return ret;
  }
  ImVec2 IM() {
    ImVec2 ret;
    ret.x = this->x;
    ret.y = this->y;
    return ret;
  }
  float dot(v3 &other) { return x * other.x + y * other.y + z * other.y; }
  float len(v3 &other) { return 1.0; }
};
#if defined(_MSC_VER) && (_MSC_VER >= 1900) &&                                 \
    !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
#include "compile.cpp"
#include "memory.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <iostream>
#include <stb_image_write.h>
#include <vector>
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"
#include "sort.cpp"
#include <fstream>

// gpt generated code
struct Vec3 {
  float x, y, z;
  Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
  Vec3 operator+(Vec3 v) const { return {x + v.x, y + v.y, z + v.z}; }
  Vec3 operator-(Vec3 v) const { return {x - v.x, y - v.y, z - v.z}; }
  float length() const { return std::sqrt(x * x + y * y + z * z); }
  Vec3 normalized() const {
    float l = length();
    return {x / l, y / l, z / l};
  }
  Vec3() {}
  Vec3(float _x, float _y, float _z) {
    x = _x;
    y = _y;
    z = _z;
  }
};
Vec3 rotate(const Vec3 &v, const Vec3 &axis, float angle) {
  // Normalize the axis
  Vec3 a = axis.normalized();

  // Compute rotation components
  float cos_theta = std::cos(angle);
  float sin_theta = std::sin(angle);

  // Rodrigues' rotation formula
  Vec3 term1 = v * cos_theta;
  Vec3 term2 = a * (a.x * v.x + a.y * v.y + a.z * v.z) * (1 - cos_theta);
  Vec3 term3 = Vec3{a.y * v.z - a.z * v.y, a.z * v.x - a.x * v.z,
                    a.x * v.y - a.y * v.x} *
               sin_theta;

  return term1 + term2 + term3;
}

struct Mat4 {
  float m[16]; // Column-major 4x4 matrix

  Vec3 operator*(Vec3 v) const {
    return {m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12],
            m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13],
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14]};
  }
  Vec3 multiplyPoint(float x, float y, float z, float w) const {
    float rx = m[0] * x + m[4] * y + m[8] * z + m[12] * w;
    float ry = m[1] * x + m[5] * y + m[9] * z + m[13] * w;
    float rz = m[2] * x + m[6] * y + m[10] * z + m[14] * w;
    float rw = m[3] * x + m[7] * y + m[11] * z + m[15] * w;
    if (rw != 0.0f) {
      rx /= rw;
      ry /= rw;
      rz /= rw;
    }
    return {rx, ry, rz};
  }
};
Mat4 Inverse(const Mat4 &m) {
  Mat4 inv;
  const float *a = m.m;

  inv.m[0] = a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] +
             a[9] * a[7] * a[14] + a[13] * a[6] * a[11] - a[13] * a[7] * a[10];

  inv.m[4] = -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] +
             a[8] * a[6] * a[15] - a[8] * a[7] * a[14] - a[12] * a[6] * a[11] +
             a[12] * a[7] * a[10];

  inv.m[8] = a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] +
             a[8] * a[7] * a[13] + a[12] * a[5] * a[11] - a[12] * a[7] * a[9];

  inv.m[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] +
              a[8] * a[5] * a[14] - a[8] * a[6] * a[13] - a[12] * a[5] * a[10] +
              a[12] * a[6] * a[9];

  inv.m[1] = -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] +
             a[9] * a[2] * a[15] - a[9] * a[3] * a[14] - a[13] * a[2] * a[11] +
             a[13] * a[3] * a[10];

  inv.m[5] = a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] +
             a[8] * a[3] * a[14] + a[12] * a[2] * a[11] - a[12] * a[3] * a[10];

  inv.m[9] = -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] -
             a[8] * a[3] * a[13] - a[12] * a[1] * a[11] + a[12] * a[3] * a[9];

  inv.m[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] +
              a[8] * a[2] * a[13] + a[12] * a[1] * a[10] - a[12] * a[2] * a[9];

  inv.m[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] +
             a[5] * a[3] * a[14] + a[13] * a[2] * a[7] - a[13] * a[3] * a[6];

  inv.m[6] = -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] -
             a[4] * a[3] * a[14] - a[12] * a[2] * a[7] + a[12] * a[3] * a[6];

  inv.m[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] +
              a[4] * a[3] * a[13] + a[12] * a[1] * a[7] - a[12] * a[3] * a[5];

  inv.m[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] -
              a[4] * a[2] * a[13] - a[12] * a[1] * a[6] + a[12] * a[2] * a[5];

  inv.m[3] = -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] -
             a[5] * a[3] * a[10] - a[9] * a[2] * a[7] + a[9] * a[3] * a[6];

  inv.m[7] = a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] +
             a[4] * a[3] * a[10] + a[8] * a[2] * a[7] - a[8] * a[3] * a[6];

  inv.m[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] -
              a[4] * a[3] * a[9] - a[8] * a[1] * a[7] + a[8] * a[3] * a[5];

  inv.m[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] +
              a[4] * a[2] * a[9] + a[8] * a[1] * a[6] - a[8] * a[2] * a[5];

  float det =
      a[0] * inv.m[0] + a[1] * inv.m[4] + a[2] * inv.m[8] + a[3] * inv.m[12];

  if (det == 0) {
    std::cerr << "Matrix inversion failed, determinant is zero.\n";
    return m; // Return original as fallback
  }

  det = 1.0f / det;

  for (int i = 0; i < 16; i++) {
    inv.m[i] *= det;
  }

  return inv;
}
Mat4 Multiply(const Mat4 &a, const Mat4 &b) {
  Mat4 result = {};
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      result.m[row * 4 + col] = a.m[row * 4 + 0] * b.m[0 * 4 + col] +
                                a.m[row * 4 + 1] * b.m[1 * 4 + col] +
                                a.m[row * 4 + 2] * b.m[2 * 4 + col] +
                                a.m[row * 4 + 3] * b.m[3 * 4 + col];
    }
  }
  return result;
}
Vec3 vec3_(float *f) {
  Vec3 ret;
  memcpy(&ret, f, sizeof(float) * 3);
  return ret;
}
Vec3 vec3_mul(Vec3 a, Vec3 b) {
  return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}
Vec3 vec3_add(Vec3 a, Vec3 b) {
  return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
  return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
  return (Vec3){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x};
}

float vec3_dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

Vec3 vec3_normalize(Vec3 v) {
  float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
  return (Vec3){v.x / length, v.y / length, v.z / length};
}
Mat4 mat4_lookAt(Vec3 eye, Vec3 center, Vec3 up) {
  Vec3 f = vec3_normalize(vec3_sub(center, eye));
  Vec3 s = vec3_normalize(vec3_cross(f, up));
  Vec3 u = vec3_cross(s, f);

  Mat4 result = {0};
  result.m[0] = s.x;
  result.m[1] = u.x;
  result.m[2] = -f.x;
  result.m[3] = 0.0f;

  result.m[4] = s.y;
  result.m[5] = u.y;
  result.m[6] = -f.y;
  result.m[7] = 0.0f;

  result.m[8] = s.z;
  result.m[9] = u.z;
  result.m[10] = -f.z;
  result.m[11] = 0.0f;

  result.m[12] = -vec3_dot(s, eye);
  result.m[13] = -vec3_dot(u, eye);
  result.m[14] = vec3_dot(f, eye);
  result.m[15] = 1.0f;

  return result;
}
void update_camera_direction(float yaw, float pitch, float roll, Vec3 *front,
                             Vec3 *up) {
  // Calculate front vector from yaw and pitch (standard FPS camera)
  front->x = cosf(yaw * DEG_TO_RAD) * cosf(pitch * DEG_TO_RAD);
  front->y = sinf(pitch * DEG_TO_RAD);
  front->z = sinf(yaw * DEG_TO_RAD) * cosf(pitch * DEG_TO_RAD);
  *front = vec3_normalize(*front); // Normalize to avoid speed variations

  // Apply roll to the up vector (rotation around the front vector)
  Vec3 worldUp = {0.0f, 1.0f, 0.0f}; // Default global up (Y-axis)
  Vec3 right = vec3_cross(*front, worldUp);
  right = vec3_normalize(right);

  // Rotate the up vector around the front vector by the roll angle
  float cr = cosf(roll * DEG_TO_RAD);
  float sr = sinf(roll * DEG_TO_RAD);
  up->x = cr * worldUp.x - sr * right.x;
  up->y = cr * worldUp.y - sr * right.y;
  up->z = cr * worldUp.z - sr * right.z;
  *up = vec3_normalize(*up);
}
///

#define KEY_HELD 1
#define KEY_DOWN 2
#define KEY_UP 4
#define KEY_RECENTLY_DOWN 8
#define KEY_REPEAT 0x10
#define KEY_DOUBLE_CLICK 0x20
#define PI 3.141592

#define TOTAL_KEYS (GLFW_KEY_LAST + 3)
#define TOTAL_TEXTURES 256
#define TOTAL_MODELS 2000

#define DOUBLE_CLICK_MAX_TIME 0.2

struct AudioClip;
struct sound_state;
struct open_gl_state;
void GetMem(int, dbg_state *dbg);

AudioClip *CreateNewAudioClip(open_gl_state *, char *name);

struct bone {
  int idx;
  own_std::string name;
  int parent;
  aiMatrix4x4 offset;
  aiMatrix4x4 to_parent;
  aiMatrix4x4 local_matrix;
  aiMatrix4x4 inv_local_matrix;
  own_std::vector<int> children;
  aiNodeAnim *keyframes;
  aiBone *b;
};
struct model_info {
  own_std::string name;
  u32 vbo;
  u32 vao;
  u32 ebo;
  int indicies;
  int verts_size;

  int vertex_stride;

  float *vertices;
  int *indices_data;
  int model_verts_count;

  Vec3 size;
  std::unordered_map<std::string, int> bones;
  own_std::vector<bone> all;
  own_std::vector<int> roots;
  aiScene *scene;
};
struct texture_info {
  bool used;
  int id;
};
struct texture_raw {
  char *name;
  unsigned char *data;
  int x_offset;
  int y_offset;
  int width;
  int height;
  char channels;
};
enum class buffer_type {
  FILE,
};
struct Buffer {
  buffer_type type;
  own_std::string name;
  own_std::string name_without_path;
  // TextEditor* ed;
};
std::vector<Buffer> buffers;
struct open_gl_state;
struct WindowEditor {
  bool on_cmd;
  Buffer *cur_buffer;
  Buffer *cmd_buffer;
  Buffer *prev_buffer;
  ImVec2 main_buffer_sz;
  own_std::vector<Buffer *> ed_buffers;
  open_gl_state *gl_state;
};

#ifdef LINUX
#define LANG_FILE int
#else
#define LANG_FILE HANDLE
#endif

struct draw_info3d {
  int model;
  float pos_x;
  float pos_y;
  float pos_z;
  float pos_w;

  float pivot_x;
  float pivot_y;
  float pivot_z;
  float pivot_w;

  float ent_size_x;
  float ent_size_y;
  float ent_size_z;
  float ent_size_w;

  float color_r;
  float color_g;
  float color_b;
  float color_a;

  float ent_rot_x;
  float ent_rot_y;
  float ent_rot_z;
  float ent_rot_w;

  int texture_id;

  float cam_size;

  unsigned long long cam_pos_addr;
  unsigned long long cam_rot_addr;
  unsigned long long cam_forward_addr;

  int flags;
  int stencil_func;
  u32 stencil_val;

  float tex_size_x;
  float tex_size_y;
  float tex_offset_x;
  float tex_offset_y;

  float lerp_color;

  float sec_color_r;
  float sec_color_g;
  float sec_color_b;
  float sec_color_a;

  float sun_dir_x;
  float sun_dir_y;
  float sun_dir_z;
  float sun_dir_w;

  float sun_color_x;
  float sun_color_y;
  float sun_color_z;
  float sun_color_w;
  int shader_id;

  unsigned long long name_offset;
};
#ifdef RENDERER_VULKAN

struct SwapchainData {
  VkSwapchainKHR swapchain;
  VkFormat imageFormat;
  VkExtent2D extent;
  uint32_t imageCount;
  VkImage *images;
  VkImageView *imageViews;
};

struct vulkan_state {
  VkInstance instance = VK_NULL_HANDLE;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkPhysicalDevice physical_device = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
  VkQueue grphics_queue = VK_NULL_HANDLE;
  VkCommandPool cmd_pool = VK_NULL_HANDLE;
  VkSemaphore render_complete = VK_NULL_HANDLE;
  VkSemaphore present_complete = VK_NULL_HANDLE;

  SwapchainData swap_chain;
  own_std::vector<VkCommandBuffer> cmd_buffers;
};
#endif
struct open_gl_state {
  int vao3d;
  int vao3d_line;
  int vbo3d_line;

  int vao3d_tri;
  int vbo3d_tri;
  int vao;
  int line_vao;
  int line_vbo;
  int shader_program3d_tex;
  int shader_program3d_tex_no_light;
  int shader_program3d;
  int terrain_shader_program3d;
  int shader_program3d_line;
  int shader_program3d_line_no_proj;
  int shader_program3d_tri;
  int shader_program;
  int line_shader_program;
  int shader_program_no_texture;
  int color_u;
  int tex_size;
  int tex_offset;
  int pos_u;

#ifdef RENDERER_VULKAN
  vulkan_state vk_state;
#endif

  u64 audio_frames;

  float mouse_last_x;
  float mouse_last_y;
  float mouse_vel_x;
  float mouse_vel_y;
  int generated_meshes;

  // ALCdevice *al_device;
  // ALCcontext *al_ctx;

  PaStream *pa_stream;

  int buttons[TOTAL_KEYS];
  float time_pressed[TOTAL_KEYS];
  texture_info textures[TOTAL_TEXTURES];
  model_info models[TOTAL_MODELS];
  own_std::vector<texture_raw> textures_raw;
  own_std::vector<RatedStuff<draw_info3d>> transparent_objs;
  double last_time;

  float model[16], view[16], projection[16];

  // YankBuffer yank[8];

  bool game_started;
  bool cursor_hidden;
  own_std::string texture_folder;
  own_std::string model_folder;
  bool is_engine;

  int width;
  int height;

  int scene_srceen_width;
  int scene_srceen_height;
  int frame_buffer;
  int frame_buffer_tex;

  int scroll;

  void *glfw_window;
  lang_state *lang_stat;
  sound_state *sound;

  float lmouse_click_timer;

  lang_state *lsp_lang_stat;
  mem_alloc *lsp_alloc;

  WindowEditor search_files_ed;
  WindowEditor main_ed;
  bool file_window;
  own_std::string cur_dir;
  own_std::vector<char *> files;
  own_std::vector<char *> files_aux;

  bool suggestion_accepted;

  own_std::string func_def_str;

  own_std::string lsp_dir_to_compile;

  float for_func_def_first_parentheses_pos_x;
  float for_func_def_first_parentheses_pos_y;

  int suggestion_cursor_line;
  int suggestion_cursor_column;
  int func_def_cursor_column;
  int suggestion_cursor_column_end;
  int selected_suggestion;
  own_std::vector<decl2> intellisense_suggestion;
  own_std::vector<RatedStuff<int>> intellisense_suggestion_aux;

  LANG_FILE lsp_process;
  LANG_FILE lsp_thread;
  LANG_FILE hStdInRead, hStdInWrite;
  LANG_FILE hStdOutRead, hStdOutWrite;

  LANG_FILE for_engine_game_process;
  LANG_FILE for_engine_game_thread;
  LANG_FILE for_engine_game_stdin;
  LANG_FILE for_engine_game_stdout;

  /*
  Buffer* cur_buffer;
  Buffer* cmd_buffer;
  own_std::vector<Buffer*>ed_buffers;
  */
  own_std::vector<RatedStuff<int>> rated_files;

  float cube_verts[8][3] = {
      {-0.5f, -0.5f, -0.5f}, // 0
      {0.5f, -0.5f, -0.5f},  // 1
      {0.5f, 0.5f, -0.5f},   // 2
      {-0.5f, 0.5f, -0.5f},  // 3
      {-0.5f, -0.5f, 0.5f},  // 4
      {0.5f, -0.5f, 0.5f},   // 5
      {0.5f, 0.5f, 0.5f},    // 6
      {-0.5f, 0.5f, 0.5f}    // 7
  };
  int cubeIndices[36] = {
      // Bottom face
      0, 4, 5, 0, 5, 1,
      // Front face
      0, 1, 2, 0, 2, 3,
      // Back face
      4, 6, 5, 4, 7, 6,
      // Left face
      0, 3, 7, 0, 7, 4,
      // Right face
      1, 5, 6, 1, 6, 2,
      // Top face
      3, 2, 6, 3, 6, 7};
};

class XAudioClass;
#define AUDIO_CLIP_FLAGS_LOOP 1
struct AudioClip {
  own_std::string name;
  own_std::string short_name;

  own_std::vector<short> buffer;
  float time;
};
struct AudioClipQueued {
  AudioClip *clip;
  unsigned int cur_idx;

  int flags;
  float volume;
  float speed;
};
struct sound_state {
  int samples_per_sec = 44100;
  int hz = 440;
  unsigned long long running_idx = 0;
  char *harmonics_buffer;
  short white_noise[256];
  short white_noise_cur_idx;
#ifdef LINUX
#else
  IXAudio2SourceVoice *pSourceVoice;
  XAUDIO2_BUFFER buffers[2];
  unsigned char cur_buffer_to_submit = 0;
  XAudioClass *audio_class;
#endif
  own_std::vector<AudioClip *> audio_clips_src;
  own_std::vector<AudioClipQueued> audio_clips_to_play;
};

void AddAudioClipToPlay(sound_state *sound, AudioClip *clip) {
  AudioClipQueued q = {};
  q.clip = clip;
  sound->audio_clips_to_play.emplace_back(q);
}
void FillBuffer(sound_state *sound, char *buffer, int total_samples_in_buffer,
                int bytes_per_sample, int hz_arg) {
  int count = total_samples_in_buffer * bytes_per_sample;

  short *sample = (short *)buffer;
  int wave_period = sound->samples_per_sec / hz_arg;
  int idx_that_was_period_complete = -1;
  int volume = 3000;

  /*
  timer t;
  InitTimer(&t);
  StartTimer(&t);
  */

  char b[256];

  for (int i = 0; i < count; i += bytes_per_sample) {

    unsigned int cur_idx = sound->running_idx % wave_period;
    float last_sin = (float)(cur_idx) / (float)wave_period;

    float sin_fundamental = sinf(last_sin * 2 * PI);
    short val = sin_fundamental * volume;
    /*
    int possible_harmonics = 8;

    for (int h = 0; h < possible_harmonics; h++)
    {
            bool harmonic_on = ((1 << h) & harmonics) != 0;

            short h_plus_one = h + 1;


            float sine_h = harmonic_on ? sinf((last_sin * 2 * PI) *
    h_plus_one) : 0;

            float wave_period_max = (possible_harmonics * wave_period);
            float cur_h_wave_period = (wave_period * h_plus_one);

            short h_squared = (h_plus_one * h_plus_one);
            short new_harmonic_val = (short)((sine_h) * (float)(volume /
    h_squared));

            float t = GetVolumeFromControls(cur_h_wave_period);
            new_harmonic_val = (short)((float)new_harmonic_val * t);

            val += new_harmonic_val;

    }
    */

    *sample++ = val;
    *sample++ = val;
    sound->running_idx++;
  }
  /*
  EndTimer(&t);

  //PushBufferToQueue((short *) buffer, samples_per_sec);

  __int64 ms = GetTimerMS(&t);
  __int64 cycles = GetCyclesElapsed(&t);
  int a = 0;
  */
}

#ifdef LINUX
#else
class XAudioClass : public IXAudio2VoiceCallback {
public:
  sound_state *sound;
  void XAudioClass::OnLoopEnd(void *data) {

    // FillBuffer((char *)data, 48000, 4, hz);
  }
  void XAudioClass::OnVoiceProcessingPassStart(UINT32) {}
  void XAudioClass::OnVoiceProcessingPassEnd(void) {}
  void XAudioClass::OnStreamEnd(void) {}
  void XAudioClass::OnBufferStart(void *) {
    HRESULT hr;
    sound->cur_buffer_to_submit++;
    sound->cur_buffer_to_submit %= 2;
    XAUDIO2_BUFFER *picked_buffer =
        &sound->buffers[sound->cur_buffer_to_submit];
    int total_sounds = sound->audio_clips_to_play.size();
    short *start = (short *)picked_buffer->pAudioData;
    memset(start, 0, picked_buffer->AudioBytes);
    FOR_VEC(it, sound->audio_clips_to_play) {
      start = (short *)picked_buffer->pAudioData;
      AudioClip *clip_ptr = it->clip;
      short *src_buffer = clip_ptr->buffer.data();
      if (it->cur_idx >= clip_ptr->buffer.size())
        continue;
      short *dbg = &src_buffer[it->cur_idx + 1];
      for (int i = 0; i < sound->samples_in_buffer * 4; i += 4) {
        float p = (float)it->cur_idx / (float)clip_ptr->buffer.size();
        *start += src_buffer[it->cur_idx];
        start++;
        *start += src_buffer[it->cur_idx + 1];
        start++;
        it->cur_idx += 2;
      }
    }
    int i = 0;
    FOR_VEC(it, sound->audio_clips_to_play) {
      AudioClip *clip_ptr = it->clip;
      if (it->cur_idx >= clip_ptr->buffer.size()) {
        sound->audio_clips_to_play.remove(i);
      }
      i++;
    }
    /// sound->audio_clips_to_play.
    // FillBuffer(sound, (char*)picked_buffer->pAudioData,
    // sound->samples_in_buffer, 4, 440);
    if (FAILED(hr = sound->pSourceVoice->SubmitSourceBuffer(picked_buffer,
                                                            nullptr))) {
      ASSERT(false);
    }
  }
  void XAudioClass::OnBufferEnd(void *) {}
  void XAudioClass::OnVoiceError(void *, HRESULT) {}
};
DWORD WINAPI GameAndEngineMsgThread(_In_ LPVOID lpParameter) {
  auto dbg = (dbg_state *)lpParameter;
  auto gl_state = (open_gl_state *)dbg->data;
  ;
  auto lang_stat = (lang_state *)dbg->lang_stat;
  ;
  HANDLE std_in = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE std_out = GetStdHandle(STD_OUTPUT_HANDLE);
  while (true) {
    if (!gl_state->is_engine) {
      own_std::string str;
      CheckPipeAndGetString(std_in, str);
      if (str.size() > 0) {
        auto br = (engine_msg_break *)str.data();
        char *file_name = (char *)(br + 1);

        own_std::string file_name_str = file_name;
        unit_file *fl = ThereIsFile(dbg->lang_stat, file_name_str);
        if (!fl)
          continue;
        func_decl *fdecl = GetFuncWithLine2(dbg->lang_stat, br->line, fl);
        if (!fdecl)
          continue;

        stmnt_dbg *s = nullptr;
        FOR_VEC(st, fdecl->wasm_stmnts) {
          if (st->line == br->line) {
            s = st;
            break;
          }
        }

        if (!s)
          continue;

        if (lang_stat->is_x64_bc_backend) {
          byte_code2 *bc = lang_stat->bcs2_start + s->start;
          MakeCurBcToBeBreakpoint(dbg, bc, s->line, false);
        } else {
          dbg->bcs[s->start].dbg_brk = br->add;
          dbg->bcs[s->start].from_engine_break = br->add;
        }
      }
      // FlushFileBuffers(std_out);
    } else if (gl_state->game_started) {
      own_std::string str;
      CheckPipeAndGetString(gl_state->for_engine_game_stdout, str);
      if (str.size() > 0) {
        printf("from game:%s", str.c_str());
      }
      // FlushFileBuffers(std_out);
    }

    Sleep(500);
  }
}
HRESULT InitXAudio2(sound_state &sound, bool start_playing) {
  sound.harmonics_buffer = (char *)malloc(sound.samples_in_buffer * 4 * 4);
  memset(sound.buffers, 0, sizeof(sound.buffers));
  void *src = &sound.harmonics_buffer[sound.samples_in_buffer * 4];
  void *dst = sound.harmonics_buffer;

  HRESULT hr;
  hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  if (FAILED(hr))
    return hr;

  IXAudio2 *pXAudio2 = nullptr;
  if (FAILED(
          hr = XAudio2Create(&pXAudio2, 0,
                             XAUDIO2_DEFAULT_PROCESSOR | XAUDIO2_DEBUG_ENGINE)))
    return hr;

  XAUDIO2_EFFECT_CHAIN chain = {};

  IXAudio2MasteringVoice *pMasterVoice = nullptr;
  if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasterVoice)))
    return hr;

  WAVEFORMATEX wave_format = {};

  wave_format.wFormatTag = WAVE_FORMAT_PCM;
  wave_format.nChannels = 2;
  wave_format.nSamplesPerSec = sound.samples_per_sec;
  wave_format.wBitsPerSample = 16;
  wave_format.nBlockAlign =
      (wave_format.nChannels * wave_format.wBitsPerSample) / 8;
  wave_format.nAvgBytesPerSec =
      wave_format.nSamplesPerSec * wave_format.nBlockAlign;

  if (FAILED(hr = pXAudio2->CreateSourceVoice(
                 &sound.pSourceVoice, (WAVEFORMATEX *)&wave_format,
                 XAUDIO2_VOICE_NOPITCH, XAUDIO2_MAX_FREQ_RATIO,
                 sound.audio_class, nullptr, nullptr)))
    return hr;

  XAUDIO2_BUFFER &xaudio_buff = sound.buffers[0];
  xaudio_buff.Flags = 0;
  xaudio_buff.AudioBytes = sound.samples_in_buffer * 4;
  xaudio_buff.pAudioData = (BYTE *)src;
  xaudio_buff.PlayLength = 0;

  XAUDIO2_BUFFER &xaudio_buff2 = sound.buffers[1];
  memcpy((void *)&xaudio_buff2, (void *)&xaudio_buff, sizeof(XAUDIO2_BUFFER));
  xaudio_buff2.pAudioData = (BYTE *)dst;

  if (FAILED(hr = sound.pSourceVoice->SubmitSourceBuffer(
                 &sound.buffers[sound.cur_buffer_to_submit], nullptr))) {
  }
  // cpy_thread = CreateThread(NULL, 0, ThreadProc, nullptr, 0, nullptr);

  if (start_playing) {
    if (FAILED(hr = sound.pSourceVoice->Start(0)))
      return hr;
  }
  return 0;
}
#endif

void Print(dbg_state *dbg) {
  int base = *(int *)GetRegValPtr(0, dbg, STACK_PTR_REG);
  int mem_alloc_addr = *(int *)&dbg->mem_buffer[base + 8];

  int a = 0;
}
#define DRAW_INFO_HAS_TEXTURE 1
#define DRAW_INFO_NO_SCREEN_RATIO 2
#define DRAW_INFO_WIREFRAME 4
#define DRAW_INFO_STENCIL_WRITE 8
#define DRAW_INFO_STENCIL_TEST 0x10
#define DRAW_INFO_DISABLE_WRITING_TO_COLOR_BUFFER 0x20
#define DRAW_INFO_LINE 0x40
#define DRAW_INFO_TRANSPARENT 0x80
#define DRAW_INFO_TRANSPARENT2 0x100
#define DRAW_INFO_TRIANGLE 0x200
#define DRAW_INFO_NO_PROJ 0x400
#define DRAW_INFO_DBG_BREAK 0x800
#define DRAW_INFO_TERRAIN 0x1000
#define DRAW_INFO_ALWAYS_ON_FRONT 0x2000
#define DRAW_INFO_NO_DEPTH_TEST 0x4000
#define DRAW_INFO_NO_LIGHT 0x8000
#define DRAW_INFO_CUSTOM_SHADER 0x10000

#define CULLING_BACK 0x1
#define CULLING_FRONT 0x2
#define CULLING_FRONT_BACK 0x3
enum class stencil_func {
  EQUAL,
  NEQUAL,
};
struct draw_info {
  float pos_x;
  float pos_y;
  float pos_z;
  float pos_w;

  float pivot_x;
  float pivot_y;
  float pivot_z;
  float pivot_w;

  float ent_size_x;
  float ent_size_y;
  float ent_size_z;
  float ent_size_w;

  float color_r;
  float color_g;
  float color_b;
  float color_a;

  float ent_rot_x;
  float ent_rot_y;
  float ent_rot_z;
  float ent_rot_w;

  int texture_id;

  float cam_size;

  unsigned long long cam_pos_addr;
  unsigned long long cam_rot_addr;

  int flags;
  int stencil_func;
  u32 stencil_val;

  float tex_size_x;
  float tex_size_y;
  float tex_offset_x;
  float tex_offset_y;
};

int GetTextureSlotId(open_gl_state *gl_state) {
  for (int i = 0; i < TOTAL_TEXTURES; i++) {
    texture_info *t = &gl_state->textures[i];
    if (!t->used) {
      t->used = true;
      return i;
    }
  }
  ASSERT(0);
  return -1;
}
v4 euler_to_quaternion(float roll, float pitch, float yaw) {
  roll = roll * DEG_TO_RAD;
  pitch = pitch * DEG_TO_RAD;
  yaw = yaw * DEG_TO_RAD;
  float cr = cosf(roll * 0.5f);
  float sr = sinf(roll * 0.5f);
  float cp = cosf(pitch * 0.5f);
  float sp = sinf(pitch * 0.5f);
  float cy = cosf(yaw * 0.5f);
  float sy = sinf(yaw * 0.5f);

  v4 q;
  q.x = sr * cp * cy - cr * sp * sy;
  q.y = cr * sp * cy + sr * cp * sy;
  q.z = cr * cp * sy - sr * sp * cy;
  q.w = cr * cp * cy + sr * sp * sy;
  return q;
}
v4 quat_mul(v4 q1, v4 q2) {
  v4 result;
  result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
  result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
  result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
  result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
  return result;
}
void quat_mul2(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int a_ = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int b_ = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int c_ = *(int *)&dbg->mem_buffer[base_ptr + 24];

  auto a_ptr = (v4 *)&dbg->mem_buffer[a_];
  auto b_ptr = (v4 *)&dbg->mem_buffer[b_];
  auto c_ptr = (v4 *)&dbg->mem_buffer[c_];

  *c_ptr = quat_mul(*a_ptr, *b_ptr);
}

void InvertMatrix(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int src_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int dst_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];

  float *src_data = (float *)&dbg->mem_buffer[src_offset];
  float *dst_data = (float *)&dbg->mem_buffer[dst_offset];

  const Mat4 &m = (const Mat4 &)*src_data;
  Mat4 inv;
  const float *a = m.m;

  inv.m[0] = a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] +
             a[9] * a[7] * a[14] + a[13] * a[6] * a[11] - a[13] * a[7] * a[10];

  inv.m[4] = -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] +
             a[8] * a[6] * a[15] - a[8] * a[7] * a[14] - a[12] * a[6] * a[11] +
             a[12] * a[7] * a[10];

  inv.m[8] = a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] +
             a[8] * a[7] * a[13] + a[12] * a[5] * a[11] - a[12] * a[7] * a[9];

  inv.m[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] +
              a[8] * a[5] * a[14] - a[8] * a[6] * a[13] - a[12] * a[5] * a[10] +
              a[12] * a[6] * a[9];

  inv.m[1] = -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] +
             a[9] * a[2] * a[15] - a[9] * a[3] * a[14] - a[13] * a[2] * a[11] +
             a[13] * a[3] * a[10];

  inv.m[5] = a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] +
             a[8] * a[3] * a[14] + a[12] * a[2] * a[11] - a[12] * a[3] * a[10];

  inv.m[9] = -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] -
             a[8] * a[3] * a[13] - a[12] * a[1] * a[11] + a[12] * a[3] * a[9];

  inv.m[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] +
              a[8] * a[2] * a[13] + a[12] * a[1] * a[10] - a[12] * a[2] * a[9];

  inv.m[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] +
             a[5] * a[3] * a[14] + a[13] * a[2] * a[7] - a[13] * a[3] * a[6];

  inv.m[6] = -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] -
             a[4] * a[3] * a[14] - a[12] * a[2] * a[7] + a[12] * a[3] * a[6];

  inv.m[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] +
              a[4] * a[3] * a[13] + a[12] * a[1] * a[7] - a[12] * a[3] * a[5];

  inv.m[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] -
              a[4] * a[2] * a[13] - a[12] * a[1] * a[6] + a[12] * a[2] * a[5];

  inv.m[3] = -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] -
             a[5] * a[3] * a[10] - a[9] * a[2] * a[7] + a[9] * a[3] * a[6];

  inv.m[7] = a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] +
             a[4] * a[3] * a[10] + a[8] * a[2] * a[7] - a[8] * a[3] * a[6];

  inv.m[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] -
              a[4] * a[3] * a[9] - a[8] * a[1] * a[7] + a[8] * a[3] * a[5];

  inv.m[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] +
              a[4] * a[2] * a[9] + a[8] * a[1] * a[6] - a[8] * a[2] * a[5];

  float det =
      a[0] * inv.m[0] + a[1] * inv.m[4] + a[2] * inv.m[8] + a[3] * inv.m[12];

  if (det == 0) {
    std::cerr << "Matrix inversion failed, determinant is zero.\n";
    memcpy(dst_data, &m, 64);
  }

  det = 1.0f / det;

  for (int i = 0; i < 16; i++) {
    inv.m[i] *= det;
  }

  memcpy(dst_data, &inv, 64);
}
void euler_to_quaternion2(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float x = *(float *)&dbg->mem_buffer[base_ptr + 8];
  float y = *(float *)&dbg->mem_buffer[base_ptr + 16];
  float z = *(float *)&dbg->mem_buffer[base_ptr + 24];

  int out_offset = *(int *)&dbg->mem_buffer[base_ptr + 32];

  auto out = (v4 *)&dbg->mem_buffer[out_offset];
  *out = euler_to_quaternion(x, y, z);
}
void Draw3DBase(int, dbg_state *dbg, draw_info3d *draw);
void Draw3DTransparency(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int draw_addr = *(int *)&dbg->mem_buffer[base_ptr + 8 * 2];

  auto wnd = (GLFWwindow *)*(long long *)&dbg->mem_buffer[base_ptr + 8];

  // raise(SIGTRAP);
  auto gl_state = (open_gl_state *)dbg->data;

  FOR_VEC(c, gl_state->transparent_objs) {
    draw_info3d *draw = &c->type;
    __m128 vec1 = _mm_loadu_ps((float *)&dbg->mem_buffer[draw->cam_pos_addr]);
    __m128 vec2 = _mm_loadu_ps(&c->type.pos_x);
    vec1 = _mm_sub_ps(vec1, vec2);
    vec1 = _mm_mul_ps(vec1, vec1);

    v4 v1;
    _mm_storeu_ps(&v1.x, vec1);
    c->val = vec3_dot(*(Vec3 *)&v1, *(Vec3 *)&v1);
  }
  SortRatedStuff(&gl_state->transparent_objs);

  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);

  glUseProgram(gl_state->shader_program3d);
  FOR_VEC(c, gl_state->transparent_objs) {
    c->type.flags |= DRAW_INFO_TRANSPARENT2;
    Draw3DBase(thread_id, dbg, &c->type);
  }
  return;
}
void quaternion_to_matrix4x4(float qx, float qy, float qz, float qw,
                             float *matrix) {
  // Normalize the quaternion (in case it's not already normalized)
  float length = sqrtf(qx * qx + qy * qy + qz * qz + qw * qw);
  qx /= length;
  qy /= length;
  qz /= length;
  qw /= length;

  // Calculate quaternion components squared
  float xx = qx * qx;
  float xy = qx * qy;
  float xz = qx * qz;
  float xw = qx * qw;

  float yy = qy * qy;
  float yz = qy * qz;
  float yw = qy * qw;

  float zz = qz * qz;
  float zw = qz * qw;

  // Set the matrix elements (column-major order)
  matrix[0] = 1.0f - 2.0f * (yy + zz); // m00
  matrix[1] = 2.0f * (xy + zw);        // m10
  matrix[2] = 2.0f * (xz - yw);        // m20
  matrix[3] = 0.0f;                    // m30

  matrix[4] = 2.0f * (xy - zw);        // m01
  matrix[5] = 1.0f - 2.0f * (xx + zz); // m11
  matrix[6] = 2.0f * (yz + xw);        // m21
  matrix[7] = 0.0f;                    // m31

  matrix[8] = 2.0f * (xz + yw);         // m02
  matrix[9] = 2.0f * (yz - xw);         // m12
  matrix[10] = 1.0f - 2.0f * (xx + yy); // m22
  matrix[11] = 0.0f;                    // m32

  matrix[12] = 0.0f; // m03
  matrix[13] = 0.0f; // m13
  matrix[14] = 0.0f; // m23
  matrix[15] = 1.0f; // m33
}
void build_model_matrix(float *out_matrix, const Vec3 *position,
                        const float *quaternion, // [x,y,z,w]
                        const Vec3 *scale) {

  // Extract quaternion components
  float x = quaternion[0];
  float y = quaternion[1];
  float z = quaternion[2];
  float w = quaternion[3];

  // Calculate quaternion products (for rotation matrix)
  float x2 = x + x;
  float y2 = y + y;
  float z2 = z + z;
  float xx = x * x2;
  float xy = x * y2;
  float xz = x * z2;
  float yy = y * y2;
  float yz = y * z2;
  float zz = z * z2;
  float wx = w * x2;
  float wy = w * y2;
  float wz = w * z2;

  // Apply scale to the rotation matrix
  float sx = scale->x;
  float sy = scale->y;
  float sz = scale->z;

  // First row
  out_matrix[0] = (1.0f - (yy + zz)) * sx;
  out_matrix[1] = (xy + wz) * sx;
  out_matrix[2] = (xz - wy) * sx;
  out_matrix[3] = 0.0f;

  // Second row
  out_matrix[4] = (xy - wz) * sy;
  out_matrix[5] = (1.0f - (xx + zz)) * sy;
  out_matrix[6] = (yz + wx) * sy;
  out_matrix[7] = 0.0f;

  // Third row
  out_matrix[8] = (xz + wy) * sz;
  out_matrix[9] = (yz - wx) * sz;
  out_matrix[10] = (1.0f - (xx + yy)) * sz;
  out_matrix[11] = 0.0f;

  // Fourth row (translation)
  out_matrix[12] = position->x;
  out_matrix[13] = position->y;
  out_matrix[14] = position->z;
  out_matrix[15] = 1.0f;
}

void GetViewMatrix(dbg_state *dbg, draw_info3d *draw) {
  int base_ptr = *(int *)GetRegValPtr(0, dbg, STACK_PTR_REG);
  int out_addr = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto mat = (float *)&dbg->mem_buffer[out_addr];

  auto gl_state = (open_gl_state *)dbg->data;

  memcpy(mat, gl_state->view, 16);
}
Vec3 ScreenMouseToWorldActual(float mouseX, float mouseY, float screenWidth,
                              float screenHeight, const Mat4 &invViewProj) {
  // Convert screen position to normalized device coordinates (-1 to +1)
  float ndcX = (2.0f * mouseX) / screenWidth - 1.0f;
  float ndcY =
      1.0f - (2.0f * mouseY) / screenHeight; // Invert Y for screen coords

  // printf("mx %.3f my %.3f, sw %.3f, sh %.3f\n", mouseX, mouseY,
  // screenWidth, screenHeight); Clip space positions at near and far plane
  Vec3 nearPoint = invViewProj.multiplyPoint(ndcX, ndcY, -1.0f, 1.0f);
  Vec3 farPoint = invViewProj.multiplyPoint(ndcX, ndcY, 1.0f, 1.0f);

  // Ray direction
  Vec3 dir = (farPoint - nearPoint).normalized();

  return dir;
}
void ScreenMouseToWorld(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float mx = *(float *)&dbg->mem_buffer[base_ptr + 8];
  float my = *(float *)&dbg->mem_buffer[base_ptr + 16];

  auto gl_state = (open_gl_state *)dbg->data;
  float screen_ratio = (float)gl_state->height / (float)gl_state->width;
  // my *= screen_ratio;

  Mat4 view;
  Mat4 proj;
  memcpy(&view, &gl_state->view, 4 * 4 * 4);
  memcpy(&proj, &gl_state->projection, 4 * 4 * 4);
  Mat4 viewProj = Multiply(view, proj);
  Mat4 invViewProj = Inverse(viewProj);
  Vec3 p = ScreenMouseToWorldActual(mx, my, gl_state->width, gl_state->height,
                                    invViewProj);

  auto ret = GetFloatRegValPtr(thread_id, dbg, FLOAT_REG_0);
  auto aux = GetRegValPtr(thread_id, dbg, RET_1_REG);

  memcpy(ret, &p, 16);
  memcpy(aux, &p, 8);
  *(((float *)ret) + 3) = 0.0f;
}
#define RAD_TO_DEG 57.29577

#define GL_CALL(call)                                                          \
  call;                                                                        \
  if (glGetError() != GL_NO_ERROR) {                                           \
    printf("\ngl error %d, line %d\n", glGetError(), __LINE__);                \
    fflush(stdout);                                                            \
    ExitProcess(1);                                                            \
  }
void Draw3DBase(int thread_id, dbg_state *dbg, draw_info3d *draw) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int draw_addr = *(int *)&dbg->mem_buffer[base_ptr + 8 * 2];

  auto wnd = (GLFWwindow *)*(long long *)&dbg->mem_buffer[base_ptr + 8];
  auto gl_state = (open_gl_state *)dbg->data;
  float cam_forward_x = 0;
  float cam_forward_y = 0;
  float cam_forward_z = 0;
  if (draw->cam_forward_addr != 0) {
    cam_forward_x = *(float *)&dbg->mem_buffer[draw->cam_forward_addr];
    cam_forward_y = *(float *)&dbg->mem_buffer[draw->cam_forward_addr + 4];
    cam_forward_z = *(float *)&dbg->mem_buffer[draw->cam_forward_addr + 8];
  }

  float cam_pos_x = 0;
  float cam_pos_y = 0;
  float cam_pos_z = 0;
  if (draw->cam_pos_addr != 0) {
    cam_pos_x = *(float *)&dbg->mem_buffer[draw->cam_pos_addr];
    cam_pos_y = *(float *)&dbg->mem_buffer[draw->cam_pos_addr + 4];
    cam_pos_z = *(float *)&dbg->mem_buffer[draw->cam_pos_addr + 8];
  }

  float cam_rot_x = 0;
  float cam_rot_y = 0;
  float cam_rot_z = 0;
  if (draw->cam_rot_addr != 0) {
    cam_rot_x = *(float *)&dbg->mem_buffer[draw->cam_rot_addr];
    cam_rot_y = *(float *)&dbg->mem_buffer[draw->cam_rot_addr + 4];
    cam_rot_z = *(float *)&dbg->mem_buffer[draw->cam_rot_addr + 8];
  }
  int shaderProgram = gl_state->shader_program3d;
  char *name;
  if (draw->name_offset) {
    name = (char *)&dbg->mem_buffer[draw->name_offset];
  }
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_HAS_TEXTURE)) {
    shaderProgram = gl_state->shader_program3d_tex;
    glUseProgram(shaderProgram);
    int error = glGetError();
    /*
    if(error != GL_NO_ERROR)
    {
            char buffer[64];
            GLint linked = 0;
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
            if (!linked) {
                    char log[512];
                    glGetProgramInfoLog(shaderProgram, 512, NULL, (GLchar *)
    log); printf("Link error: %s\n", buffer);
            }
            int count;
            glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &count);
            printf("Active Uniforms: %d\n", count);

            int len;
            int size;
            int type;
            for (int i = 0; i < count; i++)
            {
                    glGetActiveUniform(shaderProgram, (GLuint)i, 64, &len,
    &size, (GLenum *)&type, buffer);

                    printf("Uniform #%d Type: %u Name: %s\n", i, type,
    buffer);
            }
            printf("\ngl error %d, line %d\n", error, __LINE__);
    fflush(stdout); ExitProcess(1);
    }
            */

    shaderProgram = gl_state->shader_program3d_tex;
    glUseProgram(shaderProgram);
    gl_state->tex_size = glGetUniformLocation(shaderProgram, "tex_size");
    gl_state->tex_offset = glGetUniformLocation(shaderProgram, "tex_offset");
    if (draw->tex_size_x == 0)
      draw->tex_size_x = 1.0;
    if (draw->tex_size_y == 0)
      draw->tex_size_y = 1.0;
    glUniform2f(gl_state->tex_size, draw->tex_size_x, draw->tex_size_y);
    glUniform2f(gl_state->tex_offset, draw->tex_offset_x, draw->tex_offset_y);
    // gl_state->tex_offset = glGetUniformLocation(prog, "tex_offset");
    ASSERT(draw->texture_id < TOTAL_TEXTURES);
    // draw->flags &= ~DRAW_INFO_HAS_TEXTURE;
    texture_info *t = &gl_state->textures[draw->texture_id];

    int sec_color = glGetUniformLocation(shaderProgram, "sec_color");
    int sec_color_lerp = glGetUniformLocation(shaderProgram, "color_lerp");
    glUniform4f(sec_color, draw->sec_color_r, draw->sec_color_g,
                draw->sec_color_b, draw->sec_color_a);
    error = glGetError();
    if (error != GL_NO_ERROR) {
      printf("\ngl error %d, line %d\n", error, __LINE__);
      fflush(stdout);
      ExitProcess(1);
    }
    GL_CALL(glUniform1f(sec_color_lerp, draw->lerp_color));

    glBindTexture(GL_TEXTURE_2D, t->id);
  }
  // ASSERT(draw->perspective_mat != 0)
  // float * perspective_mat= (float *)dbg->mem_buffer[draw->perspective_mat];
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_DBG_BREAK)) {
    raise(SIGTRAP);
  }

  int indicies_to_draw = 36;
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  if (IS_FLAG_OFF(draw->flags, DRAW_INFO_TRANSPARENT2)) {
    glDisable(GL_BLEND);
  } else {
    glEnable(GL_BLEND);
  }
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_ALWAYS_ON_FRONT)) {
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
  }
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_TERRAIN)) {
    shaderProgram = gl_state->terrain_shader_program3d;
  }

  if (IS_FLAG_ON(draw->flags, DRAW_INFO_NO_LIGHT)) {
    shaderProgram = gl_state->shader_program3d_tex_no_light;
    glUseProgram(shaderProgram);
    int sec_color = glGetUniformLocation(shaderProgram, "sec_color");
    int sec_color_lerp = glGetUniformLocation(shaderProgram, "color_lerp");
    GL_CALL(glUniform4f(sec_color, draw->sec_color_r, draw->sec_color_g,
                        draw->sec_color_b, draw->sec_color_a));
    GL_CALL(glUniform1f(sec_color_lerp, draw->lerp_color));
  }

  if (IS_FLAG_ON(draw->flags, DRAW_INFO_LINE | DRAW_INFO_NO_PROJ)) {

    if (IS_FLAG_ON(draw->flags, DRAW_INFO_CUSTOM_SHADER)) {
      shaderProgram = draw->shader_id;
    }
    float line[6];
    memcpy(&line[0], &draw->pos_x, 12);
    memcpy(&line[3], &draw->ent_size_x, 12);
    shaderProgram = gl_state->shader_program3d_line;
    if (IS_FLAG_ON(draw->flags, DRAW_INFO_NO_PROJ)) {

      shaderProgram = gl_state->shader_program3d_line_no_proj;
    }
    glUseProgram(shaderProgram);
    glBindVertexArray(gl_state->vao3d_line);
    glBindBuffer(GL_ARRAY_BUFFER, gl_state->vbo3d_line);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(line), line);
    glLineWidth(4.0);

  } else if (IS_FLAG_ON(draw->flags, DRAW_INFO_TRIANGLE)) {
    Vec3 tri[6];

    Vec3 *a = (Vec3 *)&draw->pos_x;
    a->x -= cam_pos_x;
    a->y -= cam_pos_y;
    a->z -= cam_pos_z;
    Vec3 *b = (Vec3 *)&draw->pivot_x;
    b->x -= cam_pos_x;
    b->y -= cam_pos_y;
    b->z -= cam_pos_z;
    //*b = vec3_sub(*b, vec3_(&cam_pos_x));
    Vec3 *c = (Vec3 *)&draw->ent_size_x;
    c->x -= cam_pos_x;
    c->y -= cam_pos_y;
    c->z -= cam_pos_z;
    //*c = vec3_sub(*c, vec3_(&cam_pos_x));

    Vec3 normal = vec3_cross(vec3_sub(*a, *b), vec3_sub(*a, *c));
    normal = vec3_normalize(normal);

    tri[0] = *a;
    tri[1] = normal;
    tri[2] = *b;
    tri[3] = normal;
    tri[4] = *c;
    tri[5] = normal;
    shaderProgram = gl_state->shader_program3d_tri;
    glUseProgram(shaderProgram);
    glBindVertexArray(gl_state->vao3d_tri);
    glBindBuffer(GL_ARRAY_BUFFER, gl_state->vbo3d_tri);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tri), tri);

  } else {
    model_info *m = &gl_state->models[draw->model];
    // BREAK(draw->model == 1)
    glBindVertexArray(m->vao);
    indicies_to_draw = m->indicies;
  }
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_CUSTOM_SHADER)) {
    shaderProgram = draw->shader_id;
  } else {
    glUseProgram(shaderProgram);
    GL_CALL(GLint col = glGetUniformLocation(shaderProgram, "col"))
    glUniform4f(col, draw->color_r, draw->color_g, draw->color_b,
                draw->color_a);
  }

  int error = glGetError();
  if (error != GL_NO_ERROR) {
    char buffer[64];
    GLint linked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (!linked) {
      char log[512];
      glGetProgramInfoLog(shaderProgram, 512, NULL, (GLchar *)log);
      printf("Link error: %s\n", buffer);
    }
    int count;
    glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &count);
    printf("Active Uniforms: %d\n", count);
    count = clamp(count, 0, 10);

    int len;
    int size;
    int type;
    for (int i = 0; i < count; i++) {
      glGetActiveUniform(shaderProgram, (GLuint)i, 64, &len, &size,
                         (GLenum *)&type, buffer);

      printf("Uniform #%d Type: %u Name: %s\n", i, type, buffer);
    }
    printf("\nshader idx %d, gl error %d, line %d\n", shaderProgram, error,
           __LINE__);
    fflush(stdout);
    ExitProcess(1);
  }
  // Uniform locations
  glUseProgram(shaderProgram);
  GL_CALL(GLint modelLoc = glGetUniformLocation(shaderProgram, "model"))
  GL_CALL(GLint camPos = glGetUniformLocation(shaderProgram, "cam_pos"))
  GL_CALL(GLint viewLoc = glGetUniformLocation(shaderProgram, "view"))
  GL_CALL(GLint projLoc = glGetUniformLocation(shaderProgram, "projection"))
  GL_CALL(GLint rot_u = glGetUniformLocation(shaderProgram, "rot"))
  GL_CALL(GLint sun_dir = glGetUniformLocation(shaderProgram, "sun_dir"))
  GL_CALL(GLint sun_color = glGetUniformLocation(shaderProgram, "sun_color"))
  GL_CALL(GLint color = glGetUniformLocation(shaderProgram, "col"))
  GLint time = glGetUniformLocation(shaderProgram, "TIME");

  build_model_matrix(gl_state->model, (const Vec3 *)&draw->pos_x,
                     (const float *)&draw->ent_rot_x,
                     (const Vec3 *)&draw->ent_size_x);

  // gl_state->model[12] += -cam_pos_x;
  // gl_state->model[13] += -cam_pos_y;
  // gl_state->model[14] += -cam_pos_z;
  gl_state->model[15] = 1.0f;

  Vec3 cameraPos = {cam_pos_x, cam_pos_y, cam_pos_z};
  Vec3 cameraFront = {cam_forward_x, cam_forward_y, cam_forward_z};
  Vec3 cameraUp = {0.0f, 1.0f, 0.0f};

  float yaw = -cam_rot_y * RAD_TO_DEG + -90.0;
  float pitch = cam_rot_x * RAD_TO_DEG;
  float roll = cam_rot_z * RAD_TO_DEG;

  update_camera_direction(yaw, pitch, roll, &cameraFront, &cameraUp);
  Mat4 view =
      mat4_lookAt(cameraPos, vec3_add(cameraPos, cameraFront), cameraUp);
  memcpy(gl_state->view, view.m, sizeof(gl_state->view));

  // printf("sx %.3f, sy %.3f, sz %.3f\n", draw->ent_size_x, draw->ent_size_y,
  // draw->ent_size_z); printf("sx %.3f, sy %.3f, sz %.3f\n", cam_forward_x,
  // cam_forward_y, cam_forward_z); printf("cx %.3f, cy %.3f, cz %.3f, rx
  // %.3f, ry %.3f, rz %.3f\n", cam_pos_x, cam_pos_y, cam_pos_z, cam_rot_x,
  // cam_rot_y, cam_rot_z);
  gl_state->view[12] = -0.0;
  gl_state->view[13] = 0.0;
  gl_state->view[14] = 0.0;

  // printf("time %.3f\n", gl_state->last_time);
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, gl_state->model);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, gl_state->view);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, gl_state->projection);
  glUniform4f(camPos, -cam_pos_x, -cam_pos_y, -cam_pos_z, 1.0);
  glUniform4f(camPos, -cam_pos_x, -cam_pos_y, -cam_pos_z, 1.0);
  glUniform4f(color, draw->color_r, draw->color_g, draw->color_b,
              draw->color_a);
  // glUniform4f(rot_u, draw->ent_rot_x, draw->ent_rot_y, draw->ent_rot_z,
  // draw->ent_rot_w);
  glUniform3f(sun_dir, draw->sun_dir_x, draw->sun_dir_y, draw->sun_dir_z);
  glUniform4f(sun_color, draw->sun_color_x, draw->sun_color_y,
              draw->sun_color_z, draw->sun_color_w);
  glUniform1f(time, glfwGetTime());

  if (IS_FLAG_ON(draw->flags, DRAW_INFO_LINE)) {
    glDrawArrays(GL_LINES, 0, 2);
  } else if (IS_FLAG_ON(draw->flags, DRAW_INFO_TRIANGLE)) {
    glDrawArrays(GL_TRIANGLES, 0, 3);
  } else {
    // glBindVertexArray(gl_state->vao3d);
    glDrawElements(GL_TRIANGLES, indicies_to_draw, GL_UNSIGNED_INT, 0);
  }
  glCullFace(GL_FRONT);
}
void Draw3D(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int draw_addr = *(int *)&dbg->mem_buffer[base_ptr + 8 * 2];

  // raise(SIGTRAP);

  auto wnd = (GLFWwindow *)*(long long *)&dbg->mem_buffer[base_ptr + 8];
  auto draw = (draw_info3d *)(long long *)&dbg->mem_buffer[draw_addr];

  auto gl_state = (open_gl_state *)dbg->data;
  // raise(SIGTRAP);
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_TRANSPARENT)) {
    RatedStuff<draw_info3d> v;
    v.type = *draw;
    gl_state->transparent_objs.emplace_back(v);
    return;
  }
  Draw3DBase(thread_id, dbg, draw);
}
void Draw(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int draw_addr = *(int *)&dbg->mem_buffer[base_ptr + 8 * 2];

  auto wnd = (GLFWwindow *)*(long long *)&dbg->mem_buffer[base_ptr + 8];
  auto draw = (draw_info *)(long long *)&dbg->mem_buffer[draw_addr];

  auto gl_state = (open_gl_state *)dbg->data;

  int prog = gl_state->shader_program;
  int vao = gl_state->vao;

  if (IS_FLAG_ON(draw->flags, DRAW_INFO_HAS_TEXTURE)) {
    prog = gl_state->shader_program;
    glUseProgram(prog);
    gl_state->tex_size = glGetUniformLocation(prog, "tex_size");
    if (draw->tex_size_x == 0)
      draw->tex_size_x = 1.0;
    if (draw->tex_size_y == 0)
      draw->tex_size_y = 1.0;
    glUniform2f(gl_state->tex_size, draw->tex_size_x, draw->tex_size_y);
    // gl_state->tex_offset = glGetUniformLocation(prog, "tex_offset");
    ASSERT(draw->texture_id < TOTAL_TEXTURES);
    // draw->flags &= ~DRAW_INFO_HAS_TEXTURE;
    texture_info *t = &gl_state->textures[draw->texture_id];

    glBindTexture(GL_TEXTURE_2D, t->id);
  } else if (IS_FLAG_ON(draw->flags, DRAW_INFO_LINE)) {
    prog = gl_state->line_shader_program;
    vao = gl_state->line_vao;
    glUseProgram(prog);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, gl_state->line_vbo);

    glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * 4, &draw->pos_x);
  } else {
    prog = gl_state->shader_program_no_texture;
    // glDisable(GL_TEXTURE_2D);
  }

  glUseProgram(prog);
  glBindVertexArray(vao);

  gl_state->color_u = glGetUniformLocation(prog, "color");
  glUniform4f(gl_state->color_u, draw->color_r, draw->color_g, draw->color_b,
              draw->color_a);

  gl_state->pos_u = glGetUniformLocation(prog, "pos");
  glUniform3f(gl_state->pos_u, draw->pos_x, draw->pos_y, draw->pos_z);

  int pivot_u = glGetUniformLocation(prog, "pivot");
  glUniform3f(pivot_u, draw->pivot_x, draw->pivot_y, draw->pivot_z);

  int cam_size_u = glGetUniformLocation(prog, "cam_size");
  glUniform1f(cam_size_u, draw->cam_size);

  float screen_ratio = (float)gl_state->height / (float)gl_state->width;
  int screen_ratio_u = glGetUniformLocation(prog, "screen_ratio");

  if (gl_state->is_engine) {
    screen_ratio = (float)gl_state->scene_srceen_height /
                   (float)gl_state->scene_srceen_width;
  }
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_NO_SCREEN_RATIO)) {
    screen_ratio = 1;
  }
  glUniform1f(screen_ratio_u, screen_ratio);

  float cam_pos_x = 0;
  float cam_pos_y = 0;
  float cam_pos_z = 0;
  if (draw->cam_pos_addr != 0) {
    cam_pos_x = *(float *)&dbg->mem_buffer[draw->cam_pos_addr];
    cam_pos_y = *(float *)&dbg->mem_buffer[draw->cam_pos_addr + 4];
    cam_pos_z = *(float *)&dbg->mem_buffer[draw->cam_pos_addr + 8];
  }
  int cam_pos_u = glGetUniformLocation(prog, "cam_pos");
  glUniform3f(cam_pos_u, cam_pos_x, cam_pos_y, cam_pos_z);

  float cam_rot_x = 0;
  float cam_rot_y = 0;
  float cam_rot_z = 0;
  if (draw->cam_rot_addr != 0) {
    cam_rot_x = *(float *)&dbg->mem_buffer[draw->cam_rot_addr];
    cam_rot_y = *(float *)&dbg->mem_buffer[draw->cam_rot_addr + 4];
    cam_rot_z = *(float *)&dbg->mem_buffer[draw->cam_rot_addr + 8];
  }
  int cam_rot_u = glGetUniformLocation(prog, "cam_rot");
  glUniform3f(cam_rot_u, cam_rot_x, cam_rot_y, cam_rot_z);

  int ent_rot_u = glGetUniformLocation(prog, "ent_rot");
  glUniform3f(ent_rot_u, draw->ent_rot_x, draw->ent_rot_y, draw->ent_rot_z);

  int ent_size_u = glGetUniformLocation(prog, "ent_size");
  glUniform3f(ent_size_u, draw->ent_size_x, draw->ent_size_y, draw->ent_size_z);

  if (gl_state->is_engine) {
    // Render to our framebuffer
    // glBindFramebuffer(GL_FRAMEBUFFER, gl_state->frame_buffer);
    // glViewport(0, 0, gl_state->scene_srceen_width,
    // gl_state->scene_srceen_height); // Render on the whole framebuffer,
    // complete from the lower left corner to the upper right

    glViewport(0, gl_state->height - gl_state->scene_srceen_height,
               gl_state->scene_srceen_width, gl_state->scene_srceen_height);
  } else {
    glViewport(0, 0, gl_state->width, gl_state->height);
  }

  glDisable(GL_STENCIL_TEST);
  glColorMask(true, true, true, true);
  glDepthMask(true);
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_STENCIL_WRITE)) {
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, draw->stencil_val, 0xFF);
    glStencilMask(0xFF);
    // glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
    // glDisable(GL_STENCIL_TEST);
  }
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_STENCIL_TEST)) {
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    switch ((stencil_func)draw->stencil_func) {
    case stencil_func::EQUAL: {
      glStencilFunc(GL_EQUAL, draw->stencil_val, 0xFF);
    } break;
    case stencil_func::NEQUAL: {
      glStencilFunc(GL_NOTEQUAL, draw->stencil_val, 0xFF);
    } break;
    default:
      ASSERT(0);
    }
    // glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
    // glDisable(GL_STENCIL_TEST);
  }
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_DISABLE_WRITING_TO_COLOR_BUFFER)) {
    glColorMask(false, false, false, false);
    // glDepthMask(false);
  }
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_NO_DEPTH_TEST)) {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
  }

  // else
  //{
  if (IS_FLAG_ON(draw->flags, DRAW_INFO_WIREFRAME)) {
    glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
  } else if (IS_FLAG_ON(draw->flags, DRAW_INFO_LINE)) {
    glDrawArrays(GL_LINES, 0, 2);
  } else
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  //}

  // glDrawArrays(GL_TRIANGLES, 0, 3);
  //*(int*)GetRegValPtr(thread_id, dbg, RET_1_REG) =
  // glfwWindowShouldClose((GLFWwindow *)(long long)wnd);
}
void ClearBackground(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  void *addr = &dbg->mem_buffer[base_ptr + 8];
  float r = *(float *)&dbg->mem_buffer[base_ptr + 8];
  float g = *(float *)&dbg->mem_buffer[base_ptr + 8 * 2];
  float b = *(float *)&dbg->mem_buffer[base_ptr + 8 * 3];

  auto gl_state = (open_gl_state *)dbg->data;
  gl_state->transparent_objs.clear();
  if (gl_state->is_engine) {
    // glViewport(0, 0, 1000, 1000);
    glClearColor(0, 0, 0, 1.0f); // Set the new color
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // Step 2: Enable scissor testing
    glEnable(GL_SCISSOR_TEST);

    // Step 3: Define the area you want to clear with a different color
    glScissor(0, gl_state->height - gl_state->scene_srceen_height,
              gl_state->scene_srceen_width, gl_state->scene_srceen_height);
    glClearColor(r, g, b, 1.0f); // Set the new color
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // Clear only the scissor region

    // Step 4: Disable scissor testing (optional)
    glDisable(GL_SCISSOR_TEST);
    //*(int*)GetRegValPtr(thread_id, dbg, RET_1_REG) =
    // glfwWindowShouldClose((GLFWwindow *)(long long)wnd);
  } else {
    // glViewport(0, 0, 1000, 1000);
    glDepthMask(GL_TRUE);
    glClearColor(r, g, b, 1.0f); // Set the new color
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    /*
     */
  }
}

int FromGameToGLFWKey(int in) {
  int key;
  switch ((key_enum)in) {
  case _KEY_Z: {
    key = GLFW_KEY_Z;
  } break;
  case _KEY_X: {
    key = GLFW_KEY_X;
  } break;
  case _KEY_C: {
    key = GLFW_KEY_C;
  } break;
  case _KEY_V: {
    key = GLFW_KEY_V;
  } break;
  case _KEY_B: {
    key = GLFW_KEY_B;
  } break;
  case _KEY_N: {
    key = GLFW_KEY_N;
  } break;
  case _KEY_M: {
    key = GLFW_KEY_M;
  } break;
  case _KEY_P: {
    key = GLFW_KEY_P;
  } break;
  case _KEY_I: {
    key = GLFW_KEY_I;
  } break;
  case _KEY_UP: {
    key = GLFW_KEY_W;
  } break;
  case _KEY_DOWN: {
    key = GLFW_KEY_S;
  } break;
  case _KEY_RIGHT: {
    key = GLFW_KEY_D;
  } break;
  case _KEY_ACT1: {
    key = GLFW_KEY_J;
  } break;
  case _KEY_ACT0: {
    key = GLFW_KEY_K;
  } break;
  case _KEY_JMP: {
    key = GLFW_KEY_SPACE;
  } break;
  case _KEY_DEL: {
    key = GLFW_KEY_DELETE;
  } break;
  case _KEY_F: {
    key = GLFW_KEY_F;
  } break;
  case _KEY_K: {
    key = GLFW_KEY_K;
  } break;
  case _KEY_ENTER: {
    key = GLFW_KEY_ENTER;
  } break;
  case _KEY_F1: {
    key = GLFW_KEY_F1;
  } break;
  case _KEY_F2: {
    key = GLFW_KEY_F2;
  } break;
  case _KEY_F3: {
    key = GLFW_KEY_F3;
  } break;
  case _KEY_F4: {
    key = GLFW_KEY_F4;
  } break;
  case _KEY_F5: {
    key = GLFW_KEY_F5;
  } break;
  case _KEY_F6: {
    key = GLFW_KEY_F6;
  } break;
  case _KEY_F7: {
    key = GLFW_KEY_F7;
  } break;
  case _KEY_F8: {
    key = GLFW_KEY_F8;
  } break;
  case _KEY_F9: {
    key = GLFW_KEY_F9;
  } break;
  case _KEY_F10: {
    key = GLFW_KEY_F10;
  } break;
  case _KEY_F11: {
    key = GLFW_KEY_F11;
  } break;
  case _KEY_F12: {
    key = GLFW_KEY_F12;
  } break;
  case _KEY_LCTRL: {
    key = GLFW_KEY_LEFT_CONTROL;
  } break;
  case _KEY_ALT: {
    key = GLFW_KEY_LEFT_ALT;
  } break;
  case _KEY_TAB: {
    key = GLFW_KEY_TAB;
  } break;
  case _KEY_SHIFT: {
    key = GLFW_KEY_LEFT_SHIFT;
  } break;
  case _KEY_A: {
    key = GLFW_KEY_A;
  } break;
  case _KEY_ESCAPE: {
    key = GLFW_KEY_ESCAPE;
  } break;
  case _KEY_SPACE: {
    key = GLFW_KEY_SPACE;
  } break;
  case _KEY_S: {
    key = GLFW_KEY_S;
  } break;
  case _KEY_D: {
    key = GLFW_KEY_D;
  } break;
  case _KEY_W: {
    key = GLFW_KEY_W;
  } break;
  case _KEY_E: {
    key = GLFW_KEY_E;
  } break;
  case _KEY_Q: {
    key = GLFW_KEY_Q;
  } break;
  case _KEY_LEFT: {
    key = GLFW_KEY_A;
  } break;
  case _KEY_0: {
    key = GLFW_KEY_0;
  } break;
  case _KEY_1: {
    key = GLFW_KEY_1;
  } break;
  case _KEY_2: {
    key = GLFW_KEY_2;
  } break;
  case _KEY_3: {
    key = GLFW_KEY_3;
  } break;
  case _KEY_4: {
    key = GLFW_KEY_4;
  } break;
  case _KEY_5: {
    key = GLFW_KEY_5;
  } break;
  case _KEY_6: {
    key = GLFW_KEY_6;
  } break;
  case _KEY_7: {
    key = GLFW_KEY_7;
  } break;
  case _KEY_8: {
    key = GLFW_KEY_8;
  } break;
  case _KEY_9: {
    key = GLFW_KEY_9;
  } break;
  default:
    ASSERT(0);
  }
  return key;
}

void IsMouseDoubleClick(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int mouse = *(int *)&dbg->mem_buffer[base_ptr + 8];

  auto gl_state = (open_gl_state *)dbg->data;
  GLFWwindow *window = (GLFWwindow *)gl_state->glfw_window;

  int state = 0;
  int *addr = (int *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *addr = 0;
  if (mouse == 0) {
    if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST], KEY_DOUBLE_CLICK))
      *addr = 1;
  } else if (mouse == 1) {
    if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST + 1], KEY_DOUBLE_CLICK))
      *addr = 1;
  } else
    ASSERT(0)
}
void IsMouseDown(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int mouse = *(int *)&dbg->mem_buffer[base_ptr + 8];

  auto gl_state = (open_gl_state *)dbg->data;
  GLFWwindow *window = (GLFWwindow *)gl_state->glfw_window;

  int state = 0;
  int *addr = (int *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *addr = 0;
  if (mouse == 0) {
    if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST], KEY_DOWN))
      *addr = 1;
  } else if (mouse == 1) {
    if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST + 1], KEY_DOWN))
      *addr = 1;
  } else
    ASSERT(0)
}
void IsMouseUp(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int mouse = *(int *)&dbg->mem_buffer[base_ptr + 8];

  auto gl_state = (open_gl_state *)dbg->data;
  GLFWwindow *window = (GLFWwindow *)gl_state->glfw_window;

  int state = 0;
  int *addr = (int *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *addr = 0;
  if (mouse == 0) {
    if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST], KEY_UP))
      *addr = 1;
  } else if (mouse == 1) {
    if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST + 1], KEY_UP))
      *addr = 1;
  } else
    ASSERT(0)
}
void IsMouseHeld(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int mouse = *(int *)&dbg->mem_buffer[base_ptr + 8];

  auto gl_state = (open_gl_state *)dbg->data;
  GLFWwindow *window = (GLFWwindow *)gl_state->glfw_window;

  int state = 0;
  if (mouse == 0)
    state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
  else if (mouse == 1)
    state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
  else
    ASSERT(0)

  int *addr = (int *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  if (state == GLFW_PRESS) {
    *addr = 1;
  } else
    *addr = 0;
}
void IsKeyUp(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int key = *(int *)&dbg->mem_buffer[base_ptr + 8];

  auto gl_state = (open_gl_state *)dbg->data;

  key = FromGameToGLFWKey(key);
  int *addr = (int *)GetRegValPtr(thread_id, dbg, RET_1_REG);

  if (IS_FLAG_ON(gl_state->buttons[key], KEY_UP)) {
    *addr = 1;
  } else
    *addr = 0;
}
void IsKeyDown(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int keyo = *(int *)&dbg->mem_buffer[base_ptr + 8];

  auto gl_state = (open_gl_state *)dbg->data;

  int key = FromGameToGLFWKey(keyo);
  int *addr = (int *)GetRegValPtr(thread_id, dbg, RET_1_REG);

  if (IS_FLAG_ON(gl_state->buttons[key], KEY_DOWN) ||
      IS_FLAG_ON(gl_state->buttons[key], KEY_RECENTLY_DOWN)) {
    key = FromGameToGLFWKey(keyo);
    *addr = 1;
    gl_state->buttons[key] &= ~KEY_RECENTLY_DOWN;
    gl_state->buttons[key] = (gl_state->buttons[key] & 0xffff);
  } else
    *addr = 0;
}
bool IsKeyRepeat(int thread_id, void *data, int key) {
  auto gl_state = (open_gl_state *)data;
  // key = FromGameToGLFWKey(key);

  if (IS_FLAG_ON(gl_state->buttons[key], KEY_DOWN | KEY_REPEAT)) {
    return true;
  }
  return false;
}
bool IsKeyDown(int thread_id, void *data, key_enum keye) {
  auto gl_state = (open_gl_state *)((dbg_state *)data)->data;
  // key = FromGameToGLFWKey(key);
  auto key = FromGameToGLFWKey(keye);

  if (IS_FLAG_ON(gl_state->buttons[key], KEY_DOWN)) {
    return true;
  }
  return false;
}
void ImGuiSetKeyboardFocusHere(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int i = *(int *)&dbg->mem_buffer[base_ptr + 8];
  ImGui::SetKeyboardFocusHere(i);
}
void ImGuiCheckbox(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name = (char *)&dbg->mem_buffer[name_offset];
  int bool_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  auto bool_ptr = (bool *)&dbg->mem_buffer[bool_offset];
  int *addr = (int *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  if (ImGui::Checkbox(name, bool_ptr)) {
    *addr = 1;
  } else {
    *addr = 0;
  }
}
void ImGuiSetNextItemAllowOverlap(int thread_id, dbg_state *dbg) {
  ImGui::SetNextItemAllowOverlap();
}
void ImGuiPopItemWidth(int thread_id, dbg_state *dbg) { ImGui::PopItemWidth(); }
void ImGuiEnumCombo(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name = (char *)&dbg->mem_buffer[name_offset];

  int line = *(int *)&dbg->mem_buffer[base_ptr + 16];

  int var_addr_offset = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int *var_addr = (int *)&dbg->mem_buffer[var_addr_offset];

  scope *scp = FindScpWithLine(dbg->cur_func, line);
  type2 dummy;
  own_std::string str(name);
  decl2 *e = FindIdentifier(str, scp, &dummy);
  if (!e) {
    ImGui::Text("enum not found: %s", name);
    return;
  }
  ASSERT(e);

  bool clicked = false;

  if (e->type.type == TYPE_STRUCT_TYPE) {
    type_struct2 *strct = e->type.strct;
    *var_addr = clamp(*var_addr, 0, strct->vars.size() - 1);

    if (*var_addr > 128 || *var_addr < 0) {
      ImGui::Text("value too high %d", *var_addr);
      return;
    }
    scope *strct_scp = strct->scp;

    own_std::vector<decl2 *> *ar = &strct->scp->vars;
    char buffer[128];
    sprintf(buffer, "type##%d_%d", ar, var_addr);
    decl2 *cur = (*ar)[*var_addr + 2];
    sprintf(&buffer[64], "%.*s", cur->name.size(), cur->name.data());
    if (ImGui::BeginCombo(buffer, &buffer[64])) {
      for (int i = 0; i < ar->size() - 2; i++) {
        decl2 *d = (*ar)[i + 2];
        sprintf(buffer, "%.*s", d->name.size(), d->name.data());
        if (ImGui::Selectable(buffer)) {
          clicked = true;
          *var_addr = i;
        }
      }
      ImGui::EndCombo();
    }

  } else {
    *var_addr = clamp(*var_addr, 0, e->type.enum_names->size() - 1);

    if (*var_addr > 128 || *var_addr < 0) {
      ImGui::Text("value too high %d", *var_addr);
      return;
    }

    own_std::vector<char *> *ar = e->type.enum_names;
    char buffer[64];
    snprintf(buffer, 64, "type##%d_%d", ar, var_addr);
    if (ImGui::BeginCombo(buffer, (*ar)[*var_addr])) {
      for (int i = 0; i < ar->size(); i++) {
        char *ptr = (*ar)[i];
        if (ImGui::Selectable(ptr)) {
          clicked = true;
          *var_addr = i;
        }
      }
      ImGui::EndCombo();
    }
  }
  bool *addr = (bool *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *addr = clicked;
}
void ImGuiPopID(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  ImGui::PopID();
}
void ImGuiPushID(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  auto id = *(int *)&dbg->mem_buffer[base_ptr + 8];
  ImGui::PushID(id);
}
void ImGuiShowV4(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  auto v_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto v = (v3 *)&dbg->mem_buffer[v_offset];
  char buffer[128];
  snprintf(buffer, 128, "##%p%d", v, v_offset);
  ImGui::DragFloat4(buffer, (float *)v, 0.1);
}
void ImGuiShowV2(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  auto v_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto v = (v3 *)&dbg->mem_buffer[v_offset];
  char buffer[128];
  snprintf(buffer, 128, "##%p", v);
  ImGui::DragFloat2(buffer, (float *)v, 0.1);
}
void ImGuiShowV3(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  auto v_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto v = (v3 *)&dbg->mem_buffer[v_offset];
  char buffer[128];
  snprintf(buffer, 128, "##%p", v);
  ImGui::DragFloat3(buffer, (float *)v, 0.1);
}
void ImGuiSetCursorPos(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float x = *(float *)&dbg->mem_buffer[base_ptr + 8];
  float y = *(float *)&dbg->mem_buffer[base_ptr + 16];
  ImGui::SetCursorPos(ImVec2(x, y));
}
void ImGuiPushItemWidth(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float w = *(float *)&dbg->mem_buffer[base_ptr + 8];
  ImGui::PushItemWidth(w);
}
void ImGuiSameLine(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  ImGui::SameLine();
}

void ImGuiGetCursorPos(int thread_id, dbg_state *dbg) {
  float *addr = (float *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *addr = ImGui::GetCursorScreenPos().y;
}
void ImGuiGetCursorScreenPosY(int thread_id, dbg_state *dbg) {
  float *addr = (float *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *addr = ImGui::GetCursorScreenPos().y;
}
void ImGuiGetCursorScreenPosX(int thread_id, dbg_state *dbg) {
  float *addr = (float *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *addr = ImGui::GetCursorScreenPos().x;
}
void ImGuiGetCursorPosY(int thread_id, dbg_state *dbg) {
  float *addr = (float *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *addr = ImGui::GetCursorPosY();
}
void ImGuiGetCursorPosX(int thread_id, dbg_state *dbg) {
  float *addr = (float *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *addr = ImGui::GetCursorPosX();
}

void ImGuiButton(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];

  bool *addr = (bool *)GetRegValPtr(thread_id, dbg, RET_1_REG);

  if (ImGui::Button(name_str))
    *addr = true;
  else
    *addr = false;
}
void ImGuiSelectable(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];
  bool selected = *(bool *)&dbg->mem_buffer[base_ptr + 16];
  float w = *(float *)&dbg->mem_buffer[base_ptr + 24];
  float h = *(float *)&dbg->mem_buffer[base_ptr + 32];

  bool *addr = (bool *)GetRegValPtr(thread_id, dbg, RET_1_REG);

  if (ImGui::Selectable(name_str, selected,
                        ImGuiSelectableFlags_AllowDoubleClick, ImVec2(w, h)))
    *addr = true;
  else
    *addr = false;
}

void ImGuiTreePop(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  ImGui::TreePop();
}

void ImGuiTreeNodeEx(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];

  ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow;
  bool ret = ImGui::TreeNodeEx(name_str, flag);

  *(bool *)GetRegValPtr(thread_id, dbg, RET_1_REG) = ret;
}

void ImGuiHasFocus(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  auto &io = ImGui::GetIO();
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  if (io.WantCaptureMouse)
    *(bool *)GetRegValPtr(thread_id, dbg, RET_1_REG) = true;
  else
    *(bool *)GetRegValPtr(thread_id, dbg, RET_1_REG) = false;
}
void ImGuiAddRect(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int this_ptr = *(int *)&dbg->mem_buffer[base_ptr + 8];
  float min_x = *(float *)&dbg->mem_buffer[base_ptr + 16];
  float min_y = *(float *)&dbg->mem_buffer[base_ptr + 24];
  float max_x = *(float *)&dbg->mem_buffer[base_ptr + 32];
  float max_y = *(float *)&dbg->mem_buffer[base_ptr + 40];
  int col = *(int *)&dbg->mem_buffer[base_ptr + 48];
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->AddRect(ImVec2(min_x, min_y), ImVec2(max_x, max_y), col);
}
own_std::string GetWorkDir(unit_file *file, lang_state *lang_stat) {
  own_std::string work_dir = file->path;
  int last_bar = work_dir.find_last_of('/');
  work_dir = work_dir.substr(0, last_bar + 1);
  return work_dir;
}
void ImGuiSetWindowFontScale(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float fsz = *(float *)&dbg->mem_buffer[base_ptr + 8];
  auto gl_state = (open_gl_state *)dbg->data;

  ImGui::SetWindowFontScale(fsz);
}

bool IsKeyHeld(dbg_state *dbg, key_enum keye) {
  auto gl_state = (open_gl_state *)dbg->data;
  auto key = FromGameToGLFWKey(keye);
  if (IS_FLAG_ON(gl_state->buttons[key], KEY_HELD)) {
    return true;
  }
  return false;
}
void RenderFuncDef(void *data, float screen_x, float screen_y) {
  auto wnd = (WindowEditor *)data;
  open_gl_state *gl_state = wnd->gl_state;
  int str_sz = wnd->gl_state->func_def_str.size();
  if (str_sz == 0)
    return;
  ImVec2 min, max;
  min.x = screen_x;
  min.y = screen_y + 50.0;

  max = min;
  // max.x += 100.0;
  max.y += 50.0;

  ImVec2 prev_cursor = ImGui::GetCursorScreenPos();
  ImGui::SetCursorScreenPos(min);

  const float fontSize = ImGui::GetFont()
                             ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX,
                                             -1.0f, "#", nullptr, nullptr)
                             .x;

  // ImDrawList* draw_list = ImGui::GetWindowDrawList();
  ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(100, 0, 100, 255));
  ImGui::BeginChild("func def", ImVec2(str_sz * fontSize, 40));

  char buffer[128];
  int real_i = 0;
  ImGui::PopStyleColor();
  ImGui::EndChild();
  ImGui::SetCursorScreenPos(prev_cursor);
  // draw_list->AddRectFilled(min, max, IM_COL32(50, 50, 50, 255));
}

#ifdef USE_TEXT_EDITOR

void PrintGameStdOut(open_gl_state *gl_state) {
  own_std::string str;
  CheckPipeAndGetString(gl_state->for_engine_game_stdout, str);
  printf("%s", str.c_str());
}

void GetMsgFromGame(void *data) {
  auto gl_state = (open_gl_state *)data;
  if (!gl_state->game_started)
    return;
  own_std::string from_game_str;
  CheckPipeAndGetString(gl_state->for_engine_game_stdout, from_game_str);
  // if(from_game_str.size() > 0)
  // printf("msg from game: %s", from_game_str.c_str());
}
void LspSendFolderToCompile(open_gl_state *lang_stat, HANDLE hStdInWrite,
                            own_std::string folder);
void CheckLspProcess(lang_state *lang_stat, open_gl_state *gl_state);
void GotoPrevBuffer(void *data) {
  auto wnd = (WindowEditor *)data;
  if (!wnd->prev_buffer)
    return;
  Buffer *aux = wnd->prev_buffer;
  wnd->prev_buffer = wnd->cur_buffer;
  wnd->cur_buffer = aux;
  wnd->cur_buffer->ed->EnsureCursorVisible();
}
void GlobalClearSearchStringHighlight(void *data) {
  auto wnd = (WindowEditor *)data;
  wnd->cur_buffer->ed->ClearSearchStringHighlight();
}
void GlobalExitCmdBuffer(void *data, bool restoreOriginalPos) {
  auto wnd = (WindowEditor *)data;
  wnd->on_cmd = false;
  wnd->cmd_buffer->ed->haveKeyboardFocusAnyway = false;
  TextEditor *main_ed = wnd->gl_state->main_ed.cur_buffer->ed;
  main_ed->insertBuffer.clear();
  if (restoreOriginalPos) {
    main_ed->SetCursorPosition(main_ed->originalCPosBeforeSearchString);
    main_ed->matchedStrings.clear();
  }
}
void SaveFile(void *data, char *contents, int size,
              own_std::string *file_name) {
  auto wnd = (WindowEditor *)data;
  WriteFileLang((char *)file_name->c_str(), contents, size);
  CheckLspProcess(wnd->gl_state->lang_stat, wnd->gl_state);
  LspSendFolderToCompile(wnd->gl_state, wnd->gl_state->hStdInWrite,
                         wnd->gl_state->lsp_dir_to_compile);
}
bool OnIntellisenseSuggestions(void *data) {
  auto wnd = (WindowEditor *)data;
  return wnd->gl_state->intellisense_suggestion_aux.size() > 0;
}
void MoveSelectedIllisenseSuggestions(void *data, int add, bool absolute) {
  auto wnd = (WindowEditor *)data;
  int *selected = &wnd->gl_state->selected_suggestion;
  if (absolute)
    *selected = add;
  else {
    *selected += -add;
  }

  *selected =
      clamp(*selected, 0, wnd->gl_state->intellisense_suggestion.size() - 1);
}
void RenderIntellisenseSuggestions(void *data, float screen_x, float screen_y) {
  auto wnd = (WindowEditor *)data;
  if (wnd->gl_state->intellisense_suggestion.size() == 0 &&
      wnd->gl_state->func_def_str.size() == 0)
    return;
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  ImVec2 wnd_pos = ImGui::GetWindowPos();
  ImVec2 wnd_sz = ImGui::GetWindowSize();

  float suggestion_height = 100.0;

  ImVec2 min, max;
  min.x = screen_x;
  if ((screen_y + suggestion_height) >= (wnd_pos.y + wnd_sz.y)) {
    min.y = screen_y - (suggestion_height);
  } else
    min.y = screen_y + 20.0;

  ImGui::SetCursorScreenPos(min);

  int str_sz = wnd->gl_state->func_def_str.size();
  float width = 200.0;
  const float fontSize = ImGui::GetFont()
                             ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX,
                                             -1.0f, "#", nullptr, nullptr)
                             .x;
  if (str_sz != 0) {
    width = str_sz * fontSize;
  }

  ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(100, 0, 100, 255));
  ImGui::BeginChild("int sug", ImVec2(width, suggestion_height));
  // draw_list->AddRectFilled(min, max, IM_COL32(50, 50, 50, 255));

  if (str_sz != 0) {
    ImGui::Text(wnd->gl_state->func_def_str.c_str());
    ImGui::Separator();
  }

  char buffer[128];
  int real_i = 0;
  // for(int i = wnd->gl_state->intellisense_suggestion.size() - 1; i>=0;i--)
  FOR_VEC(sel, wnd->gl_state->intellisense_suggestion_aux) {
    // RatedStuff<int>* sel = &wnd->gl_state->intellisense_suggestion_aux[i];
    decl2 *d = &wnd->gl_state->intellisense_suggestion[sel->type];
    if (d->name.size() == 0)
      continue;

    bool selected = wnd->gl_state->selected_suggestion == real_i;
    snprintf(buffer, 128, "%s##%p", d->name.c_str(), d);
    if (ImGui::Selectable(buffer, selected))
      wnd->gl_state->selected_suggestion = real_i;
    real_i++;
  }
  ImGui::PopStyleColor();
  ImGui::EndChild();
}
void ClearIntellisenseSeggestion(void *data) {
  auto wnd = (WindowEditor *)data;

  wnd->gl_state->intellisense_suggestion.clear();
  wnd->gl_state->intellisense_suggestion_aux.clear();
  // wnd->gl_state->func_def_str.clear();
}
void AcceptIntellisenseSeggestion(void *data) {
  auto wnd = (WindowEditor *)data;
  TextEditor *ed = wnd->cur_buffer->ed;
  int start_line = wnd->gl_state->suggestion_cursor_line;
  int start_column = wnd->gl_state->suggestion_cursor_column + 3;
  int max_column = ed->GetLineMaxColumn(wnd->gl_state->suggestion_cursor_line);

  TextEditor::Coordinates coor(ed->mState.mCursorPosition);
  coor.mColumn = wnd->gl_state->suggestion_cursor_column;
  TextEditor::Coordinates start;
  TextEditor::Coordinates end;
  if (wnd->gl_state->suggestion_cursor_column >= max_column) {
    start = coor;
    start.mColumn = max_column;
  } else {
    start = ed->FindWordStart2(coor);
    end = ed->FindWordEnd(coor);
    ed->DeleteRange(start, end);
  }

  int name_idx = wnd->gl_state->selected_suggestion;
  int suggestion_idx =
      wnd->gl_state->intellisense_suggestion_aux[name_idx].type;
  own_std::string name =
      wnd->gl_state->intellisense_suggestion[suggestion_idx].name;

  ed->InsertTextAt(start, name.c_str());

  ed->MoveRight(name.size());

  wnd->gl_state->intellisense_suggestion.clear();
  wnd->gl_state->intellisense_suggestion_aux.clear();
}
bool GlobalIsCurCmdBuffer(void *data) {
  auto wnd = (WindowEditor *)data;
  // wnd->on_cmd = false;
  return wnd->on_cmd;
}
void GlobalGetFileName(void *data, own_std::string *out) {
  auto wnd = (WindowEditor *)data;
  *out = wnd->cur_buffer->name;
}
void GlobalChangeToCmdBuffer(void *data) {
  auto wnd = (WindowEditor *)data;
  wnd->on_cmd = true;
  wnd->cmd_buffer->ed->mVimMode = VI_INSERT;
  wnd->cmd_buffer->ed->ClearLines();
}

void GlobalGetCurBufferFileLines(void *data, own_std::string *file) {
  auto wnd = (WindowEditor *)data;
  // gl_state->cur_buffer = gl_state->cmd_buffer;

  std::vector<own_std::string> txt = wnd->cur_buffer->ed->GetTextLines();
  FOR_VEC(str, txt) {
    *file += *str;
    *file += "\r\n";
  }
}
float FuzzyMatch(const own_std::string &to_match, const own_std::string &src) {
  float score = 0.0f;

  // Match exact characters at the same positions
  for (size_t i = 0; i < to_match.size() && i < src.size(); ++i) {
    if (to_match[i] == src[i])
      score += 2.0f;
  }

  // Match characters at any position
  for (size_t i = 0; i < to_match.size(); ++i) {
    for (size_t j = 0; j < src.size(); ++j) {
      if (to_match[i] == src[j]) {
        score += 0.5f;

        // Calculate neighboring indices with clamping
        size_t prev_idx_src =
            clamp(static_cast<int>(j) - 1, 0, static_cast<int>(src.size()) - 1);
        size_t next_idx_src =
            clamp(static_cast<int>(j) + 1, 0, static_cast<int>(src.size()) - 1);

        size_t prev_idx_match = clamp(static_cast<int>(i) - 1, 0,
                                      static_cast<int>(to_match.size()) - 1);
        size_t next_idx_match = clamp(static_cast<int>(i) + 1, 0,
                                      static_cast<int>(to_match.size()) - 1);

        float add = 0.0f;

        if (i != prev_idx_match &&
            to_match[prev_idx_match] == src[prev_idx_src])
          add += 1.0f;

        if (i != next_idx_match &&
            to_match[next_idx_match] == src[next_idx_src])
          add += 1.0f;

        score += add;
      }
    }
  }

  return score;
}
void ShowFileAndCmdBuffer(WindowEditor *wnd, int flags) {
  bool focus_on_cmd = wnd->on_cmd;
  flags |= focus_on_cmd * TEXT_ED_DONT_HAVE_CURSOR_FOCUS;
  char buffer[32];
  ImGui::Text(wnd->cur_buffer->name_without_path.c_str());
  snprintf(buffer, 32, "editor##%p", wnd);
  wnd->cur_buffer->ed->Render(buffer, wnd->main_buffer_sz, flags);
  TextEditor *ed = wnd->cur_buffer->ed;
  VIM_mode_enum mode = wnd->cur_buffer->ed->mVimMode;
  if (mode == VI_NORMAL) {
    ImGui::TextColored(ImVec4(ImColor(255, 255, 0)), "NORMAL");
  } else if (mode == VI_VISUAL) {
    ImGui::TextColored(ImVec4(ImColor(255, 0, 0)), "VISUAL");
  } else if (mode == VI_INSERT) {
    ImGui::TextColored(ImVec4(ImColor(255, 255, 0)), "INSERT");
  }
  ImGui::SameLine();
  ImGui::Text("(%d:%d)", ed->mState.mCursorPosition.mLine + 1,
              ed->mState.mCursorPosition.mColumn + 1);
  ImGui::SameLine();
  ImGui::Text(ed->insertBuffer.c_str());

  bool isFirstCharacterSlash =
      wnd->cur_buffer->ed->firstChInInsertBufferIsSlash;
  flags = !focus_on_cmd * TEXT_ED_DONT_HAVE_CURSOR_FOCUS;
  if (isFirstCharacterSlash)
    wnd->cmd_buffer->ed->haveKeyboardFocusAnyway = true;

  snprintf(buffer, 32, "cmd##%p", wnd);
  wnd->cmd_buffer->ed->Render(buffer, ImVec2(0.0, 50.0), flags);
}

void ChangeFileOfBuffer(Buffer *b, own_std::string file_name) {
  int read = 0;
  char *buffer = ReadEntireFileLang((char *)file_name.c_str(), &read);

  b->ed->SetText(buffer);
  heap_free((mem_alloc *)__lang_globals.data, buffer);
}

void LspPushStringIntoVector(own_std::string *str, own_std::vector<char> *out) {
  int sz = str->size();

  int idx = out->size();
  out->make_count(out->size() + 4);
  *((int *)(out->data() + idx)) = sz;

  idx = out->size();
  out->make_count(out->size() + sz);
  memcpy(out->data() + idx, str->data(), sz);
}
Buffer *NewBuffer(dbg_state *dbg, WindowEditor *wnd) {
  auto buf = (Buffer *)AllocMiscData(dbg->lang_stat, sizeof(Buffer));
  buf->ed = (TextEditor *)AllocMiscData(dbg->lang_stat, sizeof(TextEditor));
  new (buf->ed) TextEditor();
  buf->ed->data = (void *)wnd;

  auto gl_state = (open_gl_state *)dbg->data;
  ASSERT(gl_state);
  buf->ed->yank = &gl_state->yank[0];

  return buf;
}
Buffer *AddFileToBuffer(dbg_state *dbg, own_std::string file_name,
                        WindowEditor *wnd) {
  for (int i = 0; i < file_name.size(); i++) {
    if (file_name[i] == '/')
      file_name[i] = '\\';
  }
  TCHAR name_buffer[MAX_PATH];

  int error = GetLongPathName((char *)file_name.c_str(), name_buffer, MAX_PATH);
  if (error == 0) {
    printf("error in opening file buffer, code %d", GetLastError());
    ASSERT(0);
  }
  file_name = name_buffer;
  bool has_it = false;
  FOR_VEC(buf, wnd->ed_buffers) {
    Buffer *b = *buf;

    if (b->type == buffer_type::FILE && b->name == file_name) {
      has_it = true;
      return b;
    }
  }
  Buffer *buf = NewBuffer(dbg, wnd);

  TextEditor *ed = buf->ed;
  buf->type = buffer_type::FILE;

  buf->name = file_name;

  int last_bar = file_name.find_last_of("\\/");

  buf->name_without_path = file_name.substr(last_bar + 1);

  std::ifstream t(file_name);
  if (t.good()) {
    own_std::string str((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
    ed->SetText(str);
  } else {
    ASSERT(0);
  }

  wnd->ed_buffers.emplace_back(buf);
  return buf;
}

void LspGetFileSyntaxHightlighting(lang_state *lang_stat, own_std::string fname,
                                   open_gl_state *gl_state) {
  lsp_header hdr;
  hdr.magic = 0x77;
  hdr.msg_type = lsp_msg_enum::LSP_SYNTAX;
  hdr.msg_len = sizeof(lsp_header) + fname.size() + 1;
  own_std::vector<char> buffer;
  char *cstr = (char *)fname.c_str();
  buffer.insert(buffer.end(), (char *)&hdr, (char *)(&hdr + 1));
  buffer.insert(buffer.end(), cstr, cstr + fname.size() + 1);

  Write(gl_state->hStdInWrite, buffer.data(), buffer.size());
}

void SetNewBuffer(WindowEditor *ed, Buffer *new_b) {
  if (ed->cur_buffer != new_b)
    ed->prev_buffer = ed->cur_buffer;
  ed->cur_buffer = new_b;
}

int LspCompile(lang_state *lang_stat, own_std::string folder,
               open_gl_state *gl_state, int line, int line_offset) {
  DWORD bytesRead;
  DWORD availableBytes = 0;

  char read_buffer[1024];
  if (PeekNamedPipe(gl_state->hStdOutRead, NULL, 0, NULL, &availableBytes,
                    NULL) &&
      availableBytes > 0) {
    own_std::string final_str;
    int cur_read = 0;
    while (cur_read < availableBytes) {
      ReadFile(gl_state->hStdOutRead, read_buffer, sizeof(read_buffer) - 1,
               &bytesRead, NULL);
      read_buffer[bytesRead] = 0;
      cur_read += bytesRead;
      final_str += own_std::string(read_buffer, bytesRead);
    }
    char *aux_buffer = (char *)final_str.data();
    auto hdr = (lsp_header *)aux_buffer;
    switch (lang_stat->intentions_to_lsp) {
    case lsp_intention_enum::DECL_DEF_LINE: {
      char *line = (char *)(hdr + 1);
      gl_state->func_def_str = line;
    } break;
    case lsp_intention_enum::WAITING_FOLDER_TO_COMPILE: {
      if (hdr->msg_type == lsp_msg_enum::LSP_TASK_DONE) {

        lang_stat->intentions_to_lsp = lsp_intention_enum::SYNTAX;
        own_std::string fname = gl_state->main_ed.cur_buffer->name;
        LspGetFileSyntaxHightlighting(lang_stat, fname, gl_state);
      }
    } break;
    case lsp_intention_enum::SYNTAX: {
      if (hdr->msg_type == lsp_msg_enum::LSP_SYNTAX_RES) {
        Buffer *ed_buffer = gl_state->main_ed.cur_buffer;
        for (int i = 0; i < ed_buffer->ed->mLines.size(); i++) {
          int col = 0;
          ed_buffer->ed->ColorizeLine(i, col);
          /*
          FOR_VEC(gl, *cur_line)
          {
                  gl->color = 0xffffffff;
          }
          */
        }
        char *cur_ptr = (char *)(hdr + 1);
        auto syntax_hdr = (lsp_syntax_hightlight_hdr *)cur_ptr;
        long long offset = ((char *)cur_ptr) - ((char *)hdr);
        while (syntax_hdr->type != lsp_syntax_hightlight_enum::SEOF &&
               offset < hdr->msg_len) {
          switch (syntax_hdr->type) {
          case lsp_syntax_hightlight_enum::WORD: {
            auto w = (lsp_syntax_hightlight_word *)syntax_hdr;
            auto *line = &ed_buffer->ed->mLines[w->line - 1];
            if (line->size() != 0) {
              if (w->column_end > line->size())
                return 0;

              for (int i = w->column_start; i < w->column_end; i++) {
                // int glyph_idx =
                // ed_buffer->ed->GetCharacterIndex(TextEditor::Coordinates(w->line
                // - 1, i));

                (*line)[i].color = w->color;
              }
            }
            cur_ptr = (char *)(w + 1);
          } break;
          default:
            ASSERT(false);
          }

          long long offset = ((char *)cur_ptr) - ((char *)hdr);
          syntax_hdr = (lsp_syntax_hightlight_hdr *)cur_ptr;
        }
      }
    } break;
    case lsp_intention_enum::GOTO_FUNC_DEF: {
      if (hdr->msg_type == lsp_msg_enum::LSP_GOTO_FUNC_RES) {
        int new_func_pos = *(int *)(hdr + 1);
        gl_state->main_ed.cur_buffer->ed->SetCursorPosition(
            TextEditor::Coordinates(new_func_pos, 0));
      }
    } break;
    case lsp_intention_enum::GOTO_DEF: {
      if (hdr->msg_type == lsp_msg_enum::LSP_GOTO_DEF_RES) {
        char *cur_ptr = (char *)(hdr + 1);
        auto gt_def = (goto_def *)cur_ptr;
        char *file_name = (char *)(gt_def + 1);

        Buffer *buf =
            AddFileToBuffer(lang_stat->dstate, file_name, &gl_state->main_ed);

        if (file_name != gl_state->main_ed.cmd_buffer->name) {
          SetNewBuffer(&gl_state->main_ed, buf);
          LspGetFileSyntaxHightlighting(lang_stat, file_name, gl_state);
          lang_stat->intentions_to_lsp = lsp_intention_enum::SYNTAX;
        }
        TextEditor *ed = gl_state->main_ed.cur_buffer->ed;
        ed->SetCursorPosition(TextEditor::Coordinates(gt_def->line.line - 1,
                                                      gt_def->line.column));
      }
    } break;
    case lsp_intention_enum::INTELLISENSE: {
      if (hdr->msg_type == lsp_msg_enum::LSP_INTELLISENSE_RES) {
        char *cur_ptr = (char *)(hdr + 1);
        int total_decls = *(int *)(cur_ptr);
        cur_ptr += 4;
        char *str_tbl = cur_ptr + total_decls * sizeof(rel_type2);

        for (int i = 0; i < total_decls; i++) {
          rel_type2 *r = ((rel_type2 *)cur_ptr) + i;
          char *name = str_tbl + r->name;
          decl2 decl;
          gl_state->intellisense_suggestion.emplace_back(decl);
          decl2 *back = &gl_state->intellisense_suggestion.back();
          back->name = own_std::string(name, r->name_len);
          back->type.type = (enum_type2)r->type;
          RatedStuff<int> rated;
          rated.type = i;
          gl_state->intellisense_suggestion_aux.emplace_back(rated);
        }
      }
    } break;
    }
    aux_buffer[bytesRead] = '\0'; // Null-terminate the string
    // std::cout << buffer;      // Output the captured data
  }
  return 0;
}

int CreateLspProcess(lang_state *lang_stat, open_gl_state *gl_state,
                     own_std::string folder);
void CheckLspProcess(lang_state *lang_stat, open_gl_state *gl_state) {
  DWORD code;
  GetExitCodeProcess(gl_state->lsp_process, &code);
  if (code != STILL_ACTIVE) {
    CreateLspProcess(lang_stat, gl_state, gl_state->lsp_dir_to_compile);
  }
}
void LspSendFolderToCompile(open_gl_state *gl_state, HANDLE hStdInWrite,
                            own_std::string folder) {
  lang_state *lang_stat = gl_state->lang_stat;
  own_std::vector<char> buffer;

  // 8 is for string sizes(folder name, and exe_dir)
  int offset_to_str = +8;
  buffer.make_count(sizeof(lsp_header));

  LspPushStringIntoVector(&folder, &buffer);
  LspPushStringIntoVector(&lang_stat->exe_dir, &buffer);
  auto hdr = (lsp_header *)buffer.data();
  hdr->magic = 0x77;
  hdr->msg_type = lsp_msg_enum::ADD_FOLDER;
  hdr->msg_len = buffer.size();

  DWORD bytesWritten;
  WriteFile(hStdInWrite, buffer.data(), buffer.size(), &bytesWritten, NULL);
  // WriteFileLang("mock.data", buffer.data(), buffer.size());
  lang_stat->intentions_to_lsp = lsp_intention_enum::WAITING_FOLDER_TO_COMPILE;
}

int CreateLspProcess(lang_state *lang_stat, open_gl_state *gl_state,
                     own_std::string folder) {
  SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
  HANDLE hStdInRead, hStdInWrite;
  HANDLE hStdOutRead, hStdOutWrite;

  // Create pipes for stdin and stdout

  if (!CreatePipe(&hStdInRead, &hStdInWrite, &sa, 0)) {
    std::cerr << "Failed to create stdin pipe.\n";
    return 1;
  }
  if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0)) {
    std::cerr << "Failed to create stdout pipe.\n";
    return 1;
  }

  // Ensure the write handle to stdin and read handle to stdout are not
  // inherited
  if (!SetHandleInformation(hStdInWrite, HANDLE_FLAG_INHERIT, 0) ||
      !SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0)) {
    std::cerr << "Failed to set pipe handle information.\n";
    return 1;
  }

  // Set up the STARTUPINFO structure
  STARTUPINFO si = {};
  si.cb = sizeof(STARTUPINFO);
  si.hStdInput = hStdInRead;    // Child's stdin
  si.hStdOutput = hStdOutWrite; // Child's stdout
  si.hStdError = hStdOutWrite;  // Redirect stderr (optional)
  si.dwFlags |= STARTF_USESTDHANDLES;

  PROCESS_INFORMATION pi = {};

  // Create the child process
  if (!CreateProcess(
          NULL,
          (LPSTR) "E:/projects/WasmGame/lang2/src/lsp/lsp.exe", // Replace
                                                                // with your
                                                                // command
          NULL, NULL,
          TRUE, // Inherit handles
          0, NULL, NULL, &si, &pi)) {
    std::cerr << "Failed to create process.\n";
    return 1;
  }

  // Close unused pipe ends in the parent process
  CloseHandle(hStdInRead);
  CloseHandle(hStdOutWrite);

  // Read from child's stdout
  LspSendFolderToCompile(gl_state, hStdInWrite, folder);
  // Write to child's stdin
  gl_state->hStdInWrite = hStdInWrite;
  gl_state->hStdOutRead = hStdOutRead;
  gl_state->lsp_process = pi.hProcess;
  gl_state->lsp_thread = pi.hThread;
  // CloseHandle(hStdInWrite); // Close stdin write end after writing

  // CloseHandle(hStdOutRead);

  // Wait for the child process to finish
  /*
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Clean up
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  */

  return 0;
}

void PushCStrIntoVector(own_std::vector<char> *out, char *ptr, int size) {
  int offset = out->size();
  out->make_count(out->size() + size);
  memcpy(&(*out)[offset], ptr, size);
}
void LspSendLineStr(HANDLE hStdInWrite, lsp_msg_enum msg_type, int line,
                    int column, own_std::string line_str,
                    own_std::string file) {
  to_lsp_linestr info;
  info.hdr.magic = 0x77;
  info.hdr.msg_type = msg_type;
  info.hdr.msg_len =
      sizeof(to_lsp_linestr) + line_str.size() + 1 + file.size() + 1;
  info.pos.line = line;
  info.pos.column = column;
  own_std::vector<char> buffer;
  PushCStrIntoVector(&buffer, (char *)line_str.c_str(), line_str.size() + 1);
  info.line_str_len = line_str.size() + 1;
  PushCStrIntoVector(&buffer, (char *)file.c_str(), file.size() + 1);
  buffer.insert(buffer.begin(), (char *)&info, (char *)(&info + 1));
  DWORD bytesWritten;
  WriteFile(hStdInWrite, buffer.data(), buffer.size(), &bytesWritten, NULL);
}

void LspSendCursorPosAndString(HANDLE hStdInWrite, lsp_msg_enum msg_type,
                               int line, int column, own_std::string str) {
  lsp_header hdr;
  hdr.magic = 0x77;
  hdr.msg_type = msg_type;
  hdr.msg_len = sizeof(lsp_header) + sizeof(lsp_pos) + str.size();
  lsp_pos int_info;
  int_info.column = column;
  int_info.line = line;

  own_std::vector<char> buffer;
  InsertIntoCharVector(&buffer, &hdr, sizeof(lsp_header));
  InsertIntoCharVector(&buffer, &int_info, sizeof(lsp_pos));
  InsertIntoCharVector(&buffer, (char *)str.data(), str.size() + 1);
  // InsertIntoCharVector(&buffer, (void*)word.c_str(), word.size() + 1);

  DWORD bytesWritten;
  WriteFile(hStdInWrite, buffer.data(), buffer.size(), &bytesWritten, NULL);
}

void StartGame(open_gl_state *gl_state, own_std::string game_dir) {
  DWORD code;
  GetExitCodeProcess(gl_state->for_engine_game_process, &code);
  if (code == STILL_ACTIVE) {
    TerminateProcess(gl_state->for_engine_game_process, 0);
    CloseHandle(gl_state->for_engine_game_process);
    CloseHandle(gl_state->for_engine_game_thread);
  }
  HANDLE hStdInRead, hStdInWrite;
  HANDLE hStdOutRead, hStdOutWrite;
#define CREATE_STDIN
#ifdef CREATE_STDIN
  SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

  // Create pipes for stdin and stdout

  if (!CreatePipe(&hStdInRead, &hStdInWrite, &sa, 0)) {
    std::cerr << "Failed to create stdin pipe.\n";
    return;
  }
  if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0)) {
    std::cerr << "Failed to create stdout pipe.\n";
    return;
  }

  // Ensure the write handle to stdin and read handle to stdout are not
  // inherited
  if (!SetHandleInformation(hStdInWrite, HANDLE_FLAG_INHERIT, 0) ||
      !SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0)) {
    std::cerr << "Failed to set pipe handle information.\n";
    return;
  }

  // Set up the STARTUPINFO structure
  STARTUPINFO si = {};
  si.cb = sizeof(STARTUPINFO);
  si.hStdInput = hStdInRead;    // Child's stdin
  si.hStdOutput = hStdOutWrite; // Child's stdout
  si.hStdError = hStdOutWrite;  // Redirect stderr (optional)
  si.dwFlags |= STARTF_USESTDHANDLES;
#else
  STARTUPINFO si = {};
  si.cb = sizeof(STARTUPINFO);
#endif

  PROCESS_INFORMATION pi = {};

  // Create the child process
  own_std::string full_cmd = "E:/projects/WasmGame/lang2/build/liz.exe run ";
  full_cmd += game_dir;
  if (!CreateProcess(nullptr,
                     (char *)full_cmd.c_str(), // Replace with your command
                     NULL, NULL,
                     TRUE, // Inherit handles
                     0, NULL, NULL, &si, &pi)) {
    printf("CreateProcess failed (%d).\n", GetLastError());
    return;
  }

#ifdef CREATE_STDIN
  // Close unused pipe ends in the parent process
  gl_state->for_engine_game_process = pi.hProcess;
  gl_state->for_engine_game_thread = pi.hThread;
  gl_state->for_engine_game_stdin = hStdInWrite;
  gl_state->for_engine_game_stdout = hStdOutRead;
  CloseHandle(hStdInRead);
  CloseHandle(hStdOutWrite);
#endif
  gl_state->game_started = true;
}

void ImGuiRenderTextEditor(dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  auto gl_state = (open_gl_state *)dbg->data;

  ShowFileAndCmdBuffer(&gl_state->main_ed, 0);
  /*
  if (gl_state->func_def_str.size() > 0)
  {
          RenderFuncDef(&gl_state->main_ed,
  gl_state->for_func_def_first_parentheses_pos_x,
  gl_state->for_func_def_first_parentheses_pos_y);
  }
  */

  TextEditor *ed = gl_state->main_ed.cur_buffer->ed;
  if (ed->IsTextChanged()) {
    bool has_suggestions = gl_state->intellisense_suggestion.size() > 0;
    if (has_suggestions)
      ;
    {

      int start_line = gl_state->suggestion_cursor_line;
      int start_column = gl_state->suggestion_cursor_column;
      // start_column =
      // ed->GetCharacterIndex(TextEditor::Coordinates(start_line,
      // start_column));
      int end_line = gl_state->suggestion_cursor_line;
      int end_column = ed->GetLineMaxColumn(start_line);
      own_std::string line_str =
          ed->GetText(TextEditor::Coordinates(start_line, start_column),
                      TextEditor::Coordinates(end_line, end_column));

      own_std::string out_str;
      GetWordStr((char *)line_str.data(), line_str.size(), 0, &out_str);

      gl_state->suggestion_cursor_column_end = start_column + out_str.size();

      own_std::vector<RatedStuff<int>> *aux_suggs =
          &gl_state->intellisense_suggestion_aux;
      aux_suggs->clear();
      memset(aux_suggs->data(), 0, aux_suggs->size() * sizeof(RatedStuff<int>));

      int i = 0;
      FOR_VEC(d, gl_state->intellisense_suggestion) {
        RatedStuff<int> rated;
        rated.type = i;

        rated.val = FuzzyMatch(out_str, d->name);
        aux_suggs->emplace_back(rated);
        i++;
      }
      SortRatedStuffDescending(aux_suggs);
      gl_state->selected_suggestion = 0;
      auto a = 0;
    }

    TextEditor::Coordinates coor = ed->GetCursorPosition();
    auto prevPos = ed->mState.mCursorPosition;
    ed->MoveLeft(1);
    own_std::string word_under_cursor = ed->GetWordUnderCursor();
    ed->SetCursorPosition(prevPos);

    bool can_get_all_scp_vars = word_under_cursor.size() == 1 &&
                                IsLetter(word_under_cursor[0]) &&
                                !has_suggestions;
    if (ed->lastInsertedChar == ',' || ed->lastInsertedChar == ' ') {
      ClearIntellisenseSeggestion(&gl_state->main_ed);
    }
    if (ed->lastInsertedChar == '(') {
      /*
      int out_line, out_column;
      if(CheckMatchLevelsOfChar('(', coor.mLine, coor.mColumn, int *out_line,
      int *out_column)
      */
      ed->MoveLeft(2);
      word_under_cursor = ed->GetWordUnderCursor();
      ed->MoveRight(2);

      gl_state->suggestion_cursor_line = coor.mLine;
      gl_state->func_def_cursor_column = coor.mColumn;

      ImVec2 cspos = ed->mCursorScreenPos;
      gl_state->for_func_def_first_parentheses_pos_x = cspos.x;
      gl_state->for_func_def_first_parentheses_pos_y = cspos.y;

      LspSendLineStr(gl_state->hStdInWrite, lsp_msg_enum::LSP_DECL_DEF_LINE,
                     coor.mLine + 1, ed->GetCharacterIndex(coor),
                     word_under_cursor, gl_state->main_ed.cur_buffer->name);

      gl_state->lang_stat->intentions_to_lsp =
          lsp_intention_enum::DECL_DEF_LINE;
      gl_state->intellisense_suggestion.clear();
      gl_state->intellisense_suggestion_aux.clear();

    } else if (ed->lastInsertedChar == '.' || can_get_all_scp_vars) {
      CheckLspProcess(dbg->lang_stat, gl_state);

      own_std::string line_str = ed->GetCurrentLineText();

      lsp_pos int_info;
      int_info.column = ed->GetCharacterIndex(coor) - 1;
      int_info.line = coor.mLine + 1;
      gl_state->suggestion_cursor_line = int_info.line - 1;
      if (can_get_all_scp_vars) {
        gl_state->suggestion_cursor_column = coor.mColumn - 1;
        gl_state->suggestion_cursor_column_end = int_info.column;
      } else {
        gl_state->suggestion_cursor_column = coor.mColumn;
        // gl_state->suggestion_cursor_column = int_info.column + 2;
        gl_state->suggestion_cursor_column_end = int_info.column + 3;
      }

      LspSendLineStr(gl_state->hStdInWrite, lsp_msg_enum::INTELLISENSE,
                     coor.mLine + 1, ed->GetCharacterIndex(coor) - 1, line_str,
                     gl_state->main_ed.cur_buffer->name);

      dbg->lang_stat->intentions_to_lsp = lsp_intention_enum::INTELLISENSE;
      gl_state->intellisense_suggestion.clear();
      gl_state->intellisense_suggestion_aux.clear();
    }
  }
  if (ed->gotoFuncSrcLine != 0) {
    own_std::vector<char> buffer;
    own_std::string *file_name = &gl_state->main_ed.cur_buffer->name;
    lsp_header hdr;
    hdr.magic = 0x77;
    hdr.msg_type = lsp_msg_enum::LSP_GOTO_FUNC_DEF;
    hdr.msg_len = sizeof(lsp_header) + sizeof(lsp_pos) + file_name->size() + 1;
    lsp_pos pos;
    pos.line = gl_state->main_ed.cur_buffer->ed->GetCursorPosition().mLine;
    buffer.insert(buffer.end(), (char *)&hdr, (char *)(&hdr + 1));
    buffer.insert(buffer.end(), (char *)&pos, (char *)(&pos + 1));

    char dir = 0;
    if (ed->gotoFuncSrcLine == 1) {
      dir = 1;
    } else if (ed->gotoFuncSrcLine == -1) {
      dir = -1;
    } else {
      ASSERT(0);
    }

    buffer.insert(buffer.end(), (char *)&dir, (char *)(&dir + 1));
    PushCStrIntoVector(&buffer, (char *)file_name->c_str(),
                       file_name->size() + 1);

    Write(gl_state->hStdInWrite, buffer.data(), buffer.size());
    ed->gotoFuncSrcLine = 0;
    gl_state->lang_stat->intentions_to_lsp = lsp_intention_enum::GOTO_FUNC_DEF;
  }
  if (ed->insertBuffer[0] == '/' &&
      gl_state->main_ed.cmd_buffer->ed->IsTextChanged()) {
    own_std::string line_str =
        gl_state->main_ed.cmd_buffer->ed->GetCurrentLineText();

    if (line_str.size() > 0) {
      // line_str = line_str.substr(1);
      int line = 0;
      int column = 0;
      if (ed->SearchStringRange(line_str, &line, &column)) {
      }
    }
  }
  if (ed->mState.mCursorPosition.mColumn < gl_state->func_def_cursor_column) {
    gl_state->func_def_str.clear();
  }
  if (ed->insertBuffer == "gd") {
    TextEditor::Coordinates coor = ed->FindWordEnd2(ed->mState.mCursorPosition);

    // own_std::string word = ed->GetWordUnderCursor();
    Buffer *cur_buffer = gl_state->main_ed.cur_buffer;
    own_std::string line_str = ed->GetCurrentLineText();
    own_std::vector<char> buffer;
    LspSendLineStr(gl_state->hStdInWrite, lsp_msg_enum::LSP_GOTO_DEF,
                   coor.mLine + 1, ed->GetCharacterIndex(coor), line_str,
                   cur_buffer->name);

    ed->insertBuffer.clear();
    dbg->lang_stat->intentions_to_lsp = lsp_intention_enum::GOTO_DEF;
  }
  if (dbg->lang_stat->intentions_to_lsp != lsp_intention_enum::PAUSED) {
    dbg->lang_stat->dstate = dbg;
    LspCompile(dbg->lang_stat, "../dev/engine/", gl_state, 0, 0);
  }

  /*
  if (IsKeyHeld(dbg, _KEY_LCTRL) && IsKeyDown(dbg, _KEY_F))
  {
          bool CheckMatchLevelsOfChar(char target_ch, int line, int column,
  int *out_line, int *out_column)
  }
  */
  if (IsKeyHeld(dbg, _KEY_SHIFT) && IsKeyDown(dbg, _KEY_SPACE)) {
    gl_state->file_window = !gl_state->file_window;
    FOR_VEC(s, gl_state->files) {
      heap_free((mem_alloc *)__lang_globals.data, *s);
    }
    gl_state->files.clear();
    WindowEditor *files_ed = &gl_state->search_files_ed;
    // files_ed->cur_buffer->ed->SetReadOnly(true);
    GetFilesInDirectory(gl_state->cur_dir, nullptr, &gl_state->files,
                        GET_FILES_DIR_ADD_PATH_TO_FILE_NAME |
                            GET_FILES_DIR_RECURSIVE);

    files_ed->cmd_buffer->ed->ClearLines();
    files_ed->cur_buffer->ed->ClearLines();
    files_ed->on_cmd = true;
    // files_ed->cur_buffer->ed->InsertText()
    files_ed->cmd_buffer->ed->mVimMode = VI_INSERT;

    own_std::string s;
    FOR_VEC(str, gl_state->files) {
      s += *str;
      s += "\r\n";
    }
    files_ed->cur_buffer->ed->InsertText(s);
  }
  if (gl_state->file_window) {
    bool val = true;
    gl_state->rated_files.make_count(gl_state->files.size());
    WindowEditor *files_ed = &gl_state->search_files_ed;
    if (files_ed->cmd_buffer->ed->IsTextChanged()) {
      gl_state->files_aux.clear();
      gl_state->rated_files.clear();
      files_ed->cur_buffer->ed->ClearLines();
      int i = 0;
      own_std::string line = files_ed->cmd_buffer->ed->GetCurrentLineText();

      FOR_VEC(f_str, gl_state->files) {
        RatedStuff<int> rated;
        rated.type = i;

        rated.val = FuzzyMatch(line, *f_str);
        gl_state->rated_files.emplace_back(rated);
        i++;
      }
      SortRatedStuff(&gl_state->rated_files);

      own_std::string s;
      FOR_VEC(r, gl_state->rated_files) {
        s += gl_state->files[r->type];
        s += "\r\n";
      }
      files_ed->cur_buffer->ed->InsertText(s);
      files_ed->cur_buffer->ed->MoveUp(1);
    }

    ImGui::Begin("file window", &val);
    ShowFileAndCmdBuffer(&gl_state->search_files_ed,
                         TEXT_ED_ALLOW_ARRAW_NAVIGATION_EVEN_WHEN_NOT_FOCUS);
    ImGui::End();

    if (IsKeyDown(dbg, _KEY_ENTER)) {
      own_std::string file_name =
          files_ed->cur_buffer->ed->GetCurrentLineText();
      if (file_name.empty())
        return;
      int bar = file_name.find_last_of("\\/");
      Buffer *buf = AddFileToBuffer(dbg, file_name, &gl_state->main_ed);
      SetNewBuffer(&gl_state->main_ed, buf);
      gl_state->file_window = false;
    }
  }
  if (IsKeyDown(dbg, _KEY_F1)) {
    StartGame(gl_state, "../dev/files");
  }
  if (IsKeyDown(dbg, _KEY_F9)) {

    int cline = ed->GetCursorPosition().mLine + 1;
    engine_msg_break msg;
    own_std::vector<char> buffer;
    msg.msg.type = engine_msg_enum::ADD_BREAK_POINT;
    msg.line = cline;
    if (ed->HasBreakpoint(cline)) {
      ed->RemoveBreakpoint(cline);
      msg.add = false;
    } else {
      ed->SetBreakpoint(cline);
      msg.add = true;
    }

    buffer.insert(buffer.end(), (char *)&msg, (char *)(&msg + 1));
    own_std::string *s = &gl_state->main_ed.cur_buffer->name;
    PushCStrIntoVector(&buffer, (char *)s->data(), s->size());
    Write(gl_state->for_engine_game_stdin, (char *)buffer.data(),
          buffer.size());
  }
  // ed->Render("editor", ImVec2(0.0, 500.0);
}

void InitWindowEditor(dbg_state *dbg, open_gl_state *gl_state,
                      WindowEditor *ed) {
  ed->cmd_buffer = NewBuffer(dbg, ed);
  ed->cur_buffer = NewBuffer(dbg, ed);
  ed->gl_state = gl_state;
}
void ImGuiInitTextEditor(dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name = (char *)&dbg->mem_buffer[name_offset];
  auto gl_state = (open_gl_state *)dbg->data;

  gl_state->lsp_alloc =
      (mem_alloc *)AllocMiscData(dbg->lang_stat, sizeof(mem_alloc));
  gl_state->lsp_lang_stat =
      (lang_state *)AllocMiscData(dbg->lang_stat, sizeof(lang_state));
  new (gl_state->lsp_alloc) mem_alloc();
  dbg->lang_stat->intentions_to_lsp = lsp_intention_enum::INTELLISENSE;
  dbg->lang_stat->intention_state = 0;

  void *prev_alloc = __lang_globals.data;

  InitMemAlloc(gl_state->lsp_alloc);

  InitLang(gl_state->lsp_lang_stat, (AllocTypeFunc)heap_alloc,
           (FreeTypeFunc)heap_free, gl_state->lsp_alloc);
  // LspCompile(gl_state->lsp_lang_stat, "../dev/engine/", gl_state);
  __lang_globals.data = prev_alloc;

  InitWindowEditor(dbg, gl_state, &gl_state->main_ed);
  InitWindowEditor(dbg, gl_state, &gl_state->search_files_ed);
  gl_state->search_files_ed.main_buffer_sz.x = 0.0;
  gl_state->search_files_ed.main_buffer_sz.y = 200.0;
  gl_state->main_ed.main_buffer_sz.x = 0.0;
  gl_state->main_ed.main_buffer_sz.y = 500.0;
  Buffer *buf = gl_state->main_ed.cur_buffer;

  own_std::string dir = dbg->cur_func->from_file->path;
  GoBackOneDir(&dir);
  gl_state->cur_dir = dir;
  dir += name;
  TextEditor *ed = buf->ed;
  buf->type = buffer_type::FILE;
  buf->name = dir;

  gl_state->lsp_dir_to_compile = "../dev/files";

  gl_state->main_ed.cur_buffer = AddFileToBuffer(dbg, dir, &gl_state->main_ed);

  std::ifstream t(dir);
  if (t.good()) {
    own_std::string str((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
    ed->SetText(str);
  } else {
    ASSERT(0);
  }

  gl_state->main_ed.ed_buffers.emplace_back(buf);

  CreateLspProcess(dbg->lang_stat, gl_state, gl_state->lsp_dir_to_compile);
}
#endif

void GoBackOneDir(own_std::string *dir) {
  dir->pop_back();
  int last_bar = dir->find_last_of("/\\");
  dir->erase(last_bar, -1);
  *dir += '\\';
}

void ImGuiSeparator(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  ImGui::Separator();
}
void ImGuiDragInt(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int label_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int var_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];

  char *label = (char *)&dbg->mem_buffer[label_offset];
  auto var_addr = (int *)&dbg->mem_buffer[var_offset];
  ImGui::DragInt(label, var_addr);
}
void ImGuiDragF32(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int label_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int var_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];

  char *label = (char *)&dbg->mem_buffer[label_offset];
  float *var_addr = (float *)&dbg->mem_buffer[var_offset];
  ImGui::DragFloat(label, var_addr, 0.1);
}
void ImGuiInputF32(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int label_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int var_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];

  char *label = (char *)&dbg->mem_buffer[label_offset];
  float *var_addr = (float *)&dbg->mem_buffer[var_offset];
  ImGui::InputFloat(label, var_addr);
}
void ImGuiInputInt(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int label_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int var_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];

  char *label = (char *)&dbg->mem_buffer[label_offset];
  int *var_addr = (int *)&dbg->mem_buffer[var_offset];
  ImGui::InputInt(label, var_addr);
}
void ImGuiInputText(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int label_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int buf_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int buf_sz = *(int *)&dbg->mem_buffer[base_ptr + 24];

  char *label = (char *)&dbg->mem_buffer[label_offset];
  if (own_std::string(label) == "scene_name")
    auto a = 0;
  char *buf = (char *)&dbg->mem_buffer[buf_offset];
  bool val = ImGui::InputText(label, buf, buf_sz);
  auto aux = GetRegValPtr(thread_id, dbg, RET_1_REG);
  *aux = val;
}
// void PrintCallBasedOnBc(dbg_state *dbg, by)
void PrintCallBasedOnBc(dbg_state *dbg, byte_code2 *bc) {
  func_decl *fdecl = GetFuncBasedOnBc2(dbg, bc);
  int offset = bc - dbg->lang_stat->bcs2_start;
  stmnt_dbg *st = GetStmntBasedOnOffset(&fdecl->wasm_stmnts, offset);
  if (st)
    printf("%s(%d): %s\n", fdecl->from_file->name.c_str(), st->line,
           fdecl->name.c_str());
  else
    printf("%s: %s\n", fdecl->from_file->name.c_str(), fdecl->name.c_str());

  fflush(stdout);
}
void PrintCallStack(int thread_id, dbg_state *dbg) {
  FOR_VEC(bc, dbg->return_stack_bc2) { PrintCallBasedOnBc(dbg, **bc); }
  PrintCallBasedOnBc(dbg, *dbg->cur_bc2);
}
/*
void PrintCallStack(int thread_id, dbg_state *dbg)
{
  HERE()
}
void PrintCallStack(int thread_id, dbg_state *dbg)
{
        FOR_VEC(f, dbg.return_stack_bc2_func)
        {
                if(!f->st)
                        printf("%s: %s", f->fdecl->from_file->name.c_str(),
f->fdecl->name.c_str()); else printf("%s(%d): %s",
f->fdecl->from_file->name.c_str(), f->st->line, f->fdecl->name.c_str());
        }
}
*/
void ImGuiImage(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int id = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int sz_x = (int)*(float *)&dbg->mem_buffer[base_ptr + 16];
  int sz_y = (int)*(float *)&dbg->mem_buffer[base_ptr + 24];
  ImGuiIO &io = ImGui::GetIO();

  ImTextureID my_tex_id = io.Fonts->TexID;
  // id = my_tex_id;

  auto gl_state = (open_gl_state *)dbg->data;
  texture_info *t = &gl_state->textures[id];
  // printf("ImGuiImage id is %d, glid is %d\n", id, t->id);
  ImGui::Image((ImTextureID)(intptr_t)t->id, ImVec2(sz_x, sz_y), ImVec2(0, 1),
               ImVec2(1, 0));
}
void ImGuiEnd(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  ImGui::End();

  dbg->imgui_begins.pop_back();
}
void ImGuiBegin(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];

  int bool_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  bool *bool_ptr = (bool *)&dbg->mem_buffer[bool_offset];
  int flags = *(int *)&dbg->mem_buffer[base_ptr + 24];
  ImGui::Begin(name_str, bool_ptr, flags);

  dbg->imgui_begins.emplace_back(0);
}
void ImGuiEndChild(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  ImGui::EndChild();
  dbg->imgui_begins.pop_back();
}
void ImGuiBeginChild(int thread_id, dbg_state *dbg) {
  if (dbg->frame_is_from_dbg)
    return;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  float sz_x = *(float *)&dbg->mem_buffer[base_ptr + 16];
  float sz_y = *(float *)&dbg->mem_buffer[base_ptr + 24];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];
  ImGui::BeginChild(name_str, ImVec2(sz_x, sz_y));
  dbg->imgui_begins.emplace_back(1);
}

void ImGuiText(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];
  ImGui::Text(name_str);
}
void IsKeyHeld(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int key = *(int *)&dbg->mem_buffer[base_ptr + 8];

  auto gl_state = (open_gl_state *)dbg->data;

  key = FromGameToGLFWKey(key);

  int *addr = (int *)GetRegValPtr(thread_id, dbg, RET_1_REG);

  if (IS_FLAG_ON(gl_state->buttons[key], KEY_HELD)) {
    *addr = 1;
  } else
    *addr = 0;
}
void GetTime(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;

  auto ret = (float *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *ret = glfwGetTime();
}
void EndFrame(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int draw_addr = *(int *)&dbg->mem_buffer[base_ptr + 8 * 2];

  auto wnd = (GLFWwindow *)*(long long *)&dbg->mem_buffer[base_ptr + 8];
  auto gl_state = (open_gl_state *)dbg->data;
  gl_state->last_time = glfwGetTime();
  ImGui::Render();
  // int display_w, display_h;
  // glfwGetFramebufferSize(window, &display_w, &display_h);
  // glViewport(0, 0, display_w, display_h);
  // glClearColor(clear_color.x * clear_color.w, clear_color.y *
  // clear_color.w, clear_color.z * clear_color.w, clear_color.w);
  // glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(wnd);
  gl_state->scroll = 0;

  dbg->imgui_begins.clear();
}
void ClearKeys(void *data) {
  auto gl_state = (open_gl_state *)data;
  for (int i = 0; i < TOTAL_KEYS; i++) {
    int retain_flags = gl_state->buttons[i] & KEY_HELD;
    gl_state->buttons[i] &=
        ~(KEY_DOWN | KEY_UP | KEY_REPEAT | KEY_DOUBLE_CLICK);
    gl_state->buttons[i] |= retain_flags;

    if (IS_FLAG_ON(gl_state->buttons[i], KEY_RECENTLY_DOWN)) {
      unsigned short held_from = (unsigned short)(gl_state->buttons[i] >> 16);
      held_from++;
      if (held_from > 24) {
        gl_state->buttons[i] &= ~KEY_RECENTLY_DOWN;
        gl_state->buttons[i] = (gl_state->buttons[i] & 0xffff);
      } else {
        gl_state->buttons[i] =
            (gl_state->buttons[i] & 0xffff) | (held_from << 16);
      }
    }
  }
}
void ShouldClose(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  long long wnd = *(long long *)&dbg->mem_buffer[base_ptr + 8];

  *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG) =
      glfwWindowShouldClose((GLFWwindow *)(long long)wnd);

  auto gl_state = (open_gl_state *)dbg->data;

  ClearKeys(gl_state);

  dbg->frame_is_from_dbg = false;
  gl_state->mouse_vel_x = 0.0;
  gl_state->mouse_vel_y = 0.0;

  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

// Callback function for window close event
void window_close_callback(GLFWwindow *window) {
  std::cout << "Window is about to close!" << std::endl;
  auto gl_state = (open_gl_state *)glfwGetWindowUserPointer(window);
  if (gl_state->is_engine) {
    TerminateProcess(gl_state->for_engine_game_process, 0);
    TerminateProcess(gl_state->lsp_process, 0);
    ExitProcess(1);
  }
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  auto gl_state = (open_gl_state *)glfwGetWindowUserPointer(window);
  gl_state->scroll = yoffset;
}

void MouseCallback(GLFWwindow *window, int button, int action, int mods) {
  auto gl_state = (open_gl_state *)glfwGetWindowUserPointer(window);
  int mouse_key = GLFW_KEY_LAST + button;
  if (action == GLFW_PRESS) {

    gl_state->buttons[mouse_key] = KEY_HELD | KEY_DOWN | KEY_RECENTLY_DOWN;
    float t = glfwGetTime();

    if ((t - gl_state->time_pressed[mouse_key]) < DOUBLE_CLICK_MAX_TIME)
      gl_state->buttons[mouse_key] |= KEY_DOUBLE_CLICK;

    gl_state->time_pressed[mouse_key] = t;
    // printf("key(%d) is %d", key, gl_state->buttons[key]);
  } else if (action == GLFW_RELEASE) {
    gl_state->buttons[mouse_key] &= ~KEY_HELD;
    gl_state->buttons[mouse_key] |= KEY_UP;
  } else if (action == GLFW_REPEAT) {
    gl_state->buttons[GLFW_KEY_LAST + button] |= KEY_REPEAT;
  }
}
void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  auto gl_state = (open_gl_state *)glfwGetWindowUserPointer(window);
  if (action == GLFW_PRESS) {
    gl_state->buttons[key] |= KEY_HELD | KEY_DOWN | KEY_RECENTLY_DOWN;
    float t = glfwGetTime();
    if ((t - gl_state->time_pressed[key]) < DOUBLE_CLICK_MAX_TIME)
      gl_state->buttons[key] |= KEY_DOUBLE_CLICK;

    gl_state->time_pressed[key] = t;
    // printf("key(%d) is %d", key, gl_state->buttons[key]);
  } else if (action == GLFW_RELEASE) {
    gl_state->buttons[key] &= ~KEY_HELD;
    gl_state->buttons[key] |= KEY_UP;
    // printf("key release(%d) is %d", key, gl_state->buttons[key]);
  } else if (action == GLFW_REPEAT) {
    gl_state->buttons[key] |= KEY_REPEAT;
  }
}
struct clip {
  unsigned int *texs_idxs;
  unsigned int total_texs;
  unsigned int id;
  float len;
  float cur_time;
  bool loop;
};
struct load_clip_args {
  unsigned char *file_name;
  unsigned long long x_offset;
  unsigned long long y_offset;
  unsigned long long sp_width;
  unsigned long long sp_height;
  unsigned long long total_sps;
  float len;
  clip *cinfo;
};
texture_raw *HasRawTexture(open_gl_state *gl_state, own_std::string name) {
  FOR_VEC(tex, gl_state->textures_raw) {
    if (own_std::string(tex->name) == name) {
      return tex;
    }
  }
  int width, height, nrChannels;
  unsigned char *src = nullptr;
  stbi_set_flip_vertically_on_load(true);
  src = stbi_load((char *)(gl_state->texture_folder + name).c_str(), &width,
                  &height, &nrChannels, 0);
  ASSERT(src);
  gl_state->textures_raw.emplace_back(texture_raw());
  texture_raw *new_tex = &gl_state->textures_raw.back();
  // new_tex->name = "";
  new_tex->name = std_str_to_heap(gl_state->lang_stat, &name);
  new_tex->data = src;
  new_tex->width = width;
  new_tex->height = height;
  new_tex->channels = nrChannels;
  // ASSERT()
  // gl_state->textures_raw.emplace_back(new_tex);
  return new_tex;
}

void CheckOpenGLError(const char *stmt, const char *fname, int line) {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
    abort();
  }
}

#ifdef _DEBUG
#define GL_CHECK(stmt)                                                         \
  do {                                                                         \
    stmt;                                                                      \
    CheckOpenGLError(#stmt, __FILE__, __LINE__);                               \
  } while (0)
#else
#define GL_CHECK(stmt) stmt
#endif
void UpdateTexture(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int tex_id = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int x_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int y_offset = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int width = *(int *)&dbg->mem_buffer[base_ptr + 32];
  int height = *(int *)&dbg->mem_buffer[base_ptr + 40];
  int data = *(int *)&dbg->mem_buffer[base_ptr + 48];
  int type = *(int *)&dbg->mem_buffer[base_ptr + 56];
  int size = *(int *)&dbg->mem_buffer[base_ptr + 64];
  auto data_ptr = (char *)&dbg->mem_buffer[data];

  texture_info *t = &gl_state->textures[tex_id];
  GLenum internalFormat;
  GLenum format;
  GLenum pixelType;
  // HERE()
  switch (size) {
  case 0: {
    pixelType = GL_UNSIGNED_BYTE;
  } break;
  case 1: {
    pixelType = GL_UNSIGNED_SHORT;
  } break;
  case 2: {
    pixelType = GL_UNSIGNED_INT;
  } break;
  default:
    ASSERT(false)
  }

  switch (type) {
  case 0: // 1 channel, 8-bit
    switch (size) {
    case 0: {
      internalFormat = GL_R8;
    } break;
    case 1: {
      internalFormat = GL_R16;
    } break;
    case 2: {
      internalFormat = GL_R32I;
    } break;
    default:
      ASSERT(false)
    }

    format = GL_RED;
    break;

  case 1: // 1 channel, 16-bit
    internalFormat = GL_R16;
    format = GL_RED;
    break;

  case 3: // 4 channels, 8-bit
    internalFormat = GL_RGBA8;
    format = GL_RGBA;
    break;

  default:
    ASSERT(false);
    break;
  }

  glBindTexture(GL_TEXTURE_2D, t->id);
  GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
                       format, pixelType, NULL));
  GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, width, height,
                           format, pixelType, data_ptr));
  /*
  for(int i= 0; i < 32;i++)
  {
          printf("vals is %d\n", *((short *)data_ptr + i));
  }
          */
  // stbi_write_png("dbg_img.png", width, height, 4, data_ptr, width * 4);
  // HERE()

  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
  // GL_UNSIGNED_BYTE, textureData.data());
}
void CopyTextureToBuffer(dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(0, dbg, STACK_PTR_REG);
  int tex_id = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int buffer_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int buffer_size = *(int *)&dbg->mem_buffer[base_ptr + 24];

  auto buffer_ptr = (char *)&dbg->mem_buffer[buffer_offset];

  texture_info *t = &gl_state->textures[tex_id];
  glBindTexture(GL_TEXTURE_2D, t->id);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  int width, height;
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

  ASSERT((width * height * 4) <= buffer_size);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer_ptr);

  stbi_write_png("dbg_img.png", width, height, 4, buffer_ptr, width * 4);
}
int GenRawTexture(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int sz_x = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int sz_y = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int type = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int filter = *(int *)&dbg->mem_buffer[base_ptr + 32];
  int size = *(int *)&dbg->mem_buffer[base_ptr + 40];
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  if (filter == 0) {
    filter = GL_NEAREST;
  }
  if (filter == 1) {
    filter = GL_LINEAR;
  }
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

  GLenum internalFormat = 0;
  GLenum format = 0;
  GLenum pixelType = 0;
  // HERE()
  switch (size) {
  case 0: {
    pixelType = GL_UNSIGNED_BYTE;
  } break;
  case 1: {
    pixelType = GL_UNSIGNED_SHORT;
  } break;
  case 2: {
    pixelType = GL_UNSIGNED_INT;
  } break;
  default:
    ASSERT(false)
  }
  type++;
  auto src = (unsigned char *)AllocMiscData(
      dbg->lang_stat, sz_x * sz_y * (type * (1 << pixelType)));
  type--;
  switch (type) {
  case 0:
    switch (size) {
    case 0: {
      internalFormat = GL_R8;
    } break;
    case 1: {
      internalFormat = GL_R16;
    } break;
    case 2: {
      internalFormat = GL_R32I;
    } break;
    default:
      ASSERT(false)
    }
    format = GL_RED;
    break;

  case 1:
    internalFormat = GL_RG8;
    format = GL_RG;
    break;

  case 2:
    internalFormat = GL_RGB8;
    format = GL_RGB;
    break;

  case 3: // 4 channels, 8-bit
    internalFormat = GL_RGBA8;
    format = GL_RGBA;
    break;

  default:
    ASSERT(false);
    break;
  }
  // memset(src, 0xffffff, 4 * 512);
  // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

  GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, sz_x, sz_y, 0, format,
                       pixelType, src));
  // GL_CALL(glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0));

  // GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
  // stbi_write_png("dbg_img.png", sz_x, sz_y, 4, src, sz_x * 2);
  // HERE()
  auto gl_state = (open_gl_state *)dbg->data;
  int idx = GetTextureSlotId(gl_state);
  texture_info *tex = &gl_state->textures[idx];
  tex->id = texture;

  *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG) = idx;

  heap_free((mem_alloc *)__lang_globals.data, (char *)src);
  *(u64 *)GetRegValPtr(thread_id, dbg, RET_1_REG) = idx;

  return idx;
}
int GenTexture2(lang_state *lang_stat, open_gl_state *gl_state,
                unsigned char *src, int width, int height) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                        GL_UNSIGNED_BYTE, src));
  // GL_CALL(glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0));

  // GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
  // stbi_write_png("dbg_img.png", width, height, 4, src, width * 4);
  int idx = GetTextureSlotId(gl_state);
  texture_info *tex = &gl_state->textures[idx];
  tex->id = texture;

  // heap_free((mem_alloc*)__lang_globals.data, (char*)sp_data);

  return idx;
}
int GenTexture(lang_state *lang_stat, open_gl_state *gl_state,
               unsigned char *src, int sp_width, int sp_height, int x_offset,
               int y_offset, int width, int height, int sp_idx) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound
  // texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // int sp_height = info->sp_width;
  // int sp_width = info->sp_height;
  auto sp_data =
      (unsigned char *)AllocMiscData(lang_stat, sp_width * sp_height * 4);
  // int sp_idx = ;

  y_offset = (height - (y_offset + sp_height));

  for (int i = 0; i < sp_height; i++) {
    int cur_x_offset = (x_offset) + sp_idx * sp_width * 4;
    // int y_offset = sp_idx * sp_height;
    memcpy(sp_data + i * sp_width * 4,
           src + cur_x_offset + ((i + y_offset) * width * 4), sp_width * 4);
    int a = 0;
  }
  if (sp_data) {
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sp_width, sp_height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, sp_data));
    // GL_CALL(glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0));

    // GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  // stbi_write_png("dbg_img.png", sp_width, sp_height, 4, sp_data, sp_width *
  // 4);
  int idx = GetTextureSlotId(gl_state);
  texture_info *tex = &gl_state->textures[idx];
  tex->id = texture;

  heap_free((mem_alloc *)__lang_globals.data, (char *)sp_data);

  return idx;
}

void MaybeAddBarToEndOfStr(own_std::string *str) {
  if (str->size() != 0 && (*str)[str->size() - 1] != '/' &&
      (*str)[str->size() - 1] != '\\')
    (*str) += '/';
}
void CopyFromSrcImgToBuffer(char *src_img, char *buffer, int buffer_width,
                            int buffer_height, int src_img_width) {
  for (int y = 0; y < buffer_height; y++) {
    memcpy(buffer, src_img, buffer_width * 4);
    buffer += buffer_width * 4;
    src_img += src_img_width * 4;
  }
}
struct sheet_file_header {
  u32 total_layers;
  u32 str_tbl_offset;
  u32 str_tbl_sz;
  u32 cell_info_size;
};
struct aux_layer_info_struct {
  u64 version;
  u32 type;
  u32 pixels_per_width;

  v4 pos;
  v4 sz;
  u32 grid_x;
  u32 grid_y;
  u32 total_of_used_cells;
  u32 cell_sz;
  struct {
    bool is_masked;
    u64 stencil_val;
  };
};

struct sp_cell {
  u64 version;
  u64 tex_name;
  u32 grid_x;
  u32 grid_y;

  u32 src_tex_offset_x;
  u32 src_tex_offset_y;
  bool is_masked;
  u64 stencil_val;
};
struct aux_cell_info {
  u64 version;
  union {
    struct {
      u64 tex_name;
      u32 grid_x;
      u32 grid_y;

      u32 src_tex_offset_x;
      u32 src_tex_offset_y;
    };
    struct {
      v4 pos;
      v4 sz;
    } col;
    struct {
      u32 type;
      u32 enemy_type;
      u64 add_info;
      v4 pos;
      v4 sz;
    } obj;
  };
  struct {
    bool is_masked;
    u64 stencil_val;
  };
};

void LoadSheetFromLayer(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;

  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int layer_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto cur_layer = (aux_layer_info_struct *)&dbg->mem_buffer[layer_offset];
  auto cur_cell = (sp_cell *)(cur_layer + 1);

  int str_tbl_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  auto str_tbl = (char *)&dbg->mem_buffer[str_tbl_offset];

  int tex_width = cur_layer->grid_x * cur_layer->pixels_per_width;
  int tex_height = cur_layer->grid_y * cur_layer->pixels_per_width;
  auto tex_data =
      (char *)AllocMiscData(dbg->lang_stat, tex_width * tex_height * 4);
  char px_width = cur_layer->pixels_per_width;
  char *aux_buffer = AllocMiscData(dbg->lang_stat, px_width * px_width * 4);
  //*tex_width = cur_layer->grid_x * cur_layer->pixels_per_width;
  //*tex_height = cur_layer->grid_y * cur_layer->pixels_per_width;
  // int sz = cur_layer->grid_x * px_width * cur_layer->grid_y * px_width;
  int tex_id =
      GenTexture2(dbg->lang_stat, gl_state, (u8 *)tex_data,
                  cur_layer->grid_x * px_width, cur_layer->grid_y * px_width);
  texture_info *t = &gl_state->textures[tex_id];
  glBindTexture(GL_TEXTURE_2D, t->id);

  for (int c = 0; c < cur_layer->total_of_used_cells; c++) {
    char *tex_name = str_tbl + cur_cell->tex_name;
    texture_raw *tex_src = HasRawTexture(gl_state, tex_name);

    int x_offset = cur_cell->src_tex_offset_x / px_width;
    int y_offset = cur_cell->src_tex_offset_y / px_width;
    auto data_ptr = tex_src->data + x_offset * px_width * 4 +
                    y_offset * tex_src->width * 4 * px_width;
    CopyFromSrcImgToBuffer((char *)data_ptr, aux_buffer, px_width, px_width,
                           tex_src->width);
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, NULL));
    GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, cur_cell->grid_x * px_width,
                             cur_cell->grid_y * px_width, px_width, px_width,
                             GL_RGBA, GL_UNSIGNED_BYTE, aux_buffer));

    cur_cell++;
  }
  heap_free((mem_alloc *)__lang_globals.data, (char *)aux_buffer);
  heap_free((mem_alloc *)__lang_globals.data, (char *)tex_data);

  *(u64 *)GetRegValPtr(thread_id, dbg, RET_1_REG) = tex_id;
}

int CreateSpriteFromLayer(lang_state *lang_stat, open_gl_state *gl_state,
                          aux_layer_info_struct *cur_layer,
                          aux_cell_info *cur_cell, char *str_table) {
  int px_width = cur_layer->pixels_per_width;
  char *aux_buffer = AllocMiscData(lang_stat, px_width * px_width * 4);
  int tex_width = cur_layer->grid_x * cur_layer->pixels_per_width;
  int tex_height = cur_layer->grid_y * cur_layer->pixels_per_width;
  char *tex_data = (char *)AllocMiscData(lang_stat, tex_width * tex_height * 4);
  // int sz = cur_layer->grid_x * px_width * cur_layer->grid_y * px_width;
  int tex_id =
      GenTexture2(lang_stat, gl_state, (u8 *)*tex_data,
                  cur_layer->grid_x * px_width, cur_layer->grid_y * px_width);
  texture_info *t = &gl_state->textures[tex_id];
  glBindTexture(GL_TEXTURE_2D, t->id);

  for (int c = 0; c < cur_layer->total_of_used_cells; c++) {
    char *tex_name = str_table + cur_cell->tex_name;
    texture_raw *tex_src = HasRawTexture(gl_state, tex_name);

    int x_offset = cur_cell->src_tex_offset_x / px_width;
    int y_offset = cur_cell->src_tex_offset_y / px_width;
    auto data_ptr = tex_src->data + x_offset * px_width * 4 +
                    y_offset * tex_src->width * 4 * px_width;
    CopyFromSrcImgToBuffer((char *)data_ptr, aux_buffer, px_width, px_width,
                           tex_src->width);
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, NULL));
    GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, cur_cell->grid_x * px_width,
                             cur_cell->grid_y * px_width, px_width, px_width,
                             GL_RGBA, GL_UNSIGNED_BYTE, aux_buffer));

    cur_cell = (aux_cell_info *)(((char *)cur_cell) + cur_layer->cell_sz);
  }
  heap_free((mem_alloc *)__lang_globals.data, (char *)aux_buffer);
  return 1;
}

int LoadSpriteSheet(dbg_state *dbg, own_std::string sp_file_name,
                    int *tex_width, int *tex_height, int *channels,
                    char **tex_data) {
  u32 read;
  char *file = ReadEntireFileLang((char *)sp_file_name.c_str(), &read);

  auto gl_state = (open_gl_state *)dbg->data;
  auto hdr = (sheet_file_header *)file;

  int cell_size = sizeof(aux_cell_info);
  if (hdr->cell_info_size != cell_size) {
    printf("error file %s: cell sizes different, on file sz is %d, but on "
           "compiler is %d",
           sp_file_name.c_str(), hdr->cell_info_size, cell_size);
    return -1;
  }
  char *str_table = (file + hdr->str_tbl_offset);

  own_std::vector<char *> image_sprite_names;

  u32 cur_ch = 0;

  while (cur_ch < hdr->str_tbl_sz) {
    u32 start = cur_ch;
    while (str_table[cur_ch] != 0) {
      cur_ch++;
    }
    image_sprite_names.emplace_back(&str_table[start]);
    cur_ch++;
  }

  char *cur_ptr = (char *)(hdr + 1);
  int tex_id = 0;
  auto cur_layer = (aux_layer_info_struct *)cur_ptr;
  auto cur_cell = (aux_cell_info *)(cur_layer + 1);
  for (int i = 0; i < hdr->total_layers; i++) {
    // sprites
    switch (cur_layer->type) {
    case 0: {
      if (cur_layer->cell_sz != sizeof(aux_layer_info_struct)) {
        ASSERT(0);
      }

      int total_sprites = cur_layer->total_of_used_cells;
      cur_layer++;
      for (int j = 0; j < total_sprites; j++) {
        cur_cell = (aux_cell_info *)(cur_layer + 1);
        cur_cell = (aux_cell_info *)(((char *)cur_cell) +
                                     cur_layer->cell_sz *
                                         cur_layer->total_of_used_cells);
        cur_layer = (aux_layer_info_struct *)cur_cell;
      }
    } break;
    // colliders
    case 1: {
      for (int c = 0; c < cur_layer->total_of_used_cells; c++) {
        cur_cell = (aux_cell_info *)(((char *)cur_cell) + cur_layer->cell_sz);
      }
    } break;
    // objs
    case 2: {
      for (int c = 0; c < cur_layer->total_of_used_cells; c++) {
        cur_cell = (aux_cell_info *)(((char *)cur_cell) + cur_layer->cell_sz);
      }
    } break;
    default:
      printf("error file %s: layer type not known", sp_file_name.c_str(),
             cur_layer->type);
      return -1;
    }
    if (cur_layer->total_of_used_cells == 0) {
      cur_cell = (aux_cell_info *)(cur_layer + 1);
    }
    cur_ptr = (char *)cur_cell;
    cur_layer = (aux_layer_info_struct *)cur_cell;
    cur_cell = (aux_cell_info *)(cur_layer + 1);
  };
  int val = *(int *)cur_ptr;
  // end of layers
  if (val != 0xbebad0) {
    printf("error file %s: value check at end of layers not matching, expected "
           "0x%04x, found 0x%04x",
           sp_file_name.c_str(), 0x1234, val);
    return -1;
  }
  return 1;
  cur_ptr = (char *)(hdr + 1);

  tex_id = 0;
  cur_layer = (aux_layer_info_struct *)cur_ptr;
  cur_cell = (aux_cell_info *)(cur_layer + 1);
  for (int i = 0; i < hdr->total_layers; i++) {
    // sprites
    switch (cur_layer->type) {
    case 0: {

      int total_sprites = cur_layer->total_of_used_cells;
      cur_layer++;
      for (int j = 0; j < total_sprites; j++) {
        auto cur_cell = (aux_cell_info *)(cur_layer + 1);
        CreateSpriteFromLayer(gl_state->lang_stat, gl_state, cur_layer,
                              cur_cell, str_table);
        cur_cell = (aux_cell_info *)(cur_layer + 1);
        cur_cell = (aux_cell_info *)(((char *)cur_cell) +
                                     cur_layer->cell_sz *
                                         cur_layer->total_of_used_cells);
        cur_layer = (aux_layer_info_struct *)cur_cell;
      }
      /*
      int px_width = cur_layer->pixels_per_width;
      char* aux_buffer = AllocMiscData(dbg->lang_stat, px_width * px_width *
      4); *tex_width = cur_layer->grid_x * cur_layer->pixels_per_width;
      *tex_height = cur_layer->grid_y * cur_layer->pixels_per_width;
      *tex_data = (char *) AllocMiscData(dbg->lang_stat, *tex_width *
      *tex_height * 4);
      //int sz = cur_layer->grid_x * px_width * cur_layer->grid_y * px_width;
      tex_id = GenTexture2(dbg->lang_stat, gl_state, (u8*)*tex_data,
      cur_layer->grid_x * px_width, cur_layer->grid_y * px_width);
      texture_info* t = &gl_state->textures[tex_id];
      glBindTexture(GL_TEXTURE_2D, t->id);

      for (int c = 0; c < cur_layer->total_of_used_cells; c++)
      {
              char* tex_name = str_table + cur_cell->tex_name;
              texture_raw* tex_src = HasRawTexture(gl_state, tex_name);

              int x_offset = cur_cell->src_tex_offset_x / px_width;
              int y_offset = cur_cell->src_tex_offset_y / px_width;
              auto data_ptr = tex_src->data + x_offset * px_width * 4 +
      y_offset
      * tex_src->width * 4 * px_width; CopyFromSrcImgToBuffer((char*)data_ptr,
      aux_buffer, px_width, px_width, tex_src->width);
              //GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, NULL));
      GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, cur_cell->grid_x * px_width,
      cur_cell->grid_y * px_width, px_width, px_width, GL_RGBA,
      GL_UNSIGNED_BYTE, aux_buffer)
              );

              cur_cell = (aux_cell_info *)(((char*)cur_cell) +
      cur_layer->cell_sz);
      }
      heap_free((mem_alloc*)__lang_globals.data, (char*)aux_buffer);
      */
    } break;
    // colliders
    case 1: {
      for (int c = 0; c < cur_layer->total_of_used_cells; c++) {
        cur_cell = (aux_cell_info *)(((char *)cur_cell) + cur_layer->cell_sz);
      }
    } break;
    // objs
    case 2: {
      for (int c = 0; c < cur_layer->total_of_used_cells; c++) {
        cur_cell = (aux_cell_info *)(((char *)cur_cell) + cur_layer->cell_sz);
      }
    } break;
    default:
      ASSERT(false);
    }
    cur_ptr = (char *)cur_cell;
    cur_layer = (aux_layer_info_struct *)cur_cell;
    cur_cell = (aux_cell_info *)(cur_layer + 1);
  }
  return tex_id;
}

void ReadFileInterp(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name = (char *)&dbg->mem_buffer[name_offset];

  int buffer_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  char *buffer_ptr = (char *)&dbg->mem_buffer[buffer_offset];

  u32 size;
  own_std::string work_dir = dbg->cur_func->from_file->path;
  work_dir = work_dir + name;
  char *file = ReadEntireFileLang((char *)work_dir.c_str(), &size);
  memcpy(buffer_ptr, file, size);
}
void GetFileSize(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name = (char *)&dbg->mem_buffer[name_offset];

  own_std::string work_dir = dbg->cur_func->from_file->path;
  work_dir = work_dir + name;
#ifdef LINUX
  struct stat st;

  if (stat(work_dir.c_str(), &st) != 0) {
    perror("stat");
    *(s64 *)GetRegValPtr(thread_id, dbg, RET_1_REG) = -1;
    return;
  }
  *(s64 *)GetRegValPtr(thread_id, dbg, RET_1_REG) = st.st_size;
#else
  LARGE_INTEGER file_size;
  own_std::string work_dir = dbg->cur_func->from_file->path;
  work_dir = work_dir + name;
  HANDLE file = OpenFileLang((char *)work_dir.c_str());
  BOOL val = GetFileSizeEx(file, &file_size);
  auto err = GetLastError();
  ASSERT(val != 0);

  *(u64 *)GetRegValPtr(thread_id, dbg, RET_1_REG) = file_size.QuadPart;
  CloseHandle(file);
#endif
}
int GetMem(int thread_id, dbg_state *dbg, int sz) {
  *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG) -= 16;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  *(int *)&dbg->mem_buffer[base_ptr + 8] = sz;
  int idx = 0;

  GetMem(thread_id, dbg);
  int offset = *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  printf("GetMem: cur %dmb, sz %dmb, offset %dmb\n", offset / 1024 / 1024,
         sz / 1024 / 1024, (offset + sz) / 1024 / 1024);

  *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG) += 16;
  return offset;
}
void LoadSceneFolder(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int folder_name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int ar_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];

  auto gl_state = (open_gl_state *)dbg->data;

  auto folder_name = (char *)&dbg->mem_buffer[folder_name_offset];

  auto ar = (own_std::vector<int> *)&dbg->mem_buffer[ar_offset];

  // new(&dbg->scene_folder)own_std::string(folder_name);
  dbg->scene_folder = dbg->cur_func->from_file->path + folder_name;
  // dbg->scene_folder = (const char *)folder_name;
  // MaybeAddBarToEndOfStr(&dbg->scene_folder);

  own_std::vector<char *> file_names;

  GetFilesInDirectory(dbg->scene_folder, nullptr, &file_names);

  int sz = 0;

  FOR_VEC(name, file_names) { sz += strlen(*name) + 1; }
  if (sz == 0)
    return;

  int str_tbl_offset = file_names.size() * 8;
  sz += str_tbl_offset;
  int offset = GetMem(thread_id, dbg, sz);

  ar->ar.start = (int *)(long long)offset;
  ar->ar.count = file_names.size();

  auto str_tbl_ptr = (char *)&dbg->mem_buffer[offset + str_tbl_offset];
  auto start_idx = (int *)&dbg->mem_buffer[offset];
  auto cur_idx = (long long *)&dbg->mem_buffer[offset];

  int fl_idx = 0;
  int cur_str_tbl_offset = 0;

  FOR_VEC(fl, file_names) {
    int ln = strlen(*fl) + 1;

    memcpy(str_tbl_ptr + cur_str_tbl_offset, *fl, ln);

    *cur_idx = offset + str_tbl_offset + cur_str_tbl_offset;
    cur_idx++;

    cur_str_tbl_offset += ln;
    fl_idx++;
  }
  int a = 0;
}
int HasModel(dbg_state *dbg, own_std::string &name, int *free_idx) {
  auto gl_state = (open_gl_state *)dbg->data;
  for (int i = 0; i < TOTAL_MODELS; i++) {
    model_info *m = &gl_state->models[i];
    if (*free_idx == -1 && m->ebo == 0) {
      *free_idx = i;
    }
    if (m->name == name) {
      return i;
    }
  }
  return -1;
}
struct create_mesh_info {
  u64 verts_offset;
  int verts_count;

  u64 tris_offset;
  int tris_count;

  u64 attribs_offset;
  int attribs_count;
};

void CreateMesh(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int create_mesh_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];

  auto minfo = (create_mesh_info *)&dbg->mem_buffer[create_mesh_offset];
  auto verts = (float *)&dbg->mem_buffer[minfo->verts_offset];
  auto inds = (int *)&dbg->mem_buffer[minfo->tris_offset];
  auto attribs = (u64 *)&dbg->mem_buffer[minfo->attribs_offset];

  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  int vertex_size = 0;
  for (int i = 0; i < minfo->attribs_count; i++) {
    u8 type = attribs[i] & 0xff;
    u8 count = (attribs[i] >> 8) & 0xff;
    switch (type) {
    case 0: {
      vertex_size += sizeof(float) * count;
    } break;
    case 1: {
      vertex_size += sizeof(char) * count;
    } break;
    default: {
      ASSERT(false)
    }
    }
  }
  u32 cur = 0;
  v4 *col = (v4 *)&verts[3];
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, minfo->verts_count * vertex_size, verts,
               GL_DYNAMIC_DRAW);

  for (int i = 0; i < minfo->attribs_count; i++) {
    u8 type = attribs[i] & 0xff;
    u8 count = (attribs[i] >> 8) & 0xff;

    glEnableVertexAttribArray(i);
    switch (type) {
    // float type
    case 0: {
      GL_CALL(glVertexAttribPointer(i, count, GL_FLOAT, GL_FALSE, vertex_size,
                                    (void *)cur));
      cur += sizeof(float) * count;
    } break;
    //  type
    case 1: {
      GL_CALL(glVertexAttribPointer(i, count, GL_BYTE, GL_FALSE, vertex_size,
                                    (void *)cur));
      cur += sizeof(char) * count;
    } break;
    default: {
      ASSERT(false)
    }
    }
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, minfo->tris_count * sizeof(int), inds,
               GL_DYNAMIC_DRAW);

  /*
  GLint stride0, stride1;
  glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride0);
  glGetVertexAttribiv(1, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride1);
  printf("stride loc0=%d, loc1=%d\n", stride0, stride1);

  GLint offset0, offset1;
  glGetVertexAttribPointerv(0, GL_VERTEX_ATTRIB_ARRAY_POINTER,
  (GLvoid**)&offset0); glGetVertexAttribPointerv(1,
  GL_VERTEX_ATTRIB_ARRAY_POINTER, (GLvoid**)&offset1); printf("offset loc0=%d,
  loc1=%d\n", offset0, offset1);
  */

  int free_idx = -1;
  auto gl_state = (open_gl_state *)dbg->data;

  char buffer[64];
  sprintf(buffer, "mesh_%d", gl_state->generated_meshes);
  own_std::string str(buffer);
  int idx = HasModel(dbg, str, &free_idx);

  ASSERT(idx == -1 && free_idx != -1);
  model_info *m = &gl_state->models[free_idx];
  m->vbo = VBO;
  m->vao = VAO;
  m->ebo = EBO;
  m->indicies = minfo->tris_count;
  m->verts_size = minfo->verts_count * vertex_size;

  *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG) = free_idx;
}
void UpdateModel(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int verts_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];

  float *verts = (float *)&dbg->mem_buffer[verts_offset];

  auto gl_state = (open_gl_state *)dbg->data;
  model_info *m = &gl_state->models[model];

  glBindVertexArray(m->vao);
  glBindBuffer(GL_ARRAY_BUFFER, m->vbo);

  glBufferSubData(GL_ARRAY_BUFFER, 0, m->verts_size, verts);
}
void print_row(float *row, int t) {
  for (int i = 0; i < t; i++) {
    printf(" ");
  }
  printf("%.3f, %.3f, %.3f, %.3f\n", row[0], row[1], row[2], row[3]);
}
void print_aimatrix4x4(aiMatrix4x4 *m, int t) {
  print_row(&m->a1, t);
  print_row(&m->b1, t);
  print_row(&m->c1, t);
  print_row(&m->d1, t);
}
#define ADD_TAB(t)                                                             \
  for (int i = 0; i < t; i++) {                                                \
    printf(" ");                                                               \
  }
void PrintBone(model_info *m, bone *b, int t) {
  ADD_TAB(t)

  printf("name: %s\n", b->name.c_str());

  ADD_TAB(t)
  printf("offset:\n");
  print_aimatrix4x4(&b->offset, t);

  /*
  ADD_TAB(t)
  printf("to_parent:\n");
  print_aimatrix4x4(&b->to_parent, t);

  ADD_TAB(t)
  printf("final_transform:\n");
  print_aimatrix4x4(&b->local_matrix, t);
  */

  /*
  ADD_TAB(t)
  printf("pos: (%.3f, %.3f, %.3f)\n", b->pos.x, b->pos.y, b->pos.z);

  ADD_TAB(t)
  printf("scale: (%.3f, %.3f, %.3f, %.3f)\n", b->scale.x, b->scale.y,
  b->scale.z, b->scale.w);

  ADD_TAB(t)
  printf("rot: (%.3f, %.3f, %.3f, %.3f)\n", b->rot.x, b->rot.y, b->rot.z,
  b->rot.w);
  */

  FOR_VEC(ch, b->children) { PrintBone(m, &m->all[*ch], t + 2); }
}
void PrintNode(aiNode *n, int t) {
  for (int i = 0; i < t; i++) {
    printf(" ");
  }

  printf("--name: %s--\n", n->mName.C_Str());

  for (int i = 0; i < t; i++) {
    printf(" ");
  }
  printf("to_parent:\n");
  print_aimatrix4x4(&n->mTransformation, t);
  /*
  if(b->parent)
  {
          printf("to_parent:\n");
          print_aimatrix4x4(&b->to_parent, t);
  }
  FOR_VEC(ch, b->children)
  {
          PrintBone(*ch, t+2);
  }
          */

  for (int i = 0; i < n->mNumChildren; i++) {
    PrintNode(n->mChildren[i], t + 2);
  }
}
void FillBone(bone *b, aiNode *nd, std::unordered_map<std::string, int> *bones,
              bone *all, aiMatrix4x4 *given_mat) {
  b->to_parent = nd->mTransformation;
  *given_mat = *given_mat * b->offset;
  for (int i = 0; i < nd->mNumChildren; i++) {
    aiNode *cur = nd->mChildren[i];
    if (bones->find(cur->mName.C_Str()) != bones->end()) {
      int idx = (*bones)[cur->mName.C_Str()];

      bone *child = &all[idx];
      child->parent = b->idx;
      child->local_matrix = child->offset;
      Mat4 aux = Inverse((const Mat4 &)child->local_matrix);
      memcpy(&child->inv_local_matrix, &aux, 64);
      b->children.emplace_back(child->idx);
      FillBone(child, cur, bones, all, given_mat);
    }
  }
}
void GetModelBonesRootsLen(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];

  model_info *m = &gl_state->models[model_idx];
  auto ret = GetRegValPtr(thread_id, dbg, RET_1_REG);
  ASSERT(m->scene)
  ASSERT(m->scene->HasAnimations())
  *ret = m->roots.size();
}
void GetModelBonesRootsData(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int out_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];

  int *out = (int *)&dbg->mem_buffer[out_offset];

  model_info *m = &gl_state->models[model_idx];

  memcpy(out, m->roots.data(), m->roots.size() * 4);
}
void GetModelBonesLen(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int bones_out = *(int *)&dbg->mem_buffer[base_ptr + 16];

  model_info *m = &gl_state->models[model_idx];
  auto ret = GetRegValPtr(thread_id, dbg, RET_1_REG);
  ASSERT(m->scene)
  ASSERT(m->scene->HasAnimations())
  *ret = m->bones.size();
}
void ModelHasAnim(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];

  model_info *m = &gl_state->models[model_idx];
  auto ret = GetRegValPtr(thread_id, dbg, RET_1_REG);
  if (m->scene) {
    *ret = m->scene->HasAnimations();
    return;
  }
  *ret = 0;
}
void GetBoneMatrices(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int bone_id = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int local_mat_out = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int inv_local_mat_out = *(int *)&dbg->mem_buffer[base_ptr + 32];

  int *local_mat_out_ptr = (int *)&dbg->mem_buffer[local_mat_out];
  int *inv_local_mat_out_ptr = (int *)&dbg->mem_buffer[inv_local_mat_out];

  model_info *m = &gl_state->models[model_idx];
  aiScene *scene = m->scene;
  ASSERT(scene)
  ASSERT(scene->HasAnimations())

  aiAnimation *cur_anim = scene->mAnimations[0];

  auto bone = &m->all[bone_id];

  memcpy(local_mat_out_ptr, &bone->local_matrix, 64);
  memcpy(inv_local_mat_out_ptr, &bone->inv_local_matrix, 64);
}
void GetBoneChildrenLen(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int bone_id = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int out_offset = *(int *)&dbg->mem_buffer[base_ptr + 24];

  int *out = (int *)&dbg->mem_buffer[out_offset];

  model_info *m = &gl_state->models[model_idx];

  bone *b = &m->all[bone_id];

  auto aux = GetRegValPtr(thread_id, dbg, RET_1_REG);
  *aux = b->children.size();
}
void GetBoneName(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int bone_id = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int buffer_offset = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int buffer_sz = *(int *)&dbg->mem_buffer[base_ptr + 32];

  char *out = (char *)&dbg->mem_buffer[buffer_offset];
  model_info *m = &gl_state->models[model_idx];

  bone *b = &m->all[bone_id];

  ASSERT(b->name.size() < buffer_sz);
  memcpy(out, b->name.data(), b->name.size());
  out[b->name.size()] = 0;
}
void GetBoneChildrenData(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int bone_id = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int out_offset = *(int *)&dbg->mem_buffer[base_ptr + 24];

  int *out = (int *)&dbg->mem_buffer[out_offset];

  model_info *m = &gl_state->models[model_idx];

  bone *b = &m->all[bone_id];

  for (int i = 0; i < b->children.size(); i++) {
    bone *cur = &m->all[b->children[i]];
    out[i] = cur->idx;
  }
}
void GetBoneKeyframesData(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int bone_id = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int pos_out = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int scales_out = *(int *)&dbg->mem_buffer[base_ptr + 32];
  int rot_out = *(int *)&dbg->mem_buffer[base_ptr + 40];

  int *pos_out_ptr = (int *)&dbg->mem_buffer[pos_out];
  int *scales_out_ptr = (int *)&dbg->mem_buffer[scales_out];
  int *rot_out_ptr = (int *)&dbg->mem_buffer[rot_out];

  model_info *m = &gl_state->models[model_idx];
  aiScene *scene = m->scene;
  ASSERT(scene)
  ASSERT(scene->HasAnimations())

  // psr means p-osition, s-cale, r-otation
  struct keyframe_psr {
    float time;
    v4 val;
  };

  auto bone_aux = &m->all[bone_id];

  aiNodeAnim *bone = bone_aux->keyframes;

  /*
  if(bone_aux->name == "ik_feet_r")
  {
          HERE()
  }
          */
  // HERE()
  for (int i = 0; i < bone->mNumPositionKeys; i++) {
    auto cur_dst = ((keyframe_psr *)pos_out_ptr) + i;
    auto cur_src = &bone->mPositionKeys[i];
    cur_dst->time = cur_src->mTime;
    memcpy(&cur_dst->val, &cur_src->mValue, 12);
    auto a = 0;
  }
  for (int i = 0; i < bone->mNumScalingKeys; i++) {
    auto cur_dst = ((keyframe_psr *)scales_out_ptr) + i;
    auto cur_src = &bone->mScalingKeys[i];
    cur_dst->time = cur_src->mTime;
    memcpy(&cur_dst->val, &cur_src->mValue, 12);
  }
  for (int i = 0; i < bone->mNumRotationKeys; i++) {
    auto cur_dst = ((keyframe_psr *)rot_out_ptr) + i;
    auto cur_src = &bone->mRotationKeys[i];
    cur_dst->time = cur_src->mTime;
    cur_dst->val.x = cur_src->mValue.x;
    cur_dst->val.y = cur_src->mValue.y;
    cur_dst->val.z = cur_src->mValue.z;
    cur_dst->val.w = cur_src->mValue.w;
  }
}
void GetBoneKeyframesLen(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int bone_id = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int pos_out = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int scales_out = *(int *)&dbg->mem_buffer[base_ptr + 32];
  int rot_out = *(int *)&dbg->mem_buffer[base_ptr + 40];

  int *pos_out_ptr = (int *)&dbg->mem_buffer[pos_out];
  int *scales_out_ptr = (int *)&dbg->mem_buffer[scales_out];
  int *rot_out_ptr = (int *)&dbg->mem_buffer[rot_out];

  model_info *m = &gl_state->models[model_idx];
  aiScene *scene = m->scene;
  ASSERT(scene)
  ASSERT(scene->HasAnimations())

  auto bone_aux = &m->all[bone_id];

  aiNodeAnim *bone = bone_aux->keyframes;

  *pos_out_ptr = bone->mNumPositionKeys;
  *scales_out_ptr = bone->mNumScalingKeys;
  *rot_out_ptr = bone->mNumRotationKeys;
}
void loadIdentity(float *mat) {
  std::fill(mat, mat + 16, 0.0f);
  mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
}

void LoadMesh(float *vertices, unsigned int *indices, aiMesh *mesh,
              model_info *m, int stride, int number_of_bone_ids_per_vertex,
              aiScene *scene, int ind_offset) {
  std::unordered_map<std::string, int> &bones = m->bones;

  if (mesh->HasBones()) {
    int bones_added = 0;

    for (int i = 0; i < mesh->mNumBones; i++) {
      aiBone *b = mesh->mBones[i];

      std::string str(b->mName.C_Str());

      m->all.make_count(m->all.size() + 1);
      bone &cur_bone = m->all.back();
      cur_bone.b = b;
      cur_bone.idx = bones_added;
      cur_bone.parent = -1;
      cur_bone.name = b->mName.C_Str();
      // HERE()
      cur_bone.offset = b->mOffsetMatrix;

      for (int v = 0; v < b->mNumWeights; v++) {
        aiVertexWeight vw = b->mWeights[v];
        int vert_idx = vw.mVertexId * stride;

        // find empty bone id slot
        int empty_slot = -1;
        int *cur_vert = (int *)&vertices[vert_idx];
        int *cur_empty_slot = (int *)&vertices[vert_idx + 5];

        int lowest_idx = 100;
        float lowest_force = 1000.0;
        for (int v = 0; v < number_of_bone_ids_per_vertex; v++) {
          if (*cur_empty_slot == 0) {
            empty_slot = v + 1;
            break;
          }
          cur_empty_slot++;
        }

        ASSERT(empty_slot != -1)

        *cur_empty_slot = i + 1;
        *(float *)&cur_empty_slot[number_of_bone_ids_per_vertex] = vw.mWeight;
      }

      bones[str] = bones_added;

      bones_added++;
    }
    // HERE()
    aiBone root;
    root.mName = m->all[0].name.c_str();
    auto cur_bone_nd = (aiNode *)scene->mRootNode->findBoneNode(&root);

    bone *cur = &m->all[0];
    aiMatrix4x4 identity;
    loadIdentity((float *)&identity);
    cur->local_matrix = cur->offset;
    FillBone(cur, cur_bone_nd, &bones, m->all.data(), &identity);
    // PrintBone(m, cur, 0);

    for (int i = 1; i < bones_added; i++) {
      cur = &m->all[i];
      if (cur->parent != -1)
        continue;
      cur_bone_nd = (aiNode *)scene->mRootNode->findBoneNode(cur->b);

      loadIdentity((float *)&identity);
      cur->local_matrix = cur->offset;
      FillBone(cur, cur_bone_nd, &bones, m->all.data(), &identity);
      // PrintBone(m, cur, 0);
    }
    for (int i = 0; i < bones_added; i++) {
      cur = &m->all[i];
      if (cur->parent != -1)
        continue;
      m->roots.emplace_back(cur->idx);
    }

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      int vert_idx = i * stride;
      v4 *cur = (v4 *)&vertices[vert_idx + 5 + number_of_bone_ids_per_vertex];
      auto total = cur->x + cur->y + cur->z + cur->w;
      if (total > 0) {
        cur->x /= total;
        cur->y /= total;
        cur->z /= total;
        cur->w /= total;
      }
    }
    // ASSERT(0)
  }
  if (mesh->HasVertexColors(0)) {
    for (int i = 0; i < mesh->mNumVertices; i++) {
      int vert_idx = i * stride;
      v4 *cur = (v4 *)&vertices[vert_idx + 5];
      *cur = *(v4 *)&(mesh->mColors[0])[i];
    }
  }
  if (scene->HasAnimations()) {
    for (int i = 0; i < scene->mNumAnimations; i++) {
      aiAnimation *cur_anim = scene->mAnimations[i];
      printf("anim tiks per sec %.3f\n", (float)cur_anim->mTicksPerSecond);
      for (int a = 0; a < cur_anim->mNumChannels; a++) {
        aiNodeAnim *nd_anim = cur_anim->mChannels[a];
        printf("ndanim_name: %s, duration\n", nd_anim->mNodeName.C_Str(),
               cur_anim->mDuration / cur_anim->mTicksPerSecond);
        auto str = nd_anim->mNodeName.C_Str();
        if (bones.find(str) != bones.end()) {
          int id = bones[str];
          bone *cur_b = &m->all[id];
          cur_b->keyframes = nd_anim;
          printf("positions\n");
          for (int pos_k = 0; pos_k < nd_anim->mNumPositionKeys; pos_k++) {
            aiVectorKey p = nd_anim->mPositionKeys[pos_k];
            printf("key time: %.3f, pos: (%.3f, %.3f, %.3f)\n", p.mTime,
                   p.mValue.x, p.mValue.y, p.mValue.z);
          }
          printf("-----");

          printf("rotations\n");
          for (int pos_k = 0; pos_k < nd_anim->mNumRotationKeys; pos_k++) {
            aiQuatKey p = nd_anim->mRotationKeys[pos_k];
            printf("key time: %.3f, pos: (%.3f, %.3f, %.3f, %.3f)\n", p.mTime,
                   p.mValue.x, p.mValue.y, p.mValue.z, p.mValue.w);
          }
          printf("-----");
        }
      }
    }
    // HERE()
  }

  float max_x = 0.0;
  float max_y = 0.0;
  float max_z = 0.0;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vec3 *v = (Vec3 *)&mesh->mVertices[i].x;

    //*v = rotate(*v, Vec3(1.0, 0.0, 0.0), -3.1415 * 0.5) * 0.5;
    //*v = rotate(*v, Vec3(0.0, 1.0, 0.0), -3.1415);

    max_x = max(max_x, abs(v->x));
    max_y = max(max_y, abs(v->y));
    max_z = max(max_z, abs(v->z));
    // scene->mMaterials

    vertices[i * stride + 0] = mesh->mVertices[i].x;
    vertices[i * stride + 1] = mesh->mVertices[i].y;
    vertices[i * stride + 2] = mesh->mVertices[i].z;
    vertices[i * stride + 3] = mesh->mTextureCoords[0][i].x;
    vertices[i * stride + 4] = mesh->mTextureCoords[0][i].y;
    if (scene->mNumMeshes > 1) {
      vertices[i * stride + 9 + mesh->mMaterialIndex] = 1.0;
    }
  }

  m->size.x = max_x * 0.5;
  m->size.y = max_y * 0.5;
  m->size.z = max_z * 0.5;

  // Create index array
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    const struct aiFace *face = &mesh->mFaces[i];
    if (face->mNumIndices != 3)
      continue; // skip non-triangles
    indices[i * 3 + 0] = face->mIndices[0] + ind_offset;
    indices[i * 3 + 1] = face->mIndices[1] + ind_offset;
    indices[i * 3 + 2] = face->mIndices[2] + ind_offset;
  }
}
void LoadModelBase(int thread_id, dbg_state *dbg, own_std::string full_path,
                   int use_model_idx = -1) {
  auto gl_state = (open_gl_state *)dbg->data;
  int free_idx = -1;
  if (use_model_idx == -1) {
    int idx = HasModel(dbg, full_path, &free_idx);
    if (idx != -1) {
      *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG) = idx;
      ASSERT(0)
      return;
    }
  } else {
    free_idx = use_model_idx;
    model_info *m = &gl_state->models[free_idx];
    full_path = m->name;
  }

  const struct aiScene *scene =
      aiImportFile(full_path.c_str(),
                   aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                       aiProcess_GenNormals | aiProcess_ImproveCacheLocality);

  if (!scene) {
    printf("Failed to load FBX: %s\n", aiGetErrorString());
    ASSERT(0)
    return;
  }

  const struct aiMesh *mesh = scene->mMeshes[0]; // Assume first mesh

  model_info *m = &gl_state->models[free_idx];
  new (m) model_info();

  m->name = full_path;

  m->scene = (aiScene *)scene;

  int stride = 5;

  int number_of_bone_ids_per_vertex = 4;
  bool has_bones = false;
  bool has_vertex_colors = false;

  if (mesh->HasBones()) {
    has_bones = true;
    // HERE()
    //  4 for bones ids, 4 for weights
    stride += number_of_bone_ids_per_vertex + 4;
  }
  if (mesh->HasVertexColors(0)) {
    has_vertex_colors = true;
    // HERE()
    if (has_bones) {
      printf("at the moment we dont allow a mesh to have bones and vertex "
             "colors\n");
      HERE();
    }
    stride += 4;
  }
  bool has_more_than_one_mesh = scene->mNumMeshes > 1;

  if (has_more_than_one_mesh) {
    if (has_bones) {
      printf("at the moment we dont allow a models that have more than one "
             "mesh to have bones "
             "colors\n");
      HERE();
    }
    // HERE()
    stride += 4;
  }

  m->vertex_stride = stride;

  int size = 0;
  unsigned int index_count = 0;
  for (int i = 0; i < scene->mNumMeshes; i++) {
    aiMesh *cur_mesh = scene->mMeshes[i];
    size += cur_mesh->mNumVertices * stride * sizeof(float);
    index_count += cur_mesh->mNumFaces * 3;

    /*
    aiMaterial *mat = scene->mMaterials[cur_mesh->mMaterialIndex];
    for (int type = aiTextureType_NONE; type <= aiTextureType_UNKNOWN; type++)
    { aiTextureType texType = (aiTextureType)type;

      aiString path;
      if (mat->GetTexture(texType, 0, &path) == AI_SUCCESS) {
        own_std::string str;
        str = path.C_Str();
        auto file_name = GetFileNameOnly(str);
            printf("Texture Type %d Path: %s\n", texType, path.C_Str());
        // PFNGLVERTEXSTREAM4IATIPROC
      }
    }
    */
  }
  auto indices = (unsigned int *)AllocMiscData(
      dbg->lang_stat, index_count * sizeof(unsigned int));

  auto vertices = (float *)AllocMiscData(dbg->lang_stat, size);
  memset(vertices, 0, size);
  auto vert_offset = 0;
  auto ind_offset = 0;
  if (has_more_than_one_mesh) {
    // HERE()
  }
  for (int i = 0; i < scene->mNumMeshes; i++) {
    aiMesh *cur_mesh = scene->mMeshes[i];
    LoadMesh(&vertices[vert_offset], &indices[ind_offset], (aiMesh *)cur_mesh,
             m, stride, number_of_bone_ids_per_vertex, (aiScene *)scene,
             vert_offset / stride);

    vert_offset += cur_mesh->mNumVertices * stride;
    ind_offset += cur_mesh->mNumFaces * 3;
  }

  printf("Loaded mesh: %d vertices, %d indices\n", mesh->mNumVertices,
         index_count);

  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vert_offset * sizeof(float), vertices,
               GL_STATIC_DRAW);

  GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                stride * sizeof(float), (void *)0));
  glEnableVertexAttribArray(0);
  GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                                stride * sizeof(float),
                                (void *)(3 * sizeof(float))));
  glEnableVertexAttribArray(1);
  if (has_bones) {
    glVertexAttribIPointer(2, 4, GL_INT, stride * sizeof(float),
                           (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GL_CALL(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,
                                  stride * sizeof(float),
                                  (void *)(9 * sizeof(float))));
    glEnableVertexAttribArray(3);
  }

  if (has_vertex_colors) {
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                          (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
  }

  if (has_more_than_one_mesh) {
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                          (void *)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(int), indices,
               GL_STATIC_DRAW);

  m->vbo = VBO;
  m->vao = VAO;
  m->ebo = EBO;
  m->indicies = index_count;

  m->model_verts_count = vert_offset;
  m->vertices = vertices;
  m->indices_data = (int *)indices;

  // free(vertices);
  // free(indices);
  // aiReleaseImport(scene);

  *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG) = free_idx;
  auto a = 0;
}
void ModelFarthestPoint(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int axis_ptr = *(int *)&dbg->mem_buffer[base_ptr + 16];
  auto v = (Vec3 *)&dbg->mem_buffer[axis_ptr];

  auto gl_state = (open_gl_state *)dbg->data;

  model_info *m = &gl_state->models[model_idx];

  int idx = 0;
  float min = 0.0;
  int vert_size = 3;
  for (int i = 0; i < m->model_verts_count; i++) {
    float *cur = &m->vertices[i * vert_size];
    auto cur_vec = (Vec3 *)cur;

    float d = vec3_dot(*v, *cur_vec);
    if (min < d) {
      idx = i;
      min = d;
    }
  }

  auto ret = GetFloatRegValPtr(thread_id, dbg, FLOAT_REG_0);
  auto aux = GetRegValPtr(thread_id, dbg, RET_1_REG);

  auto p = (Vec3 *)&m->vertices[idx * vert_size];
  memcpy(ret, p, 16);
  memcpy(aux, p, 12);
  *(((float *)ret) + 3) = 0.0f;
}
void GetInfoFromModel(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int out_verts_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int out_indices_offset = *(int *)&dbg->mem_buffer[base_ptr + 24];

  int *out_verts = (int *)&dbg->mem_buffer[out_verts_offset];
  int *out_indices = (int *)&dbg->mem_buffer[out_indices_offset];

  auto gl_state = (open_gl_state *)dbg->data;

  model_info *m = &gl_state->models[model];

  *out_verts = m->model_verts_count;
  *out_indices = m->indicies;
}
void CopyDataFromModel(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int verts_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int verts_count = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int indices_offset = *(int *)&dbg->mem_buffer[base_ptr + 32];
  int indices_count = *(int *)&dbg->mem_buffer[base_ptr + 40];

  int out_verts_offset = *(int *)&dbg->mem_buffer[base_ptr + 48];
  int out_indices_offset = *(int *)&dbg->mem_buffer[base_ptr + 56];

  int *out_verts = (int *)&dbg->mem_buffer[out_verts_offset];
  int *out_indices = (int *)&dbg->mem_buffer[out_indices_offset];

  auto gl_state = (open_gl_state *)dbg->data;

  model_info *m = &gl_state->models[model];

  int *verts_ptr = (int *)&dbg->mem_buffer[verts_offset];
  int *indices_ptr = (int *)&dbg->mem_buffer[indices_offset];
  int stride = m->vertex_stride;

  for (int i = 0; i < m->model_verts_count; i++) {
    auto m_vert = &m->vertices[i * stride];
    auto target_vert = (v4 *)&verts_ptr[i * 4];
    memcpy(target_vert, m_vert, 3 * 4);
    // printf("vx %.3f, vy %.3f, vz %.3f\n", target_vert->x, target_vert->y,
    // target_vert->z);
    ((v4 *)target_vert)->w = 0.0;
  }

  memcpy(indices_ptr, m->indices_data, m->indicies * sizeof(int));

  *out_verts = m->model_verts_count;
  *out_indices = m->indicies;
}
void FreeModel(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_path_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto gl_state = (open_gl_state *)dbg->data;
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];

  model_info *m = &gl_state->models[model_idx];

  /*
  glDeletBuffers(1, &m->ebo);
  glDeletBuffers(1, &m->vbo);

  free(m->vertices);
  free(m->indicies);

  aiReleaseImport(m->scene);
  */
}
void ReloadModel(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];

  auto gl_state = (open_gl_state *)dbg->data;

  model_info *m = &gl_state->models[model_idx];

  aiReleaseImport(m->scene);

  glDeleteBuffers(1, &m->vao);
  glDeleteBuffers(1, &m->vbo);
  glDeleteBuffers(1, &m->ebo);

  // own_std::string full_path = gl_state->model_folder + m->name;
  LoadModelBase(thread_id, dbg, "", model_idx);
}
void LoadModel(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int model_path_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *model_path = (char *)&dbg->mem_buffer[model_path_offset];

  unsigned int size = 0;
  auto gl_state = (open_gl_state *)dbg->data;
  own_std::string full_path = gl_state->model_folder + model_path;
  LoadModelBase(thread_id, dbg, full_path);
}
void LoadModelFolder(int thread_id, dbg_state *dbg) {

  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int folder_name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int ar_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];

  auto folder_name = (char *)&dbg->mem_buffer[folder_name_offset];
  auto ar = (own_std::vector<int> *)&dbg->mem_buffer[ar_offset];

  auto gl_state = (open_gl_state *)dbg->data;
  gl_state->texture_folder = folder_name;
  if (!dbg->cur_func) {
    dbg->cur_func = GetFuncBasedOnBc2(dbg, *dbg->cur_bc2);
  }
  own_std::string work_dir = dbg->cur_func->from_file->path;
  // MaybeAddBarToEndOfStr(&work_dir);

  gl_state->model_folder = work_dir + gl_state->model_folder;
  MaybeAddBarToEndOfStr(&(gl_state->model_folder));

  own_std::vector<char *> file_names;
  GetFilesInDirectory(gl_state->texture_folder, nullptr, &file_names);
}
void LoadTexFolder(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int folder_name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int ar_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];

  auto folder_name = (char *)&dbg->mem_buffer[folder_name_offset];
  auto ar = (own_std::vector<int> *)&dbg->mem_buffer[ar_offset];

  auto gl_state = (open_gl_state *)dbg->data;
  gl_state->texture_folder = folder_name;
  if (!dbg->cur_func) {
    dbg->cur_func = GetFuncBasedOnBc2(dbg, *dbg->cur_bc2);
  }
  own_std::string work_dir = dbg->cur_func->from_file->path;
  // MaybeAddBarToEndOfStr(&work_dir);

  gl_state->texture_folder = work_dir + gl_state->texture_folder;
  MaybeAddBarToEndOfStr(&(gl_state->texture_folder));

  own_std::vector<char *> file_names;
  GetFilesInDirectory(gl_state->texture_folder, nullptr, &file_names);

  struct texture_info {
    u64 name;
    u64 data;
    u32 x_offset;
    u32 y_offset;
    u32 width;
    u32 height;
    u8 channels;
    u64 idx;
    u32 idx_on_array;
    u32 channel_size;
  };
  int total_pngs = 0;
  FOR_VEC(name_ptr, file_names) {
    char *name = *name_ptr;
    own_std::string str = name;
    int p_idx = str.find_last_of('.');
    own_std::string ext = str.substr(p_idx + 1);
    if (!(ext == "png" || ext == "sp"))
      continue;
    total_pngs++;
  }

  int offset = GetMem(thread_id, dbg, total_pngs * sizeof(texture_info));

  ar->ar.start = (int *)(u64)offset;
  ar->ar.count = total_pngs;

  int i = 0;
  auto cur_tex = (texture_info *)&dbg->mem_buffer[offset];
  FOR_VEC(name_ptr, file_names) {
    char *name = *name_ptr;
    own_std::string str = name;
    int p_idx = str.find_last_of('.');
    own_std::string ext = str.substr(p_idx + 1);

    int tex_idx = 0;
    int tex_width = 0;
    int tex_height = 0;
    int tex_channels = 4;
    char *tex_data = nullptr;
    if (ext == "sp") {
      continue;
      tex_idx =
          LoadSpriteSheet(dbg, gl_state->texture_folder + name, &tex_width,
                          &tex_height, &tex_channels, &tex_data);
      if (tex_idx == -1) {
        cur_tex->idx = -1;
        continue;
      }
    } else if (ext == "png") {
      texture_raw *tex_raw = HasRawTexture(gl_state, str);

      tex_idx = GenTexture2(dbg->lang_stat, gl_state, tex_raw->data,
                            tex_raw->width, tex_raw->height);
      tex_width = tex_raw->width;
      tex_height = tex_raw->height;
      tex_channels = tex_raw->channels;
      tex_data = (char *)tex_raw->data;
      ASSERT(tex_data);
    } else {
      continue;
    }

    int len = strlen(name) + 1;
    int name_offset = GetMem(thread_id, dbg, len);
    auto name_dst = (char *)&dbg->mem_buffer[name_offset];
    memcpy(name_dst, name, len);

    int sz = tex_height * tex_width * tex_channels;
    int data_offset = GetMem(thread_id, dbg, sz);
    memcpy(&dbg->mem_buffer[data_offset], tex_data, sz);

    cur_tex->name = name_offset;
    cur_tex->data = data_offset;
    cur_tex->width = tex_width;
    cur_tex->height = tex_height;
    cur_tex->channels = tex_channels;
    cur_tex->idx = tex_idx;
    cur_tex->idx_on_array = i;

    printf("img name %s, data_off %d, width %d\n", name, cur_tex->data,
           cur_tex->width);

    cur_tex++;

    i++;
  }
  auto end = 0;
}

void LoadTex(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);

  auto info = (load_clip_args *)&dbg->mem_buffer[base_ptr + 8];
  info->file_name =
      (unsigned char *)&dbg->mem_buffer[(long long)info->file_name];
  // info->cinfo = (clip*)&dbg->mem_buffer[(long long)info->cinfo];
  // info->cinfo->total_texs = info->total_sps;
  // info->cinfo->len = info->len;

  auto gl_state = (open_gl_state *)dbg->data;

  texture_raw *tex_raw =
      HasRawTexture(gl_state, own_std::string((char *)info->file_name));
  int width, height, nrChannels;
  unsigned char *src = nullptr;
  src = tex_raw->data;
  width = tex_raw->width;
  height = tex_raw->height;
  nrChannels = tex_raw->channels;

  if (info->sp_width == 0) {
    info->sp_width = width;
    info->sp_height = height;
  }

  int idx =
      GenTexture(dbg->lang_stat, gl_state, src, info->sp_width, info->sp_height,
                 info->x_offset, info->y_offset, width, height, 0);
  auto ret = (int *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *ret = idx;
}
void ImageFolderToFile(int thread_id, own_std::string folder) {
  return;
  struct file_header {
    unsigned int total_imgs;
    unsigned int data_sect_offset;
    unsigned int str_tbl_offset;
  };
  struct file_png {
    unsigned int name;
    unsigned int width;
    unsigned int height;
    unsigned char channels;
    unsigned int data;
  };
  own_std::vector<char *> file_names;
  own_std::vector<unsigned char> file_data;
  own_std::vector<unsigned char> data_sect;
  own_std::vector<unsigned char> str_table;
  GetFilesInDirectory((char *)folder.c_str(), nullptr, &file_names);

  own_std::vector<char *> sprite_sheets;

  int total_imgs = 0;
  FOR_VEC(ptr, file_names) {
    own_std::string str = *ptr;
    int p_idx = str.find_last_of('.');
    own_std::string ext = str.substr(p_idx + 1);
    if (ext == "png") {
      int cur_str_table_offset = str_table.size();
      auto c_str = (unsigned char *)str.c_str();
      str_table.insert(str_table.end(), c_str, c_str + str.size() + 1);

      int cur_offset = file_data.size();
      file_data.make_count(file_data.size() + sizeof(file_png));
      auto cur_file = (file_png *)(file_data.begin() + cur_offset);

      int width, height, nrChannels;
      unsigned char *src = nullptr;
      stbi_set_flip_vertically_on_load(true);

      src = stbi_load((char *)(folder + str).c_str(), &width, &height,
                      &nrChannels, 0);
      ASSERT(src);

      cur_file->name = cur_str_table_offset;
      cur_file->width = width;
      cur_file->height = height;
      cur_file->channels = nrChannels;
      cur_file->data = data_sect.size();

      data_sect.insert(data_sect.end(), src,
                       src + (width * height * nrChannels));
      stbi_image_free(src);
      total_imgs++;
    }
  }
  own_std::vector<unsigned char> final_buffer;

  INSERT_VEC(final_buffer, file_data);
  int data_sect_offset = final_buffer.size();
  INSERT_VEC(final_buffer, data_sect);
  int str_tbl_offset = final_buffer.size();
  INSERT_VEC(final_buffer, str_table);

  file_header hdr;
  hdr.total_imgs = total_imgs;
  hdr.str_tbl_offset = str_tbl_offset;
  hdr.data_sect_offset = data_sect_offset;

  final_buffer.insert(final_buffer.begin(), (unsigned char *)&hdr,
                      (unsigned char *)(&hdr + 1));

  int size = final_buffer.size();
  if ((size % 4) != 0)
    size += 4 - (size % 4);
  final_buffer.make_count(size);

  own_std::string imgs_str((char *)final_buffer.data(), final_buffer.size());
  own_std::string images_encoded_str = base64_encode(imgs_str);

  WriteFileLang("../web/images.data", (void *)images_encoded_str.data(),
                images_encoded_str.size());
}

void FromGamePlayAudio(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  float volume = *(float *)&dbg->mem_buffer[base_ptr + 16];
  float speed = *(float *)&dbg->mem_buffer[base_ptr + 24];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];
  auto gl_state = (open_gl_state *)dbg->data;
  AudioClip *clip = nullptr;
  FOR_VEC(it, gl_state->sound->audio_clips_src) {
    if ((*it)->name == name_str) {
      clip = *it;
      break;
    }
  }
  ASSERT(clip);
  AudioClipQueued q = {};
  q.volume = volume;
  q.speed = speed;
  q.clip = clip;
  gl_state->sound->audio_clips_to_play.emplace_back(q);
}

void PlayAudioByHandle(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int audio_clip = *(int *)&dbg->mem_buffer[base_ptr + 8];
  float volume = *(float *)&dbg->mem_buffer[base_ptr + 16];
  float speed = *(float *)&dbg->mem_buffer[base_ptr + 24];

  auto gl_state = (open_gl_state *)dbg->data;
  AudioClip *clip = gl_state->sound->audio_clips_src[audio_clip];
  ASSERT(clip);
  AudioClipQueued q = {};
  q.volume = volume;
  q.speed = speed;
  q.clip = clip;
  gl_state->sound->audio_clips_to_play.emplace_back(q);
}
void GetAudioHandle(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];

  auto gl_state = (open_gl_state *)dbg->data;
  auto sound = gl_state->sound;

  int i = 0;
  for (; i < gl_state->sound->audio_clips_src.size(); i++) {
    if (sound->audio_clips_src[i]->name == name_str) {
      break;
    }
  }

  *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG) = i;
}
void AssignSoundFolder(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];

  auto gl_state = (open_gl_state *)dbg->data;

  own_std::string sound_folder;
  if (!dbg->cur_func) {
    dbg->cur_func = GetFuncBasedOnBc2(dbg, *dbg->cur_bc2);
  }
  own_std::string work_dir = dbg->cur_func->from_file->path;
  // MaybeAddBarToEndOfStr(&work_dir);
  sound_folder = work_dir + name_str;
  MaybeAddBarToEndOfStr(&sound_folder);

  own_std::vector<char *> file_names;
  GetFilesInDirectory((char *)sound_folder.c_str(), nullptr, &file_names);

  char buffer[256];
  int sz = sound_folder.size();
  memcpy(buffer, sound_folder.data(), sz);
  auto ar = &gl_state->sound->audio_clips_src;
  ar->reserve(4);

  FOR_VEC(str_ptr, file_names) {
    snprintf(&buffer[sz], 256, "%s", *str_ptr);
    AudioClip *clip = CreateNewAudioClip(gl_state, buffer);
    clip->name = own_std::string(*str_ptr);
    ar->emplace_back(clip);
  }

  // ImageFolderToFile(gl_state->texture_folder);
}
void AssignModelFolder(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];

  auto gl_state = (open_gl_state *)dbg->data;

  gl_state->model_folder = name_str;
  if (!dbg->cur_func) {
    dbg->cur_func = GetFuncBasedOnBc2(dbg, *dbg->cur_bc2);
  }
  own_std::string work_dir = dbg->cur_func->from_file->path;
  // MaybeAddBarToEndOfStr(&work_dir);

  gl_state->model_folder = work_dir + gl_state->model_folder;
  MaybeAddBarToEndOfStr(&(gl_state->model_folder));

  // ImageFolderToFile(gl_state->texture_folder);
  own_std::vector<char *> file_names;
  GetFilesInDirectory((char *)gl_state->model_folder.c_str(), nullptr,
                      &file_names);
}
void FreeHandle(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int hidx = *(int *)&dbg->mem_buffer[base_ptr + 8];

  handle_info *h = &dbg->handles[hidx];
  h->in_use = false;
}
void FileChangeTime(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name = *(int *)&dbg->mem_buffer[base_ptr + 8];

  char *name_ptr = (char *)&dbg->mem_buffer[name];

  if (!dbg->cur_func) {
    dbg->cur_func = GetFuncBasedOnBc2(dbg, *dbg->cur_bc2);
  }
  own_std::string work_dir = dbg->cur_func->from_file->path;
  // MaybeAddBarToEndOfStr(&work_dir);

  auto final_name = work_dir + name_ptr;

  name_ptr = final_name.c_str();

#ifdef LINUX
  struct stat st;
  auto val = stat(name_ptr, &st);
  if (val == -1) {
    printf("cant open file %s\n", name_ptr);
    ASSERT(0);
  }

  /*
  // Convert to local time
  struct tm localTime = *localtime(&st.st_mtime);
  std::cout << std::endl<<name_ptr << " last modified: "
            << localTime.tm_year + 1900 << "-"
            << localTime.tm_mon + 1 << "-"
            << localTime.tm_mday << " "
            << localTime.tm_hour << ":"
            << localTime.tm_min << ":"
            << localTime.tm_sec
            << std::endl;
            */

  *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG) = st.st_mtime;
#else
  ASSERT(0)
#endif
}
void HandleDirFilenameAt(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int handle_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int idx = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int buffer_offset = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int buffer_size = *(int *)&dbg->mem_buffer[base_ptr + 32];

  char *buffer = (char *)&dbg->mem_buffer[buffer_offset];

  handle_info *h = &dbg->handles[handle_idx];
  ASSERT(h->type == handle_enum::FILES_DIR);

  char *name = h->dir->files[idx];
  int str_ln = strlen(name);

  ASSERT(str_ln < buffer_size);

  memcpy(buffer, name, str_ln);
  buffer[str_ln] = 0;
}
void HandleDirTotalFiles(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int idx = *(int *)&dbg->mem_buffer[base_ptr + 8];

  handle_info *h = &dbg->handles[idx];
  ASSERT(h->type == handle_enum::FILES_DIR);
  *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG) = h->dir->files.size();
}
void HandleForGettingFilesInDir(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];

  int idx = GetFreeHandle(dbg);

  handle_info *h = &dbg->handles[idx];
  h->dir = (handle_info::dir_files *)AllocMiscData(
      dbg->lang_stat, sizeof(handle_info::dir_files));
  h->type = handle_enum::FILES_DIR;

  if (!dbg->cur_func) {
    dbg->cur_func = GetFuncBasedOnBc2(dbg, *dbg->cur_bc2);
  }
  own_std::string work_dir = dbg->cur_func->from_file->path;
  h->dir->path = work_dir + name_str;
  MaybeAddBarToEndOfStr(&h->dir->path);
  GetFilesInDirectory((char *)h->dir->path.c_str(), nullptr, &h->dir->files);
  *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG) = idx;
}
void CloseFile(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int h = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int data_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int data_size = *(int *)&dbg->mem_buffer[base_ptr + 24];

  auto data = (void *)&dbg->mem_buffer[data_offset];

  handle_info *hfile = &dbg->handles[h];
  ASSERT(hfile->type == handle_enum::FILE)

  fclose(hfile->file);
  hfile->in_use = false;
}
void WriteToFile(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int h = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int data_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int data_size = *(int *)&dbg->mem_buffer[base_ptr + 24];

  auto data = (void *)&dbg->mem_buffer[data_offset];

  handle_info *hfile = &dbg->handles[h];
  ASSERT(hfile->type == handle_enum::FILE)

  fwrite(data, 1, data_size, hfile->file);
}
void SetFilePtr(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int h = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int offset = *(int *)&dbg->mem_buffer[base_ptr + 16];

  handle_info *hfile = &dbg->handles[h];
  ASSERT(hfile->type == handle_enum::FILE)

  fseek(hfile->file, offset, SEEK_SET);
}
/*
void TruncateFile(int thread_id, dbg_state *dbg)
{
        int base_ptr = *(int*)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
        int h = *(int*)&dbg->mem_buffer[base_ptr + 8];
        int offset = *(int*)&dbg->mem_buffer[base_ptr + 16];

        int idx = GetFreeHandle(dbg);
        handle_info *hfile = &dbg->handles[idx];
        hfile->type = handle_enum::FILE;

        ftruncate(hfile->file, offset);
}
*/
void OpenFile(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];

  auto name = (char *)&dbg->mem_buffer[name_offset];

  int idx = GetFreeHandle(dbg);
  handle_info *hfile = &dbg->handles[idx];
  hfile->type = handle_enum::FILE;

  if (!dbg->cur_func) {
    dbg->cur_func = GetFuncBasedOnBc2(dbg, *dbg->cur_bc2);
  }
  own_std::string work_dir = dbg->cur_func->from_file->path + name;
  FILE *file = fopen(work_dir.c_str(), "wb");
  if (!file) {
    printf("Failed to open %s ", name);
    perror("Failed to open file");
    ASSERT(0)
    return;
  }
  hfile->file = file;
  *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG) = idx;
}
void AssignTexFolder(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *name_str = (char *)&dbg->mem_buffer[name_offset];

  auto gl_state = (open_gl_state *)dbg->data;

  gl_state->texture_folder = name_str;
  if (!dbg->cur_func) {
    dbg->cur_func = GetFuncBasedOnBc2(dbg, *dbg->cur_bc2);
  }
  own_std::string work_dir = dbg->cur_func->from_file->path;
  // MaybeAddBarToEndOfStr(&work_dir);

  gl_state->texture_folder = work_dir + gl_state->texture_folder;
  MaybeAddBarToEndOfStr(&(gl_state->texture_folder));

  ImageFolderToFile(thread_id, gl_state->texture_folder);
}

static int SEED = 0;

static int hash[] = {
    208, 34,  231, 213, 32,  248, 233, 56,  161, 78,  24,  140, 71,  48,  140,
    254, 245, 255, 247, 247, 40,  185, 248, 251, 245, 28,  124, 204, 204, 76,
    36,  1,   107, 28,  234, 163, 202, 224, 245, 128, 167, 204, 9,   92,  217,
    54,  239, 174, 173, 102, 193, 189, 190, 121, 100, 108, 167, 44,  43,  77,
    180, 204, 8,   81,  70,  223, 11,  38,  24,  254, 210, 210, 177, 32,  81,
    195, 243, 125, 8,   169, 112, 32,  97,  53,  195, 13,  203, 9,   47,  104,
    125, 117, 114, 124, 165, 203, 181, 235, 193, 206, 70,  180, 174, 0,   167,
    181, 41,  164, 30,  116, 127, 198, 245, 146, 87,  224, 149, 206, 57,  4,
    192, 210, 65,  210, 129, 240, 178, 105, 228, 108, 245, 148, 140, 40,  35,
    195, 38,  58,  65,  207, 215, 253, 65,  85,  208, 76,  62,  3,   237, 55,
    89,  232, 50,  217, 64,  244, 157, 199, 121, 252, 90,  17,  212, 203, 149,
    152, 140, 187, 234, 177, 73,  174, 193, 100, 192, 143, 97,  53,  145, 135,
    19,  103, 13,  90,  135, 151, 199, 91,  239, 247, 33,  39,  145, 101, 120,
    99,  3,   186, 86,  99,  41,  237, 203, 111, 79,  220, 135, 158, 42,  30,
    154, 120, 67,  87,  167, 135, 176, 183, 191, 253, 115, 184, 21,  233, 58,
    129, 233, 142, 39,  128, 211, 118, 137, 139, 255, 114, 20,  218, 113, 154,
    27,  127, 246, 250, 1,   8,   198, 250, 209, 92,  222, 173, 21,  88,  102,
    219};

int noise2(int x, int y) {
  int tmp = hash[(y + SEED) % 256];
  return hash[(tmp + x) % 256];
}

float lin_inter(float x, float y, float s) { return x + s * (y - x); }

float smooth_inter(float x, float y, float s) {
  return lin_inter(x, y, s * s * (3 - 2 * s));
}

float noise2d(float x, float y) {
  int x_int = x;
  int y_int = y;
  float x_frac = x - x_int;
  float y_frac = y - y_int;
  int s = noise2(x_int, y_int);
  int t = noise2(x_int + 1, y_int);
  int u = noise2(x_int, y_int + 1);
  int v = noise2(x_int + 1, y_int + 1);
  float low = smooth_inter(s, t, x_frac);
  float high = smooth_inter(u, v, x_frac);
  return smooth_inter(low, high, y_frac);
}

float perlin2d(float x, float y, float freq, int depth) {
  float xa = x * freq;
  float ya = y * freq;
  float amp = 1.0;
  float fin = 0;
  float div = 0.0;

  int i;
  for (i = 0; i < depth; i++) {
    div += 256 * amp;
    fin += noise2d(xa, ya) * amp;
    amp /= 2;
    xa *= 2;
    ya *= 2;
  }

  return fin / div;
}

void Perlin2D(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float x = *(float *)&dbg->mem_buffer[base_ptr + 8];
  float y = *(float *)&dbg->mem_buffer[base_ptr + 16];
  float freq = *(float *)&dbg->mem_buffer[base_ptr + 24];
  int depth = *(int *)&dbg->mem_buffer[base_ptr + 32];
  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) =
      perlin2d(x, y, freq, depth);
}
void LoadClip(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);

  auto info = (load_clip_args *)&dbg->mem_buffer[base_ptr + 8];
  info->file_name =
      (unsigned char *)&dbg->mem_buffer[(long long)info->file_name];
  info->cinfo = (clip *)&dbg->mem_buffer[(long long)info->cinfo];
  info->cinfo->total_texs = info->total_sps;
  info->cinfo->len = info->len;

  auto gl_state = (open_gl_state *)dbg->data;

  *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG) -= 16;
  base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  *(int *)&dbg->mem_buffer[base_ptr + 8] = info->total_sps * sizeof(int);
  int idx = 0;

  GetMem(thread_id, dbg);
  int offset = *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG);

  *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG) += 16;

  *(int **)&info->cinfo->texs_idxs = (int *)(long long)offset;
  info->cinfo->total_texs = info->total_sps;

  // load and generate the texture
  int width, height, nrChannels;
  texture_raw *tex_raw =
      HasRawTexture(gl_state, own_std::string((char *)info->file_name));
  ASSERT(tex_raw);
  unsigned char *src = nullptr;
  src = tex_raw->data;
  width = tex_raw->width;
  height = tex_raw->height;
  nrChannels = tex_raw->channels;

  auto texs_id = (int *)&dbg->mem_buffer[(long long)info->cinfo->texs_idxs];

  for (int cur_sp = 0; cur_sp < info->total_sps; cur_sp++) {

    texs_id[cur_sp] = GenTexture(dbg->lang_stat, gl_state, src, info->sp_width,
                                 info->sp_height, info->x_offset,
                                 info->y_offset, width, height, cur_sp);
  }
  /*
  func_decl* call_f = FuncAddedWasmInterp(dbg->wasm_state, "heap_alloc");

  block_linked* cur = NewBlock(nullptr);
  WasmDoCallInstruction(dbg, dbg->cur_bc, &cur, call_f);
  FreeBlock(cur);
  int addr = *(int*)GetRegValPtr(thread_id, dbg, RET_1_REG);
  //dbg->wasm_state->funcs
  int a = 0;
  */
}

int CompileShader(char *source, int type) {
  int success;
  char infoLog[512];
  unsigned int shader;
  shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  // ASSERT(gl_)

  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
    ASSERT(false)
  }
  return shader;
}
void UpdateLastTime(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  if (gl_state)
    gl_state->last_time = glfwGetTime();
}

void SetIsEngine(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  bool val = *(bool *)&dbg->mem_buffer[base_ptr + 8];
  auto gl_state = (open_gl_state *)dbg->data;
  gl_state->lang_stat->is_engine = val;
  gl_state->is_engine = val;

#ifdef LINUX
#else
  CreateThread(nullptr, 0, GameAndEngineMsgThread, (LPVOID)dbg, 0, nullptr);
#endif
  if (val) {
    gl_state->scene_srceen_width = 800;
    gl_state->scene_srceen_height = 480;
  }
}
// Shader compilation helper
GLuint compileShader(GLenum type, const char *source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info[512];
    printf("shader: %s", source);
    glGetShaderInfoLog(shader, 512, NULL, info);
    std::cerr << "Shader error:\n" << info << std::endl;
    ASSERT(false)
  }
  return shader;
}

// Matrix utility (replace with glm in serious projects)

void perspective(float *mat, float fov, float aspect, float near, float far) {
  float tanHalfFov = tanf(fov / 2);
  std::fill(mat, mat + 16, 0.0f);
  mat[0] = 1 / (aspect * tanHalfFov);
  mat[5] = 1 / tanHalfFov;
  mat[10] = -(far + near) / (far - near);
  mat[11] = -1;
  mat[14] = -(2 * far * near) / (far - near);
}

void ValidateTextureSlot(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int shader_id = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int name_len = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int slot = *(int *)&dbg->mem_buffer[base_ptr + 32];

  auto name_str = (char *)&dbg->mem_buffer[name_offset];

  auto ret = GetRegValPtr(thread_id, dbg, RET_1_REG);

  char prev_char = name_str[name_len];
  name_str[name_len] = 0;

  glUseProgram(shader_id);
  auto error = glGetError();
  *ret = glGetUniformLocation(shader_id, name_str);
  glUniform1i(*ret, slot);

  /*
  int count;
  glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &count);
  printf("Active Uniforms: %d\n", count);

  char buffer[64];
  int len;
  int size;
  int type;
  for (int i = 0; i < count; i++)
  {
          glGetActiveUniform(shader_id, (GLuint)i, 64, &len, &size, (GLenum
  *)&type, buffer);

          printf("Uniform #%d Type: %u Name: %s\n", i, type, buffer);
  }
          */

  name_str[name_len] = prev_char;
}
void _GetUniformLocation(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int shader_id = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int name_len = *(int *)&dbg->mem_buffer[base_ptr + 24];

  auto name_str = (char *)&dbg->mem_buffer[name_offset];

  auto ret = GetRegValPtr(thread_id, dbg, RET_1_REG);

  char prev_char = name_str[name_len];
  name_str[name_len] = 0;

  glUseProgram(shader_id);
  auto error = glGetError();
  *ret = glGetUniformLocation(shader_id, name_str);

  if (*ret == -1) {
    printf("uniform not found %s\n", name_str);
    int count;
    glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &count);
    printf("Active Uniforms: %d\n", count);

    char buffer[64];
    int len;
    int size;
    int type;
    for (int i = 0; i < count; i++) {
      glGetActiveUniform(shader_id, (GLuint)i, 64, &len, &size, (GLenum *)&type,
                         buffer);

      printf("Uniform #%d Type: %u Name: %s\n", i, type, buffer);
    }
  }

  name_str[name_len] = prev_char;
}
void SetSampler2D(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int uid = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto tex_id = *(int *)&dbg->mem_buffer[base_ptr + 16];

  auto gl_state = (open_gl_state *)dbg->data;
  texture_info *t = &gl_state->textures[tex_id];

  // printf("1d %d, 2d %d\n", tex_id, t->id);
  glActiveTexture(GL_TEXTURE0 + uid);
  glBindTexture(GL_TEXTURE_2D, t->id);
  glActiveTexture(GL_TEXTURE0);
  // glUniform4f(uid, v->x, v->y, v->z, v->w);
}
void SetUniformMatrices4x4(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int uid = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int count = *(int *)&dbg->mem_buffer[base_ptr + 16];
  auto data_offset = *(int *)&dbg->mem_buffer[base_ptr + 24];
  auto data = (float *)&dbg->mem_buffer[data_offset];
  glUniformMatrix4fv(uid, count, true, data);
}
void SetUniform1f(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int uid = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto x = *(float *)&dbg->mem_buffer[base_ptr + 16];

  glUniform1f(uid, x);
}
void SetUniform2f(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int uid = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto x = *(float *)&dbg->mem_buffer[base_ptr + 16];
  auto y = *(float *)&dbg->mem_buffer[base_ptr + 24];

  glUniform2f(uid, x, y);
}
void SetUniform3f(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int uid = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto x = *(float *)&dbg->mem_buffer[base_ptr + 16];
  auto y = *(float *)&dbg->mem_buffer[base_ptr + 24];
  auto z = *(float *)&dbg->mem_buffer[base_ptr + 32];

  glUniform3f(uid, x, y, z);
}
void SetUniform4f(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int uid = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto x = *(float *)&dbg->mem_buffer[base_ptr + 16];
  auto y = *(float *)&dbg->mem_buffer[base_ptr + 24];
  auto z = *(float *)&dbg->mem_buffer[base_ptr + 32];
  auto w = *(float *)&dbg->mem_buffer[base_ptr + 40];

  glUniform4f(uid, x, y, z, w);
}
void SetShader(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int shader_id = *(int *)&dbg->mem_buffer[base_ptr + 8];

  glUseProgram(shader_id);
}
void CompileShader2(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int vs_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int vs_len = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int fs_offset = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int fs_len = *(int *)&dbg->mem_buffer[base_ptr + 32];

  auto vs_str = (char *)&dbg->mem_buffer[vs_offset];
  auto fs_str = (char *)&dbg->mem_buffer[fs_offset];

  char prev_char = vs_str[vs_len];
  vs_str[vs_len] = 0;
  // printf("compiling vertex shader:\n%s\n", vs_str);
  GLuint vs = compileShader(GL_VERTEX_SHADER, vs_str);
  vs_str[vs_len] = prev_char;

  prev_char = fs_str[fs_len];
  fs_str[fs_len] = 0;
  // printf("compiling fragment shader:\n%s\n", fs_str);
  GLuint fs = compileShader(GL_FRAGMENT_SHADER, fs_str);
  fs_str[fs_len] = prev_char;

  int shaderProgram = glCreateProgram();
  GL_CALL(glAttachShader(shaderProgram, vs))
  GL_CALL(glAttachShader(shaderProgram, fs))
  GL_CALL(glLinkProgram(shaderProgram))

  auto ret = GetRegValPtr(thread_id, dbg, RET_1_REG);
  *ret = shaderProgram;
}
void Init3D(dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;

  char *vertexShaderSrc = "\n\
	#version 330 core\n\
	layout (location = 0) in vec3 aPos;\n\
	layout(location = 1) in vec2 aTexCoord;\n\
	out vec4 vColor;\n\
	out vec3 fragPos;\n\
	out vec2 TexCoord;\n\
	uniform mat4 model;\n\
	uniform vec4 rot;\n\
	uniform vec4 cam_pos;\n\
	uniform mat4 view;\n\
	uniform mat4 projection;\n\
	vec3 rotate_by_quaternion(vec3 v, vec4 q) {\n\
		// Extract quaternion components\n\
		float w = q.w;\n\
		vec3 u = q.xyz;\n\
		// Apply rotation: v' = v + 2.0 * cross(u, cross(u, v) + w * v)\n\
		return v + 2.0 * cross(u, cross(u, v) + w * v);\n\
	}\n\
	void main() {\n\
		vec4 aux = model * vec4(aPos, 1.0);\n\
		aux += cam_pos;\n\
		fragPos = aux.xyz;\n\
		gl_Position = projection * view * aux;\n\
		vColor = vec4(1.0, 1.0, 1.0, 1.0);\
		TexCoord = aTexCoord;\n\
	}\
	";

  char *terrainVertexShaderSrc = "\n\
	#version 330 core\n\
	layout (location = 0) in vec3 aPos;\n\
	layout (location = 1) in vec3 vertexCol;\n\
	out vec4 vColor;\n\
	out vec3 fragPos;\n\
	uniform mat4 model;\n\
	uniform vec4 rot;\n\
	uniform mat4 view;\n\
	uniform mat4 projection;\n\
	uniform mat4 time;\n\
	uniform vec4 cam_pos;\n\
	vec3 rotate_by_quaternion(vec3 v, vec4 q) {\n\
		// Extract quaternion components\n\
		float w = q.w;\n\
		vec3 u = q.xyz;\n\
		// Apply rotation: v' = v + 2.0 * cross(u, cross(u, v) + w * v)\n\
		return v + 2.0 * cross(u, cross(u, v) + w * v);\n\
	}\n\
	void main() {\n\
		float t = time[0][0];\n\
		vec4 aux = model * vec4(aPos, 1.0);\n\
		aux += cam_pos;\n\
		fragPos = aux.xyz;\n\
		gl_Position = projection * view * aux;\n\
		vColor = vec4(vertexCol.xyz, 1.0);\n\
	}\n\
	";

  // Fragment Shader
  char *fragmentShaderSrcTexNoLight = "\n\
	#version 330 core\n\
	out vec4 FragColor;\n\
	in vec2 TexCoord;\n\
	in vec4 vColor;\n\
	in vec3 fragPos;\n\
	uniform vec4 col;\n\
	uniform vec2 tex_size;\n\
	uniform vec2 tex_offset;\n\
	uniform sampler2D tex;\n\
	uniform vec4 sec_color;\n\
	uniform float color_lerp;\n\
	void main() {\n\
		vec4 tex_col =  texture(tex, TexCoord);\n\
		if(tex_col.a == 0.0){\n\
			discard;\n\
		}\n\
		FragColor = col * vColor * tex_col;\n\
		FragColor = (1.0 - color_lerp) * FragColor + color_lerp * sec_color;\n\
	}\
	";
  char *fragmentShaderSrcTex = "\n\
	#version 330 core\n\
	out vec4 FragColor;\n\
	in vec2 TexCoord;\n\
	in vec4 vColor;\n\
	in vec3 fragPos;\n\
	uniform vec4 col;\n\
	uniform vec4 sec_color;\n\
	uniform float color_lerp;\n\
	uniform vec2 tex_size;\n\
	uniform vec2 tex_offset;\n\
	uniform vec3 sun_dir;\n\
	uniform vec4 sun_color;\n\
	uniform sampler2D tex;\n\
	void main() {\n\
		vec4 tex_col =  texture(tex, TexCoord);\n\
		vec3 dx = dFdx(fragPos);\n\
		vec3 dy = dFdy(fragPos);\n\
		vec3 norm = normalize(cross(dx, dy));\n\
		vec3 lightDir = normalize(vec3(-0.5, 0.5, 0.0));\n\
		float d = max(dot(sun_dir, norm), 0.2);\n\
		FragColor = vec4(vec3(1.0, 1.0, 1.0) * d, 1.0);\n\
		FragColor *= col * vColor * tex_col * sun_color;\n\
		FragColor = (1.0 - color_lerp) * FragColor + color_lerp * sec_color;\n\
	}\
	";
  char *fragmentShaderSrc = "\n\
	#version 330 core\n\
	out vec4 FragColor;\n\
	in vec4 vColor;\n\
	in vec3 fragPos;\n\
	uniform vec4 col;\n\
	void main() {\n\
		FragColor = col;\n\
	}\
	";

  // Cube vertices with UVs (each face gets its own 4 vertices)
  GLfloat planeVerts[] = {
      // Front face
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
  };
  GLfloat cubeVerts[] = {
      // Front face
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f,
      0.5f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,

      // Back face
      -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,
      0.5f, -0.5f, 0.0f, 1.0f, -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,

      // Left face
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, -0.5f,
      0.5f, 0.5f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,

      // Right face
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f,
      0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f,

      // Top face
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 0.5f,
      0.5f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,

      // Bottom face
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,
      -0.5f, 0.5f, 1.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 1.0f};

  GLuint indicesPlane[] = {
      0, 2, 1, 2, 0, 3, // Front face
      0, 1, 2, 2, 3, 0, // Front face
  };
  GLuint indices[] = {
      0,  2,  1,  2,  0,  3,  // Front face
      4,  5,  6,  6,  7,  4,  // Back face
      8,  10, 9,  10, 8,  11, // Left face
      12, 13, 14, 14, 15, 12, // Right face
      16, 17, 18, 18, 19, 16, // Top face
      20, 22, 21, 22, 20, 23  // Bottom face
  };

  // Compile shaders
  GLuint terrain_vs = compileShader(GL_VERTEX_SHADER, terrainVertexShaderSrc);
  GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
  GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
  GLuint fs_tex_no_light =
      compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrcTexNoLight);
  GLuint fs_tex = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrcTex);
  GLuint shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);
  gl_state->shader_program3d = shaderProgram;

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs_tex);
  glLinkProgram(shaderProgram);
  gl_state->shader_program3d_tex = shaderProgram;

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs_tex_no_light);
  glLinkProgram(shaderProgram);
  gl_state->shader_program3d_tex_no_light = shaderProgram;

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, terrain_vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);
  gl_state->terrain_shader_program3d = shaderProgram;

  char *otherVertexShaderSrc = "\n\
	#version 330 core\n\
	layout (location = 0) in vec3 aPos;\n\
	out vec4 vColor;\n\
	uniform mat4 model;\n\
	uniform vec4 rot;\n\
	uniform mat4 view;\n\
	uniform mat4 projection;\n\
	uniform mat4 time;\n\
	void main() {\n\
		float t = time[0][0];\n\
		vec4 aux = view * vec4(aPos, 1.0);\n\
		gl_Position = projection * aux;\n\
		vColor = vec4(1.0, 1.0, 1.0, 1.0);\
	}\
	";

  vs = compileShader(GL_VERTEX_SHADER, otherVertexShaderSrc);
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);
  gl_state->shader_program3d_line = shaderProgram;

  otherVertexShaderSrc = "\n\
	#version 330 core\n\
	layout (location = 0) in vec3 aPos;\n\
	out vec4 vColor;\n\
	uniform mat4 model;\n\
	uniform vec4 rot;\n\
	uniform mat4 view;\n\
	uniform mat4 projection;\n\
	uniform mat4 time;\n\
	void main() {\n\
		float t = time[0][0];\n\
		vec4 aux = view * vec4(aPos, 1.0);\n\
		gl_Position = aux;\n\
		vColor = vec4(1.0, 1.0, 1.0, 1.0);\
	}\
	";

  vs = compileShader(GL_VERTEX_SHADER, otherVertexShaderSrc);
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);
  gl_state->shader_program3d_line_no_proj = shaderProgram;

  otherVertexShaderSrc = "\n\
	#version 330 core\n\
	layout (location = 0) in vec3 aPos;\n\
	layout (location = 1) in vec3 normal;\n\
	out vec4 vColor;\n\
	uniform mat4 model;\n\
	uniform vec4 rot;\n\
	uniform mat4 view;\n\
	uniform mat4 projection;\n\
	uniform mat4 time;\n\
	void main() {\n\
		float t = time[0][0];\n\
		vec4 aux = view * vec4(aPos, 1.0);\n\
		gl_Position = projection * aux;\n\
		vColor = vec4(1.0, 1.0, 1.0, 1.0);\
	}\
	";
  vs = compileShader(GL_VERTEX_SHADER, otherVertexShaderSrc);
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);
  gl_state->shader_program3d_tri = shaderProgram;

  // Vertex Array & Buffers
  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Position attribute (location = 0)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // UV attribute (location = 1)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  gl_state->vao3d = VAO;
  model_info *cube_m = &gl_state->models[0];
  cube_m->name = "cube";
  cube_m->vao = VAO;
  cube_m->ebo = EBO;
  cube_m->vbo = VBO;
  cube_m->indicies = 36;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, cubeVerts,
               GL_DYNAMIC_DRAW); // Note: GL_DYNAMIC_DRAW
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
  gl_state->vao3d_line = VAO;
  gl_state->vbo3d_line = VBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), cubeVerts,
               GL_DYNAMIC_DRAW); // Note: GL_DYNAMIC_DRAW
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  // normal
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);
  gl_state->vao3d_tri = VAO;
  gl_state->vbo3d_tri = VBO;

  glEnable(GL_DEPTH_TEST);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), planeVerts, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPlane), indicesPlane,
               GL_STATIC_DRAW);

  // Position attribute (location = 0)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // UV attribute (location = 1)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  model_info *plane_m = &gl_state->models[1];
  plane_m->name = "plane";
  plane_m->vao = VAO;
  plane_m->ebo = EBO;
  plane_m->vbo = VBO;
  plane_m->indicies = sizeof(indicesPlane) / sizeof(float);

  loadIdentity(gl_state->model);
  loadIdentity(gl_state->view);
  loadIdentity(gl_state->projection);

  perspective(gl_state->projection, 70.0f * (3.14159f / 180.0f),
              (float)gl_state->width / (float)gl_state->height, 0.01f, 500.0f);
  gl_state->view[14] = -5.0f; // translate view back
  gl_state->view[13] = -1.0f; // translate view back
  // gl_state->model[13] = -1.0f;  // translate view back
  /*
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound
  texture object) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
  GL_REPEAT); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // load and generate the texture
  int width, height, nrChannels;
  */

  GLuint fbo, texture, depthBuffer;

  // Create and bind the framebuffer
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  gl_state->frame_buffer = fbo;

  // Create the texture to render to
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  gl_state->frame_buffer_tex = texture;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gl_state->width, gl_state->height, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Attach the texture to the framebuffer's color attachment
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture, 0);

  // Create and attach a depth buffer (optional, for 3D scenes)
  glGenRenderbuffers(1, &depthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gl_state->width,
                        gl_state->height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, depthBuffer);

  auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  // Check framebuffer completeness
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
    printf("Error: Framebuffer is not complete!\n");
    ASSERT(false);
  }

  // Unbind the framebuffer for now
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Set the list of draw buffers.
  // GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  // glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
}
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  auto gl_state = (open_gl_state *)glfwGetWindowUserPointer(window);
  double currentTime = glfwGetTime();

  double deltaTime = currentTime - gl_state->last_time;

  if (deltaTime > 0.0 && gl_state->mouse_last_x != 0.0) {
    gl_state->mouse_vel_x = (xpos - gl_state->mouse_last_x);
    gl_state->mouse_vel_y = (ypos - gl_state->mouse_last_y);
  }
  auto prev_last_y = gl_state->mouse_last_y;
  // printf("cury %.3f, lasty %.3f, vely %.3f\n", ypos,
  // gl_state->mouse_last_y, gl_state->mouse_vel_y); glfwSetCursorPos(window,
  // 0, 0);
  gl_state->mouse_last_x = xpos;
  gl_state->mouse_last_y = ypos;
  // glfwSetCursorPos(window, gl_state->width / 2, gl_state->height / 2);
}
#ifdef LINUX
void hide_cursor_x11(GLFWwindow *glfwWindow) {
  // Get X11 display and window from GLFW
  Display *display = glfwGetX11Display();
  Window window = glfwGetX11Window(glfwWindow);

  // Method 2: Using XFixes extension (more reliable)
  int event_base, error_base;
  if (XFixesQueryExtension(display, &event_base, &error_base)) {
    XFixesHideCursor(display, window);
    XFlush(display);
  }
}

void show_cursor_x11(GLFWwindow *glfwWindow) {
  Display *display = glfwGetX11Display();
  Window window = glfwGetX11Window(glfwWindow);

  // Method 2: Using XFixes
  int event_base, error_base;
  if (XFixesQueryExtension(display, &event_base, &error_base)) {
    XFixesShowCursor(display, window);
    XFlush(display);
  }
}
#endif

void HideCursor(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  bool val = *(bool *)&dbg->mem_buffer[base_ptr + 8];

  if (val) {
#ifdef AOEOAE
    hide_cursor_x11((GLFWwindow *)gl_state->glfw_window);
#else
    glfwSetInputMode((GLFWwindow *)gl_state->glfw_window, GLFW_CURSOR,
                     GLFW_CURSOR_DISABLED);
#endif
  } else {
#ifdef AOEAOE
    show_cursor_x11((GLFWwindow *)gl_state->glfw_window);
#else
    glfwSetInputMode((GLFWwindow *)gl_state->glfw_window, GLFW_CURSOR,
                     GLFW_CURSOR_NORMAL);
#endif
  }
}
void SetCulling(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int type = *(int *)&dbg->mem_buffer[base_ptr + 8];

  switch (type) {
  case CULLING_FRONT_BACK: {
    glCullFace(GL_FRONT_AND_BACK);
  } break;
  case CULLING_FRONT: {
    glCullFace(GL_FRONT);
  } break;
  case CULLING_BACK: {
    glCullFace(GL_BACK);
  } break;
  }
}
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam) {
  // Ignore non-significant errors/warnings
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  const char *src_str = [&] {
    switch (source) {
    case GL_DEBUG_SOURCE_API:
      return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "Window System";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "Shader Compiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "Third Party";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "Application";
    default:
      return "Other";
    }
  }();

  const char *type_str = [&] {
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      return "Error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "Deprecated";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "Undefined Behavior";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "Portability";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "Performance";
    case GL_DEBUG_TYPE_MARKER:
      return "Marker";
    default:
      return "Other";
    }
  }();

  const char *severity_str = [&] {
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      return "High";
    case GL_DEBUG_SEVERITY_MEDIUM:
      return "Medium";
    case GL_DEBUG_SEVERITY_LOW:
      return "Low";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      return "Notification";
    default:
      return "";
    }
  }();

  fprintf(stderr, "[OpenGL DEBUG] %s: %s (%d) - %s\n", type_str, severity_str,
          id, message);

  if (severity == GL_DEBUG_SEVERITY_HIGH) {
// Breakpoint or abort for critical errors
#ifdef _MSC_VER
    __debugbreak();
#else
    raise(SIGTRAP);
#endif
  }
}
void enableGLDebugging() {
  GLint flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
  }
}
static int audio_callback(const void *input, void *output,
                          unsigned long frameCount,
                          const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags, void *userData) {
  short *out = (short *)output;
  auto gl_state = (open_gl_state *)userData;
  auto sound = gl_state->sound;

  // 1. Clear output buffer (safe memset)
  memset(out, 0, frameCount * sizeof(short) * 2); // Stereo = 2 channels

  return;
  // 2. Mix audio clips
  int active_clips = 0;
  FOR_VEC(it, sound->audio_clips_to_play) {
    if (!it->clip || it->clip->buffer.empty())
      continue;
    active_clips++;
  }

  float mul = 1.0f;

  int i = 0;
  FOR_VEC(it, sound->audio_clips_to_play) {
    AudioClip *clip_ptr = it->clip;
    /**/
    if (!clip_ptr || clip_ptr->buffer.empty()) {
      // sound->audio_clips_to_play.remove(i);
      continue;
    }

    short *src_buffer = clip_ptr->buffer.data();
    const size_t buffer_size = clip_ptr->buffer.size();

    for (int s = 0; s < frameCount; s++) {
      if ((it->cur_idx + 1) >= buffer_size)
        break;

      out[s * 2] += src_buffer[it->cur_idx] * it->volume * mul;
      out[s * 2 + 1] += src_buffer[it->cur_idx + 1] * it->volume * mul;

      it->cur_idx += static_cast<int>(2 * it->speed);
    }

    /*
    if (it->cur_idx + 1 >= buffer_size) {
            sound->audio_clips_to_play.remove(i);
    } else {
            i++;
    }
            */
  }
  out = (short *)output;
  // HERE()
  static float b0, b1, b2, filtered = 0;
  float cutoffFreq = 50.0f; // Adjust to taste (lower = darker)
  float rc = 1.0f / (2.0f * M_PI * cutoffFreq);
  float dt = 1.0f / 44100.0f; // Sample rate
  float alpha = dt / (rc + dt);

  /*
  for (int i = 0; i < frameCount; i++)
  {
          // Generate white noise
          float white = (rand() / (float)RAND_MAX) * 2.0f - 1.0f;

          // Apply pink noise filter (Paul Kellet's method)
          b0 = 0.99886f * b0 + white * 0.0555179f;
          b1 = 0.99332f * b1 + white * 0.0750759f;
          b2 = 0.96900f * b2 + white * 0.1538520f;
          float pink = b0 + b1 + b2 + white * 0.5362f;
          pink *= 0.11f; // Compensation gain
          filtered = filtered + alpha * (pink - filtered);


          *out++ += filtered * 32767 * 0.5; // Left
          *out++ += filtered * 32767 * 0.5; // Right
  }
          */
  i = 0;
  FOR_VEC(it, sound->audio_clips_to_play) {
    AudioClip *clip_ptr = it->clip;
    if (it->cur_idx >= clip_ptr->buffer.size()) {
      sound->audio_clips_to_play.remove(i);
    }
    i++;
  }

  return paContinue; // Return `paComplete` to stop
}
#ifdef RENDERER_VULKAN
void createSemaphores(VkDevice device, VkSemaphore *imageAvailable,
                      VkSemaphore *renderFinished) {
  VkSemaphoreCreateInfo semaphoreInfo = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };

  if (vkCreateSemaphore(device, &semaphoreInfo, NULL, imageAvailable) !=
          VK_SUCCESS ||
      vkCreateSemaphore(device, &semaphoreInfo, NULL, renderFinished) !=
          VK_SUCCESS) {
    fprintf(stderr, "Failed to create semaphores!\n");
    exit(1);
  }
}
// Begins command buffer recording with given usage flags
void beginCommandBuffer(VkCommandBuffer cmdBuf,
                        VkCommandBufferUsageFlags usageFlags) {
  VkCommandBufferBeginInfo beginInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = usageFlags,
      .pInheritanceInfo = NULL // not needed for primary command buffers
  };

  VkResult result = vkBeginCommandBuffer(cmdBuf, &beginInfo);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to begin recording command buffer! (error %d)\n",
            result);
    exit(EXIT_FAILURE);
  }
}
void recordCommadBuffer(VkCommandBuffer cmd, VkImage img) {
  VkClearColorValue clear_color = {1.0, 0.0, 0.0, 0.0};
  VkImageSubresourceRange range = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                   .baseMipLevel = 0,
                                   .levelCount = 1,
                                   .baseArrayLayer = 0,
                                   .layerCount = 1};

  beginCommandBuffer(cmd, 0);
  vkCmdClearColorImage(cmd, img, VK_IMAGE_LAYOUT_GENERAL, &clear_color, 1,
                       &range);
  vkEndCommandBuffer(cmd);
}
VkCommandPool createCommandPool(VkDevice device, uint32_t queueFamilyIndex) {
  VkCommandPoolCreateInfo poolInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .queueFamilyIndex = queueFamilyIndex,
      .flags =
          VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT // allow re-recording
  };

  VkCommandPool commandPool;
  VkResult result = vkCreateCommandPool(device, &poolInfo, NULL, &commandPool);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to create command pool! (error %d)\n", result);
    exit(EXIT_FAILURE);
  }

  printf("Command pool created (family %u).\n", queueFamilyIndex);
  return commandPool;
}

// Allocates `count` primary command buffers from a pool
void allocateCommandBuffers(VkDevice device, VkCommandPool commandPool,
                            uint32_t count, VkCommandBuffer *buffers) {
  VkCommandBufferAllocateInfo allocInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = commandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = count,
  };

  VkResult result = vkAllocateCommandBuffers(device, &allocInfo, buffers);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to allocate command buffers! (error %d)\n", result);
    exit(EXIT_FAILURE);
  }

  printf("%u command buffer(s) allocated.\n", count);
}

// Helper: choose surface format
VkSurfaceFormatKHR
chooseSurfaceFormat(const VkSurfaceFormatKHR *availableFormats,
                    uint32_t count) {
  for (uint32_t i = 0; i < count; i++) {
    if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormats[i];
    }
  }
  return availableFormats[0];
}

// Helper: choose present mode
VkPresentModeKHR choosePresentMode(const VkPresentModeKHR *availableModes,
                                   uint32_t count) {
  for (uint32_t i = 0; i < count; i++) {
    if (availableModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availableModes[i]; // low-latency triple buffering
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR; // always supported
}

// Helper: choose swap extent (window size)
VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR *caps,
                        GLFWwindow *window) {
  if (caps->currentExtent.width != UINT32_MAX)
    return caps->currentExtent;

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  VkExtent2D actual = {.width = (uint32_t)width, .height = (uint32_t)height};

  if (actual.width < caps->minImageExtent.width)
    actual.width = caps->minImageExtent.width;
  if (actual.width > caps->maxImageExtent.width)
    actual.width = caps->maxImageExtent.width;
  if (actual.height < caps->minImageExtent.height)
    actual.height = caps->minImageExtent.height;
  if (actual.height > caps->maxImageExtent.height)
    actual.height = caps->maxImageExtent.height;

  return actual;
}

// Main function to create swapchain
SwapchainData createSwapchain(VkPhysicalDevice physicalDevice, VkDevice device,
                              VkSurfaceKHR surface, int graphicsFamily,
                              int presentFamily, GLFWwindow *window) {
  SwapchainData sc = {0};

  // Query surface capabilities
  VkSurfaceCapabilitiesKHR caps;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                       NULL);
  VkSurfaceFormatKHR *formats =
      (VkSurfaceFormatKHR *)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                       formats);

  uint32_t modeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount,
                                            NULL);
  VkPresentModeKHR *modes =
      (VkPresentModeKHR *)malloc(sizeof(VkPresentModeKHR) * modeCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount,
                                            modes);

  VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(formats, formatCount);
  VkPresentModeKHR presentMode = choosePresentMode(modes, modeCount);
  VkExtent2D extent = chooseExtent(&caps, window);

  free(formats);
  free(modes);

  uint32_t imageCount = caps.minImageCount + 1;
  if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
    imageCount = caps.maxImageCount;

  uint32_t queueFamilyIndices[] = {(uint32_t)graphicsFamily,
                                   (uint32_t)presentFamily};
  VkSharingMode sharingMode = (graphicsFamily != presentFamily)
                                  ? VK_SHARING_MODE_CONCURRENT
                                  : VK_SHARING_MODE_EXCLUSIVE;

  VkSwapchainCreateInfoKHR createInfo = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface,
      .minImageCount = imageCount,
      .imageFormat = surfaceFormat.format,
      .imageColorSpace = surfaceFormat.colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = sharingMode,
      .queueFamilyIndexCount = (graphicsFamily != presentFamily) ? 2 : 0,
      .pQueueFamilyIndices =
          (graphicsFamily != presentFamily) ? queueFamilyIndices : NULL,
      .preTransform = caps.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = presentMode,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE,
  };

  if (vkCreateSwapchainKHR(device, &createInfo, NULL, &sc.swapchain) !=
      VK_SUCCESS) {
    fprintf(stderr, "Failed to create swapchain!\n");
    exit(EXIT_FAILURE);
  }

  vkGetSwapchainImagesKHR(device, sc.swapchain, &imageCount, NULL);
  sc.images = (VkImage *)malloc(sizeof(VkImage) * imageCount);
  vkGetSwapchainImagesKHR(device, sc.swapchain, &imageCount, sc.images);
  sc.imageCount = imageCount;
  sc.imageFormat = surfaceFormat.format;
  sc.extent = extent;

  // Create image views
  sc.imageViews = (VkImageView *)malloc(sizeof(VkImageView) * imageCount);
  for (uint32_t i = 0; i < imageCount; i++) {
    VkImageViewCreateInfo viewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = sc.images[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = sc.imageFormat,
        .components = {VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .baseMipLevel = 0,
                             .levelCount = 1,
                             .baseArrayLayer = 0,
                             .layerCount = 1}};
    if (vkCreateImageView(device, &viewInfo, NULL, &sc.imageViews[i]) !=
        VK_SUCCESS) {
      fprintf(stderr, "Failed to create image view!\n");
      exit(EXIT_FAILURE);
    }
  }

  printf("Swapchain created with %u images.\n", sc.imageCount);
  return sc;
}
VKAPI_ATTR VkBool32 VKAPI_CALL
vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {
  (void)messageType;
  (void)pUserData;

  const char *severity =
      (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
          ? "ERROR"
      : (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
          ? "WARNING"
      : (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
          ? "INFO"
          : "VERBOSE";

  fprintf(stderr, "[%s] %s\n", severity, pCallbackData->pMessage);
  return VK_FALSE;
}
void pickPhysicalDevice(VkInstance instance, VkPhysicalDevice *outDevice) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

  if (deviceCount == 0) {
    fprintf(stderr, "Failed: No GPUs with Vulkan support found.\n");
    exit(EXIT_FAILURE);
  }

  VkPhysicalDevice *devices =
      (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * deviceCount);

  vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

  printf("Found %u Vulkan-capable device(s):\n", deviceCount);

  for (uint32_t i = 0; i < deviceCount; i++) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(devices[i], &props);

    printf("  [%u] %s (API Version %u.%u.%u)\n", i, props.deviceName,
           VK_API_VERSION_MAJOR(props.apiVersion),
           VK_API_VERSION_MINOR(props.apiVersion),
           VK_API_VERSION_PATCH(props.apiVersion));

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      *outDevice = devices[i];
      printf("Selected discrete GPU: %s\n", props.deviceName);
      free(devices);
      return;
    }
  }

  // Pick the first device (you can choose more selectively)
  *outDevice = devices[0];

  free(devices);
}
int findGraphicsQueueFamily(VkPhysicalDevice device) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

  VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties *)malloc(
      sizeof(VkQueueFamilyProperties) * queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies);

  int graphicsFamily = -1;

  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphicsFamily = i;
      break;
    }
  }

  free(queueFamilies);
  return graphicsFamily;
}

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice,
                             int graphicsFamily, VkQueue *outQueue) {
  float queuePriority = 1.0f;
  VkDeviceQueueCreateInfo queueCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = (uint32_t)graphicsFamily,
      .queueCount = 1,
      .pQueuePriorities = &queuePriority,
  };

  const char *deviceExtensions[] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME, // needed for rendering to screen
      VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME};

  VkPhysicalDeviceFeatures deviceFeatures = {0};

  VkDeviceCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pQueueCreateInfos = &queueCreateInfo,
      .queueCreateInfoCount = 1,
      .pEnabledFeatures = &deviceFeatures,
      .enabledExtensionCount = 1,
      .ppEnabledExtensionNames = deviceExtensions,
  };

  VkDevice device;
  VkResult result = vkCreateDevice(physicalDevice, &createInfo, NULL, &device);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to create logical device (error %d)\n", result);
    exit(EXIT_FAILURE);
  }

  // Retrieve queue handle
  vkGetDeviceQueue(device, (uint32_t)graphicsFamily, 0, outQueue);
  return device;
}
int findPresentQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

  VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties *)malloc(
      sizeof(VkQueueFamilyProperties) * queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies);

  int presentFamily = -1;

  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    VkBool32 presentSupport = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

    if (presentSupport) {
      presentFamily = (int)i;
      break;
    }
  }

  free(queueFamilies);
  return presentFamily;
}
void InitVulkan(dbg_state *dbg, GLFWwindow *window) {
  auto gl_state = (open_gl_state *)dbg->data;
  vulkan_state *vk_state = &gl_state->vk_state;
  VkAllocationCallbacks *allocator = VK_NULL_HANDLE;

  own_std::vector<const char *> layers;
  layers.emplace_back("VK_LAYER_KHRONOS_validation");

  own_std::vector<const char *> extensions;
  extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
  extensions.emplace_back("VK_KHR_wayland_surface");
  extensions.emplace_back("VK_KHR_xcb_surface");
  extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

  VkResult result;

  // 1. Create Vulkan instance
  VkApplicationInfo appInfo = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "Minimal Vulkan App",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_0,
  };

  VkInstanceCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .enabledExtensionCount = extensions.size(),
      .ppEnabledExtensionNames = extensions.data(),
      .enabledLayerCount = layers.size(),
      .ppEnabledLayerNames = layers.data(),
  };

  // Create debug messenger info (so we can reuse it later)
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = vkDebugCallback,
      .pUserData = NULL};

  result = vkCreateInstance(&createInfo, allocator, &vk_state->instance);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to create Vulkan instance! Error code: %d\n",
            result);
    return EXIT_FAILURE;
  }
  printf("Vulkan: instance created!\n");

  PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          vk_state->instance, "vkCreateDebugUtilsMessengerEXT");
  if (func == NULL) {
    fprintf(stderr, "couldnt find vkCreateDebugUtilsMessengerEXT");
    ASSERT(false)
  }
  result = func(vk_state->instance, &debugCreateInfo, allocator,
                &vk_state->debug_messenger);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to create Vulkan debug! Error code: %d\n", result);
    ASSERT(false)
    return EXIT_FAILURE;
  }
  printf("Vulkan: debug created!\n");

  if (glfwCreateWindowSurface(vk_state->instance, window,
                              (const VkAllocationCallbacks *)allocator,
                              &vk_state->surface)) {
    fprintf(stderr, "Failed to create surface with glfw!");
    ASSERT(false)
  }

  printf("Vulkan: surface created!\n");

  pickPhysicalDevice(vk_state->instance, &vk_state->physical_device);

  printf("Vulkan: physical device picked!\n");

  int graphicsFamily = findGraphicsQueueFamily(vk_state->physical_device);
  if (graphicsFamily == -1) {
    fprintf(stderr, "Failed to find graphics queue family!\n");
    ASSERT(false)
    return EXIT_FAILURE;
  }

  vk_state->device = createLogicalDevice(
      vk_state->physical_device, graphicsFamily, &vk_state->grphics_queue);
  printf("Vulkan: Logical device and graphics queue created successfully!\n");

  auto presentFamily =
      findPresentQueueFamily(vk_state->physical_device, vk_state->surface);

  vk_state->swap_chain =
      createSwapchain(vk_state->physical_device, vk_state->device,
                      vk_state->surface, graphicsFamily, presentFamily, window);
  printf("Vulkan: swapchain created successfully!\n");

  vk_state->cmd_pool = createCommandPool(vk_state->device, graphicsFamily);

  // Allocate N command buffers
  uint32_t cmdCount = vk_state->swap_chain.imageCount;
  vk_state->cmd_buffers.reserve(cmdCount);
  allocateCommandBuffers(vk_state->device, vk_state->cmd_pool, cmdCount,
                         vk_state->cmd_buffers.data());

  createSemaphores(vk_state->device, &vk_state->render_complete,
                   &vk_state->present_complete);

  VkPipelineStageFlags wait_flags =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  while (!glfwWindowShouldClose(window)) {
    uint32_t imageIndex;
    vkAcquireNextImageKHR(vk_state->device, vk_state->swap_chain.swapchain,
                          UINT64_MAX, vk_state->present_complete,
                          VK_NULL_HANDLE, &imageIndex);
    recordCommadBuffer(vk_state->cmd_buffers[imageIndex],
                       vk_state->swap_chain.images[imageIndex]);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &vk_state->present_complete,
        .pWaitDstStageMask = &wait_flags,
        .commandBufferCount = 1,
        .pCommandBuffers = &vk_state->cmd_buffers[imageIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &vk_state->render_complete,
    };

    if (vkQueueSubmit(vk_state->grphics_queue, 1, &submitInfo,
                      VK_NULL_HANDLE) != VK_SUCCESS) {
      fprintf(stderr, "Failed to submit draw command buffer!\n");
    }
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &vk_state->render_complete,
        .swapchainCount = 1,
        .pSwapchains = &vk_state->swap_chain.swapchain,
        .pImageIndices = &imageIndex,
    };

    vkQueuePresentKHR(vk_state->grphics_queue, &presentInfo);
  }
}
#endif
void OpenWindow(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int wnd_width = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int wnd_height = *(int *)&dbg->mem_buffer[base_ptr + 16];
  // open_simplex_noise(77374, &dbg->simplex_ctx);

  auto gl_state = (open_gl_state *)dbg->data;
  gl_state->mouse_vel_x = 0.0;
  gl_state->mouse_vel_y = 0.0;

  for (int i = 0; i < 256; i++) {
    auto r = (std::rand() % 65536 - 32768);
    gl_state->sound->white_noise[i] = r;
  }
  if (!gl_state->is_engine) {
    gl_state->scene_srceen_width = wnd_width;
    gl_state->scene_srceen_height = wnd_height;
    // gl_state->width = wnd_width;
    // gl_state->height = wnd_height;
  }
  if (gl_state->glfw_window) {
    if (wnd_width == 0) {
      wnd_width = 100;
    }
    if (wnd_height == 0) {
      wnd_height = 100;
    }
    printf("will try to resize %p\n", gl_state->glfw_window);
    glfwSetWindowSize((GLFWwindow *)gl_state->glfw_window, wnd_width,
                      wnd_height);
    gl_state->width = wnd_width;
    gl_state->height = wnd_height;
    *(long long *)GetRegValPtr(thread_id, dbg, RET_1_REG) =
        (long long)gl_state->glfw_window;
    printf("resized");
    return;
  }

  if (!gl_state->pa_stream) {

    int err = Pa_Initialize();

    PaStreamParameters outputParams = {.device = Pa_GetDefaultOutputDevice(),
                                       .channelCount = 2, // Stereo
                                       .sampleFormat =
                                           paInt16, // 32-bit float samples
                                       .suggestedLatency = 0.05, // 50ms latency
                                       .hostApiSpecificStreamInfo = NULL};

    err = Pa_OpenStream(&gl_state->pa_stream,
                        NULL, // No input
                        &outputParams,
                        44100,          // Sample rate
                        256,            // Frames per buffer
                        paClipOff,      // No clipping
                        audio_callback, // Callback function
                        gl_state        // Passed to callback
    );
    if (err != paNoError) {
      fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
      Pa_Terminate();
      ASSERT(0)
      return;
    }
    Pa_StartStream(gl_state->pa_stream);
  }

  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return;

  wnd_width = 1000;
  wnd_height = 1000;
  gl_state->width = wnd_width;
  gl_state->height = wnd_height;
  /* Create a windowed mode window and its OpenGL context */
  const char *glsl_version = "#version 430";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  /*
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required
  on Mac
  */

  // glfwWindowHint(GLFW_REFRESH_RATE, 60);

  window = glfwCreateWindow(gl_state->width, gl_state->height, "Hello World",
                            NULL, NULL);
  InitVulkan(dbg, window);
  HERE()

  if (!window) {
    ASSERT(0);
    glfwTerminate();
    return;
  }
  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }
  gl_state->glfw_window = window;

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  glfwSetWindowUserPointer(window, (void *)gl_state);
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetWindowCloseCallback(window, window_close_callback);
  glfwSetMouseButtonCallback(window, MouseCallback);
  glfwSetCursorPosCallback(window, cursor_position_callback);

  *(long long *)GetRegValPtr(thread_id, dbg, RET_1_REG) = (long long)window;

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  io.ConfigNavEscapeClearFocusItem = false;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigWindowsMoveFromTitleBarOnly = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Single initialization call
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    // Handle error
  }
  // int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  enableGLDebugging();

  float vertices[] = {
      // positions          // texture coords
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top right
      1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };
  unsigned int indices[] = {
      // note that we start from 0!
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  unsigned int LINEVAO;
  unsigned int VBO;
  glGenVertexArrays(1, &LINEVAO);
  glBindVertexArray(LINEVAO);
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, 4 * 4, vertices, GL_DYNAMIC_DRAW);
  GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                                (void *)0));
  glEnableVertexAttribArray(0);

  gl_state->line_vbo = VBO;

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                (void *)0));
  GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                (void *)(3 * sizeof(float))));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  const char *lineVertexShaderSource =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "layout (location = 1) in vec2 uv;\n"
      "uniform vec3 pos;\n"
      "uniform vec3 pivot;\n"
      "uniform float cam_size;\n"
      "uniform float screen_ratio;\n"
      "uniform vec3 ent_size;\n"
      "uniform vec3 cam_pos;\n"
      "uniform vec3 cam_rot;\n"
      "uniform vec3 ent_rot;\n"
      "out vec2 TexCoord;\n"
      "void main()\n"
      "{\n"
      "	mat3 A = mat3(cos(cam_rot.z), -sin(cam_rot.z), 0.0,\n"
      "		 sin(cam_rot.z), cos(cam_rot.z), 0.0,\n"
      "		 0.0, 0.0, 1.0);\n"
      "	mat3 rot = mat3(cos(ent_rot.z), -sin(ent_rot.z), 0.0,\n"
      "		 sin(ent_rot.z), cos(ent_rot.z), 0.0,\n"
      "		 0.0, 0.0, 1.0);\n"
      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "   gl_Position.xy -= cam_pos.xy;\n"
      "   gl_Position.xy /= cam_size;\n"
      "   gl_Position = vec4(A * gl_Position.xyz, 1.0);\n"
      "}\0";
  u32 lineVertexShader;
  lineVertexShader = glCreateShader(GL_VERTEX_SHADER);
  GL_CALL(glShaderSource(lineVertexShader, 1, &lineVertexShaderSource, NULL));
  GL_CALL(glCompileShader(lineVertexShader));

  int success;
  char infoLog[512];
  glGetShaderiv(lineVertexShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(lineVertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
    ASSERT(false);
  }

  const char *vertexShaderSource =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "layout (location = 1) in vec2 uv;\n"
      "uniform vec3 pos;\n"
      "uniform vec3 pivot;\n"
      "uniform float cam_size;\n"
      "uniform float screen_ratio;\n"
      "uniform vec3 ent_size;\n"
      "uniform vec3 cam_pos;\n"
      "uniform vec3 cam_rot;\n"
      "uniform vec3 ent_rot;\n"
      "out vec2 TexCoord;\n"
      "void main()\n"
      "{\n"
      "	mat3 A = mat3(cos(cam_rot.z), -sin(cam_rot.z), 0.0,\n"
      "		 sin(cam_rot.z), cos(cam_rot.z), 0.0,\n"
      "		 0.0, 0.0, 1.0);\n"
      "	mat3 rot = mat3(cos(ent_rot.z), -sin(ent_rot.z), 0.0,\n"
      "		 sin(ent_rot.z), cos(ent_rot.z), 0.0,\n"
      "		 0.0, 0.0, 1.0);\n"
      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "   gl_Position.xy -= pivot.xy;\n"
      "   gl_Position.xy *= ent_size.xy;\n"
      "   gl_Position = vec4(rot * gl_Position.xyz, 1.0);\n"
      "   gl_Position.xy += pos.xy;\n"
      "   gl_Position.xy -= cam_pos.xy;\n"
      "   gl_Position.xy /= cam_size;\n"
      "   gl_Position = vec4(A * gl_Position.xyz, 1.0);\n"
      "   gl_Position.x *= screen_ratio;\n"
      "   gl_Position.z = pos.z;\n"
      "   TexCoord = uv;\n"
      "}\0";

  u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
  GL_CALL(glShaderSource(vertexShader, 1, &vertexShaderSource, NULL));
  GL_CALL(glCompileShader(vertexShader));

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
    ASSERT(false);
  }
  const char *fragmentShaderNoTextureSource =
      "#version 330 core\n"
      "out vec4 FragColor;\n"
      "in vec2 TexCoord;\n"
      "uniform vec4 color;\n"
      "void main(){\n"
      //"vec4 tex_col =  texture(tex, uv);\n"
      "FragColor =  color;\n"
      "}\n";
  const char *fragmentShaderSource =
      "#version 330 core\n"
      "out vec4 FragColor;\n"
      "in vec2 TexCoord;\n"
      "uniform vec4 color;\n"
      "uniform vec2 tex_size;\n"
      "uniform vec2 tex_offset;\n"
      "uniform sampler2D tex;\n"
      "void main(){\n"
      //"vec4 tex_col =  texture(tex, TexCoord + vec2(0.0, tex_size.y
      /// 16.0));\n"
      "vec4 tex_col =  texture(tex, (TexCoord + tex_offset)* tex_size);\n"
      "if(tex_col.a == 0.0)discard;\n"
      //"vec4 tex_col =  texture(tex, uv);\n"
      "FragColor =  tex_col * color;\n"
      "}\n";

  unsigned int fragmentShader =
      CompileShader((char *)fragmentShaderSource, GL_FRAGMENT_SHADER);
  unsigned int fragmentNoTextureShader =
      CompileShader((char *)fragmentShaderNoTextureSource, GL_FRAGMENT_SHADER);

  unsigned int lineShaderProgram;
  lineShaderProgram = glCreateProgram();
  glAttachShader(lineShaderProgram, lineVertexShader);
  glAttachShader(lineShaderProgram, fragmentNoTextureShader);
  glLinkProgram(lineShaderProgram);
  glUseProgram(lineShaderProgram);

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  unsigned int shaderProgramNoTexture;
  shaderProgramNoTexture = glCreateProgram();
  glAttachShader(shaderProgramNoTexture, vertexShader);
  glAttachShader(shaderProgramNoTexture, fragmentNoTextureShader);
  glLinkProgram(shaderProgramNoTexture);
  // glUseProgram(shaderProgram);

  gl_state->vao = VAO;
  gl_state->line_vao = LINEVAO;
  gl_state->line_shader_program = lineShaderProgram;
  gl_state->shader_program = shaderProgram;
  gl_state->shader_program_no_texture = shaderProgramNoTexture;

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Init3D(dbg);

  // Vertex Shader
}
void AddMemoryWatch(int thread_id, dbg_state *dbg) {
  int base = *(int *)GetRegValPtr(0, dbg, STACK_PTR_REG);
  int address = *(int *)&dbg->mem_buffer[base + 8];

  int val = *(int *)&dbg->mem_buffer[address];
  memory_watch m;
  m.address = address;
  m.prev_val = val;
  dbg->mem_watches.emplace_back(m);
}
/*
void DebuggerCommand(dbg_state* dbg)
{
        int base_ptr = *(int*)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
        int str_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
        //ASSERT(sz > 0)

        char* str = (char*)&dbg->mem_buffer[str_offset];


        int addr = *(int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR];
        //int *max = (int*)&dbg->mem_buffer[MEM_PTR_MAX_ADDR];
        *(int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR] += sz;
        ASSERT((addr + sz) < 64000);
        //*max += sz;

        *(int*)GetRegValPtr(thread_id, dbg, RET_1_REG) = addr;

}
*/
void SetMem(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int sz = *(int *)&dbg->mem_buffer[base_ptr + 8];
  ASSERT(sz > 0)

  int addr = *(int *)&dbg->mem_buffer[MEM_PTR_CUR_ADDR];
  // int *max = (int*)&dbg->mem_buffer[MEM_PTR_MAX_ADDR];
  *(int *)&dbg->mem_buffer[MEM_PTR_CUR_ADDR] = sz;
}
void SubMem(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int sz = *(int *)&dbg->mem_buffer[base_ptr + 8];
  ASSERT(sz > 0)

  int addr = *(int *)&dbg->mem_buffer[MEM_PTR_CUR_ADDR];
  // int *max = (int*)&dbg->mem_buffer[MEM_PTR_MAX_ADDR];
  *(int *)&dbg->mem_buffer[MEM_PTR_CUR_ADDR] -= sz;
  ASSERT((addr - sz) >= 0);
  //*(int*)GetRegValPtr(thread_id, dbg, RET_1_REG) = addr;
}
void GetMem(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int sz = *(int *)&dbg->mem_buffer[base_ptr + 8];
  ASSERT(sz > 0)
  ASSERT(base_ptr > (MEM_PTR_CUR_ADDR + 8));

  int addr = *(int *)&dbg->mem_buffer[MEM_PTR_CUR_ADDR];
  // int *max = (int*)&dbg->mem_buffer[MEM_PTR_MAX_ADDR];
  *(int *)&dbg->mem_buffer[MEM_PTR_CUR_ADDR] += sz;

  u64 max = DATA_SECT_OFFSET;
  printf("GetMem: cur %dmb, sz %dmb, offset %dmb\n", addr / 1024 / 1024,
         sz / 1024 / 1024, (addr + sz) / 1024 / 1024);
  if ((addr + sz) > max) {
    printf("GetMem: not enough space for %db,%dmb\n", sz, sz / 1024 / 1024);
    ASSERT(false)
  }
  //*max += sz;

  *(long long *)GetRegValPtr(thread_id, dbg, RET_1_REG) = addr;
}

void GetTimeSinceStart(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float val = *(float *)&dbg->mem_buffer[base_ptr + 8];
  // auto gl_state = (open_gl_state*)dbg->data;

  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = (float)glfwGetTime();
}
void Sqrt(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float val = *(float *)&dbg->mem_buffer[base_ptr + 8];

  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = sqrt(val);
}
void PrintStr(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int str_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  char *str = (char *)&dbg->mem_buffer[str_offset];
  printf("%s", str);

  //*(float*)GetRegValPtr(thread_id, dbg, RET_1_REG) = sinf(val);
}
void PrintV3Int(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int x = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int y = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int z = *(int *)&dbg->mem_buffer[base_ptr + 24];
#ifdef LINUX
  printf("x: %d, y: %d, z: %d\n", x, y, z);
#else
  char buffer[128];
  int sz = snprintf(buffer, 128, "x: %d, y: %d, z: %d\n", x, y, z);
  DWORD written = 0;
  WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, sz, &written, NULL);
#endif

  //*(float*)GetRegValPtr(thread_id, dbg, RET_1_REG) = sinf(val);
}
void PrintV3(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float x = *(float *)&dbg->mem_buffer[base_ptr + 8];
  float y = *(float *)&dbg->mem_buffer[base_ptr + 16];
  float z = *(float *)&dbg->mem_buffer[base_ptr + 24];
  printf("x: %.3f, y: %.3f, z: %.3f\n", x, y, z);

  //*(float*)GetRegValPtr(thread_id, dbg, RET_1_REG) = sinf(val);
}
void OpenLocalsWindow(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)&dbg->mem_buffer[BASE_STACK_PTR_REG * 8];
  int stack_base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int line = *(int *)&dbg->mem_buffer[stack_base_ptr + 8];
  if (dbg->frame_is_from_dbg)
    return;

  byte_code2 *addr = *(byte_code2 **)&dbg->mem_buffer[RIP_REG * 8];

  func_decl *fdecl = GetFuncBasedOnBc2(dbg, addr);
  scope *scp = FindScpWithLine(fdecl, line);
  ASSERT(scp);
  BeginLocalsChild(*dbg, base_ptr + 8, scp);
  // ImGui::Text("AOe");
  /// glfwSwapBuffers(window);
}
void MemSet(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int a_ptr = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int b = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int c = *(int *)&dbg->mem_buffer[base_ptr + 24];
  int *a = (int *)&dbg->mem_buffer[a_ptr];
  memset(a, b, c);
}
void MemCpy(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int a_ptr = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int b_ptr = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int c_ptr = *(int *)&dbg->mem_buffer[base_ptr + 24];
  void *a = (void *)&dbg->mem_buffer[a_ptr];
  void *b = (void *)&dbg->mem_buffer[b_ptr];
  memcpy(a, b, c_ptr);
}
void PointLineDistance(int thread_id, dbg_state *dbg) {
  /*
  int base_ptr = *(int*)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int a_ptr = *(int*)&dbg->mem_buffer[base_ptr + 8];
  int b_ptr = *(int*)&dbg->mem_buffer[base_ptr + 16];
  int c_ptr = *(int*)&dbg->mem_buffer[base_ptr + 24];
  int d_ptr = *(int*)&dbg->mem_buffer[base_ptr + 32];

  auto a = (v4*)&dbg->mem_buffer[a_ptr];
  auto b = (v4*)&dbg->mem_buffer[b_ptr];
  auto p = (v4*)&dbg->mem_buffer[c_ptr];
  auto closest_point = (v3*)&dbg->mem_buffer[d_ptr];

  v3 ab;
  ab.x = b->x - a->x;
  ab.y = b->y - a->y;
  ab.z = b->z - a->z;

  v3 ap;
  ap.x = p->x - a->x;
  ap.y = p->y - a->y;
  ap.z = p->z - a->z;

  float proj = ab.dot(ap);
  float d = proj / ab.dot(ab);
  if (d <= 0)
  {
          *closest_point = *a;
  }
  else if (d >= 1)
  {
          *closest_point = *b;
  }
  else
  {
          //*closest_point = *a + ab.mul(d);
  }
  v3 ret;
  //ret.x = closest_point->x -
  //*(float*)GetRegValPtr(thread_id, dbg, RET_1_REG) =
  */
}
void DotV3(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int a_ptr = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int b_ptr = *(int *)&dbg->mem_buffer[base_ptr + 16];
  float *a = (float *)&dbg->mem_buffer[a_ptr];
  float *b = (float *)&dbg->mem_buffer[b_ptr];

  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) =
      a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
void WriteFileInterpreter(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int name_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int buffer_offset = *(int *)&dbg->mem_buffer[base_ptr + 16];
  int buffer_sz = *(int *)&dbg->mem_buffer[base_ptr + 24];

  auto name_ptr = (char *)&dbg->mem_buffer[name_offset];
  auto buffer_ptr = (char *)&dbg->mem_buffer[buffer_offset];

  own_std::string work_dir = dbg->cur_func->from_file->path;
  // MaybeAddBarToEndOfStr(&work_dir);

  work_dir = work_dir + name_ptr;
  // MaybeAddBarToEndOfStr(&work_dir);

  WriteFileLang((char *)work_dir.c_str(), buffer_ptr, buffer_sz);
}
void GetInstRealAddr(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int inst_idx = *(int *)&dbg->mem_buffer[base_ptr + 8];
  *(u64 *)GetRegValPtr(thread_id, dbg, RET_1_REG) =
      (u64)(dbg->lang_stat->bcs2_start + inst_idx);
}
void GetTopStackPtr(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  *(u64 *)GetRegValPtr(thread_id, dbg, RET_1_REG) = base_ptr + 8;
}
void Asin(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float val = *(float *)&dbg->mem_buffer[base_ptr + 8];

  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = asin(val);
}
void Atan2(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float val = *(float *)&dbg->mem_buffer[base_ptr + 8];
  float val2 = *(float *)&dbg->mem_buffer[base_ptr + 16];

  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = atan2(val, val2);
}
void Acos(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float val = *(float *)&dbg->mem_buffer[base_ptr + 8];

  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = acosf(val);
}
void Cos(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float val = *(float *)&dbg->mem_buffer[base_ptr + 8];

  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = cosf(val);
}
void GetMouseScroll(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  *(int *)GetRegValPtr(thread_id, dbg, RET_1_REG) = gl_state->scroll;
}
void Tan(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float val = *(float *)&dbg->mem_buffer[base_ptr + 8];

  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = tanf(val);
}
void Sin(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  float val = *(float *)&dbg->mem_buffer[base_ptr + 8];

  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = sinf(val);
}
void Stub() {}

own_std::string GetFileNameOnly(own_std::string &path) {
  int last_bar = path.find_last_of('/');

  if (last_bar == (path.size() - 1)) {
    return "";
  }
  if (last_bar == -1)
    last_bar = 0;

  return path.substr(last_bar + 1);
}
own_std::string GetFolderName(own_std::string path) {
  int last_bar = path.find_last_of('/');

  if (last_bar == (path.size() - 1)) {
    last_bar--;
    while (path[last_bar] != '/' && path[last_bar] != '\\' && last_bar > 0)
      last_bar--;
    // last_bar = path.fi(path.data(), 0, last_bar - 1);
  }
  if (last_bar == -1)
    last_bar = 0;

  return path.substr(last_bar + 1);
}
void IsMouseOnGameWindow(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  GLFWwindow *window = (GLFWwindow *)gl_state->glfw_window;
  double ypos, xpos;
  glfwGetCursorPos(window, &xpos, &ypos);

  if (gl_state->is_engine) {
    ypos /= gl_state->scene_srceen_height;
    xpos /= gl_state->scene_srceen_width;
  } else {
    ypos /= gl_state->height;
    xpos /= gl_state->width;
  }
  // ypos *= 10;
  bool ret = true;

  if (xpos > 1 || xpos < 0)
    ret = false;
  if (ypos > 1 || ypos < 0)
    ret = false;
  *(bool *)GetRegValPtr(thread_id, dbg, RET_1_REG) = ret;
}
void GetMouseNormalizedPosY(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  GLFWwindow *window = (GLFWwindow *)gl_state->glfw_window;
  double ypos, xpos;
  glfwGetCursorPos(window, &xpos, &ypos);

  if (gl_state->is_engine) {
    float ratio = (float)gl_state->scene_srceen_height /
                  (float)gl_state->scene_srceen_width;
    ypos /= gl_state->scene_srceen_height;
  } else {
    float ratio = (float)gl_state->height / (float)gl_state->width;
    ypos /= gl_state->height;
  }
  ypos = ypos * 2 - 1;
  // ypos *= 10;
  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = ypos;
}
void GetMouseNormalizedPosX(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  GLFWwindow *window = (GLFWwindow *)gl_state->glfw_window;

  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  float ratio = (float)gl_state->height / (float)gl_state->width;
  if (gl_state->is_engine) {
    ratio = (float)gl_state->scene_srceen_height /
            (float)gl_state->scene_srceen_width;
    xpos /= gl_state->scene_srceen_width;
  } else {
    // ratio = (float)gl_state->height / (float)gl_state->width;
    xpos /= gl_state->width;
  }
  xpos = xpos * 2 - 1;
  xpos /= ratio;
  // ypos *= ;
  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = xpos;
}
void GetMouseVelY(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = gl_state->mouse_vel_y;
}
void GetMouseVelX(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = gl_state->mouse_vel_x;
}
void GetMouseScreenPosY(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  GLFWwindow *window = (GLFWwindow *)gl_state->glfw_window;
  double ypos, xpos;
  glfwGetCursorPos(window, &xpos, &ypos);
  if (gl_state->is_engine) {
    float ratio =
        (float)gl_state->height / (float)gl_state->scene_srceen_height;
    // ypos *= ratio;
  }
  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = ypos;
}
void GetMouseScreenPosX(int thread_id, dbg_state *dbg) {
  auto gl_state = (open_gl_state *)dbg->data;
  GLFWwindow *window = (GLFWwindow *)gl_state->glfw_window;

  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  if (gl_state->is_engine) {
    float ratio = (float)gl_state->width / (float)gl_state->scene_srceen_width;
    // xpos *= ratio;
  }
  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = xpos;
}
void HackFunc(int thread_id, dbg_state *dbg, GLFWwindow *window,
              byte_code2 *cur_bc);
void *CreateThreadAux(void *data) {
  auto a = (thread_creation *)data;

  auto gl_state = (open_gl_state *)a->dbg->data;
  byte_code2 *start = a->dbg->lang_stat->bcs2_start + a->func_bc_idx;
  auto window = (GLFWwindow *)gl_state->glfw_window;
  auto dbg = a->dbg;
  int addr = a->args_addr;

  auto stack_ptr = GetRegValPtr(a->thread_id, dbg, PRE_X64_RSP_REG);
  *stack_ptr = STACK_PTR_START_THREAD2;

  // thread_id
  int *offset = (int *)&dbg->mem_buffer[STACK_PTR_START_THREAD2 + 8];
  *offset = a->heandle_id;

  // args for thread
  offset = (int *)&dbg->mem_buffer[STACK_PTR_START_THREAD2 + 16];
  *offset = addr;

  // nulling the ret address
  offset = (int *)(int *)&dbg->mem_buffer[STACK_PTR_START_THREAD2];
  *offset = 0;

  ThreadFunc(a, a->dbg, window, start);
  return nullptr;
}
void _WaitThread(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int th_id = *(int *)&dbg->mem_buffer[base_ptr + 8];

  handle_info *h = &dbg->handles[th_id];
  // printf("trying to resume %d\n", th_id);
#ifdef LINUX
  pthread_mutex_lock(&h->th->mutex);
  pthread_mutex_unlock(&h->th->mutex);
#else
#endif
}
void _ResumeThread(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int th_id = *(int *)&dbg->mem_buffer[base_ptr + 8];

  handle_info *h = &dbg->handles[th_id];
  // printf("trying to resume %d\n", th_id);
#ifdef LINUX
  pthread_mutex_lock(&h->th->mutex);
  pthread_cond_signal(&h->th->cond);
  pthread_mutex_unlock(&h->th->mutex);
#else
#endif
}
void SuspendThread(thread_creation *th, dbg_state *dbg) {
#ifdef LINUX

  pthread_mutex_lock(&th->mutex);
  printf("\nthread %d will sleep\n", th->thread_id);
  pthread_cond_wait(&th->cond, &th->mutex);
  printf("\nthread %d woke up\n", th->thread_id);
  pthread_mutex_unlock(&th->mutex);
#else
#endif
}
void _SuspendThread(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int th_id = *(int *)&dbg->mem_buffer[base_ptr + 8];

  handle_info *h = &dbg->handles[th_id];
  SuspendThread(h->th, dbg);
}
void _JoinThread(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int th_id = *(int *)&dbg->mem_buffer[base_ptr + 8];

  handle_info *h = &dbg->handles[th_id];

#ifdef LINUX
  pthread_join(h->th->thread, NULL);
#else
#endif
}
void _CreateThread(int thread_id, dbg_state *dbg) {
  dbg->total_threads++;
  auto new_thread_id = dbg->total_threads;
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int func_addr = *(int *)&dbg->mem_buffer[base_ptr + 8];
  int idx = GetFreeHandle(dbg);

  handle_info *h = &dbg->handles[idx];
  h->th =
      (thread_creation *)AllocMiscData(dbg->lang_stat, sizeof(thread_creation));
  auto args = h->th;
  h->type = handle_enum::THREAD;
#ifdef LINUX
  memset(args, 0, sizeof(thread_creation));

  args->args_addr = *(int *)&dbg->mem_buffer[base_ptr + 16];

  args->dbg = dbg;
  args->thread_id = new_thread_id;
  args->func_bc_idx = func_addr;
  args->parent_thread = thread_id;

  pthread_create(&args->thread, NULL, CreateThreadAux, args);

#else
#endif
  int *ret = (int *)GetRegValPtr(thread_id, dbg, RET_1_REG);
  *ret = idx;
}
void Rand01(int thread_id, dbg_state *dbg) {
  auto r = ((unsigned int)rand()) % 20000;
  double f = (double)r / 20000;
  *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = f;
}

void FreeTexture(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  int tex_id = *(int *)&dbg->mem_buffer[base_ptr + 8];
  auto gl_state = (open_gl_state *)dbg->data;
  texture_info *t = &gl_state->textures[tex_id];
  if (t->used) {
    glDeleteTextures(1, (GLuint *)&t->id);
    t->used = false;
  }
}
void ScreenRatio(int thread_id, dbg_state *dbg) {
  int base_ptr = *(int *)GetRegValPtr(thread_id, dbg, STACK_PTR_REG);
  auto gl_state = (open_gl_state *)dbg->data;
  GLFWwindow *window = (GLFWwindow *)gl_state->glfw_window;

  if (gl_state->is_engine) {
    float ratio = (float)gl_state->scene_srceen_height /
                  (float)gl_state->scene_srceen_width;
    *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = ratio;
  } else {
    float ratio = (float)gl_state->height / (float)gl_state->width;
    *(float *)GetRegValPtr(thread_id, dbg, RET_1_REG) = ratio;
  }
}

bool fileExists(const char *filename) {
  std::ifstream file(filename);
  return file.is_open();
}
void *alloc_own(int size, mem_alloc *alloc) { return heap_alloc(alloc, size); }
void free_own(char *ptr, mem_alloc *alloc) { heap_free(alloc, ptr); }
void *realloc_own(char *ptr, int size, mem_alloc *alloc) {
  void *ret = heap_alloc(alloc, size);
  auto chunk = (mem_chunk *)alloc->in_use.Get(ptr);

  memcpy(ret, ptr, min(chunk->size * BYTES_PER_CHUNK, size));

  heap_free(alloc, ptr);
  return ret;
}
AudioClip *CreateNewAudioClip(open_gl_state *gl_state, char *name) {
  if (!fileExists(name)) {
    std::cerr << "File could not be opened or does not exist: " << name
              << std::endl;
    ASSERT(false);
    return nullptr;
  }

  auto ret =
      (AudioClip *)__lang_globals.alloc(__lang_globals.data, sizeof(AudioClip));
  memset(ret, 0, sizeof(AudioClip));
  // ret->name = name;
  memset(&ret->buffer, 0, sizeof(own_std::vector<int>));

  unsigned int channels;
  unsigned int sampleRate;
  drflac_uint64 totalPCMFrameCount;
  // dr
  drflac_int32 *pSampleData = drflac_open_file_and_read_pcm_frames_s32(
      name, &channels, &sampleRate, &totalPCMFrameCount, NULL);

  ret->buffer.make_count(totalPCMFrameCount * channels);
  ret->time = (float)(totalPCMFrameCount) / (float)sampleRate;
  int sz = totalPCMFrameCount * channels;

  for (int i = 0; i < sz; i++) {

    ret->buffer[i] = pSampleData[i] >> 16;
  }
  // memcpy(&ret->buffer[0], pSampleData, sz);
  free(pSampleData);

  /*
  drflac* pFlac = drflac_open_file("01.flac", &allocationCallbacks);
  int sampleRate, channels;
  short* buffer;
  //int res = stb_vorbis_decode_filename(name, channels, &sampleRate,
  &buffer);
  */
  return ret;
}

int main(int argc, char *argv[]) {
  auto ttt = 0;
  mem_alloc alloc;
  alloc.main_buffer = nullptr;
  InitMemAlloc(&alloc);

  auto stat = (lang_state *)malloc(sizeof(lang_state));
  lang_state &lang_stat = *stat;

  InitLang(&lang_stat, (AllocTypeFunc)heap_alloc, (FreeTypeFunc)heap_free,
           &alloc);

#ifdef LINUX
  char path[1024];
  ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);

  own_std::string exe_full;
  if (len != -1) {
    path[len] = '\0';
    exe_full = path;
  } else {
    ASSERT(false);
    // handle error
  }
  int last_bar = exe_full.find_last_of("\\/");
  own_std::string exe_dir = exe_full.substr(0, last_bar + 1);
  chdir(exe_dir.c_str());
#else
  TCHAR buffer[MAX_PATH] = {0};
  GetModuleFileName(NULL, buffer, MAX_PATH);
  own_std::string exe_full = buffer;
  int last_bar = exe_full.find_last_of("\\/");
  own_std::string exe_dir = exe_full.substr(0, last_bar + 1);
  SetCurrentDirectory(exe_dir.c_str());
#endif

  sound_state sound;
#ifdef LINUX
#else
  sound.audio_class = new XAudioClass();
  // memset(sound.audio_class, 0, sizeof(sound.audio_class));
  sound.audio_class->sound = &sound;

  InitXAudio2(sound, false);
#endif
  /*
  auto addr = heap_alloc(&alloc, 12);
  auto addr2 = heap_alloc(&alloc, 12);
  heap_free(&alloc, addr);
  heap_free(&alloc, addr2);
  */

  // sound.audio_clips_src.emplace_back(CreateNewAudioClip("02.flac"));
  // AddAudioClipToPlay(&sound, sound.audio_clips_src[0]);

  own_std::string oexe_dir = lang_stat.exe_dir.c_str();
  last_bar = oexe_dir.find_last_of("/\\");
  own_std::string dir = oexe_dir.substr(0, last_bar + 1);

  dir = "hello";
  dir = dir + "jij";

  ASSERT(dir == "hellojij");

  compile_options opts = {};
  // opts.file = "../lang2/files";
  // opts.wasm_dir = "../lang2/web/";
  int arg_start = 0;
  own_std::string arg1 = argv[1];
  if (arg1 == "chk_nptr") {
    lang_stat.check_nil_ptr = true;
    arg_start = 1;
  }

  if (argc > 1) {
    arg1 = argv[arg_start + 1];
    if (arg1 == "run") {
      if (argc <= 2) {
        printf("no folder specified\n");
        return 0;
      } else {
        opts.file = argv[arg_start + 2];
        opts.folder_name = GetFolderName(opts.file);
        opts.wasm_dir = "";
        opts.release = false;
      }

    } else if (arg1 == "webgame") {
      if (argc != 4) {
        printf("not all arguments were provided. webgame needs 1) the "
               "path/to/src/folder and 2)the path/to/output\n");
        return 0;
      } else {
        opts.file = argv[2];

        MaybeAddBarToEndOfStr(&opts.file);
        opts.folder_name = GetFolderName(opts.file);

        opts.wasm_dir = argv[3];
        // opts.wasm_dir += folder_name;
        opts.release = true;
        ImageFolderToFile(0, opts.file + "/images/");
        opts.file += "files";
      }
    } else {
      printf("Command not recognized '%s'", arg1.c_str());
      return 0;
    }
  } else {
    printf("no command provided");
    return 0;
  }

  MaybeAddBarToEndOfStr(&opts.wasm_dir);

  auto wasm_dir = std_str_to_heap2(&opts.wasm_dir);
  auto folder_name = std_str_to_heap2(&opts.folder_name);

  Compile(&lang_stat, &opts);
  memset(&lang_stat, 0, sizeof(lang_stat));
  InitMemAlloc(&alloc);
  InitLang(&lang_stat, (AllocTypeFunc)heap_alloc, (FreeTypeFunc)heap_free,
           &alloc);
  // AssertFuncByteCode(&lang_stat);

  AssignOutsiderFunc(&lang_stat, "GetMem", (OutsiderFuncType)GetMem);
  AssignOutsiderFunc(&lang_stat, "SetMem", (OutsiderFuncType)SetMem);
  AssignOutsiderFunc(&lang_stat, "SubMem", (OutsiderFuncType)SubMem);
  AssignOutsiderFunc(&lang_stat, "Print", (OutsiderFuncType)Print);
  AssignOutsiderFunc(&lang_stat, "OpenWindow", (OutsiderFuncType)OpenWindow);
  AssignOutsiderFunc(&lang_stat, "ShouldClose", (OutsiderFuncType)ShouldClose);
  AssignOutsiderFunc(&lang_stat, "ClearBackground",
                     (OutsiderFuncType)ClearBackground);
  AssignOutsiderFunc(&lang_stat, "Draw", (OutsiderFuncType)Draw);
  AssignOutsiderFunc(&lang_stat, "Draw3D", (OutsiderFuncType)Draw3D);
  AssignOutsiderFunc(&lang_stat, "Draw3DTransparency",
                     (OutsiderFuncType)Draw3DTransparency);
  AssignOutsiderFunc(&lang_stat, "GetTime", (OutsiderFuncType)GetTime);

  AssignOutsiderFunc(&lang_stat, "CreateThread",
                     (OutsiderFuncType)_CreateThread);
  AssignOutsiderFunc(&lang_stat, "JoinThread", (OutsiderFuncType)_JoinThread);
  AssignOutsiderFunc(&lang_stat, "SuspendThread",
                     (OutsiderFuncType)_SuspendThread);
  AssignOutsiderFunc(&lang_stat, "ResumeThread",
                     (OutsiderFuncType)_ResumeThread);
  AssignOutsiderFunc(&lang_stat, "WaitThread", (OutsiderFuncType)_WaitThread);

  AssignOutsiderFunc(&lang_stat, "IsKeyHeld", (OutsiderFuncType)IsKeyHeld);
  AssignOutsiderFunc(&lang_stat, "IsKeyDown", (OutsiderFuncType)IsKeyDown);
  AssignOutsiderFunc(&lang_stat, "IsKeyUp", (OutsiderFuncType)IsKeyUp);

  AssignOutsiderFunc(&lang_stat, "LoadClip", (OutsiderFuncType)LoadClip);
  AssignOutsiderFunc(&lang_stat, "Perlin2D", (OutsiderFuncType)Perlin2D);
  AssignOutsiderFunc(&lang_stat, "LoadTex", (OutsiderFuncType)LoadTex);
  AssignOutsiderFunc(&lang_stat, "LoadSceneFolder",
                     (OutsiderFuncType)LoadSceneFolder);
  // AssignOutsiderFunc(&lang_stat, "GetDeltaTime",
  // (OutsiderFuncType)GetDeltaTime);
  AssignOutsiderFunc(&lang_stat, "EndFrame", (OutsiderFuncType)EndFrame);
  AssignOutsiderFunc(&lang_stat, "GetTimeSinceStart",
                     (OutsiderFuncType)GetTimeSinceStart);
  AssignOutsiderFunc(&lang_stat, "sqrt", (OutsiderFuncType)Sqrt);
  AssignOutsiderFunc(&lang_stat, "AssignCtxAddr", (OutsiderFuncType)Stub);
  AssignOutsiderFunc(&lang_stat, "WasmDbg", (OutsiderFuncType)Stub);

  AssignOutsiderFunc(&lang_stat, "PrintV3", (OutsiderFuncType)PrintV3);
  AssignOutsiderFunc(&lang_stat, "PrintV3Int", (OutsiderFuncType)PrintV3Int);
  AssignOutsiderFunc(&lang_stat, "PrintStr", (OutsiderFuncType)PrintStr);

  AssignOutsiderFunc(&lang_stat, "OpenFile", (OutsiderFuncType)OpenFile);
  AssignOutsiderFunc(&lang_stat, "SetFilePtr", (OutsiderFuncType)SetFilePtr);
  AssignOutsiderFunc(&lang_stat, "WriteToFile", (OutsiderFuncType)WriteToFile);
  AssignOutsiderFunc(&lang_stat, "CloseFile", (OutsiderFuncType)CloseFile);

  AssignOutsiderFunc(&lang_stat, "HandleForGettingFilesInDir",
                     (OutsiderFuncType)HandleForGettingFilesInDir);
  AssignOutsiderFunc(&lang_stat, "FileChangeTime",
                     (OutsiderFuncType)FileChangeTime);

  AssignOutsiderFunc(&lang_stat, "HandleDirFilenameAt",
                     (OutsiderFuncType)HandleDirFilenameAt);

  AssignOutsiderFunc(&lang_stat, "HandleDirTotalFiles",
                     (OutsiderFuncType)HandleDirTotalFiles);
  AssignOutsiderFunc(&lang_stat, "FreeHandle", (OutsiderFuncType)FreeHandle);

  AssignOutsiderFunc(&lang_stat, "AssignTexFolder",
                     (OutsiderFuncType)AssignTexFolder);
  AssignOutsiderFunc(&lang_stat, "AssignModelFolder",
                     (OutsiderFuncType)AssignModelFolder);

  AssignOutsiderFunc(&lang_stat, "GetMouseNormalizedPosX",
                     (OutsiderFuncType)GetMouseNormalizedPosX);
  AssignOutsiderFunc(&lang_stat, "GetMouseNormalizedPosY",
                     (OutsiderFuncType)GetMouseNormalizedPosY);
  AssignOutsiderFunc(&lang_stat, "GetMouseScreenPosX",
                     (OutsiderFuncType)GetMouseScreenPosX);
  AssignOutsiderFunc(&lang_stat, "GetMouseScreenPosY",
                     (OutsiderFuncType)GetMouseScreenPosY);
  AssignOutsiderFunc(&lang_stat, "GetMouseVelX",
                     (OutsiderFuncType)GetMouseVelX);
  AssignOutsiderFunc(&lang_stat, "GetMouseVelY",
                     (OutsiderFuncType)GetMouseVelY);

  AssignOutsiderFunc(&lang_stat, "FreeTexture", (OutsiderFuncType)FreeTexture);

  AssignOutsiderFunc(&lang_stat, "IsMouseHeld", (OutsiderFuncType)IsMouseHeld);
  AssignOutsiderFunc(&lang_stat, "IsMouseUp", (OutsiderFuncType)IsMouseUp);
  AssignOutsiderFunc(&lang_stat, "IsMouseDown", (OutsiderFuncType)IsMouseDown);
  AssignOutsiderFunc(&lang_stat, "IsMouseDoubleClick",
                     (OutsiderFuncType)IsMouseDoubleClick);

  AssignOutsiderFunc(&lang_stat, "AssignSoundFolder",
                     (OutsiderFuncType)AssignSoundFolder);
  AssignOutsiderFunc(&lang_stat, "PlayAudio",
                     (OutsiderFuncType)FromGamePlayAudio);
  AssignOutsiderFunc(&lang_stat, "GetAudioHandle",
                     (OutsiderFuncType)GetAudioHandle);
  AssignOutsiderFunc(&lang_stat, "PlayAudioByHandle",
                     (OutsiderFuncType)PlayAudioByHandle);

  AssignOutsiderFunc(&lang_stat, "ScreenRatio", (OutsiderFuncType)ScreenRatio);
  // AssignOutsiderFunc(&lang_stat, "DebuggerCommand",
  // (OutsiderFuncType)DebuggerCommand);
  AssignOutsiderFunc(&lang_stat, "ScreenMouseToWorld",
                     (OutsiderFuncType)ScreenMouseToWorld);
  AssignOutsiderFunc(&lang_stat, "sin", (OutsiderFuncType)Sin);
  AssignOutsiderFunc(&lang_stat, "tanf", (OutsiderFuncType)Tan);
  AssignOutsiderFunc(&lang_stat, "cos", (OutsiderFuncType)Cos);
  AssignOutsiderFunc(&lang_stat, "acos", (OutsiderFuncType)Acos);
  AssignOutsiderFunc(&lang_stat, "atan2", (OutsiderFuncType)Atan2);
  AssignOutsiderFunc(&lang_stat, "asin", (OutsiderFuncType)Asin);
  AssignOutsiderFunc(&lang_stat, "dot_v3", (OutsiderFuncType)DotV3);
  AssignOutsiderFunc(&lang_stat, "memcpy", (OutsiderFuncType)MemCpy);
  AssignOutsiderFunc(&lang_stat, "memset", (OutsiderFuncType)MemSet);
  AssignOutsiderFunc(&lang_stat, "PointLineDistance",
                     (OutsiderFuncType)PointLineDistance);
  AssignOutsiderFunc(&lang_stat, "OpenLocalsWindow",
                     (OutsiderFuncType)OpenLocalsWindow);
  AssignOutsiderFunc(&lang_stat, "Rand01", (OutsiderFuncType)Rand01);

  AssignOutsiderFunc(&lang_stat, "ImGuiBegin", (OutsiderFuncType)ImGuiBegin);
  AssignOutsiderFunc(&lang_stat, "ImGuiEnd", (OutsiderFuncType)ImGuiEnd);
  AssignOutsiderFunc(&lang_stat, "ImGuiBeginChild",
                     (OutsiderFuncType)ImGuiBeginChild);
  AssignOutsiderFunc(&lang_stat, "ImGuiEndChild",
                     (OutsiderFuncType)ImGuiEndChild);
  AssignOutsiderFunc(&lang_stat, "ImGuiText", (OutsiderFuncType)ImGuiText);
  AssignOutsiderFunc(&lang_stat, "ImGuiImage", (OutsiderFuncType)ImGuiImage);
  AssignOutsiderFunc(&lang_stat, "ImGuiSelectable",
                     (OutsiderFuncType)ImGuiSelectable);
  AssignOutsiderFunc(&lang_stat, "ImGuiButton", (OutsiderFuncType)ImGuiButton);
  AssignOutsiderFunc(&lang_stat, "ImGuiSameLine",
                     (OutsiderFuncType)ImGuiSameLine);
  AssignOutsiderFunc(&lang_stat, "ImGuiPushItemWidth",
                     (OutsiderFuncType)ImGuiPushItemWidth);
  AssignOutsiderFunc(&lang_stat, "ImGuiPopItemWidth",
                     (OutsiderFuncType)ImGuiPopItemWidth);
  AssignOutsiderFunc(&lang_stat, "ImGuiSetNextItemAllowOverlap",
                     (OutsiderFuncType)ImGuiSetNextItemAllowOverlap);
  AssignOutsiderFunc(&lang_stat, "ImGuiGetCursorPosX",
                     (OutsiderFuncType)ImGuiGetCursorPosX);
  AssignOutsiderFunc(&lang_stat, "ImGuiGetCursorPosY",
                     (OutsiderFuncType)ImGuiGetCursorPosY);
  AssignOutsiderFunc(&lang_stat, "ImGuiGetCursorScreenPosX",
                     (OutsiderFuncType)ImGuiGetCursorScreenPosX);
  AssignOutsiderFunc(&lang_stat, "ImGuiGetCursorScreenPosY",
                     (OutsiderFuncType)ImGuiGetCursorScreenPosY);
  AssignOutsiderFunc(&lang_stat, "ImGuiSetCursorPos",
                     (OutsiderFuncType)ImGuiSetCursorPos);
  AssignOutsiderFunc(&lang_stat, "ImGuiAddRect",
                     (OutsiderFuncType)ImGuiAddRect);
  AssignOutsiderFunc(&lang_stat, "ImGuiHasFocus",
                     (OutsiderFuncType)ImGuiHasFocus);
  AssignOutsiderFunc(&lang_stat, "ImGuiTreeNodeEx",
                     (OutsiderFuncType)ImGuiTreeNodeEx);
  AssignOutsiderFunc(&lang_stat, "ImGuiTreePop",
                     (OutsiderFuncType)ImGuiTreePop);
  AssignOutsiderFunc(&lang_stat, "ImGuiEnumCombo",
                     (OutsiderFuncType)ImGuiEnumCombo);
  // AssignOutsiderFunc(&lang_stat, "ImGuiInitTextEditor",
  // (OutsiderFuncType)ImGuiInitTextEditor);
  AssignOutsiderFunc(&lang_stat, "ImGuiInputText",
                     (OutsiderFuncType)ImGuiInputText);
  AssignOutsiderFunc(&lang_stat, "ImGuiInputInt",
                     (OutsiderFuncType)ImGuiInputInt);
  AssignOutsiderFunc(&lang_stat, "ImGuiInputF32",
                     (OutsiderFuncType)ImGuiInputF32);
  AssignOutsiderFunc(&lang_stat, "ImGuiDragInt",
                     (OutsiderFuncType)ImGuiDragInt);
  AssignOutsiderFunc(&lang_stat, "ImGuiSeparator",
                     (OutsiderFuncType)ImGuiSeparator);
  AssignOutsiderFunc(&lang_stat, "ImGuiDragF32",
                     (OutsiderFuncType)ImGuiDragF32);
  // AssignOutsiderFunc(&lang_stat, "ImGuiRenderTextEditor",
  // (OutsiderFuncType)ImGuiRenderTextEditor);
  AssignOutsiderFunc(&lang_stat, "ImGuiSetWindowFontScale",
                     (OutsiderFuncType)ImGuiSetWindowFontScale);
  AssignOutsiderFunc(&lang_stat, "ImGuiCheckbox",
                     (OutsiderFuncType)ImGuiCheckbox);

  AssignOutsiderFunc(&lang_stat, "CopyTextureToBuffer",
                     (OutsiderFuncType)CopyTextureToBuffer);

  AssignOutsiderFunc(&lang_stat, "LoadSheetFromLayer",
                     (OutsiderFuncType)LoadSheetFromLayer);

  AssignOutsiderFunc(&lang_stat, "WriteFile",
                     (OutsiderFuncType)WriteFileInterpreter);
  AssignOutsiderFunc(&lang_stat, "ReadFile", (OutsiderFuncType)ReadFileInterp);
  AssignOutsiderFunc(&lang_stat, "GetFileSize", (OutsiderFuncType)GetFileSize);

  AssignOutsiderFunc(&lang_stat, "LoadTexFolder",
                     (OutsiderFuncType)LoadTexFolder);
  AssignOutsiderFunc(&lang_stat, "LoadModel", (OutsiderFuncType)LoadModel);
  AssignOutsiderFunc(&lang_stat, "ReloadModel", (OutsiderFuncType)ReloadModel);
  AssignOutsiderFunc(&lang_stat, "FreeModel", (OutsiderFuncType)FreeModel);
  AssignOutsiderFunc(&lang_stat, "ModelFarthestPoint",
                     (OutsiderFuncType)ModelFarthestPoint);
  AssignOutsiderFunc(&lang_stat, "UpdateModel", (OutsiderFuncType)UpdateModel);
  AssignOutsiderFunc(&lang_stat, "CreateMesh", (OutsiderFuncType)CreateMesh);
  AssignOutsiderFunc(&lang_stat, "GenRawTexture",
                     (OutsiderFuncType)GenRawTexture);
  AssignOutsiderFunc(&lang_stat, "UpdateTexture",
                     (OutsiderFuncType)UpdateTexture);
  AssignOutsiderFunc(&lang_stat, "GetMouseScroll",
                     (OutsiderFuncType)GetMouseScroll);
  AssignOutsiderFunc(&lang_stat, "SetIsEngine", (OutsiderFuncType)SetIsEngine);
  AssignOutsiderFunc(&lang_stat, "ImGuiPushID", (OutsiderFuncType)ImGuiPushID);
  AssignOutsiderFunc(&lang_stat, "ImGuiPopID", (OutsiderFuncType)ImGuiPopID);
  AssignOutsiderFunc(&lang_stat, "ImGuiShowV3", (OutsiderFuncType)ImGuiShowV3);
  AssignOutsiderFunc(&lang_stat, "ImGuiShowV2", (OutsiderFuncType)ImGuiShowV2);
  AssignOutsiderFunc(&lang_stat, "ImGuiShowV3", (OutsiderFuncType)ImGuiShowV3);
  AssignOutsiderFunc(&lang_stat, "ImGuiShowV4", (OutsiderFuncType)ImGuiShowV4);
  AssignOutsiderFunc(&lang_stat, "ImGuiSetKeyboardFocusHere",
                     (OutsiderFuncType)ImGuiSetKeyboardFocusHere);
  AssignOutsiderFunc(&lang_stat, "IsMouseOnGameWindow",
                     (OutsiderFuncType)IsMouseOnGameWindow);
  AssignOutsiderFunc(&lang_stat, "GetTopStackPtr",
                     (OutsiderFuncType)GetTopStackPtr);
  AssignOutsiderFunc(&lang_stat, "GetInstRealAddr",
                     (OutsiderFuncType)GetInstRealAddr);
  AssignOutsiderFunc(&lang_stat, "HideCursor", (OutsiderFuncType)HideCursor);

  AssignOutsiderFunc(&lang_stat, "CompileShader",
                     (OutsiderFuncType)CompileShader2);
  AssignOutsiderFunc(&lang_stat, "GetUniformLocation",
                     (OutsiderFuncType)_GetUniformLocation);
  AssignOutsiderFunc(&lang_stat, "ValidateTextureSlot",
                     (OutsiderFuncType)ValidateTextureSlot);
  AssignOutsiderFunc(&lang_stat, "SetUniform4f",
                     (OutsiderFuncType)SetUniform4f);
  AssignOutsiderFunc(&lang_stat, "SetUniform3f",
                     (OutsiderFuncType)SetUniform3f);
  AssignOutsiderFunc(&lang_stat, "SetUniform2f",
                     (OutsiderFuncType)SetUniform2f);
  AssignOutsiderFunc(&lang_stat, "SetUniform1f",
                     (OutsiderFuncType)SetUniform1f);
  AssignOutsiderFunc(&lang_stat, "SetUniformMatrices4x4",
                     (OutsiderFuncType)SetUniformMatrices4x4);
  AssignOutsiderFunc(&lang_stat, "SetSampler2D",
                     (OutsiderFuncType)SetSampler2D);
  AssignOutsiderFunc(&lang_stat, "SetShader", (OutsiderFuncType)SetShader);

  AssignOutsiderFunc(&lang_stat, "GetBoneChildrenData",
                     (OutsiderFuncType)GetBoneChildrenData);
  AssignOutsiderFunc(&lang_stat, "GetBoneChildrenLen",
                     (OutsiderFuncType)GetBoneChildrenLen);
  AssignOutsiderFunc(&lang_stat, "GetBoneKeyframesLen",
                     (OutsiderFuncType)GetBoneKeyframesLen);
  AssignOutsiderFunc(&lang_stat, "GetBoneKeyframesData",
                     (OutsiderFuncType)GetBoneKeyframesData);
  AssignOutsiderFunc(&lang_stat, "GetModelBonesLen",
                     (OutsiderFuncType)GetModelBonesLen);
  AssignOutsiderFunc(&lang_stat, "GetModelBonesRootsLen",
                     (OutsiderFuncType)GetModelBonesRootsLen);
  AssignOutsiderFunc(&lang_stat, "GetModelBonesRootsData",
                     (OutsiderFuncType)GetModelBonesRootsData);
  AssignOutsiderFunc(&lang_stat, "GetBoneMatrices",
                     (OutsiderFuncType)GetBoneMatrices);
  AssignOutsiderFunc(&lang_stat, "GetBoneName", (OutsiderFuncType)GetBoneName);
  AssignOutsiderFunc(&lang_stat, "ModelHasAnim",
                     (OutsiderFuncType)ModelHasAnim);

  AssignOutsiderFunc(&lang_stat, "CopyDataFromModel",
                     (OutsiderFuncType)CopyDataFromModel);
  AssignOutsiderFunc(&lang_stat, "GetInfoFromModel",
                     (OutsiderFuncType)GetInfoFromModel);
  AssignOutsiderFunc(&lang_stat, "InvertMatrix",
                     (OutsiderFuncType)InvertMatrix);

  AssignOutsiderFunc(&lang_stat, "SetCulling", (OutsiderFuncType)SetCulling);

  AssignOutsiderFunc(&lang_stat, "euler_to_quaternion2",
                     (OutsiderFuncType)euler_to_quaternion2);
  AssignOutsiderFunc(&lang_stat, "quat_mul2", (OutsiderFuncType)quat_mul2);
  AssignOutsiderFunc(&lang_stat, "AddMemoryWatch",
                     (OutsiderFuncType)AddMemoryWatch);
  AssignOutsiderFunc(&lang_stat, "PrintCallStack",
                     (OutsiderFuncType)PrintCallStack);
  lang_stat.cur_decl = 0;

  opts.wasm_dir = wasm_dir;
  opts.folder_name = folder_name;

  new (&sound) sound_state();
  sound.audio_clips_to_play.reserve(32);
  if (!opts.release) {
    long long args[] = {0};

    open_gl_state gl_state = {};
    memset(&gl_state, 0, sizeof(open_gl_state));
    gl_state.transparent_objs.reserve(32);
    gl_state.sound = &sound;
    gl_state.lang_stat = &lang_stat;

    AssignDbgFile(&lang_stat,
                  (opts.wasm_dir + opts.folder_name + ".dbg").c_str());
    // AssignDbgFile(&lang_stat, opts);
    lang_stat.winterp->dbg->data = (void *)&gl_state;
    RunDbgFunc(&lang_stat, "tests", args, 1);
    RunDbgFunc(&lang_stat, "main", args, 1);

    int ret_val = *(int *)&lang_stat.winterp->dbg->mem_buffer[RET_1_REG * 8];
  }
  ExitProcess(1);
  int a = 0;
}
