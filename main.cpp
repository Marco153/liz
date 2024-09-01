#include "compile.cpp"
#include "memory.cpp"

#include "../include/GLFW/glfw3.h"

void Print(dbg_state* dbg)
{
	int base = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int mem_alloc_addr = *(int*)&dbg->mem_buffer[base + 8];

	int a = 0;


}
void Draw(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	void* addr = &dbg->mem_buffer[base_ptr + 8];

	GLFWwindow *wnd = (GLFWwindow *)*(long long*)&dbg->mem_buffer[base_ptr + 8];
	glfwPollEvents();
	glfwSwapBuffers(wnd);
	//*(int*)&dbg->mem_buffer[RET_1_REG * 8] = glfwWindowShouldClose((GLFWwindow *)(long long)wnd);
}
void ClearBackground(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	void* addr = &dbg->mem_buffer[base_ptr + 8];
	float r = *(float*)&dbg->mem_buffer[base_ptr + 8];
	float g = *(float*)&dbg->mem_buffer[base_ptr + 8 * 2];
	float b = *(float*)&dbg->mem_buffer[base_ptr + 8 * 3];

	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//*(int*)&dbg->mem_buffer[RET_1_REG * 8] = glfwWindowShouldClose((GLFWwindow *)(long long)wnd);
}
void ShouldClose(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	long long wnd = *(long long*)&dbg->mem_buffer[base_ptr + 8];

	*(int*)&dbg->mem_buffer[RET_1_REG * 8] = glfwWindowShouldClose((GLFWwindow *)(long long)wnd);
}
void OpenWindow(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int sz = *(int*)&dbg->mem_buffer[base_ptr + 8];

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	*(long long*)&dbg->mem_buffer[RET_1_REG * 8] = (long long )window;

}
void GetMem(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int sz = *(int*)&dbg->mem_buffer[base_ptr + 8];

	int *addr = (int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR];
	int *max = (int*)&dbg->mem_buffer[MEM_PTR_MAX_ADDR];
	*(int*)&dbg->mem_buffer[MEM_PTR_CUR_ADDR] += sz;
	ASSERT(*max < 64000);
	*max += sz;

	*(int*)&dbg->mem_buffer[RET_1_REG * 8] = *addr;

}

int main()
{
	lang_state lang_stat;
	mem_alloc alloc;
	InitMemAlloc(&alloc);
	/*
	auto addr = heap_alloc(&alloc, 12);
	auto addr2 = heap_alloc(&alloc, 12);
	heap_free(&alloc, addr);
	heap_free(&alloc, addr2);
	*/


	InitLang(&lang_stat, (AllocTypeFunc)heap_alloc, (FreeTypeFunc)heap_free, &alloc);
	compile_options opts = {};
	opts.file = "../lang2/files";
	opts.wasm_dir = "../../wabt/";
	opts.release = false;

	AssignOutsiderFunc(&lang_stat, "GetMem", (OutsiderFuncType)GetMem);
	AssignOutsiderFunc(&lang_stat, "Print", (OutsiderFuncType)Print);
	AssignOutsiderFunc(&lang_stat, "OpenWindow", (OutsiderFuncType)OpenWindow);
	AssignOutsiderFunc(&lang_stat, "ShouldClose", (OutsiderFuncType)ShouldClose);
	AssignOutsiderFunc(&lang_stat, "ClearBackground", (OutsiderFuncType)ClearBackground);
	AssignOutsiderFunc(&lang_stat, "Draw", (OutsiderFuncType)Draw);
	Compile(&lang_stat, &opts);
	if (!opts.release)
	{
		long long args[] = { 0 };

		AssignDbgFile(&lang_stat, "../../wabt/dbg_wasm.dbg");
		RunDbgFile(&lang_stat, "tests", args, 1);
		RunDbgFile(&lang_stat, "main", args, 1);

		int ret_val = *(int *)&lang_stat.winterp->dbg->mem_buffer[RET_1_REG * 8];
	}
	int a = 0;

}

