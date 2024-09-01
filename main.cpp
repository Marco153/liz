#include "compile.cpp"
#include "memory.cpp"


void Print(dbg_state* dbg)
{
	int base = *(int*)&dbg->mem_buffer[STACK_PTR_REG * 8];
	int mem_alloc_addr = *(int*)&dbg->mem_buffer[base + 8];

	int a = 0;

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
	opts.file = "../lang2/files/memory.lng";
	opts.wasm_dir = "../../wabt/";
	opts.release = false;

	AssignOutsiderFunc(&lang_stat, "GetMem", (OutsiderFuncType)GetMem);
	AssignOutsiderFunc(&lang_stat, "Print", (OutsiderFuncType)Print);
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

