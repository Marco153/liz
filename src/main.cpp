typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned char u8;
typedef long long s64;
#include <glad/glad.h> 
#include <glad/glad.c> 
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "../include/GLFW/glfw3.h"

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


#define KEY_HELD 1
#define KEY_DOWN 2
#define KEY_UP   4
#define KEY_RECENTLY_DOWN   8
#define KEY_REPEAT   0x10
#define PI   3.141592

#define TOTAL_KEYS   (GLFW_KEY_LAST + 3)
#define TOTAL_TEXTURES   64

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
struct open_gl_state
{
	int vao;
	int shader_program;
	int shader_program_no_texture;
	int color_u;
	int pos_u;
	int buttons[TOTAL_KEYS];
	texture_info textures[TOTAL_TEXTURES];
	own_std::vector<texture_raw> textures_raw;
	double last_time;

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
};
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
struct draw_info
{
	float pos_x;
	float pos_y;
	float pos_z;

	float pivot_x;
	float pivot_y;
	float pivot_z;

	float ent_size_x;
	float ent_size_y;
	float ent_size_z;

	float color_r;
	float color_g;
	float color_b;
	float color_a;


	int texture_id;

	float cam_size;

	unsigned long long cam_pos_addr;
	unsigned long long cam_rot_addr;

	int flags;
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


	int ent_size_u = glGetUniformLocation(prog, "ent_size");
	glUniform3f(ent_size_u, draw->ent_size_x, draw->ent_size_y, draw->ent_size_z);

	if (gl_state->is_engine)
	{
		// Render to our framebuffer
		//glBindFramebuffer(GL_FRAMEBUFFER, gl_state->frame_buffer);
		//glViewport(0, 0, gl_state->scene_srceen_width, gl_state->scene_srceen_height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		glViewport(0, gl_state->height - gl_state->scene_srceen_height, gl_state->scene_srceen_width, gl_state->scene_srceen_height); // Define the 640x480 region
	}
	else
	{
		//glViewport(0, 0, 640, 480);
	}

	if (IS_FLAG_ON(draw->flags, DRAW_INFO_LINE))
	{
		glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
	}
	else
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	case _KEY_LEFT:
	{
		key = GLFW_KEY_A;
	}break;
	default:
		ASSERT(0);
	}
	return key;
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
bool IsKeyDown(void *data, int key)
{
	auto gl_state = (open_gl_state*)data;
	//key = FromGameToGLFWKey(key);

	if (IS_FLAG_ON(gl_state->buttons[key], KEY_DOWN))
	{
		return true;
	}
	return false;

}
void ImGuiSetNextItemAllowOverlap(dbg_state* dbg)
{
	ImGui::SetNextItemAllowOverlap();
}
void ImGuiPopItemWidth(dbg_state* dbg)
{
	ImGui::PopItemWidth();
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

	if (ImGui::Selectable(name_str, selected, 0, ImVec2(w, h)))
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
		gl_state->buttons[i] &= ~(KEY_DOWN | KEY_UP | KEY_REPEAT);
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


	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto gl_state = (open_gl_state*)glfwGetWindowUserPointer(window);
	gl_state->scroll = yoffset;
}

void MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto gl_state = (open_gl_state*)glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS)
	{
		gl_state->buttons[GLFW_KEY_LAST + button] = KEY_HELD | KEY_DOWN | KEY_RECENTLY_DOWN;
		//printf("key(%d) is %d", key, gl_state->buttons[key]);
	}
	else if (action == GLFW_RELEASE)
	{
		gl_state->buttons[GLFW_KEY_LAST + button] &= ~KEY_HELD;
		gl_state->buttons[GLFW_KEY_LAST + button] |= KEY_UP;
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
		//printf("key(%d) is %d", key, gl_state->buttons[key]);
	}
	else if (action == GLFW_RELEASE)
	{
		gl_state->buttons[key] &= ~KEY_HELD;
		gl_state->buttons[key] |= KEY_UP;
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, src));
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
int LoadSpriteSheet(dbg_state* dbg, std::string sp_file_name, int *tex_width, int *tex_height, int *channels, char **tex_data)
{
	int read;
	char* file = ReadEntireFileLang((char *)sp_file_name.c_str(), &read);

	struct sheet_file_header
	{
		u32 total_layers;
		u32 str_tbl_offset;
		u32 str_tbl_sz;
	};
	struct aux_layer_info_struct
	{
		u32 type;
		u32 pixels_per_width;

		u32 grid_x;
		u32 grid_y;
		u32 total_of_used_cells;
	};
	struct aux_cell_info
	{
		u64 tex_name;
		u32 grid_x;
		u32 grid_y;

		u32 src_tex_offset_x;
		u32 src_tex_offset_y;
	};

	auto gl_state = (open_gl_state*)dbg->data;
	auto hdr = (sheet_file_header*)file;
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
	auto cur_layer = (aux_layer_info_struct*)cur_ptr;
	int px_width = cur_layer->pixels_per_width;
	char* aux_buffer = AllocMiscData(dbg->lang_stat, px_width * px_width * 4);
	*tex_width = cur_layer->grid_x * cur_layer->pixels_per_width;
	*tex_height = cur_layer->grid_y * cur_layer->pixels_per_width;
	*tex_data = (char *) AllocMiscData(dbg->lang_stat, *tex_width * *tex_height * 4);
	
	int tex_id = 0;
	for (int i = 0; i < hdr->total_layers; i++)
	{
		auto cur_cell = (aux_cell_info *)(cur_layer + 1);
		//int sz = cur_layer->grid_x * px_width * cur_layer->grid_y * px_width;
		tex_id = GenTexture2(dbg->lang_stat, gl_state, (u8 *)*tex_data, cur_layer->grid_x * px_width, cur_layer->grid_y * px_width);
		texture_info* t = &gl_state->textures[tex_id];
		glBindTexture(GL_TEXTURE_2D, t->id);

		for (int c = 0; c < cur_layer->total_of_used_cells; c++)
		{
			char* tex_name = str_table + cur_cell->tex_name;
			texture_raw *tex_src = HasRawTexture(gl_state, tex_name);

			int x_offset = cur_cell->src_tex_offset_x / px_width;
			int y_offset = cur_cell->src_tex_offset_y / px_width;
			auto data_ptr = tex_src->data + x_offset * px_width * 4 + y_offset * tex_src->width * 4 * px_width;
			CopyFromSrcImgToBuffer((char *)data_ptr, aux_buffer, px_width, px_width, tex_src->width);
			//GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
			GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 
				cur_cell->grid_x * px_width, 
				cur_cell->grid_y * px_width, 
				px_width, px_width, 
				GL_RGBA, GL_UNSIGNED_BYTE, aux_buffer)
			);

			cur_cell++;
		}
		cur_layer++;
	}
	heap_free((mem_alloc*)__lang_globals.data, (char*)aux_buffer);
	return tex_id;
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
void LoadTexFolder(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int folder_name_offset = *(int*)&dbg->mem_buffer[base_ptr + 8];
	int ar_offset = *(int*)&dbg->mem_buffer[base_ptr + 16];

	auto folder_name = (char *)&dbg->mem_buffer[folder_name_offset];
	auto ar = (own_std::vector<int> *)&dbg->mem_buffer[ar_offset];

	auto gl_state = (open_gl_state*)dbg->data;
	gl_state->texture_folder = folder_name;
	std::string work_dir = dbg->cur_func->from_file->name;
	int last_bar = work_dir.find_last_of('/');
	work_dir = work_dir.substr(0, last_bar + 1);
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
			tex_idx = LoadSpriteSheet(dbg, gl_state->texture_folder + name, &tex_width, &tex_height, &tex_channels, &tex_data);
		}
		else if (ext == "png")
		{
			texture_raw* tex_raw = HasRawTexture(gl_state, str);

			tex_idx = GenTexture2(dbg->lang_stat, gl_state, tex_raw->data, tex_raw->width, tex_raw->height);
			tex_width = tex_raw->width;
			tex_height = tex_raw->height;
			tex_channels = tex_raw->channels;
			tex_data = (char *)tex_raw->data;
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
	std::string work_dir = dbg->cur_func->from_file->name;
	int last_bar = work_dir.find_last_of('/');
	work_dir = work_dir.substr(0, last_bar + 1);
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
	std::string work_dir = dbg->cur_func->from_file->name;
	int last_bar = work_dir.find_last_of('/');
	work_dir = work_dir.substr(0, last_bar + 1);
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

void SetIsEngine(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	auto gl_state = (open_gl_state*)dbg->data;
	gl_state->is_engine = true;


	gl_state->scene_srceen_width = 640 + 200;
	gl_state->scene_srceen_height = 480 + 200;

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
	int sz = *(int*)&dbg->mem_buffer[base_ptr + 8];

	auto gl_state = (open_gl_state*)dbg->data;
	if (gl_state->glfw_window)
	{
		*(long long*)&dbg->mem_buffer[RET_1_REG * 8] = (long long)gl_state->glfw_window;
		return;
	}

	GLFWwindow* window;


	/* Initialize the library */
	if (!glfwInit())
		return;

	gl_state->width = 1700;
	gl_state->height = 1000;
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
		glfwTerminate();
		return;
	}
	gl_state->glfw_window = window;

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSetWindowUserPointer(window, (void*)gl_state);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, MouseCallback);

	*(long long*)&dbg->mem_buffer[RET_1_REG * 8] = (long long)window;
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

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
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
		"out vec2 TexCoord;\n"
		"void main()\n"
		"{\n"
		"	mat3 A = mat3(cos(cam_rot.z), -sin(cam_rot.z), 0.0,\n"
		"		 sin(cam_rot.z), cos(cam_rot.z), 0.0,\n"
		"		 0.0, 0.0, 1.0);\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"   gl_Position.xy -= pivot.xy;\n"
		"   gl_Position.xy *= ent_size.xy;\n"
		"   gl_Position.xy += pos.xy;\n"
		"   gl_Position.xy -= cam_pos.xy;\n"
		"   gl_Position.xy /= cam_size;\n"
		"   gl_Position = vec4(A * gl_Position.xyz, 1.0);\n"
		"   gl_Position.x *= screen_ratio;\n"

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
		"uniform sampler2D tex;\n"
		"void main(){\n"
		"vec4 tex_col =  texture(tex, TexCoord);\n"
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
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_ALWAYS);



	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	printf("x: %d, y: %d, z: %d\n", x, y, z);

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


	scope *scp = FindScpWithLine(dbg->cur_func, line);
	ASSERT(scp);
	BeginLocalsChild(*dbg, base_ptr, scp);
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

	std::string work_dir = dbg->cur_func->from_file->name;
	int last_bar = work_dir.find_last_of('/');
	work_dir = work_dir.substr(0, last_bar + 1);
	//MaybeAddBarToEndOfStr(&work_dir);

	work_dir = work_dir + name_ptr;
	MaybeAddBarToEndOfStr(&work_dir);

	WriteFileLang((char *)work_dir.c_str(), buffer_ptr, buffer_sz);

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
			printf("Command not recognized");
			return 0;
		}
	}
	else
	{
		printf("no command provided");
		return 0;
	}

	MaybeAddBarToEndOfStr(&opts.wasm_dir);

	AssignOutsiderFunc(&lang_stat, "GetMem", (OutsiderFuncType)GetMem);
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

	AssignOutsiderFunc(&lang_stat, "IsMouseHeld", (OutsiderFuncType)IsMouseHeld);
	AssignOutsiderFunc(&lang_stat, "IsMouseUp", (OutsiderFuncType)IsMouseUp);
	AssignOutsiderFunc(&lang_stat, "IsMouseDown", (OutsiderFuncType)IsMouseDown);

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

	AssignOutsiderFunc(&lang_stat, "CopyTextureToBuffer", (OutsiderFuncType)CopyTextureToBuffer);
	AssignOutsiderFunc(&lang_stat, "WriteFile", (OutsiderFuncType)WriteFileInterpreter);

	AssignOutsiderFunc(&lang_stat, "LoadTexFolder", (OutsiderFuncType)LoadTexFolder);
	AssignOutsiderFunc(&lang_stat, "GenRawTexture", (OutsiderFuncType)GenRawTexture);
	AssignOutsiderFunc(&lang_stat, "UpdateTexture", (OutsiderFuncType)UpdateTexture);
	AssignOutsiderFunc(&lang_stat, "GetMouseScroll", (OutsiderFuncType)GetMouseScroll);
	AssignOutsiderFunc(&lang_stat, "SetIsEngine", (OutsiderFuncType)SetIsEngine);

	Compile(&lang_stat, &opts);
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
	int a = 0;

}
