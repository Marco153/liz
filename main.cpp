#include "compile.cpp"
#include "memory.cpp"
#include <glad/glad.h> 
#include <glad/glad.c> 

#include "../include/GLFW/glfw3.h"

struct open_gl_state
{
	int vao;
	int shader_program;
	int color_u;
	int pos_u;
};

void Print(dbg_state* dbg)
{
	int base = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int mem_alloc_addr = *(int*)&dbg->mem_buffer[base + 8];

	int a = 0;


}
struct draw_info
{
	float pos_x;
	float pos_y;
	float pos_z;

	float color_r;
	float color_g;
	float color_b;
	float color_a;
};
void Draw(dbg_state* dbg)
{
	int base_ptr = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int draw_addr = *(int *)&dbg->mem_buffer[base_ptr + 8 * 2];


	auto wnd = (GLFWwindow *)*(long long*)&dbg->mem_buffer[base_ptr + 8];
	auto draw = (draw_info *)(long long*)&dbg->mem_buffer[draw_addr];

	auto gl_state = (open_gl_state*)dbg->data;
	glUseProgram(gl_state->shader_program);
	glBindVertexArray(gl_state->vao);

	gl_state->color_u = glGetUniformLocation(gl_state->shader_program, "color");
	glUniform4f(gl_state->color_u, draw->color_r, draw->color_b, draw->color_g, 1.0f);

	gl_state->pos_u = glGetUniformLocation(gl_state->shader_program, "pos");
	glUniform3f(gl_state->pos_u, draw->pos_x, draw->pos_y, draw->pos_z);

	
	glDrawArrays(GL_TRIANGLES, 0, 3);
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

	auto gl_state = (open_gl_state*)dbg->data;

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

	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);




	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"uniform vec3 pos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x + pos.x, aPos.y + pos.y, aPos.z + pos.z, 1.0);\n"
		"}\0";

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"uniform vec4 color;\n"
		"void main(){\n"
		"FragColor = color;\n}\n";

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	//ASSERT(gl_)

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	gl_state->vao = VAO;
	gl_state->shader_program = shaderProgram;

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


	open_gl_state gl_state;
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
		lang_stat.winterp->dbg->data = (void*)&gl_state;
		RunDbgFile(&lang_stat, "main", args, 1);

		int ret_val = *(int *)&lang_stat.winterp->dbg->mem_buffer[RET_1_REG * 8];
	}
	int a = 0;

}

