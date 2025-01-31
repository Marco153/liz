
enum key_enum
{
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
	_KEY_ESCAPE,
	_KEY_SPACE,
	_KEY_F1,
	_KEY_F5,
	_KEY_F9,
	_KEY_F10,
	_KEY_F11,
	_KEY_ENTER,
	_KEY_K,
};

#include <editor/TextEditor.cpp>
#include <glad/glad.h> 
#include <glad/glad.c> 
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "../include/GLFW/glfw3.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

struct v3
{
	float x;
	float y;
	float z;

	v3 mul(float m)
	{
		v3 ret;
		ret.x = x * m;
		ret.y = y * m;
		ret.z = z * m;
		return ret;
	}
	v3 operator *(float f)
	{
		v3 ret;
		ret.x = this->x * f;
		ret.y = this->y + f;
		return ret;
	}
	v3 operator -(v3& other)
	{
		v3 ret;
		ret.x = this->x - other.x;
		ret.y = this->y - other.y;
		return ret;
	}
	v3 operator +(v3& other)
	{
		v3 ret;
		ret.x = this->x + other.x;
		ret.y = this->y + other.y;
		return ret;
	}
	ImVec2 IM()
	{
		ImVec2 ret;
		ret.x = this->x;
		ret.y = this->y;
		return ret;
	}
	float dot(v3& other)
	{
		return x * other.x + y * other.y + z * other.y;
	}
	float len(v3& other)
	{
		return 1.0;
	}
};
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
#include "compile.cpp"
#include "memory.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> 
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h> 
#include <iostream>
#include <vector>
#include <sndfile.h>  // Library for reading WAV files
#include <dsound.h>
#include <xaudio2.h>
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"
#include <fstream>
#include "sort.cpp"


#define KEY_HELD 1
#define KEY_DOWN 2
#define KEY_UP   4
#define KEY_RECENTLY_DOWN   8
#define KEY_REPEAT   0x10
#define KEY_DOUBLE_CLICK   0x20
#define PI   3.141592

#define TOTAL_KEYS   (GLFW_KEY_LAST + 3)
#define TOTAL_TEXTURES   256

#define DOUBLE_CLICK_MAX_TIME 0.2

struct AudioClip;
struct sound_state;
void GetMem(dbg_state* dbg);

AudioClip* CreateNewAudioClip(char* name);

struct texture_info
{
	bool used;
	int id;
};
struct texture_raw
{
	char* name;
	unsigned char* data;
	int width;
	int height;
	char channels;
};
enum class buffer_type
{
	FILE,
};
struct Buffer
{
	buffer_type type;
	std::string name;
	std::string name_without_path;
	TextEditor* ed;
};
std::vector<Buffer> buffers;
struct open_gl_state;
struct WindowEditor
{
	bool on_cmd;
	Buffer* cur_buffer;
	Buffer* cmd_buffer;
	Buffer* prev_buffer;
	ImVec2 main_buffer_sz;
	own_std::vector<Buffer*>ed_buffers;
	open_gl_state* gl_state;
};

struct open_gl_state
{
	int vao;
	int shader_program;
	int shader_program_no_texture;
	int color_u;
	int tex_size;
	int tex_offset;
	int pos_u;
	int buttons[TOTAL_KEYS];
	float time_pressed[TOTAL_KEYS];
	texture_info textures[TOTAL_TEXTURES];
	own_std::vector<texture_raw> textures_raw;
	double last_time;

	YankBuffer yank[8];

	bool game_started;
	std::string texture_folder;
	bool is_engine;

	int width;
	int height;

	int scene_srceen_width;
	int scene_srceen_height;
	int frame_buffer;
	int frame_buffer_tex;

	int scroll;

	void* glfw_window;
	lang_state* lang_stat;
	sound_state* sound;

	float lmouse_click_timer;

	lang_state* lsp_lang_stat;
	mem_alloc* lsp_alloc;

	WindowEditor search_files_ed;
	WindowEditor main_ed;
	bool file_window;
	std::string  cur_dir;
	own_std::vector<char*>files;
	own_std::vector<char*>files_aux;

	bool suggestion_accepted;

	std::string  func_def_str;

	std::string lsp_dir_to_compile;

	float for_func_def_first_parentheses_pos_x;
	float for_func_def_first_parentheses_pos_y;

	int suggestion_cursor_line;
	int suggestion_cursor_column;
	int func_def_cursor_column;
	int suggestion_cursor_column_end;
	int selected_suggestion;
	own_std::vector<decl2> intellisense_suggestion;
	own_std::vector<RatedStuff<int>> intellisense_suggestion_aux;

	HANDLE lsp_process;
	HANDLE lsp_thread;
    HANDLE hStdInRead, hStdInWrite;
    HANDLE hStdOutRead, hStdOutWrite;

	HANDLE for_engine_game_process;
	HANDLE for_engine_game_thread;
	HANDLE for_engine_game_stdin;
	HANDLE for_engine_game_stdout;

	/*
	Buffer* cur_buffer;
	Buffer* cmd_buffer;
	own_std::vector<Buffer*>ed_buffers;
	*/
	own_std::vector<RatedStuff<int>> rated_files;
};

class XAudioClass;
typedef struct  WAV_HEADER {
	char                RIFF[4];        // RIFF Header      Magic header
	unsigned long       ChunkSize;      // RIFF Chunk Size  
	char                WAVE[4];        // WAVE Header      
	char                fmt[4];         // FMT header       
	unsigned long       Subchunk1Size;  // Size of the fmt chunk                                
	unsigned short      AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM 
	unsigned short      NumOfChan;      // Number of channels 1=Mono 2=Sterio                   
	unsigned long       SamplesPerSec;  // Sampling Frequency in Hz                             
	unsigned long       bytesPerSec;    // bytes per second 
	unsigned short      blockAlign;     // 2=16-bit mono, 4=16-bit stereo 
	unsigned short      bitsPerSample;  // Number of bits per sample      
	char                Subchunk2ID[4]; // "data"  string   
	unsigned long       Subchunk2Size;  // Sampled data length    

}wav_hdr;
#define AUDIO_CLIP_FLAGS_LOOP 1
struct AudioClip
{
	own_std::string name;
	own_std::vector<short> buffer;
	float time;
};
struct AudioClipQueued
{
	AudioClip* clip;
	unsigned int cur_idx;

	int flags;
};
struct sound_state
{
	int samples_per_sec = 44100;
	int samples_in_buffer = 1;
	int hz = 440;
	unsigned long long running_idx = 0;
	char* harmonics_buffer;
	IXAudio2SourceVoice* pSourceVoice;
	XAUDIO2_BUFFER buffers[2];
	unsigned char cur_buffer_to_submit = 0;
	XAudioClass *audio_class;
	own_std::vector<AudioClip *> audio_clips_src;
	own_std::vector<AudioClipQueued> audio_clips_to_play;

};


void AddAudioClipToPlay(sound_state* sound, AudioClip* clip)
{
	AudioClipQueued q = {};
	q.clip = clip;
	sound->audio_clips_to_play.emplace_back(q);
}
void FillBuffer(sound_state *sound, char* buffer, int total_samples_in_buffer, int bytes_per_sample, int hz_arg)
{
	int count = total_samples_in_buffer * bytes_per_sample;

	short* sample = (short*)buffer;
	int wave_period = sound->samples_per_sec / hz_arg;
	int idx_that_was_period_complete = -1;
	int volume = 3000;

	/*
	timer t;
	InitTimer(&t);
	StartTimer(&t);
	*/


	char b[256];

	for (int i = 0; i < count; i += bytes_per_sample)
	{

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


			float sine_h = harmonic_on ? sinf((last_sin * 2 * PI) * h_plus_one) : 0;

			float wave_period_max = (possible_harmonics * wave_period);
			float cur_h_wave_period = (wave_period * h_plus_one);

			short h_squared = (h_plus_one * h_plus_one);
			short new_harmonic_val = (short)((sine_h) * (float)(volume / h_squared));

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


class XAudioClass : public IXAudio2VoiceCallback
{
public :
	sound_state* sound;
	void XAudioClass::OnLoopEnd(void* data)
	{

		//FillBuffer((char *)data, 48000, 4, hz);
	}
	void XAudioClass::OnVoiceProcessingPassStart(UINT32)
	{
	}
	void XAudioClass::OnVoiceProcessingPassEnd(void)
	{
	}
	void XAudioClass::OnStreamEnd(void)
	{
	}
	void XAudioClass::OnBufferStart(void*)
	{
		HRESULT hr;
		sound->cur_buffer_to_submit++;
		sound->cur_buffer_to_submit %= 2;
		XAUDIO2_BUFFER* picked_buffer = &sound->buffers[sound->cur_buffer_to_submit];
		int total_sounds = sound->audio_clips_to_play.size();
		short* start = (short *)picked_buffer->pAudioData;
		memset(start, 0, picked_buffer->AudioBytes);
		FOR_VEC(it, sound->audio_clips_to_play)
		{
			start = (short *)picked_buffer->pAudioData;
			AudioClip* clip_ptr = it->clip;
			short* src_buffer = clip_ptr->buffer.data();
			if (it->cur_idx >= clip_ptr->buffer.size())
				continue;
			short* dbg = &src_buffer[it->cur_idx + 1];
			for (int i = 0; i < sound->samples_in_buffer * 4; i += 4)
			{
				float p = (float)it->cur_idx / (float)clip_ptr->buffer.size();
				*start += src_buffer[it->cur_idx];
				start++;
				*start += src_buffer[it->cur_idx + 1];
				start++;
				it->cur_idx += 2;
			}
		}
		int i = 0;
		FOR_VEC(it, sound->audio_clips_to_play)
		{
			AudioClip* clip_ptr = it->clip;
			if (it->cur_idx >= clip_ptr->buffer.size())
			{
				sound->audio_clips_to_play.remove(i);
			}
			i++;
		}
		///sound->audio_clips_to_play.
		//FillBuffer(sound, (char*)picked_buffer->pAudioData, sound->samples_in_buffer, 4, 440);
		if (FAILED(hr = sound->pSourceVoice->SubmitSourceBuffer(picked_buffer, nullptr)))
		{
			ASSERT(false);
		}


	}
	void XAudioClass::OnBufferEnd(void*)
	{
	}
	void XAudioClass::OnVoiceError(void*, HRESULT)
	{
	}
};

void Print(dbg_state* dbg)
{
	int base = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int mem_alloc_addr = *(int*)&dbg->mem_buffer[base + 8];

	int a = 0;


}
#define DRAW_INFO_HAS_TEXTURE 1
#define DRAW_INFO_NO_SCREEN_RATIO 2
#define DRAW_INFO_LINE 4
#define DRAW_INFO_STENCIL_WRITE 8
#define DRAW_INFO_STENCIL_TEST 16
#define DRAW_INFO_DISABLE_WRITING_TO_COLOR_BUFFER 32
enum class stencil_func
{
	EQUAL,
	NEQUAL,
};
struct draw_info
{
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

int GetTextureSlotId(open_gl_state* gl_state)
{
	for (int i = 0; i < TOTAL_TEXTURES; i++)
	{
		texture_info* t = &gl_state->textures[i];
		if (!t->used)
		{
			t->used = true;
			return i;
		}
	}
	ASSERT(0);
	return -1;
}
#define GL_CALL(call) call; if(glGetError() != GL_NO_ERROR) {printf("gl error %d", glGetError()); ASSERT(0)}
void Draw(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int draw_addr = *(int*)&dbg->mem_buffer[base_ptr + 8 * 2];


	auto wnd = (GLFWwindow*)*(long long*)&dbg->mem_buffer[base_ptr + 8];
	auto draw = (draw_info*)(long long*)&dbg->mem_buffer[draw_addr];

	auto gl_state = (open_gl_state*)dbg->data;

	int prog = gl_state->shader_program;

	if (IS_FLAG_ON(draw->flags, DRAW_INFO_HAS_TEXTURE))
	{
		prog = gl_state->shader_program;
		glUseProgram(prog);
		gl_state->tex_size = glGetUniformLocation(prog, "tex_size");
		if (draw->tex_size_x == 0)
			draw->tex_size_x = 1.0;
		if (draw->tex_size_y == 0)
			draw->tex_size_y = 1.0;
		glUniform2f(gl_state->tex_size, draw->tex_size_x, draw->tex_size_y);
		//gl_state->tex_offset = glGetUniformLocation(prog, "tex_offset");
		ASSERT(draw->texture_id < TOTAL_TEXTURES);
		//draw->flags &= ~DRAW_INFO_HAS_TEXTURE;
		texture_info* t = &gl_state->textures[draw->texture_id];

		glBindTexture(GL_TEXTURE_2D, t->id);
	}
	else
	{
		prog = gl_state->shader_program_no_texture;
		//glDisable(GL_TEXTURE_2D);
	}

	glUseProgram(prog);
	glBindVertexArray(gl_state->vao);

	gl_state->color_u = glGetUniformLocation(prog, "color");
	glUniform4f(gl_state->color_u, draw->color_r, draw->color_b, draw->color_g, draw->color_a);

	gl_state->pos_u = glGetUniformLocation(prog, "pos");
	glUniform3f(gl_state->pos_u, draw->pos_x, draw->pos_y, draw->pos_z);

	int pivot_u = glGetUniformLocation(prog, "pivot");
	glUniform3f(pivot_u, draw->pivot_x, draw->pivot_y, draw->pivot_z);

	int cam_size_u = glGetUniformLocation(prog, "cam_size");
	glUniform1f(cam_size_u, draw->cam_size);

	float screen_ratio = (float)gl_state->height / (float)gl_state->width;
	int screen_ratio_u = glGetUniformLocation(prog, "screen_ratio");

	if (gl_state->is_engine)
	{
		screen_ratio = (float)gl_state->scene_srceen_height / (float)gl_state->scene_srceen_width;
	}
	if (IS_FLAG_ON(draw->flags, DRAW_INFO_NO_SCREEN_RATIO))
	{
		screen_ratio = 1;
	}
	glUniform1f(screen_ratio_u, screen_ratio);

	float cam_pos_x = 0;
	float cam_pos_y = 0;
	float cam_pos_z = 0;
	if (draw->cam_pos_addr != 0)
	{
		cam_pos_x = *(float*)&dbg->mem_buffer[draw->cam_pos_addr];
		cam_pos_y = *(float*)&dbg->mem_buffer[draw->cam_pos_addr + 4];
		cam_pos_z = *(float*)&dbg->mem_buffer[draw->cam_pos_addr + 8];
	}
	int cam_pos_u = glGetUniformLocation(prog, "cam_pos");
	glUniform3f(cam_pos_u, cam_pos_x, cam_pos_y, cam_pos_z);

	float cam_rot_x = 0;
	float cam_rot_y = 0;
	float cam_rot_z = 0;
	if (draw->cam_rot_addr != 0)
	{
		cam_rot_x = *(float*)&dbg->mem_buffer[draw->cam_rot_addr];
		cam_rot_y = *(float*)&dbg->mem_buffer[draw->cam_rot_addr + 4];
		cam_rot_z = *(float*)&dbg->mem_buffer[draw->cam_rot_addr + 8];
	}
	int cam_rot_u = glGetUniformLocation(prog, "cam_rot");
	glUniform3f(cam_rot_u, cam_rot_x, cam_rot_y, cam_rot_z);

	int ent_rot_u = glGetUniformLocation(prog, "ent_rot");
	glUniform3f(ent_rot_u, draw->ent_rot_x, draw->ent_rot_y, draw->ent_rot_z);


	int ent_size_u = glGetUniformLocation(prog, "ent_size");
	glUniform3f(ent_size_u, draw->ent_size_x, draw->ent_size_y, draw->ent_size_z);

	if (gl_state->is_engine)
	{
		// Render to our framebuffer
		//glBindFramebuffer(GL_FRAMEBUFFER, gl_state->frame_buffer);
		//glViewport(0, 0, gl_state->scene_srceen_width, gl_state->scene_srceen_height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		glViewport(0, gl_state->height - gl_state->scene_srceen_height, gl_state->scene_srceen_width, gl_state->scene_srceen_height);
	}
	else
	{
		glViewport(0, 0, gl_state->width, gl_state->height);
	}

	glDisable(GL_STENCIL_TEST);
	glColorMask(true, true, true, true);
	glDepthMask(true);
	if (IS_FLAG_ON(draw->flags, DRAW_INFO_STENCIL_WRITE))
	{
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); 
		glStencilFunc(GL_ALWAYS, draw->stencil_val, 0xFF);
		glStencilMask(0xFF);
		//glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
		//glDisable(GL_STENCIL_TEST);

	}
	if (IS_FLAG_ON(draw->flags, DRAW_INFO_STENCIL_TEST))
	{
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); 
		switch (draw->stencil_func)
		{
		case stencil_func::EQUAL:
		{
			glStencilFunc(GL_EQUAL, draw->stencil_val, 0xFF);
		}break;
		case stencil_func::NEQUAL:
		{
			glStencilFunc(GL_NOTEQUAL, draw->stencil_val, 0xFF);
		}break;
		default:
			ASSERT(0);
		}
		//glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
		//glDisable(GL_STENCIL_TEST);

	}
	if (IS_FLAG_ON(draw->flags, DRAW_INFO_DISABLE_WRITING_TO_COLOR_BUFFER))
	{
		glColorMask(false, false, false, false);
		//glDepthMask(false);
	}
	//else
	//{
		if (IS_FLAG_ON(draw->flags, DRAW_INFO_LINE))
		{
			glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
		}
		else
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//}



	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//*(int*)&dbg->mem_buffer[RET_1_REG * 8] = glfwWindowShouldClose((GLFWwindow *)(long long)wnd);
}
void ClearBackground(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	void* addr = &dbg->mem_buffer[base_ptr + 8];
	float r = *(float*)&dbg->mem_buffer[base_ptr + 8];
	float g = *(float*)&dbg->mem_buffer[base_ptr + 8 * 2];
	float b = *(float*)&dbg->mem_buffer[base_ptr + 8 * 3];

	auto gl_state = (open_gl_state*)dbg->data;
	if (gl_state->is_engine)
	{
		//glViewport(0, 0, 1000, 1000);
		glClearColor(0, 0, 0, 1.0f); // Set the new color
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Step 2: Enable scissor testing
		glEnable(GL_SCISSOR_TEST);

		// Step 3: Define the area you want to clear with a different color
		glScissor(0, gl_state->height - gl_state->scene_srceen_height,
			gl_state->scene_srceen_width, gl_state->scene_srceen_height);
		glClearColor(r, g, b, 1.0f); // Set the new color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear only the scissor region

		// Step 4: Disable scissor testing (optional)
		glDisable(GL_SCISSOR_TEST);
		//*(int*)&dbg->mem_buffer[RET_1_REG * 8] = glfwWindowShouldClose((GLFWwindow *)(long long)wnd);
	}
	else
	{
		//glViewport(0, 0, 1000, 1000);
		glClearColor(r, g, b, 1.0f); // Set the new color
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		/*
		*/
	}
}

int FromGameToGLFWKey(int in)
{
	int key;
	switch ((key_enum)in)
	{
	case _KEY_UP:
	{
		key = GLFW_KEY_W;
	}break;
	case _KEY_DOWN:
	{
		key = GLFW_KEY_S;
	}break;
	case _KEY_RIGHT:
	{
		key = GLFW_KEY_D;
	}break;
	case _KEY_ACT1:
	{
		key = GLFW_KEY_J;
	}break;
	case _KEY_ACT0:
	{
		key = GLFW_KEY_K;
	}break;
	case _KEY_JMP:
	{
		key = GLFW_KEY_SPACE;
	}break;
	case _KEY_DEL:
	{
		key = GLFW_KEY_DELETE;
	}break;
	case _KEY_F5:
	{
		key = GLFW_KEY_F5;
	}break;
	case _KEY_F9:
	{
		key = GLFW_KEY_F9;
	}break;
	case _KEY_F:
	{
		key = GLFW_KEY_F;
	}break;
	case _KEY_K:
	{
		key = GLFW_KEY_K;
	}break;
	case _KEY_ENTER:
	{
		key = GLFW_KEY_ENTER;
	}break;
	case _KEY_F1:
	{
		key = GLFW_KEY_F1;
	}break;
	case _KEY_F11:
	{
		key = GLFW_KEY_F11;
	}break;
	case _KEY_LCTRL:
	{
		key = GLFW_KEY_LEFT_CONTROL;
	}break;
	case _KEY_ALT:
	{
		key = GLFW_KEY_LEFT_ALT;
	}break;
	case _KEY_TAB:
	{
		key = GLFW_KEY_TAB;
	}break;
	case _KEY_SHIFT:
	{
		key = GLFW_KEY_LEFT_SHIFT;
	}break;
	case _KEY_A:
	{
		key = GLFW_KEY_A;
	}break;
	case _KEY_ESCAPE:
	{
		key = GLFW_KEY_ESCAPE;
	}break;
	case _KEY_SPACE:
	{
		key = GLFW_KEY_SPACE;
	}break;
	case _KEY_S:
	{
		key = GLFW_KEY_S;
	}break;
	case _KEY_D:
	{
		key = GLFW_KEY_D;
	}break;
	case _KEY_W:
	{
		key = GLFW_KEY_W;
	}break;
	case _KEY_E:
	{
		key = GLFW_KEY_E;
	}break;
	case _KEY_Q:
	{
		key = GLFW_KEY_Q;
	}break;
	case _KEY_LEFT:
	{
		key = GLFW_KEY_A;
	}break;
	default:
		ASSERT(0);
	}
	return key;
}

void IsMouseDoubleClick(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int mouse = *(int*)&dbg->mem_buffer[base_ptr + 8];

	auto gl_state = (open_gl_state*)dbg->data;
	GLFWwindow* window = (GLFWwindow*)gl_state->glfw_window;

	int state = 0;
	int* addr = (int*)&dbg->mem_buffer[RET_1_REG * 8];
	*addr = 0;
	if (mouse == 0)
	{
		if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST], KEY_DOUBLE_CLICK))
			*addr = 1;
	}
	else if (mouse == 1)
	{
		if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST + 1], KEY_DOUBLE_CLICK))
			*addr = 1;
	}
	else
		ASSERT(0)

}
void IsMouseDown(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int mouse = *(int*)&dbg->mem_buffer[base_ptr + 8];

	auto gl_state = (open_gl_state*)dbg->data;
	GLFWwindow* window = (GLFWwindow*)gl_state->glfw_window;

	int state = 0;
	int* addr = (int*)&dbg->mem_buffer[RET_1_REG * 8];
	*addr = 0;
	if (mouse == 0)
	{
		if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST], KEY_DOWN))
			*addr = 1;
	}
	else if (mouse == 1)
	{
		if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST + 1], KEY_DOWN))
			*addr = 1;
	}
	else
		ASSERT(0)


}void IsMouseUp(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int mouse = *(int*)&dbg->mem_buffer[base_ptr + 8];

	auto gl_state = (open_gl_state*)dbg->data;
	GLFWwindow* window = (GLFWwindow*)gl_state->glfw_window;

	int state = 0;
	int* addr = (int*)&dbg->mem_buffer[RET_1_REG * 8];
	*addr = 0;
	if (mouse == 0)
	{
		if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST], KEY_UP))
			*addr = 1;
	}
	else if (mouse == 1)
	{
		if (IS_FLAG_ON(gl_state->buttons[GLFW_KEY_LAST + 1], KEY_UP))
			*addr = 1;
	}
	else
		ASSERT(0)

}
void IsMouseHeld(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int mouse = *(int*)&dbg->mem_buffer[base_ptr + 8];

	auto gl_state = (open_gl_state*)dbg->data;
	GLFWwindow* window = (GLFWwindow*)gl_state->glfw_window;

	int state = 0;
	if(mouse == 0)
		state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	else if(mouse == 1)
		state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	else
		ASSERT(0)

	int* addr = (int*)&dbg->mem_buffer[RET_1_REG * 8];
	if (state == GLFW_PRESS)
	{
		* addr = 1;
	}
	else
		* addr = 0;
}
void IsKeyUp(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int key = *(int*)&dbg->mem_buffer[base_ptr + 8];

	auto gl_state = (open_gl_state*)dbg->data;

	key = FromGameToGLFWKey(key);
	int* addr = (int*)&dbg->mem_buffer[RET_1_REG * 8];

	if (IS_FLAG_ON(gl_state->buttons[key], KEY_UP))
	{
		*addr = 1;
	}
	else
		*addr = 0;

}
void IsKeyDown(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int key = *(int*)&dbg->mem_buffer[base_ptr + 8];

	auto gl_state = (open_gl_state*)dbg->data;

	key = FromGameToGLFWKey(key);
	int* addr = (int*)&dbg->mem_buffer[RET_1_REG * 8];

	if (IS_FLAG_ON(gl_state->buttons[key], KEY_DOWN) || IS_FLAG_ON(gl_state->buttons[key], KEY_RECENTLY_DOWN))
	{
		*addr = 1;
		gl_state->buttons[key] &= ~KEY_RECENTLY_DOWN;
		gl_state->buttons[key] = (gl_state->buttons[key] & 0xffff);
	}
	else
		*addr = 0;

}
bool IsKeyRepeat(void *data, int key)
{
	auto gl_state = (open_gl_state*)data;
	//key = FromGameToGLFWKey(key);

	if (IS_FLAG_ON(gl_state->buttons[key], KEY_DOWN | KEY_REPEAT))
	{
		return true;
	}
	return false;

}
bool IsKeyDown(void *data, key_enum keye)
{
	auto gl_state = (open_gl_state*)((dbg_state*)data)->data;
	//key = FromGameToGLFWKey(key);
	auto key = FromGameToGLFWKey(keye);

	if (IS_FLAG_ON(gl_state->buttons[key], KEY_DOWN))
	{
		return true;
	}
	return false;

}
void ImGuiCheckbox(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char *name = (char *)&dbg->mem_buffer[name_offset];
	int bool_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];
	auto bool_ptr = (bool*)&dbg->mem_buffer[bool_offset];
	ImGui::Checkbox(name, bool_ptr);
}
void ImGuiSetNextItemAllowOverlap(dbg_state* dbg)
{
	ImGui::SetNextItemAllowOverlap();
}
void ImGuiPopItemWidth(dbg_state* dbg)
{
	ImGui::PopItemWidth();
}
void ImGuiEnumCombo(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char *name = (char *)&dbg->mem_buffer[name_offset];

	int line = *(int*)&dbg->mem_buffer[base_ptr + 16];

	int var_addr_offset = *(int*)&dbg->mem_buffer[base_ptr + 24];
	int *var_addr = (int*)&dbg->mem_buffer[var_addr_offset];

	scope *scp = FindScpWithLine(dbg->cur_func, line);
	type2 dummy;
	decl2 *e = FindIdentifier(name, scp, &dummy);
	if (!e)
	{
		ImGui::Text("enum not found: %s", name);
		return;
	}
	ASSERT(e);
	*var_addr = clamp(*var_addr, 0, e->type.enum_names->size() - 1);

	if (*var_addr > 128 || *var_addr < 0)
	{
		ImGui::Text("value too high %d", *var_addr);
		return;
	}

	own_std::vector<char*>* ar = e->type.enum_names;
	if (ImGui::BeginCombo("type##obj_type", (*ar)[*var_addr]))
	{
		for (int i = 0; i < ar->size(); i++)
		{
			char* ptr = (*ar)[i];
			if (ImGui::Selectable(ptr))
				*var_addr = i;
		}
		ImGui::EndCombo();
	}

}
void ImGuiShowV3(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	auto v_offset = *(int *)&dbg->mem_buffer[base_ptr + 8];
	auto v = (v3*)&dbg->mem_buffer[v_offset];
	char buffer[128];
	snprintf(buffer, 128, "##%p", v);
	ImGui::DragFloat3(buffer, (float*)v, 0.5);
}
void ImGuiPushItemWidth(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	float w = *(float*)&dbg->mem_buffer[base_ptr + 8];
	ImGui::PushItemWidth(w);
}
void ImGuiSameLine(dbg_state* dbg)
{
	ImGui::SameLine();
}

void ImGuiGetCursorScreenPosY(dbg_state* dbg)
{
	float* addr = (float*)&dbg->mem_buffer[RET_1_REG * 8];
	*addr = ImGui::GetCursorScreenPos().y;
}
void ImGuiGetCursorScreenPosX(dbg_state* dbg)
{
	float* addr = (float*)&dbg->mem_buffer[RET_1_REG * 8];
	*addr = ImGui::GetCursorScreenPos().x;
}
void ImGuiGetCursorPosY(dbg_state* dbg)
{
	float* addr = (float*)&dbg->mem_buffer[RET_1_REG * 8];
	*addr = ImGui::GetCursorPosY();
}
void ImGuiGetCursorPosX(dbg_state* dbg)
{
	float* addr = (float*)&dbg->mem_buffer[RET_1_REG * 8];
	*addr = ImGui::GetCursorPosX();
}

void ImGuiSelectable(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char *name_str = (char *)&dbg->mem_buffer[name_offset];
	bool selected = *(bool*)&dbg->mem_buffer[base_ptr + 16];
	float w = *(float*)&dbg->mem_buffer[base_ptr + 24];
	float h = *(float*)&dbg->mem_buffer[base_ptr + 32];

	bool* addr = (bool*)&dbg->mem_buffer[RET_1_REG * 8];

	if (ImGui::Selectable(name_str, selected, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(w, h)))
		*addr = true;
	else
		*addr = false;

}

void ImGuiTreePop(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	ImGui::TreePop();
}

void ImGuiTreeNodeEx(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char *name_str = (char *)&dbg->mem_buffer[name_offset];
	
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow;
	bool ret = ImGui::TreeNodeEx(name_str, flag);

	*(bool*)&dbg->mem_buffer[RET_1_REG * 8] = ret;
}

void ImGuiHasFocus(dbg_state* dbg)
{
	auto& io = ImGui::GetIO(); 
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	if (io.WantCaptureMouse)
		*(bool*)&dbg->mem_buffer[RET_1_REG * 8] = true;
	else
		*(bool*)&dbg->mem_buffer[RET_1_REG * 8] = false;
}
void ImGuiAddRect(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int this_ptr = *(int*)&dbg->mem_buffer[base_ptr + 8];
	float min_x = *(float*)&dbg->mem_buffer[base_ptr + 16];
	float min_y = *(float*)&dbg->mem_buffer[base_ptr + 24];
	float max_x = *(float*)&dbg->mem_buffer[base_ptr + 32];
	float max_y = *(float*)&dbg->mem_buffer[base_ptr + 40];
	int col = *(int*)&dbg->mem_buffer[base_ptr + 48];
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRect(ImVec2(min_x, min_y), ImVec2(max_x, max_y), col);
}
std::string GetWorkDir(unit_file *file, lang_state* lang_stat)
{
	std::string work_dir = file->path;
	int last_bar = work_dir.find_last_of('/');
	work_dir = work_dir.substr(0, last_bar + 1);
	return work_dir;
}
void ImGuiSetWindowFontScale(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	float fsz = *(float*)&dbg->mem_buffer[base_ptr + 8];
	auto gl_state = (open_gl_state*)dbg->data;

	ImGui::SetWindowFontScale(fsz);

}
void GotoPrevBuffer(void* data)
{
	auto wnd = (WindowEditor*)data;
	if (!wnd->prev_buffer)
		return;
	Buffer* aux = wnd->prev_buffer;
	wnd->prev_buffer = wnd->cur_buffer;
	wnd->cur_buffer = aux;
	wnd->cur_buffer->ed->EnsureCursorVisible();
}
void GlobalClearSearchStringHighlight(void *data)
{
	auto wnd = (WindowEditor*)data;
	wnd->cur_buffer->ed->ClearSearchStringHighlight();
}
void GlobalExitCmdBuffer(void* data, bool restoreOriginalPos)
{
	auto wnd = (WindowEditor*)data;
	wnd->on_cmd = false;
	wnd->cmd_buffer->ed->haveKeyboardFocusAnyway = false;
	TextEditor* main_ed = wnd->gl_state->main_ed.cur_buffer->ed;
	main_ed->insertBuffer.clear();
	if (restoreOriginalPos)
	{
		main_ed->SetCursorPosition(main_ed->originalCPosBeforeSearchString);
		main_ed->matchedStrings.clear();
	}
}
void LspSendFolderToCompile(open_gl_state* lang_stat, HANDLE hStdInWrite, std::string folder);
void CheckLspProcess(lang_state* lang_stat, open_gl_state* gl_state);
void SaveFile(void *data, char* contents, int size, std::string *file_name)
{
	auto wnd = (WindowEditor*)data;
	WriteFileLang((char*)file_name->c_str(), contents, size);
	CheckLspProcess(wnd->gl_state->lang_stat, wnd->gl_state);
	LspSendFolderToCompile(wnd->gl_state,
					wnd->gl_state->hStdInWrite, wnd->gl_state->lsp_dir_to_compile);
}
bool OnIntellisenseSuggestions(void* data)
{
	auto wnd = (WindowEditor*)data;
	return wnd->gl_state->intellisense_suggestion_aux.size() > 0;
}
void MoveSelectedIllisenseSuggestions(void* data, int add, bool absolute)
{
	auto wnd = (WindowEditor*)data;
	int* selected = &wnd->gl_state->selected_suggestion;
	if (absolute)
		*selected = add;
	else
	{
		*selected += -add;
	}



	*selected = clamp(*selected, 0, wnd->gl_state->intellisense_suggestion.size() - 1);

}
void RenderFuncDef(void *data, float screen_x, float screen_y)
{
	auto wnd = (WindowEditor*)data;
	open_gl_state* gl_state = wnd->gl_state;
	int str_sz = wnd->gl_state->func_def_str.size();
	if (str_sz == 0)
		return;
	ImVec2 min, max;
	min.x = screen_x;
	min.y = screen_y + 50.0;

	max = min;
	//max.x += 100.0;
	max.y += 50.0;

	ImVec2 prev_cursor = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(min);

	const float fontSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#", nullptr, nullptr).x;

	//ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(100, 0, 100, 255));
	ImGui::BeginChild("func def", ImVec2(str_sz * fontSize, 40));


	char buffer[128];
	int real_i = 0;
	ImGui::PopStyleColor();
	ImGui::EndChild();
	ImGui::SetCursorScreenPos(prev_cursor);
	//draw_list->AddRectFilled(min, max, IM_COL32(50, 50, 50, 255));
}
void RenderIntellisenseSuggestions(void *data, float screen_x, float screen_y)
{
	auto wnd = (WindowEditor*)data;
	if (wnd->gl_state->intellisense_suggestion.size() == 0 && wnd->gl_state->func_def_str.size() == 0)
		return;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 wnd_pos = ImGui::GetWindowPos();
	ImVec2 wnd_sz = ImGui::GetWindowSize();

	float suggestion_height = 100.0;


	ImVec2 min, max;
	min.x = screen_x;
	if((screen_y + suggestion_height) >= (wnd_pos.y + wnd_sz.y))
	{
		min.y = screen_y -(suggestion_height);
	}
	else
		min.y = screen_y + 20.0;

	ImGui::SetCursorScreenPos(min);

	int str_sz = wnd->gl_state->func_def_str.size();
	float width = 200.0;
	const float fontSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#", nullptr, nullptr).x;
	if (str_sz != 0)
	{
		width = str_sz * fontSize;
	}

	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(100, 0, 100, 255));
	ImGui::BeginChild("int sug", ImVec2(width, suggestion_height));
	//draw_list->AddRectFilled(min, max, IM_COL32(50, 50, 50, 255));


	if (str_sz != 0)
	{
		ImGui::Text(wnd->gl_state->func_def_str.c_str());
		ImGui::Separator();
	}

	char buffer[128];
	int real_i = 0;
	//for(int i = wnd->gl_state->intellisense_suggestion.size() - 1; i>=0;i--)
	FOR_VEC(sel, wnd->gl_state->intellisense_suggestion_aux)
	{
		//RatedStuff<int>* sel = &wnd->gl_state->intellisense_suggestion_aux[i];
		decl2* d = &wnd->gl_state->intellisense_suggestion[sel->type];
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
void ClearIntellisenseSeggestion(void* data)
{
	auto wnd = (WindowEditor*)data;

	wnd->gl_state->intellisense_suggestion.clear();
	wnd->gl_state->intellisense_suggestion_aux.clear();
	//wnd->gl_state->func_def_str.clear();

}
void AcceptIntellisenseSeggestion(void* data)
{
	auto wnd = (WindowEditor*)data;
	TextEditor* ed = wnd->cur_buffer->ed;
	int start_line = wnd->gl_state->suggestion_cursor_line;
	int start_column = wnd->gl_state->suggestion_cursor_column + 3;
	int max_column = ed->GetLineMaxColumn(wnd->gl_state->suggestion_cursor_line);

	TextEditor::Coordinates coor(ed->mState.mCursorPosition);
	coor.mColumn = wnd->gl_state->suggestion_cursor_column;
	TextEditor::Coordinates start;
	TextEditor::Coordinates end;
	if(wnd->gl_state->suggestion_cursor_column >= max_column)
	{
		start = coor;
		start.mColumn = max_column;
	}
	else
	{
		start = ed->FindWordStart2(coor);
		end = ed->FindWordEnd(coor);
		ed->DeleteRange(start, end);
	}


	int name_idx = wnd->gl_state->selected_suggestion;
	int suggestion_idx = wnd->gl_state->intellisense_suggestion_aux[name_idx].type;
	std::string name = wnd->gl_state->intellisense_suggestion[suggestion_idx].name;

	ed->InsertTextAt(start, name.c_str());

	ed->MoveRight(name.size());

	wnd->gl_state->intellisense_suggestion.clear();
	wnd->gl_state->intellisense_suggestion_aux.clear();

}
bool GlobalIsCurCmdBuffer(void* data)
{
	auto wnd = (WindowEditor*)data;
	//wnd->on_cmd = false;
	return wnd->on_cmd;
}
void GlobalGetFileName(void *data, std::string *out)
{
	auto wnd = (WindowEditor*)data;
	*out = wnd->cur_buffer->name;
}
void GlobalChangeToCmdBuffer(void *data)
{
	auto wnd = (WindowEditor*)data;
	wnd->on_cmd = true;
	wnd->cmd_buffer->ed->mVimMode = VI_INSERT;
	wnd->cmd_buffer->ed->ClearLines();
}

void GlobalGetCurBufferFileLines(void *data, std::string *file)
{
	auto wnd = (WindowEditor*)data;
	//gl_state->cur_buffer = gl_state->cmd_buffer;

	std::vector<std::string> txt = wnd->cur_buffer->ed->GetTextLines();
	FOR_VEC(str, txt)
	{
		*file += *str;
		*file += "\r\n";
	}
}
bool IsKeyHeld(dbg_state* dbg, key_enum keye)
{
	auto gl_state = (open_gl_state*)dbg->data;
	auto key = FromGameToGLFWKey(keye);
	if (IS_FLAG_ON(gl_state->buttons[key], KEY_HELD))
	{
		return true;
	}
	return false;

}
float FuzzyMatch(const std::string& to_match, const std::string& src)
{
    float score = 0.0f;

    // Match exact characters at the same positions
    for (size_t i = 0; i < to_match.size() && i < src.size(); ++i)
    {
        if (to_match[i] == src[i])
            score += 2.0f;
    }

    // Match characters at any position
    for (size_t i = 0; i < to_match.size(); ++i)
    {
        for (size_t j = 0; j < src.size(); ++j)
        {
            if (to_match[i] == src[j])
            {
                score += 0.5f;

                // Calculate neighboring indices with clamping
                size_t prev_idx_src = clamp(static_cast<int>(j) - 1, 0, static_cast<int>(src.size()) - 1);
                size_t next_idx_src = clamp(static_cast<int>(j) + 1, 0, static_cast<int>(src.size()) - 1);

                size_t prev_idx_match = clamp(static_cast<int>(i) - 1, 0, static_cast<int>(to_match.size()) - 1);
                size_t next_idx_match = clamp(static_cast<int>(i) + 1, 0, static_cast<int>(to_match.size()) - 1);

                float add = 0.0f;

                if (i != prev_idx_match && to_match[prev_idx_match] == src[prev_idx_src])
                    add += 1.0f;

                if (i != next_idx_match && to_match[next_idx_match] == src[next_idx_src])
                    add += 1.0f;

                score += add;
            }
        }
    }

    return score;
}
void ShowFileAndCmdBuffer(WindowEditor *wnd, int flags)
{
	bool focus_on_cmd = wnd->on_cmd;
	flags |= focus_on_cmd * TEXT_ED_DONT_HAVE_CURSOR_FOCUS;
	char buffer[32];
	ImGui::Text(wnd->cur_buffer->name_without_path.c_str());
	snprintf(buffer, 32, "editor##%p", wnd);
	wnd->cur_buffer->ed->Render(buffer, wnd->main_buffer_sz, flags);
	TextEditor* ed = wnd->cur_buffer->ed;
	VIM_mode_enum mode = wnd->cur_buffer->ed->mVimMode;
	if (mode == VI_NORMAL)
	{
		ImGui::TextColored(ImVec4(ImColor(255, 255, 0)), "NORMAL");
	}
	else if (mode == VI_VISUAL)
	{
		ImGui::TextColored(ImVec4(ImColor(255, 0, 0)), "VISUAL");
	}
	else if (mode == VI_INSERT)
	{
		ImGui::TextColored(ImVec4(ImColor(255, 255, 0)), "INSERT");
	}
	ImGui::SameLine();
	ImGui::Text("(%d:%d)", ed->mState.mCursorPosition.mLine + 1, ed->mState.mCursorPosition.mColumn + 1);
	ImGui::SameLine();
	ImGui::Text(ed->insertBuffer.c_str());



	bool isFirstCharacterSlash = wnd->cur_buffer->ed->firstChInInsertBufferIsSlash;
	flags = !focus_on_cmd * TEXT_ED_DONT_HAVE_CURSOR_FOCUS;
	if (isFirstCharacterSlash)
		wnd->cmd_buffer->ed->haveKeyboardFocusAnyway = true;

	snprintf(buffer, 32, "cmd##%p", wnd);
	wnd->cmd_buffer->ed->Render(buffer, ImVec2(0.0, 50.0), flags);
}

void ChangeFileOfBuffer(Buffer* b, std::string file_name)
{
	int read = 0;
	char* buffer = ReadEntireFileLang((char*)file_name.c_str(), &read);

	b->ed->SetText(buffer);
	heap_free((mem_alloc*)__lang_globals.data, buffer);

}

void LspPushStringIntoVector(std::string* str, own_std::vector<char>* out)
{
	int sz = str->size();

	int idx = out->size();
	out->make_count(out->size() + 4);
	*((int*)(out->data() + idx)) = sz;

	idx = out->size();
	out->make_count(out->size() + sz);
	memcpy(out->data() + idx, str->data(), sz);

}
Buffer* NewBuffer(dbg_state* dbg, WindowEditor* wnd)
{
	auto buf = (Buffer*)AllocMiscData(dbg->lang_stat, sizeof(Buffer));
	buf->ed = (TextEditor*)AllocMiscData(dbg->lang_stat, sizeof(TextEditor));
	new(buf->ed)TextEditor();
	buf->ed->data = (void*)wnd;

	auto gl_state = (open_gl_state*)dbg->data;
	ASSERT(gl_state);
	buf->ed->yank = &gl_state->yank[0];

	return buf;
}
Buffer* AddFileToBuffer(dbg_state* dbg, std::string file_name, WindowEditor* wnd)
{
	for (int i = 0; i < file_name.size(); i++)
	{
		if (file_name[i] == '/')
			file_name[i] = '\\';
	}
	TCHAR name_buffer[MAX_PATH];

	int error = GetLongPathName((char*)file_name.c_str(), name_buffer, MAX_PATH);
	if (error == 0)
	{
		printf("error in opening file buffer, code %d", GetLastError());
		ASSERT(0);
	}
	file_name = name_buffer;
	bool has_it = false;
	FOR_VEC(buf, wnd->ed_buffers)
	{
		Buffer* b = *buf;

		if (b->type == buffer_type::FILE && b->name == file_name)
		{
			has_it = true;
			return b;
		}
	}
	Buffer* buf = NewBuffer(dbg, wnd);

	TextEditor* ed = buf->ed;
	buf->type = buffer_type::FILE;

	buf->name = file_name;

	int last_bar = file_name.find_last_of("\\/");

	buf->name_without_path = file_name.substr(last_bar + 1);


	std::ifstream t(file_name);
	if (t.good())
	{
		std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		ed->SetText(str);
	}
	else
	{
		ASSERT(0);
	}

	wnd->ed_buffers.emplace_back(buf);
	return buf;
}

void LspGetFileSyntaxHightlighting(lang_state* lang_stat, std::string fname, open_gl_state* gl_state)
{
	lsp_header hdr;
	hdr.magic = 0x77;
	hdr.msg_type = lsp_msg_enum::LSP_SYNTAX;
	hdr.msg_len = sizeof(lsp_header) + fname.size() + 1;
	own_std::vector<char> buffer;
	char* cstr = (char*)fname.c_str();
	buffer.insert(buffer.end(), (char*)&hdr, (char*)(&hdr + 1));
	buffer.insert(buffer.end(), cstr, cstr + fname.size() + 1);

	Write(gl_state->hStdInWrite, buffer.data(), buffer.size());
}

void SetNewBuffer(WindowEditor* ed, Buffer* new_b)
{
	if(ed->cur_buffer != new_b)
		ed->prev_buffer = ed->cur_buffer;
	ed->cur_buffer = new_b;
}

void PrintGameStdOut(open_gl_state* gl_state)
{
	std::string str;
	CheckPipeAndGetString(gl_state->for_engine_game_stdout, str);
	printf("%s", str.c_str());
}

void GetMsgFromGame(void* data)
{
	auto gl_state = (open_gl_state*)data;
	if (!gl_state->game_started)
		return;
	std::string from_game_str;
	CheckPipeAndGetString(gl_state->for_engine_game_stdout, from_game_str);
	//if(from_game_str.size() > 0)
		//printf("msg from game: %s", from_game_str.c_str());

}

int LspCompile(lang_state* lang_stat, std::string folder, open_gl_state* gl_state, int line, int line_offset)
{
	DWORD bytesRead;
	DWORD availableBytes = 0;

	char read_buffer[1024];
	if (PeekNamedPipe(gl_state->hStdOutRead, NULL, 0, NULL, &availableBytes, NULL) && availableBytes > 0)
	{
		std::string final_str;
		int cur_read = 0;
		while (cur_read < availableBytes)
		{
			ReadFile(gl_state->hStdOutRead, read_buffer, sizeof(read_buffer) - 1, &bytesRead, NULL);
			read_buffer[bytesRead] = 0;
			cur_read += bytesRead;
			final_str += std::string(read_buffer, bytesRead);
		}
		char* aux_buffer = (char*)final_str.data();
		auto hdr = (lsp_header*)aux_buffer;
		switch (lang_stat->intentions_to_lsp)
		{
		case lsp_intention_enum::DECL_DEF_LINE:
		{
			char* line = (char*)(hdr + 1);
			gl_state->func_def_str = line;
		}break;
		case lsp_intention_enum::WAITING_FOLDER_TO_COMPILE:
		{
			if (hdr->msg_type == lsp_msg_enum::LSP_TASK_DONE)
			{

				lang_stat->intentions_to_lsp = lsp_intention_enum::SYNTAX;
				std::string fname = gl_state->main_ed.cur_buffer->name;
				LspGetFileSyntaxHightlighting(lang_stat, fname, gl_state);
			}
		}break;
		case lsp_intention_enum::SYNTAX:
		{
			if (hdr->msg_type == lsp_msg_enum::LSP_SYNTAX_RES)
			{
				Buffer* ed_buffer = gl_state->main_ed.cur_buffer;
				for (int i = 0; i < ed_buffer->ed->mLines.size(); i++)
				{
					int col = 0;
					ed_buffer->ed->ColorizeLine(i, col);
					/*
					FOR_VEC(gl, *cur_line)
					{
						gl->color = 0xffffffff;
					}
					*/
				}
				char* cur_ptr = (char*)(hdr + 1);
				auto syntax_hdr = (lsp_syntax_hightlight_hdr*)cur_ptr;
				long long offset = ((char*)cur_ptr) - ((char*)hdr);
				while (syntax_hdr->type != lsp_syntax_hightlight_enum::SEOF && offset < hdr->msg_len)
				{
					switch (syntax_hdr->type)
					{
					case lsp_syntax_hightlight_enum::WORD:
					{
						auto w = (lsp_syntax_hightlight_word*)syntax_hdr;
						auto* line = &ed_buffer->ed->mLines[w->line - 1];
						if (line->size() != 0)
						{
							if (w->column_end > line->size())
								return 0 ;

							for (int i = w->column_start; i < w->column_end; i++)
							{
								//int glyph_idx = ed_buffer->ed->GetCharacterIndex(TextEditor::Coordinates(w->line - 1, i));

								(*line)[i].color = w->color;
							}
						}
						cur_ptr = (char*)(w + 1);
					}break;
					default:
						ASSERT(false);
					}

					long long offset = ((char*)cur_ptr) - ((char*)hdr);
					syntax_hdr = (lsp_syntax_hightlight_hdr*)cur_ptr;
				}
			}
		}break;
		case lsp_intention_enum::GOTO_FUNC_DEF:
		{
			if (hdr->msg_type == lsp_msg_enum::LSP_GOTO_FUNC_RES)
			{
				int new_func_pos = *(int*)(hdr + 1);
				gl_state->main_ed.cur_buffer->ed->SetCursorPosition(
					TextEditor::Coordinates(new_func_pos, 0));
			}
		}break;
		case lsp_intention_enum::GOTO_DEF:
		{
			if (hdr->msg_type == lsp_msg_enum::LSP_GOTO_DEF_RES)
			{
				char* cur_ptr = (char*)(hdr + 1);
				auto gt_def = (goto_def*)cur_ptr;
				char* file_name = (char*)(gt_def + 1);

				Buffer* buf = AddFileToBuffer(lang_stat->dstate, file_name, &gl_state->main_ed);

				if (file_name != gl_state->main_ed.cmd_buffer->name)
				{
					SetNewBuffer(&gl_state->main_ed, buf);
					LspGetFileSyntaxHightlighting(lang_stat, file_name, gl_state);
					lang_stat->intentions_to_lsp = lsp_intention_enum::SYNTAX;
				}
				TextEditor* ed = gl_state->main_ed.cur_buffer->ed;
				ed->SetCursorPosition(TextEditor::Coordinates(gt_def->line.line - 1, gt_def->line.column));

			}
		}break;
		case lsp_intention_enum::INTELLISENSE:
		{
			if (hdr->msg_type == lsp_msg_enum::LSP_INTELLISENSE_RES)
			{
				char* cur_ptr = (char*)(hdr + 1);
				int total_decls = *(int*)(cur_ptr);
				cur_ptr += 4;
				char* str_tbl = cur_ptr + total_decls * sizeof(rel_type2);

				for (int i = 0; i < total_decls; i++)
				{
					rel_type2* r = ((rel_type2*)cur_ptr) + i;
					char* name = str_tbl + r->name;
					decl2 decl;
					gl_state->intellisense_suggestion.emplace_back(decl);
					decl2* back = &gl_state->intellisense_suggestion.back();
					back->name = std::string(name, r->name_len);
					back->type.type = (enum_type2)r->type;
					RatedStuff<int> rated;
					rated.type = i;
					gl_state->intellisense_suggestion_aux.emplace_back(rated);
				}


			}
		}break;
		}
		aux_buffer[bytesRead] = '\0'; // Null-terminate the string
		//std::cout << buffer;      // Output the captured data

	}
	return 0;
}

int CreateLspProcess(lang_state* lang_stat, open_gl_state* gl_state, std::string folder);
void CheckLspProcess(lang_state* lang_stat, open_gl_state* gl_state)
{
	DWORD code;
	GetExitCodeProcess(gl_state->lsp_process, &code);
	if (code != STILL_ACTIVE)
	{
		CreateLspProcess(lang_stat, gl_state, gl_state->lsp_dir_to_compile);
	}

}
void LspSendFolderToCompile(open_gl_state* gl_state, HANDLE hStdInWrite, std::string folder)
{
	lang_state* lang_stat = gl_state->lang_stat;
	own_std::vector<char> buffer;

	// 8 is for string sizes(folder name, and exe_dir)
	int offset_to_str = +8;
	buffer.make_count(sizeof(lsp_header));

	LspPushStringIntoVector(&folder, &buffer);
	LspPushStringIntoVector(&lang_stat->exe_dir, &buffer);
	auto hdr = (lsp_header*)buffer.data();
	hdr->magic = 0x77;
	hdr->msg_type = lsp_msg_enum::ADD_FOLDER;
	hdr->msg_len = buffer.size();

	DWORD bytesWritten;
	WriteFile(hStdInWrite, buffer.data(), buffer.size(), &bytesWritten, NULL);
	//WriteFileLang("mock.data", buffer.data(), buffer.size());
	lang_stat->intentions_to_lsp = lsp_intention_enum::WAITING_FOLDER_TO_COMPILE;

}

void StartGame(open_gl_state *gl_state, std::string game_dir)
{
	DWORD code;
	GetExitCodeProcess(gl_state->for_engine_game_process, &code);
	if (code == STILL_ACTIVE)
	{
		TerminateProcess(gl_state->for_engine_game_process, 0);
		CloseHandle(gl_state->for_engine_game_process);
		CloseHandle(gl_state->for_engine_game_thread);
	}
HANDLE hStdInRead, hStdInWrite;
HANDLE hStdOutRead, hStdOutWrite;
#define CREATE_STDIN 
#ifdef CREATE_STDIN
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

	// Create pipes for stdin and stdout

	if (!CreatePipe(&hStdInRead, &hStdInWrite, &sa, 0)) {
		std::cerr << "Failed to create stdin pipe.\n";
		return;
	}
	if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0)) {
		std::cerr << "Failed to create stdout pipe.\n";
		return;
	}

	// Ensure the write handle to stdin and read handle to stdout are not inherited
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
	std::string full_cmd = "E:/projects/WasmGame/lang2/build/liz.exe run ";
	full_cmd += game_dir;
	if (!CreateProcess(
		nullptr,
		(char*) full_cmd.c_str(), // Replace with your command
		NULL,
		NULL,
		TRUE, // Inherit handles
		0,
		NULL,
		NULL,
		&si,
		&pi)) {
			printf( "CreateProcess failed (%d).\n", GetLastError() );
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

int CreateLspProcess(lang_state* lang_stat, open_gl_state* gl_state, std::string folder)
{
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
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

	// Ensure the write handle to stdin and read handle to stdout are not inherited
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
		(LPSTR)"E:/projects/WasmGame/lang2/src/lsp/lsp.exe", // Replace with your command
		NULL,
		NULL,
		TRUE, // Inherit handles
		0,
		NULL,
		NULL,
		&si,
		&pi)) {
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
	//CloseHandle(hStdInWrite); // Close stdin write end after writing

	//CloseHandle(hStdOutRead);

	// Wait for the child process to finish
	/*
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Clean up
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	*/

	return 0;
}

void PushCStrIntoVector(own_std::vector<char>* out, char* ptr, int size)
{
	int offset = out->size();
	out->make_count(out->size() + size);
	memcpy(&(*out)[offset], ptr, size);
}
void LspSendLineStr(HANDLE hStdInWrite, lsp_msg_enum msg_type, int line, int column, std::string line_str, std::string file)
{
	to_lsp_linestr info;
	info.hdr.magic = 0x77;
	info.hdr.msg_type = msg_type;
	info.hdr.msg_len = sizeof(to_lsp_linestr) + line_str.size() + 1 + file.size() + 1;
	info.pos.line = line;
	info.pos.column = column;
	own_std::vector<char>buffer;
	PushCStrIntoVector(&buffer, (char*)line_str.c_str(), line_str.size() + 1);
	info.line_str_len = line_str.size() + 1;
	PushCStrIntoVector(&buffer, (char*)file.c_str(), file.size() + 1);
	buffer.insert(buffer.begin(), (char*)&info, (char*)(&info + 1));
	DWORD bytesWritten;
	WriteFile(hStdInWrite, buffer.data(), buffer.size(), &bytesWritten, NULL);
}

void LspSendCursorPosAndString(HANDLE hStdInWrite, lsp_msg_enum msg_type, int line, int column, std::string str)
{
	lsp_header hdr;
	hdr.magic = 0x77;
	hdr.msg_type = msg_type;
	hdr.msg_len = sizeof(lsp_header) + sizeof(lsp_pos) + str.size();
	lsp_pos int_info;
	int_info.column = column;
	int_info.line = line;


	own_std::vector<char>buffer;
	InsertIntoCharVector(&buffer, &hdr, sizeof(lsp_header));
	InsertIntoCharVector(&buffer, &int_info, sizeof(lsp_pos));
	InsertIntoCharVector(&buffer, (char*)str.data(), str.size() + 1);
	//InsertIntoCharVector(&buffer, (void*)word.c_str(), word.size() + 1);

	DWORD bytesWritten;
	WriteFile(hStdInWrite, buffer.data(), buffer.size(), &bytesWritten, NULL);
}


void ImGuiRenderTextEditor(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	auto gl_state = (open_gl_state*)dbg->data;


	ShowFileAndCmdBuffer(&gl_state->main_ed, 0);
	/*
	if (gl_state->func_def_str.size() > 0)
	{
		RenderFuncDef(&gl_state->main_ed, gl_state->for_func_def_first_parentheses_pos_x, gl_state->for_func_def_first_parentheses_pos_y);
	}
	*/

	TextEditor* ed = gl_state->main_ed.cur_buffer->ed;
	if (ed->IsTextChanged())
	{
		bool has_suggestions = gl_state->intellisense_suggestion.size() > 0;
		if (has_suggestions);
		{

			int start_line = gl_state->suggestion_cursor_line;
			int start_column = gl_state->suggestion_cursor_column;
			//start_column = ed->GetCharacterIndex(TextEditor::Coordinates(start_line, start_column));
			int end_line = gl_state->suggestion_cursor_line;
			int end_column = ed->GetLineMaxColumn(start_line);
			std::string line_str = ed->GetText(TextEditor::Coordinates(start_line, start_column),
				TextEditor::Coordinates(end_line, end_column));

			std::string out_str;
			GetWordStr((char*)line_str.data(), line_str.size(), 0, &out_str);

			gl_state->suggestion_cursor_column_end = start_column + out_str.size();

			own_std::vector<RatedStuff<int>>* aux_suggs = &gl_state->intellisense_suggestion_aux;
			aux_suggs->clear();
			memset(aux_suggs->data(), 0, aux_suggs->size() * sizeof(RatedStuff<int>));

			int i = 0;
			FOR_VEC(d, gl_state->intellisense_suggestion)
			{
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
		std::string word_under_cursor = ed->GetWordUnderCursor();
		ed->SetCursorPosition(prevPos);

		bool can_get_all_scp_vars = word_under_cursor.size() == 1 && IsLetter(word_under_cursor[0]) && !has_suggestions;
		if (ed->lastInsertedChar == ',' || ed->lastInsertedChar == ' ')
		{
			ClearIntellisenseSeggestion(&gl_state->main_ed);
		}
		if (ed->lastInsertedChar == '(')
		{
			/*
			int out_line, out_column;
			if(CheckMatchLevelsOfChar('(', coor.mLine, coor.mColumn, int *out_line, int *out_column)
			*/
			ed->MoveLeft(2);
			word_under_cursor = ed->GetWordUnderCursor();
			ed->MoveRight(2);

			gl_state->suggestion_cursor_line = coor.mLine;
			gl_state->func_def_cursor_column = coor.mColumn;


			ImVec2 cspos = ed->mCursorScreenPos;
			gl_state->for_func_def_first_parentheses_pos_x = cspos.x;
			gl_state->for_func_def_first_parentheses_pos_y = cspos.y;

			LspSendLineStr(gl_state->hStdInWrite, lsp_msg_enum::LSP_DECL_DEF_LINE, coor.mLine + 1,
				ed->GetCharacterIndex(coor), word_under_cursor, gl_state->main_ed.cur_buffer->name);

			gl_state->lang_stat->intentions_to_lsp = lsp_intention_enum::DECL_DEF_LINE;
			gl_state->intellisense_suggestion.clear();
			gl_state->intellisense_suggestion_aux.clear();

		}
		else if (ed->lastInsertedChar == '.' || can_get_all_scp_vars)
		{
			CheckLspProcess(dbg->lang_stat, gl_state);

			std::string line_str = ed->GetCurrentLineText();

			lsp_pos int_info;
			int_info.column = ed->GetCharacterIndex(coor) - 1;
			int_info.line = coor.mLine + 1;
			gl_state->suggestion_cursor_line = int_info.line - 1;
			if (can_get_all_scp_vars)
			{
				gl_state->suggestion_cursor_column = coor.mColumn - 1;
				gl_state->suggestion_cursor_column_end = int_info.column;
			}
			else
			{
				gl_state->suggestion_cursor_column = coor.mColumn;
				//gl_state->suggestion_cursor_column = int_info.column + 2;
				gl_state->suggestion_cursor_column_end = int_info.column + 3;
			}


			LspSendLineStr(gl_state->hStdInWrite, lsp_msg_enum::INTELLISENSE, coor.mLine + 1,
				ed->GetCharacterIndex(coor) - 1, line_str, gl_state->main_ed.cur_buffer->name);

			dbg->lang_stat->intentions_to_lsp = lsp_intention_enum::INTELLISENSE;
			gl_state->intellisense_suggestion.clear();
			gl_state->intellisense_suggestion_aux.clear();

		}
	}
	if (ed->gotoFuncSrcLine != 0)
	{
		own_std::vector<char> buffer;
		std::string *file_name = &gl_state->main_ed.cur_buffer->name;
		lsp_header hdr;
		hdr.magic = 0x77;
		hdr.msg_type = lsp_msg_enum::LSP_GOTO_FUNC_DEF;
		hdr.msg_len = sizeof(lsp_header) + sizeof(lsp_pos) + file_name->size() + 1;
		lsp_pos pos;
		pos.line = gl_state->main_ed.cur_buffer->ed->GetCursorPosition().mLine;
		buffer.insert(buffer.end(), (char*)&hdr, (char*)(&hdr + 1));
		buffer.insert(buffer.end(), (char*)&pos, (char*)(&pos + 1));

		char dir = 0;
		if (ed->gotoFuncSrcLine == 1)
		{
			dir = 1;
		}
		else if (ed->gotoFuncSrcLine == -1)
		{
			dir = -1;
		}
		else
		{
			ASSERT(0);
		}

		buffer.insert(buffer.end(), (char*)&dir, (char*)(&dir+ 1));
		PushCStrIntoVector(&buffer, (char*)file_name->c_str(), file_name->size() + 1);

		Write(gl_state->hStdInWrite, buffer.data(), buffer.size());
		ed->gotoFuncSrcLine = 0;
		gl_state->lang_stat->intentions_to_lsp = lsp_intention_enum::GOTO_FUNC_DEF;
		
	}
	if (ed->insertBuffer[0] == '/' &&
		gl_state->main_ed.cmd_buffer->ed->IsTextChanged())
	{
		std::string line_str = gl_state->main_ed.cmd_buffer->ed->GetCurrentLineText();

		if (line_str.size() > 0)
		{
			//line_str = line_str.substr(1);
			int line = 0;
			int column = 0;
			if (ed->SearchStringRange(line_str, &line, &column))
			{
			}
		}
	}
	if (ed->mState.mCursorPosition.mColumn < gl_state->func_def_cursor_column)
	{
		gl_state->func_def_str.clear();
	}
	if (ed->insertBuffer == "gd")
	{
		TextEditor::Coordinates coor = ed->FindWordEnd2(ed->mState.mCursorPosition);
		
		//std::string word = ed->GetWordUnderCursor();
		Buffer* cur_buffer = gl_state->main_ed.cur_buffer;
		std::string line_str = ed->GetCurrentLineText();
		own_std::vector<char> buffer;
		LspSendLineStr(gl_state->hStdInWrite, lsp_msg_enum::LSP_GOTO_DEF, coor.mLine + 1,
			ed->GetCharacterIndex(coor), line_str, cur_buffer->name);

		ed->insertBuffer.clear();
		dbg->lang_stat->intentions_to_lsp = lsp_intention_enum::GOTO_DEF;
	}
	if (dbg->lang_stat->intentions_to_lsp != lsp_intention_enum::PAUSED)
	{
		dbg->lang_stat->dstate = dbg;
		LspCompile(dbg->lang_stat, "../dev/engine/", gl_state, 0, 0);
	}

	/*
	if (IsKeyHeld(dbg, _KEY_LCTRL) && IsKeyDown(dbg, _KEY_F))
	{
		bool CheckMatchLevelsOfChar(char target_ch, int line, int column, int *out_line, int *out_column)
	}
	*/
	if (IsKeyHeld(dbg, _KEY_SHIFT) && IsKeyDown(dbg, _KEY_SPACE))
	{
		gl_state->file_window = !gl_state->file_window;
		FOR_VEC(s, gl_state->files)
		{
			heap_free((mem_alloc*)__lang_globals.data, *s);
		}
		gl_state->files.clear();
		WindowEditor* files_ed = &gl_state->search_files_ed;
		//files_ed->cur_buffer->ed->SetReadOnly(true);
		GetFilesInDirectory(gl_state->cur_dir, nullptr, &gl_state->files, 
			GET_FILES_DIR_ADD_PATH_TO_FILE_NAME | GET_FILES_DIR_RECURSIVE);

		files_ed->cmd_buffer->ed->ClearLines();
		files_ed->cur_buffer->ed->ClearLines();
		files_ed->on_cmd = true;
		//files_ed->cur_buffer->ed->InsertText()
		files_ed->cmd_buffer->ed->mVimMode = VI_INSERT;

		std::string s;
		FOR_VEC(str, gl_state->files)
		{
			s += *str;
			s += "\r\n";
		}
		files_ed->cur_buffer->ed->InsertText(s);

	}
	if(gl_state->file_window)
	{
		bool val = true;
		gl_state->rated_files.make_count(gl_state->files.size());
		WindowEditor* files_ed = &gl_state->search_files_ed;
		if (files_ed->cmd_buffer->ed->IsTextChanged())
		{
			gl_state->files_aux.clear();
			gl_state->rated_files.clear();
			files_ed->cur_buffer->ed->ClearLines();
			int i = 0;
			std::string line = files_ed->cmd_buffer->ed->GetCurrentLineText();

			FOR_VEC(f_str, gl_state->files)
			{
				RatedStuff<int> rated;
				rated.type = i;

				rated.val = FuzzyMatch(line, *f_str);
				gl_state->rated_files.emplace_back(rated);
				i++;
			}
			SortRatedStuff(&gl_state->rated_files);

			std::string s;
			FOR_VEC(r, gl_state->rated_files)
			{
				s += gl_state->files[r->type];
				s += "\r\n";
			}
			files_ed->cur_buffer->ed->InsertText(s);
			files_ed->cur_buffer->ed->MoveUp(1);
		}

		ImGui::Begin("file window", &val);
		ShowFileAndCmdBuffer(&gl_state->search_files_ed, TEXT_ED_ALLOW_ARRAW_NAVIGATION_EVEN_WHEN_NOT_FOCUS);
		ImGui::End();

		if (IsKeyDown(dbg, _KEY_ENTER))
		{
			std::string file_name = files_ed->cur_buffer->ed->GetCurrentLineText();
			if (file_name.empty())
				return;
			int bar = file_name.find_last_of("\\/");
			Buffer* buf = AddFileToBuffer(dbg, file_name, &gl_state->main_ed);
			SetNewBuffer(&gl_state->main_ed, buf);
			gl_state->file_window = false;
		}
	}
	if (IsKeyDown(dbg, _KEY_F1))
	{
		StartGame(gl_state, "../dev/files");
	}
	if (IsKeyDown(dbg, _KEY_F9))
	{

		int cline = ed->GetCursorPosition().mLine + 1;
		engine_msg_break msg;
		own_std::vector<char> buffer;
		msg.msg.type = engine_msg_enum::ADD_BREAK_POINT;
		msg.line = cline;
		if (ed->HasBreakpoint(cline))
		{
			ed->RemoveBreakpoint(cline);
			msg.add = false;
		}
		else
		{
			ed->SetBreakpoint(cline);
			msg.add = true;
		}
		
		buffer.insert(buffer.end(), (char*)&msg, (char*)(&msg + 1));
		std::string *s = &gl_state->main_ed.cur_buffer->name;
		PushCStrIntoVector(&buffer, (char*)s->data(), s->size());
		Write(gl_state->for_engine_game_stdin, (char*)buffer.data(), buffer.size());
	}
	//ed->Render("editor", ImVec2(0.0, 500.0);
}


void InitWindowEditor(dbg_state* dbg, open_gl_state *gl_state, WindowEditor* ed)
{
	ed->cmd_buffer = NewBuffer(dbg, ed);
	ed->cur_buffer = NewBuffer(dbg, ed);
	ed->gl_state = gl_state;
	
}

void GoBackOneDir(std::string* dir)
{
	dir->pop_back();
	int last_bar = dir->find_last_of("/\\");
	dir->erase(last_bar, -1);
	*dir += '\\';
}

void ImGuiInputInt(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int label_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int var_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];

	char* label = (char *)&dbg->mem_buffer[label_offset];
	int* var_addr = (int *)&dbg->mem_buffer[var_offset];
	ImGui::InputInt(label, var_addr);
}
void ImGuiInputText(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int label_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int buf_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];
	int buf_sz = *(int*)&dbg->mem_buffer[base_ptr + 24];

	char* label = (char *)&dbg->mem_buffer[label_offset];
	if (std::string(label) == "scene_name")
		auto a = 0;
	char* buf = (char *)&dbg->mem_buffer[buf_offset];
	ImGui::InputText(label, buf, buf_sz);
}
void ImGuiInitTextEditor(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char *name = (char*)&dbg->mem_buffer[name_offset];
	auto gl_state = (open_gl_state*)dbg->data;
	
	gl_state->lsp_alloc = (mem_alloc *)AllocMiscData(dbg->lang_stat, sizeof(mem_alloc));
	gl_state->lsp_lang_stat = (lang_state *)AllocMiscData(dbg->lang_stat, sizeof(lang_state));
	new(gl_state->lsp_alloc)mem_alloc();
	dbg->lang_stat->intentions_to_lsp = lsp_intention_enum::INTELLISENSE;
	dbg->lang_stat->intention_state = 0;

	void* prev_alloc = __lang_globals.data;

	InitMemAlloc(gl_state->lsp_alloc);

	InitLang(gl_state->lsp_lang_stat, (AllocTypeFunc)heap_alloc, (FreeTypeFunc)heap_free, gl_state->lsp_alloc);
	//LspCompile(gl_state->lsp_lang_stat, "../dev/engine/", gl_state);
	__lang_globals.data = prev_alloc;

	InitWindowEditor(dbg, gl_state, &gl_state->main_ed);
	InitWindowEditor(dbg, gl_state, &gl_state->search_files_ed);
	gl_state->search_files_ed.main_buffer_sz.x = 0.0;
	gl_state->search_files_ed.main_buffer_sz.y = 200.0;
	gl_state->main_ed.main_buffer_sz.x = 0.0;
	gl_state->main_ed.main_buffer_sz.y = 500.0;
	Buffer* buf = gl_state->main_ed.cur_buffer;


	std::string dir = dbg->cur_func->from_file->path;
	GoBackOneDir(&dir);
	gl_state->cur_dir = dir;
	dir += name;
	TextEditor* ed = buf->ed;
	buf->type = buffer_type::FILE;
	buf->name = dir;

	gl_state->lsp_dir_to_compile = "../dev/files";

	
	gl_state->main_ed.cur_buffer = AddFileToBuffer(dbg, dir, &gl_state->main_ed);


	std::ifstream t(dir);
	if (t.good())
	{
		std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		ed->SetText(str);
	}
	else
	{
		ASSERT(0);
	}

	gl_state->main_ed.ed_buffers.emplace_back(buf);

	CreateLspProcess(dbg->lang_stat, gl_state, gl_state->lsp_dir_to_compile);

}
void ImGuiImage(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int id = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int sz_x = (int)*(float*)&dbg->mem_buffer[base_ptr + 16];
	int sz_y = (int)*(float*)&dbg->mem_buffer[base_ptr + 24];
	ImGuiIO& io = ImGui::GetIO();

	ImTextureID my_tex_id = io.Fonts->TexID;
	//id = my_tex_id;

	auto gl_state = (open_gl_state*)dbg->data;
	texture_info* t = &gl_state->textures[id];
	ImGui::Image((ImTextureID)(intptr_t)t->id, ImVec2(sz_x, sz_y), ImVec2(0, 1), ImVec2(1, 0));

}
void ImGuiEnd(dbg_state* dbg)
{
	ImGui::End();

}
void ImGuiBegin(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char *name_str = (char *)&dbg->mem_buffer[name_offset];

	int bool_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];
	bool *bool_ptr = (bool*)&dbg->mem_buffer[bool_offset];
	int flags = *(int*)&dbg->mem_buffer[base_ptr + 24];
	ImGui::Begin(name_str, bool_ptr, flags);
}
void ImGuiEndChild(dbg_state* dbg)
{
	ImGui::EndChild();

}
void ImGuiBeginChild(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	float sz_x = *(float*)&dbg->mem_buffer[base_ptr + 16];
	float sz_y = *(float*)&dbg->mem_buffer[base_ptr + 24];
	char *name_str = (char *)&dbg->mem_buffer[name_offset];
	ImGui::BeginChild(name_str, ImVec2(sz_x, sz_y));
}

void ImGuiText(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char *name_str = (char *)&dbg->mem_buffer[name_offset];
	ImGui::Text(name_str);
}
void IsKeyHeld(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int key = *(int*)&dbg->mem_buffer[base_ptr + 8];

	auto gl_state = (open_gl_state*)dbg->data;

	key = FromGameToGLFWKey(key);

	int* addr = (int*)&dbg->mem_buffer[RET_1_REG * 8];

	if (IS_FLAG_ON(gl_state->buttons[key], KEY_HELD))
	{
		*addr = 1;
	}
	else
		*addr = 0;

}
void GetTime(dbg_state* dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;

	auto ret = (float*)&dbg->mem_buffer[RET_1_REG * 8];
	*ret = glfwGetTime();
}
void EndFrame(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int draw_addr = *(int*)&dbg->mem_buffer[base_ptr + 8 * 2];


	auto wnd = (GLFWwindow*)*(long long*)&dbg->mem_buffer[base_ptr + 8];
	auto gl_state = (open_gl_state*)dbg->data;
	//gl_state->last_time = glfwGetTime();
	ImGui::Render();
	//int display_w, display_h;
	//glfwGetFramebufferSize(window, &display_w, &display_h);
	//glViewport(0, 0, display_w, display_h);
	//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(wnd);
	gl_state->scroll = 0;
}
void ClearKeys(void *data)
{
	auto gl_state = (open_gl_state*)data;
	for (int i = 0; i < TOTAL_KEYS; i++)
	{
		int retain_flags = gl_state->buttons[i] & KEY_HELD;
		gl_state->buttons[i] &= ~(KEY_DOWN | KEY_UP | KEY_REPEAT | KEY_DOUBLE_CLICK);
		gl_state->buttons[i] |= retain_flags;

		if (IS_FLAG_ON(gl_state->buttons[i], KEY_RECENTLY_DOWN))
		{
			unsigned short held_from = (unsigned short)(gl_state->buttons[i] >> 16);
			held_from++;
			if (held_from > 24)
			{
				gl_state->buttons[i] &= ~KEY_RECENTLY_DOWN;
				gl_state->buttons[i] = (gl_state->buttons[i] & 0xffff);
			}
			else
			{
				gl_state->buttons[i] = (gl_state->buttons[i] & 0xffff) | (held_from << 16);
			}
		}
	}

}
void ShouldClose(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	long long wnd = *(long long*)&dbg->mem_buffer[base_ptr + 8];

	*(int*)&dbg->mem_buffer[RET_1_REG * 8] = glfwWindowShouldClose((GLFWwindow*)(long long)wnd);



	auto gl_state = (open_gl_state*)dbg->data;

	ClearKeys(gl_state);

	dbg->frame_is_from_dbg = false;

	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

// Callback function for window close event
void window_close_callback(GLFWwindow* window) 
{
    std::cout << "Window is about to close!" << std::endl;
	auto gl_state = (open_gl_state*)glfwGetWindowUserPointer(window);
	if (gl_state->is_engine)
	{
		TerminateProcess(gl_state->for_engine_game_process, 0);
		TerminateProcess(gl_state->lsp_process, 0);
		ExitProcess(1);
	}

}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto gl_state = (open_gl_state*)glfwGetWindowUserPointer(window);
	gl_state->scroll = yoffset;
}

void MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto gl_state = (open_gl_state*)glfwGetWindowUserPointer(window);
	int mouse_key = GLFW_KEY_LAST + button;
	if (action == GLFW_PRESS)
	{

		gl_state->buttons[mouse_key] = KEY_HELD | KEY_DOWN | KEY_RECENTLY_DOWN;
		float t = glfwGetTime();

		if ((t - gl_state->time_pressed[mouse_key]) < DOUBLE_CLICK_MAX_TIME)
			gl_state->buttons[mouse_key] |= KEY_DOUBLE_CLICK;

		gl_state->time_pressed[mouse_key] = t;
		//printf("key(%d) is %d", key, gl_state->buttons[key]);
	}
	else if (action == GLFW_RELEASE)
	{
		gl_state->buttons[mouse_key] &= ~KEY_HELD;
		gl_state->buttons[mouse_key] |= KEY_UP;
	}
	else if (action == GLFW_REPEAT)
	{
		gl_state->buttons[GLFW_KEY_LAST + button] |= KEY_REPEAT;
	}
}
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto gl_state = (open_gl_state*)glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS)
	{
		gl_state->buttons[key] |= KEY_HELD | KEY_DOWN | KEY_RECENTLY_DOWN;
		float t = glfwGetTime();
		if ((t - gl_state->time_pressed[key]) < DOUBLE_CLICK_MAX_TIME)
			gl_state->buttons[key] |= KEY_DOUBLE_CLICK;

		gl_state->time_pressed[key] = t;
		//printf("key(%d) is %d", key, gl_state->buttons[key]);
	}
	else if (action == GLFW_RELEASE)
	{
		gl_state->buttons[key] &= ~KEY_HELD;
		gl_state->buttons[key] |= KEY_UP;
		//printf("key release(%d) is %d", key, gl_state->buttons[key]);
	}
	else if (action == GLFW_REPEAT)
	{
		gl_state->buttons[key] |= KEY_REPEAT;
	}
}
struct clip
{
	unsigned int* texs_idxs;
	unsigned int total_texs;
	unsigned int id;
	float len;
	float cur_time;
	bool loop;
};
struct load_clip_args
{
	unsigned char* file_name;
	unsigned long long x_offset;
	unsigned long long y_offset;
	unsigned long long sp_width;
	unsigned long long sp_height;
	unsigned long long total_sps;
	float len;
	clip* cinfo;
};
texture_raw* HasRawTexture(open_gl_state* gl_state, std::string name)
{
	FOR_VEC(tex, gl_state->textures_raw)
	{
		if (std::string(tex->name) == name)
		{
			return tex;
		}
	}
	int width, height, nrChannels;
	unsigned char* src = nullptr;
	stbi_set_flip_vertically_on_load(true);
	src = stbi_load((char*)(gl_state->texture_folder + name).c_str(), &width, &height, &nrChannels, 0);
	ASSERT(src);
	gl_state->textures_raw.emplace_back(texture_raw());
	texture_raw* new_tex = &gl_state->textures_raw.back();
	//new_tex->name = "";
	new_tex->name = std_str_to_heap(gl_state->lang_stat, &name);
	new_tex->data = src;
	new_tex->width = width;
	new_tex->height = height;
	new_tex->channels = nrChannels;
	//ASSERT()
	//gl_state->textures_raw.emplace_back(new_tex);
	return new_tex;
}

void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
		abort();
	}
}

#ifdef _DEBUG
#define GL_CHECK(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
#define GL_CHECK(stmt) stmt
#endif
void UpdateTexture(dbg_state* dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int tex_id = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int x_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];
	int y_offset = *(int*)&dbg->mem_buffer[base_ptr + 24];
	int width = *(int*)&dbg->mem_buffer[base_ptr + 32];
	int height = *(int*)&dbg->mem_buffer[base_ptr + 40];
	int data = *(int*)&dbg->mem_buffer[base_ptr + 48];
	auto data_ptr = (char*)&dbg->mem_buffer[data];

	texture_info* t = &gl_state->textures[tex_id];

	glBindTexture(GL_TEXTURE_2D, t->id);
	//GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
	GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data_ptr));
	stbi_write_png("dbg_img.png", width, height, 4, data_ptr, width * 4);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData.data());



}
void CopyTextureToBuffer(dbg_state* dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int tex_id = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int buffer_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];
	int buffer_size = *(int*)&dbg->mem_buffer[base_ptr + 24];

	auto buffer_ptr = (char*)&dbg->mem_buffer[buffer_offset];

	texture_info* t = &gl_state->textures[tex_id];
	glBindTexture(GL_TEXTURE_2D, t->id);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	int width, height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	ASSERT((width * height * 4) <= buffer_size);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer_ptr);

	stbi_write_png("dbg_img.png", width, height, 4, buffer_ptr, width * 4);

}
int GenRawTexture(dbg_state* dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int sz_x = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int sz_y = *(int*)&dbg->mem_buffer[base_ptr + 16];
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	auto src = (unsigned char*)AllocMiscData(dbg->lang_stat, sz_x * sz_y * 4);
	//memset(src, 0xffffff, 4 * 512);
	//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sz_x, sz_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, src));
	//GL_CALL(glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0));

	//GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
	//stbi_write_png("dbg_img.png", width, height, 4, src, width * 4);
	int idx = GetTextureSlotId(gl_state);
	texture_info* tex = &gl_state->textures[idx];
	tex->id = texture;

	*(int*)&dbg->mem_buffer[RET_1_REG * 8] = idx;

	heap_free((mem_alloc*)__lang_globals.data, (char*)src);

	return idx;
}
int GenTexture2(lang_state* lang_stat, open_gl_state* gl_state, unsigned char* src, int width, int height)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, src));
	//GL_CALL(glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0));

	//GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
	//stbi_write_png("dbg_img.png", width, height, 4, src, width * 4);
	int idx = GetTextureSlotId(gl_state);
	texture_info* tex = &gl_state->textures[idx];
	tex->id = texture;

	//heap_free((mem_alloc*)__lang_globals.data, (char*)sp_data);

	return idx;
}
int GenTexture(lang_state* lang_stat, open_gl_state* gl_state, unsigned char* src, int sp_width, int sp_height, int x_offset, int y_offset, int width, int height, int sp_idx)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//int sp_height = info->sp_width;
	//int sp_width = info->sp_height;
	auto sp_data = (unsigned char*)AllocMiscData(lang_stat, sp_width * sp_height * 4);
	//int sp_idx = ;

	y_offset = (height - (y_offset + sp_height));

	for (int i = 0; i < sp_height; i++)
	{
		int cur_x_offset = (x_offset)+sp_idx * sp_width * 4;
		//int y_offset = sp_idx * sp_height;
		memcpy(sp_data + i * sp_width * 4, src + cur_x_offset + ((i + y_offset) * width * 4), sp_width * 4);
		int  a = 0;
	}
	if (sp_data)
	{
		GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sp_width, sp_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, sp_data));
		//GL_CALL(glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0));

		GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	//stbi_write_png("dbg_img.png", sp_width, sp_height, 4, sp_data, sp_width * 4);
	int idx = GetTextureSlotId(gl_state);
	texture_info* tex = &gl_state->textures[idx];
	tex->id = texture;

	heap_free((mem_alloc*)__lang_globals.data, (char*)sp_data);

	return idx;
}

void MaybeAddBarToEndOfStr(std::string* str)
{
	if (str->size() != 0 && (*str)[str->size() - 1] != '/' && (*str)[str->size() - 1] != '\\')
		(*str) += '/';

}
void CopyFromSrcImgToBuffer(char* src_img, char* buffer, int buffer_width, int buffer_height, int src_img_width)
{
	for (int y = 0; y < buffer_height; y++)
	{
		memcpy(buffer, src_img, buffer_width * 4);
		buffer += buffer_width * 4;
		src_img += src_img_width * 4;
	}
}
struct sheet_file_header
{
	u32 total_layers;
	u32 str_tbl_offset;
	u32 str_tbl_sz;
	u32 cell_info_size;
};
struct aux_layer_info_struct
{
	u64 version;
	u32 type;
	u32 pixels_per_width;

	v3 pos;
	v3 sz;
	u32 grid_x;
	u32 grid_y;
	u32 total_of_used_cells;
	u32 cell_sz;
	struct 
	{
		bool is_masked;
		u64 stencil_val;
	};
};

struct sp_cell
{
	u64 version;
	u64 tex_name;
	u32 grid_x;
	u32 grid_y;

	u32 src_tex_offset_x;
	u32 src_tex_offset_y;
	bool is_masked;
	u64 stencil_val;
};
struct aux_cell_info
{
	u64 version;
	union
	{
		struct
		{
			u64 tex_name;
			u32 grid_x;
			u32 grid_y;

			u32 src_tex_offset_x;
			u32 src_tex_offset_y;
		};
		struct
		{
			v3 pos;
			v3 sz;
		}col;
		struct
		{
			u32 type;
			u32 enemy_type;
			u64 add_info;
			v3 pos;
			v3 sz;
		}obj;
	};
	struct 
	{
		bool is_masked;
		u64 stencil_val;
	};
};

void LoadSheetFromLayer(dbg_state* dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;

	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int layer_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	auto cur_layer = (aux_layer_info_struct*)&dbg->mem_buffer[layer_offset];
	auto cur_cell = (sp_cell *)(cur_layer + 1);

	int str_tbl_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];
	auto str_tbl = (char*)&dbg->mem_buffer[str_tbl_offset];

	
	int tex_width = cur_layer->grid_x * cur_layer->pixels_per_width;
	int tex_height = cur_layer->grid_y * cur_layer->pixels_per_width;
	auto tex_data = (char *) AllocMiscData(dbg->lang_stat, tex_width * tex_height * 4);
	char px_width = cur_layer->pixels_per_width;
	char* aux_buffer = AllocMiscData(dbg->lang_stat, px_width * px_width * 4);
	//*tex_width = cur_layer->grid_x * cur_layer->pixels_per_width;
	//*tex_height = cur_layer->grid_y * cur_layer->pixels_per_width;
	//int sz = cur_layer->grid_x * px_width * cur_layer->grid_y * px_width;
	int tex_id = GenTexture2(dbg->lang_stat, gl_state, (u8*)tex_data, cur_layer->grid_x * px_width, cur_layer->grid_y * px_width);
	texture_info* t = &gl_state->textures[tex_id];
	glBindTexture(GL_TEXTURE_2D, t->id);

	for (int c = 0; c < cur_layer->total_of_used_cells; c++)
	{
		char* tex_name = str_tbl + cur_cell->tex_name;
		texture_raw* tex_src = HasRawTexture(gl_state, tex_name);

		int x_offset = cur_cell->src_tex_offset_x / px_width;
		int y_offset = cur_cell->src_tex_offset_y / px_width;
		auto data_ptr = tex_src->data + x_offset * px_width * 4 + y_offset * tex_src->width * 4 * px_width;
		CopyFromSrcImgToBuffer((char*)data_ptr, aux_buffer, px_width, px_width, tex_src->width);
		//GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0,
			cur_cell->grid_x * px_width,
			cur_cell->grid_y * px_width,
			px_width, px_width,
			GL_RGBA, GL_UNSIGNED_BYTE, aux_buffer)
		);

		cur_cell++;
	}
	heap_free((mem_alloc*)__lang_globals.data, (char*)aux_buffer);
	heap_free((mem_alloc*)__lang_globals.data, (char*)tex_data);

	*(u64*)&dbg->mem_buffer[RET_1_REG * 8] = tex_id;
}

int CreateSpriteFromLayer(lang_state *lang_stat, open_gl_state *gl_state, aux_layer_info_struct *cur_layer, aux_cell_info *cur_cell, char *str_table)
{
	int px_width = cur_layer->pixels_per_width;
	char* aux_buffer = AllocMiscData(lang_stat, px_width * px_width * 4);
	int tex_width = cur_layer->grid_x * cur_layer->pixels_per_width;
	int tex_height = cur_layer->grid_y * cur_layer->pixels_per_width;
	char *tex_data = (char *) AllocMiscData(lang_stat, tex_width * tex_height * 4);
	//int sz = cur_layer->grid_x * px_width * cur_layer->grid_y * px_width;
	int tex_id = GenTexture2(lang_stat, gl_state, (u8*)*tex_data, cur_layer->grid_x * px_width, cur_layer->grid_y * px_width);
	texture_info* t = &gl_state->textures[tex_id];
	glBindTexture(GL_TEXTURE_2D, t->id);

	for (int c = 0; c < cur_layer->total_of_used_cells; c++)
	{
		char* tex_name = str_table + cur_cell->tex_name;
		texture_raw* tex_src = HasRawTexture(gl_state, tex_name);

		int x_offset = cur_cell->src_tex_offset_x / px_width;
		int y_offset = cur_cell->src_tex_offset_y / px_width;
		auto data_ptr = tex_src->data + x_offset * px_width * 4 + y_offset * tex_src->width * 4 * px_width;
		CopyFromSrcImgToBuffer((char*)data_ptr, aux_buffer, px_width, px_width, tex_src->width);
		//GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
		GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0,
			cur_cell->grid_x * px_width,
			cur_cell->grid_y * px_width,
			px_width, px_width,
			GL_RGBA, GL_UNSIGNED_BYTE, aux_buffer)
		);

		cur_cell = (aux_cell_info *)(((char*)cur_cell) + cur_layer->cell_sz);
	}
	heap_free((mem_alloc*)__lang_globals.data, (char*)aux_buffer);
	return 1;

}

int LoadSpriteSheet(dbg_state* dbg, std::string sp_file_name, int *tex_width, int *tex_height, int *channels, char **tex_data)
{
	int read;
	char* file = ReadEntireFileLang((char *)sp_file_name.c_str(), &read);


	auto gl_state = (open_gl_state*)dbg->data;
	auto hdr = (sheet_file_header*)file;

	int cell_size = sizeof(aux_cell_info);
	if(hdr->cell_info_size != cell_size)
	{
		printf("error file %s: cell sizes different, on file sz is %d, but on compiler is %d", sp_file_name.c_str(), hdr->cell_info_size, cell_size);
		return -1;
	}
	char* str_table = (file + hdr->str_tbl_offset);
	
	own_std::vector<char*> image_sprite_names;

	u32 cur_ch = 0;

	while (cur_ch < hdr->str_tbl_sz)
	{
		u32 start = cur_ch;
		while (str_table[cur_ch] != 0)
		{
			cur_ch++;
		}
		image_sprite_names.emplace_back(&str_table[start]);
		cur_ch++;

	}

	char* cur_ptr = (char*)(hdr + 1);
	int tex_id = 0;
	auto cur_layer = (aux_layer_info_struct*)cur_ptr;
	auto cur_cell = (aux_cell_info *)(cur_layer + 1);
	for (int i = 0; i < hdr->total_layers; i++)
	{
		// sprites
		switch (cur_layer->type)
		{
		case 0:
		{
			if(cur_layer->cell_sz != sizeof(aux_layer_info_struct))
			{
				ASSERT(0);
			}

			int total_sprites = cur_layer->total_of_used_cells;
			cur_layer++;
			for (int j = 0; j < total_sprites; j++)
			{
				cur_cell = (aux_cell_info*)(cur_layer + 1);
				cur_cell = (aux_cell_info*)(((char*)cur_cell) + cur_layer->cell_sz * cur_layer->total_of_used_cells);
				cur_layer = (aux_layer_info_struct*)cur_cell;
			}
		}break;
		// colliders
		case 1:
		{
			for (int c = 0; c < cur_layer->total_of_used_cells; c++)
			{
				cur_cell = (aux_cell_info *)(((char*)cur_cell) + cur_layer->cell_sz);
			}
		}break;
		// objs
		case 2:
		{
			for (int c = 0; c < cur_layer->total_of_used_cells; c++)
			{
				cur_cell = (aux_cell_info *)(((char*)cur_cell) + cur_layer->cell_sz);
			}
		}break;
		default:
			printf("error file %s: layer type not known", sp_file_name.c_str(), cur_layer->type);
			return -1;
		}
		if(cur_layer->total_of_used_cells == 0)
		{
			cur_cell = (aux_cell_info*)(cur_layer + 1);
		}
		cur_ptr = (char*)cur_cell;
		cur_layer = (aux_layer_info_struct*)cur_cell;
		cur_cell = (aux_cell_info*)(cur_layer + 1);
	};
	int val = *(int*)cur_ptr;
	// end of layers
	if(val != 0xbebad0)
	{
		printf("error file %s: value check at end of layers not matching, expected 0x%04x, found 0x%04x", sp_file_name.c_str(), 0x1234, val);
		return -1;
	}
	return 1;
	cur_ptr = (char*)(hdr + 1);
	
	tex_id = 0;
	cur_layer = (aux_layer_info_struct*)cur_ptr;
	cur_cell = (aux_cell_info *)(cur_layer + 1);
	for (int i = 0; i < hdr->total_layers; i++)
	{
		// sprites
		switch(cur_layer->type)
		{
		case 0:
		{

			int total_sprites = cur_layer->total_of_used_cells;
			cur_layer++;
			for (int j = 0; j < total_sprites; j++)
			{
				auto cur_cell = (aux_cell_info*)(cur_layer + 1);
				CreateSpriteFromLayer(gl_state->lang_stat, gl_state, cur_layer, cur_cell, str_table);
				cur_cell = (aux_cell_info*)(cur_layer + 1);
				cur_cell = (aux_cell_info*)(((char*)cur_cell) + cur_layer->cell_sz * cur_layer->total_of_used_cells);
				cur_layer = (aux_layer_info_struct*)cur_cell;
			}
			/*
			int px_width = cur_layer->pixels_per_width;
			char* aux_buffer = AllocMiscData(dbg->lang_stat, px_width * px_width * 4);
			*tex_width = cur_layer->grid_x * cur_layer->pixels_per_width;
			*tex_height = cur_layer->grid_y * cur_layer->pixels_per_width;
			*tex_data = (char *) AllocMiscData(dbg->lang_stat, *tex_width * *tex_height * 4);
			//int sz = cur_layer->grid_x * px_width * cur_layer->grid_y * px_width;
			tex_id = GenTexture2(dbg->lang_stat, gl_state, (u8*)*tex_data, cur_layer->grid_x * px_width, cur_layer->grid_y * px_width);
			texture_info* t = &gl_state->textures[tex_id];
			glBindTexture(GL_TEXTURE_2D, t->id);

			for (int c = 0; c < cur_layer->total_of_used_cells; c++)
			{
				char* tex_name = str_table + cur_cell->tex_name;
				texture_raw* tex_src = HasRawTexture(gl_state, tex_name);

				int x_offset = cur_cell->src_tex_offset_x / px_width;
				int y_offset = cur_cell->src_tex_offset_y / px_width;
				auto data_ptr = tex_src->data + x_offset * px_width * 4 + y_offset * tex_src->width * 4 * px_width;
				CopyFromSrcImgToBuffer((char*)data_ptr, aux_buffer, px_width, px_width, tex_src->width);
				//GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
				GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0,
					cur_cell->grid_x * px_width,
					cur_cell->grid_y * px_width,
					px_width, px_width,
					GL_RGBA, GL_UNSIGNED_BYTE, aux_buffer)
				);

				cur_cell = (aux_cell_info *)(((char*)cur_cell) + cur_layer->cell_sz);
			}
			heap_free((mem_alloc*)__lang_globals.data, (char*)aux_buffer);
			*/
		}break;
		// colliders
		case 1:
		{
			for (int c = 0; c < cur_layer->total_of_used_cells; c++)
			{
				cur_cell = (aux_cell_info *)(((char*)cur_cell) + cur_layer->cell_sz);
			}
		}break;
		// objs
		case 2:
		{
			for (int c = 0; c < cur_layer->total_of_used_cells; c++)
			{
				cur_cell = (aux_cell_info *)(((char*)cur_cell) + cur_layer->cell_sz);
			}
		}break;
		default:
			ASSERT(false);
		}
		cur_ptr = (char*)cur_cell;
		cur_layer = (aux_layer_info_struct*)cur_cell;
		cur_cell = (aux_cell_info *)( cur_layer + 1);
	}
	return tex_id;
}


void ReadFileInterp(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char* name = (char*)&dbg->mem_buffer[name_offset];

	int buffer_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];
	char *buffer_ptr = (char*)&dbg->mem_buffer[buffer_offset];

	int size;
	std::string work_dir = dbg->cur_func->from_file->path;
	work_dir = work_dir + name;
	char *file = ReadEntireFileLang((char *)work_dir.c_str(), &size);
	memcpy(buffer_ptr, file, size);
}
void GetFileSize(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char* name = (char*)&dbg->mem_buffer[name_offset];


	LARGE_INTEGER file_size;
	std::string work_dir = dbg->cur_func->from_file->path;
	work_dir = work_dir + name;
	HANDLE file = OpenFileLang((char*)work_dir.c_str());
	BOOL val = GetFileSizeEx(file, &file_size);
	auto err = GetLastError();
	ASSERT(val != 0);

	*(u64*)&dbg->mem_buffer[RET_1_REG * 8] = file_size.QuadPart;
	CloseHandle(file);
}
int GetMem(dbg_state* dbg, int sz)
{
	*(int*)&dbg->mem_buffer[STACK_PTR_REG * 8] -= 16;
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	*(int*)&dbg->mem_buffer[base_ptr + 8] = sz;
	int idx = 0;

	GetMem(dbg);
	int offset = *(int*)&dbg->mem_buffer[RET_1_REG * 8];

	*(int*)&dbg->mem_buffer[STACK_PTR_REG * 8] += 16;
	return offset;
}
void LoadSceneFolder(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int folder_name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int ar_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];

	auto gl_state = (open_gl_state*)dbg->data;

	auto folder_name = (char *)&dbg->mem_buffer[folder_name_offset];

	auto ar = (own_std::vector<int> *)&dbg->mem_buffer[ar_offset];

	//new(&dbg->scene_folder)std::string(folder_name);
	dbg->scene_folder = dbg->cur_func->from_file->path + folder_name;
	//dbg->scene_folder = (const char *)folder_name;
	//MaybeAddBarToEndOfStr(&dbg->scene_folder);

	own_std::vector<char *> file_names;

	GetFilesInDirectory(dbg->scene_folder, nullptr, &file_names);

	int sz = 0;

	FOR_VEC(name, file_names)
	{
		sz += strlen(*name) + 1;
	}
	if (sz == 0)
		return;

	int str_tbl_offset = file_names.size() * 8;
	sz += str_tbl_offset;
	int offset = GetMem(dbg, sz);

	ar->ar.start = (int *)(long long)offset;
	ar->ar.count = file_names.size();
		 

	auto str_tbl_ptr = (char*)&dbg->mem_buffer[offset + str_tbl_offset];
	auto start_idx = (int*)&dbg->mem_buffer[offset];
	auto cur_idx = (long long*)&dbg->mem_buffer[offset];

	int fl_idx = 0;
	int cur_str_tbl_offset = 0;

	FOR_VEC(fl, file_names)
	{
		int ln = strlen(*fl) + 1;

		memcpy(str_tbl_ptr + cur_str_tbl_offset, *fl, ln);

		*cur_idx = offset + str_tbl_offset + cur_str_tbl_offset;
		cur_idx++;

		cur_str_tbl_offset += ln;
		fl_idx++;
	}
	int a = 0;


}
void LoadTexFolder(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int folder_name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int ar_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];

	auto folder_name = (char *)&dbg->mem_buffer[folder_name_offset];
	auto ar = (own_std::vector<int> *)&dbg->mem_buffer[ar_offset];

	auto gl_state = (open_gl_state*)dbg->data;
	gl_state->texture_folder = folder_name;
	if(!dbg->cur_func)
	{
		dbg->cur_func = GetFuncBasedOnBc2(dbg, *dbg->cur_bc2);
	}
	std::string work_dir = dbg->cur_func->from_file->path;
	//MaybeAddBarToEndOfStr(&work_dir);

	gl_state->texture_folder = work_dir + gl_state->texture_folder;
	MaybeAddBarToEndOfStr(&(gl_state->texture_folder));
	
	own_std::vector<char*> file_names;
	GetFilesInDirectory(gl_state->texture_folder, nullptr, &file_names);

	struct texture_info
	{
		u64 name;
		u64 data;
		u32 width;
		u32 height;
		u8 channels;
		u64 idx;
	};
	int total_pngs = 0;
	FOR_VEC(name_ptr, file_names)
	{
		char* name = *name_ptr;
		std::string str = name;
		int p_idx = str.find_last_of('.');
		std::string ext = str.substr(p_idx + 1);
		if (!(ext == "png" || ext == "sp"))
			continue;
		total_pngs++;
	}

	int offset = GetMem(dbg, total_pngs * sizeof(texture_info));

	ar->ar.start = (int *)(u64)offset;
	ar->ar.count = total_pngs;

	int i = 0;
	auto cur_tex = (texture_info*)&dbg->mem_buffer[offset];
	FOR_VEC(name_ptr, file_names)
	{
		char* name = *name_ptr;
		std::string str = name;
		int p_idx = str.find_last_of('.');
		std::string ext = str.substr(p_idx + 1);

		int tex_idx = 0;
		int tex_width = 0;
		int tex_height = 0;
		int tex_channels = 4;
		char *tex_data = nullptr;
		if (ext == "sp")
		{
				continue;
			tex_idx = LoadSpriteSheet(dbg, gl_state->texture_folder + name, &tex_width, &tex_height, &tex_channels, &tex_data);
			if(tex_idx == -1)
			{
				cur_tex->idx = -1;
				continue;
			}
		}
		else if (ext == "png")
		{
			texture_raw* tex_raw = HasRawTexture(gl_state, str);


			tex_idx = GenTexture2(dbg->lang_stat, gl_state, tex_raw->data, tex_raw->width, tex_raw->height);
			tex_width = tex_raw->width;
			tex_height = tex_raw->height;
			tex_channels = tex_raw->channels;
			tex_data = (char *)tex_raw->data;
			ASSERT(tex_data);
		}
		else
		{
			continue;
		}


		int len = strlen(name) + 1;
		int name_offset = GetMem(dbg, len);
		auto name_dst = (char*)&dbg->mem_buffer[name_offset];
		memcpy(name_dst, name, len);

		int sz = tex_height * tex_width * tex_channels;
		int data_offset = GetMem(dbg, sz);
		memcpy(&dbg->mem_buffer[data_offset], tex_data, sz);


		cur_tex->name = name_offset;
		cur_tex->data = data_offset;
		cur_tex->width = tex_width;
		cur_tex->height = tex_height;
		cur_tex->channels = tex_channels;
		cur_tex->idx = tex_idx;

		cur_tex++;

		i++;
	}
	auto end = 0;

}

void LoadTex(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];

	auto info = (load_clip_args*)&dbg->mem_buffer[base_ptr + 8];
	info->file_name = (unsigned char*)&dbg->mem_buffer[(long long)info->file_name];
	//info->cinfo = (clip*)&dbg->mem_buffer[(long long)info->cinfo];
	//info->cinfo->total_texs = info->total_sps;
	//info->cinfo->len = info->len;

	auto gl_state = (open_gl_state*)dbg->data;

	texture_raw* tex_raw = HasRawTexture(gl_state, std::string((char*)info->file_name));
	int width, height, nrChannels;
	unsigned char* src = nullptr;
	src = tex_raw->data;
	width = tex_raw->width;
	height = tex_raw->height;
	nrChannels = tex_raw->channels;

	if (info->sp_width == 0)
	{
		info->sp_width = width;
		info->sp_height = height;
	}

	int idx = GenTexture(dbg->lang_stat, gl_state, src, info->sp_width, info->sp_height, info->x_offset, info->y_offset, width, height, 0);
	auto ret = (int*)&dbg->mem_buffer[RET_1_REG * 8];
	*ret = idx;
}
void ImageFolderToFile(std::string folder)
{
	struct file_header
	{
		unsigned int total_imgs;
		unsigned int data_sect_offset;
		unsigned int str_tbl_offset;
	};
	struct file_png
	{
		unsigned int name;
		unsigned int width;
		unsigned int height;
		unsigned char channels;
		unsigned int data;
	};
	own_std::vector<char*> file_names;
	own_std::vector<unsigned char> file_data;
	own_std::vector<unsigned char> data_sect;
	own_std::vector<unsigned char> str_table;
	GetFilesInDirectory((char *)folder.c_str(), nullptr, &file_names);

	own_std::vector<char*> sprite_sheets;

	int total_imgs = 0;
	FOR_VEC(ptr, file_names)
	{
		std::string str = *ptr;
		int p_idx = str.find_last_of('.');
		std::string ext = str.substr(p_idx + 1);
		if (ext == "png")
		{
			int cur_str_table_offset = str_table.size();
			auto c_str = (unsigned char*)str.c_str();
			str_table.insert(str_table.end(), c_str, c_str + str.size() + 1);

			int cur_offset = file_data.size();
			file_data.make_count(file_data.size() + sizeof(file_png));
			auto cur_file = (file_png*)(file_data.begin() + cur_offset);

			int width, height, nrChannels;
			unsigned char* src = nullptr;
			stbi_set_flip_vertically_on_load(true);

			src = stbi_load((char*)(folder + str).c_str(), &width, &height, &nrChannels, 0);
			ASSERT(src);


			cur_file->name = cur_str_table_offset;
			cur_file->width = width;
			cur_file->height = height;
			cur_file->channels = nrChannels;
			cur_file->data = data_sect.size();

			data_sect.insert(data_sect.end(), src, src + (width * height * nrChannels));
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

	final_buffer.insert(final_buffer.begin(), (unsigned char*)&hdr, (unsigned char*)(&hdr + 1));

	int size = final_buffer.size();
	if ((size % 4) != 0)
		size += 4 - (size % 4);
	final_buffer.make_count(size);

	std::string imgs_str((char*)final_buffer.data(), final_buffer.size());
	std::string images_encoded_str = base64_encode(imgs_str);

	WriteFileLang("../web/images.data", (void *)images_encoded_str.data(), images_encoded_str.size());
}

void FromGamePlayAudio(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char *name_str = (char *)&dbg->mem_buffer[name_offset];
	auto gl_state = (open_gl_state*)dbg->data;
	AudioClip* clip = nullptr;
	FOR_VEC(it, gl_state->sound->audio_clips_src)
	{
		if ((*it)->name == name_str)
		{
			clip = *it;
			break;
		}
	}
	ASSERT(clip);
	AudioClipQueued q = {};
	q.clip = clip;
	gl_state->sound->audio_clips_to_play.emplace_back(q);
}

void AssignSoundFolder(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char *name_str = (char *)&dbg->mem_buffer[name_offset];

	auto gl_state = (open_gl_state*)dbg->data;
	
	///gl_state->texture_folder = name_str;
	std::string work_dir = dbg->cur_func->from_file->path;
	//MaybeAddBarToEndOfStr(&work_dir);

	std::string sound_folder;
	sound_folder = work_dir + name_str;
	MaybeAddBarToEndOfStr(&(gl_state->texture_folder));

	own_std::vector<char*> file_names;
	GetFilesInDirectory((char *)sound_folder.c_str(), nullptr, &file_names);

	FOR_VEC(str_ptr, file_names)
	{
		gl_state->sound->audio_clips_src.emplace_back(CreateNewAudioClip(*str_ptr));
	}

	//ImageFolderToFile(gl_state->texture_folder);

}
void AssignTexFolder(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char *name_str = (char *)&dbg->mem_buffer[name_offset];

	auto gl_state = (open_gl_state*)dbg->data;
	
	gl_state->texture_folder = name_str;
	if(!dbg->cur_func)
	{
		dbg->cur_func = GetFuncBasedOnBc2(dbg, *dbg->cur_bc2);
	}
	std::string work_dir = dbg->cur_func->from_file->path;
	//MaybeAddBarToEndOfStr(&work_dir);

	gl_state->texture_folder = work_dir + gl_state->texture_folder;
	MaybeAddBarToEndOfStr(&(gl_state->texture_folder));

	ImageFolderToFile(gl_state->texture_folder);

}
void LoadClip(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];

	auto info = (load_clip_args*)&dbg->mem_buffer[base_ptr + 8];
	info->file_name = (unsigned char*)&dbg->mem_buffer[(long long)info->file_name];
	info->cinfo = (clip*)&dbg->mem_buffer[(long long)info->cinfo];
	info->cinfo->total_texs = info->total_sps;
	info->cinfo->len = info->len;

	auto gl_state = (open_gl_state*)dbg->data;

	*(int*)&dbg->mem_buffer[STACK_PTR_REG * 8] -= 16;
	base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	*(int*)&dbg->mem_buffer[base_ptr + 8] = info->total_sps * sizeof(int);
	int idx = 0;

	GetMem(dbg);
	int offset = *(int*)&dbg->mem_buffer[RET_1_REG * 8];

	*(int*)&dbg->mem_buffer[STACK_PTR_REG * 8] += 16;

	*(int**)&info->cinfo->texs_idxs = (int*)(long long)offset;
	info->cinfo->total_texs = info->total_sps;

	// load and generate the texture
	int width, height, nrChannels;
	texture_raw* tex_raw = HasRawTexture(gl_state, std::string((char*)info->file_name));
	ASSERT(tex_raw);
	unsigned char* src = nullptr;
	src = tex_raw->data;
	width = tex_raw->width;
	height = tex_raw->height;
	nrChannels = tex_raw->channels;

	auto texs_id = (int*)&dbg->mem_buffer[(long long)info->cinfo->texs_idxs];

	for (int cur_sp = 0; cur_sp < info->total_sps; cur_sp++)
	{

		texs_id[cur_sp] = GenTexture(dbg->lang_stat, gl_state, src, info->sp_width, info->sp_height, info->x_offset, info->y_offset, width, height, cur_sp);
	}
	/*
	func_decl* call_f = FuncAddedWasmInterp(dbg->wasm_state, "heap_alloc");

	block_linked* cur = NewBlock(nullptr);
	WasmDoCallInstruction(dbg, dbg->cur_bc, &cur, call_f);
	FreeBlock(cur);
	int addr = *(int*)&dbg->mem_buffer[RET_1_REG * 8];
	//dbg->wasm_state->funcs
	int a = 0;
	*/
}

int CompileShader(char* source, int type)
{
	int  success;
	char infoLog[512];
	unsigned int shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	//ASSERT(gl_)

	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return shader;
}
void UpdateLastTime(dbg_state* dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;
	if (gl_state)
		gl_state->last_time = glfwGetTime();

}
DWORD WINAPI GameAndEngineMsgThread(
  _In_ LPVOID lpParameter
)
{
	auto dbg = (dbg_state* )lpParameter;
	auto gl_state = (open_gl_state*)dbg->data;;
	auto lang_stat = (lang_state*)dbg->lang_stat;;
	HANDLE std_in = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE std_out = GetStdHandle(STD_OUTPUT_HANDLE);
	while(true)
	{
		if(!gl_state->is_engine)
		{
			std::string str;
			CheckPipeAndGetString(std_in, str);
			if (str.size() > 0)
			{
				auto br = (engine_msg_break *)str.data();
				char* file_name = (char*)(br + 1);

				std::string file_name_str = file_name;
				unit_file *fl=ThereIsFile(dbg->lang_stat, file_name_str);
				if (!fl)
					continue;
				func_decl* fdecl = GetFuncWithLine2(dbg->lang_stat, br->line, fl);
				if (!fdecl)
					continue;

				stmnt_dbg* s = nullptr;
				FOR_VEC(st, fdecl->wasm_stmnts)
				{
					if(st->line == br->line)
					{
						s = st;
						break;
					}
				}

				if (!s)
					continue;
				
				
				if (lang_stat->is_x64_bc_backend)
				{
					byte_code2* bc = lang_stat->bcs2_start + s->start;
					MakeCurBcToBeBreakpoint(dbg, bc, s->line, false);
				}
				else
				{
					dbg->bcs[s->start].dbg_brk = br->add;
					dbg->bcs[s->start].from_engine_break = br->add;
				}
			}
			//FlushFileBuffers(std_out);
		}
		else if(gl_state->game_started)
		{
			std::string str;
			CheckPipeAndGetString(gl_state->for_engine_game_stdout, str);
			if (str.size() > 0)
			{
				printf("from game:%s", str.c_str());
			}
			//FlushFileBuffers(std_out);
		}

		Sleep(500);
	}
}

void SetIsEngine(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	bool val = *(bool*)&dbg->mem_buffer[base_ptr + 8];
	auto gl_state = (open_gl_state*)dbg->data;
	gl_state->lang_stat->is_engine = val;
	gl_state->is_engine = val;

	CreateThread(nullptr, 0, GameAndEngineMsgThread, (LPVOID)dbg, 0, nullptr);
	if(val)
	{
		gl_state->scene_srceen_width = 800;
		gl_state->scene_srceen_height = 480;
	}


	/*
	GLuint fbo, texture, depthBuffer;

	// Create and bind the framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	gl_state->frame_buffer = fbo;

	// Create the texture to render to
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	gl_state->frame_buffer_tex = texture;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		gl_state->scene_srceen_width, 
		gl_state->scene_srceen_height, 
		0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Attach the texture to the framebuffer's color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	// Create and attach a depth buffer (optional, for 3D scenes)
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
		gl_state->scene_srceen_width,
		gl_state->scene_srceen_height
	);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	// Check framebuffer completeness
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		printf("Error: Framebuffer is not complete!\n");
		ASSERT(false);
	}

	// Unbind the framebuffer for now
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
	*/

}

void OpenWindow(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int wnd_width = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int wnd_height = *(int*)&dbg->mem_buffer[base_ptr + 16];

	auto gl_state = (open_gl_state*)dbg->data;
	if (!gl_state->is_engine)
	{
		gl_state->scene_srceen_width = wnd_width;
		gl_state->scene_srceen_height = wnd_height;
	}
	if (gl_state->glfw_window)
	{
		glfwSetWindowSize((GLFWwindow *)gl_state->glfw_window, wnd_width, wnd_height);
		gl_state->width = wnd_width;
		gl_state->height = wnd_height;
		*(long long*)&dbg->mem_buffer[RET_1_REG * 8] = (long long)gl_state->glfw_window;
		return;
	}

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return;

	wnd_width = 1000;
	wnd_height = 900;
	gl_state->width = wnd_width;
	gl_state->height = wnd_height;
	/* Create a windowed mode window and its OpenGL context */
	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	/*
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
	*/

	//glfwWindowHint(GLFW_REFRESH_RATE, 60);

	window = glfwCreateWindow(gl_state->width, gl_state->height, "Hello World", NULL, NULL);
	if (!window)
	{
		ASSERT(0);
		glfwTerminate();
		return;
	}
	gl_state->glfw_window = window;

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSetWindowUserPointer(window, (void*)gl_state);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowCloseCallback(window, window_close_callback);
	glfwSetMouseButtonCallback(window, MouseCallback);

	*(long long*)&dbg->mem_buffer[RET_1_REG * 8] = (long long)window;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigNavEscapeClearFocusItem = false;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigWindowsMoveFromTitleBarOnly = true;



	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);


	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	float vertices[] = {
		// positions          // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 1.0f,   // top right
		 1.0f,   0.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		 0.0f,	0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
		 0.0f,  1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


	const char* vertexShaderSource = "#version 330 core\n"
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

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GL_CALL(glShaderSource(vertexShader, 1, &vertexShaderSource, NULL));
	GL_CALL(glCompileShader(vertexShader));

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		ASSERT(false);
	}
	const char* fragmentShaderNoTextureSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoord;\n"
		"uniform vec4 color;\n"
		"void main(){\n"
		//"vec4 tex_col =  texture(tex, uv);\n"
		"FragColor =  color;\n"
		"}\n";
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoord;\n"
		"uniform vec4 color;\n"
		"uniform vec2 tex_size;\n"
		"uniform vec2 tex_offset;\n"
		"uniform sampler2D tex;\n"
		"void main(){\n"
		//"vec4 tex_col =  texture(tex, TexCoord + vec2(0.0, tex_size.y / 16.0));\n"
		"vec4 tex_col =  texture(tex, (TexCoord + tex_offset)* tex_size);\n"
		"if(tex_col.a == 0.0)discard;\n"
		//"vec4 tex_col =  texture(tex, uv);\n"
		"FragColor =  tex_col * color;\n"
		"}\n";
	const char* fragmentShaderSourceLight = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoord;\n"
		"uniform vec4 color;\n"
		"uniform sampler2D tex;\n"
		"void main(){\n"
		"vec4 tex_col =  texture(tex, TexCoord);\n"
		//"vec4 tex_col =  texture(tex, uv);\n"
		"FragColor =  tex_col * color;\n"
		"}\n";

	unsigned int fragmentShader = CompileShader((char*)fragmentShaderSource, GL_FRAGMENT_SHADER);
	unsigned int fragmentNoTextureShader = CompileShader((char*)fragmentShaderNoTextureSource, GL_FRAGMENT_SHADER);


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
	//glUseProgram(shaderProgram);


	gl_state->vao = VAO;
	gl_state->shader_program = shaderProgram;
	gl_state->shader_program_no_texture = shaderProgramNoTexture;


	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);



	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load and generate the texture
	int width, height, nrChannels;
	*/


}
/*
void DebuggerCommand(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int str_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	//ASSERT(sz > 0)

	char* str = (char*)&dbg->mem_buffer[str_offset];


	int addr = *(int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR];
	//int *max = (int*)&dbg->mem_buffer[MEM_PTR_MAX_ADDR];
	*(int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR] += sz;
	ASSERT((addr + sz) < 64000);
	//*max += sz;

	*(int*)&dbg->mem_buffer[RET_1_REG * 8] = addr;

}
*/
void SetMem(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int sz = *(int*)&dbg->mem_buffer[base_ptr + 8];
	ASSERT(sz > 0)

	int addr = *(int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR];
	//int *max = (int*)&dbg->mem_buffer[MEM_PTR_MAX_ADDR];
	*(int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR] = sz;
}
void SubMem(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int sz = *(int*)&dbg->mem_buffer[base_ptr + 8];
	ASSERT(sz > 0)

		int addr = *(int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR];
	//int *max = (int*)&dbg->mem_buffer[MEM_PTR_MAX_ADDR];
	*(int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR] -= sz;
	ASSERT((addr - sz) >= 0);
	//*(int*)&dbg->mem_buffer[RET_1_REG * 8] = addr;

}
void GetMem(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int sz = *(int*)&dbg->mem_buffer[base_ptr + 8];
	ASSERT(sz > 0)
	ASSERT(base_ptr > (MEM_PTR_CUR_ADDR + 8));

		int addr = *(int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR];
	//int *max = (int*)&dbg->mem_buffer[MEM_PTR_MAX_ADDR];
	*(int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR] += sz;
	ASSERT((addr + sz) < DATA_SECT_OFFSET);
	//*max += sz;

	*(long long*)&dbg->mem_buffer[RET_1_REG * 8] = addr;

}

void GetTimeSinceStart(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	float val = *(float*)&dbg->mem_buffer[base_ptr + 8];
	//auto gl_state = (open_gl_state*)dbg->data;

	*(float*)&dbg->mem_buffer[RET_1_REG * 8] = (float)glfwGetTime();
}
void Sqrt(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	float val = *(float*)&dbg->mem_buffer[base_ptr + 8];

	*(float*)&dbg->mem_buffer[RET_1_REG * 8] = sqrt(val);
}
void PrintStr(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int str_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	char* str = (char*)&dbg->mem_buffer[str_offset];
	printf("%s", str);

	//*(float*)&dbg->mem_buffer[RET_1_REG * 8] = sinf(val);
}
void PrintV3Int(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int x = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int y = *(int*)&dbg->mem_buffer[base_ptr + 16];
	int z = *(int*)&dbg->mem_buffer[base_ptr + 24];
	char buffer[128];
	int sz = snprintf(buffer, 128, "x: %d, y: %d, z: %d\n", x, y, z);
	DWORD written = 0;
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, sz, &written, NULL);

	//*(float*)&dbg->mem_buffer[RET_1_REG * 8] = sinf(val);
}
void PrintV3(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	float x = *(float*)&dbg->mem_buffer[base_ptr + 8];
	float y = *(float*)&dbg->mem_buffer[base_ptr + 16];
	float z = *(float*)&dbg->mem_buffer[base_ptr + 24];
	printf("x: %.3f, y: %.3f, z: %.3f\n", x, y, z);

	//*(float*)&dbg->mem_buffer[RET_1_REG * 8] = sinf(val);
}
void OpenLocalsWindow(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[BASE_STACK_PTR_REG * 8];
	int stack_base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int line = *(int*)&dbg->mem_buffer[stack_base_ptr + 8];
	if (dbg->frame_is_from_dbg)
		return;

	byte_code2 *addr = *(byte_code2 **)&dbg->mem_buffer[RIP_REG * 8];

	func_decl *fdecl = GetFuncBasedOnBc2(dbg, addr);
	scope *scp = FindScpWithLine(fdecl, line);
	ASSERT(scp);
	BeginLocalsChild(*dbg, base_ptr + 8, scp);
	//ImGui::Text("AOe");
	///glfwSwapBuffers(window);
}
void MemSet(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int a_ptr = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int b = *(int*)&dbg->mem_buffer[base_ptr + 16];
	int c = *(int*)&dbg->mem_buffer[base_ptr + 24];
	int *a = (int*)&dbg->mem_buffer[a_ptr];
	memset(a, b, c);

}
void MemCpy(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int a_ptr = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int b_ptr = *(int*)&dbg->mem_buffer[base_ptr + 16];
	int c_ptr = *(int*)&dbg->mem_buffer[base_ptr + 24];
	int *a = (int*)&dbg->mem_buffer[a_ptr];
	int *b = (int*)&dbg->mem_buffer[b_ptr];
	memcpy(a, b, c_ptr);

}
void PointLineDistance(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int a_ptr = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int b_ptr = *(int*)&dbg->mem_buffer[base_ptr + 16];
	int c_ptr = *(int*)&dbg->mem_buffer[base_ptr + 24];
	int d_ptr = *(int*)&dbg->mem_buffer[base_ptr + 32];

	auto a = (v3*)&dbg->mem_buffer[a_ptr];
	auto b = (v3*)&dbg->mem_buffer[b_ptr];
	auto p = (v3*)&dbg->mem_buffer[c_ptr];
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
	//*(float*)&dbg->mem_buffer[RET_1_REG * 8] = 
}
void DotV3(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int a_ptr = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int b_ptr = *(int*)&dbg->mem_buffer[base_ptr + 16];
	float *a = (float*)&dbg->mem_buffer[a_ptr];
	float *b = (float*)&dbg->mem_buffer[b_ptr];

	*(float*)&dbg->mem_buffer[RET_1_REG * 8] = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];

}
void WriteFileInterpreter(dbg_state* dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int buffer_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];
	int buffer_sz = *(int*)&dbg->mem_buffer[base_ptr + 24];

	auto name_ptr = (char*)&dbg->mem_buffer[name_offset];
	auto buffer_ptr = (char*)&dbg->mem_buffer[buffer_offset];

	std::string work_dir = dbg->cur_func->from_file->path;
	//MaybeAddBarToEndOfStr(&work_dir);

	work_dir = work_dir + name_ptr;
	MaybeAddBarToEndOfStr(&work_dir);

	WriteFileLang((char *)work_dir.c_str(), buffer_ptr, buffer_sz);

}
void GetInstRealAddr(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int inst_idx = *(int*)&dbg->mem_buffer[base_ptr + 8];
	*(u64*)&dbg->mem_buffer[RET_1_REG * 8] = (u64)(dbg->lang_stat->bcs2_start + inst_idx);
}
void GetTopStackPtr(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	*(u64*)&dbg->mem_buffer[RET_1_REG * 8] = base_ptr + 8;

}
void Cos(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	float val = *(float*)&dbg->mem_buffer[base_ptr + 8];

	*(float*)&dbg->mem_buffer[RET_1_REG * 8] = cosf(val);
}
void GetMouseScroll(dbg_state* dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;
	*(int*)&dbg->mem_buffer[RET_1_REG * 8] = gl_state->scroll;
}
void Sin(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	float val = *(float*)&dbg->mem_buffer[base_ptr + 8];

	*(float*)&dbg->mem_buffer[RET_1_REG * 8] = sinf(val);
}
void Stub()
{

}

std::string GetFolderName(std::string path)
{
	int last_bar = path.find_last_of('/');

	if (last_bar == (path.size() - 1))
	{
		last_bar--;
		while (path[last_bar] != '/' && path[last_bar] != '\\' && last_bar > 0)
			last_bar--;
		//last_bar = path.fi(path.data(), 0, last_bar - 1);
	}
	if (last_bar == -1)
		last_bar = 0;

	return path.substr(last_bar + 1);
}
void IsMouseOnGameWindow(dbg_state *dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;
	GLFWwindow* window = (GLFWwindow*)gl_state->glfw_window;
	double ypos, xpos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (gl_state->is_engine)
	{
		ypos /= gl_state->scene_srceen_height;
		xpos /= gl_state->scene_srceen_width;
	}
	else
	{
		ypos /= gl_state->height;
		xpos /= gl_state->width;
	}
	//ypos *= 10;
	bool ret = true;

	if (xpos > 1 || xpos < 0)
		ret = false;
	if (ypos > 1 || ypos < 0)
		ret = false;
	*(bool*)&dbg->mem_buffer[RET_1_REG * 8] = ret;
}
void GetMouseNormalizedPosY(dbg_state *dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;
	GLFWwindow* window = (GLFWwindow*)gl_state->glfw_window;
	double ypos, xpos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (gl_state->is_engine)
	{
		float ratio = (float)gl_state->scene_srceen_height / (float)gl_state->scene_srceen_width;
		ypos /= gl_state->scene_srceen_height;
	}
	else
	{
		float ratio = (float)gl_state->height / (float)gl_state->width;
		ypos /= gl_state->height;
	}
	ypos = ypos * 2 - 1;
	//ypos *= 10;
	*(float*)&dbg->mem_buffer[RET_1_REG * 8] = ypos;

}
void GetMouseNormalizedPosX(dbg_state *dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;
	GLFWwindow* window = (GLFWwindow*)gl_state->glfw_window;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	float ratio = (float)gl_state->height / (float)gl_state->width;
	if (gl_state->is_engine)
	{
		ratio = (float)gl_state->scene_srceen_height / (float)gl_state->scene_srceen_width;
		xpos /= gl_state->scene_srceen_width;
	}
	else
	{
		//ratio = (float)gl_state->height / (float)gl_state->width;
		xpos /= gl_state->width;
	}
	xpos = xpos * 2 - 1;
	xpos /= ratio;
	//ypos *= ;
	*(float*)&dbg->mem_buffer[RET_1_REG * 8] = xpos;

}
void GetMouseScreenPosY(dbg_state *dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;
	GLFWwindow* window = (GLFWwindow*)gl_state->glfw_window;
	double ypos, xpos;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (gl_state->is_engine)
	{
		float ratio = (float)gl_state->height / (float)gl_state->scene_srceen_height;
		//ypos *= ratio;
	}
	*(float*)&dbg->mem_buffer[RET_1_REG * 8] = ypos;

}
void GetMouseScreenPosX(dbg_state *dbg)
{
	auto gl_state = (open_gl_state*)dbg->data;
	GLFWwindow* window = (GLFWwindow*)gl_state->glfw_window;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (gl_state->is_engine)
	{
		float ratio = (float)gl_state->width / (float)gl_state->scene_srceen_width;
		//xpos *= ratio;
	}
	*(float*)&dbg->mem_buffer[RET_1_REG * 8] = xpos;

}
void Rand01(dbg_state* dbg)
{
	auto r = ((unsigned int)rand()) % 2000;
	double f = (double)r / 2000;
	*(float*)&dbg->mem_buffer[RET_1_REG * 8] = f;
}

void FreeTexture(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int tex_id = *(int*)&dbg->mem_buffer[base_ptr + 8];
	auto gl_state = (open_gl_state*)dbg->data;
	texture_info* t = &gl_state->textures[tex_id];
	if (t->used)
	{
		glDeleteTextures(1, (GLuint *)&t->id);
		t->used = false;
	}
}
void ScreenRatio(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	auto gl_state = (open_gl_state*)dbg->data;
	GLFWwindow* window = (GLFWwindow*)gl_state->glfw_window;

	if (gl_state->is_engine)
	{
		float ratio = (float)gl_state->scene_srceen_height / (float)gl_state->scene_srceen_width;
		*(float*)&dbg->mem_buffer[RET_1_REG * 8] = ratio;
	}
	else
	{
		float ratio = (float)gl_state->height / (float)gl_state->width;
		*(float*)&dbg->mem_buffer[RET_1_REG * 8] = ratio;
	}
}


HRESULT InitXAudio2(sound_state &sound, bool start_playing)
{
	sound.harmonics_buffer = (char*)malloc(sound.samples_in_buffer * 4 * 4);
	memset(sound.buffers, 0, sizeof(sound.buffers));
	void *src = &sound.harmonics_buffer[sound.samples_in_buffer * 4];
	void *dst = sound.harmonics_buffer;

	HRESULT hr;
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
		return hr;

	IXAudio2* pXAudio2 = nullptr;
	if (FAILED(hr = XAudio2Create(&pXAudio2, 0,  XAUDIO2_DEFAULT_PROCESSOR | XAUDIO2_DEBUG_ENGINE)))
		return hr;


	XAUDIO2_EFFECT_CHAIN chain = {};

	IXAudio2MasteringVoice* pMasterVoice = nullptr;
	if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasterVoice)))
		return hr;

	WAVEFORMATEX wave_format = {};

	wave_format.wFormatTag = WAVE_FORMAT_PCM;
	wave_format.nChannels = 2;
	wave_format.nSamplesPerSec = sound.samples_per_sec;
	wave_format.wBitsPerSample = 16;
	wave_format.nBlockAlign = (wave_format.nChannels * wave_format.wBitsPerSample) / 8;
	wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;


	if (FAILED(hr = pXAudio2->CreateSourceVoice(&sound.pSourceVoice, (WAVEFORMATEX*)&wave_format, XAUDIO2_VOICE_NOPITCH, XAUDIO2_MAX_FREQ_RATIO, sound.audio_class, nullptr, nullptr)))
		return hr;


	XAUDIO2_BUFFER& xaudio_buff = sound.buffers[0];
	xaudio_buff.Flags = 0;
	xaudio_buff.AudioBytes = sound.samples_in_buffer * 4;
	xaudio_buff.pAudioData = (BYTE*)src;
	xaudio_buff.PlayLength = 0;


	XAUDIO2_BUFFER& xaudio_buff2 = sound.buffers[1];
	memcpy((void*)&xaudio_buff2, (void*)&xaudio_buff, sizeof(XAUDIO2_BUFFER));
	xaudio_buff2.pAudioData = (BYTE*)dst;

	if (FAILED(hr = sound.pSourceVoice->SubmitSourceBuffer(&sound.buffers[sound.cur_buffer_to_submit], nullptr)))
	{
	}
	//cpy_thread = CreateThread(NULL, 0, ThreadProc, nullptr, 0, nullptr);

	if (start_playing)
	{
		if (FAILED(hr = sound.pSourceVoice->Start(0)))
			return hr;
	}
	return 0;
}

bool fileExists(const char* filename) {
	std::ifstream file(filename);
	return file.is_open();
}
void* alloc_own(int size, mem_alloc* alloc)
{
	return heap_alloc(alloc, size);
}
void free_own(char* ptr, mem_alloc* alloc)
{
	heap_free(alloc, ptr);
}
void* realloc_own(char* ptr, int size, mem_alloc* alloc)
{
	void* ret = heap_alloc(alloc, size);
	auto chunk = (mem_chunk *)alloc->in_use.Get(ptr);


	memcpy(ret, ptr, min(chunk->size * BYTES_PER_CHUNK, size));

	heap_free(alloc, ptr);
	return ret;
}
AudioClip* CreateNewAudioClip(char* name)
{
	if (!fileExists(name)) {
		std::cerr << "File could not be opened or does not exist: " << name << std::endl;
		ASSERT(false);
		return nullptr;
	}

	auto ret = (AudioClip *)__lang_globals.alloc(__lang_globals.data, sizeof(AudioClip));
	memset(ret, 0, sizeof(AudioClip));
	ret->name = name;
	memset(&ret->buffer, 0, sizeof(own_std::vector<int>));

	unsigned int channels;
	unsigned int sampleRate;
	drflac_uint64 totalPCMFrameCount;
	//dr
	drflac_int32* pSampleData = drflac_open_file_and_read_pcm_frames_s32(name, &channels, &sampleRate, &totalPCMFrameCount, NULL);


	ret->buffer.make_count(totalPCMFrameCount * channels);
	ret->time = (float)(totalPCMFrameCount ) / (float)sampleRate;
	int sz = totalPCMFrameCount * channels;

	for (int i = 0; i < sz; i++)
	{

		ret->buffer[i] = pSampleData[i] >> 16;
	}
	//memcpy(&ret->buffer[0], pSampleData, sz);
	free(pSampleData);

	/*
	drflac* pFlac = drflac_open_file("01.flac", &allocationCallbacks);
	int sampleRate, channels;
	short* buffer;
	//int res = stb_vorbis_decode_filename(name, channels, &sampleRate, &buffer);
	*/
	return ret;

}


int main(int argc, char* argv[])
{
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string exe_full = buffer;
	int last_bar = exe_full.find_last_of("\\/");
	std::string exe_dir = exe_full.substr(0, last_bar + 1);
	SetCurrentDirectory(exe_dir.c_str());

	lang_state lang_stat;
	mem_alloc alloc;
	alloc.main_buffer = nullptr;
	InitMemAlloc(&alloc);


	sound_state sound;
	sound.audio_class = new XAudioClass();
	//memset(sound.audio_class, 0, sizeof(sound.audio_class));
	sound.audio_class->sound = &sound;

	InitXAudio2(sound, false);
	/*
	auto addr = heap_alloc(&alloc, 12);
	auto addr2 = heap_alloc(&alloc, 12);
	heap_free(&alloc, addr);
	heap_free(&alloc, addr2);
	*/


	open_gl_state gl_state = {};
	gl_state.sound = &sound;
	gl_state.lang_stat = &lang_stat;
	InitLang(&lang_stat, (AllocTypeFunc)heap_alloc, (FreeTypeFunc)heap_free, &alloc);

	//sound.audio_clips_src.emplace_back(CreateNewAudioClip("02.flac"));
	//AddAudioClipToPlay(&sound, sound.audio_clips_src[0]);

	own_std::string oexe_dir = lang_stat.exe_dir.c_str();
	last_bar = oexe_dir.find_last_of("/\\");
	own_std::string dir = oexe_dir.substr(0, last_bar + 1);

	dir = "hello";
	dir = dir + "jij";

	ASSERT(dir == "hellojij");


	compile_options opts = {};
	//opts.file = "../lang2/files";
	//opts.wasm_dir = "../lang2/web/";
	if (argc > 1)
	{
		std::string arg1 = argv[1];
		if (arg1 == "run")
		{
			if (argc <= 2)
			{
				printf("no folder specified\n");
				return 0;
			}
			else
			{
				opts.file = argv[2];
				opts.folder_name = GetFolderName(opts.file);
				opts.wasm_dir = "";
				opts.release = false;
			}

		}
		else if (arg1 == "webgame")
		{
			if (argc != 4)
			{
				printf("not all arguments were provided. webgame needs 1) the path/to/src/folder and 2)the path/to/output\n");
				return 0;
			}
			else
			{
				opts.file = argv[2];

				MaybeAddBarToEndOfStr(&opts.file);
				opts.folder_name = GetFolderName(opts.file);

				opts.wasm_dir = argv[3];
				//opts.wasm_dir += folder_name;
				opts.release = true;
				ImageFolderToFile(opts.file + "/images/");
				opts.file += "files";
			}
		}
		else
		{
			printf("Command not recognized '%s'", arg1.c_str());
			return 0;
		}
	}
	else
	{
		printf("no command provided");
		return 0;
	}

	MaybeAddBarToEndOfStr(&opts.wasm_dir);
	Compile(&lang_stat, &opts);
	memset(&lang_stat, 0, sizeof(lang_stat));
	InitMemAlloc(&alloc);
	InitLang(&lang_stat, (AllocTypeFunc)heap_alloc, (FreeTypeFunc)heap_free, &alloc);
	//AssertFuncByteCode(&lang_stat);

	AssignOutsiderFunc(&lang_stat, "GetMem", (OutsiderFuncType)GetMem);
	AssignOutsiderFunc(&lang_stat, "SetMem", (OutsiderFuncType)SetMem);
	AssignOutsiderFunc(&lang_stat, "SubMem", (OutsiderFuncType)SubMem);
	AssignOutsiderFunc(&lang_stat, "Print", (OutsiderFuncType)Print);
	AssignOutsiderFunc(&lang_stat, "OpenWindow", (OutsiderFuncType)OpenWindow);
	AssignOutsiderFunc(&lang_stat, "ShouldClose", (OutsiderFuncType)ShouldClose);
	AssignOutsiderFunc(&lang_stat, "ClearBackground", (OutsiderFuncType)ClearBackground);
	AssignOutsiderFunc(&lang_stat, "Draw", (OutsiderFuncType)Draw);
	AssignOutsiderFunc(&lang_stat, "GetTime", (OutsiderFuncType)GetTime);

	AssignOutsiderFunc(&lang_stat, "IsKeyHeld", (OutsiderFuncType)IsKeyHeld);
	AssignOutsiderFunc(&lang_stat, "IsKeyDown", (OutsiderFuncType)IsKeyDown);
	AssignOutsiderFunc(&lang_stat, "IsKeyUp", (OutsiderFuncType)IsKeyUp);

	AssignOutsiderFunc(&lang_stat, "LoadClip", (OutsiderFuncType)LoadClip);
	AssignOutsiderFunc(&lang_stat, "LoadTex", (OutsiderFuncType)LoadTex);
	AssignOutsiderFunc(&lang_stat, "LoadSceneFolder", (OutsiderFuncType)LoadSceneFolder);
	//AssignOutsiderFunc(&lang_stat, "GetDeltaTime", (OutsiderFuncType)GetDeltaTime);
	AssignOutsiderFunc(&lang_stat, "EndFrame", (OutsiderFuncType)EndFrame);
	AssignOutsiderFunc(&lang_stat, "GetTimeSinceStart", (OutsiderFuncType)GetTimeSinceStart);
	AssignOutsiderFunc(&lang_stat, "sqrt", (OutsiderFuncType)Sqrt);
	AssignOutsiderFunc(&lang_stat, "AssignCtxAddr", (OutsiderFuncType)Stub);
	AssignOutsiderFunc(&lang_stat, "WasmDbg", (OutsiderFuncType)Stub);

	AssignOutsiderFunc(&lang_stat, "PrintV3", (OutsiderFuncType)PrintV3);
	AssignOutsiderFunc(&lang_stat, "PrintV3Int", (OutsiderFuncType)PrintV3Int);
	AssignOutsiderFunc(&lang_stat, "PrintStr", (OutsiderFuncType)PrintStr);

	AssignOutsiderFunc(&lang_stat, "AssignTexFolder", (OutsiderFuncType)AssignTexFolder);

	AssignOutsiderFunc(&lang_stat, "GetMouseNormalizedPosX", (OutsiderFuncType)GetMouseNormalizedPosX);
	AssignOutsiderFunc(&lang_stat, "GetMouseNormalizedPosY", (OutsiderFuncType)GetMouseNormalizedPosY);
	AssignOutsiderFunc(&lang_stat, "GetMouseScreenPosX", (OutsiderFuncType)GetMouseScreenPosX);
	AssignOutsiderFunc(&lang_stat, "GetMouseScreenPosY", (OutsiderFuncType)GetMouseScreenPosY);


	AssignOutsiderFunc(&lang_stat, "FreeTexture", (OutsiderFuncType)FreeTexture);
	

	AssignOutsiderFunc(&lang_stat, "IsMouseHeld", (OutsiderFuncType)IsMouseHeld);
	AssignOutsiderFunc(&lang_stat, "IsMouseUp", (OutsiderFuncType)IsMouseUp);
	AssignOutsiderFunc(&lang_stat, "IsMouseDown", (OutsiderFuncType)IsMouseDown);
	AssignOutsiderFunc(&lang_stat, "IsMouseDoubleClick", (OutsiderFuncType)IsMouseDoubleClick);

	AssignOutsiderFunc(&lang_stat, "ScreenRatio", (OutsiderFuncType)ScreenRatio);
	AssignOutsiderFunc(&lang_stat, "AssignSoundFolder", (OutsiderFuncType)AssignSoundFolder);
	AssignOutsiderFunc(&lang_stat, "PlayAudio", (OutsiderFuncType)FromGamePlayAudio);
	//AssignOutsiderFunc(&lang_stat, "DebuggerCommand", (OutsiderFuncType)DebuggerCommand);
	AssignOutsiderFunc(&lang_stat, "sin", (OutsiderFuncType)Sin);
	AssignOutsiderFunc(&lang_stat, "cos", (OutsiderFuncType)Cos);
	AssignOutsiderFunc(&lang_stat, "dot_v3", (OutsiderFuncType)DotV3);
	AssignOutsiderFunc(&lang_stat, "memcpy", (OutsiderFuncType)MemCpy);
	AssignOutsiderFunc(&lang_stat, "memset", (OutsiderFuncType)MemSet);
	AssignOutsiderFunc(&lang_stat, "PointLineDistance", (OutsiderFuncType)PointLineDistance);
	AssignOutsiderFunc(&lang_stat, "OpenLocalsWindow", (OutsiderFuncType)OpenLocalsWindow);
	AssignOutsiderFunc(&lang_stat, "Rand01", (OutsiderFuncType)Rand01);

	AssignOutsiderFunc(&lang_stat, "ImGuiBegin", (OutsiderFuncType)ImGuiBegin);
	AssignOutsiderFunc(&lang_stat, "ImGuiEnd", (OutsiderFuncType)ImGuiEnd);
	AssignOutsiderFunc(&lang_stat, "ImGuiBeginChild", (OutsiderFuncType)ImGuiBeginChild);
	AssignOutsiderFunc(&lang_stat, "ImGuiEndChild", (OutsiderFuncType)ImGuiEndChild);
	AssignOutsiderFunc(&lang_stat, "ImGuiText", (OutsiderFuncType)ImGuiText);
	AssignOutsiderFunc(&lang_stat, "ImGuiImage", (OutsiderFuncType)ImGuiImage);
	AssignOutsiderFunc(&lang_stat, "ImGuiSelectable", (OutsiderFuncType)ImGuiSelectable);
	AssignOutsiderFunc(&lang_stat, "ImGuiSameLine", (OutsiderFuncType)ImGuiSameLine);
	AssignOutsiderFunc(&lang_stat, "ImGuiPushItemWidth", (OutsiderFuncType)ImGuiPushItemWidth);
	AssignOutsiderFunc(&lang_stat, "ImGuiPopItemWidth", (OutsiderFuncType)ImGuiPopItemWidth);
	AssignOutsiderFunc(&lang_stat, "ImGuiSetNextItemAllowOverlap", (OutsiderFuncType)ImGuiSetNextItemAllowOverlap);
	AssignOutsiderFunc(&lang_stat, "ImGuiGetCursorPosX", (OutsiderFuncType)ImGuiGetCursorPosX);
	AssignOutsiderFunc(&lang_stat, "ImGuiGetCursorPosY", (OutsiderFuncType)ImGuiGetCursorPosY);
	AssignOutsiderFunc(&lang_stat, "ImGuiGetCursorScreenPosX", (OutsiderFuncType)ImGuiGetCursorScreenPosX);
	AssignOutsiderFunc(&lang_stat, "ImGuiGetCursorScreenPosY", (OutsiderFuncType)ImGuiGetCursorScreenPosY);
	AssignOutsiderFunc(&lang_stat, "ImGuiAddRect", (OutsiderFuncType)ImGuiAddRect);
	AssignOutsiderFunc(&lang_stat, "ImGuiHasFocus", (OutsiderFuncType)ImGuiHasFocus);
	AssignOutsiderFunc(&lang_stat, "ImGuiTreeNodeEx", (OutsiderFuncType)ImGuiTreeNodeEx);
	AssignOutsiderFunc(&lang_stat, "ImGuiTreePop", (OutsiderFuncType)ImGuiTreePop);
	AssignOutsiderFunc(&lang_stat, "ImGuiEnumCombo", (OutsiderFuncType)ImGuiEnumCombo);
	AssignOutsiderFunc(&lang_stat, "ImGuiInitTextEditor", (OutsiderFuncType)ImGuiInitTextEditor);
	AssignOutsiderFunc(&lang_stat, "ImGuiInputText", (OutsiderFuncType)ImGuiInputText);
	AssignOutsiderFunc(&lang_stat, "ImGuiInputInt", (OutsiderFuncType)ImGuiInputInt);
	AssignOutsiderFunc(&lang_stat, "ImGuiRenderTextEditor", (OutsiderFuncType)ImGuiRenderTextEditor);
	AssignOutsiderFunc(&lang_stat, "ImGuiSetWindowFontScale", (OutsiderFuncType)ImGuiSetWindowFontScale);
	AssignOutsiderFunc(&lang_stat, "ImGuiCheckbox", (OutsiderFuncType)ImGuiCheckbox);

	AssignOutsiderFunc(&lang_stat, "CopyTextureToBuffer", (OutsiderFuncType)CopyTextureToBuffer);

	AssignOutsiderFunc(&lang_stat, "LoadSheetFromLayer", (OutsiderFuncType)LoadSheetFromLayer);

	AssignOutsiderFunc(&lang_stat, "WriteFile", (OutsiderFuncType)WriteFileInterpreter);
	AssignOutsiderFunc(&lang_stat, "ReadFile", (OutsiderFuncType)ReadFileInterp);
	AssignOutsiderFunc(&lang_stat, "GetFileSize", (OutsiderFuncType)GetFileSize);

	AssignOutsiderFunc(&lang_stat, "LoadTexFolder", (OutsiderFuncType)LoadTexFolder);
	AssignOutsiderFunc(&lang_stat, "GenRawTexture", (OutsiderFuncType)GenRawTexture);
	AssignOutsiderFunc(&lang_stat, "UpdateTexture", (OutsiderFuncType)UpdateTexture);
	AssignOutsiderFunc(&lang_stat, "GetMouseScroll", (OutsiderFuncType)GetMouseScroll);
	AssignOutsiderFunc(&lang_stat, "SetIsEngine", (OutsiderFuncType)SetIsEngine);
	AssignOutsiderFunc(&lang_stat, "ImGuiShowV3", (OutsiderFuncType)ImGuiShowV3);
	AssignOutsiderFunc(&lang_stat, "IsMouseOnGameWindow", (OutsiderFuncType)IsMouseOnGameWindow);
	AssignOutsiderFunc(&lang_stat, "GetTopStackPtr", (OutsiderFuncType)GetTopStackPtr);
	AssignOutsiderFunc(&lang_stat, "GetInstRealAddr", (OutsiderFuncType)GetInstRealAddr);

	if (!opts.release)
	{
		long long args[] = { 0 };

		AssignDbgFile(&lang_stat, (opts.wasm_dir + opts.folder_name + ".dbg").c_str());
		//AssignDbgFile(&lang_stat, opts);
		lang_stat.winterp->dbg->data = (void*)&gl_state;
		RunDbgFunc(&lang_stat, "tests", args, 1);
		RunDbgFunc(&lang_stat, "main", args, 1);

		int ret_val = *(int*)&lang_stat.winterp->dbg->mem_buffer[RET_1_REG * 8];
	}
	ExitProcess(1);
	int a = 0;

}
