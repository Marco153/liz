typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned char u8;
typedef long long s64;

//#define WASM_DBG

#ifndef FOR_VEC
#define FOR_VEC(a, vec) for(auto a = (vec).begin(); a < (vec).end(); a++)
#endif // !1
#include "compile.h"
#include "timer.cpp"
#include <windows.h>
#include <iostream>
#include <string>
#include <chrono> 
#include <thread>
#include <vector>
#include <unordered_map>
#include <conio.h>
#include <setjmp.h>
#include <emmintrin.h>  // SSE2 intrinsics


#include "Array.cpp"
#include "rel_utils.h"

#define AUX_DECL_REG 3

#define ANSI_RESET   "\033[0m"
#define ANSI_BLACK     "\033[30m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_WHITE   "\033[37m"

#define ANSI_BG_BLACK   "\033[40m"
#define ANSI_BG_RED     "\033[41m"
#define ANSI_BG_GREEN   "\033[42m"
#define ANSI_BG_YELLOW  "\033[43m"
#define ANSI_BG_BLUE    "\033[44m"
#define ANSI_BG_MAGENTA "\033[45m"
#define ANSI_BG_CYAN    "\033[46m"
#define ANSI_BG_WHITE   "\033[47m"

#define HEAP_START 19000
//#define MEM_ALLOC_ADDR 17000
#define MEM_PTR_CUR_ADDR 10000
#define STACK_PTR_START 20000
#define MEM_PTR_MAX_ADDR 18008

#define DATA_SECT_MAX 4048
#define DATA_SECT_OFFSET 1024 * 1024 * 8
#define BUFFER_MEM_MAX (DATA_SECT_OFFSET + DATA_SECT_MAX)

#define STACK_PTR_REG 10
#define BASE_STACK_PTR_REG 11
#define RET_1_REG 12
#define RET_2_REG 13
#define FILTER_PTR 14
// we have a few float registers up to 33
#define FLOAT_REG_SIZE_BYTES 16
#define FLOAT_REG_0 35
#define RIP_REG 34
#define GLOBALS_OFFSET 11000

//#define DEBUG_GLOBAL_NOT_FOUND 


#define INSTS_SWITCH(type, add, sub, cmp, equal, lea, or_, and_, mod, mul, div)\
	switch(type){\
	case MOD_M_2_M:\
	case MOD_R_2_M:\
	case MOD_M_2_R:\
	case MOD_I_2_R:\
	case MOD_I_2_M:\
	case MOD_R_2_R:\
			mod;\
		break;\
	case DIV_M_2_M:\
	case DIV_R_2_M:\
	case DIV_M_2_R:\
	case DIV_I_2_R:\
	case DIV_I_2_M:\
	case DIV_R_2_R:\
	case DIV_SSE_2_SSE:\
	case DIV_MEM_2_SSE:\
	case DIV_SSE_2_MEM:\
	case DIV_PCKD_SSE_2_PCKD_SSE:\
			div;\
		break;\
	case MUL_M_2_M:\
	case MUL_R_2_M:\
	case MUL_M_2_R:\
	case MUL_I_2_R:\
	case MUL_I_2_M:\
	case MUL_R_2_R:\
	case MUL_SSE_2_SSE:\
	case MUL_MEM_2_SSE:\
	case MUL_SSE_2_MEM:\
	case MUL_PCKD_SSE_2_PCKD_SSE:\
			mul;\
		break;\
	case AND_M_2_M:\
	case AND_R_2_M:\
	case AND_M_2_R:\
	case AND_I_2_R:\
	case AND_I_2_M:\
	case AND_R_2_R:\
	case AND_SSE_2_SSE:\
	case AND_MEM_2_SSE:\
	case AND_SSE_2_MEM:\
			and_;\
		break;\
	case OR_M_2_M:\
	case OR_R_2_M:\
	case OR_M_2_R:\
	case OR_I_2_R:\
	case OR_I_2_M:\
	case OR_R_2_R:\
	case OR_SSE_2_SSE:\
	case OR_MEM_2_SSE:\
	case OR_SSE_2_MEM:\
			or_;\
		break;\
	case ADD_M_2_M:\
	case ADD_R_2_M:\
	case ADD_M_2_R:\
	case ADD_I_2_R:\
	case ADD_I_2_M:\
	case ADD_R_2_R:\
	case ADD_SSE_2_SSE:\
	case ADD_MEM_2_SSE:\
	case ADD_PCKD_SSE_2_PCKD_SSE:\
	case ADD_SSE_2_MEM:\
			add;\
		break;\
	case CMP_M_2_M:\
	case CMP_R_2_M:\
	case CMP_M_2_R:\
	case CMP_I_2_R:\
	case CMP_I_2_M:\
	case CMP_R_2_R:\
	case CMP_SSE_2_SSE:\
	case CMP_MEM_2_SSE:\
	case CMP_SSE_2_MEM:\
		cmp;\
		break;\
	case SUB_M_2_M:\
	case SUB_R_2_M:\
	case SUB_M_2_R:\
	case SUB_I_2_R:\
	case SUB_I_2_M:\
	case SUB_R_2_R:\
	case SUB_SSE_2_SSE:\
	case SUB_MEM_2_SSE:\
	case SUB_SSE_2_MEM:\
		sub;\
		break;\
	case STORE_I_2_M:\
	case MOV_I_2_REG_PARAM:\
	case MOV_R_2_REG_PARAM:\
	case MOV_M_2_REG_PARAM:\
	case MOV_SSE_2_MEM:\
	case MOV_M_2_SSE:\
	case MOV_SSE_2_SSE:\
	case MOV_M_2_PCKD_SSE:\
	case MOV_SSE_2_R:\
	case STORE_R_2_M:\
	case MOV_PCKD_SSE_2_M:\
	case MOV_PCKD_SSE_2_PCKD_SSE:\
	case MOV_M:\
	case MOV_R:\
	case MOV_I:\
		equal;\
		break;\
	case INST_LEA:\
		lea;\
		break;\
	default:\
		ASSERT(0);\
	}

struct comp_time_type_info
{
	//var arg struct, defined at base.lng
	long long val;
	char ptr;
	void* ptr_to_type_data;

};

struct dbg_state;
struct block_linked;

typedef void* (*FreeTypeFunc)(void* this_ptr, void* ptr);
typedef void* (*AllocTypeFunc)(void* this_ptr, u64 sz);

struct global_variables_lang
{
	char* main_buffer;
	bool string_use_page_allocator = false;

	void* data;
	AllocTypeFunc alloc;
	FreeTypeFunc free;

	block_linked* blocks;
	int total_blocks;
	int cur_block;

	float find_ident_timer;

	bool use_cached_decls = true;


	//LangArray<type_struct2> *structs;
	//LangArray<type_struct2> *template_strcts;

	//page_allocator *cur_page_allocator = nullptr;
	//sub_systems_enum string_sub_system_operations = sub_systems_enum::STRING;

}__lang_globals;


#include "own_vector.cpp"
#include "sort.cpp"

/*
#include "udis86.h"
#include "libudis86/types.h"
#include "libudis86/udis86.c"
#include "libudis86/decode.c"
#include "libudis86/syn.c"
//#include "libudis86/syn-att.c"
#include "libudis86/syn-intel.c"
//#include "libudis86/itab.c"
*/

#include "debugger.cpp"
//#include "FileIO.cpp"


#define FOR_LESS(var_name, start_val, cond_val) for(int var_name = start_val; var_name < (cond_val); var_name++)

#ifndef IS_FLAG_ON
#define IS_FLAG_ON(a, flag) (((a) & (flag)) != 0)
#define IS_FLAG_OFF(a, flag) (((a) & (flag)) == 0)
#endif

#define COMPILER

#include "memory.cpp"
struct scope;
struct node;
struct token2;
struct unit_file;
struct type_struct2;
struct func_decl;
struct decl2;
struct machine_reloc;
struct machine_sym;
struct lang_state;
char* AllocMiscData(lang_state*, int sz);

void split(const std::string& s, char separator, own_std::vector<std::string>& ret, std::string* aux)
{
	int start = 0;
	for (int i = 0; i < s.size(); i++)
	{
		if (s[i] == separator)
		{
			*aux = s.substr(start, i - start);
			start = i + 1;
			ret.emplace_back(*aux);
		}
	}
	*aux = s.substr(start, s.size() - start);
	ret.emplace_back(*aux);
}
/*
#include "../memory.h"
#include "../sub_system.h"
#include "../LangArray.cpp"
#include "../memory.cpp"
*/
enum gen_enum
{
	GEN_WASM,
	GEN_X64,
};

struct ptr_decl_that_have_len
{
	decl2* decl;
	scope* scp;
};

struct web_assembly_state;
struct wasm_interp;
struct byte_code2;
struct ast_rep;
#define MAX_ARGS 32

enum lsp_stage_enum
{
	LSP_STAGE_PAUSE,
	LSP_STAGE_ADD_FILES,
	LSP_STAGE_NAME_FINDING,
	LSP_STAGE_TYPE_CHECKING,
	LSP_STAGE_GET_INTELISENSE,
	LSP_STAGE_DONE,
};
struct get_func_bc_info
{
	gen_enum type;

	union
	{
		int x64_bc_idx;
		int wasm_bc_idx;
	};
};
struct lang_state
{
	int cur_idx;
	int execute_id;
	int flags;

	bool something_was_declared;
	bool in_ir_stmnt;
	bool is_lsp;
	bool use_node_arena;
	bool track_alloc_regs;
	own_std::vector<int> tracked_regs;

	bool is_engine;
	scope* root;

	byte_code2* bcs2_start;
	byte_code2* bcs2_end;

	lsp_stage_enum lsp_stage;
	lsp_intention_enum intentions_to_lsp;
	int intention_state;
	node* not_found_nd;
#ifdef DEBUG_NAME
	own_std::vector<node*> not_founds;
#endif
	bool gen_wasm;
	bool release;
	gen_enum gen_type;
	
	bool is_x64_bc_backend;


	
	jmp_buf jump_buffer;
	bool ir_in_stmnt;
	func_decl* cur_func;

	int cur_strct_constrct_size_per_statement;
	int cur_strct_ret_size_per_statement;

	int cur_spilled_offset;

	unsigned short regs[32];
	unsigned short float_regs[32];
	unsigned short arg_regs[MAX_ARGS];

	node* cur_stmnt;
	own_std::vector<node*> global_decl_not_found;
	own_std::vector<scope**> scope_stack;
	own_std::vector<func_decl*> func_ptrs_decls;
	//own_std::vector<get_func_addr_info*> func_ptrs_decls;
	own_std::vector<func_decl*> outsider_funcs;
	own_std::vector<own_std::vector<token2>*> allocated_vectors;
	own_std::vector<ptr_decl_that_have_len> ptrs_have_len;
	func_decl* plugins_for_func;
	std::unordered_map<func_decl*, func_decl*> comp_time_funcs;
	std::unordered_map<std::string, unsigned int> symbols_offset_on_type_sect;

	std::unordered_map<std::string, char*> internal_funcs_addr;

	std::string liz_file_path;

	wasm_interp* winterp;
	dbg_state* dstate;

	ast_rep* dbg_equal;


	own_std::vector<unit_file*> files;

	scope* funcs_scp;

	unit_file* cur_file;

	int parentheses_opened;
	int scopes_opened;

	own_std::vector<char> data_sect;
	own_std::vector<char> globals_sect;
	own_std::vector<char> type_sect;
	own_std::vector<unsigned char, 1> code_sect;

	int already_inserted_of_type_sect_in_code_sect;

	own_std::vector<char> type_sect_str_table;
	own_std::vector<own_std::vector<char>> type_sect_str_table2;


	std::vector<std::string> struct_scope;


	int type_sect_str_tbl_sz;

	decl2* base_lang;


	own_std::vector<machine_reloc> type_sect_rels;

	own_std::vector<machine_sym> type_sect_syms;

	own_std::vector<func_decl*> global_funcs;

	std::string work_dir;
	std::string exe_dir;

	int call_regs_used;
	int max_bytes_in_ar_lit;
	int lhs_saved;
	int cur_ar_lit_offset;
	int cur_ar_lit_sz;
	int cur_per_stmnt_strct_val_sz;
	int cur_per_stmnt_strct_val_offset;

	std::string linker_options;
	web_assembly_state* wasm_state;

	//for_it_desugared fit_dsug;
	char deref_reg;

	decl2* i64_decl;
	decl2* s64_decl;
	decl2* u64_decl;
	decl2* s32_decl;
	decl2* u32_decl;
	decl2* s16_decl;
	decl2* u16_decl;
	decl2* s8_decl;
	decl2* u8_decl;

	decl2* bool_decl;
	decl2* f32_decl;
	decl2* f64_decl;

	decl2* void_decl;
	decl2* char_decl;

	decl2* func_ptr_decl;

	decl2* _vec_strct;


	node* node_arena;
	int cur_nd;
	int max_nd;

	decl2* decl_arena;
	int cur_decl;
	int max_decl;

	char* misc_arena;
	int cur_misc;
	int max_misc;

	node* zero_int_node;

	char* GetCodeAddr(int offset)
	{
		return (char*)&code_sect[offset + type_sect.size()];
	}

};

struct type_struct2;


HANDLE OpenFileLang(char* name)
{
	HANDLE file = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (file == nullptr )
	{
		printf("file \"%s\" not found", name);
		ExitProcess(0);
	}
	if (file == INVALID_HANDLE_VALUE)
	{
		TCHAR buffer[MAX_PATH] = { 0 };
		GetCurrentDirectory(MAX_PATH, buffer);
		//TCHAR full_file_path[MAX_PATH] = { 0 };
		//GetFullPathName(filename, MAX_PATH, fullFilename, nullptr);


		printf("file \"%s\" was invalid handle, last error %d, full exe path %s", name, GetLastError(), buffer);

		ExitProcess(0);
	}
}
char* ReadEntireFileLang(char* name, int* read)
{
	HANDLE file = OpenFileLang(name);
	LARGE_INTEGER file_size;
	GetFileSizeEx(file, &file_size);
	char* f = (char*)__lang_globals.alloc(__lang_globals.data, file_size.QuadPart + 1);

	int bytes_read;
	ReadFile(file, (void*)f, file_size.QuadPart, (LPDWORD)&bytes_read, 0);
	f[file_size.QuadPart] = 0;

	*read = bytes_read;

	CloseHandle(file);
	return f;
}
void WriteFileLang(char* name, void* data, int size)
{
	HANDLE file = CreateFile(name, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	ASSERT(file != INVALID_HANDLE_VALUE);

	int written;
	WriteFile(file, data, size, (LPDWORD)&written, nullptr);

	CloseHandle(file);
}
struct node;
struct scope;

void DescendStmntMode(node* stmnt, scope* scp, int mode, void* data);
// Descend Mode DescendNode
#define DMODE_DNODE 1
#define DMODE_DNAME 2
#define DMODE_DFUNC 3

//#include "../sub_system.cpp"
#define STRING_IMPLEMENTATION
//#include "../tokenizer.cpp"
#include "token.cpp"
#include "node.cpp"
#include "bytecode.cpp"
#include "obj_generator.h"
#include "IR.cpp"
#include "memory.h"

#include "error_report.cpp"

struct macro_call
{
	//string macro_name;
	//string inner;
	int start_ch;
	int end_ch;
};
enum wasm_bc_enum
{
	WASM_INST_F32_CONST,
	WASM_INST_F32_STORE,
	WASM_INST_F32_ADD,
	WASM_INST_F32_SUB,
	WASM_INST_F32_LOAD,
	WASM_INST_I32_CONST,
	WASM_INST_I32_LOAD,
	WASM_INST_I32_LOAD_8_S,
	WASM_INST_I32_STORE,
	WASM_INST_I32_STORE8,
	WASM_INST_I32_SUB,
	WASM_INST_I32_REMAINDER_U,
	WASM_INST_I32_REMAINDER_S,
	WASM_INST_I32_ADD,
	WASM_INST_I32_AND,
	WASM_INST_I32_OR,
	WASM_INST_I32_MUL,


	WASM_INST_I64_LOAD,
	WASM_INST_I64_STORE,

	WASM_INST_F32_GE,
	WASM_INST_F32_LE,
	WASM_INST_F32_LT,
	WASM_INST_F32_GT,
	WASM_INST_F32_DIV,
	WASM_INST_F32_MUL,
	WASM_INST_F32_NE,
	WASM_INST_F32_EQ,

	WASM_INST_CAST_S64_2_F32,
	WASM_INST_CAST_S32_2_F32,
	WASM_INST_CAST_U32_2_F32,
	WASM_INST_CAST_F32_2_S32,

	WASM_INST_I32_GE_U,
	WASM_INST_I32_GE_S,
	WASM_INST_I32_LE_U,
	WASM_INST_I32_LE_S,
	WASM_INST_I32_LT_U,
	WASM_INST_I32_LT_S,
	WASM_INST_I32_GT_U,
	WASM_INST_I32_GT_S,
	WASM_INST_I32_DIV_U,
	WASM_INST_I32_DIV_S,
	WASM_INST_I32_NE,
	WASM_INST_I32_EQ,
	WASM_INST_BLOCK,
	WASM_INST_END,
	WASM_INST_LOOP,
	WASM_INST_BREAK,
	WASM_INST_BREAK_IF,
	WASM_INST_NOP,
	WASM_INST_CALL,
	WASM_INST_INDIRECT_CALL,
	WASM_INST_RET,
	WASM_INST_DBG_BREAK
};
enum wasm_reg
{
	R0_WASM = 0,
	R1_WASM = 1 * 8,
	R2_WASM = 2 * 8,
	R3_WASM = 3 * 8,
	R4_WASM = 4 * 8,
	R5_WASM = 5 * 8,
	RS_WASM = STACK_PTR_REG * 8,
	RBS_WASM = BASE_STACK_PTR_REG * 8,
};
struct wasm_bc
{
	wasm_bc_enum type;
	bool dbg_brk;
	bool one_time_dbg_brk;
	bool break_on_first_loop_bc;
	bool dont_dbg_brk;
	bool from_engine_break;
	int start_code;
	int end_code;
	wasm_bc* jmps_to;
	union
	{
		wasm_bc* block_end;
		wasm_reg reg;
		int i;
		float f32;
	};
};
struct wasm_func
{
	int idx;
	own_std::vector<wasm_bc> bcs;
};
// setting and compiling the macros.cpp file

// forward decls
bool AreIRValsEqual(ir_val* lhs, ir_val* rhs);
std::string GetTypeInfoLangArrayStr(std::string str)
{
	std::string final_ret;
	final_ret.reserve(64);

	for (auto c : str)
	{

		char aux[16];
		sprintf_s(aux, 16, "\'\\x%x\', ", (unsigned char)c);
		final_ret.append(aux);
	}
	return final_ret;
}

void MapMacro(node* n, node* marco_inner, node* stmnt, void* out)
{
	int a = 0;
}
void ttt()
{
}
void call(int a, int b, int c, int d, int e, int f, int g)
{
	int gss = 9;
	int dss = 10;
	d = d + e;
	ttt();
	return;
}

char* std_str_to_heap(lang_state*, std::string* str);

void NewTypeToSection(lang_state* lang_stat, char* type_name, enum_type2 idx)
{
	std::string final_name = std::string("$$") + type_name;
	lang_stat->type_sect_syms.push_back(
		machine_sym(lang_stat, SYM_TYPE_DATA, (unsigned int)lang_stat->type_sect.size() + sizeof(type_data), std_str_to_heap(lang_stat, &final_name))
	);

	auto& buffer = lang_stat->type_sect;
	int sz = buffer.size();
	buffer.insert(buffer.begin(), sizeof(type_data), 0);
	type_data* strct_ptr = (type_data*)((char*)buffer.data());
	strct_ptr->name = 0;
	strct_ptr->tp = idx;
}
int CreateDbgScope(std::vector<byte_code>* byte_codes, int start)
{

	int i = 0;
	for (auto bc = byte_codes->begin() + start; bc < byte_codes->end(); bc++)
	{
		i++;
	}
	return i;
}
struct dbg_reloc
{
	int type;
	char* to_fill;
	union
	{
		enum_type2 tp;
		char* strct_name;
	};
};
/*
struct dbg_state
{
	dbg_scp_info *scp_info;
	dbg_func_info *cur_func;

	std::vector<dbg_reloc> rels;
	std::vector<dbg_scp> scps;
	std::vector<dbg_decl> decls;
	std::vector<dbg_type> types;
	std::vector<dbg_func> funcs;


};
*/


/*
void FillDbgScpInfo(dbg_scp_info *info, scope *ref_scp, dbg_state *state)
{
	int decls_sz = ref_scp->vars.size() * sizeof(dbg_decl);
	int name_sz = 0;

	FOR_VEC(d, ref_scp->vars)
	{
		name_sz += (*d)->name.length();
	}

	info->data           = AllocMiscData(lang_stat, decls_sz + name_sz);
	info->decls_offset   = (dbg_decl *)info->data;
	info->str_tbl_offset = info->data + decls_sz;

	//info->total_sz = total_sz;

	int cur_decl = 0;

	// assigning the decls to buffer
	dbg_decl *d_decl = (dbg_decl *)info->decls_offset;
	FOR_VEC(dd, ref_scp->vars)
	{
		auto d = *dd;
		//gettind the dst name addr
		char *name_offset = (char *)(info->str_tbl_offset + info->str_tbl_sz);

		d_decl->name = (char *)(info->str_tbl_offset + info->str_tbl_sz);

		int name_sz = d->name.length();

		memcpy(name_offset, d->name.data(), name_sz);

		// one is because the string is null-terminated
		info->str_tbl_sz += name_sz + 1;

		dbg_reloc rel;

		if(d->type.type == TYPE_STRUCT)
		{
			rel.type = 1;
			rel.strct_name = (char *)d->type.strct->name.data();
		}
		else
		{
			rel.type = 0;
			rel.tp   = d->type.type;
		}

		if(IS_FLAG_ON(d->flags, DECL_IS_ARG))
			d_decl->reg = 5;
		else
			d_decl->reg = 4;

		d_decl->offset = d->offset;


		rel.to_fill = (char *)&d_decl->type;
		state->rels.push_back(rel);

		d_decl++;
		cur_decl++;
	}
}
dbg_file *GetDbgFile(char *name)
{
	return nullptr;
}

int CreateDbg(std::vector<byte_code> *byte_codes, int start, dbg_state *state)
{
	std::vector<dbg_line> lines;

	int i = 0;

	dbg_scp_info *last_scp;

	while(i < byte_codes->size())
	{
		auto bc = &(*byte_codes)[i];

		switch(bc->type)
		{
		case BEGIN_SCP:
		{
			last_scp = state->scp_info;
			auto new_scp = (dbg_scp_info *)AllocMiscData(lang_stat, sizeof(dbg_scp_info));;

			FillDbgScpInfo(new_scp, bc->scp, state);

			state->scp_info = new_scp;

			i += CreateDbg(byte_codes, i + 1, state);
		}break;
		case END_SCP:
		{
			state->scp_info = last_scp;
		}break;
		case BEGIN_FUNC:
		{
			last_scp = state->scp_info;
			auto new_scp  = (dbg_scp_info *)AllocMiscData(lang_stat, sizeof(dbg_scp_info));;
			auto new_func = (dbg_func_info *)AllocMiscData(lang_stat, sizeof(dbg_func_info));


			FillDbgScpInfo(new_scp, bc->fdecl->scp, state);

			state->scp_info = new_scp;
			state->cur_func = new_func;
			i += CreateDbg(byte_codes, i + 1, state);
		}break;
		case END_FUNC:
		{
			state->cur_func = nullptr;
			state->scp_info = last_scp;
		}break;
		case NEW_LINE:
		{
			dbg_line ln;
			ln.code_idx = bc->machine_code_idx;
			ln.ln_num  = bc->line;

			state->cur_func->lines.push_back(ln);
		}break;
		}

		i++;
	}

	return i;
}
*/

int ForeignFuncAdd(char a)
{
	return a + 2;
}

bool GetTypeInTypeSect(lang_state* lang_stat, machine_reloc* c, own_std::vector<machine_sym>& symbols, int data_sect_start, unsigned int& sym_idx)
{
	std::string cur_rel_name = std::string(c->name);
	FOR_VEC(sym, symbols)
	{
		if (cur_rel_name == std::string(sym->name))
		{
			sym_idx = sym->idx;

			if (c->type == machine_rel_type::TYPE_DATA)
				sym_idx = lang_stat->type_sect.size() - sym_idx;
			else
			{
				sym_idx = sym_idx + data_sect_start;
			}
			return true;
		}
	}

	return false;
}


char* CompleteMachineCode(lang_state* lang_stat, machine_code& code)
{
	ResolveJmpInsts(&code);

	//int& offset = lang_stat->already_inserted_of_type_sect_in_code_sect;
	int offset = 0;

	lang_stat->code_sect.insert(lang_stat->code_sect.begin(), (unsigned char*)lang_stat->type_sect.begin() + offset, (unsigned char*)lang_stat->type_sect.end());

	offset += lang_stat->type_sect.size() - offset;

	code.executable = lang_stat->code_sect.size() - lang_stat->type_sect.size();

	lang_stat->code_sect.insert(lang_stat->code_sect.end(), code.code.begin(), code.code.end());

	auto base_addr = (char*)lang_stat->code_sect.data() + lang_stat->type_sect.size();

	// resolving calls
	FOR_VEC(c, code.call_rels)
	{
		auto to_fill = (int*)&base_addr[code.executable + c->call_idx];
		long long end_call_inst = (long long)((char*)to_fill) + 4;

		long long dst = (long long)&base_addr[c->fdecl->code_start_idx];
		*to_fill = dst - end_call_inst;
	}
	// resolving calls to internal funcs as if they were dll funcs
	FOR_VEC(c, code.rels)
	{
		char* addr = nullptr;
		auto name = std::string(c->name);
		if (lang_stat->internal_funcs_addr.find(name) != lang_stat->internal_funcs_addr.end())
		{
			addr = (char*)lang_stat->internal_funcs_addr[name];

		}
		if (addr != nullptr)
		{
			auto to_fill = (long long*)&base_addr[code.executable + c->idx];
			long long end_call_inst = (long long)((char*)to_fill) + 4;

			long long dst = (long long)addr;
			*to_fill = dst - end_call_inst;
		}
	}
	//code.symbols.insert(code.symbols.end(), lang_stat->type_sect_syms.begin(), plang_stat->type_sect_syms.end());



	auto data = code.code.data();

	char msg_hdr[256];
	//auto exec_funcs = (char*)VirtualAlloc(0, code.code.size(), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	//memcpy(exec_funcs, code.code.data(), code.code.size());


	int data_sect_start = lang_stat->code_sect.size();
	lang_stat->code_sect.insert(lang_stat->code_sect.end(), (unsigned char*)lang_stat->data_sect.begin(), (unsigned char*)lang_stat->data_sect.end());
	//lang_stat->data_sect.clear();

	char* exec_funcs = (char*)lang_stat->code_sect.data() + code.executable + lang_stat->type_sect.size();



	// resolving data rels
	FOR_VEC(c, code.rels)
	{
		if (c->type != machine_rel_type::TYPE_DATA && c->type != machine_rel_type::DATA)
			continue;

		int dst_offset = 0;

		int* to_fill = (int*)&exec_funcs[c->idx];
		long long end_call_inst = (long long)((char*)to_fill) + 4;

		bool found = false;
		unsigned int sym_idx = 0;
		unsigned int sym_idx_aux = 0;
		if (!GetTypeInTypeSect(lang_stat, c, code.symbols, data_sect_start, sym_idx))
		{
			ASSERT(GetTypeInTypeSect(lang_stat, c, lang_stat->type_sect_syms, data_sect_start, sym_idx));
		}

		long long dst = (long long)&lang_stat->code_sect[sym_idx];
		*to_fill = dst - end_call_inst;
	}
	return exec_funcs;
}
void SetNodeIdent(lang_state* lang_stat, node* n, char* data, int len)
{
	n->type = N_IDENTIFIER;
	if (n->t == nullptr)
	{
		n->t = (token2*)AllocMiscData(lang_stat, sizeof(token2));
	}
	n->t->str = std::string(data, len);
	n->flags |= NODE_FLAGS_WAS_MODIFIED;
}
node* GetNodeR(node* n)
{
	return n->r;
}
void* GetProcessedNodeType(comp_time_type_info* tp)
{
	return &tp->val;
}
bool IsNodeType(node* n, node_type tp)
{
	return n->type == tp;
}
node* GetNodeL(node* n)
{
	return n->l;
}
node_type GetNodeType(node* n)
{
	return n->type;
}


struct wasm_interp;
struct wasm_interp
{
	dbg_state *dbg;
	own_std::vector<func_decl*> funcs;
	std::unordered_map<std::string, OutsiderFuncType> outsiders;
};
struct web_assembly_state
{
	own_std::vector<decl2*> types;
	own_std::vector<func_decl*> funcs;
	own_std::vector<decl2*> imports;
	own_std::vector<decl2*> exports;

	std::unordered_map<std::string, OutsiderFuncType> outsiders;

	own_std::vector<unsigned char> final_buf;
	std::string wasm_dir;
	std::string folder_name;

	lang_state* lang_stat;
};
struct wasm_gen_state
{
	own_std::vector<ir_val*> similar;
	int advance_ptr;
	//int strcts_construct_stack_offset;
	//int strcts_ret_stack_offset;
	//int to_spill_offset;

	func_decl* cur_func;
	web_assembly_state* wasm_state;

	int cur_line;
};
void WasmPushIRVal(wasm_gen_state* gen_state, ir_val* val, own_std::vector<unsigned char>& code_sect, bool = false);

func_decl* WasmGetFuncAtIdx(wasm_interp* state, int idx)
{
	return (state->funcs[idx]);
}
func_decl *FuncAddedWasmInterp(wasm_interp* interp, std::string name)
{
	int i = 0;
	bool found = false;
	FOR_VEC(func, interp->funcs)
	{
		func_decl* f = *func;
		if (f->name == name)
		{
			return f;
		}
	}

	return nullptr;
}
bool FuncAddedWasm(web_assembly_state* state, std::string name, int* idx = nullptr)
{
	int i = 0;
	bool found = false;
	FOR_VEC(decl, state->imports)
	{
		decl2* d = (*decl);
		if (d->type.type == TYPE_FUNC && d->type.fdecl->name == name)
		{
			found = true;
			break;
		}
		i++;
	}
	if (!found)
	{
		i = state->imports.size();
		FOR_VEC(func, state->funcs)
		{
			func_decl* f = *func;
			if (f->name == name)
			{
				found = true;
				break;
			}
			i++;
		}
	}
	if (idx)
		*idx = i;

	return found;;
}
void AddFuncToWasm(web_assembly_state* state, func_decl* func, bool export_this = true)
{
	type2 dummy;
	decl2* decl = FindIdentifier(func->name, state->lang_stat->funcs_scp, &dummy);
	func->this_decl = decl;
	ASSERT(decl)
		state->types.emplace_back(decl);
	state->funcs.emplace_back(func);

	if (export_this)
		state->exports.emplace_back(decl);
}
// https://llvm.org/doxygen/LEB128_8h_source.html
/// Utility function to decode a SLEB128 value.
///
/// If \p error is non-null, it will point to a static error message,
/// if an error occured. It will not be modified on success.
inline int64_t decodeSLEB128(const uint8_t* p, unsigned* n = nullptr,
	const uint8_t* end = nullptr,
	const char** error = nullptr) {
	const uint8_t* orig_p = p;
	int64_t Value = 0;
	unsigned Shift = 0;
	uint8_t Byte;
	do {
		if (p == end) {
			if (error)
				*error = "malformed sleb128, extends past end";
			if (n)
				*n = (unsigned)(p - orig_p);
			return 0;
		}
		Byte = *p;
		uint64_t Slice = Byte & 0x7f;
		if (Shift >= 63 &&
			((Shift == 63 && Slice != 0 && Slice != 0x7f) ||
				(Shift > 63 && Slice != (Value < 0 ? 0x7f : 0x00)))) {
			if (error)
				*error = "sleb128 too big for int64";
			if (n)
				*n = (unsigned)(p - orig_p);
			return 0;
		}
		Value |= Slice << Shift;
		Shift += 7;
		++p;
	} while (Byte >= 128);
	// Sign extend negative numbers if needed.
	if (Shift < 64 && (Byte & 0x40))
		Value |= UINT64_MAX << Shift;
	if (n)
		*n = (unsigned)(p - orig_p);
	return Value;
}

// adapted from https://llvm.org/doxygen/LEB128_8h_source.html
inline unsigned int encodeSLEB128(own_std::vector<unsigned char>* OS, int64_t value,
	unsigned PadTo = 0) {
	bool more;
	unsigned count = 0;
	do {
		uint8_t byte = value & 0x7f;
		// NOTE: this assumes that this signed shift is an arithmetic right shift.
		value >>= 7;
		more = !((((value == 0) && ((byte & 0x40) == 0)) ||
			((value == -1) && ((byte & 0x40) != 0))));
		count++;
		if (more || count < PadTo)
			byte |= 0x80; // Mark this byte to show that more bytes will follow.
		OS->emplace_back(byte);
	} while (more);

	// Pad with 0x80 and emit a terminating byte at the end.
	if (count < PadTo) {
		uint8_t PadValue = value < 0 ? 0x7f : 0x00;
		for (; count < PadTo - 1; ++count)
		{
			OS->emplace_back(char(PadValue | 0x80));
		}
		OS->emplace_back(char(PadValue | 0x80));
		count++;
	}
	return count;
}

int GenUleb128(own_std::vector<unsigned char>* out, unsigned int val)
{
	int count = 0;
	bool new_loop = false;
	do {
		new_loop = false;
		unsigned char byte = val & 0x7f;
		if ((val & 0x40) != 0)
			new_loop = true;
		val >>= 7;

		if (val != 0 || new_loop)
			byte |= 0x80;  // mark this byte to show that more bytes will follow

		out->emplace_back(byte);
		count++;
	} while (val != 0 || new_loop);

	return count;
}

#define WASM_TYPE_INT 0
#define WASM_TYPE_FLOAT 1

#define WASM_LOAD_INT 0
#define WASM_LOAD_FLOAT 1



#define WASM_I32_CONST 0x41

#define WASM_STORE_OP 0x36
#define WASM_STORE_F32_OP 0x38
#define WASM_LOAD_OP  0x28
#define WASM_LOAD_F32_OP  0x2a

void WasmPushImm(int imm, own_std::vector<unsigned char>* out)
{
	own_std::vector<unsigned char> uleb;
	encodeSLEB128(&uleb, imm);
	out->insert(out->end(), uleb.begin(), uleb.end());
}
void WasmPushConst(char type_int_or_float, char size, int offset, own_std::vector<unsigned char>* out)
{
	if (type_int_or_float == WASM_TYPE_INT)
	{
		out->emplace_back(WASM_I32_CONST + size);
		own_std::vector<unsigned char> uleb;
		encodeSLEB128(&uleb, offset);
		out->insert(out->end(), uleb.begin(), uleb.end());
	}
	else if (type_int_or_float == WASM_TYPE_FLOAT)
	{
		int f_inst = WASM_I32_CONST + size + 2;
		out->emplace_back(f_inst);
		out->emplace_back(offset & 0xff);
		out->emplace_back(offset >> 8);
		out->emplace_back(offset >> 16);
		out->emplace_back(offset >> 24);

	}
	else
	{
		ASSERT(0);
	}

}
// if it is 64 bit load, make size 1

void WasmStoreInst(lang_state *lang_stat, own_std::vector<unsigned char>& code_sect, int size, char inst = WASM_STORE_OP)
{
	int final_inst = 0;
	if (lang_stat->release && size == 8)
		size = 4;
	switch (inst)
	{
	case WASM_LOAD_F32_OP:
	{
		final_inst = WASM_LOAD_F32_OP;
	}break;
	case WASM_STORE_F32_OP:
	{
		final_inst = WASM_STORE_F32_OP;
	}break;
	case WASM_LOAD_OP:
	{
		switch (size)
		{
		case 1:
			final_inst = 0x2c;
			break;
		case 2:
			final_inst = 0x2e;
			break;
		case 0:
		case 4:
			final_inst = 0x28;
			break;
		case 8:
			final_inst = 0x29;
			break;
		default:
			ASSERT(0);
		}
	}break;
	case WASM_STORE_OP:
	{
		switch (size)
		{
		case 1:
			final_inst = 0x3a;
			break;
		case 2:
			final_inst = 0x3b;
			break;
		case 0:
		case 4:
			final_inst = 0x36;
			break;
		case 8:
			final_inst = 0x37;
			break;
		default:
			ASSERT(0);
		}
	}break;
	default:
		ASSERT(0);
	}
	code_sect.emplace_back(final_inst);
	char alignment = 0;

	switch (size)
	{
	case 1:
		alignment = 0;
		break;
	default:
		alignment = 2;
	}
	/*
	if (size == 1)
		// 8-byte aligned
		code_sect.emplace_back(0x3);
	else if (size == 0)
		code_sect.emplace_back(0x2);
	else
		ASSERT(0);
		*/
	code_sect.emplace_back(alignment);
	// offset
	code_sect.emplace_back(0x0);
}
void WasmPushLoadOrStore(char size, char type, char op_code, int offset, own_std::vector<unsigned char>* out)
{
	WasmPushConst(WASM_TYPE_INT, 0, offset, out);
	ASSERT(size != 8);
	if (type == WASM_LOAD_INT)
	{
		out->emplace_back(op_code + size);
	}
	else if (WASM_LOAD_FLOAT)
	{
		out->emplace_back(op_code + 2 + size);
	}
	else
	{
		ASSERT(0);
	}
	if (size == 1)
		// 8-byte aligned
		out->emplace_back(0x3);
	else if (size == 0)
		out->emplace_back(0x2);
	else
		ASSERT(0);

	// offset
	out->emplace_back(0x0);
}

void WasmGenBinOpImmToReg(lang_state *lang_stat, int reg, char reg_sz, int imm, own_std::vector<unsigned char>& code_sect, char inst_32bit)
{
	ASSERT(reg_sz >= 4)
		int dst_reg = reg;

	int size = reg_sz <= 4 ? 0 : 1;

	// pushing reg dst offset
	WasmPushConst(WASM_TYPE_INT, 0, dst_reg * 8, &code_sect);


	WasmPushLoadOrStore(size, WASM_TYPE_INT, WASM_LOAD_OP, dst_reg * 8, &code_sect);
	WasmPushConst(WASM_TYPE_INT, size, imm, &code_sect);
	// if the size is 1 (64 bit), we will get the 64 bit instruction, which
	// is the 32bit one plus 0x12
	// TODO: this a pattern i noticed, but i dont know if is valid for all of them, so i have to check that
	// 
	code_sect.emplace_back(inst_32bit + (size * 0x12));

	WasmStoreInst(lang_stat, code_sect, size);
}
void WasmGenRegToMem(lang_state *lang_stat, byte_code* bc, own_std::vector<unsigned char>& code_sect)
{
	int src_reg = FromAsmRegToWasmReg(bc->bin.rhs.reg);
	int dst_reg = FromAsmRegToWasmReg(bc->bin.lhs.reg);

	int size = 0;

	// pushing dst reg offset in mem
	WasmPushConst(WASM_TYPE_INT, size, bc->bin.lhs.voffset, &code_sect);

	// getting dst reg val and summing with offset
	WasmPushLoadOrStore(size, WASM_TYPE_INT, WASM_LOAD_OP, dst_reg * 8, &code_sect);
	// 64bit add inst
	code_sect.emplace_back(0x6a);

	// src reg val to mem
	WasmPushLoadOrStore(size, WASM_TYPE_INT, WASM_LOAD_OP, src_reg * 8, &code_sect);

	WasmStoreInst(lang_stat, code_sect, size);
}

static std::string base64_encode(const std::string& in) {

	std::string out;

	int val = 0, valb = -6;
	for (unsigned char c : in) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4) out.push_back('=');
	return out;
}

static std::string base64_decode(const std::string& in) {

	std::string out;

	std::vector<int> T(256, -1);
	for (int i = 0; i < 64; i++) {
		T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
	}

	int val = 0, valb = -8;
	for (unsigned char c : in) {
		if (T[c] == -1) break;
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0) {
			out.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return out;
}

struct x64_rel_info
{
	int bc;
};
struct block_linked
{
	bool used;
	int bc_idx;
	block_linked* parent;
	union
	{
		byte_code* bc;
		wasm_bc* wbc;

		ir_rep* ir;
		int begin;
	};
	own_std::vector<x64_rel_info> rels;
};
void FreeBlock(block_linked* block)
{
	__lang_globals.cur_block--; (__lang_globals.data, (void*)block);
}
block_linked* NewBlock(block_linked* parent)
{
	ASSERT((__lang_globals.cur_block + 1) < __lang_globals.total_blocks)
	auto ret = __lang_globals.blocks + __lang_globals.cur_block;
	memset(ret, 0, sizeof(block_linked));
	__lang_globals.cur_block++;
	ret->parent = parent;
	return ret;

}
void WesmGenMovR(lang_state *lang_stat, int src_reg, int dst_reg, own_std::vector<unsigned char>& code_sect)
{

	// pushing dst reg offset in mem
	WasmPushConst(WASM_TYPE_INT, 0, dst_reg * 8, &code_sect);
	// loading rhs 
	WasmPushLoadOrStore(0, WASM_TYPE_INT, WASM_LOAD_OP, src_reg * 8, &code_sect);


	WasmStoreInst(lang_stat, code_sect, 0);
}
int GetDepthBreak(block_linked* cur, int cur_bc_idx, int jmp_rel)
{
	int depth_to_break = -1;
	int dst_bc_idx = cur_bc_idx + jmp_rel;

	block_linked* b = cur;
	bool found = false;
	while (b->parent && !found && b->bc)
	{
		// the bc is a block inst, which contains the size of the block
		int min = b->bc_idx;
		int max = b->bc_idx + b->bc->val;

		if (dst_bc_idx >= min && dst_bc_idx <= max)
		{
			found = true;
		}
		depth_to_break++;
		b = b->parent;
	}

	ASSERT(depth_to_break > -1);

	return depth_to_break;
}
void GenConditionalJump(own_std::vector<unsigned char>& code_sect, char inst, int cur_bc_idx, int jmp_rel, block_linked* cur)
{
	int eflags = FromAsmRegToWasmReg(13);
	WasmPushLoadOrStore(0, WASM_TYPE_INT, WASM_LOAD_OP, eflags * 8, &code_sect);
	WasmPushConst(WASM_TYPE_INT, 0, 0, &code_sect);

	// not equal (ne) inst
	code_sect.emplace_back(inst);
	int depth_to_break = GetDepthBreak(cur, cur_bc_idx - 1, jmp_rel);

	// br if
	code_sect.emplace_back(0xd);
	WasmPushImm(depth_to_break, &code_sect);
}

void WasmPushLocalSet(own_std::vector<unsigned char>& code_sect, int idx)
{
	code_sect.emplace_back(0x21);
	WasmPushImm(idx, &code_sect);
}
void WasmPushLocalGet(own_std::vector<unsigned char>& code_sect, int idx)
{
	code_sect.emplace_back(0x20);
	WasmPushImm(idx, &code_sect);
}

void WasmPushWhateverIRValIs(lang_state *lang_stat, std::unordered_map<decl2*, int>& decl_to_local_idx, ir_val* val, ir_val* last_on_stack, own_std::vector<unsigned char>& code_sect)
{
	switch (val->type)
	{
	case IR_TYPE_REG:
	{
		WasmPushLoadOrStore(0, WASM_TYPE_INT, WASM_LOAD_OP, val->reg * 8, &code_sect);
	}break;
	case IR_TYPE_INT:
	{
		WasmPushConst(WASM_TYPE_INT, 0, val->i, &code_sect);
	}break;
	case IR_TYPE_DECL:
	{
		int idx = decl_to_local_idx[val->decl];

		WasmPushLoadOrStore(0, WASM_TYPE_INT, WASM_LOAD_OP, BASE_STACK_PTR_REG * 8, &code_sect);
		WasmPushConst(WASM_LOAD_INT, 0, val->decl->offset, &code_sect);
		code_sect.emplace_back(0x6a);
		WasmStoreInst(lang_stat, code_sect, 0, WASM_LOAD_OP);

		//WasmPushLocalGet(code_sect, idx);
	}break;
	default:
		ASSERT(0)
	}
}
enum wstack_val_type
{
	WSTACK_VAL_INT,
	WSTACK_VAL_F32,
};

void WasmPushInst(tkn_type2 op, bool is_unsigned, own_std::vector<unsigned char>& code_sect, wstack_val_type type = WSTACK_VAL_INT)
{
	if (type == WSTACK_VAL_INT)
	{
		switch (op)
		{
		case T_COND_AND:
		case T_AMPERSAND:
		{
			code_sect.emplace_back(0x71);
		}break;
		case T_COND_OR:
		{
			code_sect.emplace_back(0x72);
		}break;
		case T_DIV:
		{
			// unsigned
			if (is_unsigned)
				code_sect.emplace_back(0x6e);
			// signed
			else
				code_sect.emplace_back(0x6d);
		}break;
		case T_GREATER_THAN:
		{
			// unsigned
			if (is_unsigned)
				code_sect.emplace_back(0x4b);
			// signed
			else
				code_sect.emplace_back(0x4a);
		}break;
		case T_LESSER_EQ:
		{
			// unsigned
			if (is_unsigned)
				code_sect.emplace_back(0x4d);
			// signed
			else
				code_sect.emplace_back(0x4c);
		}break;
		case T_LESSER_THAN:
		{
			// unsigned
			if (is_unsigned)
				code_sect.emplace_back(0x49);
			// signed
			else
				code_sect.emplace_back(0x48);
		}break;
		case T_GREATER_EQ:
		{

			// unsigned
			if (is_unsigned)
				code_sect.emplace_back(0x4f);
			// signed
			else
				code_sect.emplace_back(0x4e);
		}break;
		case T_PIPE:
		{
			code_sect.emplace_back(0x72);
		}break;
		case T_MUL:
		{
			code_sect.emplace_back(0x6c);
		}break;
		case T_COND_NE:
		{
			code_sect.emplace_back(0x47);
		}break;
		case T_COND_EQ:
		{
			code_sect.emplace_back(0x46);
		}break;
		case T_MINUS:
		{
			code_sect.emplace_back(0x6b);
		}break;
		case T_POINT:
		case T_PLUS:
		{
			code_sect.emplace_back(0x6a);
		}break;
		case T_PERCENT:
		{
			// unsigned
			if (is_unsigned)
				code_sect.emplace_back(0x70);
			// signed
			else
				code_sect.emplace_back(0x6f);

		}break;
		default:
			ASSERT(0);
		}
	}
	else
	{
		switch (op)
		{
		case T_GREATER_THAN:
		{
			code_sect.emplace_back(0x5e);
		}break;
		case T_LESSER_EQ:
		{
			code_sect.emplace_back(0x5f);
		}break;
		case T_LESSER_THAN:
		{
			code_sect.emplace_back(0x5d);
		}break;
		case T_GREATER_EQ:
		{
			code_sect.emplace_back(0x60);
		}break;
		case T_COND_NE:
		{
			code_sect.emplace_back(0x5c);
		}break;
		case T_COND_EQ:
		{
			code_sect.emplace_back(0x5b);
		}break;
		case T_MUL:
		{
			code_sect.emplace_back(0x94);
		}break;
		case T_DIV:
		{
			code_sect.emplace_back(0x95);
		}break;
		case T_MINUS:
		{
			code_sect.emplace_back(0x93);
		}break;
		case T_PLUS:
		{
			code_sect.emplace_back(0x92);
		}break;
		default:
			ASSERT(0);
		}

	}
}

void WasmIrIncDerefsPushBin(ir_val &lhs, ir_val &rhs)
{
	bool can_inc = !((lhs.type == IR_TYPE_RET_REG || lhs.type == IR_TYPE_REG) && rhs.type == IR_TYPE_INT);
	can_inc = can_inc || (lhs.type == IR_TYPE_DECL && rhs.type == IR_TYPE_INT);
	if (can_inc)
		lhs.deref++;

	if (lhs.deref < 0)
		lhs.deref = 0;
	if (rhs.deref < 0)
		rhs.deref = 0;
}
void WasmPushMultiple(wasm_gen_state* gen_state, bool only_lhs, ir_val& lhs, ir_val& rhs, ir_val* last_on_stack, tkn_type2 op, own_std::vector<unsigned char>& code_sect, bool deref = false)
{
	if (only_lhs)
	{
		WasmPushIRVal(gen_state, &lhs, code_sect, deref);
		//WasmPushWhateverIRValIs(decl_to_local_idx, &lhs, last_on_stack, code_sect);
	}
	else
	{
		char prev_deref_lhs = lhs.deref;
		char prev_deref_rhs = rhs.deref;
		
		//WasmIrIncDerefsPushBin(lhs, rhs);

		wstack_val_type type = WSTACK_VAL_INT;
		if (lhs.is_float)
			type = WSTACK_VAL_F32;

		if (lhs.type == IR_TYPE_REG)
		{
			if(lhs.ptr == -1)
				lhs.ptr = 1;
			deref = true;
		}
		if (op == T_POINT)
		{
			WasmPushIRVal(gen_state, &lhs, code_sect, deref);
			WasmPushConst(WASM_LOAD_INT, 0, rhs.i, &code_sect);
			WasmPushInst(op, lhs.is_unsigned, code_sect);

			//if (deref && rhs.ptr != -1)
				//WasmStoreInst(code_sect, 0, WASM_LOAD_OP);
		}
		else
		{
			WasmPushIRVal(gen_state, &lhs, code_sect, true);
			WasmPushIRVal(gen_state, &rhs, code_sect, true);
			WasmPushInst(op, lhs.is_unsigned, code_sect, type);

		}
		//WasmPushWhateverIRValIs(decl_to_local_idx, &lhs, last_on_stack, code_sect);
		//WasmPushWhateverIRValIs(decl_to_local_idx, &rhs, last_on_stack, code_sect);

		bool is_decl = lhs.type == IR_TYPE_DECL;
		bool is_struct = is_decl && lhs.decl->type.IsStrct(nullptr);
		bool is_float = is_decl && lhs.decl->type.type == TYPE_F32;
		if(rhs.type != IR_TYPE_INT)
			ASSERT( lhs.is_unsigned == rhs.is_unsigned  || is_struct || is_float);

		lhs.deref = prev_deref_lhs;
		rhs.deref = prev_deref_rhs;


	}
}

ir_type GetOppositeCondIR(ir_type type)
{
	switch (type)
	{
	case IR_CMP_EQ:
		return IR_CMP_NE;
	case IR_CMP_NE:
		return IR_CMP_EQ;
	default:
		ASSERT(0)
	}
	return IR_CMP_EQ;
}

void WasmBeginStack(lang_state *lang_stat, own_std::vector<unsigned char>& code_sect, int stack_size)
{
	WesmGenMovR(lang_stat, STACK_PTR_REG, BASE_STACK_PTR_REG, code_sect);
	// sub inst
	WasmGenBinOpImmToReg(lang_stat, STACK_PTR_REG, 4, stack_size, code_sect, 0x6b);
}
void WasmEndStack(lang_state *lang_stat, own_std::vector<unsigned char>& code_sect, int stack_size)
{
	// sum inst
	WasmGenBinOpImmToReg(lang_stat, STACK_PTR_REG, 4, stack_size, code_sect, 0x6a);
	WesmGenMovR(lang_stat, STACK_PTR_REG, BASE_STACK_PTR_REG, code_sect);
}

bool AreIRValsEqual(ir_val* lhs, ir_val* rhs)
{
	if (lhs->type != rhs->type)
		return false;

	switch (lhs->type)
	{
	case IR_TYPE_ON_STACK:
		return lhs->i == rhs->i;

	case IR_TYPE_DECL:
		return lhs->decl == rhs->decl;
	case IR_TYPE_REG:
	case IR_TYPE_PARAM_REG:
	case IR_TYPE_ARG_REG:
	case IR_TYPE_RET_REG:
		return lhs->reg == rhs->reg;
	default:
		ASSERT(0)
	}

	return false;
}

int GetOnStackTypeOffset(func_decl* cur_func, ir_val* val)
{
	int base_offset = 0;
	switch (val->on_stack_type)
	{
	case ON_STACK_STRUCT_RET:
	{
		base_offset = cur_func->strct_ret_size_per_statement_offset;
	}break;
	case ON_STACK_STRUCT_CONSTR:
	{
		base_offset = cur_func->strct_constrct_at_offset;
	}break;
	case ON_STACK_SPILL:
	{
		base_offset = cur_func->to_spill_offset;
	}break;
	default:
		ASSERT(0)
	}
	
	int offset = val->i;
	return -(base_offset - offset);
}

void WasmPushIRVal(wasm_gen_state *gen_state, ir_val *val, own_std::vector<unsigned char> &code_sect, bool deref)
{
	int deref_times = val->deref;
	int prev_reg = val->reg;
	if (val->type == IR_TYPE_REG && val->is_float && val->deref < 0)
		val->reg = val->reg + FLOAT_REG_0;
	switch (val->type)
	{
	case IR_TYPE_GET_FUNC_BC:
	{
	}break;
	case IR_TYPE_STR_LIT:
	{

		lang_state* lang_stat = gen_state->wasm_state->lang_stat;
		int data_offset = lang_stat->data_sect.size();
		InsertIntoDataSect(lang_stat, val->str, strlen(val->str) + 1);
		WasmPushConst(WASM_LOAD_INT, 0, DATA_SECT_OFFSET + data_offset , &code_sect);
		val->on_data_sect_offset = data_offset;
	}break;
	case IR_TYPE_F32:
	{
		WasmPushConst(WASM_LOAD_FLOAT, 0, *(int *) & val->f32, &code_sect);
		//code_sect.emplace_back(0x6a);
	}break;
	case IR_TYPE_INT:
	{
		WasmPushConst(WASM_LOAD_INT, 0, val->i, &code_sect);
		//code_sect.emplace_back(0x6a);
	}break;
	case IR_TYPE_ON_STACK:
	{
		WasmPushLoadOrStore(0, WASM_TYPE_INT, WASM_LOAD_OP, BASE_STACK_PTR_REG * 8, &code_sect);
		/*
		int base_offset = 0;
		switch (val->on_stack_type)
		{
		case ON_STACK_STRUCT_RET:
		{
			base_offset = gen_state->cur_func->strct_ret_size_per_statement_offset;
		}break;
		case ON_STACK_STRUCT_CONSTR:
		{
			base_offset = gen_state->cur_func->strct_constrct_at_offset;
		}break;
		case ON_STACK_SPILL:
		{
			base_offset = gen_state->cur_func->to_spill_offset;
		}break;
		default:
			ASSERT(0)
		}
		
		*/
		int offset = GetOnStackTypeOffset(gen_state->cur_func, val);
		//WasmPushConst(WASM_LOAD_INT, 0, -(base_offset - offset), &code_sect);
		WasmPushConst(WASM_LOAD_INT, 0, offset, &code_sect);
		code_sect.emplace_back(0x6a);
	}break;
	case IR_TYPE_REG:
	{
		//WasmPushLoadOrStore(0, WASM_TYPE_INT, WASM_LOAD_OP, val->reg * 8, &code_sect);
		WasmPushConst(WASM_LOAD_INT, 0, val->reg * 8, &code_sect);
		if (deref)
		{
			deref_times++;
		}
	}break;
	case IR_TYPE_RET_REG:
	{
		WasmPushConst(WASM_LOAD_INT, 0, (RET_1_REG + val->reg) * 8, &code_sect);
		if(deref)
			deref_times++;

	}break;
	case IR_TYPE_ARG_REG:
	{
		WasmPushLoadOrStore(0, WASM_TYPE_INT, WASM_LOAD_OP, STACK_PTR_REG * 8, &code_sect);
		WasmPushConst(WASM_LOAD_INT, 0, val->reg * 8, &code_sect);
		code_sect.emplace_back(0x6a);
	}break;
	case IR_TYPE_PARAM_REG:
	{
		WasmPushLoadOrStore(0, WASM_TYPE_INT, WASM_LOAD_OP, BASE_STACK_PTR_REG * 8, &code_sect);
		WasmPushConst(WASM_LOAD_INT, 0, val->reg * 8, &code_sect);
		code_sect.emplace_back(0x6a);
	}break;
	case IR_TYPE_DECL:
	{
		if (val->decl->type.type == TYPE_FUNC)
		{
			int idx = 0;
			if (IS_FLAG_OFF(val->decl->type.fdecl->flags, FUNC_DECL_LABEL))
			{
				ASSERT(FuncAddedWasm(gen_state->wasm_state, val->decl->name, &idx));
				WasmPushConst(WASM_LOAD_INT, 0, idx, &code_sect);
				deref_times = -1;
			}

		}
		else
		{
			if (IS_FLAG_ON(val->decl->flags, DECL_ABSOLUTE_ADDRESS))
			{
				WasmPushConst(WASM_LOAD_INT, 0, val->decl->offset, &code_sect);
			}
			else if (IS_FLAG_ON(val->decl->flags, DECL_IS_GLOBAL))
			{
				WasmPushConst(WASM_LOAD_INT, 0, GLOBALS_OFFSET + val->decl->offset, &code_sect);
			}
			else
			{
				bool prev_val = val->is_float;
				// if the deref is less or equal than the ptr 
				if (val->decl->type.ptr >= val->deref && IsIrValFloat(val))
					val->is_float = false;

				//int idx = decl_to_local_idx[val->decl];
				WasmPushLoadOrStore(0, WASM_TYPE_INT, WASM_LOAD_OP, BASE_STACK_PTR_REG * 8, &code_sect);
				WasmPushConst(WASM_LOAD_INT, 0, val->decl->offset, &code_sect);
				code_sect.emplace_back(0x6a);
				val->is_float = prev_val;
			}
		}
		//deref = false;
	}break;
	default:
		ASSERT(0)
	}
	val->reg = prev_reg;

	bool is_decl = val->type == IR_TYPE_DECL;
	bool is_decl_not_ptr = is_decl && val->decl->type.ptr == 0;

	while ((deref_times >= 0) && val->type != IR_TYPE_INT && val->type != IR_TYPE_F32 && val->type != IR_TYPE_STR_LIT)
	{

		int reg_sz = val->reg_sz;
		int inst = WASM_LOAD_OP;
		//if (IsIrValFloat(val) && deref_times == 1)

		if (IsIrValFloat(val) && deref_times == 0 && (is_decl_not_ptr || !is_decl_not_ptr && val->deref > val->ptr || !is_decl) && deref)
			inst = WASM_LOAD_F32_OP;
		if (deref_times > 0 || !deref)
		{
			reg_sz = 8;
		}
		if(gen_state->wasm_state->lang_stat->release)
			reg_sz = 4;
		WasmStoreInst(gen_state->wasm_state->lang_stat, code_sect, reg_sz, inst);
		//if(!deref)
			deref_times--;
		//deref = false;
	}
}

void WasmPopToRegister(wasm_gen_state* state, int reg_dst, own_std::vector<unsigned char>& code_sect)
{
	lang_state* lang_stat = state->wasm_state->lang_stat;
	WasmPushConst(WASM_TYPE_INT, 0, BASE_STACK_PTR_REG * 8, &code_sect);
	WasmPushConst(WASM_TYPE_INT, 0, STACK_PTR_REG * 8, &code_sect);
	WasmStoreInst(lang_stat, code_sect, 0, WASM_LOAD_OP);
	WasmStoreInst(lang_stat, code_sect, 0, WASM_LOAD_OP);
	WasmStoreInst(lang_stat, code_sect, 0, WASM_STORE_OP);
	WasmGenBinOpImmToReg(lang_stat, STACK_PTR_REG, 4, 8, code_sect, 0x6a);
}
void WasmPushRegister(wasm_gen_state* state, int reg, own_std::vector<unsigned char>& code_sect)
{
	lang_state* lang_stat = state->wasm_state->lang_stat;
	WasmGenBinOpImmToReg(lang_stat, STACK_PTR_REG, 4, 8, code_sect, 0x6b);

	WasmPushConst(WASM_TYPE_INT, 0, STACK_PTR_REG * 8, &code_sect);
	WasmStoreInst(lang_stat, code_sect, 0, WASM_LOAD_OP);
	WasmPushConst(WASM_TYPE_INT, 0, BASE_STACK_PTR_REG * 8, &code_sect);
	WasmStoreInst(lang_stat, code_sect, 0, WASM_LOAD_OP);
	WasmStoreInst(lang_stat, code_sect, 0, WASM_STORE_OP);
}

void WasmFromSingleIR(std::unordered_map<decl2*, int> &decl_to_local_idx, 
					  int &total_of_args, int &total_of_locals, 
				      ir_rep *cur_ir, own_std::vector<unsigned char> &code_sect, 
					  int *stack_size, own_std::vector<ir_rep> *irs, block_linked **cur,
					  ir_val *last_on_stack, wasm_gen_state *gen_state, int ir_idx)
{
#define PUSH_IMM(i) \
		code_sect.emplace_back(i & 0xff);\
		code_sect.emplace_back(i >> 8);\
		code_sect.emplace_back(i >> 16);\
		code_sect.emplace_back(i >> 24);

	cur_ir->idx = cur_ir - irs->begin();
	gen_state->advance_ptr = 0;
	cur_ir->start = code_sect.size();
	cur_ir->dbg_int = 0;
	lang_state* lang_stat = gen_state->wasm_state->lang_stat;
	switch (cur_ir->type)
	{
	case IR_SPILL:
	{

		code_sect.emplace_back(0x1);
	}break;
	case IR_NOP:
	{
		code_sect.emplace_back(0x1);
	}break;
	case IR_RET:
	{
		if (!cur_ir->ret.no_ret_val)
		{
			WasmPushConst(WASM_TYPE_INT, 0, (RET_1_REG + cur_ir->ret.assign.to_assign.reg) * 8, &code_sect);
			WasmPushMultiple(gen_state, cur_ir->ret.assign.only_lhs, cur_ir->ret.assign.lhs, cur_ir->ret.assign.rhs, last_on_stack, cur_ir->ret.assign.op, code_sect, true);
			//WasmPushLoadOrStore(0, WASM_TYPE_INT, WASM_LOAD_OP, BASE_STACK_PTR_REG * 8, &code_sect);
			int inst = WASM_STORE_OP;
			if(cur_ir->ret.assign.to_assign.is_float)
				inst = WASM_STORE_F32_OP;
			WasmStoreInst(lang_stat, code_sect, cur_ir->ret.assign.to_assign.reg_sz, inst);

		}
		WasmEndStack(lang_stat, code_sect, *stack_size);
		// adding stack_ptr
		//WasmGenBinOpImmToReg(BASE_STACK_PTR_REG, 4, *stack_size, code_sect, 0x6a);

        code_sect.emplace_back(0xf);
	}break;
	case IR_BREAK_OUT_IF_BLOCK:
    {
		int depth = 0;

		block_linked *aux = *cur;
		while (aux->parent)
		{
			if (aux->ir->type == IR_BEGIN_IF_BLOCK || aux->ir->type == IR_BEGIN_LOOP_BLOCK)
                break;
			depth++;
			aux = aux->parent;
		}
		code_sect.emplace_back(0xc);
		WasmPushImm(depth, &code_sect);
		cur_ir->block.other_idx = aux->ir->block.other_idx;
    }break;
	case IR_CONTINUE:
	{
		int depth = 0;

		block_linked *aux = *cur;
		while (aux->parent)
		{
			if (aux->ir->type == IR_BEGIN_LOOP_BLOCK)
                break;
			depth++;
			aux = aux->parent;
		}
		depth--;
		if (aux->ir->block.is_for_loop)
		{
			cur_ir->block.other_idx = aux->ir->block.other_idx - 1;
		}
		else
			cur_ir->block.other_idx = aux->ir->idx;
		code_sect.emplace_back(0xc);
		WasmPushImm(depth, &code_sect);
	}break;
	case IR_BEGIN_STMNT:
	{
		cur_ir->block.stmnt.code_start = code_sect.size();
		gen_state->cur_line = cur_ir->block.stmnt.line;
#ifdef WASM_DBG
		code_sect.emplace_back(0x01);
		code_sect.emplace_back(0x23);
		code_sect.emplace_back(0x45);
		code_sect.emplace_back(0x67);
		
		int line = cur_ir->block.stmnt.line;
		PUSH_IMM(line);
#endif

	}break;
	case IR_END_STMNT:
	{
		stmnt_dbg st;
		ir_rep* begin = irs->begin() + cur_ir->block.other_idx;
		ASSERT(begin->type == IR_BEGIN_STMNT);
		st.start = begin->block.stmnt.code_start;
		st.end = max(st.start, code_sect.size() - 1);
		st.start_ir = cur_ir->block.other_idx;
		st.end_ir = ir_idx;
		st.line = begin->block.stmnt.line;
		ASSERT(st.end >= st.start);
		ASSERT(st.end_ir >= st.start_ir && st.end_ir >= 0);

		//if(st.start != st.end)
		gen_state->cur_func->wasm_stmnts.emplace_back(st);
#ifdef WASM_DBG
		code_sect.emplace_back(0x10);
		code_sect.emplace_back(0x32);
		code_sect.emplace_back(0x54);
		code_sect.emplace_back(0x86);
		PUSH_IMM(st.start_ir);
		PUSH_IMM(st.end_ir);
#endif

	}break;
	case IR_BEGIN_LOOP_BLOCK:
	{
		code_sect.emplace_back(0x3);
		// some blocks have to return void?
		code_sect.emplace_back(0x40);

		*cur = NewBlock(*cur);
		(*cur)->ir = cur_ir;
	}break;
	case IR_END_LOOP_BLOCK:
	{
		code_sect.emplace_back(0xc);
		// some blocks have to return void?
		code_sect.emplace_back(0x0);
		code_sect.emplace_back(0xb);
		FreeBlock(*cur);
		*cur = (*cur)->parent;
	}break;
	case IR_BEGIN_SUB_IF_BLOCK:
	case IR_BEGIN_AND_BLOCK:
	case IR_BEGIN_OR_BLOCK:
	case IR_BEGIN_COND_BLOCK:
	case IR_BEGIN_IF_BLOCK:
	{
		code_sect.emplace_back(0x2);
		// some blocks have to return void?
		code_sect.emplace_back(0x40);

		*cur = NewBlock(*cur);
		(*cur)->ir = cur_ir;
	}break;
	case IR_END_AND_BLOCK:
	case IR_END_OR_BLOCK:
	case IR_END_COND_BLOCK:
	case IR_END_SUB_IF_BLOCK:
	case IR_END_IF_BLOCK:
	{
		FreeBlock(*cur);
		*cur = (*cur)->parent;
		code_sect.emplace_back(0xb);
	}break;
	case IR_CMP_NE:
	case IR_CMP_GT:
	case IR_CMP_LE:
	case IR_CMP_LT:
	case IR_CMP_GE:
	case IR_CMP_EQ:
	{
		ir_rep* next = cur_ir + 1;
		bool val = cur_ir->bin.it_is_jmp_if_true;

		if (cur_ir->bin.it_is_jmp_if_true)
		{
			if (next->type == IR_END_COND_BLOCK || next->type == IR_END_AND_BLOCK)
			{
				val = false;
			}
		}
		else
		{
			if (next->type == IR_END_OR_BLOCK)
			{
				val = true;
			}
		}

		tkn_type2 opposite = cur_ir->bin.op;
		if (cur_ir->bin.it_is_jmp_if_true != val)
			opposite = OppositeCondCmp(opposite);


		int depth = 0;

		block_linked* aux = *cur;
		while (aux->parent)
		{
			//if (!val && (aux->ir->type == IR_BEGIN_LOOP_BLOCK))
				//depth++;
				//break;
			if (!val && (aux->ir->type == IR_BEGIN_OR_BLOCK || aux->ir->type == IR_BEGIN_SUB_IF_BLOCK || aux->ir->type == IR_BEGIN_IF_BLOCK || aux->ir->type == IR_BEGIN_LOOP_BLOCK))
				break;
			if (val && (aux->ir->type == IR_BEGIN_AND_BLOCK || aux->ir->type == IR_BEGIN_COND_BLOCK))
				break;
			depth++;
			aux = aux->parent;
		}


		WasmPushMultiple(gen_state, cur_ir->bin.only_lhs, cur_ir->bin.lhs, cur_ir->bin.rhs, last_on_stack, opposite, code_sect, true);
		if (aux->ir->type == IR_BEGIN_LOOP_BLOCK)
		{
			cur_ir->dst_ir_rel_idx = aux->ir->block.other_idx - cur_ir->idx;
		}
		else
		{
			cur_ir->dst_ir_rel_idx = aux->ir->block.other_idx - cur_ir->idx;
		}

		//WasmPushConst(WASM_TYPE_INT, 0, 7, &code_sect);
		// br if
		code_sect.emplace_back(0xd);
		WasmPushImm(depth, &code_sect);

	}break;
	case IR_ASSIGNMENT:
	{

		gen_state->similar.clear();
		ir_val* lhs = &cur_ir->assign.lhs;;
		ir_val* rhs = &cur_ir->assign.rhs;;

		gen_state->similar.emplace_back(lhs);
		gen_state->similar.emplace_back(rhs);

		ir_rep* next = cur_ir + 1;

		/*
		while(cur_ir->type == next->type && AreIRValsEqual(&cur_ir->assign.to_assign, &next->assign.to_assign)
			&& cur_ir->assign.to_assign.type != IR_TYPE_PARAM_REG)
		{
			if(!AreIRValsEqual(&cur_ir->assign.to_assign, &next->assign.lhs))
				gen_state->similar.emplace_back(&next->assign.lhs);

			gen_state->similar.emplace_back(&next->assign.rhs);
			next++;
			gen_state->advance_ptr++;
		}
		*/

		int prev_reg_sz = cur_ir->assign.to_assign.reg_sz;
		int prev_reg = cur_ir->assign.to_assign.reg;
		if (cur_ir->assign.to_assign.type == IR_TYPE_REG && cur_ir->assign.to_assign.deref >= 0)
			cur_ir->assign.to_assign.reg_sz = 8;

		//if (cur_ir->assign.to_assign.is_float && cur_ir->assign.to_assign.deref < 0)
			//cur_ir->assign.to_assign.reg = FLOAT_REG_0;
		

		bool float_val = cur_ir->assign.to_assign.is_float;
		//cur_ir->assign.to_assign.is_float = false;
		WasmPushIRVal(gen_state, &cur_ir->assign.to_assign, code_sect, false);
		//cur_ir->assign.to_assign.is_float = float_val;

		cur_ir->assign.to_assign.reg_sz = prev_reg_sz;
		cur_ir->assign.to_assign.reg = prev_reg;
		char prev_deref = lhs->deref;
		if (lhs->deref < -1)
			lhs->deref = -1;

		WasmPushMultiple(gen_state, cur_ir->assign.only_lhs, *lhs, *rhs, last_on_stack, cur_ir->assign.op, code_sect, true);

		if (lhs->ptr < 0)
			lhs->deref = prev_deref;

		/*
		if(!cur_ir->assign.only_lhs && cur_ir->assign.op != T_POINT)
			ASSERT(cur_ir->assign.rhs.is_float == cur_ir->assign.lhs.is_float);
			*/
		/*
		for(int i = 2; i < gen_state->similar.size();i++)
		{
			ir_val * r = gen_state->similar[i];
			WasmPushWhateverIRValIs(decl_to_local_idx, r, last_on_stack, code_sect);
			WasmPushInst(cur_ir->assign.op, lhs->is_unsigned, code_sect);
		}
		*/

		int r_sz = cur_ir->assign.to_assign.reg_sz;
		ASSERT(r_sz > 0 && r_sz <= 8);

		switch (cur_ir->assign.to_assign.type)
		{
		case IR_TYPE_REG:
		case IR_TYPE_ON_STACK:
		case IR_TYPE_PARAM_REG:
		case IR_TYPE_ARG_REG:
		case IR_TYPE_RET_REG:
		case IR_TYPE_DECL:
		{
			int inst = WASM_STORE_OP;
			if (cur_ir->assign.to_assign.is_float)
				inst = WASM_STORE_F32_OP;
			WasmStoreInst(lang_stat, code_sect, r_sz, inst);
		}break;
		default:
			ASSERT(0)
		}



		*last_on_stack = cur_ir->assign.to_assign;
		//WasmPushConst(WASM_TYPE_INT, 0, decl_to_local_idx[]);
	}break;
	case IR_END_CALL:
	{
        code_sect.emplace_back(0x10);
        WasmPushImm(gen_state->wasm_state->imports.size() + cur_ir->call.fdecl->wasm_func_sect_idx, &code_sect);
		// sum inst
		WasmGenBinOpImmToReg(lang_stat, STACK_PTR_REG, 4, cur_ir->call.fdecl->args.size() * 8, code_sect, 0x6a);
		WasmPopToRegister(gen_state, BASE_STACK_PTR_REG, code_sect);

	}break;
	case IR_CALL:
	{
		int idx = 0;
		if (IS_FLAG_OFF(cur_ir->call.fdecl->flags, FUNC_DECL_INTRINSIC))
		{
			ASSERT(FuncAddedWasm(gen_state->wasm_state, cur_ir->call.fdecl->name, &idx));
			WasmPushRegister(gen_state, BASE_STACK_PTR_REG, code_sect);
			//WasmPushRegister(gen_state, 0, code_sect);

			code_sect.emplace_back(0x10);
			WasmPushImm(idx, &code_sect);

			//WasmPopToRegister(gen_state, 0, code_sect);
			WasmPopToRegister(gen_state, BASE_STACK_PTR_REG, code_sect);
		}
	}break;
	case IR_BEGIN_CALL:
	{
		ASSERT(FuncAddedWasm(gen_state->wasm_state, cur_ir->call.fdecl->name));
		// sub inst
		WasmPushRegister(gen_state, BASE_STACK_PTR_REG, code_sect);
		WasmGenBinOpImmToReg(lang_stat, STACK_PTR_REG, 4, cur_ir->call.fdecl->args.size() * 8, code_sect, 0x6b);

	}break;
	case IR_INDIRECT_CALL:
	{
		int idx = 0;
		//ASSERT(FuncAddedWasm(gen_state->wasm_state, cur_ir->call.fdecl->name, &idx));
		//WasmPushConst(WASM_TYPE_INT, 0, idx, &code_sect);
		WasmPushRegister(gen_state, BASE_STACK_PTR_REG, code_sect);
		//WasmPushRegister(gen_state, 0, code_sect);
		WasmPushIRVal(gen_state, &cur_ir->bin.lhs, code_sect, true);
		code_sect.emplace_back(0x11);
		code_sect.emplace_back(0x0);
		code_sect.emplace_back(0x0);
		WasmPopToRegister(gen_state, BASE_STACK_PTR_REG, code_sect);
	}break;
	case IR_STACK_END:
	{
		//*stack_size = cur_ir->num;
		// sub inst

		WasmEndStack(lang_stat, code_sect, *stack_size);
		code_sect.emplace_back(0xf);
	}break;
	case IR_STACK_BEGIN:
	{
		*stack_size += cur_ir->fdecl->strct_constrct_size_per_statement;
		cur_ir->fdecl->strct_constrct_at_offset = *stack_size;

		*stack_size += cur_ir->fdecl->to_spill_size * 8;
		cur_ir->fdecl->to_spill_offset = *stack_size;

		*stack_size += cur_ir->fdecl->strct_ret_size_per_statement;
		cur_ir->fdecl->strct_ret_size_per_statement_offset = *stack_size;

		*stack_size += cur_ir->fdecl->biggest_call_args * 8;

		WasmBeginStack(lang_stat, code_sect, *stack_size);
	}break;
	case IR_DECLARE_LOCAL:
	{
		if (IS_FLAG_ON(cur_ir->decl->flags, DECL_IS_GLOBAL))
			break;
		decl_to_local_idx[cur_ir->decl] = total_of_args + total_of_locals;
		total_of_locals++;
		int to_sum = GetTypeSize(&cur_ir->decl->type);
		*stack_size += to_sum <= 4 ? 4 : to_sum;
		cur_ir->decl->offset = -*stack_size;
		
        //gen_state->cur_func->wasm_scp->vars.emplace_back(cur_ir->decl);

	}break;
	case IR_CAST_F32_TO_INT:
	{
		WasmPushIRVal(gen_state, &cur_ir->bin.lhs, code_sect);
		WasmPushIRVal(gen_state, &cur_ir->bin.rhs, code_sect, true);

		if (gen_state->wasm_state->lang_stat->release && cur_ir->bin.rhs.reg_sz == 8)
			cur_ir->bin.rhs.reg_sz = 4;

		code_sect.emplace_back(0xa8);

		WasmStoreInst(lang_stat, code_sect, 4, WASM_STORE_OP);
	}break;
	case IR_CAST_INT_TO_F32:
	{
		WasmPushIRVal(gen_state, &cur_ir->bin.lhs, code_sect);
		WasmPushIRVal(gen_state, &cur_ir->bin.rhs, code_sect, true);

		if (gen_state->wasm_state->lang_stat->release && cur_ir->bin.rhs.reg_sz == 8)
			cur_ir->bin.rhs.reg_sz = 4;
		if (cur_ir->bin.rhs.is_unsigned)
		{
			switch (cur_ir->bin.rhs.reg_sz)
			{
			case 1:
			case 4:
			case 8:
			{
				code_sect.emplace_back(0xb3);
			}break;
			/*
			{
				code_sect.emplace_back(0xb8);
			}break;
			*/
			default:
				ASSERT(0)
			}
		}
		else
		{
			switch (cur_ir->bin.rhs.reg_sz)
			{
			case 8:
			case 1:
			case 4:
			{
				code_sect.emplace_back(0xb2);
			}break;
			/*
			case 8:
			{
				code_sect.emplace_back(0xb7);
			}break;
			*/
			default:
				ASSERT(0)
			}
		}

		WasmStoreInst(lang_stat, code_sect, 4, WASM_STORE_F32_OP);
	}break;
	case IR_DECLARE_ARG:
	{
		decl_to_local_idx[cur_ir->decl] = total_of_args;
	
		int offset_to_args = 8;
		cur_ir->decl->offset = offset_to_args + (total_of_args * 8);
		total_of_args++;

		//gen_state->cur_func->wasm_all_vars.emplace_back(cur_ir->decl);
        //gen_state->cur_func->wasm_scp->vars.emplace_back(cur_ir->decl);
		/*
		cur_ir->decl->offset = *stack_size;
		int to_sum = GetTypeSize(&cur_ir->decl->type);
		*stack_size += to_sum <= 4 ? 4 : to_sum;
		*/
	}break;
	case IR_DBG_BREAK:
	{
		code_sect.emplace_back(0xff);
	}break;
	case IR_BEGIN_BLOCK:
	{
		*cur = NewBlock(*cur);
		(*cur)->ir = cur_ir;

		code_sect.emplace_back(0x2);
		code_sect.emplace_back(0x40);
	}break;
	case IR_END_BLOCK:
	{

		FreeBlock(*cur);
		*cur = (*cur)->parent;
		code_sect.emplace_back(0xb);
	}break;
	case IR_BREAK:
	{
		int depth = 0;

		block_linked *aux = *cur;
		while (aux->parent)
		{
			if (aux->ir->type == IR_BEGIN_LOOP_BLOCK)
                break;

			depth++;
			aux = aux->parent;
		}
		cur_ir->block.other_idx = aux->ir->block.other_idx;
		//if(!aux->ir->block.is_for_loop)
			depth++;
		code_sect.emplace_back(0xc);
		WasmPushImm(depth, &code_sect);
	}break;
	case IR_CAST_INT_TO_INT:
	{
		WasmPushIRVal(gen_state, &cur_ir->bin.lhs, code_sect, false);
		WasmPushIRVal(gen_state, &cur_ir->bin.rhs, code_sect, true);

		if (gen_state->wasm_state->lang_stat->release && cur_ir->bin.rhs.reg_sz == 8)
			cur_ir->bin.rhs.reg_sz = 4;

		WasmStoreInst(lang_stat, code_sect, 4, WASM_STORE_OP);
	}break;
	case IR_BEGIN_COMPLEX:
	{
		//WasmPushIRAddress(gen_state, &cur_ir->complx.dst, code_sect);

		ir_rep* original_ir = cur_ir;

		cur_ir++;
		
		// only trying to push on the stack instead of doing assignments
		while(cur_ir->type != IR_END_COMPLEX)
		{
			// we're not handling complex blocks inside another
			ASSERT(cur_ir->type != IR_BEGIN_COMPLEX);

			switch (cur_ir->type)
			{
			case IR_ASSIGNMENT:
			{
				if (AreIRValsEqual(&cur_ir->assign.to_assign, &original_ir->assign.to_assign))
					break;
				bool to_assign_reg = cur_ir->assign.to_assign.type == IR_TYPE_REG;
				bool lhs_reg = cur_ir->assign.lhs.type == IR_TYPE_REG;
				bool rhs_reg = cur_ir->assign.rhs.type == IR_TYPE_REG;
				bool only_lhs = cur_ir->assign.only_lhs;

				char val = (char)to_assign_reg | (((char)lhs_reg) << 1) | (((char)rhs_reg) << 2);

				ir_val* lhs = &cur_ir->assign.lhs;;
				ir_val* rhs = &cur_ir->assign.rhs;;

				switch (val)
				{
				// everything is reg, meaning they're the stack
				case 7:
				{
					WasmPushInst(cur_ir->assign.op, lhs->is_unsigned, code_sect);
					// make sure that the lhs reg is the same as the reg to assign
					//ASSERT(AreIRValsEqual(&cur_ir->assign.to_assign, &cur_ir->assign.lhs));
				}break;
				// only assign is reg 
				case 5:
				{
					WasmPushMultiple(gen_state, cur_ir->assign.only_lhs, *lhs, *rhs, last_on_stack, cur_ir->assign.op, code_sect);
				}break;
				case 1:
				{
					WasmPushMultiple(gen_state, cur_ir->assign.only_lhs, *lhs, *rhs, last_on_stack, cur_ir->assign.op, code_sect);
				}break;
				default:
					ASSERT(0);
				}

			}break;
			default:
				ASSERT(0);
			}
			gen_state->advance_ptr++;
			cur_ir++;
		}
		ASSERT(cur_ir->type == IR_END_COMPLEX);


		WasmStoreInst(lang_stat, code_sect, 0, WASM_STORE_OP);
		gen_state->advance_ptr++;

	}break;
	case IR_LABEL:
	{
	}break;
	default:
		ASSERT(0);
	}
	cur_ir->end = code_sect.size();
}



struct wasm_stack_val
{
	union
	{
		float f32;
		unsigned int u32;
		int s32;
		char s8;
		short s16;
		unsigned long long u64;
		long long s64;
		wasm_reg reg;
	};
	wstack_val_type type;
};
scope *FindScpWithLine(func_decl* func, int line)
{
	scope* cur_scp = func->scp;
	bool can_break = false;
	while (line >= cur_scp->line_start && line <= cur_scp->line_end && !can_break)
	{
		can_break = true;
		FOR_VEC(scp, cur_scp->children)
		{
			scope* s = *scp;
			if (line >= s->line_start && line <= s->line_end)
			{
				cur_scp = s;
				can_break = false;
				break;
			}
		}
	}
	return cur_scp;
}

bool FindVarWithOffset(func_decl *func, int offset, decl2 **out, int line)
{
	scope* cur_scp = FindScpWithLine(func, line);

	FOR_VEC(d, cur_scp->vars)
	{
		if ((*d)->offset == offset)
		{
			*out = *d;
			return true;
		}
	}

	return false;
}

enum dbg_break_type
{
	DBG_NO_BREAK,   
	DBG_BREAK_RIP_CORRUPTED,
	DBG_BREAK_ON_DIFF_STAT,
	DBG_BREAK_ON_DIFF_STAT_BUT_SAME_FUNC,
	DBG_BREAK_ON_NEXT_STAT,
	DBG_BREAK_ON_NEXT_BC,
	DBG_BREAK_ON_NEXT_IR,
	DBG_BREAK_ON_DIFF_IR,   
	DBG_BREAK_ON_DIFF_IR_BUT_SAME_FUNC,   
};
enum dbg_expr_type
{
	DBG_EXPR_SHOW_SINGLE_VAL,
	DBG_EXPR_SHOW_VAL_X_TIMES,
	DBG_EXPR_FILTER,
};
enum dbg_print_numbers_format
{
	DBG_PRINT_DECIMAL,
	DBG_PRINT_HEX,
};
struct dbg_expr2
{
	decl2 d;
	int multiple_vals;
	int offset;
	std::string str;
	std::string val;
	std::string tp_str;
	own_std::vector<ir_rep> irs;
};
struct dbg_expr
{
	dbg_expr_type type;
	own_std::vector<ast_rep*> expr;
	own_std::vector<ast_rep*> x_times;
	own_std::vector<ir_rep > filter_cond;

	func_decl* from_func;
	std::string exp_str;
};
struct command_info_args
{
	std::string incomplete_str;
	int cur_tkn;
	own_std::vector<token2>* tkns;

};
typedef std::string(*CommandFunc)(dbg_state* dbg, command_info_args *info);
struct command_info
{
	own_std::vector<std::string> names;
	own_std::vector<command_info *> cmds;
	CommandFunc func;
	CommandFunc execute;
	bool end;
};
struct breakpoint
{
	int line;
	byte_code_enum prev_inst;
	byte_code2* bc;
	bool one_time_bp;
};
struct dbg_state
{
	dbg_break_type break_type;
	dbg_print_numbers_format print_numbers_format;
	wasm_bc **cur_bc;
	byte_code2 **cur_bc2;
	ir_rep *cur_ir;
	byte_code2* prev_valid_bc;
	union
	{
		ir_rep* prev_break_ir;
		byte_code2* prev_bc;
	};
	char* mem_buffer;
	int mem_size;
	func_decl* cur_func;
	bool frame_is_from_dbg;
	func_decl* prev_func;
	union
	{
		func_decl* next_stat_break_func;
		func_decl* info_cur_func;
	};
	stmnt_dbg* cur_st;
	stmnt_dbg* prev_st;
	own_std::vector<breakpoint> breakpoints;
	own_std::vector<func_decl*> func_stack;
	own_std::vector<block_linked *> block_stack;
	own_std::vector<wasm_bc*> return_stack;
	own_std::vector<ir_rep*> return_stack_ir;
	own_std::vector<byte_code2 **> return_stack_bc2;
	own_std::vector<wasm_bc> bcs;
	own_std::vector<wasm_stack_val> wasm_stack;
	own_std::vector<dbg_expr *> exprs;
	own_std::vector<dbg_expr2 *> exprs2;
	std::string scene_folder;
	//own_std::vector<command_info> cmds;

	command_info* global_cmd;

	lang_state* lang_stat;
	wasm_interp* wasm_state;
	mem_alloc* dbg_alloc;


	bool some_bc_modified;
	bool *can_execute;

	void* data;
};
void WasmModifyCurBcPtr(dbg_state* dbg, wasm_bc* to)
{
	dbg->some_bc_modified = true;
	(*dbg->cur_bc) = to;
}
enum print_num_type
{
	PRINT_INT,
	PRINT_FLOAT,
	PRINT_CHAR,
};

std::string WasmNumToString(dbg_state* dbg, int num, char limit = -1, print_num_type num_type = PRINT_INT)
{
	std::string ret = "";
	char buffer[32];
	if (num_type == PRINT_FLOAT)
	{
		float num_f = *(float*)&num;
		if(limit == -1)
			snprintf(buffer, 32, "%.3f", num_f);
		else 
			// search how to arbitrary number of decimals in float
			ASSERT(0)
	}
	else if (num_type == PRINT_CHAR)
	{
		char num_ch = (char)num;
		snprintf(buffer, 32, "0x%x %c", num_ch, num_ch);

	}
	else
	{
		switch (dbg->print_numbers_format)
		{
		case DBG_PRINT_DECIMAL:
		{
			if (limit == -1)
				snprintf(buffer, 32, "%d", num);
			else
				snprintf(buffer, 32, "%0*d", limit, num);

		}break;
		case DBG_PRINT_HEX:
		{
			if (limit == -1)
				snprintf(buffer, 32, "0x%x", num);
			else
				snprintf(buffer, 32, "0x%0*x", limit, num);

		}break;
		default:
			ASSERT(0);
		}
	}
	ret = buffer;
	return ret;
}

std::string WasmGetBCString(dbg_state *dbg, func_decl* func, wasm_bc *bc, own_std::vector<wasm_bc>* bcs)
{
	char buffer[64];
	std::string ret = "       ";
	switch (bc->type)
	{
	case WASM_INST_DBG_BREAK:
	{
		ret += "dbg_break";
	}break;
	case WASM_INST_I32_NE:
	{
		ret += "i32.ne";
	}break;
	case WASM_INST_I32_LT_S:
	{
		ret += "i32.lt_s";
	}break;
	case WASM_INST_I32_LE_S:
	{
		ret += "i32.le_u";
	}break;
	case WASM_INST_F32_DIV:
	{
		ret += "f32.div";
	}break;
	case WASM_INST_F32_NE:
	{
		ret += "f32.ne";
	}break;
	case WASM_INST_F32_LE:
	{
		ret += "f32.le";
	}break;
	case WASM_INST_F32_GT:
	{
		ret += "f32.gt";
	}break;
	case WASM_INST_F32_LT:
	{
		ret += "f32.lt";
	}break;
	case WASM_INST_F32_GE:
	{
		ret += "f32.ge";
	}break;
	case WASM_INST_I32_GT_U:
	{
		ret += "i32.gs_u";
	}break;
	case WASM_INST_I32_GT_S:
	{
		ret += "i32.gs_t";
	}break;
	case WASM_INST_I32_LT_U:
	{
		ret += "i32.lt_u";
	}break;
	case WASM_INST_I32_LE_U:
	{
		ret += "i32.le_u";
	}break;
	case WASM_INST_I32_GE_S:
	{
		ret += "i32.ge_s";
	}break;
	case WASM_INST_I32_GE_U:
	{
		ret += "i32.ge_u";
	}break;
	case WASM_INST_I32_EQ:
	{
		ret += "i32.eq";
	}break;
	case WASM_INST_RET:
	{
		ret += "ret";
	}break;
	case WASM_INST_LOOP:
	{
		ret += "loop";
	}break;
	case WASM_INST_I32_STORE8:
	{
		ret += "i32.store8";
	}break;
	case WASM_INST_F32_STORE:
	{
		ret += "f32.store";
	}break;
	case WASM_INST_I64_STORE:
	{
		ret += "i64.store";
	}break;
	case WASM_INST_I32_STORE:
	{
		ret += "i32.store";
	}break;
	case WASM_INST_F32_LOAD:
	{
		ret += "f32.load";
	}break;
	case WASM_INST_I32_LOAD_8_S:
	{
		ret += "i32.load_8_s";
	}break;
	case WASM_INST_I64_LOAD:
	{
		ret += "i64.load";
	}break;
	case WASM_INST_I32_LOAD:
	{
		ret += "i32.load";
	}break;
	case WASM_INST_END:
	{
		ret += "end";
	}break;
	case WASM_INST_I32_DIV_S:
	{
		ret += "i32.div_s";
	}break;
	case WASM_INST_I32_DIV_U:
	{
		ret += "i32.div_u";
	}break;
	case WASM_INST_I32_SUB:
	{
		ret += "i32.sub";
	}break;
	case WASM_INST_F32_SUB:
	{
		ret += "f32.sub";
	}break;
	case WASM_INST_F32_ADD:
	{
		ret += "f32.add";
	}break;
	case WASM_INST_I32_ADD:
	{
		ret += "i32.add";
	}break;
	case WASM_INST_INDIRECT_CALL:
	{
		std::string func_name = WasmGetFuncAtIdx(dbg->wasm_state, bc->i)->name;
		ret += "indirect call ";
		ret += "\t \\" + func_name;
	}break;
	case WASM_INST_CALL:
	{
		std::string func_name = WasmGetFuncAtIdx(dbg->wasm_state, bc->i)->name;
		ret += "call "+ WasmNumToString(dbg, bc->i);
		ret += "\t \\" + func_name;
	}break;
	case WASM_INST_BREAK_IF:
	{
		ret += "br_if "+ WasmNumToString(dbg, bc->i);;
	}break;
	case WASM_INST_BREAK:
	{
		ret += "br " + WasmNumToString(dbg, bc->i);
	}break;
	case WASM_INST_I32_OR:
	{
		ret += "i32.or";
	}break;
	case WASM_INST_CAST_S32_2_F32:
	{
		ret += "f32.cast_s32";
	}break;
	case WASM_INST_CAST_S64_2_F32:
	{
		ret += "f32.cast_s64";
	}break;
	case WASM_INST_F32_MUL:
	{
		ret += "f32.mul";
	}break;
	case WASM_INST_CAST_F32_2_S32:
	{
		ret += "i32.cast_f32";
	}break;
	case WASM_INST_CAST_U32_2_F32:
	{
		ret += "f32.cast_u32";
	}break;
	case WASM_INST_I32_REMAINDER_U:
	{
		ret += "i32.rem_u";
	}break;
	case WASM_INST_I32_REMAINDER_S:
	{
		ret += "i32.rem_s";
	}break;
	case WASM_INST_I32_AND:
	{
		ret += "i32.and";
	}break;
	case WASM_INST_I32_MUL:
	{
		ret += "i32.mul";
	}break;
	case WASM_INST_BLOCK:
	{
		ret += "block";
	}break;
	case WASM_INST_NOP:
	{
		ret += "nop";
	}break;
	case WASM_INST_F32_CONST:
	{
		snprintf(buffer, 64, "f32.const %.3f", bc->f32);
		ret += buffer;
	}break;
	case WASM_INST_I32_CONST:
	{
		bool is_base_ptr = bc->reg == RBS_WASM;
		bool is_n1_load = (bc + 1)->type == WASM_INST_I32_LOAD;
		bool is_n2_i32_const = (bc + 2)->type == WASM_INST_I32_CONST;
		bool is_n3_i32_add = (bc + 3)->type == WASM_INST_I32_ADD;
		
		bool loading_var_address = is_base_ptr && is_n1_load && is_n2_i32_const && is_n3_i32_add;

		wasm_bc* probable_var_offset = bc + 2;
		std::string loaded_var = "";
		decl2* var;
		if (loading_var_address && FindVarWithOffset(func, probable_var_offset->i, &var, dbg->cur_st->line))
		{
			loaded_var = std::string("\t//pushing $") + var->name + " on the stack";
		}
		ret += "i32.const " + WasmNumToString(dbg, bc->i) + loaded_var;
	}break;
	default:
		ASSERT(0);
	}
	return ret;
}
u64 WasmGetRegVal(dbg_state* dbg, int reg)
{
	return *(u64*)&dbg->mem_buffer[reg * 8];

}
std::string WasmIrValToString(dbg_state* dbg, ir_val* val)
{
	std::string ret = "";
	char buffer[64];
	int ptr = 0;
	int deref = val->deref;
	if(deref < 0)
	{
		ret += "&";
		deref++;
	}
	while (ptr < val->deref && val->type != IR_TYPE_INT)
	{
		ret += "*";
		ptr++;
	}
	switch (val->type)
	{
	case IR_TYPE_REG:
	{
		int r_sz = val->reg_sz;
		std::string reg_sz_str = "";
		if (r_sz > 0)
		{
			r_sz = max(r_sz, 4);
			r_sz = 2 << r_sz;
		}
		std::string float_str = "";
		if(val->is_float)
			float_str = "_f32_";
		snprintf(buffer, 32, "reg%s%d[%s]", float_str.c_str(), r_sz, WasmNumToString(dbg, val->reg).c_str());
		ret += buffer;
	}break;
	case IR_TYPE_ON_STACK:
	{
		int base_ptr = WasmGetRegVal(dbg, BASE_STACK_PTR_REG);
		std::string stack_type_name = "";
		switch (val->on_stack_type)
		{
		case ON_STACK_STRUCT_RET:
		{
			base_ptr = (base_ptr - dbg->cur_func->strct_ret_size_per_statement_offset);
			snprintf(buffer, 64, "struct ret(start: %s, at: %s, sz: %d)", 
				WasmNumToString(dbg, base_ptr).c_str(), 
				WasmNumToString(dbg, val->i).c_str(), 0);
			base_ptr += val->i;
			stack_type_name = buffer;
			//base_ptr -= dbg->cur_func->strct_ret_size_per_statement_offset;
			
		}break;
		case ON_STACK_STRUCT_CONSTR:
		{
			stack_type_name = "struct constr";
			base_ptr = base_ptr - (dbg->cur_func->strct_constrct_at_offset - val->i);
		}break;
		case ON_STACK_SPILL:
		{
			stack_type_name = "spill";
			base_ptr = base_ptr - (dbg->cur_func->to_spill_offset - val->i);
		}break;
		default:
			ASSERT(0);
		}
		snprintf(buffer, 64, "%s (address %s)", stack_type_name.c_str(), WasmNumToString(dbg, base_ptr).c_str());
		ret += buffer;
	}break;
	case IR_TYPE_PARAM_REG:
	{
		snprintf(buffer, 32, "param_reg[%s]", WasmNumToString(dbg, val->reg).c_str());
		ret += buffer;
	}break;
	case IR_TYPE_DECL:
	{
		snprintf(buffer, 32, "%s", val->decl->name.c_str());
		ret += buffer;
	}break;
	case IR_TYPE_RET_REG:
	{
		snprintf(buffer, 32, "ret_reg[%s]", WasmNumToString(dbg, val->reg).c_str());
		ret += buffer;
	}break;
	case IR_TYPE_ARG_REG:
	{
		snprintf(buffer, 32, "arg_reg[%s]", WasmNumToString(dbg, val->reg).c_str());
		ret += buffer;
	}break;
	case IR_TYPE_STR_LIT:
	{
		char* data = dbg->lang_stat->data_sect.begin() + val->i;
		snprintf(buffer, 32, "str \"%s\"", data);
		ret += buffer;
	}break;
	case IR_TYPE_F32:
	{
		snprintf(buffer, 32, "%.4f", val->f32);
		ret += buffer;
	}break;
	case IR_TYPE_INT:
	{
		ret += WasmNumToString(dbg, val->i);
	}break;
	default:
		ASSERT(0);
	}
	return ret;
}

std::string WasmGetBinIR(dbg_state* dbg, ir_rep* ir)
{
	char buffer[64];
	std::string lhs = WasmIrValToString(dbg, &ir->bin.lhs);
	std::string rhs = WasmIrValToString(dbg, &ir->bin.rhs);
	std::string op = OperatorToString(ir->bin.op);
	snprintf(buffer, 64, "%s %s %s", lhs.c_str(), op.c_str(), rhs.c_str());
	return buffer;
}

std::string WasmIrAssignment(dbg_state* dbg, assign_info* assign)
{
	char buffer[128];
	std::string ret = "";
	std::string to_assign = WasmIrValToString(dbg, &assign->to_assign);
	std::string lhs = WasmIrValToString(dbg, &assign->lhs);
	if (assign->only_lhs)
		snprintf(buffer, 128, "%s = %s", to_assign.c_str(), lhs.c_str());
	else
	{
		std::string rhs = WasmIrValToString(dbg, &assign->rhs);
		std::string op = OperatorToString(assign->op);
		snprintf(buffer, 128, "%s = %s %s %s", to_assign.c_str(), lhs.c_str(), op.c_str(), rhs.c_str());
	}
	ret = buffer;
	//ret += "\n";
	return ret;

}
void WasmIrToString(dbg_state* dbg, ir_rep *ir, std::string &ret)
{
	char buffer[128];
	ret="";
	switch (ir->type)
	{
	case IR_RET:
	{
		ret += "ret: ";
		if(!ir->ret.no_ret_val)
			ret += WasmIrAssignment(dbg, &ir->ret.assign) + "\n";
		else
			ret += " no return val\n";
	}break;
	case IR_DBG_BREAK:
	{
		ret = "dbg_break\n";
	}break;
	case IR_END_CALL:
	{
		ret = "calling "+ir->call.fdecl->name + " \n";
	}break;
	case IR_INDIRECT_CALL:
	{
		ret = "indirect calling to " + ir->call.fdecl->name + " \n";
	}break;
	case IR_CALL:
	{
		ret = "calling to "+ir->call.fdecl->name + " \n";
	}break;
	case IR_BEGIN_CALL:
	{
		ret = "beginning call to "+ir->call.fdecl->name + " \n";
	}break;
	case IR_CAST_INT_TO_INT:
	{
		std::string lhs = WasmIrValToString(dbg, &ir->bin.lhs);
		std::string rhs = WasmIrValToString(dbg, &ir->bin.rhs);
		snprintf(buffer, 64, "cast int to int: %s = %s\n", lhs.c_str(), rhs.c_str());
		ret = buffer;
	}break;
	case IR_CMP_NE:
	case IR_CMP_GT:
	case IR_CMP_LT:
	case IR_CMP_EQ:
	case IR_CMP_LE:
	case IR_CMP_GE:
	{
		ret = WasmGetBinIR(dbg, ir) + "\n";
	}break;
	case IR_BEGIN_BLOCK:
	{
		ret = "begin block\n";
	}break;
	case IR_END_BLOCK:
	{
		ret = "end block\n";
	}break;
	case IR_BEGIN_SUB_IF_BLOCK:
	{
		ret = "begin sub if loop\n";
	}break;
	case IR_END_SUB_IF_BLOCK:
	{
		ret = "end sub if loop\n";
	}break;
	case IR_BREAK_OUT_IF_BLOCK:
	{
		ret = "break if block\n";
	}break;
	case IR_BEGIN_OR_BLOCK:
	{
		ret = "begin or block\n";
	}break;
	case IR_END_OR_BLOCK:
	{
		ret = "end or block\n";
	}break;
	case IR_END_LOOP_BLOCK:
	{
		ret = "end loop\n";
	}break;
	case IR_BEGIN_LOOP_BLOCK:
	{
		ret = "begin loop\n";
	}break;
	case IR_BEGIN_COND_BLOCK:
	{
		ret = "begin cond block\n";
	}break;
	case IR_BREAK:
	{
		ret = "break";
	}break;
	case IR_CAST_INT_TO_F32:
	{
		ret = "cast int to f32\n";
		std::string lhs = WasmIrValToString(dbg, &ir->bin.lhs);
		std::string rhs = WasmIrValToString(dbg, &ir->bin.rhs);
		snprintf(buffer, 64, "cast int to f32: %s = %s\n", lhs.c_str(), rhs.c_str());
		ret = buffer;
	}break;
	case IR_END_COND_BLOCK:
	{
		ret = "end cond block\n";
	}break;
	case IR_CAST_F32_TO_INT:
	{
		std::string lhs = WasmIrValToString(dbg, &ir->bin.lhs);
		std::string rhs = WasmIrValToString(dbg, &ir->bin.rhs);
		snprintf(buffer, 64, "cast f32 to int: %s = %s\n", lhs.c_str(), rhs.c_str());
		ret = buffer;
	}break;
	case IR_CONTINUE:
	{
		ret = "continue";
	}break;
	case IR_END_IF_BLOCK:
	{
		ret = "end if block\n";
	}break;
	case IR_END_AND_BLOCK:
	{
		ret = "end and block\n";
	}break;
	case IR_BEGIN_AND_BLOCK:
	{
		ret = "begin and block\n";
	}break;
	case IR_BEGIN_IF_BLOCK:
	{
		ret = "begin if block\n";
	}break;
	case IR_STACK_BEGIN:
	case IR_STACK_END:
	case IR_BEGIN_STMNT:
	case IR_DECLARE_LOCAL:
	case IR_DECLARE_ARG:
	case IR_END_STMNT:
	case IR_NOP:
	{

	}break;
	case IR_ASSIGNMENT:
	{
		ret+= WasmIrAssignment(dbg, &ir->assign);
		ret += "\n";
	}break;
	default:
		ASSERT(0);
	}
}

void WasmGetIrWithIdx(dbg_state* dbg, func_decl *func, int idx, ir_rep **ir_start, int *len_of_same_start, int start = -1, int end = -1)
{

	if (start == -1)
		dbg->cur_st->start;
	if (end == -1)
		dbg->cur_st->end;
	own_std::vector<ir_rep>* ir_ar = (own_std::vector<ir_rep> *) &func->ir;
	ir_rep* ir = ir_ar->begin();
	for (int i = 0; i < ir_ar->size(); i++)
	{
		if (ir->start == idx)
		{
			*ir_start = ir;
			(*len_of_same_start) = 0;
			while (ir->start == idx)
			{
				(*len_of_same_start)++;
				ir++;
			}

			return;
		}
		ir++;
	}
	*ir_start = nullptr;
}

std::string WasmPrintVars(dbg_state *dbg)
{
	std::string ret = "";
	char* mem_buffer = dbg->mem_buffer;
	func_decl* func = dbg->cur_func;
	stmnt_dbg* stmnt = dbg->cur_st;
	own_std::vector<wasm_bc>* bcs = &dbg->bcs;

	int base_stack_ptr = *(int *)&mem_buffer[BASE_STACK_PTR_REG * 8];
	printf("**vars and params\n");
	scope* cur_scp = FindScpWithLine(func, dbg->cur_st->line);
	while(cur_scp->parent)
	{
		FOR_VEC(decl, cur_scp->vars)
		{
			decl2* d = *decl;
			if (d->type.type == TYPE_FUNC || d->type.type == TYPE_FUNC_TYPE || d->type.type == TYPE_STRUCT_TYPE || d->type.type == TYPE_FUNC_PTR)
				continue;

			int val = *(int*)&mem_buffer[base_stack_ptr + d->offset];

			std::string type;

			if (IS_FLAG_ON(d->flags, DECL_IS_ARG))
				type = "param";
			else
				type = "local";

			std::string offset_str = WasmNumToString(dbg, d->offset);
			std::string val_str = WasmNumToString(dbg, val);

			char buffer[512];
			sprintf(buffer, "%s: %s, value: %s, offset: %s\n", type.c_str(), d->name.c_str(), val_str.c_str(), offset_str.c_str());
			ret += buffer;
		}
		cur_scp = cur_scp->parent;
	}
	//printf("wasm:\n %s", WasmPrintCode(mem_buffer, func, stmnt, bcs).c_str());
	ret += "\n\n";
	return ret;
}
long long WasmInterpGetReVal(char* mem_buffer, int reg, bool deref)
{
	auto ptr = *(long long *)&mem_buffer[reg * 8];
	if (deref)
		return *(long long*)&mem_buffer[ptr];
	return ptr;
}

ir_rep* GetIrBasedOnOffset(dbg_state* dbg, int offset, int start_ir, int end_ir)
{
	own_std::vector<ir_rep>* ir_ar = (own_std::vector<ir_rep> *) &dbg->cur_func->ir;
	ir_rep* ir = ir_ar->begin() + start_ir;
	ir_rep* end = ir_ar->begin() + end_ir;
	while(ir < end)
	{
		if (offset == ir->start)
			return ir;
		ir++;
	}
	return nullptr;
}
ir_rep *GetIrBasedOnOffset(dbg_state *dbg, int offset)
{
	own_std::vector<ir_rep>* ir_ar = (own_std::vector<ir_rep> *) &dbg->cur_func->ir;
	ir_rep* ir = ir_ar->begin();
	ir_rep* end = ir_ar->end();
	while(ir < end)
	{
		if (offset >= ir->start && offset <= ir->end)
			return ir;
		ir++;
	}
	return nullptr;
}
stmnt_dbg* GetStmntBasedOnOffsetIr(own_std::vector<stmnt_dbg>* ar, int offset)
{
	stmnt_dbg* st = ar->begin();
	stmnt_dbg* end = ar->end();
	while(st < end)
	{
		if (offset >= st->start_ir && offset < st->end_ir)
			return st;
		st++;
	}
	return nullptr;
}
stmnt_dbg* GetStmntBasedOnOffset(own_std::vector<stmnt_dbg>* ar, int offset)
{
	stmnt_dbg* st = ar->begin();
	stmnt_dbg* end = ar->end();
	while(st < end)
	{
		if (offset >= st->start && offset < st->end)
			return st;
		st++;
	}
	return nullptr;
}

#define DBG_STATE_ON_LANG 0
#define DBG_STATE_ON_WASM 1



std::string WasmGetStack(dbg_state* dbg)
{
	std::string ret = "stack: \n";
	char buffer[512];
	int i = 0;
	FOR_VEC(s, dbg->wasm_stack)
	{
		std::string type = "";
		std::string val_str = "";
		switch (s->type)
		{
			// f32
		case WSTACK_VAL_F32:
		{
			snprintf(buffer, 512, "%.3f", s->f32);
			val_str = buffer;
			type = "f32";
		}break;
		case WSTACK_VAL_INT:
		{
			val_str = WasmNumToString(dbg, s->s32);
			type = "i32";
		}break;
		default:
			ASSERT(0);
		}
		sprintf(buffer, "%d: %s %s\n", i, type.c_str(), val_str.c_str());
		ret += buffer;
		i++;
	}

	return ret;
}

struct typed_stack_val
{
	ast_rep* a;
	bool decl;
	int offset;
	type2 type;
	bool no_deref;
};

int WasmGetMemOffsetVal(dbg_state* dbg, unsigned int offset)
{
	if (offset > dbg->mem_size)
		return -1;
	return *(int*)&dbg->mem_buffer[offset];
}
int WasmGetDeclVal(dbg_state* dbg, int offset)
{
	int mem = *(int*)&dbg->mem_buffer[BASE_STACK_PTR_REG * 8];
	return *(int*)&dbg->mem_buffer[mem + offset];
}
void WasmFromAstArrToStackVal(dbg_state* dbg, own_std::vector<ast_rep *> expr, typed_stack_val* out)
{
	own_std::vector<typed_stack_val> expr_stack;
	FOR_VEC(ast, expr)
	{
		ast_rep* a = *ast;
		typed_stack_val val = {};
		val.a = a;

		switch (a->type)
		{
		case AST_DEREF:
		{
			typed_stack_val *top = &expr_stack.back();
			expr_stack.pop_back();

			int deref_times = a->deref.times;
			while (deref_times > 0)
			{
				top->offset = WasmGetMemOffsetVal(dbg, top->offset);
				top->type.ptr--;
				deref_times--;
			}
			val = *top;
		}break;
		case AST_BINOP:
		{
			switch (a->op)
			{
			case T_POINT:
			{
				typed_stack_val *top = expr_stack.end();
				typed_stack_val* first = top - a->points.size();

				expr_stack.pop_back();
				typed_stack_val* punultimate = first;
				for (int i = 1; i < a->points.size(); i++)
				{
					while (punultimate->type.ptr > 0)
					{
						punultimate->offset = WasmGetMemOffsetVal(dbg, punultimate->offset);
						punultimate->type.ptr--;
					}
					typed_stack_val* next = first + i;
					punultimate->offset += next->type.e_decl->offset;
					punultimate->type = next->type;
				}
				for (int i = 0; i < a->points.size(); i++)
				{
					expr_stack.pop_back();
				}
				expr_stack.emplace_back(*punultimate);
			}break;
			default:
				ASSERT(0);
			}
			typed_stack_val *top = &expr_stack.back();
			ASSERT(top->decl == true)
			expr_stack.pop_back();
			val.type = top->type;
			val.type.ptr;
			val.offset = top->offset;
			char ptr = val.type.ptr;
			//val.offset = WasmGetMemOffsetVal(dbg, val.offset);
			//val.type.ptr--;
		}break;
		case AST_UNOP:
		{
			// only handling this un op at the moment
			ASSERT(a->op == T_MUL);
			typed_stack_val *top = &expr_stack.back();
			ASSERT(top->decl == true)
			expr_stack.pop_back();
			val.type = top->type;
			val.type.ptr++;
			val.offset = top->offset;
		}break;
		case AST_CAST:
		{
			typed_stack_val *top = &expr_stack.back();
			expr_stack.pop_back();
			val.a = top->a;
			val.offset = top->offset;
			decl2* d = top->type.e_decl;
			val.type = a->cast.type;
			//val.type.type = FromTypeToVarType(a->cast.type.type)
			val.type.e_decl = d;

			if (a->cast.type.ptr > 0 && top->type.type == TYPE_S32)
				val.no_deref = true;

			val.offset = top->offset;
		}break;
		case AST_INT:
		{
			val.type.type = TYPE_S32;
			val.offset = a->num;
		}break;
		case AST_IDENT:
		{
			val.type = a->decl->type;
			val.type.e_decl = a->decl;
			val.decl = true;
			val.offset = WasmGetRegVal(dbg, BASE_STACK_PTR_REG) + a->decl->offset;
			//int decl_val = WasmGetDeclVal(dbg, a->decl->offset);

			//val.type.u32 = decl_val;
		}break;
		default:
			ASSERT(0);
		}
		expr_stack.emplace_back(val);
	}
	ASSERT(expr_stack.size() == 1);
	*out = expr_stack.back();

}
std::string WasmGenRegStr(dbg_state* dbg, int reg, int reg_sz, std::string reg_name)
{
	unsigned int reg_val = WasmGetMemOffsetVal(dbg, reg);
	char buffer[64];
	std::string reg_idx = WasmNumToString(dbg, reg);
	std::string reg_val_str = WasmNumToString(dbg, reg_val);
	std::string ptr_val_str;
	std::string reg_size_str;

	if (reg_sz > 4)
		reg_sz = 4;

	reg_sz = 2 << reg_sz;

	if (reg_sz != 0)
		reg_size_str = std::to_string(reg_sz);


	if (reg_val < 0 || reg_val >dbg->mem_size)
		ptr_val_str = "out of bounds";
	else
	{
		int ptr_deref_val = WasmGetMemOffsetVal(dbg, reg_val);
		ptr_val_str = WasmNumToString(dbg, ptr_deref_val);
	}

	snprintf(buffer, 64, "%s%s[%s] = %s {%s}", reg_name.c_str(), reg_size_str.c_str(), reg_idx.c_str(), reg_val_str.c_str(), ptr_val_str.c_str());
	return buffer;

}
void WasmPrintRegisters(dbg_state* dbg)
{
	std::string str = "";
	char buffer[64];
	for (int i = 0; i < 16; i++)
	{
		auto reg = (wasm_reg)(i * 8);

		switch (reg)
		{
		case wasm_reg::RS_WASM:
		{

			str += WasmGenRegStr(dbg, reg, 4, "stack");
			str += "\n";
		}break;
		case RBS_WASM:
		{
			str += WasmGenRegStr(dbg, reg, 4, "base");
			str += "\n";
		}break;
		case R5_WASM:
		case R4_WASM:
		case R3_WASM:
		case R2_WASM:
		case R1_WASM:
		case R0_WASM:
		{
			str += WasmGenRegStr(dbg, reg, 4, "reg");
			str += "\n";
		}break;
		default:
			break;
		}
	}
	int stack_ptr = WasmGetMemOffsetVal(dbg, STACK_PTR_REG * 8);
	for (int i = 0; i < 4; i++)
	{
		str += WasmGenRegStr(dbg, stack_ptr + (i * 8), 4, "arg_reg");
		str += "\n";
	}
	printf("%s", str.c_str());
}
std::string WasmVarToString(dbg_state* dbg, char indent, decl2* d, int struct_offset, char ptr, bool is_self_ref = false)
{

	char buffer[512];
	char indent_buffer[32];

	memset(indent_buffer, ' ', indent);
	indent_buffer[indent] = 0;

	std::string ptr_val_str = "";
	if (ptr > 0)
	{
		int ptr_val = WasmGetMemOffsetVal(dbg, struct_offset);
		ptr_val_str = std::string("(*") + WasmNumToString(dbg, ptr_val) + std::string(")");
	}
	std::string ret;
	if (!is_self_ref && (d->type.type == TYPE_STRUCT || d->type.type == TYPE_STRUCT_TYPE))
	{

		//char ptr = ptr;
		while (ptr > 0)
		{
			struct_offset = WasmGetMemOffsetVal(dbg, struct_offset);
			break;
		}

		snprintf(buffer, 512, "%s%s%s : {\n", indent_buffer, d->name.c_str(), ptr_val_str.c_str());
		ret += buffer;
		scope* strct_scp = d->type.strct->scp;

		indent++;
		memset(indent_buffer, ' ', indent);
		indent_buffer[indent] = 0;


		if (ptr > 0 && struct_offset < 128)
		{
		}
		else
		{

			//std::string ptr_addr = WasmNumToString(dbg, struct_offset);
			FOR_VEC(struct_d, strct_scp->vars)
			{
				decl2* ds = *struct_d;
				bool is_self_ref = ds->type.type == TYPE_STRUCT && ds->type.strct == d->type.strct;
				if (ds->type.type == TYPE_STRUCT_TYPE)
					continue;
				std::string var_str = WasmVarToString(dbg, indent, ds, struct_offset + ds->offset, ds->type.ptr, is_self_ref);
				snprintf(buffer, 512, "%s,\n", var_str.c_str());
				ret += buffer;
			}
			if (strct_scp->vars.size() > 0)
			{
				ret.pop_back();
				ret.pop_back();
			}
		}

		indent--;
		memset(indent_buffer, ' ', indent);
		indent_buffer[indent] = 0;

		snprintf(buffer, 512, "\n%s}", indent_buffer);
		ret += buffer;
	}
	else// if(d->type.type != TYPE_STRUCT_TYPE)
	{

		std::string val_str = "";
		if (struct_offset < 128)
			val_str = "0";
		else
		{
			print_num_type ptype = d->type.type == TYPE_F32 ? PRINT_FLOAT : PRINT_INT;
			val_str = WasmNumToString(dbg, WasmGetMemOffsetVal(dbg, struct_offset), -1, ptype);
		}
		snprintf(buffer, 512, "%s%s%s: %s", indent_buffer, d->name.c_str(), ptr_val_str.c_str(), val_str.c_str());
		ret += buffer;
	}

	return ret;
}
std::string WasmGetSingleExprToStr(dbg_state* dbg, dbg_expr* exp)
{
	std::string ret = "expression: " + exp->exp_str;

	//if(exp->)
	typed_stack_val expr_val;
	WasmFromAstArrToStackVal(dbg, exp->expr, &expr_val);
	char buffer[512];
	std::string addr_str = WasmNumToString(dbg, expr_val.offset);

	int ptr = expr_val.type.ptr;

	int in_ptr_addr = WasmGetMemOffsetVal(dbg, expr_val.offset);
	while (ptr > 0)
	{
		if (in_ptr_addr > dbg->mem_size || in_ptr_addr < 0)
			snprintf(buffer, 512, "->out of bounds");
		else
			snprintf(buffer, 512, "->%s", WasmNumToString(dbg, in_ptr_addr).c_str());
		addr_str += buffer;
		in_ptr_addr = WasmGetMemOffsetVal(dbg, in_ptr_addr);
		ptr--;
	}
	if (expr_val.type.type == TYPE_STRUCT)
	{
		decl2* d = expr_val.type.strct->this_decl;
		//std::string WasmVarToString(dbg_state* dbg, char indent, decl2* d, int struct_offset)


		ptr = expr_val.type.ptr;
		if (ptr > 0)
		{
			expr_val.offset = WasmGetMemOffsetVal(dbg, expr_val.offset);
			ptr--;
		}
		

		std::string struct_str = WasmVarToString(dbg, 0, d, expr_val.offset, ptr);
		snprintf(buffer, 512," addr(%s) (%s) %s ", addr_str.c_str(), TypeToString(expr_val.type).c_str(), struct_str.c_str());
	}
	else
	{
		int val = 0;
		print_num_type print_type = PRINT_INT;
		std::string str_val = "";
		if (expr_val.type.ptr > 0)
		{
			val = WasmGetMemOffsetVal(dbg, expr_val.offset);
		}
		else if (expr_val.type.type == TYPE_STR_LIT)
		{
			val = WasmGetMemOffsetVal(dbg, expr_val.offset);
			auto addr = (char*)&dbg->mem_buffer[val];
			snprintf(buffer, 512, "%s", addr);
			str_val = buffer;

			//unsigned int len = strlen(addr);
			//len = 0;
		}
		else
		{
			switch (expr_val.type.type)
			{
			case TYPE_STATIC_ARRAY:
			{
				val = expr_val.offset;

				exp->type = DBG_EXPR_SHOW_VAL_X_TIMES;
				ast_rep* ast = NewAst();
				ast->type = AST_INT;
				ast->num = expr_val.type.ar_size;
				exp->x_times.clear();
				exp->x_times.emplace_back(ast);
			}break;
			case TYPE_CHAR:
			{
				val = WasmGetMemOffsetVal(dbg, expr_val.offset) & 0xff;
				print_type = PRINT_CHAR;
			}break;
			case TYPE_U8:
			case TYPE_BOOL:
			{
				val = WasmGetMemOffsetVal(dbg, expr_val.offset) & 0xff;

			}break;
			case TYPE_VOID:
			case TYPE_U64:
			case TYPE_S32:
			case TYPE_U32:
			{
				val = WasmGetMemOffsetVal(dbg, expr_val.offset);

			}break;
			case TYPE_F32:
			{
				val = WasmGetMemOffsetVal(dbg, expr_val.offset);
				print_type = PRINT_FLOAT;
			}break;
			default:
				ASSERT(0);
			}
			str_val = WasmNumToString(dbg, val, -1, print_type);
		}
	
		snprintf(buffer, 512, "addr(%s) (%s) %s ", addr_str.c_str(), TypeToString(expr_val.type).c_str(), str_val.c_str());
	}
	ret += buffer;

	switch (exp->type)
	{
	case DBG_EXPR_SHOW_SINGLE_VAL:
	{
		if (expr_val.type.ptr > 0)
		{

			int ptr_deref_val = WasmGetMemOffsetVal(dbg, expr_val.offset);

			snprintf(buffer, 512, "{%s}", WasmNumToString(dbg, ptr_deref_val).c_str());
			ret += buffer;

		}
	}break;
	case DBG_EXPR_SHOW_VAL_X_TIMES:
	{
		typed_stack_val x_times;
		WasmFromAstArrToStackVal(dbg, exp->x_times, &x_times);
		ASSERT(expr_val.type.ptr > 0  || expr_val.type.ptr == -1 || expr_val.type.type == TYPE_STATIC_ARRAY);
		ASSERT(x_times.type.ptr == 0);

		void *ptr_buffer = (void *) & dbg->mem_buffer[expr_val.offset];
		//if(1expr_val.no_deref)
		//ptr_buffer = *) & dbg->mem_buffer[expr_val.offset];
		std::string show = "";
		if (expr_val.type.ptr == 1 || expr_val.type.type == TYPE_STATIC_ARRAY)
		{
			if (expr_val.type.type != TYPE_STATIC_ARRAY)
			{
				if (!expr_val.no_deref)
					ptr_buffer = &dbg->mem_buffer[*(int*)ptr_buffer];
			}
			else
			{
				expr_val.type = *expr_val.type.tp;
			}
			switch (expr_val.type.type)
			{
			case TYPE_U8:
			{
			}break;
			case TYPE_F32:
			{
				for (int i = 0; i < x_times.offset; i++)
				{
					unsigned int num = *(((unsigned int*)ptr_buffer) + i);
					show += WasmNumToString(dbg, num, -1, PRINT_FLOAT);
					show += ", ";
				}

				show.pop_back();
				show.pop_back();
			}break;
			case TYPE_U32:
			{
				for (int i = 0; i < x_times.offset; i++)
				{
					unsigned int num = *(((unsigned int*)ptr_buffer) + i);
					show += WasmNumToString(dbg, num);
				}
			}break;
			case TYPE_CHAR:
			{
				for (int i = 0; i < x_times.offset; i++)
				{
					show += *(((char*)ptr_buffer) + i);
				}
			}break;
			case TYPE_STRUCT:
			{
				
				if (x_times.offset > 10)
				{
					show.reserve(1024 * 4);
				}
				for (int i = 0; i < x_times.offset; i++)
				{
					snprintf(buffer, 512, "%d: \n", i);
					show += buffer;
					show += WasmVarToString(dbg, 0, expr_val.type.strct->this_decl, expr_val.offset + i * expr_val.type.strct->size, 0);
				}
			}break;
			default:
			{
				ASSERT(0);
			}break;
			}
		}
		else if (expr_val.type.ptr > 1)
		{
			for (int i = 0; i < x_times.offset; i++)
			{
				show += WasmNumToString(dbg, *(((unsigned int*)ptr_buffer) + i * 8));
				show += ", ";
			}
			if (x_times.offset > 0)
			{
				show.pop_back();
				show.pop_back();
			}
		}
		snprintf(buffer, 512, " {%s} ", show.c_str());
		ret += buffer;

	}break;
	default:
		ASSERT(0);
	}

	return ret;
}

void WasmPrintExpressions(dbg_state* dbg)
{
	FOR_VEC(e, dbg->exprs)
	{
		printf("%s\n", WasmGetSingleExprToStr(dbg, *e).c_str());
	}
}

/*
void WasmPrintStackAndBcs(dbg_state* dbg, int max_bcs = -1)
{
	printf("%s\n", WasmPrintCodeGranular(dbg, dbg->cur_func, *dbg->cur_bc, dbg->cur_st->start, dbg->cur_st->end, max_bcs).c_str());
	printf("%s\n", WasmGetStack(dbg).c_str());
}
*/

void WasmBreakOnNextStmnt(dbg_state* dbg, bool *args_break)
{
	stmnt_dbg* next = dbg->cur_st;
	stmnt_dbg* prev = next;
	next++;
	while (next->line == prev->line && next <= &dbg->cur_func->wasm_stmnts.back())
	{
		prev = next;
		next++;
	}
	if (next <= &dbg->cur_func->wasm_stmnts.back())
	{
		wasm_bc* next_bc = dbg->bcs.begin() + next->start;
		next_bc->one_time_dbg_brk = true;
		dbg->next_stat_break_func = dbg->cur_func;
		dbg->break_type = DBG_BREAK_ON_DIFF_STAT_BUT_SAME_FUNC;
	}

	*args_break = true;
}

void printInput(const std::string& input, size_t cursorPos) {
	std::cout << "\033[2K\r" << input<<"\r";  // Clear the current line and print the input
	if(cursorPos > 0)
		std::cout << "\033[" << cursorPos<< "C";  // Move the cursor back

}
command_info* GetSuggestion(command_info *cur, std::string incomplete_str)
{

	FOR_VEC(cmd, cur->cmds)
	{
		if (cur->end)
		{
			return cur;
		}
		else
		{
			command_info* c = *cmd;

			bool choose_command = false;
			FOR_VEC(name, c->names)
			{
				std::string n = *name;
				bool is_equal = true;
				for (int j = 0; j < incomplete_str.size(); j++)
				{
					if (incomplete_str[j] != n[j])
					{
						is_equal = false;
						break;
					}
				}
				if (is_equal)
					return c;
			}
		}
	}
	return nullptr;
}
void InsertSuggestion(dbg_state* dbg, std::string &input, int *cursor_pos)
{
	own_std::vector<std::string> args;

	//std::string aux;
	//split(input, ' ', args, &aux);
	own_std::vector<token2> tkns;
	Tokenize2((char *)input.c_str(), input.size(), &tkns);

	command_info* cur = dbg->global_cmd;
	command_info* last = nullptr;
	// poping EOF
	tkns.pop_back();
	
	FOR_VEC(t, tkns)
	{
		if (t->type == T_COMMA)
			continue;

		if (cur->end)
		{
			if ((t + 1)->type == T_COMMA)
			{
				cur = last;
				continue;
			}
			if (cur->func)
			{
				command_info_args args;
				args.incomplete_str = t->str;

				ASSERT(cur->func);
				std::string final_str = cur->func(dbg, &args);

				input.erase(t->line_offset, t->str.size());
				input.insert(t->line_offset, final_str);
				*cursor_pos = t->line_offset + final_str.size();
			}
			continue;

		}
		command_info* c = GetSuggestion(cur, t->str);

		if (c)
		{
			input.erase(t->line_offset, t->str.size());
			std::string final_str = c->names[0];

			input.insert(t->line_offset,  final_str);
			*cursor_pos = t->line_offset + final_str.size();
			last = cur;
			cur = c;
		}

	}
}

dbg_expr* WasmGetExprFromTkns(dbg_state* dbg, own_std::vector<token2> *tkns)
{
	func_decl* func = dbg->cur_func;
	node_iter niter = node_iter(tkns, dbg->lang_stat);
	node *n = niter.parse_all();

	type_struct2* strct_for_filter;
	//Scope
	scope* cur_scp = FindScpWithLine(func, dbg->cur_st->line);

	if (IS_COMMA(n))
	{
		own_std::vector<node *> commas_val;
		node* cur = n;
		while (IS_COMMA(cur))
		{
			ASSERT(!IS_COMMA(cur->r));
			commas_val.emplace_back(cur->r);
			cur = cur->l;
		}
		commas_val.emplace_back(cur);
		
		switch (commas_val.size())
		{
		case 2:
		case 1:
		{

			dbg->lang_stat->flags |= PSR_FLAGS_REPORT_UNDECLARED_IDENTS;
			DescendNameFinding(dbg->lang_stat, n, cur_scp);

			DescendNode(dbg->lang_stat, n, cur_scp);
		}break;
		// filter vals
		case 3:
		{
			
			dbg->lang_stat->flags |= PSR_FLAGS_REPORT_UNDECLARED_IDENTS;
			DescendNameFinding(dbg->lang_stat, commas_val[2], cur_scp);
			type2 strct = DescendNode(dbg->lang_stat, commas_val[2], cur_scp);

			ASSERT(strct.type == TYPE_STRUCT && strct.ptr == 1)

			DescendNameFinding(dbg->lang_stat, commas_val[1], cur_scp);
			DescendNode(dbg->lang_stat, commas_val[1], cur_scp);

			// creating a new scope to not polute the original one
			scope* scp = NewScope(dbg->lang_stat, cur_scp);
			cur_scp = scp;

			auto _ptr = (decl2*)AllocMiscData(dbg->lang_stat, sizeof(decl2));
			_ptr->name = std::string("_ptr");
			_ptr->flags = DECL_ABSOLUTE_ADDRESS;
			_ptr->offset = FILTER_PTR;
			_ptr->type = strct;
			cur_scp->AddDecl(_ptr);
			DescendNameFinding(dbg->lang_stat, commas_val[0], cur_scp);
			DescendNode(dbg->lang_stat, commas_val[0], cur_scp);
		}break;
		default:
			ASSERT(0);
		}
	}
	else
	{
		dbg->lang_stat->flags |= PSR_FLAGS_REPORT_UNDECLARED_IDENTS;
		DescendNameFinding(dbg->lang_stat, n, cur_scp);

		DescendNode(dbg->lang_stat, n, cur_scp);
	}



	ast_rep* ast = AstFromNode(dbg->lang_stat, n, cur_scp);

	n->FreeTree();
	/*
	int last_idx = dbg->lang_stat->allocated_vectors.size() - 1;
	if(last_idx >= 0)
	{
	dbg->lang_stat->allocated_vectors[last_idx]->~vector();
	dbg->lang_stat->allocated_vectors.pop_back();
	*/

	auto exp = (dbg_expr*)AllocMiscData(dbg->lang_stat, sizeof(dbg_expr));
	//exp->exp_str = exp_str.substr();
	exp->from_func = dbg->cur_func;

	//exp_str = "";
	if (ast->type == AST_BINOP && ast->op == T_COMMA)
	{
		ASSERT(ast->expr.size() >= 2);


		PushAstsInOrder(dbg->lang_stat, ast->expr[0], &exp->expr);
		PushAstsInOrder(dbg->lang_stat, ast->expr[1], &exp->x_times);

		if(ast->expr.size() == 2)
			exp->type = DBG_EXPR_SHOW_VAL_X_TIMES;
		else if (ast->expr.size() == 3)
		{
			exp->type = DBG_EXPR_FILTER;
			ast_rep* ret = NewAst();
			ret->type = AST_RET;
			ret->ast = ast->expr[2];
			FreeAllRegs(dbg->lang_stat);
			GetIRFromAst(dbg->lang_stat, ret, &exp->filter_cond);
		}
		else
			ASSERT(0)
		//exp_str = WasmGetSingleExprToStr(dbg, &exp);
	}
	else
	{
		exp->type = DBG_EXPR_SHOW_SINGLE_VAL;

		PushAstsInOrder(dbg->lang_stat, ast, &exp->expr);
		//exp_str = WasmGetSingleExprToStr(dbg, &exp);
	}

	return exp;
}
dbg_expr* WasmGetExprFromStr(dbg_state* dbg, std::string exp_str)
{
	func_decl* func = dbg->cur_func;

	own_std::vector<token2> tkns;
	Tokenize2((char *)exp_str.c_str(), exp_str.size(), &tkns);
	return WasmGetExprFromTkns(dbg, &tkns);
}

u64 DoAndOpInValBasedOnSize(u64 lhs_val, char size)
{
	switch (size)
	{
	case 1:
		lhs_val &= 0xff;
	break;
	case 2:
		lhs_val &= 0xffff;
	break;
	case 4:
		lhs_val &= 0xffffffff;
	break;
	case 8:
		break;
	default:
		ASSERT(0);
	}
	return lhs_val;
}
template <typename T>
T WasmIrInterpGetIrVal2(dbg_state* dbg, ir_val* val)
{
	T ret = 0;
	s64 offset = 0;
	char ptr = val->deref;
	int reg = val->reg;
	switch (val->type)
	{
	case IR_TYPE_STR_LIT:
	{
		return DATA_SECT_OFFSET + val->on_data_sect_offset;
	}break;
	case IR_TYPE_F32:
	{
		ret = val->f32;
	}break;
	case IR_TYPE_INT:
	{
		ret = val->i;
	}break;
	case IR_TYPE_RET_REG:
	{
		if (val->is_float && ptr < 0)
		{
			*(int*)&ret = WasmGetRegVal(dbg, RET_1_REG);
			return ret;
		}
		else
			ret = WasmGetRegVal(dbg, RET_1_REG );
	}break;
	case IR_TYPE_ON_STACK:
	{
		offset = GetOnStackTypeOffset(dbg->cur_func, val);
		offset = WasmGetMemOffsetVal(dbg, BASE_STACK_PTR_REG * 8) + offset;
		if (val->deref < 0)
		{
			return DoAndOpInValBasedOnSize(offset, val->reg_sz);
		}

	}break;
	case IR_TYPE_REG:
	{
		if(val->is_float && ptr < 0)
			reg = reg + FLOAT_REG_0;
		ret = WasmGetRegVal(dbg, reg);
		if (val->is_float && ptr < 0)
		{
			*(int *)&ret = WasmGetRegVal(dbg, reg);
			return ret;
		}
	}break;
	case IR_TYPE_DECL:
	{
		u64 base_reg = WasmGetMemOffsetVal(dbg, BASE_STACK_PTR_REG * 8);
		if (IS_FLAG_ON(val->decl->flags, DECL_ABSOLUTE_ADDRESS))
		{
			offset = WasmGetRegVal(dbg, val->decl->offset);
			//ptr--;
		}
		else if (IS_FLAG_ON(val->decl->flags, DECL_IS_GLOBAL))
		{
			offset = val->decl->offset + GLOBALS_OFFSET;
			break;
		}
		else if (val->decl->type.is_const)
		{
			offset = val->decl->offset;
			//ptr--;
		}
		else if (val->decl->type.type == TYPE_FUNC)
		{
			return val->decl->type.fdecl->func_idx;
		}
		else
			offset = val->decl->offset;
		offset = base_reg + offset;
		if (ptr <= -1)
			return offset;
	}break;
	default:
		ASSERT(0);
	}
	if (val->type == IR_TYPE_REG)
	{
		if(val->is_float)
			offset = WasmGetRegVal(dbg, reg);
		else
			offset = ret;
	}

	while (ptr >= 0 && val->type != IR_TYPE_INT && val->type != IR_TYPE_F32)
	{
		if (ptr == 0)
		{
			if (val->is_float)
			{
				*(int *)&ret = WasmGetMemOffsetVal(dbg, offset);
			}
			else
				ret = (T)WasmGetMemOffsetVal(dbg, offset);


		}
		else
		{
			offset = WasmGetMemOffsetVal(dbg, offset);
		}
		ptr--;
	}
	return ret;
}
template <typename T>
T WasmIrInterpGetIrVal(dbg_state* dbg, ir_val* val)
{
	T ret = 0;
	char ptr = val->deref;
	switch (val->type)
	{
	case IR_TYPE_F32:
	{
		ret = val->f32;
	}break;
	case IR_TYPE_RET_REG:
	{
		ret = val->reg * RET_1_REG;
	}break;
	case IR_TYPE_INT:
	{
		ret = val->i;
	}break;
	case IR_TYPE_REG:
	{
		ret = WasmGetRegVal(dbg, val->reg);
	}break;
	case IR_TYPE_DECL:
	{

		if (IS_FLAG_ON(val->decl->flags, DECL_ABSOLUTE_ADDRESS))
		{
			ret = WasmGetRegVal(dbg, val->decl->offset);
			ptr--;
		}
		else if (IS_FLAG_ON(val->decl->flags, DECL_IS_GLOBAL))
		{
			ret = val->decl->offset + GLOBALS_OFFSET;
		}
		else if (val->decl->type.is_const)
		{
			ret = val->decl->offset;
			ptr--;
		}
		else
			ret = val->decl->offset;
	}break;
	default:
		ASSERT(0);
	}
	//ptr--;
	if (val->type == IR_TYPE_REG)
		ptr--;
	while (ptr > 0 && val->type != IR_TYPE_INT && val->type != IR_TYPE_F32)
	{
		ret = WasmGetMemOffsetVal(dbg, ret);
		ptr--;
	}
	return ret;
}

template<typename T>
T WasmIrInterpSingle(dbg_state* dbg, ir_val* lhs)
{
	u64 lhs_val = WasmIrInterpGetIrVal<u64>(dbg, lhs);
	switch (lhs->reg_sz)
	{
	case 1:
		lhs_val &= 0xff;
	break;
	case 2:
		lhs_val &= 0xffff;
	break;
	case 4:
		lhs_val &= 0xffffffff;
	break;
	case 8:
		break;
	default:
		ASSERT(0);
	}
	return (T)lhs;

}

u64 WasmIrInterpBin(dbg_state* dbg, ir_val* lhs, ir_val* rhs, tkn_type2 op)
{
	u64 ret = 0;
	if(lhs->is_float)
	{
		float lhs_val = WasmIrInterpGetIrVal2<float>(dbg, lhs);
		float rhs_val = WasmIrInterpGetIrVal2<float>(dbg, rhs);

		if (op >= T_GREATER_THAN && op <= T_COND_EQ)
		{
			switch (op)
			{
			case T_LESSER_THAN:
				return lhs_val < rhs_val;
			case T_LESSER_EQ:
				return lhs_val <= rhs_val;
			case T_GREATER_THAN:
				return lhs_val > rhs_val;
			case T_GREATER_EQ:
				return lhs_val >= rhs_val;
			case T_COND_EQ:
				return lhs_val == rhs_val;
			case T_COND_NE:
				return lhs_val != rhs_val;
			default:
				ASSERT(0);
			}
		}
		ret = GetExpressionValT<u64>(op, lhs_val, rhs_val);
	}
	else
	{
		if (lhs->is_unsigned)
		{
			u64 lhs_val = WasmIrInterpGetIrVal2<u64>(dbg, lhs);
			u64 rhs_val = WasmIrInterpGetIrVal2<u64>(dbg, rhs);
			ret = GetExpressionValT<u64>(op, lhs_val, rhs_val);
		}
		else
		{
			s64 lhs_val = WasmIrInterpGetIrVal2<s64>(dbg, lhs);
			s64 rhs_val = WasmIrInterpGetIrVal2<s64>(dbg, rhs);
			ret = GetExpressionValT<s64>(op, lhs_val, rhs_val);
		}
	}

	return ret;
}
template<typename T>
void WasmIrInterpAssign2(dbg_state* dbg, ir_rep *ir)
{
	T final_val = 0;
	if (ir->assign.only_lhs)
	{
		final_val = WasmIrInterpGetIrVal2<T>(dbg, &ir->assign.lhs);
	}
	else
	{
		T lhs_val = WasmIrInterpGetIrVal2<T>(dbg, &ir->assign.lhs);
		if (ir->assign.op == T_POINT)
			final_val = lhs_val + ir->assign.rhs.i;

		else
		{
			T rhs_val = WasmIrInterpGetIrVal2<T>(dbg, &ir->assign.rhs);
			final_val = GetExpressionValT<T>(ir->assign.op, lhs_val, rhs_val);
		}
	}

	switch (ir->assign.to_assign.type)
	{
	case IR_TYPE_ARG_REG:
	case IR_TYPE_DECL:
	case IR_TYPE_ON_STACK:
	case IR_TYPE_REG:
	{
		int offset = 0;
		int sz = 0;
		char ptr = ir->assign.to_assign.deref;
		if (ir->assign.to_assign.type == IR_TYPE_REG)
		{
			int reg = ir->assign.to_assign.reg;
			if (ir->assign.to_assign.is_float && ptr < 0)
				reg = reg + FLOAT_REG_0;
			offset = reg * 8;
			
			sz = ir->assign.to_assign.reg_sz;
		}
		else if (ir->assign.to_assign.type == IR_TYPE_ON_STACK)
		{
			offset = GetOnStackTypeOffset(dbg->cur_func, &ir->assign.to_assign);
			offset = WasmGetMemOffsetVal(dbg, BASE_STACK_PTR_REG * 8) + offset;
			sz = ir->assign.to_assign.reg_sz;
		}
		else if (ir->assign.to_assign.type == IR_TYPE_ARG_REG)
		{
			offset = ir->assign.to_assign.reg * 8;
			offset += WasmGetMemOffsetVal(dbg, STACK_PTR_REG * 8);

			sz = 8;
		}
		else if (ir->assign.to_assign.type == IR_TYPE_DECL)
		{
			decl2* d = ir->assign.to_assign.decl;
			offset = ir->assign.to_assign.decl->offset;
			if (IS_FLAG_ON(d->flags, DECL_IS_GLOBAL))
				offset += GLOBALS_OFFSET;
			else
			{
				offset += WasmGetMemOffsetVal(dbg, BASE_STACK_PTR_REG * 8);
			}

			sz = GetTypeSize(&d->type);
			/*
			if (d->type.is_const)
			{
				//d->type.i = final_val;
				break;
			}
			*/
		}

		while (ptr >= 0)
		{
			offset = WasmGetMemOffsetVal(dbg, offset);
			ptr--;
		}

		if (ir->assign.to_assign.is_float)
			sz = 4;
		switch (sz)
		{
		case 1:
		{
			char* reg = (char*)&dbg->mem_buffer[offset];
			*reg = final_val;
		}break;
		case 2:
		{
			short* reg = (short*)&dbg->mem_buffer[offset];
			*reg = final_val;
		}break;
		case 4:
		{
			T* reg = (T*)&dbg->mem_buffer[offset];
			*reg = final_val;
		}break;
		case 8:
		{
			long long* reg = (long long*)&dbg->mem_buffer[offset];
			*reg = final_val;
		}break;
		default:
			ASSERT(0);
		}
	}break;
	case IR_TYPE_RET_REG:
	{
		int* reg = (int*)&dbg->mem_buffer[(RET_1_REG + ir->assign.to_assign.reg) * 8];
		if (ir->assign.to_assign.is_float)
		{
			*(float *)reg = final_val;
		}
		else
			*reg = final_val;
	}break;
	default:
		ASSERT(0);
	}

}
template<typename T>
void WasmIrInterpAssign(dbg_state* dbg, ir_rep *ir)
{
	T final_val = 0;
	if (ir->assign.only_lhs)
	{
		final_val = WasmIrInterpGetIrVal<T>(dbg, &ir->assign.lhs);
	}
	else
	{
		T lhs_val = WasmIrInterpGetIrVal<T>(dbg, &ir->assign.lhs);
		if (ir->assign.op == T_POINT)
			final_val = lhs_val + ir->assign.rhs.i;

		else
		{
			T rhs_val = WasmIrInterpGetIrVal<T>(dbg, &ir->assign.rhs);
			final_val = GetExpressionValT<T>(ir->assign.op, lhs_val, rhs_val);
		}
	}

	switch (ir->assign.to_assign.type)
	{
	case IR_TYPE_DECL:
	case IR_TYPE_REG:
	{
		int offset = 0;
		int sz = 0;
		char ptr = ir->assign.to_assign.ptr;
		if (ir->assign.to_assign.type == IR_TYPE_REG)
		{
			offset = ir->assign.to_assign.reg * 8;
			sz = ir->assign.to_assign.reg_sz;
		}
		else if (ir->assign.to_assign.type == IR_TYPE_DECL)
		{
			decl2* d = ir->assign.to_assign.decl;
			offset = d->offset;
			if (IS_FLAG_ON(d->flags, DECL_IS_GLOBAL))
				offset += GLOBALS_OFFSET;
			else
			{
				offset += WasmGetMemOffsetVal(dbg, BASE_STACK_PTR_REG * 8);
			}

			sz = GetTypeSize(&d->type);
			if (d->type.is_const)
			{
				d->type.i = final_val;
				break;
			}
			ptr--;
		}

		while (ptr > 0)
		{
			offset = WasmGetMemOffsetVal(dbg, offset);
			ptr--;
		}

		if (ir->assign.to_assign.is_float)
			sz = 4;
		switch (sz)
		{
		case 1:
		{
			char* reg = (char*)&dbg->mem_buffer[offset];
			*reg = final_val;
		}break;
		case 2:
		{
			short* reg = (short*)&dbg->mem_buffer[offset];
			*reg = final_val;
		}break;
		case 4:
		{
			T* reg = (T*)&dbg->mem_buffer[offset];
			*reg = final_val;
		}break;
		case 8:
		{
			long long* reg = (long long*)&dbg->mem_buffer[offset];
			*reg = final_val;
		}break;
		default:
			ASSERT(0);
		}
	}break;
	case IR_TYPE_RET_REG:
	{
		int* reg = (int*)dbg->mem_buffer[(RET_1_REG + ir->assign.to_assign.reg) * 8];
		*reg = final_val;
	}break;
	default:
		ASSERT(0);
	}

}

#pragma optimize("", off)
void WasmCallX64(wasm_interp* winterp, dbg_state& dbg, unsigned char* mem_buffer, func_decl *call_f, int base_ptr)
{
	int tsize = winterp->dbg->lang_stat->type_sect.size();
	unsigned char *cdata = winterp->dbg->lang_stat->code_sect.data();
	cdata += tsize;
	unsigned char* func_code = cdata + call_f->for_interpreter_code_start_idx;
	//int base_ptr = *(int*)&dbg.mem_buffer[STACK_PTR_REG * 8];
	void *a_ptr = (void*)&dbg.mem_buffer[base_ptr + 8];

	void* addr = nullptr;
	if(call_f->ret_type.IsFloat() && call_f->ret_type.ptr == 0)
		*(float *)&addr = (((float(*)(void *, void*))func_code)(dbg.mem_buffer, a_ptr));
	else
		addr = (((void *(*)(void *, void*))func_code)(dbg.mem_buffer, a_ptr));
	if (!((call_f->ret_type.type == TYPE_VOID || call_f->ret_type.type == TYPE_AUTO) && call_f->ret_type.ptr <= 0))
	{
		call_f->ret_type.ptr--;
		if (call_f->ret_type.ptr < 0)
		{
			void* final_val = addr;
			if (call_f->ret_type.type == TYPE_STRUCT)
			{
				memcpy(&dbg.mem_buffer[2000], addr, GetTypeSize(&call_f->ret_type));
				final_val = (void *)(long long)2000;
			}
			*(long long *)& dbg.mem_buffer[RET_1_REG * 8] = (long long) final_val;

		}
		else
		{
			if(addr > dbg.mem_buffer)
				*(long long *)& dbg.mem_buffer[RET_1_REG * 8] = (long long) ((unsigned char *)addr - (unsigned char *)dbg.mem_buffer);
			else
				*(long long *)& dbg.mem_buffer[RET_1_REG * 8] = (long long) (addr);
		}
		call_f->ret_type.ptr++;
	}


	auto a = 0;
}
#pragma optimize("", on)
void WasmDoCallInstructionIr(dbg_state *dbg, ir_rep **bc, block_linked **cur, func_decl *call_f)
{
	dbg->cur_func = call_f;
	dbg->func_stack.emplace_back(call_f);
	//dbg->block_stack.emplace_back(*cur);
	dbg->return_stack_ir.emplace_back(*bc);
	int idx = call_f->wasm_code_sect_idx - 1;
	//if (idx < 0)
		//idx = 0;
	//*bc = dbg->bcs.begin() + idx;
	auto ir = (own_std::vector<ir_rep> *)(&call_f->ir);
	*bc = (ir->begin() + call_f->ir_stack_begin_idx);
	(*bc)--;

}

void IrEndStack(dbg_state* dbg, ir_rep** ptr, ir_rep* start)
{
	auto base = (u64 *)&dbg->mem_buffer[BASE_STACK_PTR_REG * 8];
	auto stack = (u64 *)&dbg->mem_buffer[STACK_PTR_REG * 8];
	*stack += dbg->cur_func->stack_size + 8;
	*base = *(u64 *)&dbg->mem_buffer[*stack];
	*stack += 8;

	dbg->func_stack.pop_back();
	if (dbg->func_stack.size() == 0)
	{
		return;
	}
	func_decl* prev_func = dbg->cur_func;
	dbg->cur_func = dbg->func_stack.back();

	*ptr = dbg->return_stack_ir.back();
	dbg->return_stack_ir.pop_back();
}
bool IrLogic2(dbg_state* dbg, ir_rep** ptr, ir_rep *start)
{
	ir_rep* ir = *ptr;
	ir_rep assign;
	switch (ir->type)
	{
	case IR_STACK_END:
	{
		IrEndStack(dbg, ptr, start);
	}break;
	case IR_STACK_BEGIN:
	{
		auto base_reg = (u64 *)&dbg->mem_buffer[BASE_STACK_PTR_REG * 8];
		auto stack_reg = (u64 *)&dbg->mem_buffer[STACK_PTR_REG * 8];
		*stack_reg -= 8;
		// saving prev base ptr
		*(u64 *)&dbg->mem_buffer[*stack_reg] = *base_reg;
		*base_reg = *stack_reg;

		*base_reg = *stack_reg;
		*stack_reg -= ir->fdecl->stack_size + 8;
	}break;
	case IR_BREAK_OUT_IF_BLOCK:
	{
		*ptr = start + ir->block.other_idx - 1;
	}break;
	case IR_BREAK:
	{
		*ptr = start + ir->block.other_idx;
	}break;
	case IR_DBG_BREAK:
	{
		return false;
	}break;
	case IR_CONTINUE:
	{
		*ptr = start + ir->block.other_idx;
	}break;
	case IR_END_LOOP_BLOCK:
	{
		*ptr = start + ir->block.other_idx;
	}break;
	case IR_END_IF_BLOCK:
	case IR_BEGIN_IF_BLOCK:
	case IR_END_SUB_IF_BLOCK:
	case IR_BEGIN_SUB_IF_BLOCK:
	case IR_END_BLOCK:
	case IR_BEGIN_COND_BLOCK:
	case IR_BEGIN_LOOP_BLOCK:
	case IR_END_COND_BLOCK:
	case IR_BEGIN_BLOCK:
	case IR_DECLARE_ARG:
	case IR_DECLARE_LOCAL:
	{
	}break;
	case IR_CMP_LE:
	case IR_CMP_LT:
	case IR_CMP_GT:
	case IR_CMP_NE:
	case IR_CMP_GE:
	case IR_CMP_EQ:
	{
		u64 val = WasmIrInterpBin(dbg, &ir->bin.lhs, &ir->bin.rhs, ir->bin.op);
		if (val == 1)
		{
			*ptr += ir->dst_ir_rel_idx - 1;
		}
	}break;
	case IR_END_STMNT:
	case IR_BEGIN_STMNT:
	{

	}break;
	case IR_CAST_F32_TO_INT:
	{
		assign.assign.only_lhs = true;
		assign.assign.lhs.type = IR_TYPE_INT;
		assign.assign.lhs.i = WasmIrInterpGetIrVal2<float>(dbg, &ir->bin.rhs);
		assign.assign.to_assign = ir->bin.lhs;
		WasmIrInterpAssign2<int>(dbg, &assign);
	}break;
	case IR_CAST_INT_TO_INT:
	{

		assign.assign.only_lhs = true;
		assign.assign.lhs.type = IR_TYPE_INT;
		if (ir->bin.lhs.is_unsigned)
		{
			u64 val = WasmIrInterpGetIrVal2<u64>(dbg, &ir->bin.rhs);
			val = DoAndOpInValBasedOnSize(val, ir->bin.lhs.reg_sz);
			assign.assign.lhs.i = val;
		}
		else
		{
			s64 val = WasmIrInterpGetIrVal2<s64>(dbg, &ir->bin.rhs);
			val = DoAndOpInValBasedOnSize(val, ir->bin.lhs.reg_sz);
			assign.assign.lhs.i = val;
		}
		assign.assign.lhs.is_float = false;
		assign.assign.to_assign = ir->bin.lhs;
		WasmIrInterpAssign2<int>(dbg, &assign);
	}break;
	case IR_CAST_INT_TO_F32:
	{

		assign.assign.only_lhs = true;
		assign.assign.lhs.type = IR_TYPE_F32;
		if (ir->bin.lhs.is_unsigned)
		{
			assign.assign.lhs.f32 = WasmIrInterpGetIrVal2<u32>(dbg, &ir->bin.rhs);
		}
		else
		{
			assign.assign.lhs.f32 = WasmIrInterpGetIrVal2<int>(dbg, &ir->bin.rhs);
		}
		assign.assign.lhs.is_float = true;
		assign.assign.to_assign = ir->bin.lhs;
		WasmIrInterpAssign2<float>(dbg, &assign);
	}break;
	case IR_RET:
	{
		if (ir->ret.no_ret_val)
		{
		}
		else
		{
			int final_val = WasmIrInterpGetIrVal2<int>(dbg, &ir->ret.assign.lhs);
			int* reg = (int*)&dbg->mem_buffer[(RET_1_REG + ir->ret.assign.to_assign.reg) * 8];
			*reg = final_val;

		}
		IrEndStack(dbg, ptr, start);
	}break;
	case IR_INDIRECT_CALL:
	{
		ir_val val;
		val.type = IR_TYPE_DECL;
		val.decl = ir->decl;
		val.deref = 0;
		int idx = WasmIrInterpGetIrVal2<int>(dbg, &val);
		func_decl* call_f = WasmGetFuncAtIdx(dbg->wasm_state, idx);
		WasmDoCallInstructionIr(dbg, ptr, nullptr, call_f);
	}break;
	case IR_CALL:
	{
		func_decl* call_f = ir->call.fdecl;
		if (IS_FLAG_ON(call_f->flags, FUNC_DECL_IS_OUTSIDER))
		{
			if (dbg->wasm_state->outsiders.find(call_f->name) != dbg->wasm_state->outsiders.end())
			{
				auto stack_reg = (u64 *)&dbg->mem_buffer[STACK_PTR_REG * 8];
				*stack_reg -= 8;
				OutsiderFuncType func_ptr = dbg->wasm_state->outsiders[call_f->name];
				func_ptr(dbg);
				*stack_reg += 8;
			}
			else
				ASSERT(0);

		}
		else if (IS_FLAG_ON(call_f->flags, FUNC_DECL_X64))
		{
			
			auto stack_reg = (u64 *)&dbg->mem_buffer[PRE_X64_RSP_REG * 8];
			*stack_reg -= 8;
			//WasmCallX64(dbg->wasm_state, *dbg, (u8 *)dbg->mem_buffer, call_f, *stack_ptr);
			*stack_reg += 8;
		}
		else
		{
			WasmDoCallInstructionIr(dbg, ptr, nullptr, call_f);
		}
	}break;
	case IR_ASSIGNMENT:
	{
		if (ir->assign.to_assign.is_float || ir->assign.to_assign.type == IR_TYPE_DECL && ir->assign.to_assign.decl->type.type == TYPE_F32)
			WasmIrInterpAssign2<float>(dbg, ir);
		else
			WasmIrInterpAssign2<int>(dbg,ir);
	}break;

	default:
		stmnt_dbg* st = GetStmntBasedOnOffsetIr(&dbg->cur_func->wasm_stmnts, ir->idx);
		ASSERT(0);
	}
	return true;

}
bool IrLogic(dbg_state* dbg, ir_rep** ptr)
{
	ir_rep* ir = *ptr;
	switch (ir->type)
	{
	case IR_STACK_BEGIN:
	{
		auto base_reg = (u64 *)&dbg->mem_buffer[BASE_STACK_PTR_REG * 8];
		auto stack_reg = (u64 *)&dbg->mem_buffer[STACK_PTR_REG * 8];
		stack_reg += 8;
		*stack_reg = *base_reg;
		*base_reg = *stack_reg;
		stack_reg -= ir->fdecl->stack_size;
	}break;
	case IR_DBG_BREAK:
	{
		return false;
	}break;
	case IR_BEGIN_IF_BLOCK:
	case IR_BEGIN_COND_BLOCK:
	case IR_END_COND_BLOCK:
	case IR_DECLARE_ARG:
	case IR_DECLARE_LOCAL:
	{
	}break;
	case IR_CMP_NE:
	case IR_CMP_EQ:
	{
		u64 val = WasmIrInterpBin(dbg, &ir->bin.lhs, &ir->bin.rhs, ir->bin.op);
		if (val == 1)
		{
			*ptr += ir->dst_ir_rel_idx - 1;
		}
	}break;
	case IR_END_STMNT:
	case IR_BEGIN_STMNT:
	{

	}break;
	case IR_RET:
	{
		if (ir->ret.no_ret_val)
		{
			ASSERT(0);
		}
		else
		{
			int final_val = WasmIrInterpGetIrVal<int>(dbg, &ir->ret.assign.lhs);
			int* reg = (int*)&dbg->mem_buffer[(RET_1_REG + ir->ret.assign.to_assign.reg) * 8];
			*reg = final_val;

		}
	}break;
	case IR_ASSIGNMENT:
	{
		if (ir->assign.to_assign.is_float || ir->assign.to_assign.type == IR_TYPE_DECL && ir->assign.to_assign.decl->type.type == TYPE_F32)
			WasmIrInterpAssign<float>(dbg, ir);
		else
			WasmIrInterpAssign<int>(dbg,ir);
	}break;
	default:
		ASSERT(0);
	}
	return true;

}

enum dbg_code_type
{
	DBG_CODE_WASM,
};
struct dbg_file_seriealize
{
	int func_sect;
	int total_funcs;
	int stmnts_sect;
	int string_sect;
	int scopes_sect;
	int types_sect;
	int vars_sect;
	int code_sect;
	int ir_sect;
	int files_sect;

	int bc2_sect;
	int bc2_sect_size;

	int globals_sect;
	int globals_sect_size;

	int data_sect;
	int data_sect_size;

	int x64_code_sect;
	int x64_code_sect_size;
	int x64_code_type_sect_size;

	int total_files;


	dbg_code_type code_type;
};
void WasmIrInterp(dbg_state* dbg, own_std::vector<int>* ar)
{
	auto ir_ar = (own_std::vector<ir_rep>*)ar;
	ir_rep* ir = ir_ar->begin();
	while (ir < ir_ar->end())
	{
		IrLogic(dbg, &ir);
		ir++;
	}
}
#ifndef LANG_NO_ENGINE

bool IsKeyDown(void* data, key_enum key);
void ImGuiPrintScopeVars(char* name, dbg_state& dbg, scope* cur_scp, int base_ptr);
void BeginLocalsChild(dbg_state &dbg, int base_ptr, scope *cur_scp)
{
	ImGui::BeginChild("locals", ImVec2(500, 400));
	if (cur_scp)
	{

		scope* aux_scp = cur_scp;
		ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow;
		if (ImGui::TreeNodeEx("root", flag))
		{
			while (aux_scp && IS_FLAG_OFF(aux_scp->flags, SCOPE_IS_GLOBAL))
			{
				ImGuiPrintScopeVars("root", dbg, aux_scp, base_ptr);
				aux_scp = aux_scp->parent;
			}
			ImGui::TreePop();  // This is required at the end of the if block
		}
	}
	ImGui::EndChild();
}
bool IsExecutableIr(ir_rep *ir)
{
	bool is_cmp = (int)ir->type >= (int)IR_CMP_EQ && (int)ir->type <= (int)IR_CMP_GT;
	return ir->type == IR_ASSIGNMENT ||
		ir->type == IR_BREAK ||
		ir->type == IR_RET ||
		ir->type == IR_CONTINUE ||
		ir->type == IR_CAST_F32_TO_INT ||
		ir->type == IR_CAST_INT_TO_F32 ||
		ir->type == IR_INDIRECT_CALL ||
		ir->type == IR_CAST_INT_TO_INT ||
		ir->type == IR_CALL ||
		ir->type == IR_DBG_BREAK ||
		is_cmp;
}
void AdvanceIrToNext(ir_rep** ptr)
{
	while (!IsExecutableIr(*ptr))
	{
		(*ptr)++;
	}
}
std::string GetMemAddrString(dbg_state &dbg, int mem_wnd_offset, int type_sz, int limit, int mem_wnd_show_type)
{
	std::string ret = "";
	for (int i = 0; i < limit; i += type_sz)
	{
		long long val = 0;
		print_num_type ptype = PRINT_INT;
		switch (mem_wnd_show_type)
		{
			// u8
		case 0:
		{
			*(unsigned char*)&val = *(unsigned char*)&dbg.mem_buffer[mem_wnd_offset + i];
		}break;
		// s8
		case 1:
		{
			*(char*)&val = *(char*)&dbg.mem_buffer[mem_wnd_offset + i];
		}break;
		// u16
		case 2:
		{
			*(unsigned short*)&val = *(unsigned short*)&dbg.mem_buffer[mem_wnd_offset + i];
		}break;
		// s16
		case 3:
		{
			*(short*)&val = *(short*)&dbg.mem_buffer[mem_wnd_offset + i];
		}break;
		// u32
		case 4:
		{
			*(unsigned int*)&val = *(unsigned int*)&dbg.mem_buffer[mem_wnd_offset + i];
		}break;
		// s32
		case 5:
		{
			*(int*)&val = *(int*)&dbg.mem_buffer[mem_wnd_offset + i];
		}break;
		case 6:
		{
			*(long long*)&val = *(long long*)&dbg.mem_buffer[mem_wnd_offset + i];
		}break;
		case 7:
		{
			*(unsigned long long*)& val = *(unsigned long long*) & dbg.mem_buffer[mem_wnd_offset + i];
		}break;
		case 8:
		{
			*(float *)& val = *(float *) &dbg.mem_buffer[mem_wnd_offset + i];
			ptype = PRINT_FLOAT;
		}break;
		case 9:
		{
			*(char *)& val = *(char *) &dbg.mem_buffer[mem_wnd_offset + i];
			ret += val;
			continue;
		}break;
		default:
			ASSERT(0);
		}
		ret += WasmNumToString(&dbg, val, -1, ptype) + " ";
	}
	return ret;
}
void UpdateExprWindow(dbg_state& dbg, int stack_reg, int line)
{
	char saved_regs[258];
	memcpy(saved_regs, dbg.mem_buffer, 258);
	*(u64 *)&dbg.mem_buffer[BASE_STACK_PTR_REG * 8] = *(u64 *)&dbg.mem_buffer[stack_reg * 8];

	FOR_VEC(e_ptr, dbg.exprs2)
	{
		dbg_expr2* e = *e_ptr;
		
		ir_rep *end = e->irs.end();
		ir_rep *start = e->irs.begin();
		ir_rep* ptr = e->irs.begin();
		while (ptr < end)
		{
			IrLogic2(&dbg, &ptr, start);
			ptr++;
		}
		ptr--;

		char reg = ptr->assign.to_assign.reg;

		// all dbgs vals at end are put in __global_dummy var, which it is at this offset
		auto val = (u64 *)&dbg.mem_buffer[GLOBALS_OFFSET - 8];
		e->offset = *val;
		e->val = std::to_string(*val);

	}
	memcpy(dbg.mem_buffer, saved_regs, 258);
}
u64* GetRegValPtr(dbg_state* dbg, short reg);
void MaybeAddNewDbgExpr(dbg_state* dbg, std::string &str, int stack_reg, int line)
{
	dbg->lang_stat->flags |= PSR_FLAGS_ON_JMP_WHEN_ERROR;
	//own_std::vector<token2> tkns;
	//Tokenize2((char *)str.c_str(), str.size(), &tkns);
	scope* scp = FindScpWithLine(dbg->cur_func, line);

	if (!scp)
		return;

	void* prev_alloc = __lang_globals.data;
	__lang_globals.data = (void *)dbg->dbg_alloc;

	ast_rep* ast;
	node* len = nullptr;
	node* n;
	{
		own_std::vector<token2> tkns;
		Tokenize2((char*)str.c_str(), str.size(), &tkns);
		node_iter niter(&tkns, dbg->lang_stat);
		dbg->lang_stat->use_node_arena = true;
		n = niter.parse_all();

		int dummy_prec = 0;
		if (CMP_NTYPE_BIN(n, T_COMMA))
		{
			len = n->r;
			n = n->l;
		}

		ast = AstFromNode(dbg->lang_stat, n, scp);
	}
	__lang_globals.data = prev_alloc;


	dbg_expr2* exp;
	type2 tp;
	ir_rep ir = {};
	ir.type = IR_ASSIGNMENT;
	ir.assign.to_assign.type = IR_TYPE_DECL;
	ir.assign.to_assign.decl = dbg->lang_stat->dbg_equal->expr[0]->decl;
	ir.assign.to_assign.deref = -1;
	ir.assign.only_lhs = true;
	ast_rep* dbg_equal = dbg->lang_stat->dbg_equal;
	if (ast->type == AST_IDENT)
	{
		exp = (dbg_expr2 *)AllocMiscData(dbg->lang_stat, sizeof(dbg_expr2));;
		if (!ast->decl)
		{
			return;
		}
		exp->d.type = DescendNode(dbg->lang_stat, n, scp);
		dbg_equal->lhs_tp = exp->d.type;
		ir.assign.lhs.type = IR_TYPE_DECL;
		ir.assign.lhs.decl = ast->decl;
		ir.assign.lhs.deref = 0;
		if (ast->decl->type.type == TYPE_STRUCT)
		{
			if(exp->d.type.ptr == 0)
				ir.assign.lhs.deref = -1;
			exp->irs.emplace_back(ir);
		}
		else
		{
			dbg_equal->expr[1] = ast;
			GetIRFromAst(dbg->lang_stat, dbg_equal, &exp->irs);
		}
		exp->offset = *GetRegValPtr(dbg, stack_reg) + ast->decl->offset;
	}
	else
	{
		exp = (dbg_expr2 *)AllocMiscData(dbg->lang_stat, sizeof(dbg_expr2));;

		exp->d.type = DescendNode(dbg->lang_stat, n, scp);
		dbg_equal->lhs_tp = exp->d.type;

		
		dbg_equal->expr[1] = ast;
		GetIRFromAst(dbg->lang_stat, dbg_equal, &exp->irs);
	}
	if (len)
	{
		type2 tp = DescendNode(dbg->lang_stat, len, scp);
		ASSERT(tp.type == TYPE_INT && exp->d.type.ptr > 0);
		exp->d.flags = DECL_PTR_HAS_LEN;
		exp->d.len_for_ptr_offset = 8;
		exp->multiple_vals = tp.i;

	}
	exp->d.name = str.substr();
	dbg->exprs2.emplace_back(exp);

	UpdateExprWindow(*dbg, stack_reg, line);
}


void ImGuiPrintVar(char* buffer_in, dbg_state& dbg, decl2* d, int base_ptr, char ptr_decl);
u64* GetMemValPtr(dbg_state* dbg, short reg, int offset);
void ShowExprWindow(dbg_state& dbg, int stack_reg, int line)
{
	char buffer[128];
	ImGui::BeginChild("expr", ImVec2(500.0, 400.0));

	FOR_VEC(e_ptr, dbg.exprs2)
	{
		dbg_expr2* e = *e_ptr;
		auto reg_val = (u64 *)&dbg.mem_buffer[e->offset - 16];
		u64 saved_val = *(reg_val);
		u64 saved_val2 = *(reg_val + 1);

		if (IS_FLAG_ON(e->d.flags, DECL_PTR_HAS_LEN))
		{
			*(reg_val + 1) = e->multiple_vals;
		}
		if (e->d.type.ptr > 0)
		{
			*(reg_val) = e->offset;
		}


		if (e->d.type.type != TYPE_STRUCT)
		{
			ImGui::Text("%s: %d", e->d.name.c_str(), e->offset);
		}
		else
			ImGuiPrintVar(buffer, dbg, &e->d, e->offset - 16, e->d.type.ptr);

		*(reg_val) = saved_val;
		*(reg_val + 1) = saved_val2;
	}

	ImGui::InputText("##addexpr", buffer, 128);
	if(ImGui::IsItemDeactivatedAfterEdit())
	{

		InitMemAlloc(dbg.dbg_alloc);

		std::string str = buffer;
		int val = setjmp(dbg.lang_stat->jump_buffer);
		int prev_nd = dbg.lang_stat->cur_nd;
		void* prev_alloc = __lang_globals.data;
		if (val == 0)
		{

			MaybeAddNewDbgExpr(&dbg, str, stack_reg, line);
			dbg_expr2 exp;
			exp.str = buffer;
		}
		// error
		else if (val == 1)
		{
			
		}
		dbg.lang_stat->cur_nd = prev_nd;
		__lang_globals.data = prev_alloc;
	}
	ImGui::EndChild();
}
void SHowMemWindow(dbg_state &dbg, char *mem_wnd_items[], int &mem_wnd_show_type, int &mem_wnd_offset, int total_items, int stack_reg, func_decl *fdecl)
{
	ImGui::BeginChild("mem", ImVec2(500, 400));

	if (ImGui::BeginCombo("show type", mem_wnd_items[mem_wnd_show_type]))
	{
		for (int i = 0; i < total_items; i++)
			if (ImGui::Selectable(mem_wnd_items[i]))
				mem_wnd_show_type = i;
		ImGui::EndCombo();
	}
	//ImGui::ListBox("type: ", &mem_wnd_show_type, mem_wnd_items, IM_ARRAYSIZE(mem_wnd_items));
	ImGui::InputInt("address: ", &mem_wnd_offset);
	char type_sz = BuiltinTypeSize((enum_type2)(mem_wnd_show_type + TYPE_U8));
	std::string mem_val;
	for (int r = 0; r <= (BASE_STACK_PTR_REG + 7); r++)
	{
		mem_val = GetMemAddrString(dbg, mem_wnd_offset + r * 8, type_sz, 8, mem_wnd_show_type);

		std::string addr_name = WasmNumToString(&dbg, mem_wnd_offset + r * 8);

		int cur_addr = mem_wnd_offset + r;
		if (cur_addr == (BASE_STACK_PTR_REG))
			addr_name += "(base_stack)";
		else if (cur_addr == (stack_reg))
			addr_name += "(top_stack)";

		ImGui::Text("%s: %s", addr_name.c_str(), mem_val.c_str());
	}
	int base_ptr = WasmGetMemOffsetVal(&dbg, stack_reg * 8);
	for (int r = 0; r < 6; r++)
	{
		int addr = base_ptr + r * 8;
		mem_val = GetMemAddrString(dbg, addr, type_sz, 8, mem_wnd_show_type);

		std::string addr_name = WasmNumToString(&dbg, mem_wnd_offset + r * 8);
		ImGui::Text("arg_reg[%d, &%d]: %s", r, addr, mem_val.c_str());
	}
	for (int r = 0; r < 6; r++)
	{
		int addr = (FLOAT_REG_0 + r) *  FLOAT_REG_SIZE_BYTES;
		std::string f_str;
		for (int i = 0; i < 4; i++)
		{
			float f = *(float*)&dbg.mem_buffer[addr + i * 4];
			f_str += std::to_string(f);
			f_str += " ";

		}
		ImGui::Text("xmm[%d, &%d]: %s", r, addr, f_str.c_str());
	}
	for (int r = 0; r < 6; r++)
	{
		int addr = base_ptr + fdecl->to_spill_offset + r * 8;
		mem_val = GetMemAddrString(dbg, addr, type_sz, 8, mem_wnd_show_type);

		std::string addr_name = WasmNumToString(&dbg, mem_wnd_offset + r * 8);
		ImGui::Text("spill[%d, &%d]: %s", r, addr, mem_val.c_str());
	}
	ImGui::Separator();
	ImGui::EndChild();
}
bool IsKeyRepeat(void* data, int key);
void ClearKeys(void* data);
void PrintExpressionTkns(dbg_state* dbg, own_std::vector<token2> *tkns)
{
	mem_alloc temp_alloc;
	temp_alloc.chunks_cap = 1024 * 1024;

	InitMemAlloc(&temp_alloc);
	void* prev_alloc = __lang_globals.data;
	dbg_expr* exp = nullptr;
	int val = setjmp(dbg->lang_stat->jump_buffer);
	if (val == 0)
	{
		dbg->lang_stat->flags |= PSR_FLAGS_ON_JMP_WHEN_ERROR;
		exp = WasmGetExprFromTkns(dbg, tkns);
	}
	// error
	else if (val == 1)
	{
		
	}
	if(exp)
		printf("\n%s\n", WasmGetSingleExprToStr(dbg, exp).c_str());

	FreeMemAlloc(&temp_alloc);

	__lang_globals.data = prev_alloc;

}

/*
//void UpdateLastTime(dbg_state* dbg);
void WasmOnArgs(dbg_state* dbg)
{
	bool args_break = false;
	bool first_timer = true;

	printf("cur_block_bc %d\n", __lang_globals.cur_block);
	while (!args_break)
	{

		if (first_timer)
		{
			if ((dbg->break_type == DBG_BREAK_ON_NEXT_BC)
				|| (*dbg->cur_bc)->dbg_brk || (*dbg->cur_bc)->one_time_dbg_brk)
			{
				//WasmPrintStackAndBcs(dbg, 16);
			}
			dbg->break_type = DBG_BREAK_ON_DIFF_STAT;
			(*dbg->cur_bc)->one_time_dbg_brk = false;
		}
		first_timer = false;

		func_decl* func = dbg->cur_func;
		stmnt_dbg* stmnt = dbg->cur_st;


		for (int i = stmnt->line - 2; i < (stmnt->line + 1); i++)
		{
			i = max(0, i);
			i = min(func->from_file->lines.size(), i);
			char* cur_line = func->from_file->lines[i];

			if(i == (stmnt->line - 1))
				printf(ANSI_BG_WHITE ANSI_BLACK "%s" ANSI_RESET, cur_line);
			else
				printf("%s", cur_line);
			printf("\n", cur_line);
		}

		std::string input;

		// chatgt helped me here
		unsigned char ch;
		int cursorPos = 0;
		bool done = false;

		while (!done) {
			ch = _getch(); // Get a single character

			switch (ch) {
			case '\r': // Enter key
				cursorPos = 0;
				done = true;
			break;
			case '\t': // Tab key
				InsertSuggestion(dbg, input, &cursorPos);

				printInput(input, cursorPos);
			//displaySuggestions(input);
			break;
			case 27: // ESC key
				done = true;
			break;
			case 0:  // Special keys (like arrow keys) start with 0x00
			case 0xE0:  // For extended keys
				ch = _getch(); // Get the actual special key code
				if (ch == 75) { // Left arrow
					if (cursorPos > 0) {
						std::cout << "\033[" << 1 << "D";  // Move the cursor back
						--cursorPos;
					}
				} else if (ch == 77) { // Right arrow
					if (cursorPos < input.size()) {
						++cursorPos;
						std::cout << "\033[" << 1 << "C";  // Move the cursor back
					}
				} else if (ch == 83) { // DEL key
					if (cursorPos < input.size()) {
						input.erase(cursorPos, 1);
						printInput(input, cursorPos);
					}
				}
			break;
			default:
				if (ch == 8) { // Backspace key
					if (cursorPos > 0) {
						input.erase(cursorPos - 1, 1);
						--cursorPos;
						printInput(input, cursorPos);
					}
				} else {
					input.insert(cursorPos, 1, ch);
					++cursorPos;
					printInput(input, cursorPos);
				}
			break;
			}
		}


		//std::cin >> input;
		own_std::vector<std::string> args;

		own_std::vector<token2> tkns;
		Tokenize2((char*)input.c_str(), input.size(), &tkns);

		if (tkns[0].type == T_WORD && tkns[0].str == "print")
		{
			int i = 1;
			ASSERT(tkns[1].type == T_WORD);
			if (tkns[i].str == "wasm")
			{
				i++;
				bool can_break = false;

				func_decl* fdecl = dbg->cur_func;
				wasm_bc* cur_bc = *dbg->cur_bc;
				stmnt_dbg* cur_st = dbg->cur_st;

				int lines = -1;
				if (tkns[i].type == T_INT)
				{
					lines = tkns[i].i;
					can_break = true;
				}
				bool print_bc = true;
				while (!can_break)
				{
					can_break = true;
					if (tkns[i].str == "func")
					{
						i++;

						ASSERT(tkns[i].type == T_WORD);
						fdecl = FuncAddedWasmInterp(dbg->lang_stat->winterp, tkns[i].str);
						cur_st = fdecl->wasm_stmnts.begin();
						cur_bc = dbg->bcs.begin() + cur_st->start;
						i++;
					}
					else if (tkns[i].str == "ir")
					{
						i++;
						ASSERT(tkns[i].type == T_INT);

						print_bc = false;
						lines = tkns[i].i;
					}
					else if (tkns[i].str == "lines")
					{
						i++;
						ASSERT(tkns[i].type == T_INT);

						lines = tkns[i].i;
					}

					if (tkns[i].type == T_COMMA)
					{
						can_break = false;
						i++;
					}
				}

				//std::string ret = WasmPrintCodeGranular(dbg, fdecl, cur_bc, cur_st->start, cur_st->end, lines, true, print_bc);
				printf("%s\n", ret.c_str());
			}
			else if (tkns[i].str == "ex")
			{
				i++;
				std::string exp_str = "";
				tkns.ar.start += i;
				PrintExpressionTkns(dbg, &tkns);
				tkns.ar.start -= i;


			}
			else if (tkns[i].str == "callstack")
			{
				int idx = dbg->func_stack.size() - 1;
				FOR_VEC(func, dbg->func_stack)
				{
					func_decl* f = *func;
					wasm_bc* ret_bc = dbg->return_stack[idx];
					int ret_bc_idx = (ret_bc - &dbg->bcs[0]);
					stmnt_dbg *ret_st = GetStmntBasedOnOffset(&f->wasm_stmnts, ret_bc_idx);
					//printf("func %s, ln: %d\n", f->name.c_str(), ret_st->line);
					printf("func %s\n", f->name.c_str());
					idx--;
				}
			}
		}
		std::string aux;
		split(input, ' ', args, &aux);

		if (args[0] == "print")
		{
			if (args.size() == 1)
				continue;

			else if (args[1] == "locals")
			{
				printf("%s\n", WasmPrintVars(dbg).c_str());
			}
			else if (args[1] == "filter")
			{
				std::string exp_str = "";
				for (int i = 2; i < args.size(); i++)
				{
					exp_str += args[i];
				}
				mem_alloc temp_alloc;
				temp_alloc.chunks_cap = 1024 * 1024;

				InitMemAlloc(&temp_alloc);
				void* prev_alloc = __lang_globals.data;
				dbg_expr* exp = nullptr;
				int val = setjmp(dbg->lang_stat->jump_buffer);
				if (val == 0)
				{
					dbg->lang_stat->flags |= PSR_FLAGS_ON_JMP_WHEN_ERROR;
					exp = WasmGetExprFromStr(dbg, exp_str);
				}
				// error
				else if (val == 1)
				{
					
				}
				if (!exp)
					continue;

				typed_stack_val expr_val;
				typed_stack_val x_times;
				WasmFromAstArrToStackVal(dbg, exp->expr, &expr_val);
				WasmFromAstArrToStackVal(dbg, exp->x_times, &x_times);

				type2 dummy_type;
				ASSERT(expr_val.type.type == TYPE_STRUCT && expr_val.type.ptr);

				expr_val.offset = WasmGetMemOffsetVal(dbg, expr_val.offset);
				int start_offset = expr_val.offset;
				char saved_regs[512];

				memcpy(saved_regs, dbg->mem_buffer, 128);

				std::string filtered = "";
				decl2* strct_decl = expr_val.type.strct->this_decl;

				for (int i = 0; i < x_times.offset; i++)
				{
					int cur_offset = start_offset + i * expr_val.type.strct->size;
					int* reg = (int*)&dbg->mem_buffer[FILTER_PTR * 8];
					*reg = cur_offset;
					//wasm_bc *bc = exp->f

					ASSERT(0)
					WasmIrInterp(dbg, (own_std::vector<int>*)&exp->filter_cond);

					char ret = WasmGetRegVal(dbg, RET_1_REG);
					if (ret == 1)
					{
						filtered += WasmVarToString(dbg, 0, strct_decl, cur_offset, 0);
					}
				}

				printf("%s", filtered.c_str());

				memcpy(dbg->mem_buffer, saved_regs, 128);
				FreeMemAlloc(&temp_alloc);

				__lang_globals.data = prev_alloc;


			}
			else if (args[1] == "onnext")
			{
				std::string exp_str = "";
				for (int i = 2; i < args.size(); i++)
				{
					exp_str += args[i];
				}
				mem_alloc temp_alloc;
				temp_alloc.chunks_cap = 1024 * 1024;

				InitMemAlloc(&temp_alloc);
				void* prev_alloc = __lang_globals.data;
				dbg_expr* exp = nullptr;
				int val = setjmp(dbg->lang_stat->jump_buffer);
				if (val == 0)
				{
					dbg->lang_stat->flags |= PSR_FLAGS_ON_JMP_WHEN_ERROR;
					exp = WasmGetExprFromStr(dbg, exp_str);
				}
				// error
				else if (val == 1)
				{
					
				}
				if (!exp)
					continue;
				ASSERT(exp->type == DBG_EXPR_SHOW_SINGLE_VAL);

				typed_stack_val expr_val;
				WasmFromAstArrToStackVal(dbg, exp->expr, &expr_val);

				type2 dummy_type;
				decl2 *next_var = FindIdentifier("next", expr_val.type.strct->scp, &dummy_type);
				ASSERT(expr_val.type.type == TYPE_STRUCT && next_var);
				ASSERT(next_var->type.type == TYPE_STRUCT && next_var->type.strct == expr_val.type.strct);
				ASSERT(next_var->type.ptr == 1);

				//expr_val.offset = WasmGetMemOffsetVal(dbg, expr_val.offset);
				if (expr_val.type.ptr > 0)
				{
					expr_val.offset = WasmGetMemOffsetVal(dbg, expr_val.offset);
				}
				int cur_offset = expr_val.offset;

				std::string struct_str = WasmVarToString(dbg, 0, next_var->type.strct->this_decl, cur_offset, 0);
				while (WasmGetMemOffsetVal(dbg, cur_offset + next_var->offset) != 0)
				{
					cur_offset = WasmGetMemOffsetVal(dbg, cur_offset + next_var->offset);
					struct_str += WasmVarToString(dbg, 0, next_var->type.strct->this_decl, cur_offset, 0);

				}

				printf("%s", struct_str.c_str());


				FreeMemAlloc(&temp_alloc);

				__lang_globals.data = prev_alloc;
			}
			else if (args[1] == "ex")
			{
			}
			else if (args[1] == "exs")
			{
				WasmPrintExpressions(dbg);

			}
			else if (args[1] == "stack")
			{
				printf("%s\n", WasmGetStack(dbg).c_str());
			}
			else if (args[1] == "regs")
			{
				WasmPrintRegisters(dbg);
			}
		}
		else if (args[0] == "con")
		{
			dbg->break_type = DBG_NO_BREAK;
			args_break = true;
		}
		else if (args[0] == "abs")
		{
			if (args.size() == 1)
			{
				continue;
			}
			int offset = atof(args[1].c_str());
			void *addr = dbg->mem_buffer + offset;
			printf("\naddr: %p", (void *)addr);
		}
		else if (args[0] == "babsi")
		{
			if (args.size() == 1)
			{
				continue;
			}
			int offset = atof(args[1].c_str());
			auto ir = (own_std::vector<ir_rep>*) &dbg->cur_func->ir;
			ir_rep* i = &((*ir)[offset]);
			int bc_idx = i->start;
			wasm_bc* wbc = &dbg->bcs[bc_idx];
			wbc->dbg_brk = true;
		}
		else if (args[0] == "brelw")
		{
			if (args.size() == 1)
			{
				continue;
			}
			int offset = atof(args[1].c_str());

			((*dbg->cur_bc) + offset)->dbg_brk = true;


		}
		else if (args[0] == "set")
		{
			if (args.size() == 1)
				continue;

			if (args[1] == "numformat")
			{
				if (args.size() == 2)
					continue;
				if (args[2] == "hex")
				{
					dbg->print_numbers_format = DBG_PRINT_HEX;
				}
				else if (args[2] == "dec")
				{
					dbg->print_numbers_format = DBG_PRINT_DECIMAL;
				}
			}
		}
		else if (args[0] == "new")
		{
			if (args.size() == 2)
				continue;
			if (args[1] == "ex")
			{
				std::string exp_str = "";
				for (int i = 2; i < args.size(); i++)
				{
					exp_str += args[i];
				}


				dbg_expr* exp = WasmGetExprFromStr(dbg, exp_str);
				dbg->exprs.emplace_back(exp);
				int a = 0;
			}
		}
		else if (args[0] == "ns")
		{ 
			WasmBreakOnNextStmnt(dbg, &args_break);
		}
		// step into func
		else if (args[0] == "si")
		{
			wasm_bc* cur = *dbg->cur_bc;
			wasm_bc* end = dbg->bcs.begin() + dbg->cur_st->end;
			while (cur <= end && (cur->type != WASM_INST_CALL && cur->type != WASM_INST_INDIRECT_CALL))
			{
				cur++;
			}
			// found call
			if (cur <= end)
			{
				func_decl* first_func = dbg->wasm_state->funcs[cur->i];

				stmnt_dbg* first_st = &first_func->wasm_stmnts[0];

				wasm_bc* first_bc = dbg->bcs.begin() + first_st->start;
				args_break = true;
				first_bc->one_time_dbg_brk = true;
				
			}
			else
			{
				WasmBreakOnNextStmnt(dbg, &args_break);
			}

		}
		// previous ir
		else if (args[0] == "pi")
		{
			args_break = true;
			ir_rep* cur = dbg->cur_ir;
			while (cur->start == cur->end)
				cur->start--;
			cur--;

			wasm_bc *to = dbg->bcs.begin() + cur->start;

			WasmModifyCurBcPtr(dbg, to);
			to->one_time_dbg_brk = true;
		}
		// next ir
		else if (args[0] == "ni")
		{
			args_break = true;
			ir_rep* cur = dbg->cur_ir;
			while (cur->start == cur->end)
				cur->start++;
			cur++;

			wasm_bc* cur_ir_bc = dbg->bcs.begin() + cur->start;
			cur_ir_bc->one_time_dbg_brk = true;
			dbg->some_bc_modified = true;

			dbg->break_type = DBG_NO_BREAK;
		}
		else if (args[0] == "nw")
		{
			args_break = true;
			dbg->break_type = DBG_BREAK_ON_NEXT_BC;

		}
	}
	//UpdateLastTime(dbg);
}
*/

void JsPrint(dbg_state* dbg)
{
	*(int*)&dbg->mem_buffer[RET_1_REG * 8] = 7;
}
void WasmDoCallInstruction(dbg_state *dbg, wasm_bc **bc, block_linked **cur, func_decl *call_f)
{
	dbg->cur_func = call_f;
	dbg->func_stack.emplace_back(call_f);
	dbg->block_stack.emplace_back(*cur);
	dbg->return_stack.emplace_back(*bc);
	int idx = call_f->wasm_code_sect_idx - 1;
	//if (idx < 0)
		//idx = 0;
	*bc = dbg->bcs.begin() + idx;
	*cur = nullptr;

}
void WasmSerializePushU32IntoVector(own_std::vector<unsigned char>* out, int val)
{
	out->emplace_back(val & 0xff);
	out->emplace_back((val & 0xff00)>>8);
	out->emplace_back((val & 0xff0000)>>16);
	out->emplace_back((val & 0xff000000)>>24);
}
struct str_dbg
{
	int name_on_string_sect;
	int name_len;
};
struct struct_dbg
{
	str_dbg name;
	int struct_size;
	int vars_offset;
	int num_of_vars;
};
struct var_dbg
{
	str_dbg name;
	str_dbg ptr_has_len_var_name;
	enum_type2 type;
	int type_idx;
	int offset;
	int ar_size;
	int flags;
	char ptr;
	decl2* decl;
};
#define TYPE_DBG_CREATED_TYPE_STRUCT 1
#define TYPE_DBG_CREATED_TYPE_ENUM 2
struct type_dbg
{
	enum_type2 type;
	union
	{
		struct
		{
			import_type imp_type;
			str_dbg alias;
			union
			{
				int file_idx;
				unit_file *file;
			};
		}imp;
		int struct_type_offset;
		str_dbg name;
		type_struct2 *strct;
		decl2 *e_decl;
		func_decl *fdecl;
		unit_file *file;
	};
	char ptr;
	int struct_size;
	int vars_offset;
	int num_of_vars;
	int flags;
};
struct scope_dbg
{
	int vars_offset;
	int num_of_vars;

	int imports;
	int num_of_imports;

	unsigned int parent;
	unsigned int next_sibling;
	int children;
	int children_len;

	int line_start;
	int line_end;

	scp_type type;
	union
	{
		int type_idx;
		int func_idx;
		int file_idx;
	};
	bool created;
	scope* scp;
};
struct file_dbg
{
	union
	{
		str_dbg name;
		unit_file* fl;
	};
	int scp_offset;
};
struct func_dbg
{
	union
	{
		str_dbg name;
		decl2* decl;
	}; 
	enum_type2 ret_type;
	char ret_ptr_type;
	int ret_type_offset;

	int scope;
	int code_start;

	int stmnts_offset;
	int stmnts_len;

	int ir_offset;
	int ir_len;

	int file_idx;

	int flags;
	bool created;

	int ir_begin_stack_idx;

	int strct_constr_offset;
	int strct_ret_offset;
	int to_spill_offset;
	int for_interpreter_x64_code_start;
	int x64_code_start;

	int bcs2_start;
	int bcs2_end;

	int stack_size;

	int line;
};
struct serialize_state
{
	own_std::vector<unsigned char> func_sect;
	own_std::vector<unsigned char> stmnts_sect;
	own_std::vector<unsigned char> string_sect;
	//own_std::vector<unsigned char> ts_sect;
	own_std::vector<unsigned char> scopes_sect;
	own_std::vector<unsigned char> types_sect;
	own_std::vector<unsigned char> vars_sect;
	own_std::vector<unsigned char> ir_sect;
	own_std::vector<unsigned char> bc2_sect;
	own_std::vector<unsigned char> file_sect;
	own_std::vector<unsigned char> data_sect;
	own_std::vector<unsigned char> x64_code;

	own_std::vector<func_decl*> serialized_funcs;

	unsigned int f32_type_offset;
	unsigned int u32_type_offset;
};
void WasmSerializePushString(serialize_state* ser_state, std::string* name, str_dbg *);
void WasmSerializeStructType(web_assembly_state* wasm_state, serialize_state* ser_state, type_struct2* strct);

var_dbg *WasmSerializeSimpleVar(web_assembly_state* wasm_state, serialize_state* ser_state, decl2* var, int var_idx)
{
	auto var_ser = (var_dbg*)(ser_state->vars_sect.begin() + var_idx);
	memset(var_ser, 0, sizeof(var_dbg));
	if (var->type.type == TYPE_STRUCT )
	{
		type_struct2* strct = var->type.strct;
		if(IS_FLAG_OFF(strct->flags, TP_STRCT_STRUCT_SERIALIZED));
		{
			WasmSerializeStructType(wasm_state, ser_state, strct);
		}
		var_ser->type_idx = strct->serialized_type_idx;
	}
	if(IS_FLAG_ON(var->flags, DECL_PTR_HAS_LEN))
	{
		WasmSerializePushString(ser_state, &var->len_for_ptr->name, &var_ser->ptr_has_len_var_name);
	}
	var_ser->type = var->type.type;
	var_ser->ptr = var->type.ptr;
	var_ser->offset = var->offset;
	var_ser->flags = var->flags;
	WasmSerializePushString(ser_state, &var->name, &var_ser->name);
	return var_ser;
}
void WasmSerializeStructType(web_assembly_state* wasm_state, serialize_state* ser_state, type_struct2* strct)
{
	if (IS_FLAG_ON(strct->flags, TP_STRCT_STRUCT_SERIALIZED))
		return;

	int type_offset = ser_state->types_sect.size();
	int var_offset = ser_state->vars_sect.size();

	/*
	FOR_VEC(decl, strct->vars)
	{
		decl2* d = *decl;
		// self
		if (d->type.strct == strct)
			continue;
		switch (d->type.type)
		{
		case TYPE_STRUCT:
		case TYPE_STRUCT_TYPE:
		{
			if(IS_FLAG_OFF(d->type.strct->flags, TP_STRCT_STRUCT_SERIALIZED))
				WasmSerializeStructType(wasm_state, ser_state, d->type.strct);
		}break;
		}
	}
	*/


	strct->serialized_type_idx = type_offset;

	//ser_state->vars_sect.make_count(ser_state->vars_sect.size() + strct->vars.size() * sizeof(var_dbg));
	ser_state->types_sect.make_count(ser_state->types_sect.size() + sizeof(type_dbg));

	auto type = (type_dbg*)(ser_state->types_sect.begin() + type_offset);
	memset(type, 0, sizeof(type_dbg));
	type->num_of_vars = strct->vars.size();
	type->vars_offset = var_offset;
	type->struct_size = strct->size;
	WasmSerializePushString(ser_state, &strct->name, &type->name);

	/*
	int i = 0; 
	FOR_VEC(decl, strct->vars)
	{
		decl2* d = *decl;
		
		WasmSerializeSimpleVar(wasm_state, ser_state, d, var_offset + i * sizeof(var_dbg));
		i++;
	}
	*/
	strct->flags |= TP_STRCT_STRUCT_SERIALIZED;
}

void IrChangeDeclAddrToTypeOffset(ir_val* val)
{
	if(val->type == IR_TYPE_DECL)
		val->i = val->decl->serialized_type_idx;
}
void WasmSerializeFuncIr(serialize_state *ser_state, func_decl *fdecl)
{
	auto ir_ar = (own_std::vector<ir_rep> *) &fdecl->ir;
	auto fdbg = (func_dbg*)(ser_state->func_sect.begin() + fdecl->func_dbg_idx);
	fdbg->ir_offset = ser_state->ir_sect.size();
	fdbg->ir_len = ir_ar->size();
	FOR_VEC(ir, (*ir_ar))
	{
		switch (ir->type)
		{
		case IR_STACK_BEGIN:
		{
			func_decl* call = ir->fdecl;
			ir->num = call->func_dbg_idx;
		}break;
		case IR_RET:
		{
			IrChangeDeclAddrToTypeOffset(&ir->ret.assign.to_assign);
			IrChangeDeclAddrToTypeOffset(&ir->ret.assign.lhs);
			if (!ir->ret.assign.only_lhs)
				IrChangeDeclAddrToTypeOffset(&ir->ret.assign.rhs);
		}break;
		case IR_CAST_F32_TO_INT:
		case IR_CAST_INT_TO_F32:
		case IR_CAST_INT_TO_INT:
		case IR_CMP_NE:
		case IR_CMP_LT:
		case IR_CMP_LE:
		case IR_CMP_GT:
		case IR_CMP_GE:
		case IR_CMP_EQ:
		{
			IrChangeDeclAddrToTypeOffset(&ir->bin.lhs);
			IrChangeDeclAddrToTypeOffset(&ir->bin.rhs);
		}break;
		case IR_ASSIGNMENT:
		{
			IrChangeDeclAddrToTypeOffset(&ir->assign.to_assign);
			IrChangeDeclAddrToTypeOffset(&ir->assign.lhs);
			if (!ir->assign.only_lhs)
				IrChangeDeclAddrToTypeOffset(&ir->assign.rhs);
		}break;
		case IR_INDIRECT_CALL:
		{
			if (ir->bin.lhs.type == IR_TYPE_DECL)
			{
				decl2* decl = ir->bin.lhs.decl;
				ASSERT(IS_FLAG_ON(decl->flags, DECL_IS_SERIALIZED));
				ir->call.i = decl->serialized_type_idx;
			}
			else
			{
				//decl2* decl = ir->bin.lhs.decl;
				//ASSERT(IS_FLAG_ON(decl->flags, DECL_IS_SERIALIZED));
				ir->call.i = 0;
			}
		}break;
		case IR_CALL:
		{
			func_decl* call = ir->call.fdecl;
			ir->call.i = call->func_dbg_idx;
		}break;
		}
	}
	unsigned char* start = (unsigned char *)ir_ar->begin();
	unsigned char* end = (unsigned char *)ir_ar->end();
	ser_state->ir_sect.insert(ser_state->ir_sect.end(), start, end);
}
void TestFuncsWithSameName(own_std::vector<func_decl*> *funcs, std::string name, own_std::vector<int>* idxs)
{
	int i = 0;

	FOR_VEC(f_ptr, *funcs)
	{
		if ((*f_ptr)->name == name)
		{
			idxs->emplace_back(i);
		}
		i++;
	}
}
void WasmSerializeFunc(web_assembly_state* wasm_state, serialize_state *ser_state, func_decl *f)
{
	if (IS_FLAG_ON(f->flags, FUNC_DECL_SERIALIZED))
		return;
	bool is_outsider = IS_FLAG_ON(f->flags, FUNC_DECL_IS_OUTSIDER);
	int func_offset = ser_state->func_sect.size();
	int stmnt_offset = ser_state->stmnts_sect.size();


	ser_state->func_sect.make_count(ser_state->func_sect.size() + sizeof(func_dbg));
	auto fdbg = (func_dbg*)(ser_state->func_sect.begin() + func_offset);
	memset(fdbg, 0, sizeof(func_dbg));

	if (!is_outsider)
	{
		ser_state->stmnts_sect.insert(ser_state->stmnts_sect.end(), (unsigned char*)f->wasm_stmnts.begin(), (unsigned char*)f->wasm_stmnts.end());

		own_std::vector<ir_rep>* ir_ar = (own_std::vector<ir_rep> *) &f->ir;
	}

	fdbg->scope = f->scp->serialized_offset;
	fdbg->code_start = f->wasm_code_sect_idx;
	fdbg->line = f->func_node->t->line;
	fdbg->stmnts_offset = stmnt_offset;
	fdbg->flags = f->flags;
	fdbg->stmnts_len = f->wasm_stmnts.size();
	fdbg->strct_constr_offset = f->strct_constrct_at_offset;
	fdbg->strct_ret_offset = f->strct_ret_size_per_statement_offset;
	fdbg->to_spill_offset = f->to_spill_offset;
	fdbg->for_interpreter_x64_code_start = f->for_interpreter_code_start_idx;
	fdbg->stack_size = f->stack_size;
	fdbg->x64_code_start = f->code_start_idx;
	fdbg->bcs2_start = f->bcs2_start;
	fdbg->bcs2_end = f->bcs2_end;
	fdbg->ret_type = f->ret_type.type;
	fdbg->ret_ptr_type = f->ret_type.ptr;
	fdbg->ir_begin_stack_idx = f->ir_stack_begin_idx;

	if (f->ret_type.type == TYPE_STRUCT)
	{
		WasmSerializeStructType(wasm_state, ser_state, f->ret_type.strct);
		fdbg->ret_type_offset = f->ret_type.strct->serialized_type_idx;
	}
	WasmSerializePushString(ser_state, &f->name, &fdbg->name);

	f->func_dbg_idx = func_offset;
	f->scp->type = SCP_TYPE_FUNC;
	f->flags |= FUNC_DECL_SERIALIZED;

	own_std::vector<int> idxs;
	ser_state->serialized_funcs.emplace_back(f);
	TestFuncsWithSameName(&ser_state->serialized_funcs, f->name, &idxs);
	// functions with same name
	ASSERT(idxs.size() == 1);
}
int WasmSerializeType(web_assembly_state* wasm_state, serialize_state* ser_state, type2 *tp)
{
	int offset = ser_state->types_sect.size();

	ser_state->types_sect.make_count(ser_state->types_sect.size() + sizeof(type_dbg));

	auto type = (type_dbg *)(ser_state->types_sect.begin() + offset);
	if (tp->type == TYPE_IMPORT)
	{
		type->imp.imp_type = tp->imp->type;
		type->imp.file_idx = tp->imp->fl->file_dbg_idx;

		WasmSerializePushString(ser_state, &tp->imp->alias, &type->imp.alias);
		//type->imp.alias = tp->imp->fl->file_dbg_idx;
	}
	else
	{
		type->type = tp->type;
		type->ptr = tp->ptr;
		if(tp->type == TYPE_STRUCT)
			type->struct_type_offset = tp->strct->serialized_type_idx;
	}
	return offset;

}
void WasmSerializeScope(web_assembly_state* wasm_state, serialize_state *ser_state, scope *scp, unsigned int parent, unsigned int scp_offset)
{
	if(IS_FLAG_ON(scp->flags, SCOPE_SKIP_SERIALIZATION))
		return;
	struct f_ptr_info
	{
		int var_dbg_idx;
		decl2* decl;
	};
	if (IS_FLAG_ON(scp->flags, SCOPE_INSIDE_STRUCT))
	{
		int a = 0;
	}

	INSERT_VEC(scp->vars, scp->imports);

	int vars_offset = ser_state->vars_sect.size();
	int children_offset = ser_state->scopes_sect.size();

	//ser_state->scopes_sect.make_count(ser_state->scopes_sect.size() + sizeof(scope_dbg));
	int size_children = scp->children.size() * sizeof(scope_dbg);
	ser_state->scopes_sect.make_count(ser_state->scopes_sect.size() + size_children);
	memset(ser_state->scopes_sect.begin() + children_offset, 0, size_children);
	ser_state->vars_sect.make_count(ser_state->vars_sect.size() + scp->vars.size() * sizeof(var_dbg));

	if (scp->type == SCP_TYPE_FUNC && scp->fdecl->name == "main")
		int a = 0;

	own_std::vector<f_ptr_info> func_ptr_idxs;
	int i = 0;
	FOR_VEC(decl, scp->vars)
	{
		decl2* d = *decl;
		var_dbg* vdbg = nullptr;
		int var_offset = vars_offset + i * sizeof(var_dbg);
		d->serialized_type_idx = var_offset;
		vdbg = WasmSerializeSimpleVar(wasm_state, ser_state, d, var_offset);

		switch (d->type.type)
		{
		case TYPE_ENUM_TYPE:
		{
			decl2* e_decl = d->type.e_decl;
			e_decl->type.scp->e_decl = e_decl;
			e_decl->type.scp->type = SCP_TYPE_ENUM;
			unsigned int type_offset = ser_state->types_sect.size();

			//ser_state->vars_sect.make_count(ser_state->vars_sect.size() + strct->vars.size() * sizeof(var_dbg));
			ser_state->types_sect.make_count(ser_state->types_sect.size() + sizeof(type_dbg));

			auto type = (type_dbg*)(ser_state->types_sect.begin() + type_offset);
			memset(type, 0, sizeof(type_dbg));

			e_decl->offset = type_offset;


			WasmSerializePushString(ser_state, &e_decl->name, &type->name);
			vdbg->type_idx = type_offset;
		}break;
		case TYPE_TEMPLATE:
		{
			
		}break;
		case TYPE_FUNC_EXTERN:
		{
			int a = 0;
			goto func_label;
		};
		case TYPE_FUNC:
		{
			func_label:
			func_decl* f = d->type.fdecl;
			bool is_outsider = IS_FLAG_ON(f->flags, FUNC_DECL_IS_OUTSIDER);
			vdbg->type_idx = ser_state->func_sect.size();
			if (IS_FLAG_ON(f->flags, FUNC_DECL_SERIALIZED))
				vdbg->type_idx = f->func_dbg_idx;

			if (IS_FLAG_ON(f->flags, FUNC_DECL_LABEL | FUNC_DECL_INTERNAL | FUNC_DECL_SERIALIZED | FUNC_DECL_SERIALIZED | FUNC_DECL_TEMPLATED))
				break;

			WasmSerializeFunc(wasm_state, ser_state, f);
		}break;
		case TYPE_MACRO_EXPR:
		{

		}break;
		case TYPE_STRUCT_TYPE:
		{
			d->type.strct->scp->type = SCP_TYPE_STRUCT;
			WasmSerializeStructType(wasm_state, ser_state, d->type.strct);
			vdbg->type_idx = d->type.strct->serialized_type_idx;
		}break;
		case TYPE_FUNC_PTR:
		{
			f_ptr_info info = {  };
			info.decl = d;
			info.var_dbg_idx = var_offset;
			func_ptr_idxs.emplace_back(info);

		}break;
		case TYPE_STRUCT:
		{
			ASSERT(IS_FLAG_ON(d->type.strct->flags, TP_STRCT_STRUCT_SERIALIZED));
			if (IS_FLAG_ON(d->type.flags, TYPE_SELF_REF))
			{
				vdbg->type_idx = d->type.strct->serialized_type_idx;
			}
		}break;
		case TYPE_IMPORT:
		{
			vdbg->type_idx = WasmSerializeType(wasm_state, ser_state, &d->type);
		}break;
		case TYPE_STATIC_ARRAY:
		{
			vdbg->type_idx = WasmSerializeType(wasm_state, ser_state, d->type.tp);
			vdbg->ar_size = d->type.ar_size;
		}break;
		case TYPE_ENUM:
		{
			vdbg->type_idx = d->type.from_enum->offset;
		}break;
		case TYPE_BOOL:
		case TYPE_INT:
		case TYPE_F32_RAW:
		case TYPE_F32:
		case TYPE_U8:
		case TYPE_S8:
		case TYPE_U16:
		case TYPE_S16:
		case TYPE_S32:
		case TYPE_U32:
		case TYPE_S64:
		case TYPE_VOID:
		case TYPE_U64:
		case TYPE_FUNC_TYPE:
		case TYPE_STR_LIT:
		case TYPE_ENUM_IDX_32:
		case TYPE_OVERLOADED_FUNCS:
		case TYPE_U32_TYPE:
		case TYPE_S32_TYPE:
		case TYPE_CHAR_TYPE:
		case TYPE_VECTOR_TYPE:
		case TYPE_U8_TYPE:
		case TYPE_CHAR:
		case TYPE_VECTOR:
		
		break;
		default:
			ASSERT(0);
		}
		d->flags |= DECL_IS_SERIALIZED;
		i++;

	}
	FOR_VEC(f_ptr, func_ptr_idxs)
	{
		auto vdbg = (var_dbg*)(ser_state->vars_sect.begin() + f_ptr->var_dbg_idx);
		func_decl* f = f_ptr->decl->type.fdecl;
		// will skip the scope because it a func ptr declared as variable, and not outsiders or nomal functions
		if (!IS_FLAG_ON(f->flags, FUNC_DECL_SERIALIZED))
		{
			f->scp->flags |= SCOPE_SKIP_SERIALIZATION;
		}

		bool is_outsider = IS_FLAG_ON(f->flags, FUNC_DECL_IS_OUTSIDER);
		vdbg->type_idx = f->func_dbg_idx;
	}
	i = 0;
	FOR_VEC(ch, scp->children)
	{
		scope* c = *ch;
		int offset = children_offset + sizeof(scope_dbg) * i;
		WasmSerializeScope(wasm_state, ser_state, c, scp_offset, offset );
		auto cur = (scope_dbg*)(ser_state->scopes_sect.begin() + scp_offset);
		auto s = (scope_dbg*)(ser_state->scopes_sect.begin() + offset);
		s->next_sibling = ser_state->scopes_sect.size();
		i++;
	}

	auto s = (scope_dbg*)(ser_state->scopes_sect.begin() + scp_offset);
	memset(s, 0, sizeof(scope_dbg));
	s->children = children_offset;
	s->children_len = scp->children.size();
	s->num_of_vars  = scp->vars.size();
	s->line_start = scp->line_start;
	s->line_end = scp->line_end;
	s->vars_offset  = vars_offset;
	s->type = SCP_TYPE_UNDEFINED;




	switch(scp->type)
	{
	case SCP_TYPE_FUNC:
	{
		if (IS_FLAG_OFF(scp->fdecl->flags, FUNC_DECL_TEMPLATED))
		{
			ASSERT(IS_FLAG_ON(scp->fdecl->flags, FUNC_DECL_SERIALIZED));
		}
		s->type = SCP_TYPE_FUNC;
		s->func_idx = scp->fdecl->func_dbg_idx;
	}break;
	case SCP_TYPE_FILE:
	{

		s->type = SCP_TYPE_FILE;
		s->file_idx = scp->file->file_dbg_idx;
	}break;
	case SCP_TYPE_ENUM:
	{
		s->type = SCP_TYPE_ENUM;
		s->type_idx = scp->e_decl->offset;
	}break;
	case SCP_TYPE_STRUCT:
	{
		ASSERT(IS_FLAG_ON(scp->tstrct->flags, TP_STRCT_STRUCT_SERIALIZED));
		s->type = SCP_TYPE_STRUCT;
		s->type_idx = scp->tstrct->serialized_type_idx;
	}break;
	}
	scp->serialized_offset = scp_offset;
	s->parent = parent;
}
void WasmSerializeBuiltTypes(serialize_state* ser_state)
{
	int offset = ser_state->types_sect.size();
	ser_state->f32_type_offset = offset;

	ser_state->types_sect.make_count(ser_state->types_sect.size() + sizeof(type_dbg));

	auto f32_type = (type_dbg *)(ser_state->types_sect.begin() + offset);
	f32_type->type = TYPE_F32;
}
void WasmSerializePushString(serialize_state* ser_state, std::string* name, str_dbg *out)
{
	out->name_on_string_sect = ser_state->string_sect.size();
	out->name_len = name->size();
	ASSERT(out->name_len <= 128 && out->name_on_string_sect < 100000 && out->name_on_string_sect >= 0);
	ser_state->string_sect.insert(ser_state->string_sect.end(), (unsigned char *)name->data(), (unsigned char *)(name->data() + name->size()));
}
void WasmSerialize(web_assembly_state* wasm_state, own_std::vector<unsigned char>& code, own_std::vector<byte_code2> &bcs2)
{
	own_std::vector<unsigned char> final_buffer;
	final_buffer.reserve(1024 * 8);
	serialize_state ser_state;
	//printf("\nscop : \n%s\n", wasm_state->lang_stat->root->Print(0).c_str());
	
	FOR_VEC(func, wasm_state->funcs)
	{
		func_decl* f = *func;
		auto fdbg = (func_dbg *)(ser_state.func_sect.begin() + f->func_dbg_idx);
		WasmSerializeFunc(wasm_state, &ser_state, f);
		fdbg = (func_dbg *)(ser_state.func_sect.begin() + f->func_dbg_idx);
		fdbg->file_idx = f->from_file->file_dbg_idx;
		//WasmSerializeFuncIr(&ser_state, f);

	}
	ser_state.file_sect.make_count(wasm_state->lang_stat->files.size() * sizeof(file_dbg));
	int i = 0;
	FOR_VEC(file, wasm_state->lang_stat->files)
	{
		unit_file* f = *file;
		
		int file_offset = i * sizeof(file_dbg);

		f->file_dbg_idx = file_offset;

		auto cur_dbg_f = (file_dbg*)(ser_state.file_sect.begin() + file_offset);

		WasmSerializePushString(&ser_state, &(f->path + f->name), &cur_dbg_f->name);
		i++;

	}

	WasmSerializeBuiltTypes(&ser_state);
	ser_state.scopes_sect.make_count(ser_state.scopes_sect.size() + sizeof(scope_dbg));
	WasmSerializeScope(wasm_state, &ser_state, wasm_state->lang_stat->root, -1, 0);

	i = 0;
	//printf("\nscop2: \n%s\n", wasm_state->lang_stat->root->Print(0).c_str());

	FOR_VEC(decl, wasm_state->lang_stat->funcs_scp->vars)
	{
		if ((*decl)->type.type == TYPE_OVERLOADED_FUNCS)
			continue;
		func_decl* f = (*decl)->type.fdecl;
		auto fdbg = (func_dbg *)(ser_state.func_sect.begin() + f->func_dbg_idx);
		//fdbg->ir_offset = ser_state.ir_sect.size();
		//fdbg->ir_len = f->ir.size();
		fdbg->file_idx = f->from_file->file_dbg_idx;
		WasmSerializeFuncIr(&ser_state, f);

	}
	/*
	ser_state.func_sect.make_count(ser_state.func_sect.size() + wasm_state->funcs.size() * sizeof(func_dbg));

	int i = 0;
	FOR_VEC(func, wasm_state->lang_stat->global_funcs)
	{
		func_decl* f = *func;
		auto fdbg = (func_dbg *) (ser_state.func_sect.begin() + i * sizeof(func_dbg));
		int stmnt_offset = ser_state.stmnts_sect.size();

		ser_state.stmnts_sect.insert(ser_state.stmnts_sect.end(), (unsigned char*)f->wasm_stmnts.begin(), (unsigned char*)f->wasm_stmnts.end());

		fdbg->scope = f->scp->serialized_offset;
		fdbg->stmnts_offset = stmnt_offset;
		fdbg->stmnts_len = f->wasm_stmnts.size();

		i++;
	}
	*/
	//final_buffer.make_count(sizeof(dbg_file));
	dbg_file_seriealize file;
	file.code_type = DBG_CODE_WASM;

	file.func_sect = final_buffer.size();
	INSERT_VEC(final_buffer, ser_state.func_sect);

	file.total_funcs = ser_state.func_sect.size() / sizeof(func_dbg);

	file.vars_sect = final_buffer.size();
	INSERT_VEC(final_buffer, ser_state.vars_sect);

	file.scopes_sect = final_buffer.size();
	INSERT_VEC(final_buffer, ser_state.scopes_sect);

	file.stmnts_sect = final_buffer.size();
	INSERT_VEC(final_buffer, ser_state.stmnts_sect);

	file.string_sect = final_buffer.size();
	INSERT_VEC(final_buffer, ser_state.string_sect);

	file.types_sect = final_buffer.size();
	INSERT_VEC(final_buffer, ser_state.types_sect);
	
	file.code_sect = final_buffer.size();
	INSERT_VEC(final_buffer, code);

	file.bc2_sect = final_buffer.size();
	file.bc2_sect_size = bcs2.size();
	final_buffer.insert(final_buffer.end(), (u8*)bcs2.begin(), (u8*)bcs2.end());

	file.ir_sect = final_buffer.size();
	INSERT_VEC(final_buffer, ser_state.ir_sect);

	file.files_sect = final_buffer.size();
	INSERT_VEC(final_buffer, ser_state.file_sect);

	file.x64_code_sect = final_buffer.size();
	INSERT_VEC(final_buffer, wasm_state->lang_stat->code_sect);
	file.x64_code_sect_size = wasm_state->lang_stat->code_sect.size();
	file.x64_code_type_sect_size = wasm_state->lang_stat->type_sect.size();

	file.data_sect = final_buffer.size();

	auto casted_ar = (own_std::vector<unsigned char> *) &wasm_state->lang_stat->data_sect;
	final_buffer.insert(final_buffer.end(), casted_ar->begin(), casted_ar->end());
	file.data_sect_size = wasm_state->lang_stat->data_sect.size();

	file.globals_sect = final_buffer.size();
	casted_ar = (own_std::vector<unsigned char> *) &wasm_state->lang_stat->globals_sect;
	final_buffer.insert(final_buffer.end(), casted_ar->begin(), casted_ar->end());
	file.globals_sect_size = casted_ar->size();

	file.total_files = wasm_state->lang_stat->files.size();

	final_buffer.insert(final_buffer.begin(), (unsigned char*)&file, ((unsigned char*)&file) + sizeof(dbg_file_seriealize));
	
	WriteFileLang((char *)(wasm_state->wasm_dir + wasm_state->folder_name + ".dbg").c_str(), final_buffer.begin(), final_buffer.size());

}
std::string WasmInterpNameFromOffsetAndLen(unsigned char* data, dbg_file_seriealize* file, str_dbg *name)
{
	ASSERT(name->name_on_string_sect >= 0);
	unsigned char* string_sect = data + file->string_sect;
	return std::string((const char *)string_sect + name->name_on_string_sect, name->name_len);
}

decl2 *WasmInterpBuildSimplaVar(unsigned char* data, unsigned int len, lang_state* lang_stat, dbg_file_seriealize* file, scope* scp_final, var_dbg* var)
{
	std::string name = WasmInterpNameFromOffsetAndLen(data, file, &var->name);
	type2 tp;
	tp.type = var->type;
	decl2* ret = NewDecl(lang_stat, name, tp);
	return ret;

}
decl2 *WasmInterpBuildStruct(unsigned char* data, unsigned int len, lang_state* lang_stat, dbg_file_seriealize* file, type_dbg *dstrct)
{
	if (IS_FLAG_ON(dstrct->flags, TYPE_DBG_CREATED_TYPE_STRUCT))
		return dstrct->strct->this_decl;

	unsigned char* string_sect = data + file->string_sect;
	auto cur_var = (var_dbg *)(data + file->vars_sect + dstrct->vars_offset);

	auto final_struct = (type_struct2* )AllocMiscData(lang_stat, sizeof(type_struct2));
	type2 tp;
	tp.type = TYPE_STRUCT_TYPE;
	tp.strct = final_struct;
	std::string name = WasmInterpNameFromOffsetAndLen(data, file, &dstrct->name);
	dstrct->strct = final_struct;
	final_struct->name = std::string(name);
	final_struct->size = dstrct->struct_size;
	dstrct->flags = TYPE_DBG_CREATED_TYPE_STRUCT;

	decl2* d = NewDecl(lang_stat, name, tp);
	final_struct->this_decl = d;
	return d;

}
unit_file* WasmInterpSearchFile(lang_state* lang_stat, std::string* name);
decl2 *WasmInterpBuildFunc(unsigned char *data, wasm_interp *winterp, lang_state *lang_stat, dbg_file_seriealize *file, func_dbg *fdbg)
{
	if (fdbg->created)
		return fdbg->decl;
	auto fdecl = (func_decl* )AllocMiscData(lang_stat, sizeof(func_decl));
	fdecl->code_start_idx = fdbg->code_start;
	fdecl->flags = fdbg->flags;
	fdecl->line = fdbg->line;

	std::string fname = WasmInterpNameFromOffsetAndLen(data, file, &fdbg->name);
	if (fname == "dyn_array_item[]")
		int a = 0;
	fdecl->name = std::string(fname);
	type2 tp;
	tp.type = TYPE_FUNC;
	tp.fdecl = fdecl;
	decl2* d = NewDecl(lang_stat, fname, tp);
	d->name = fdecl->name;

	//ret_scp->vars.emplace_back(d);


	if (IS_FLAG_OFF(fdecl->flags, FUNC_DECL_MACRO| FUNC_DECL_IS_OUTSIDER))
	{
		own_std::vector<ir_rep>* ir_ar = (own_std::vector<ir_rep> *) & fdecl->ir;
		auto start_ir = (ir_rep*)(data + file->ir_sect + fdbg->ir_offset);
		ir_ar->insert(ir_ar->begin(), start_ir, start_ir + fdbg->ir_len);

		auto start_st = (stmnt_dbg*)(data + file->stmnts_sect + fdbg->stmnts_offset);
		fdecl->wasm_stmnts.insert(fdecl->wasm_stmnts.begin(), start_st, start_st + fdbg->stmnts_len);
	}


	// for some reason macro functions'a from_files arent set, this is hack to make it now check it
	if (IS_FLAG_OFF(fdecl->flags, FUNC_DECL_MACRO))
	{
		auto fl_dbg = (file_dbg*)(data + file->files_sect + fdbg->file_idx);
		//std::string file_name = WasmInterpNameFromOffsetAndLen(data, file, &fl_dbg->name);
		fdecl->from_file = fl_dbg->fl;
		fl_dbg->fl->funcs_scp->vars.emplace_back(d);
		ASSERT(fdecl->from_file);
		winterp->funcs.emplace_back(fdecl);

		own_std::vector<int> idxs;
		TestFuncsWithSameName(&winterp->funcs, fdecl->name, &idxs);
		// functions with same name
		ASSERT(idxs.size() == 1);
	}
	fdecl->this_decl = d;
	fdecl->strct_constrct_at_offset = fdbg->strct_constr_offset;
	fdecl->strct_ret_size_per_statement_offset = fdbg->strct_ret_offset;
	fdecl->to_spill_offset = fdbg->to_spill_offset;
	fdecl->ir_stack_begin_idx = fdbg->ir_begin_stack_idx;
	fdecl->stack_size = fdbg->stack_size;
	if(IS_FLAG_ON(fdbg->flags, FUNC_DECL_X64))
		fdecl->code_start_idx = fdbg->x64_code_start;
	fdecl->for_interpreter_code_start_idx = fdbg->for_interpreter_x64_code_start;
	fdecl->bcs2_start = fdbg->bcs2_start;
	fdecl->bcs2_end = fdbg->bcs2_end;

	fdecl->ret_type.type = fdbg->ret_type;
	fdecl->ret_type.ptr = fdbg->ret_ptr_type;

	fdecl->flags = fdbg->flags;


	if (fdecl->ret_type.type == TYPE_STRUCT)
	{
		auto dstrct = (type_dbg*)(data + file->types_sect + fdbg->ret_type_offset);

		auto d = WasmInterpBuildStruct(data, 0, lang_stat, file, dstrct);
		fdecl->ret_type.strct = d->type.strct;
	}

	/*
	enum_type2 ret_type;
	char ret_ptr_type;
	int ret_type_offset;
	*/

	fdbg->created = true;
	fdbg->decl = d;


	return d;

}
void WasmInterpBuildVarsForScope(unsigned char* data, unsigned int len, lang_state* lang_stat, dbg_file_seriealize* file, scope_dbg *scp_pre, scope *scp_final)
{
	unsigned char* string_sect = data + file->string_sect;
	auto cur_var = (var_dbg*)(data + file->vars_sect + scp_pre->vars_offset);
	for (int i = 0; i < scp_pre->num_of_vars; i++)
	{
		type2 tp = {};
		std::string name = std::string((const char *)string_sect + cur_var->name.name_on_string_sect, cur_var->name.name_len);
		decl2* from_enum = nullptr;
		if (name == "main_type")
			int a = 0;
		switch (cur_var->type)
		{
		case TYPE_FUNC_PTR:
		case TYPE_FUNC_EXTERN:
		{
			auto type_strct = (func_dbg*)(data + file->func_sect + cur_var->type_idx);
			tp.fdecl = type_strct->decl->type.fdecl;
			/*
			int a = 0;
			auto fdbg = (func_dbg*)(data + file->func_sect + cur_var->type_idx);
			//WasmInterpBuildFunc(data, lang_stat->winterp, lang_stat, file, fdbg, scp_final);
			int a = 0;
			*/
		}break;
		case TYPE_IMPORT:
		{
			auto tp_dbg = (type_dbg*)(data + file->types_sect + cur_var->type_idx);
			auto fl_dbg = (file_dbg*)(data + file->files_sect + tp_dbg->imp.file_idx);
			//auto fl_dbg = (file_dbg *)(data + file->files_sect + tp_dbg->imp.file_idx)

			//std::string name = WasmInterpNameFromOffsetAndLen(data, file, &type_strct->name);
			type2 tp;
			tp.type = enum_type2::TYPE_IMPORT;

			std::string alias;
			if (tp_dbg->imp.alias.name_len > 0)
			{
				alias = std::string((const char*)string_sect + tp_dbg->imp.alias.name_on_string_sect, tp_dbg->imp.alias.name_len);
			}

			tp.imp = NewImport(lang_stat, tp_dbg->imp.imp_type, alias, fl_dbg->fl);
			tp.imp->alias = alias.substr();

			std::string name = std::string("imp_") + alias;

			scp_final->imports.emplace_back(NewDecl(lang_stat, name, tp));
		}break;
		case TYPE_STRUCT:
		case TYPE_STRUCT_TYPE:
		{
			auto type_strct = (type_dbg*)(data + file->types_sect + cur_var->type_idx);
			ASSERT(IS_FLAG_ON(type_strct->flags, TYPE_DBG_CREATED_TYPE_STRUCT));
			type_struct2 * strct = type_strct->strct;
			strct->size = type_strct->struct_size;
			//std::string name = WasmInterpNameFromOffsetAndLen(data, file, &type_strct->name);
			//decl2* d = FindIdentifier(name, scp_final, &tp);
			//ASSERT(d);
			tp.type = TYPE_STRUCT;
			tp.strct = strct;

		}break;
		case TYPE_STATIC_ARRAY:
		{
			tp.ar_size = cur_var->ar_size;

		}break;
		case TYPE_ENUM:
		{
			auto type_strct = (type_dbg*)(data + file->types_sect + cur_var->type_idx);
			from_enum = type_strct->e_decl;
		}break;
		case TYPE_AUTO:
		case TYPE_TEMPLATE:
		case TYPE_FUNC_TYPE:
		case TYPE_FUNC:
		case TYPE_BOOL:
		case TYPE_INT:
		case TYPE_F32_RAW:
		case TYPE_MACRO_EXPR:
		case TYPE_ENUM_IDX_32:
		case TYPE_ENUM_TYPE:
		case TYPE_STR_LIT:
		case TYPE_OVERLOADED_FUNCS:
		case TYPE_CHAR:
		{
			int a = 0;
		}break;
		case TYPE_F32:
		case TYPE_U8:
		case TYPE_S8:
		case TYPE_S32:
		case TYPE_U32:
		case TYPE_U16:
		case TYPE_S16:
		case TYPE_S64:
		case TYPE_VOID:
		case TYPE_U64:
		case TYPE_U32_TYPE:
		case TYPE_S32_TYPE:
		case TYPE_CHAR_TYPE:
		case TYPE_U8_TYPE:
		case TYPE_VECTOR_TYPE:
		case TYPE_VECTOR:
			break;
		default:
			ASSERT(0);
		}
		tp.type = cur_var->type;
		tp.ptr = cur_var->ptr;

		decl2 *d = FindIdentifier(name, scp_final, &tp);
		if (!d)
		{
			d = NewDecl(lang_stat, name, tp);
			d->offset = cur_var->offset;

			scp_final->AddDecl(d);
		}
		if (d->type.type == TYPE_STATIC_ARRAY)
		{
			auto type = (type_dbg*)(data + file->types_sect + cur_var->type_idx);
			auto ar_tp = (type2 *)AllocMiscData(lang_stat, sizeof(type2));
			ar_tp->type = type->type;
			ar_tp->ptr = type->ptr;
			if (ar_tp->type == TYPE_STRUCT)
			{
				auto strct_tp = (type_dbg*)(data + file->types_sect + type->struct_type_offset);
				ar_tp->strct = strct_tp->strct;
			}
			d->type.tp = ar_tp;
		}
		if (d->type.type == TYPE_ENUM)
		{
			ASSERT(from_enum);
			d->type.from_enum = from_enum;
		}
		d->flags = cur_var->flags;
		if (IS_FLAG_ON(d->flags, DECL_PTR_HAS_LEN))
		{
			std::string ptr_len_name = std::string((const char *)string_sect + cur_var->ptr_has_len_var_name.name_on_string_sect, cur_var->ptr_has_len_var_name.name_len);
			d->len_for_ptr_name = ptr_len_name.substr();
			ptr_decl_that_have_len ptr_decl;
			ptr_decl.decl = d;
			ptr_decl.scp = scp_final;
			lang_stat->ptrs_have_len.emplace_back(ptr_decl);
		}

		cur_var->decl = d;
		cur_var++;
	}

}
unit_file *WasmInterpSearchFile(lang_state* lang_stat, std::string *name)
{
	FOR_VEC(file, lang_stat->files)
	{
		if ((*file)->name == *name)
			return *file;
	}
	return nullptr;
}


scope *WasmInterpBuildScopes(wasm_interp *winterp, unsigned char* data, unsigned int len, lang_state* lang_stat, dbg_file_seriealize *file, scope *parent, scope_dbg *scp_pre, bool create_vars = false)
{
	scope* ret_scp = nullptr;
	if (scp_pre->created)
		ret_scp = scp_pre->scp;
	else
	{
		ret_scp = NewScope(lang_stat, parent);
		scp_pre->scp = ret_scp;
		scp_pre->created = true;
	}
	ret_scp->type = scp_pre->type;
	ret_scp->line_start = scp_pre->line_start;
	ret_scp->line_end = scp_pre->line_end;

	int child_offset = file->scopes_sect + scp_pre->children;
	auto cur_scp = (scope_dbg*)(data + child_offset);
	for (int i = 0; i < scp_pre->children_len; i++)
	{
		decl2* d = nullptr;
		if (cur_scp->type == SCP_TYPE_STRUCT)
		{
			auto dstrct = (type_dbg*)(data + file->types_sect + cur_scp->type_idx);

			d = WasmInterpBuildStruct(data, len, lang_stat, file, dstrct);

			//d->type.strct->scp = child_scp;


			//WasmInterpBuildVarsForScope(unsigned char* data, unsigned int len, lang_state* lang_stat, dbg_file* file, scope_dbg *scp_pre, scope *scp_final)
		}
		else if (cur_scp->type == SCP_TYPE_ENUM)
		{
			auto type = (type_dbg*)(data + file->types_sect + cur_scp->type_idx);
			if (IS_FLAG_ON(type->flags, TYPE_DBG_CREATED_TYPE_ENUM))
			{
				d = type->e_decl;
			}
			else
			{

				std::string name = WasmInterpNameFromOffsetAndLen(data, file, &type->name);
				type2 tp;
				tp.type = TYPE_ENUM_TYPE;
				d = NewDecl(lang_stat, name, tp);
				type->e_decl = d;
				type->flags = TYPE_DBG_CREATED_TYPE_ENUM;
			}

			///d->type.scp = chi



		}
		else if (cur_scp->type == SCP_TYPE_FUNC)
		{
			auto fdbg = (func_dbg *)(data + file->func_sect + cur_scp->func_idx);

			d = WasmInterpBuildFunc(data, winterp, lang_stat, file, fdbg);
			if(d->name == "main")
				int a = 0;

			//WasmInterpBuildVarsForScope(unsigned char* data, unsigned int len, lang_state* lang_stat, dbg_file* file, scope_dbg *scp_pre, scope *scp_final)
		}
		//scope* new_scp = NewScope(lang_stat, ret_scp);
		scope *child_scp = WasmInterpBuildScopes(winterp, data, len, lang_stat, file, ret_scp, cur_scp, create_vars);

		if (!create_vars)
		{
			if (cur_scp->type == SCP_TYPE_FUNC)
			{
				ret_scp->AddDecl(d);
				d->type.fdecl->scp = child_scp;

				child_scp->type = SCP_TYPE_FUNC;
				child_scp->fdecl = d->type.fdecl;
				auto tp_dbg = (func_dbg*)(data + file->func_sect + cur_scp->func_idx);
				tp_dbg->decl = d->type.fdecl->this_decl;

				//WasmInterpBuildVarsForScope(unsigned char* data, unsigned int len, lang_state* lang_stat, dbg_file* file, scope_dbg *scp_pre, scope *scp_final)
			}
			if (cur_scp->type == SCP_TYPE_ENUM)
			{
				ret_scp->AddDecl(d);
				child_scp->type = SCP_TYPE_ENUM;
				child_scp->e_decl = d;

				d->type.scp = child_scp;

			}
			if (cur_scp->type == SCP_TYPE_STRUCT)
			{
				//auto dstrct = (struct_dbg*)(data + file->types_sect + cur_scp->type_idx);

				//decl2 *d = WasmInterpBuildStruct(data, len, lang_stat, file, dstrct);
				child_scp->type = SCP_TYPE_STRUCT;
				child_scp->tstrct = d->type.strct;

				d->type.strct->scp = child_scp;

				ret_scp->AddDecl(d);

				//WasmInterpBuildVarsForScope(unsigned char* data, unsigned int len, lang_state* lang_stat, dbg_file* file, scope_dbg *scp_pre, scope *scp_final)
			}
		}
		cur_scp++;
	}
	if (create_vars)
	{
		WasmInterpBuildVarsForScope(data, len, lang_stat, file, scp_pre, ret_scp);
		if (ret_scp->type == SCP_TYPE_ENUM)
		{
			decl2* d = ret_scp->e_decl;
			d->type.enum_names = (own_std::vector<char*> *)AllocMiscData(lang_stat, sizeof(own_std::vector<int>));
			auto ar = d->type.enum_names;

			FOR_VEC(d_ptr, ret_scp->vars)
			{
				decl2* de = *d_ptr;
				ar->emplace_back((char *)de->name.c_str());
			}
		}
	}

	if (ret_scp->type == SCP_TYPE_FILE)
	{
		auto fl = (file_dbg*)(data + file->files_sect + scp_pre->file_idx);
		fl->fl->global = ret_scp;
	}


	return ret_scp;
}

inline bool GetWasmBcsBlockJmpsTo(own_std::vector<wasm_bc> *ar)
{
	auto cur = NewBlock(nullptr);
	FOR_VEC(bc, *ar)
	{
		if (bc->type == WASM_INST_BLOCK)
		{
			cur = NewBlock(cur);

			//cur->
		}
		if (bc->type == WASM_INST_END)
		{
			cur = cur->parent;
		}
	}

}


bool WasmBcLogic(wasm_interp* winterp, dbg_state& dbg, wasm_bc** cur_bc, unsigned char* mem_buffer, block_linked** cur, bool &can_break)
{
	own_std::vector<wasm_stack_val> &wasm_stack = dbg.wasm_stack;
	wasm_stack_val val = {};
	switch ((*cur_bc)->type)
	{
	case WASM_INST_NOP:
	{
		//WasmPrintVars(&dbg);
		int a = 0;
	}break;
	case WASM_INST_BREAK:
	{
		int i = 0;
		wasm_bc *label;
		/*
		while (i < (*cur_bc)->i)
		{
			(*cur) = (*cur)->parent;
			FreeBlock((*cur));
			i++;
		}
		*/
		if ((*cur_bc)->jmps_to->type == WASM_INST_LOOP)
		{
			wasm_bc* prev_bc = (*cur_bc);
			(*cur_bc) = prev_bc->jmps_to + 1;
			if (prev_bc->break_on_first_loop_bc || dbg.break_type == DBG_BREAK_ON_DIFF_STAT_BUT_SAME_FUNC && dbg.next_stat_break_func == dbg.cur_func)
			{
				(*cur_bc)->one_time_dbg_brk = true;
				prev_bc->break_on_first_loop_bc = false;
			}
			return true;
		}

		(*cur_bc) = (*cur_bc)->jmps_to;
		return true;
		
	}break;
	case WASM_INST_BREAK_IF:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();

		if (top.s32 != 1)
			break;
		wasm_bc *label;
		int i = 0;
		/*
		while (i < (*cur_bc)->i)
		{
			(*cur) = (*cur)->parent;
			FreeBlock((*cur));
			i++;
		}
		*/
		wasm_bc* prev_bc = (*cur_bc);
		(*cur_bc) = prev_bc->jmps_to;
		if (prev_bc->break_on_first_loop_bc || dbg.break_type == DBG_BREAK_ON_DIFF_STAT_BUT_SAME_FUNC && dbg.next_stat_break_func == dbg.cur_func)
		{
			((*cur_bc) + 1)->one_time_dbg_brk = true;
			prev_bc->break_on_first_loop_bc = false;
		}
		return true;
	}break;
	case WASM_INST_INDIRECT_CALL:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		func_decl* call_f = dbg.wasm_state->funcs[top.u32];
		WasmDoCallInstruction(&dbg, &(*cur_bc), &(*cur), call_f);
	}break;
	case WASM_INST_CALL:
	{
		func_decl* call_f = dbg.wasm_state->funcs[(*cur_bc)->i];
		if (IS_FLAG_ON(call_f->flags, FUNC_DECL_IS_OUTSIDER))
		{
			if (winterp->outsiders.find(call_f->name) != winterp->outsiders.end())
			{
				OutsiderFuncType func_ptr = winterp->outsiders[call_f->name];
				func_ptr(&dbg);
			}
			else
				ASSERT(0);

		}
		else if (IS_FLAG_ON(call_f->flags, FUNC_DECL_X64))
		{
			auto stack_reg = (u64 *)&dbg.mem_buffer[STACK_PTR_REG * 8];
			WasmCallX64(winterp, dbg, mem_buffer, call_f, *stack_reg);
		}
		else
		{
			WasmDoCallInstruction(&dbg, &(*cur_bc), &(*cur), call_f);
		}

		// assert is 32bit
		int a = 0;
	}break;
	case WASM_INST_RET:
	{
		dbg.func_stack.pop_back();
		if (dbg.func_stack.size() == 0)
		{
			can_break = true;
			break;
		}
		while ((*cur))
		{
			FreeBlock((*cur));
			(*cur) = (*cur)->parent;

		}
		func_decl* prev_func = dbg.cur_func;
		dbg.cur_func = dbg.func_stack.back();

		if (prev_func == dbg.next_stat_break_func)
		{
			dbg.next_stat_break_func = dbg.cur_func;
		}
		
		(*cur_bc) = dbg.return_stack.back();
		dbg.return_stack.pop_back();

		(*cur) = dbg.block_stack.back();
		dbg.block_stack.pop_back();
	}break;
	case WASM_INST_I32_EQ:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->s32 = (int) (penultimate->u32 == top.u32);
		int a = 0;
	}break;
	case WASM_INST_I32_GE_U:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->s32 = (int)(penultimate->u32 >= top.u32);
		int a = 0;
	}break;
	case WASM_INST_I32_GT_U:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->u32 = (int)(penultimate->u32 > top.u32);
		int a = 0;
	}break;
	case WASM_INST_I32_NE:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->u32 = (int)(penultimate->u32 != top.u32);
		int a = 0;
	}break;
	case WASM_INST_I32_LE_S:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->u32 = (int)(penultimate->s32 <= top.s32);
		int a = 0;
	}break;
	case WASM_INST_I32_LT_S:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->u32 = (int)(penultimate->s32 < top.s32);
		int a = 0;
	}break;
	case WASM_INST_I32_GT_S:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->u32 = (int)(penultimate->s32 > top.s32);
		int a = 0;
	}break;
	case WASM_INST_I32_GE_S:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->u32 = (int)(penultimate->s32 >= top.s32);
		int a = 0;
	}break;
	case WASM_INST_I32_LE_U:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->u32 = (int)(penultimate->u32 <= top.u32);
		int a = 0;
	}break;
	case WASM_INST_I32_LT_U:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->u32 = (int)(penultimate->u32 < top.u32);
		int a = 0;
	}break;
	case WASM_INST_I32_REMAINDER_S:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->u32 = (penultimate->s32 % top.s32);
		int a = 0;
	}break;
	case WASM_INST_I32_REMAINDER_U:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->u32 = (penultimate->u32 % top.u32);
		int a = 0;
	}break;
	case WASM_INST_I32_DIV_S:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->s32 = (penultimate->s32 / top.s32);
		int a = 0;
	}break;
	case WASM_INST_I32_DIV_U:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->u32 = (penultimate->u32 / top.u32);
		int a = 0;
	}break;
	case WASM_INST_I32_STORE8:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();

		auto penultimate = wasm_stack.back();
		wasm_stack.pop_back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate.type == 0);
		ASSERT(penultimate.u32 < dbg.mem_size);
		*(char*)&mem_buffer[penultimate.u32] = top.s32;
		int a = 0;
	}break;
	case WASM_INST_F32_STORE:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();

		auto penultimate = wasm_stack.back();
		wasm_stack.pop_back();
		// assert is 32bit
		ASSERT(top.type == WSTACK_VAL_F32 && penultimate.type == 0);
		ASSERT(penultimate.u32 < dbg.mem_size);
		*(float*)&mem_buffer[penultimate.u32] = top.f32;
		int a = 0;
	}break;
	case WASM_INST_I64_STORE:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();

		auto penultimate = wasm_stack.back();
		wasm_stack.pop_back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate.type == 0);
		ASSERT(penultimate.u32 < dbg.mem_size);
		*(long long*)&mem_buffer[penultimate.u32] = top.s64;
		int a = 0;
	}break;
	case WASM_INST_I32_STORE:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();

		auto penultimate = wasm_stack.back();
		wasm_stack.pop_back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate.type == 0);
		ASSERT(penultimate.u32 < dbg.mem_size);
		*(int*)&mem_buffer[penultimate.u32] = top.s32;
		int a = 0;
	}break;
	case WASM_INST_F32_CONST:
	{
		auto top = wasm_stack.back();
		val.type = WSTACK_VAL_F32;
		val.f32 = (*cur_bc)->f32;
		wasm_stack.emplace_back(val);
	}break;
	case WASM_INST_F32_LOAD:
	{
		auto w = &wasm_stack.back();
		// assert is 32bit
		ASSERT(w->type == WSTACK_VAL_INT);
		w->f32 = *(float*)&mem_buffer[w->s32];
		w->type = WSTACK_VAL_F32;
		int a = 0;
	}break;
	case WASM_INST_F32_GE:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == wstack_val_type::WSTACK_VAL_F32 && penultimate->type == wstack_val_type::WSTACK_VAL_F32)
		penultimate->u32 = (int)(penultimate->f32 >= top.f32);
		int a = 0;
	}break;
	case WASM_INST_F32_DIV:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == wstack_val_type::WSTACK_VAL_F32 && penultimate->type == wstack_val_type::WSTACK_VAL_F32)
		penultimate->f32 = (penultimate->f32 / top.f32);
		int a = 0;
	}break;
	case WASM_INST_F32_NE:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == wstack_val_type::WSTACK_VAL_F32 && penultimate->type == wstack_val_type::WSTACK_VAL_F32)
		penultimate->u32 = (int)(penultimate->f32 != top.f32);
		int a = 0;
	}break;
	case WASM_INST_F32_EQ:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == wstack_val_type::WSTACK_VAL_F32 && penultimate->type == wstack_val_type::WSTACK_VAL_F32)
		penultimate->u32 = (int)(penultimate->f32 == top.f32);
		int a = 0;
	}break;
	case WASM_INST_F32_LE:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == wstack_val_type::WSTACK_VAL_F32 && penultimate->type == wstack_val_type::WSTACK_VAL_F32)
		penultimate->u32 = (int)(penultimate->f32 <= top.f32);
		int a = 0;
	}break;
	case WASM_INST_F32_GT:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == wstack_val_type::WSTACK_VAL_F32 && penultimate->type == wstack_val_type::WSTACK_VAL_F32)
		penultimate->u32 = (int)(penultimate->f32 > top.f32);
		int a = 0;
	}break;
	case WASM_INST_F32_LT:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == wstack_val_type::WSTACK_VAL_F32 && penultimate->type == wstack_val_type::WSTACK_VAL_F32)
		penultimate->type = WSTACK_VAL_INT;
		penultimate->u32 = (int)(penultimate->f32 < top.f32);
		int a = 0;
	}break;
	case WASM_INST_I32_LOAD_8_S:
	{
		auto w = &wasm_stack.back();
		// assert is 32bit
		ASSERT(w->type == WSTACK_VAL_INT);
		w->s32 = *(char*)&mem_buffer[w->s32];
		int a = 0;
	}break;
	case WASM_INST_I64_LOAD:
	{
		auto w = &wasm_stack.back();
		// assert is 32bit
		ASSERT(w->type == WSTACK_VAL_INT);
		w->s64 = *(long long*)&mem_buffer[w->u32];
		int a = 0;
	}break;
	case WASM_INST_I32_LOAD:
	{
		auto w = &wasm_stack.back();
		// assert is 32bit
		ASSERT(w->type == WSTACK_VAL_INT);
		w->s32 = *(int*)&mem_buffer[w->s32];
		int a = 0;
	}break;
	case WASM_INST_I32_CONST:
	{
		val.type = WSTACK_VAL_INT;
		val.s32 = (*cur_bc)->i;
		wasm_stack.emplace_back(val);
	}break;
	case WASM_INST_LOOP:
	case WASM_INST_BLOCK:
	{
		//(*cur) = NewBlock((*cur));
		//(*cur)->wbc = (*cur_bc);
	}break;
	case WASM_INST_I32_OR:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->s32 |= top.s32;
	}break;
	case WASM_INST_I32_MUL:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->s32 *= top.s32;
	}break;
	case WASM_INST_CAST_F32_2_S32:
	{
		auto top = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top->type == WSTACK_VAL_F32)
		top->s32 = top->f32;
		top->type = WSTACK_VAL_INT;
	}break;
	case WASM_INST_CAST_S64_2_F32:
	{
		auto top = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top->type == WSTACK_VAL_INT)
		top->f32 = top->s64;
		top->type = WSTACK_VAL_F32;
	}break;
	case WASM_INST_CAST_S32_2_F32:
	{
		auto top = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top->type == WSTACK_VAL_INT)
		top->f32 = top->s32;
		top->type = WSTACK_VAL_F32;
	}break;
	case WASM_INST_CAST_U32_2_F32:
	{
		auto top = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top->type == WSTACK_VAL_INT)
		top->f32 = top->u32;
		top->type = WSTACK_VAL_F32;
	}break;
	case WASM_INST_F32_MUL:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == WSTACK_VAL_F32 && penultimate->type == WSTACK_VAL_F32);
		penultimate->f32 *= top.f32;
	}break;
	case WASM_INST_F32_SUB:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == WSTACK_VAL_F32 && penultimate->type == WSTACK_VAL_F32);
		penultimate->f32 -= top.f32;
	}break;
	case WASM_INST_F32_ADD:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == WSTACK_VAL_F32 && penultimate->type == WSTACK_VAL_F32);
		penultimate->f32 += top.f32;
	}break;
	case WASM_INST_DBG_BREAK:
	{
		printf(ANSI_RED "debug break hit\n" ANSI_RESET);
		dbg.break_type = DBG_BREAK_ON_NEXT_BC;
		return true;

	}break;
	case WASM_INST_I32_ADD:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);

		penultimate->s32 += top.s32;
	}break;
	case WASM_INST_I32_AND:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->s32 &= top.s32;
	}break;
	case WASM_INST_I32_SUB:
	{
		auto top = wasm_stack.back();
		wasm_stack.pop_back();
		auto penultimate = &wasm_stack.back();
		// assert is 32bit
		ASSERT(top.type == 0 && penultimate->type == 0);
		penultimate->s32 -= top.s32;
	}break;
	case WASM_INST_END:
	{
		/*
		if ((*cur))
		{
			FreeBlock((*cur));
			(*cur) = (*cur)->parent;
		}
		*/

	}break;
	default:
		ASSERT(0)
	}

	return false;
}
void OpenWindow(dbg_state* dbg);
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void ExitDebugger(scope **cur_scp, wasm_bc **bc)
{
	*cur_scp = nullptr;
	wasm_bc* prev_bc = *bc;
	prev_bc->one_time_dbg_brk = false;
	if (prev_bc->from_engine_break)
		prev_bc->dbg_brk = false;
}



void ImGuiDrawSquare(dbg_state &dbg, int addr, ImU32 col_a)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	auto sqr1_left_bottom = (v3 *)&dbg.mem_buffer[addr];
	auto sqr1_left_top = sqr1_left_bottom + 1;
	auto sqr1_right_top = sqr1_left_bottom + 2;
	auto sqr1_right_bottom = sqr1_left_bottom + 3;

	auto s1_lb_2_lt = *sqr1_left_top - *sqr1_left_bottom;
	auto s1_lt_2_rt = *sqr1_right_top - *sqr1_left_top;
	auto s1_rt_2_rb = *sqr1_right_bottom - *sqr1_right_top;
	auto s1_rb_2_lb = *sqr1_left_bottom - *sqr1_right_bottom;

	float mul = 200;
	ImVec2 im_s1_lb(100, 100);
	ImVec2 im_s1_lt(100 + s1_lb_2_lt.x * mul, 100 + s1_lb_2_lt.y * mul);
	ImVec2 im_s1_rt(im_s1_lt.x + s1_lt_2_rt.x * mul, im_s1_lt.y + s1_lt_2_rt.y * mul);
	ImVec2 im_s1_rb(im_s1_rt.x + s1_rt_2_rb.x * mul, im_s1_rt.y + s1_rt_2_rb.y * mul);

	draw_list->AddLine(im_s1_lb, im_s1_lt, col_a, 1.0);
	draw_list->AddLine(im_s1_lt, im_s1_rt, col_a, 1.0);
	draw_list->AddLine(im_s1_rt, im_s1_rb, col_a, 1.0);
	draw_list->AddLine(im_s1_rb, im_s1_lb, col_a, 1.0);

}
void CheckPipeAndGetString(HANDLE pipe, std::string &final_str)
{
	DWORD bytesRead;
	DWORD availableBytes = 0;

	char read_buffer[1024];
	int res = PeekNamedPipe(pipe, NULL, 0, NULL, &availableBytes, NULL);
	if (res == 0)
	{
		int code = GetLastError();
		printf("error pipe peek %d\n", code);
		FlushFileBuffers(GetStdHandle(STD_OUTPUT_HANDLE));
	}
	if (availableBytes > 0)
	{
		int cur_read = 0;
		while (cur_read < availableBytes)
		{
			ReadFile(pipe, read_buffer, sizeof(read_buffer) - 1, &bytesRead, NULL);
			read_buffer[bytesRead] = 0;
			cur_read += bytesRead;
			final_str += std::string(read_buffer, bytesRead);
		}
	}
}
void Write(HANDLE hFile, char *str, int sz)
{
	int dwBytesWritten = 0;
	auto bErrorFlag = WriteFile(
		hFile,           // open file handle
		str,      // start of data to write
		sz,  // number of bytes to write
		(LPDWORD)&dwBytesWritten, // number of bytes that were written
		NULL);           // no overlapped structure

	/*
	if (FALSE == bErrorFlag) {
		//DisplayError(TEXT("WriteFile"));
		printf("Terminal failure: Unable to write to file.\n");
	} 
	*/
}

tkn_type2 GetOpBasedOnInst(byte_code_enum type)
{
	INSTS_SWITCH(type, return T_PLUS, return T_MINUS, return T_MINUS, return T_EQUAL, return T_EQUAL, return T_PIPE, return T_AMPERSAND, return T_PERCENT, return T_MUL, return T_DIV);
}
void DoOperationOnPtrFloat(char* ptr, char sz, float rhs, tkn_type2 op)
{
	switch (sz)
	{
	case 2:
		*(float *)ptr = GetExpressionValT<float>(op, *(float *)ptr, rhs);
	break;
	default:
		*(float *)ptr = GetExpressionValT<float>(op, *(float *)ptr, rhs);
	}
}
void DoOperationOnPtrUnsigned(char* ptr, char sz, u64 val, tkn_type2 op)
{
	switch (sz)
	{
	case 0:
		*ptr = GetExpressionValT<u8>(op, *ptr, val);
		break;
	case 1:
		*(unsigned short *)ptr = GetExpressionValT<unsigned short>(op, *(short *)ptr, val);
		break;
	case 2:
		*(u32 *)ptr = GetExpressionValT<u32>(op, *(u32 *)ptr, val);
		break;
	case 3:
		*(u64 *)ptr = GetExpressionValT<u64>(op, *(u64 *)ptr, val);
		break;
	default:
		ASSERT(0);
	}
}
void DoOperationOnPtr(char* ptr, char sz, u64 val, tkn_type2 op)
{
	switch (sz)
	{
	case 0:
		*ptr = GetExpressionValT<u8>(op, *ptr, val);
		break;
	case 1:
		*(short *)ptr = GetExpressionValT<short>(op, *(short *)ptr, val);
		break;
	case 2:
		*(int *)ptr = GetExpressionValT<int>(op, *(int *)ptr, val);
		break;
	case 3:
		*(long long *)ptr = GetExpressionValT<long long>(op, *(long long *)ptr, val);
		break;
	default:
		ASSERT(0);
	}
}

#define REG_PARAM_START 0
#define EFLAGS_REG 28
#define EFLAGS_ZERO 1
#define EFLAGS_ABOVE 2
#define EFLAGS_BELOW 4
u64 *GetFloatRegValPtr(dbg_state *dbg, short reg)
{
	return (u64*)&dbg->mem_buffer[reg * FLOAT_REG_SIZE_BYTES];
}
u64 *GetRegValPtr(dbg_state *dbg, short reg)
{
	return (u64*)&dbg->mem_buffer[reg * 8];
}
u64 *GetMemValPtr(dbg_state *dbg, short reg, int offset)
{
	auto reg_src_ptr = (u64*)&dbg->mem_buffer[reg * 8];
	u64 offset_ = *reg_src_ptr + offset;
	return (u64*)&dbg->mem_buffer[offset_];
}
char * GetRegStr(short reg, char sz, char *buffer)
{
	//std::string ret = "r" + std::to_string(reg);
	char sz_char;
	switch (sz)
	{
	case 0:
		sz_char = 'b';
		break;
	case 1:
		sz_char = 'w';
		break;
	case 2:
		sz_char = 'd';
		break;
	case 3:
		sz_char = 'q';
		break;
	}
	sprintf(buffer, "r%c%d", sz_char, reg);
	return buffer;
}

char *SizeToStr(char sz)
{
	switch (sz)
	{
	case 0:
		return "byte";
	case 1:
		return "word";
	case 2:
		return "dword";
	case 3:
		return "qword";
	default:
		return "unknown size";
	}
	return nullptr;
}
char *InstToStr(byte_code_enum type)
{
	INSTS_SWITCH(type, return "add", return "sub", return "cmp", return "mov", return "lea", return "or", return "and", return "mod", return "mul", return "div");
}
void Bc2ToString(dbg_state *dbg, byte_code2* bc, char *buffer, int buffer_size)
{
	short reg_src = (bc->regs >> RHS_REG_BIT) & 0x3f;
	short reg_dst = bc->regs & 0x3f;
	int imm = bc->i;
	int mem_offset = bc->mem_offset;
	char sz = bc->regs >> REG_SZ_BIT;
	char* sz_str = SizeToStr(sz);

	char* reg_src_str = GetRegStr(reg_src, sz, buffer + 64);
	char* reg_dst_str = GetRegStr(reg_dst, sz, buffer + 128);
	char* inst_name;
	switch (bc->bc_type)
	{
	case RELOC:
	{
		switch (bc->rel_type)
		{
		case REL_DATA:
		{
			snprintf(buffer, 128, "rel_data %s, %d",reg_dst_str, imm);
			
		}break;
		default:
			ASSERT(0);
		}
	}break;;
	case BEGIN_FUNC:
	{
		sprintf(buffer, "%s", "begin func");
		//ret = "begin func";
	}break;;
	case BEGIN_FUNC_FOR_INTERPRETER:
	{
	}break;
	case CVTSD_REG_2_SS:
	{
		snprintf(buffer, 128, "cvtsd xmm%d, %s", reg_dst, reg_src_str);
		
	}break;
	case MOVZX_M:
	{
		char lhs_sz = bc->rhs_and_lhs_reg_sz & 0xf;
		char rhs_sz = bc->rhs_and_lhs_reg_sz >> 4;

		reg_dst_str = GetRegStr(reg_dst, lhs_sz, buffer + 64);
		reg_src_str = GetRegStr(reg_src, rhs_sz, buffer + 128);
		snprintf(buffer, 128, "movzx %s, %s[%s + %d]", reg_dst_str, sz_str, reg_src_str, mem_offset);
		
	}break;
	case MOVZX_R:
	{
		char lhs_sz = bc->rhs_and_lhs_reg_sz & 0xf;
		char rhs_sz = bc->rhs_and_lhs_reg_sz >> 4;

		reg_dst_str = GetRegStr(reg_dst, lhs_sz, buffer + 64);
		reg_src_str = GetRegStr(reg_src, rhs_sz, buffer + 128);
		snprintf(buffer, 128, "movzx %s, %s", reg_dst_str, reg_src_str);
		
	}break;
	case ADD_R_2_R:
	case AND_R_2_R:
	case CMP_R_2_R:
	case MOD_R_2_R:
	case OR_R_2_R:
	case DIV_R_2_R:
	case MUL_R_2_R:
	case MOV_R:
	case SUB_R_2_R:
	{
		inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s %s, %s", inst_name, reg_dst_str, reg_src_str);
		
	}break;
	case ADD_I_2_R:
	case OR_I_2_R:
	case CMP_I_2_R:
	case AND_I_2_R:
	case SUB_I_2_R:
	case MOD_I_2_R:
	case MUL_I_2_R:
	case DIV_I_2_R:
	case MOV_I:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s %s, %d", inst_name, reg_dst_str, imm);
		
	}break;
	case RET:
	{
		sprintf(buffer, "ret");
	}break;
	case INT3:
	{
		sprintf(buffer, "int3");
	}break;
	case JMP:
	{
		snprintf(buffer, 128, "jmp %d", imm);
		
	}break;
	case INST_CALL:
	{
		snprintf(buffer, 128, "call %d", imm);
		
	}break;
	case JMP_E:
	{
		snprintf(buffer, 128, "je %d", imm);
		
	}break;
	case JMP_L:
	{
		snprintf(buffer, 128, "jl %d", imm);
		
	}break;
	case JMP_LE:
	{
		snprintf(buffer, 128, "jle %d", imm);
		
	}break;
	case INST_CALL_OUTSIDER:
	{
		func_decl* f = dbg->wasm_state->funcs[imm];
		snprintf(buffer, 128, "call_outsider %s", f->name.c_str());
		
	}break;
	case JMP_G:
	{
		snprintf(buffer, 128, "jg %d", imm);
		
	}break;
	case JMP_NE:
	{
		snprintf(buffer, 128, "jne %d", imm);
		
	}break;
	case JMP_AE:
	{
		snprintf(buffer, 128, "jae %d", imm);
		
	}break;
	case JMP_A:
	{
		snprintf(buffer, 128, "ja %d", imm);
		
	}break;
	case JMP_B:
	{
		snprintf(buffer, 128, "jb %d", imm);
		
	}break;
	case JMP_BE:
	{
		snprintf(buffer, 128, "jbe %d", imm);
		
	}break;
	case JMP_GE:
	{
		snprintf(buffer, 128, "jge %d", imm);
		
	}break;
	case MOV_I_2_REG_PARAM:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s_i_param %d, %d", inst_name, reg_dst, imm);
		
	}break;
	case MOV_R_2_REG_PARAM:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s_param %d, %s", inst_name, reg_dst, reg_src_str);
		
	}break;
	case CVTSD_MEM_2_SS:
	{
		// inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "cvtsd xmm%d, %s[%s + %d]", reg_dst, sz_str, reg_src_str, mem_offset);
		
	}break;
	case MOV_M_2_REG_PARAM:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s_param %d, %s[%s + %d]", inst_name, reg_dst, sz_str, reg_src_str, mem_offset);
		
	}break;
	case INST_CALL_REG:
	{
		snprintf(buffer, 128, "call_reg %s", reg_dst_str);
		
	}break;
	case ADD_M_2_R:
	case SUB_M_2_R:
	case CMP_M_2_R:
	case AND_M_2_R:
	case OR_M_2_R:
	case MOD_M_2_R:
	case DIV_M_2_R:
	case MUL_M_2_R:
	case INST_LEA:
	case MOV_M:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s %s, %s[%s + %d]", inst_name, reg_dst_str, sz_str, reg_src_str, mem_offset);
		
	}break;
	case MOV_SSE_2_REG_PARAM:
	{
		snprintf(buffer, 128, "mov param %d,  xmm%d", reg_dst, reg_src);
		
	}break;
	case MOV_PCKD_SSE_2_PCKD_SSE:
	case ADD_PCKD_SSE_2_PCKD_SSE:
	case MUL_PCKD_SSE_2_PCKD_SSE:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s pxmm%d,  pxmm%d", inst_name, reg_dst, reg_src);
		
	}break;
	case MOV_SSE_2_SSE:
	case CMP_SSE_2_SSE:
	case ADD_SSE_2_SSE:
	case SUB_SSE_2_SSE:
	case MUL_SSE_2_SSE:
	case DIV_SSE_2_SSE:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s xmm%d,  xmm%d", inst_name, reg_dst, reg_src);
		
	}break;
	case SQRT_SSE:
	{
		snprintf(buffer, 128, "sqrt xmm%d,  xmm%d", reg_dst, reg_src);
		
	}break;
	case MOV_M_2_PCKD_SSE:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s pxmm%d,  %s[%s + %d]", inst_name, reg_dst, sz_str, reg_src_str, mem_offset);
		
	}break;
	case ADD_MEM_2_SSE:
	case SUB_MEM_2_SSE:
	case DIV_MEM_2_SSE:
	case MUL_MEM_2_SSE:
	case CMP_MEM_2_SSE:
	case MOV_M_2_SSE:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s xmm%d,  %s[%s + %d]", inst_name, reg_dst, sz_str, reg_src_str, mem_offset);
		
	}break;
	case MOV_SSE_2_R:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s %s, xmm%d", inst_name, reg_dst_str, reg_src);
		
	}break;
	case MOV_SSE_2_MEM:
	case CMP_SSE_2_MEM:
	case ADD_SSE_2_MEM:
	case SUB_SSE_2_MEM:
	case MUL_SSE_2_MEM:
	case DIV_SSE_2_MEM:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s %s[%s + %d], xmm%d", inst_name, sz_str, reg_dst_str, mem_offset, reg_src);
		
	}break;
	case MOV_PCKD_SSE_2_M:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "mov%s[%s + %d], pxmm%d", sz_str, reg_dst_str, mem_offset, reg_src);
		
	}break;
	case CMP_R_2_M:
	case ADD_R_2_M:
	case SUB_R_2_M:
	case AND_R_2_M:
	case MUL_R_2_M:
	case DIV_R_2_M:
	case STORE_R_2_M:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s %s[%s + %d], %s", inst_name, sz_str, reg_dst_str, mem_offset, reg_src_str);
		
	}break;
	case CMP_I_2_M:
	case STORE_I_2_M:
	{
		 inst_name = InstToStr(bc->bc_type);
		snprintf(buffer, 128, "%s %s[%s + %d], %d", inst_name, sz_str, reg_dst_str, mem_offset, imm);
		
	}break;
	case MOV_F_2_PCKED_SSE:
	{
		snprintf(buffer, 128, "mov pxmm%d, %.3f", reg_dst, bc->f32);
		
	}break;
	case MOV_F_2_SSE:
	{
		snprintf(buffer, 128, "mov xmm%d, %.3f", reg_dst, bc->f32);
		
	}break;
	case NOP:
	{
		sprintf(buffer, "nop");
	}break;
	case STORE_REG_PARAM:
	{
		sprintf(buffer, "store reg param");
	}break;
	default:
		ASSERT(0);
	}
}
inline void CheckValAssignFlags(u64* eflags, u64 val, u64 sign_flag)
{
	if (val == 0)
	{
		*eflags = EFLAGS_ZERO;
	}
	else
	{
		bool below = ((val & sign_flag) != 0);
		*eflags = below * EFLAGS_BELOW;
		*eflags |= !below * EFLAGS_ABOVE;
	}
}
inline void DoCmpInstFloat(float lhs, float rhs, u64* eflags)
{
	float val = lhs - rhs;
	int sign_flag = 1 << 31;
	if (val == 0)
	{
		*eflags = EFLAGS_ZERO;
	}
	else
	{
		bool below = ((*(int *)&val & sign_flag) != 0);
		*eflags = below * EFLAGS_BELOW;
		*eflags |= !below * EFLAGS_ABOVE;
	}
}
inline void DoCmpInst(void* dst, u64 in_val, u64* eflags, char sz)
{
	switch (sz)
	{
	case 0:
	{
		auto val = (*(u8*)dst) - (char)in_val;
		CheckValAssignFlags(eflags, val, 1<<7);
	}break;
	case 1:
	{
		auto val = (*(unsigned short*)dst) - (short)in_val;
		CheckValAssignFlags(eflags, val, 1<<15);
	}break;
	case 2:
	{
		auto val = (*(unsigned int*)dst) - (int)in_val;
		CheckValAssignFlags(eflags, val, 1<<31);
	}break;
	case 3:
	{
		auto val = (*(unsigned long long*)dst) - in_val;
		CheckValAssignFlags(eflags, val, (long long)1<<63);
	}break;
	}
}

inline void DoMovZXInts(dbg_state* dbg, u64* dst_ptr, u64* src_ptr, int sz)
{
	switch (sz)
	{
		/*
	// weird qword to qword, but necessary for the moment
	// because the way casting works with point is somewhat hacky
	// the code that generates this cast probably is at IR.cpp CAST: case in GinIRFromStack func
	case 0x33:
	{
		*dst_ptr = *(long long*)src_ptr;
	}break;
	*/
	// dword to qword
	case 0x23:
	{
		*dst_ptr = *(int*)src_ptr;
	}break;
	// byte to dword
	case 0x2:
	{
		*(int *)dst_ptr = *(char *)src_ptr;
	}break;
	// dword to byte
	case 0x20:
	{
		*(int *)dst_ptr = *(char *)src_ptr;
	}break;
	// qword to byte
	case 0x30:
	{
		*dst_ptr = *(char *)src_ptr;
	}break;
	// qword to dword
	case 0x32:
	{
		*(int *)dst_ptr = *(int*)src_ptr;
	}break;
	default:
		ASSERT(0);
	}
}

func_decl *GetFuncBasedOnBc2(dbg_state *dbg, byte_code2 *bc)
{

	FOR_VEC(it, dbg->wasm_state->funcs)
	{
		auto f = *it;
		byte_code2* f_start = dbg->lang_stat->bcs2_start + f->bcs2_start;
		byte_code2* f_end = dbg->lang_stat->bcs2_start + f->bcs2_end;
		if (bc >= f_start && bc < f_end)
		{
			return f;
		}
	}
	return nullptr;
}
void MakeCurBcToBeBreakpoint(dbg_state* dbg, byte_code2* bc, int line, bool one_time = true)
{
	breakpoint bp;
	bp.line = line;
	bp.prev_inst = bc->bc_type;
	bp.bc = bc;
	bp.one_time_bp = one_time;
	dbg->breakpoints.emplace_back(bp);

	bc->type = INT3;
}
#pragma optimize("", off)
void Bc2CallX64(dbg_state* dbg, byte_code2** ptr, func_decl *call_f)
{
	auto stack_reg = (u64 *)&dbg->mem_buffer[PRE_X64_RSP_REG * 8];
	*stack_reg -= 8;
	WasmCallX64(dbg->wasm_state, *dbg, (u8 *)dbg->mem_buffer, call_f, *stack_reg);
	*stack_reg += 8;
	auto reg_src_ptr = (u64*)&dbg->mem_buffer[RET_1_REG * 8];
	if (call_f->ret_type.IsFloat())
	{
		auto reg_dst_ptr = (float*)&dbg->mem_buffer[FLOAT_REG_0 * FLOAT_REG_SIZE_BYTES];
		*reg_dst_ptr = *(float*)reg_src_ptr;
	}
	else
	{
		auto reg_dst_ptr = (u64*)&dbg->mem_buffer[0];
		*reg_dst_ptr = *reg_src_ptr;
	}

}
#pragma optimize("", on)
void Bc2Logic(dbg_state* dbg, byte_code2 **ptr, bool *inc_ptr, bool *valid, int offset)
{
	byte_code2* bc = *ptr;;
	if (bc < dbg->lang_stat->bcs2_start || bc > dbg->lang_stat->bcs2_end)
	{
		*inc_ptr = false;
		if (dbg->break_type != DBG_BREAK_RIP_CORRUPTED)
		{
			func_decl* f = GetFuncBasedOnBc2(dbg, dbg->prev_valid_bc);
			int offset = dbg->prev_valid_bc - dbg->lang_stat->bcs2_start;
			stmnt_dbg * st = GetStmntBasedOnOffset(&f->wasm_stmnts, offset);
			if (!st)
			{
				st = &f->wasm_stmnts.back();
				dbg->prev_valid_bc = dbg->lang_stat->bcs2_start + st->start;
			}
			MakeCurBcToBeBreakpoint(dbg, dbg->prev_valid_bc, st->line);
			dbg->break_type = DBG_BREAK_RIP_CORRUPTED;
			*valid = false;
		}
		return;
	}
	
	/*
	stmnt_dbg* cur_st;
	func_decl *cur_func = GetFuncBasedOnBc2(dbg, bc);
	if (cur_func)
	{
		cur_st = GetStmntBasedOnOffset(&cur_func->wasm_stmnts, offset);
	}
	*/

	short reg_src = (bc->regs >> RHS_REG_BIT) & 0x3f;
	bool is_unsigned = IS_FLAG_ON(bc->regs, 1<<FLAG_UNSIGNED);
	short reg_dst = bc->regs & 0x3f;
	int imm = bc->i;
	int mem_offset = bc->mem_offset;
	char sz = bc->regs >> REG_SZ_BIT;
	switch (bc->bc_type)
	{
	case BEGIN_FUNC:
	case BEGIN_FUNC_FOR_INTERPRETER:
	{
		while ((*ptr)->type != BEGIN_FUNC)
		{
			*ptr += 1;
		}
	}break;
	case MOV_M_2_REG_PARAM:
	{
		u64* reg_src_ptr = GetMemValPtr(dbg, reg_src * 8, mem_offset);
		u64* mem_ptr = GetMemValPtr(dbg, PRE_X64_RSP_REG, reg_dst * 8);

		DoOperationOnPtr((char *)mem_ptr, sz, *reg_src_ptr, T_EQUAL); 

	}break;
	case MOV_SSE_2_REG_PARAM:
	{
		u64* reg_src_ptr = GetFloatRegValPtr(dbg, FLOAT_REG_0 + reg_src);
		u64* mem_ptr = GetMemValPtr(dbg, PRE_X64_RSP_REG, reg_dst * 8);

		DoOperationOnPtr((char *)mem_ptr, sz, *reg_src_ptr, T_EQUAL); 

	}break;
	case MOV_R_2_REG_PARAM:
	{
		u64* reg_src_ptr = GetRegValPtr(dbg, reg_src);
		u64* mem_ptr = GetMemValPtr(dbg, PRE_X64_RSP_REG, reg_dst * 8);


		DoOperationOnPtr((char *)mem_ptr, sz, *reg_src_ptr, T_EQUAL); 

	}break;
	case ADD_M_2_R:
	case SUB_M_2_R:
	case MUL_M_2_R:
	case DIV_M_2_R:
	case AND_M_2_R:
	case OR_M_2_R:
	case MOD_M_2_R:
	case MOV_M:
	{
		tkn_type2 op = GetOpBasedOnInst(bc->bc_type);
		u64* mem_ptr = GetMemValPtr(dbg, reg_src, mem_offset);
		u64* reg = GetRegValPtr(dbg, reg_dst);

		if (*reg == 4575657222473777172)
		{
			stmnt_dbg* cur_st;
			func_decl *cur_func = GetFuncBasedOnBc2(dbg, bc);
			if (cur_func)
			{
				cur_st = GetStmntBasedOnOffset(&cur_func->wasm_stmnts, offset);
			}
			auto a = 0;
		}
		DoOperationOnPtr((char *)reg, sz, *mem_ptr, op); 
	}break;
	case SUB_R_2_R:
	case DIV_R_2_R:
	case MUL_R_2_R:
	case MOD_R_2_R:
	case ADD_R_2_R:
	case AND_R_2_R:
	case OR_R_2_R:
	case MOV_R:
	{
		tkn_type2 op = GetOpBasedOnInst(bc->bc_type);
		auto reg_dst_ptr = (u64*)&dbg->mem_buffer[reg_dst * 8];
		auto reg_src_ptr = (u64*)&dbg->mem_buffer[reg_src * 8];


		if(is_unsigned)
			DoOperationOnPtrUnsigned((char *)reg_dst_ptr, sz, *reg_src_ptr, op);
		else
			DoOperationOnPtr((char *)reg_dst_ptr, sz, *reg_src_ptr, op);
	}break;
	case MOV_I:
	{

		tkn_type2 op = GetOpBasedOnInst(bc->bc_type);
		auto reg_dst_ptr = (long long*)&dbg->mem_buffer[reg_dst * 8];

		*reg_dst_ptr = imm;
		/*
		if(is_unsigned)
			DoOperationOnPtrUnsigned(reg_dst_ptr, sz, imm, op);
		else
			DoOperationOnPtr(reg_dst_ptr, sz, imm, op);
			*/
	}break;
	case OR_I_2_R:
	case AND_I_2_R:
	case MOD_I_2_R:
	case MUL_I_2_R:
	case DIV_I_2_R:
	case ADD_I_2_R:
	case SUB_I_2_R:
	{

		tkn_type2 op = GetOpBasedOnInst(bc->bc_type);
		auto reg_dst_ptr = (char*)&dbg->mem_buffer[reg_dst * 8];


		if(is_unsigned)
			DoOperationOnPtrUnsigned(reg_dst_ptr, sz, imm, op);
		else
			DoOperationOnPtr(reg_dst_ptr, sz, imm, op);
	}break;
	case CMP_I_2_R:
	{
		auto reg_dst_ptr = (u64*)&dbg->mem_buffer[reg_dst * 8];
		
		u64* eflags = GetRegValPtr(dbg, EFLAGS_REG);
		*eflags = 0;

		DoCmpInst(reg_dst_ptr, imm, eflags, sz);
		//bool sgnd =
	}break;
	case CMP_R_2_R:
	{
		auto reg_dst_ptr = GetRegValPtr(dbg, reg_dst);
		auto reg_src_ptr = GetRegValPtr(dbg, reg_src);

		u64* eflags = GetRegValPtr(dbg, EFLAGS_REG);
		*eflags = 0;

		DoCmpInst(reg_dst_ptr, *reg_src_ptr, eflags, sz);
		//bool sgnd =
	}break;
	case CMP_M_2_R:
	{
		auto reg_src_ptr = (u64*)&dbg->mem_buffer[reg_src * 8];
		u64 offset = *reg_src_ptr + mem_offset;
		
		auto mem_ptr = (s64*)&dbg->mem_buffer[offset];

		auto reg_dst_ptr = GetRegValPtr(dbg, reg_dst);

		u64* eflags = GetRegValPtr(dbg, EFLAGS_REG);
		*eflags = 0;

		DoCmpInst(reg_dst_ptr, *mem_ptr, eflags, sz);
		//bool sgnd =
	}break;
	case CMP_R_2_M:
	{
		auto reg_dst_ptr = (u64*)&dbg->mem_buffer[reg_dst * 8];
		u64 offset = *reg_dst_ptr + mem_offset;
		
		auto dst = (s64*)&dbg->mem_buffer[offset];

		auto reg_src_ptr = GetRegValPtr(dbg, reg_src);

		u64* eflags = GetRegValPtr(dbg, EFLAGS_REG);
		*eflags = 0;

		DoCmpInst(dst, *reg_src_ptr, eflags, sz);
		//bool sgnd =
	}break;
	case MOV_M_2_PCKD_SSE:
	{
		auto reg_src_ptr = (float*)GetMemValPtr(dbg, reg_src, mem_offset);
		auto reg_dst_ptr = (float*)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);
		memcpy(reg_dst_ptr, reg_src_ptr, FLOAT_REG_SIZE_BYTES);

	}break;
	case CMP_MEM_2_SSE:
	{
		auto reg_src_ptr = (float*)GetMemValPtr(dbg, reg_src, mem_offset);
		auto reg_dst_ptr = (float*)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);

		u64* eflags = GetRegValPtr(dbg, EFLAGS_REG);
		*eflags = 0;
		DoCmpInstFloat(*reg_dst_ptr, *reg_src_ptr, eflags);
		//bool sgnd =
	}break;
	case CMP_SSE_2_MEM:
	{
		auto reg_src_ptr = (float *)GetFloatRegValPtr(dbg, reg_src + FLOAT_REG_0);
		auto reg_dst_ptr = (float*)GetMemValPtr(dbg, reg_dst, mem_offset);

		u64* eflags = GetRegValPtr(dbg, EFLAGS_REG);
		*eflags = 0;
		DoCmpInstFloat(*reg_dst_ptr, *reg_src_ptr, eflags);
		//bool sgnd =
	}break;
	case CMP_SSE_2_SSE:
	{
		auto reg_src_ptr = (float *)GetFloatRegValPtr(dbg, reg_src + FLOAT_REG_0);
		auto reg_dst_ptr = (float *)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);

		u64* eflags = GetRegValPtr(dbg, EFLAGS_REG);
		*eflags = 0;
		DoCmpInstFloat(*reg_dst_ptr, *reg_src_ptr, eflags);
		//bool sgnd =
	}break;
	case CMP_I_2_M:
	{
		auto reg_dst_ptr = (u64*)&dbg->mem_buffer[reg_dst * 8];
		u64 offset = *reg_dst_ptr + mem_offset;
		
		auto dst = (s64*)&dbg->mem_buffer[offset];

		u64* eflags = GetRegValPtr(dbg, EFLAGS_REG);
		*eflags = 0;

		DoCmpInst(dst, imm, eflags, sz);
		//bool sgnd =
	}break;
	case JMP_E:
	{
		u64 flags = *(u64*)&dbg->mem_buffer[EFLAGS_REG * 8];
		if (IS_FLAG_ON(flags, EFLAGS_ZERO))
		{
			*ptr += bc->i + 1;
			*inc_ptr = false;
		}
	}break;
	case JMP_NE:
	{
		u64 flags = *(u64*)&dbg->mem_buffer[EFLAGS_REG * 8];
		if (IS_FLAG_OFF(flags, EFLAGS_ZERO))
		{
			*ptr += bc->i + 1;
			*inc_ptr = false;
		}
	}break;
	case JMP_B:
	case JMP_L:
	{
		u64 flags = *(u64*)&dbg->mem_buffer[EFLAGS_REG * 8];
		if (IS_FLAG_ON(flags, EFLAGS_BELOW))
		{
			*ptr += bc->i + 1;
			*inc_ptr = false;
		}
	}break;
	case JMP_BE:
	case JMP_LE:
	{
		u64 flags = *(u64*)&dbg->mem_buffer[EFLAGS_REG * 8];
		if (IS_FLAG_ON(flags, EFLAGS_BELOW | EFLAGS_ZERO))
		{
			*ptr += bc->i + 1;
			*inc_ptr = false;
		}
	}break;
	case JMP_A:
	case JMP_G:
	{
		u64 flags = *(u64*)&dbg->mem_buffer[EFLAGS_REG * 8];
		if (IS_FLAG_ON(flags, EFLAGS_ABOVE))
		{
			*ptr += bc->i + 1;
			*inc_ptr = false;
		}
	}break;
	case JMP_AE:
	case JMP_GE:
	{
		u64 flags = *(u64*)&dbg->mem_buffer[EFLAGS_REG * 8];
		if (IS_FLAG_ON(flags, EFLAGS_ABOVE | EFLAGS_ZERO))
		{
			*ptr += bc->i + 1;
			*inc_ptr = false;
		}
	}break;
	case CVTSD_MEM_2_SS:
	{
		//tkn_type2 op = GetOpBasedOnInst(bc->bc_type);
		auto reg_dst_ptr = (int *)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);
		auto mem_ptr = (int *)GetMemValPtr(dbg, reg_src, mem_offset);
		DoOperationOnPtrFloat((char *)reg_dst_ptr, 2, (float)*mem_ptr, T_EQUAL);
	}break;
	case MOV_SSE_2_R:
	{
		//tkn_type2 op = GetOpBasedOnInst(bc->bc_type);
		auto reg_dst_ptr = (int *)GetRegValPtr(dbg, reg_dst);
		auto reg_src_ptr = (float *)GetFloatRegValPtr(dbg, reg_src + FLOAT_REG_0);
		DoOperationOnPtr((char *)reg_dst_ptr, 2, (int)*reg_src_ptr, T_EQUAL);
	}break;
	case CVTSD_REG_2_SS:
	{
		//tkn_type2 op = GetOpBasedOnInst(bc->bc_type);
		auto reg_dst_ptr = (int *)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);
		auto reg_src_ptr = (int *)GetRegValPtr(dbg, reg_src);
		DoOperationOnPtrFloat((char *)reg_dst_ptr, 2, (float)*reg_src_ptr, T_EQUAL);
	}break;
	case MOV_SSE_2_SSE:
	case ADD_SSE_2_SSE:
	case SUB_SSE_2_SSE:
	case MUL_SSE_2_SSE:
	case DIV_SSE_2_SSE:
	{
		tkn_type2 op = GetOpBasedOnInst(bc->bc_type);
		auto reg_dst_ptr = (int *)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);
		auto reg_src_ptr = (float *)GetFloatRegValPtr(dbg, reg_src + FLOAT_REG_0);
		DoOperationOnPtrFloat((char *)reg_dst_ptr, 2, *reg_src_ptr, op);
	}break;
	case ADD_MEM_2_SSE:
	case SUB_MEM_2_SSE:
	case DIV_MEM_2_SSE:
	case MUL_MEM_2_SSE:
	case MOV_M_2_SSE:
	{
		tkn_type2 op = GetOpBasedOnInst(bc->bc_type);
		auto reg_dst_ptr = (int *)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);
		auto mem_ptr = (float *)GetMemValPtr(dbg, reg_src, mem_offset);

		DoOperationOnPtrFloat((char*)reg_dst_ptr, 2, *mem_ptr, op);
	}break;
	case MOV_SSE_2_MEM:
	{
		tkn_type2 op = GetOpBasedOnInst(bc->bc_type);
		auto reg_src_ptr = (int *)GetFloatRegValPtr(dbg, reg_src + FLOAT_REG_0);
		u64* mem_ptr = GetMemValPtr(dbg, reg_dst, mem_offset);
		DoOperationOnPtr((char *)mem_ptr, 2, *reg_src_ptr, op);
	}break;
	case MOV_F_2_PCKED_SSE:
	{
		auto reg_dst_ptr = (float *)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);
		__m128 sse_reg = _mm_set_ps(bc->f32, bc->f32, bc->f32, bc->f32);
		_mm_store_ps(reg_dst_ptr, sse_reg);
	}break;
	case MOV_F_2_SSE:
	{
		auto reg_dst_ptr = (float *)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);
		*reg_dst_ptr = bc->f32;
	}break;
	case MOV_PCKD_SSE_2_M:
	{
		auto reg_src_ptr = (float *)GetFloatRegValPtr(dbg, reg_src + FLOAT_REG_0);
		u64* mem_ptr = GetMemValPtr(dbg, reg_dst, mem_offset);
		
		memcpy(mem_ptr, reg_src_ptr, FLOAT_REG_SIZE_BYTES);
		//_mm_storeu_ps(reg_dst_ptr, dst);
	}break;
	case MOV_PCKD_SSE_2_PCKD_SSE:
	{
		auto reg_dst_ptr = (float *)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);
		auto reg_src_ptr = (float *)GetFloatRegValPtr(dbg, reg_src + FLOAT_REG_0);
		memcpy(reg_dst_ptr, reg_src_ptr, FLOAT_REG_SIZE_BYTES);
	}break;
	case MUL_PCKD_SSE_2_PCKD_SSE:
	{
		auto reg_dst_ptr = (float *)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);
		auto reg_src_ptr = (float *)GetFloatRegValPtr(dbg, reg_src + FLOAT_REG_0);
		
		__m128 src = _mm_loadu_ps(reg_src_ptr);
		__m128 dst = _mm_loadu_ps(reg_dst_ptr);

		__m128 mul = _mm_mul_ps(dst, src);
		_mm_storeu_ps(reg_dst_ptr, mul);
	}break;
	case ADD_PCKD_SSE_2_PCKD_SSE:
	{
		auto reg_dst_ptr = (float *)GetFloatRegValPtr(dbg, reg_dst + FLOAT_REG_0);
		auto reg_src_ptr = (float *)GetFloatRegValPtr(dbg, reg_src + FLOAT_REG_0);
		
		__m128 src = _mm_loadu_ps(reg_src_ptr);
		__m128 dst = _mm_loadu_ps(reg_dst_ptr);

		__m128 summed = _mm_add_ps(dst, src);
		_mm_storeu_ps(reg_dst_ptr, summed);
	}break;
	case STORE_R_2_M:
	{
		u64* reg_src_ptr = GetRegValPtr(dbg, reg_src);
		auto reg_dst_ptr = (u64*)&dbg->mem_buffer[reg_dst * 8];
		u64 offset = *reg_dst_ptr + mem_offset;
		
		auto dst = (char*)&dbg->mem_buffer[offset];
		DoOperationOnPtr(dst, sz, *reg_src_ptr, T_EQUAL);
	}break;
	case RET:
	{
		u64* reg_src_ptr = GetRegValPtr(dbg, reg_src);
		u64* reg_stack_ptr = GetRegValPtr(dbg, PRE_X64_RSP_REG);
		u64* mem_ptr = GetMemValPtr(dbg, PRE_X64_RSP_REG, 0);
		*ptr =(byte_code2 *) *mem_ptr;
		(*reg_stack_ptr) += 8;
		*inc_ptr = false;
		dbg->return_stack_bc2.pop_back();

		switch (dbg->break_type)
		{
		case DBG_BREAK_ON_DIFF_IR_BUT_SAME_FUNC:
		{
			if(offset >= dbg->next_stat_break_func->bcs2_start && offset <= dbg->next_stat_break_func->bcs2_end)
				dbg->break_type = DBG_BREAK_ON_DIFF_IR;
		}break;
		case DBG_BREAK_ON_DIFF_STAT_BUT_SAME_FUNC:
		{
			if(offset >= dbg->next_stat_break_func->bcs2_start && offset <= dbg->next_stat_break_func->bcs2_end)
				dbg->break_type = DBG_BREAK_ON_DIFF_STAT;
		}break;
		}
	}break;
	case JMP:
	{
		*ptr += bc->i + 1;
		*inc_ptr = false;
	}break;
	case INST_CALL_OUTSIDER:
	{
		wasm_interp *wasm_state = dbg->wasm_state;
		func_decl* call_f = wasm_state->funcs[bc->i];
		if (IS_FLAG_ON(call_f->flags, FUNC_DECL_IS_OUTSIDER))
		{
			if (wasm_state->outsiders.find(call_f->name) != wasm_state->outsiders.end())
			{
				u64 prev_reg_val = *(u64*)&dbg->mem_buffer[STACK_PTR_REG * 8];
				u64 prev_base_reg_val = *(u64*)&dbg->mem_buffer[BASE_STACK_PTR_REG * 8];

				auto stack_reg = (u64 *)&dbg->mem_buffer[PRE_X64_RSP_REG * 8];
				*stack_reg -= 8;

				*(u64*)&dbg->mem_buffer[STACK_PTR_REG * 8] = *stack_reg;
				*(u64*)&dbg->mem_buffer[BASE_STACK_PTR_REG * 8] = *stack_reg;

				OutsiderFuncType func_ptr = wasm_state->outsiders[call_f->name];
				func_ptr(dbg);
				*stack_reg += 8;
				auto reg_src_ptr = (u64*)&dbg->mem_buffer[RET_1_REG * 8];
				auto reg_dst_ptr = (u64*)&dbg->mem_buffer[0];
				auto reg_xmm0_dst_ptr = (u64*)&dbg->mem_buffer[FLOAT_REG_0 * FLOAT_REG_SIZE_BYTES];

				*reg_dst_ptr = *reg_src_ptr;
				*reg_xmm0_dst_ptr = *reg_src_ptr;

				*(u64*)&dbg->mem_buffer[STACK_PTR_REG * 8] = prev_reg_val;
				*(u64*)&dbg->mem_buffer[BASE_STACK_PTR_REG * 8] = prev_base_reg_val;
			}
			else
				ASSERT(0);

		}
		else if (IS_FLAG_ON(call_f->flags, FUNC_DECL_X64))
		{
			Bc2CallX64(dbg, ptr, call_f);
			/*
			auto stack_reg = (u64 *)&dbg->mem_buffer[PRE_X64_RSP_REG * 8];
			*stack_reg -= 8;
			WasmCallX64(dbg->wasm_state, *dbg, (u8 *)dbg->mem_buffer, call_f, *stack_reg);
			*stack_reg += 8;
			auto reg_src_ptr = (u64*)&dbg->mem_buffer[RET_1_REG * 8];
			if (call_f->ret_type.IsFloat())
			{
				auto reg_dst_ptr = (float*)&dbg->mem_buffer[FLOAT_REG_0 * 8];
				*reg_dst_ptr = *(float*)reg_src_ptr;
			}
			else
			{
				auto reg_dst_ptr = (u64*)&dbg->mem_buffer[0];
				*reg_dst_ptr = *reg_src_ptr;
			}
			*/
		}
		else
		{
			ASSERT(0);
		}
	
	}break;
	case MOVZX_M:
	{
		u64* dst_ptr = GetRegValPtr(dbg, reg_dst);
		u64* src_ptr = GetMemValPtr(dbg, reg_src, mem_offset);
		DoMovZXInts(dbg, dst_ptr, src_ptr, bc->rhs_and_lhs_reg_sz);
	}break;
	case MOVZX_R:
	{
		u64* dst_ptr = GetRegValPtr(dbg, reg_dst);
		u64* src_ptr = GetRegValPtr(dbg, reg_src);
		DoMovZXInts(dbg, dst_ptr, src_ptr, bc->rhs_and_lhs_reg_sz);

	}break;
	case INST_CALL:
	{
		u64* reg_stack_ptr = GetRegValPtr(dbg, PRE_X64_RSP_REG);
		(*reg_stack_ptr) -= 8;
		u64* mem_ptr = GetMemValPtr(dbg, PRE_X64_RSP_REG, 0);
		*mem_ptr = (u64)(*ptr + 1);
		dbg->return_stack_bc2.emplace_back((byte_code2**)(mem_ptr));
		*ptr += imm;
		*inc_ptr = false;
	}break;
	case INST_CALL_REG:
	{
		u64* reg_src_ptr = GetRegValPtr(dbg, reg_dst);
		u64* reg_stack_ptr = GetRegValPtr(dbg, PRE_X64_RSP_REG);
		(*reg_stack_ptr) -= 8;
		u64* mem_ptr = GetMemValPtr(dbg, PRE_X64_RSP_REG, 0);
		*mem_ptr = (u64)(*ptr + 1);
		dbg->return_stack_bc2.emplace_back((byte_code2**)(mem_ptr));
		*ptr = dbg->lang_stat->bcs2_start + *reg_src_ptr;
		*inc_ptr = false;
	}break;
	case INST_LEA:
	{
		u64* reg_dst_ptr = GetRegValPtr(dbg, reg_dst);

		auto reg_src_ptr = (u64*)&dbg->mem_buffer[reg_src * 8];
		u64 offset = *reg_src_ptr + mem_offset;
		
		auto dst = (u64)offset;
		DoOperationOnPtr((char *)reg_dst_ptr, sz, dst, T_EQUAL);
	}break;
	case STORE_I_2_M:
	{
		auto reg_dst_ptr = (u64*)&dbg->mem_buffer[reg_dst * 8];
		u64 offset = *reg_dst_ptr + mem_offset;
		
		auto dst = (char*)&dbg->mem_buffer[offset];
		DoOperationOnPtr(dst, sz, imm, T_EQUAL);
	}break;
	case INT3:
	{
		*inc_ptr = false;
	}break;
	case MOV_I_2_REG_PARAM:
	{
		reg_src += REG_PARAM_START;
		auto stack_ptr = (u64*)&dbg->mem_buffer[PRE_X64_RSP_REG * 8];
		u64 stack_val = *stack_ptr;
		stack_val += reg_dst * 8;
		*(u64*)&dbg->mem_buffer[stack_val] = imm;

		auto a = 0;
	}break;
	case STORE_REG_PARAM:
	{
		/*
		reg_src += REG_PARAM_START;
		auto stack_ptr = (u64*)&dbg->mem_buffer[PRE_X64_RSP_REG * 8];
		u64 stack_val = *stack_ptr;
		stack_val += bc->mem_offset;
		auto reg_src_ptr = (u64*)&dbg->mem_buffer[reg_src * 8];

		*(u64*)&dbg->mem_buffer[stack_val] = *reg_src_ptr;

		
		auto a = 0;
		*/
	}break;
	case RELOC:
	{
		u64* reg_dst_ptr = GetRegValPtr(dbg, reg_dst);
		*reg_dst_ptr = GLOBALS_OFFSET + bc->i;
		
	}break;
	default:
		ASSERT(0);
	}
}

bool StatHasInst(stmnt_dbg *cur_st, byte_code2 *start_bc, byte_code2 **out, byte_code_enum type)
{
	byte_code2* start_st_bc = start_bc + cur_st->start;
	byte_code2* end_st_bc = start_bc + cur_st->end;
	while (start_st_bc->type != type && start_st_bc < end_st_bc)
	{
		start_st_bc++;
	}
	if (start_st_bc->type == type)
	{
		*out = start_st_bc;
		return true;
	}
	return false;
}

void Bc2Interpreter(dbg_state* dbg, GLFWwindow *window, func_decl* start_f)
{
	char buffer[512];

	*(u64*)&dbg->mem_buffer[RIP_REG * 8] = (u64)(dbg->lang_stat->bcs2_start + start_f->bcs2_start);
	byte_code2** rip_ptr = (byte_code2**)&dbg->mem_buffer[RIP_REG * 8];
	byte_code2 * cur_bc = *rip_ptr;
	byte_code2* start_bc = dbg->lang_stat->bcs2_start;

	byte_code2* func_start_bc = cur_bc;
	byte_code2* func_end_bc = dbg->lang_stat->bcs2_start + start_f->bcs2_end;;
	*(u64*)&dbg->mem_buffer[PRE_X64_RSP_REG * 8] = STACK_PTR_START;
	std::string str;
	byte_code2* aux_bc = cur_bc;


	scope* cur_scp = nullptr;
	bool done = false;
	bool show_bc = false;

	int mem_wnd_offset = 0;
	int mem_wnd_show_type = 5;
	char* mem_wnd_items[] = {"u8", "s8", "u16", "s16", "u32", "s32", "u64", "s64", "f32", "char"};
	int wasm_bcs_to_show = 500;

	int total_items = IM_ARRAYSIZE(mem_wnd_items);
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	stmnt_dbg* cur_st = nullptr;

	func_decl* aux_func = nullptr;
	dbg->cur_func = nullptr;
	bool center_inst = true;
	dbg->cur_bc2 = &cur_bc;

	std::string aux_string;
	while (cur_bc != nullptr)
	{
		if (dbg->break_type != DBG_BREAK_RIP_CORRUPTED)
		{
			dbg->prev_valid_bc = cur_bc;
		}
		cur_bc = *(byte_code2**)&dbg->mem_buffer[RIP_REG * 8];
		int offset = cur_bc - start_bc;


		switch (dbg->break_type)
		{
		case DBG_BREAK_ON_DIFF_STAT_BUT_SAME_FUNC:
		{
			if (dbg->next_stat_break_func && (offset >= dbg->next_stat_break_func->bcs2_start && offset <= dbg->next_stat_break_func->bcs2_end))
			{
				breakpoint bp;
				cur_st = GetStmntBasedOnOffset(&dbg->next_stat_break_func->wasm_stmnts, offset);
				if (cur_st && cur_st != dbg->prev_st)
				{
					MakeCurBcToBeBreakpoint(dbg, cur_bc, cur_st->line);
					dbg->prev_st = cur_st;
				}
			}
		}break;
		case DBG_BREAK_ON_DIFF_STAT:
		{
				dbg->cur_func = GetFuncBasedOnBc2(dbg, cur_bc);
				if (dbg->cur_func)
				{
					cur_st = GetStmntBasedOnOffset(&dbg->cur_func->wasm_stmnts, offset);
					if (cur_st && cur_st != dbg->prev_st)
					{
						MakeCurBcToBeBreakpoint(dbg, cur_bc, cur_st->line);
						dbg->prev_st = cur_st;
					}
				}
		}break;
		case DBG_BREAK_ON_DIFF_IR:
		{
			if (cur_bc != dbg->prev_bc)
			{
				breakpoint bp;
				if (!dbg->cur_func || dbg->cur_func && (offset < dbg->cur_func->bcs2_start || offset > dbg->cur_func->bcs2_end))
				{
					dbg->cur_func = GetFuncBasedOnBc2(dbg, cur_bc);
				}
				cur_st = GetStmntBasedOnOffset(&dbg->cur_func->wasm_stmnts, offset);
				if (cur_st)
				{
					MakeCurBcToBeBreakpoint(dbg, cur_bc, cur_st->line);
					dbg->prev_bc = cur_bc;
				}
			}
		}break;
		case DBG_BREAK_RIP_CORRUPTED:
		{
			cur_bc = dbg->prev_valid_bc;
		}break;
		case DBG_BREAK_ON_DIFF_IR_BUT_SAME_FUNC:
		{
			if (dbg->next_stat_break_func && (offset >= dbg->next_stat_break_func->bcs2_start && offset <= dbg->next_stat_break_func->bcs2_end) &&
				dbg->prev_bc != cur_bc)
			{
				breakpoint bp;
				cur_st = GetStmntBasedOnOffset(&dbg->next_stat_break_func->wasm_stmnts, offset);
				if (cur_st)
				{
					MakeCurBcToBeBreakpoint(dbg, cur_bc, cur_st->line);
					dbg->prev_bc = cur_bc;
				}
			}
		}break;
		}

		
		bool inc_ptr = true;
		bool valid = true;
		Bc2Logic(dbg, (byte_code2**)&dbg->mem_buffer[RIP_REG * 8], &inc_ptr, &valid, offset);
		if (!valid)
			continue;
		if (!*rip_ptr)
			return;
		if (inc_ptr)
		{
			(*rip_ptr)++;
		}
		if (cur_bc->type == INT3)
		{
			dbg->frame_is_from_dbg = true;
			if (!dbg->cur_func || dbg->cur_func != dbg->prev_func)
			{
				dbg->cur_func = GetFuncBasedOnBc2(dbg, cur_bc);
				func_end_bc = dbg->lang_stat->bcs2_start + dbg->cur_func->bcs2_end;
				dbg->prev_func = dbg->cur_func;
			}
			if (!dbg->cur_func)
				continue;
			offset = cur_bc - start_bc;
			if (!cur_scp)
			{
				cur_st = GetStmntBasedOnOffset(&dbg->cur_func->wasm_stmnts, offset);
				if (cur_st)
				{
					cur_scp = FindScpWithLine(dbg->cur_func, cur_st->line);
					UpdateExprWindow(*dbg, PRE_X64_RSP_REG, cur_st->line);
					auto a = 0;
					dbg->cur_st = cur_st;
				}
				else
				{
					dbg->prev_func = nullptr;
					//continue;
				}

				//dbg.cu = cur_st;
			}
			int bp_idx = 0;
			breakpoint* bp = nullptr;
			if (cur_st)
			{
				FOR_VEC(b, dbg->breakpoints)
				{
					if (b->line == cur_st->line)
					{
						bp = b;
					}
					b->bc->type = b->prev_inst;
					if (!bp)
						bp_idx++;
				}
			}

			if (!bp && !cur_st)
			{
				dbg->cur_func = nullptr;
				continue;
			}
			glfwPollEvents();
			if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
			{
				ImGui_ImplGlfw_Sleep(10);
				continue;
			}

			//ImGui::ImGuiContext& g = *ImGui::GImGui;
			//if(g.WithinFrameScope)
			if(ImGui::WithinFrame())
				ImGui::Render();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (dbg->break_type == DBG_BREAK_RIP_CORRUPTED)
			{
				cur_bc = dbg->prev_valid_bc;
				ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "rip got corrupted");
			}
			if (ImGui::Selectable("show bc", &show_bc))
			{
				if(show_bc)
					center_inst = true;
			}
			int height = 300;
			ImGui::BeginChild("code", ImVec2(500, height));
			bool release_inst = false;
			if (cur_st)
			{
				stmnt_dbg* next_st = cur_st + 1;
				stmnt_dbg* start_func_st = dbg->cur_func->wasm_stmnts.begin();;
				stmnt_dbg* aux_cur_st = start_func_st;
				bool first_stat = true;
				//int max_ir = min(irs_to_show, ir_ar->size());
				byte_code2* end = func_end_bc;
				int line_start = dbg->cur_func->scp->line_start;
				int line_end = dbg->cur_func->scp->line_end;

				for (int i = line_start; i <= line_end; i++)
				{
					if (show_bc)
					{
						ir_rep* start_ir = ((own_std::vector<ir_rep > *) & dbg->cur_func->ir)->begin();
						ir_rep* cur_ir = start_ir + aux_cur_st->start_ir;
						while (aux_cur_st->line > 0 && i > aux_cur_st->line)
						{

							stmnt_dbg* start_func_st = dbg->cur_func->wasm_stmnts.begin();;
							func_start_bc = start_bc + dbg->cur_func->bcs2_start;
							byte_code2* aux_bc = start_bc + aux_cur_st->start;
							byte_code2* bc_end_st = start_bc + aux_cur_st->end;
							while (aux_bc < bc_end_st)
							{
								int offset_bc = aux_bc - start_bc;


								while (cur_ir->start == offset_bc)
								{
									WasmIrToString(dbg, cur_ir, aux_string);
									ImGui::TextColored(ImVec4(0.6, 0.4, 0.6, 1.0), "%d|%s", cur_ir->idx, aux_string.c_str());
									cur_ir++;
								}

								sprintf(buffer, "%d", offset_bc);
								if (ImGui::Button(buffer, ImVec2(50, 20)))
								{
									MakeCurBcToBeBreakpoint(dbg, aux_bc, aux_cur_st->line);
									release_inst = true;
								}
								//ImGui::TextColored(ImVec4(0.4, 0.4, 0.4, 1.0), "%d|", offset_bc);
								ImGui::SameLine();
								Bc2ToString(dbg, aux_bc, buffer, 512);
								if (cur_bc == aux_bc)
								{
									if (center_inst)
									{
										ImVec2 pos = ImGui::GetCursorPos();
										ImGui::SetScrollY(pos.y - height / 2);
										center_inst = false;
									}
									ImGui::TextColored(ImVec4(0.8, 0.5, 0.5, 1.0), "\t%s", buffer);
								}
								else
								{
									ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1.0), "\t%s", buffer);
								}
								aux_bc++;
							}
							aux_cur_st++;

						}
					}
					if (cur_st->line == i)
					{
						if (center_inst)
						{
							ImVec2 pos = ImGui::GetCursorPos();
							ImGui::SetScrollY(pos.y - height / 2);
							center_inst = false;
						}
						ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "%d: %s", i, GetFileLn(dbg->lang_stat, i - 1, dbg->cur_func->from_file));
					}
					else
						ImGui::Text("%d: %s", i, GetFileLn(dbg->lang_stat, i - 1, dbg->cur_func->from_file));

				}
			}
			else
			{
			}

			FOR_VEC(b, dbg->breakpoints)
			{
				b->bc->type = INT3;
			}
			ImGui::EndChild();

			ImGui::SameLine();
			SHowMemWindow(*dbg, (char **)&mem_wnd_items, mem_wnd_show_type, mem_wnd_offset, total_items, PRE_X64_RSP_REG, dbg->cur_func);

			int base_ptr = WasmGetRegVal(dbg, PRE_X64_RSP_REG);
			BeginLocalsChild(*dbg, base_ptr, cur_scp);

			ImGui::SameLine();
			ShowExprWindow(*dbg, PRE_X64_RSP_REG, cur_st->line);
			bool f11_pressed_but_dint_find_call_so_normal_step = false;

			if (IsKeyRepeat(dbg->data, GLFW_KEY_F11))
			{

				byte_code2* out;
				if (StatHasInst(cur_st, start_bc, &out, INST_CALL))
				{
					/*
					byte_code2* func_call_st = start_st_bc + start_st_bc->i;
					func_decl *found_f = GetFuncBasedOnBc2(dbg, func_call_st);

					stmnt_dbg* found_f_first_stat = found_f->wasm_stmnts.begin();
					byte_code2* func_call_first_st_bc = start_bc + found_f_first_stat->start;

					//MakeCurBcToBeBreakpoint(dbg, func_call_first_st_bc, found_f_first_stat->line);
					*/
					release_inst = true;
					dbg->break_type = DBG_BREAK_ON_DIFF_STAT_BUT_SAME_FUNC;
					byte_code2* dst_bc = out + out->i;
					func_decl *dst_func = GetFuncBasedOnBc2(dbg, dst_bc);
					dbg->next_stat_break_func = dst_func;
					dbg->cur_func = nullptr;
					dbg->prev_st = cur_st;
				}
				else
					f11_pressed_but_dint_find_call_so_normal_step = true;
			}
			if (IsKeyRepeat(dbg->data, GLFW_KEY_F5))
			{
				release_inst = true;
				dbg->break_type = DBG_NO_BREAK;
			}
			if (IsKeyRepeat(dbg->data, GLFW_KEY_F10) || f11_pressed_but_dint_find_call_so_normal_step)
			{
				dbg->prev_bc = cur_bc;
				if (show_bc)
				{
					dbg->break_type = DBG_BREAK_ON_DIFF_IR_BUT_SAME_FUNC;
				}
				else
				{
					stmnt_dbg* next_st = cur_st + 1;
					dbg->prev_st = cur_st;
					byte_code2* out;
					if (next_st < dbg->cur_func->wasm_stmnts.end())
					{
						dbg->break_type = DBG_BREAK_ON_DIFF_STAT_BUT_SAME_FUNC;
					}
					else
					{
						dbg->break_type = DBG_BREAK_ON_DIFF_STAT;
					}
				}
				dbg->next_stat_break_func = dbg->cur_func;
				release_inst = true;
			}

			if (release_inst)
			{
				if (bp)
				{
					cur_bc->type = bp->prev_inst;
					if (bp->one_time_bp)
						dbg->breakpoints.remove(bp_idx);
				}
				else
				{
					(*rip_ptr)++;
					//cur_bc++;
				}
				//dbg->cur_func = nullptr;

				cur_scp = nullptr;
				if(!show_bc)
					center_inst = true;
			}


			auto  a = 0;

			// Rendering
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window);
			ClearKeys(dbg->data);

			//auto a = 0;
			//ir_rep *ir = GetIrBasedOnOffset(dbg, offset);
		}

	}
}
void GetMsgFromGame(void* gl_state);
void WasmInterpRun(wasm_interp* winterp, unsigned char* mem_buffer, unsigned int len, std::string func_start, long long* args, int total_args)
{
	char buffer[64];
	dbg_state& dbg = *winterp->dbg;
	
	own_std::vector<wasm_bc>& bcs = winterp->dbg->bcs;


	own_std::vector<func_decl*> &func_stack = dbg.func_stack;
	int i = 0;
	FOR_VEC(func, winterp->funcs)
	{
		func_decl* f = *func;
		if (f->name == func_start)
		{
			func_stack.emplace_back(f);
		}
		f->func_idx = i;
		i++;
	}
	ASSERT(func_stack.size() == 1);
	func_decl* cur_func = func_stack.back();

	block_linked* cur = nullptr;
	
	int a = 0;



	own_std::vector<wasm_stack_val> &wasm_stack = dbg.wasm_stack;
	wasm_stack.reserve(16);

	//FOR_VEC(bc, wf->bcs)
	wasm_bc *bc = &bcs[cur_func->wasm_code_sect_idx];
	//bc->one_time_dbg_brk = true;


	dbg.cur_bc = &bc;
	dbg.cur_func = cur_func;
	dbg.mem_buffer = (char *)mem_buffer;
	dbg.wasm_state = winterp;

	int first_start_offset = dbg.cur_func->wasm_stmnts[0].start;
	//bcs[first_start_offset].one_time_dbg_brk = true;
	
	// WASM BYTECODE

	bool can_break = false;
	bool can_execute = true;
	dbg.can_execute = &can_execute;
	dbg.break_type = DBG_BREAK_ON_NEXT_STAT;

	own_std::vector<ir_rep>* ir_ar = (own_std::vector<ir_rep> *) &cur_func->ir;
	//ir_rep* cur_ir = ir_ar->begin();

	glfwSetErrorCallback(glfw_error_callback);
	OpenWindow(&dbg);
	auto window = *(GLFWwindow** ) &dbg.mem_buffer[RET_1_REG * 8];
	dbg.dbg_alloc = (mem_alloc *)AllocMiscData(dbg.lang_stat, sizeof(mem_alloc));
	dbg.dbg_alloc->chunks_cap = 1024 * 1024;
	dbg.dbg_alloc->in_use.hash_table_size = 1024 * 1024;
	InitMemAlloc(dbg.dbg_alloc);

	timer tm;
	InitTimer(&tm);
	StartTimer(&tm);

#ifndef  WASM_DBG
	dbg.lang_stat->is_x64_bc_backend = true;
	Bc2Interpreter(&dbg, window, cur_func);
	dbg.lang_stat->is_x64_bc_backend = false;
#endif
	EndTimer(&tm);
	float time = GetTimerMSFloat(&tm);
	printf("ir time: %.3f\n", time);
	/*
	int base_ = WasmGetRegVal(&dbg, STACK_PTR_REG);
	*(u64*)&dbg.mem_buffer[base_] = 20016;
	WasmIrInterp(&dbg, window, cur_func);
	*/

	StartTimer(&tm);

	int stack_offset = STACK_PTR_START;
	*(int*)&mem_buffer[STACK_PTR_REG * 8] = stack_offset;
	*(int*)&mem_buffer[BASE_STACK_PTR_REG * 8] = stack_offset;
	*(int*)&mem_buffer[stack_offset + 8] = STACK_PTR_START;

	auto stack_ptr_val = (long long *)&mem_buffer[stack_offset];

	for (int i = 0; i < total_args; i++)
	{
		*stack_ptr_val = args[i];
		stack_ptr_val++;
	}
	bool on_break = false;
	stmnt_dbg* cur_st = nullptr;
	ir_rep* cur_ir = nullptr;
	scope* cur_scp = nullptr;
	int idx = 0;
	bool show_wasm = false;
	bool res = true;

	int mem_wnd_offset = 0;
	int mem_wnd_show_type = 0;
	char* mem_wnd_items[] = {"u8", "s8", "u16", "s16", "u32", "s32", "u64", "s64", "f32", "char"};
	int wasm_bcs_to_show = 500;

	int square_1_addr = 0;
	int square_2_addr = 0;
	//int total_wasm_bcs = 100;
	std::string from_engine_str;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	func_stack.clear();
	return;
	while(!can_break)
	{
		int bc_idx = (long long)(bc - &bcs[0]);
		
		wasm_stack_val val = {};
		if(dbg.break_type == DBG_BREAK_ON_DIFF_STAT || dbg.break_type == DBG_BREAK_ON_DIFF_STAT_BUT_SAME_FUNC)
			cur_st = GetStmntBasedOnOffset(&dbg.cur_func->wasm_stmnts, bc_idx);
		//cur_ir = GetIrBasedOnOffset(&dbg, bc_idx);
		bool found_stat = cur_st && dbg.cur_st;
		bool is_different_stmnt =  found_stat && dbg.break_type == DBG_BREAK_ON_DIFF_STAT && cur_st->line != dbg.cur_st->line;
		bool is_different_stmnt_same_func = found_stat && dbg.break_type == DBG_BREAK_ON_DIFF_STAT_BUT_SAME_FUNC && cur_st->line != dbg.cur_st->line && dbg.next_stat_break_func == dbg.cur_func;

		/*
		if ( dbg.break_type == DBG_BREAK_ON_NEXT_BC || is_different_stmnt || is_different_stmnt_same_func || bc->dbg_brk || bc->one_time_dbg_brk)
		{
			if (!cur_st)
				cur_st = &dbg.cur_func->wasm_stmnts[0];
			dbg.cur_st = cur_st;
			dbg.cur_ir = cur_ir;
			WasmOnArgs(&dbg);
			if (dbg.some_bc_modified)
			{
				dbg.some_bc_modified = false;
				continue;
			}
			if (bc->type == WASM_INST_DBG_BREAK)
			{
				bc++;
				continue;
			}
			bc->one_time_dbg_brk = false;
		}
		*/
		//bool res = WasmBcLogic(winterp, dbg, &bc, mem_buffer, &cur, can_break);

		/*
		if (!dbg.lang_stat->is_engine)
		{
			CheckPipeAndGetString(std_in, from_engine_str);

			if (from_engine_str.size() > 0)
			{
				Write(std_out, (char *)from_engine_str.data(), from_engine_str.size());
				FlushFileBuffers(std_out);
			}

		}
		else
		{
			GetMsgFromGame(dbg.data);
		}
		*/


		if((!bc->dbg_brk || bc->dont_dbg_brk) && !bc->one_time_dbg_brk && can_execute && !is_different_stmnt_same_func)
			res = WasmBcLogic(winterp, dbg, &bc, mem_buffer, &cur, can_break);
		if (bc < &bcs[0])
		{
			bc++;
			continue;
		}
		bool got_executed_because_its_from_engine_so_no_need_brk_again = false;
		if (bc->from_engine_break && !bc->dbg_brk )
		{
			bc->dbg_brk = true;
			got_executed_because_its_from_engine_so_no_need_brk_again = true;
			 
		}
		//IrLogic(dbg_state* dbg, ir_rep* ir)
		if(is_different_stmnt_same_func || !can_execute|| bc->type == WASM_INST_DBG_BREAK || 
			(bc->dbg_brk && !bc->dont_dbg_brk && !got_executed_because_its_from_engine_so_no_need_brk_again)|| 
			bc->one_time_dbg_brk || dbg.break_type == DBG_BREAK_ON_NEXT_BC && dbg.cur_func == dbg.next_stat_break_func && !bc->dont_dbg_brk)
		{
			//bc->dbg_brk = true;
			can_execute = false;
			if (!cur_scp)
			{
				bc_idx = (long long)(bc - &bcs[0]);
				cur_st = GetStmntBasedOnOffset(&dbg.cur_func->wasm_stmnts, bc_idx);
				if (cur_st)
				{
					cur_ir = GetIrBasedOnOffset(&dbg, bc_idx);
					cur_scp = FindScpWithLine(dbg.cur_func, cur_st->line);
					auto a = 0;
					dbg.cur_st = cur_st;
				}

				//dbg.cu = cur_st;
			}
			glfwPollEvents();
			if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
			{
				ImGui_ImplGlfw_Sleep(10);
				continue;
			}

			//ImGui::ImGuiContext& g = *ImGui::GImGui;
			//if(g.WithinFrameScope)
			if(ImGui::WithinFrame())
				ImGui::Render();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			bool show_demo = true;

			ImGui::Selectable("wasm", &show_wasm);

			ImGui::BeginChild("code", ImVec2(500, 200));
			//if (cur_st)
			//{
				if (show_wasm)
				{
					if (cur_st)
					{
						ImGui::Text("%s", GetFileLn(dbg.lang_stat, cur_st->line - 1, dbg.cur_func->from_file));

						
					}
					std::string wstr = "";
					ir_rep *cur_ir_aux = cur_ir;

					wasm_bc* aux_bc = bc;

					int cur_tab = 0;
					if (cur_st)
					{
						for (int i = 0; i < wasm_bcs_to_show; i++)
						{
							cur_ir = GetIrBasedOnOffset(&dbg, aux_bc->start_code, cur_st->start_ir, cur_st->end_ir);
							if (cur_ir)
							{
								//ImGui::Text("%s", WasmIrToString(&dbg, cur_ir).c_str());
							}

							if (aux_bc->type == WASM_INST_BLOCK || aux_bc->type == WASM_INST_LOOP)
							{
								cur_tab++;
							}
							if (aux_bc->type == WASM_INST_END)
							{
								cur_tab--;
							}

							for (int tab = 0; tab < cur_tab; tab++)
							{
								buffer[tab] = ' ';
							}
							buffer[max(cur_tab, 0)] = 0;
							//if (aux_bc->type == WASM_INST_BLOCK ||aux_bc->type == WASM_INST_BREAK  || aux_bc->type == WASM_INST_LOOP ||  aux_bc->type == WASM_INST_BREAK_IF){
								wstr = std::string(buffer) + WasmGetBCString(&dbg, dbg.cur_func, aux_bc, &bcs) + "\n";


								snprintf(buffer, 64, "O##%d", i);
								wasm_bc* cur_bc = &aux_bc[i];
								bool before_val = cur_bc->dbg_brk;

								if (cur_bc->dbg_brk)
									ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 0, 0)));

								if (ImGui::Button(buffer, ImVec2(20, 15)))
								{
									cur_bc->dbg_brk = !cur_bc->dbg_brk;
								}
								if (before_val)
									ImGui::PopStyleColor();

								ImGui::SameLine();
								ImGui::Text(" %d ", aux_bc->start_code);
								ImGui::SameLine();

								if ((bc) == &bcs[i])
									ImGui::TextColored(ImVec4(ImColor(255, 255, 0)), (char*)wstr.c_str());
								else
									ImGui::Text((char*)wstr.c_str());

								if (aux_bc->type == WASM_INST_BREAK || aux_bc->type == WASM_INST_BREAK_IF)
								{
									ImGui::SameLine();
									ImGui::Text(" jmps to %d", aux_bc->jmps_to->start_code);

								}
							//}
							aux_bc++;

						}
					}
						//cur_ir_aux++;

				}
				else if(cur_st)
				{
					
					int sz = sprintf(buffer, "%d", cur_st->line);
					memset(buffer, ' ', sz + 1);
					buffer[sz + 1] = 0;

					ImGui::Text("%s%s", buffer, GetFileLn(dbg.lang_stat, cur_st->line - 3, dbg.cur_func->from_file));
					ImGui::Text("%s%s", buffer, GetFileLn(dbg.lang_stat, cur_st->line - 2, dbg.cur_func->from_file));
					ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "%d: %s", cur_st->line, GetFileLn(dbg.lang_stat, cur_st->line - 1, dbg.cur_func->from_file));
					ImGui::Text("%s%s", buffer, GetFileLn(dbg.lang_stat, cur_st->line, dbg.cur_func->from_file));
					ImGui::Text("%s%s", buffer, GetFileLn(dbg.lang_stat, cur_st->line + 1, dbg.cur_func->from_file));
				}
			//}
			ImGui::EndChild();

			ImGui::SameLine();
			ImGui::BeginChild("values", ImVec2(500, 100));

			FOR_VEC(val, dbg.wasm_stack)
			{
				std::string type = "";
				switch (val->type)
				{
				case WSTACK_VAL_INT:
					type = "i32 ";
					type += WasmNumToString(&dbg, val->s32);
					break;
				case WSTACK_VAL_F32:
				{
					type = "f32 ";
					int aux = 0;
					*(float*)&aux = val->f32;
					type += WasmNumToString(&dbg, aux, -1, PRINT_FLOAT);
				}break;
				default:
					ASSERT(false);
				}
				ImGui::Text(type.c_str());
			}

			ImGui::EndChild();


			
			int base_ptr = WasmGetRegVal(&dbg, BASE_STACK_PTR_REG);
			BeginLocalsChild(dbg, base_ptr, cur_scp);
			ImGui::SameLine();


			ImGui::BeginChild("mem", ImVec2(500, 400));

			if (ImGui::BeginCombo("show type", mem_wnd_items[mem_wnd_show_type]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(mem_wnd_items); i++)
					if (ImGui::Selectable(mem_wnd_items[i]))
						mem_wnd_show_type = i;
				ImGui::EndCombo();
			}
			//ImGui::ListBox("type: ", &mem_wnd_show_type, mem_wnd_items, IM_ARRAYSIZE(mem_wnd_items));
			ImGui::InputInt("address: ", &mem_wnd_offset);
			char type_sz = BuiltinTypeSize((enum_type2)(mem_wnd_show_type + TYPE_U8));
			std::string mem_val;
			for (int r = 0; r <= BASE_STACK_PTR_REG; r++)
			{
				mem_val = GetMemAddrString(dbg, mem_wnd_offset + r * 8, type_sz, 8, mem_wnd_show_type);

				std::string addr_name = WasmNumToString(&dbg, mem_wnd_offset + r * 8);

				int cur_addr = mem_wnd_offset + r;
				if (cur_addr == (BASE_STACK_PTR_REG))
					addr_name += "(base_stack)";
				else if (cur_addr == (STACK_PTR_REG))
					addr_name += "(top_stack)";

				ImGui::Text("%s: %s", addr_name.c_str(), mem_val.c_str());
			}
			base_ptr = WasmGetMemOffsetVal(&dbg, STACK_PTR_REG * 8);
			for (int r = 0; r < 6; r++)
			{
				int addr = base_ptr + r * 8;
				mem_val = GetMemAddrString(dbg, addr, type_sz, 8, mem_wnd_show_type);

				std::string addr_name = WasmNumToString(&dbg, mem_wnd_offset + r * 8);
				ImGui::Text("arg_reg[%d, &%d]: %s", r, addr, mem_val.c_str());
			}
			ImGui::Separator();
			ImGui::EndChild();

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImVec2 p0 = ImGui::GetCursorScreenPos();

			ImGui::BeginChild("col", ImVec2(500, 400));
			ImGui::Text("draw cols");
			ImGui::InputInt("sqr 1: ", &square_1_addr);
			ImGui::InputInt("sqr 2: ", &square_2_addr);
			
			ImGuiDrawSquare(dbg, square_1_addr, ImGui::GetColorU32(IM_COL32(0, 255, 0, 255)));
			ImGuiDrawSquare(dbg, square_2_addr, ImGui::GetColorU32(IM_COL32(0, 255, 0, 255)));
			ImGui::EndChild();

			// Rendering
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window);

			bool dummy_bool;
			bool was_one_time = bc->one_time_dbg_brk;

			wasm_bc* prev_bc = bc;
			if (IsKeyRepeat(dbg.data, GLFW_KEY_F10))
			{

				
				if (show_wasm)
				{
					dbg.break_type = DBG_BREAK_ON_NEXT_BC;
					bc->dont_dbg_brk = true;
					(bc + 1)->one_time_dbg_brk = true;
					
				}
				else
				{
					WasmBreakOnNextStmnt(&dbg, &dummy_bool);
					//dbg.break_type = DBG_BREAK_ON_DIFF_STAT;
				}
				dbg.next_stat_break_func = dbg.cur_func;

				bool was_brk = bc->dbg_brk;
				//bc->dbg_brk = false;
				bc->one_time_dbg_brk = false;
				bc->break_on_first_loop_bc = true;
				ExitDebugger(&cur_scp, &bc);
				if ((bc->type == WASM_INST_DBG_BREAK || dbg.break_type == DBG_BREAK_ON_NEXT_BC) && !was_one_time && !was_brk)
					bc++;

				can_execute = true;
			}
			else if (IsKeyDown((void *)&dbg, _KEY_F11))
			{
				//bc->dbg_brk = false;
				block_linked** cur_block = &cur;
				wasm_bc* cur = *dbg.cur_bc;
				wasm_bc* end = dbg.bcs.begin() + dbg.cur_st->end;
				while (cur <= end && (cur->type != WASM_INST_CALL && cur->type != WASM_INST_INDIRECT_CALL))
				{
					cur++;
				}
				// found call
				if (cur <= end)
				{
					func_decl* first_func = dbg.wasm_state->funcs[cur->i];

					stmnt_dbg* first_st = &first_func->wasm_stmnts[0];

					wasm_bc* first_bc = dbg.bcs.begin() + first_st->start;
					first_bc->one_time_dbg_brk = true;

				}
				else
				{
					WasmBreakOnNextStmnt(&dbg, &dummy_bool);
				}

				ExitDebugger(&cur_scp, &bc);
				can_execute = true;
			}
			else if (IsKeyRepeat(dbg.data, GLFW_KEY_F5))
			{
				//bc->dbg_brk = false;
				ExitDebugger(&cur_scp, &bc);
				dbg.break_type = DBG_NO_BREAK;
				can_execute = true;

			}
			ClearKeys(dbg.data);
		}
		else
		{
			if (!res)
			{
				bc->dont_dbg_brk = false;
				idx++;
				bc++;
			}
		}

	}
	EndTimer(&tm);
	time = GetTimerMSFloat(&tm);
	printf("wasm time: %.3f\n", time);
}
void RunDbgFunc(lang_state* lang_stat, std::string func, long long* args, int total_args)
{


	int mem_size = BUFFER_MEM_MAX;
	auto buffer = (unsigned char*)AllocMiscData(lang_stat, mem_size);
	lang_stat->winterp->dbg->mem_size = mem_size;
	*(int*)&buffer[MEM_PTR_CUR_ADDR] = 21000;
	*(int*)&buffer[MEM_PTR_MAX_ADDR] = 0;

	ASSERT(lang_stat->data_sect.size() < DATA_SECT_MAX);
	memcpy(&buffer[DATA_SECT_OFFSET], lang_stat->data_sect.begin(), lang_stat->data_sect.size());
	memcpy(&buffer[GLOBALS_OFFSET], lang_stat->globals_sect.begin(), lang_stat->globals_sect.size());
	WasmInterpRun(lang_stat->winterp, buffer, mem_size, func.c_str(), args, total_args);
	__lang_globals.free(__lang_globals.data, buffer);

}
void ImGuiPrintVar(char* buffer_in, dbg_state& dbg, decl2* d, int base_ptr, char ptr_decl)
{
	char buffer[64];
	//ImGui::Text("offset: %d", d->offset);
	//ImGui::SameLine();
	if (d->type.type == TYPE_STRUCT || d->type.type == TYPE_STRUCT_TYPE)
	{
		int offset = base_ptr;
		char ptr = ptr_decl;

		std::string name = d->name;
		std::string ptr_str = "";

		if (IS_FLAG_ON(d->flags, DECL_PTR_HAS_LEN))
		{
			if(ptr_decl > 0)
			{
				ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow;
				int len_offset = base_ptr + d->len_for_ptr_offset;
				int len = *(int*)&dbg.mem_buffer[len_offset];
				if (len > 1000)
				{
					ImGui::Text("more than 1000 items");
					return;
				}
				int addr = *(int*)&dbg.mem_buffer[base_ptr];
				char prev_ptr = d->type.ptr;
				d->type.ptr = ptr_decl;
				d->type.ptr--;
				int tp_sz = GetTypeSize(&d->type);
				d->type.ptr++;
				d->flags &= ~DECL_PTR_HAS_LEN;
				for (int i = 0; i < len; i++)
				{
					int cur_addr = addr + i * tp_sz;
					snprintf(buffer, 64, "[%d]##%d", i, cur_addr);
					ImGui::Text("(%d)", cur_addr);
					ImGui::SameLine();
					/*
					if (d->type.type == TYPE_STRUCT && ptr_decl > 1)
					{
						if (ImGui::TreeNodeEx(buffer, flag))
						{
							ImGuiPrintScopeVars(buffer, dbg, d->type.strct->scp, cur_addr);
							ImGui::TreePop();  // This is required at the end of the if block
						}
					}
					else
					*/

					ImGuiPrintVar(buffer, dbg, d, cur_addr, ptr_decl - 1);

				}
				d->flags |= DECL_PTR_HAS_LEN;
				d->type.ptr = prev_ptr;
			}
		}
		else
		{
			/*
			if(ptr_str.empty())
				snprintf(buffer, 64, "%s##%d", name.c_str(), base_ptr);
			else
				snprintf(buffer, 64, "%s(%s)##%d", name.c_str(), ptr_str.c_str(), base_ptr);
				*/

			if (ptr_decl == 0)
			{
				snprintf(buffer, 64, "%s(&%d)##%d",  name.c_str(), base_ptr, base_ptr);
				ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx(buffer, flag))
				{
					ImGuiPrintScopeVars(buffer, dbg, d->type.strct->scp, offset);
					ImGui::TreePop();  // This is required at the end of the if block
				}
			}
			else
			{
				if (base_ptr > dbg.mem_size)
				{
					ImGui::Text("%s higher than max: %d", d->name.c_str(), dbg.mem_size);
					return;
				}
				int addr = *(int*)&dbg.mem_buffer[base_ptr];
				snprintf(buffer, 64, "%d->%d", base_ptr, addr);
				ImGui::Text(buffer);
				ImGui::SameLine();
				ImGuiPrintVar(buffer, dbg, d, addr, ptr_decl - 1);
			}

		}
		//ImGui::Text("%s", d->name.c_str());
	}
	else if (d->type.type == TYPE_ENUM)
	{
		int offset = base_ptr;
		offset = *(int*)&dbg.mem_buffer[offset];

		scope* scp = d->type.from_enum->type.scp;
		if (!scp)
		{
			ImGui::Text("Error: no scp for this enum(but var val:%d)", offset);
		}
		else if (offset < scp->vars.size())
		{
			decl2* e_decl = scp->vars[offset];
			ImGui::Text("%s(%d): %s", d->name.c_str(), offset, e_decl->name.c_str());
		}
		
	}
	else if (d->type.type == TYPE_STR_LIT)
	{
		int offset = base_ptr;
		int addr = offset;
		offset = *(int*)&dbg.mem_buffer[offset];
		auto ptr = (char*)&dbg.mem_buffer[offset];
		if (ptr == nullptr)
		{
			ptr = "";
		}
		ImGui::Text("%s(%d, &%d): %s", d->name.c_str(), offset, addr, ptr);
	}
	else if (d->type.type == TYPE_VECTOR || d->type.type == TYPE_VECTOR_TYPE)
	{
		snprintf(buffer, 64, "%s(&%d)##%d",  d->name.c_str(), base_ptr, base_ptr);
		ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow;
		
		if (ImGui::TreeNodeEx(buffer, flag))
		{
			for (int i = 0; i < 4; i++)
			{
				int final_ptr = base_ptr + i * 4;
				float f = *(float*)&dbg.mem_buffer[final_ptr];
				ImGui::Text("%.3f", f);
			}
			ImGui::TreePop();  // This is required at the end of the if block
		}
	}
	else if (d->type.type == TYPE_STATIC_ARRAY)
	{
		snprintf(buffer, 64, "%s(&%d)##%d",  d->name.c_str(), base_ptr, base_ptr);
		ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow;
		
		if (ImGui::TreeNodeEx(buffer, flag))
		{
			for (int i = 0; i < d->type.ar_size; i++)
			{
				type2 prev_type = d->type;
				d->type = *d->type.tp;
				int final_ptr = base_ptr + i * GetTypeSize(&d->type);
				snprintf(buffer, 64, "%s(&%d)##%d",  d->name.c_str(), final_ptr, final_ptr);
				ImGuiPrintVar(buffer, dbg, d, final_ptr, d->type.ptr);
				d->type = prev_type;
			}
			ImGui::TreePop();  // This is required at the end of the if block
		}
	}
	else if (d->type.type == TYPE_TEMPLATE || d->type.type == TYPE_FUNC || d->type.type == TYPE_IMPORT || d->type.type == TYPE_FUNC_TYPE || d->type.type == TYPE_OVERLOADED_FUNCS)
	{
		//ImGui::Text("template %s", d->name.c_str());
	}
	else
	{
		int offset = base_ptr;
		long long val = 0;
		print_num_type ptype = PRINT_INT;

		char ptr = d->type.ptr;
		while (ptr > 0)
		{
			offset = *(int*)&dbg.mem_buffer[offset];
			ptr--;
		}
		if (offset >= dbg.mem_size)
		{
			ImGui::Text("%s, error(greater than mem_buffer size)", d->name.c_str());
			return;
		}
		if (d->type.IsFloat())
		{
			ptype = PRINT_FLOAT;
			val = *(int*)&dbg.mem_buffer[offset];
		}
		else
		{
			if (d->name == "window")
				return;
			if (offset < 0)
			{
				ImGui::Text("%s less than zero", d->name.c_str());
				return;
			}
			switch (GetTypeSize(&d->type))
			{
			case 1:
			{
				val = *(char*)&dbg.mem_buffer[offset];
			}break;
			case 2:
			{
				val = *(short*)&dbg.mem_buffer[offset];
			}break;
			case 4:
			{
				val = *(int*)&dbg.mem_buffer[offset];
			}break;
			case 8:
			{
				val = *(long long*)&dbg.mem_buffer[offset];
			}break;
			default:
				ASSERT(false);
			}
		}

		std::string name = d->name;
		if (d->type.ptr > 0)
		{
			name += std::string("(&") + WasmNumToString(&dbg, offset)+")";
		}
		if (d->type.type == TYPE_CHAR)
		{
			ptype = PRINT_CHAR;
		}
		ImGui::Text("%s, %s, type: %s", name.c_str(), WasmNumToString(&dbg, val, -1, ptype).c_str(), TypeToString(d->type).c_str());
		//ImGui::Text("%s, %s", name.c_str(), WasmNumToString(&dbg, val, -1, ptype).c_str());
	}

}

void ImGuiPrintScopeVars(char *name, dbg_state &dbg, scope* cur_scp, int base_ptr)
{
	char buffer[64];
	// Call ImGui::TreeNodeEx() recursively to populate each level of children
	FOR_VEC(decl, cur_scp->vars)
	{
		decl2* d = *decl;
		if (d->type.type == TYPE_STRUCT_TYPE || d->type.type == TYPE_UNION_TYPE)
			continue;
		if (IS_FLAG_OFF(d->flags, DECL_FROM_USING))
			ImGuiPrintVar(name, dbg, d, base_ptr + d->offset, d->type.ptr);
	}
}
void WasmInterpPatchIrVal(ir_val* val, dbg_file_seriealize* file)
{
	unsigned char* start_f = ((unsigned char*)(file + 1)) + file->func_sect;
	unsigned char* start_vars = ((unsigned char*)(file + 1)) + file->vars_sect;
	unsigned char* start_type = ((unsigned char*)(file + 1)) + file->types_sect;

	if (val->type == IR_TYPE_DECL)
	{
		auto vdbg = (var_dbg*)(start_vars + val->i);
		decl2* d = vdbg->decl;
		ASSERT(d);
		val->decl = d;
		val->decl_offset = d->offset;
	}
}
void WasmInterpPatchIr(own_std::vector<ir_rep>* ir_ar, wasm_interp* winterp, dbg_file_seriealize* file)
{
	unsigned char* start_f = ((unsigned char*)(file + 1)) + file->func_sect;
	unsigned char* start_vars = ((unsigned char*)(file + 1)) + file->vars_sect;
	unsigned char* start_type = ((unsigned char*)(file + 1)) + file->types_sect;
	FOR_VEC(ir, (*ir_ar))
	{
		switch (ir->type)
		{
		case IR_STACK_BEGIN:
		{
			auto fdbg = (func_dbg*)(start_f + ir->num);
			ir->fdecl = fdbg->decl->type.fdecl;

		}break;
		case IR_CAST_F32_TO_INT:
		case IR_CAST_INT_TO_F32:
		case IR_CAST_INT_TO_INT:
		case IR_CMP_NE:
		case IR_CMP_LT:
		case IR_CMP_LE:
		case IR_CMP_GT:
		case IR_CMP_GE:
		case IR_CMP_EQ:
		{
			WasmInterpPatchIrVal(&ir->bin.lhs, file);
			WasmInterpPatchIrVal(&ir->bin.rhs, file);
		}break;
		case IR_RET:
		{
			WasmInterpPatchIrVal(&ir->ret.assign.to_assign, file);
			WasmInterpPatchIrVal(&ir->ret.assign.lhs, file);
			if(!ir->ret.assign.only_lhs)
				WasmInterpPatchIrVal(&ir->ret.assign.rhs, file);
		}break;
		case IR_ASSIGNMENT:
		{
			WasmInterpPatchIrVal(&ir->assign.to_assign, file);
			WasmInterpPatchIrVal(&ir->assign.lhs, file);
			if(!ir->assign.only_lhs)
				WasmInterpPatchIrVal(&ir->assign.rhs, file);
		}break;
		case IR_INDIRECT_CALL:
		{
			auto vdbg = (var_dbg*)(start_vars + ir->call.i);
			decl2* d = vdbg->decl;
			ASSERT(d);
			ir->decl = d;

		}break;
		case IR_CALL:
		{
			auto fdbg = (func_dbg*)(start_f + ir->call.i);
			ASSERT(fdbg->created);
			func_decl* fdecl = fdbg->decl->type.fdecl;
			if (IS_FLAG_ON(fdecl->flags, FUNC_DECL_IS_OUTSIDER))
				ir->call.outsider = winterp->outsiders[fdecl->name];

			ASSERT(fdecl);

			ir->call.fdecl = fdecl;
		}break;
		}
	}
}
std::string PrintScpPre(unsigned char* start, scope_dbg* s)
{

	std::string ret = "{";
	char buffer[512];
	snprintf(buffer, 512, "type: %d", s->type);
	ret += buffer;

	for (int i = 0; i < s->children_len; i++)
	{
		ret += PrintScpPre(start, (scope_dbg*)(start + s->children + i * sizeof(scope_dbg)));
		ret += ", ";
		//snprintf(buffer, 512,"")
	}
	if (s->children_len > 0)
	{
		ret.pop_back();
		ret.pop_back();
	}
	ret += " }";
	return ret;
}
void WasmInterpInit(wasm_interp* winterp, unsigned char* data, unsigned int len, lang_state* lang_stat)
{

	auto file = (dbg_file_seriealize*)data;
	data = (unsigned char*)(file + 1);

	unsigned char* code = data + file->code_sect;

	//int point_idx = file_name.find_last_of('.');
	//std::string data_file_name = file_name.substr(0, point_idx)+"_data.dbg";
	auto file_data_sect = (char*)data + file->data_sect;
	auto globals_sect = (char*)data + file->globals_sect;
	lang_stat->data_sect.insert(lang_stat->data_sect.begin(), file_data_sect, file_data_sect + file->data_sect_size);
	lang_stat->globals_sect.insert(lang_stat->globals_sect.begin(), globals_sect, globals_sect + file->globals_sect_size);

	lang_stat->files.clear();

	for (int i = 0; i < file->total_files; i++)
	{
		auto new_f = (unit_file*)AllocMiscData(lang_stat, sizeof(unit_file));

		auto cur_file = (file_dbg*)(data + file->files_sect + i * sizeof(file_dbg));

		std::string name = WasmInterpNameFromOffsetAndLen(data, file, &cur_file->name);

		int last_bar = name.find_last_of("\\/");
		new_f->path = name.substr(0, last_bar + 1);
		new_f->name = name.substr(last_bar + 1, -1);
		int read = 0;
		new_f->contents = ReadEntireFileLang((char*)name.c_str(), &read);
		new_f->funcs_scp = NewScope(lang_stat, nullptr);
		char* cur_str = new_f->contents;
		cur_file->fl = new_f;

		for (int j = 0; j < read; j++)
		{
			if (new_f->contents[j] == '\n')
			{
				*(char*)&new_f->contents[j] = 0;
				new_f->lines.emplace_back(cur_str);
				cur_str = &new_f->contents[j + 1];

			}
		}
		lang_stat->files.emplace_back(new_f);
	}
	own_std::vector<char *> strs;

	for (int f = 0; f < file->total_funcs; f++)
	{
		auto fdbg = (func_dbg*)(data + file->func_sect + f * sizeof(func_dbg));
		if (IS_FLAG_ON(fdbg->flags, FUNC_DECL_MACRO))
			continue;
		std::string name = WasmInterpNameFromOffsetAndLen(data, file, &fdbg->name);
		//printf("\n%d: func_name: %s", (int)strs.size(), name.c_str());
		strs.emplace_back(std_str_to_heap(lang_stat, &name));

		auto d = WasmInterpBuildFunc(data, winterp, lang_stat, file, fdbg);

	}

	lang_stat->code_sect.ar.start = data + file->x64_code_sect;
	lang_stat->code_sect.ar.count = file->x64_code_sect;
	lang_stat->type_sect.ar.count = file->x64_code_type_sect_size;

	auto scp_pre = (scope_dbg*)(data + file->scopes_sect);
	scope* root = WasmInterpBuildScopes(winterp, data, len, lang_stat, file, nullptr, scp_pre, false);
	root = WasmInterpBuildScopes(winterp, data, len, lang_stat, file, nullptr, scp_pre, true);
	std::string scp_pre_str = PrintScpPre(data + file->scopes_sect, scp_pre);

	//printf("\nscop 3:\n%s", root->Print(0).c_str());

	type2 dummy_type;
	FOR_VEC(pinfo, lang_stat->ptrs_have_len)
	{
		decl2 *d = FindIdentifier(pinfo->decl->len_for_ptr_name, pinfo->scp, &dummy_type);
		ASSERT(d);
		pinfo->decl->len_for_ptr = d;
		pinfo->decl->len_for_ptr_offset = d->offset;
	}
	for (int i = 0; i < file->total_funcs; i++)
	{
		auto fdbg = (func_dbg*)(data + file->func_sect + i * sizeof(func_dbg));
		if (IS_FLAG_ON(fdbg->flags, FUNC_DECL_IS_OUTSIDER))
		{
			//std::string name = WasmInterpNameFromOffsetAndLen(data, file, &fdbg->name);

		}
	}

	FOR_VEC(func, winterp->funcs)
	{
		func_decl* f = *func;
		//if(f->)
		own_std::vector<ir_rep>* ir_ar = (own_std::vector<ir_rep> *) & f->ir;
		WasmInterpPatchIr(ir_ar, winterp, file);

	}

	own_std::vector<RatedStuff<decl2 *, int>> funcs;
	FOR_VEC(fl_ptr, lang_stat->files)
	{
		funcs.clear();
		unit_file* fl = *fl_ptr;
		FOR_VEC(f_ptr, fl->funcs_scp->vars)
		{
			if ((*f_ptr)->type.type != TYPE_FUNC)
				continue;
			decl2 * d = (*f_ptr);
			RatedStuff<decl2 *, int> r;
			r.val = d->type.fdecl->scp->line_start;
			r.type = d;
			funcs.emplace_back(r);
			//funcs
		}
		SortRatedStuff(&funcs);

		int i = 0;
		FOR_VEC(f_ptr, fl->funcs_scp->vars)
		{
			*f_ptr = funcs[i].type;
			//funcs
			i++;
		}
	}


	int ptr = 0;
	int sect_code = code[ptr];
	ASSERT(sect_code == 0xa)
		ptr++;
	unsigned int consumed = 0;
	int sect_size = decodeSLEB128(&code[ptr], &consumed);
	ptr += consumed;

	int number_of_functions = decodeSLEB128(&code[ptr], &consumed);
	ptr += consumed;

	int i = 0;

	winterp->dbg = (dbg_state*)AllocMiscData(lang_stat, sizeof(dbg_state));
	dbg_state& dbg = *winterp->dbg;
	//dbg.mem_size = size;
	dbg.lang_stat = lang_stat;

	auto global_cmds = (command_info *)AllocMiscData(lang_stat, sizeof(command_info));
	dbg.global_cmd = global_cmds;

	auto print_command = (command_info *)AllocMiscData(lang_stat, sizeof(command_info));
	print_command->names.emplace_back("print");
	print_command->names.emplace_back("p");

	auto wasm_sub_cmd = (command_info *)AllocMiscData(lang_stat, sizeof(command_info));
	wasm_sub_cmd->names.emplace_back("wasm");

	auto wasm_sub_cmd_func = (command_info *)AllocMiscData(lang_stat, sizeof(command_info));
	//wasm_sub_cmd_func->func = WasmCmdPrintWasmFuncAutoComplete;
	wasm_sub_cmd_func->end = true;
	wasm_sub_cmd_func->names.emplace_back("func");

	auto wasm_sub_cmd_lines = (command_info *)AllocMiscData(lang_stat, sizeof(command_info));
	wasm_sub_cmd_lines->end = true;
	wasm_sub_cmd_lines->names.emplace_back("lines");

	wasm_sub_cmd->cmds.emplace_back(wasm_sub_cmd_func);
	wasm_sub_cmd->cmds.emplace_back(wasm_sub_cmd_lines);

	print_command->cmds.emplace_back(wasm_sub_cmd);


	global_cmds->cmds.emplace_back(print_command);


	//print_command_func->names.emplace_back("func");


	//dbg.wasm_state = wasm_state;

	own_std::vector<wasm_bc>& bcs = dbg.bcs;
	while (i < winterp->funcs.size())
	{
		func_decl* cur_f = winterp->funcs[i];
		if (IS_FLAG_ON(cur_f->flags, FUNC_DECL_IS_OUTSIDER | FUNC_DECL_MACRO | FUNC_DECL_INTRINSIC))
		{
			i++;
			continue;
		}
		int func_sz = decodeSLEB128(&code[ptr], &consumed);
		ASSERT(func_sz > 0);
		ptr += consumed;
		int next_func = ptr + func_sz;

		int local_entries = decodeSLEB128(&code[ptr], &consumed);
		ptr += consumed;
		// not handling entries at the moment
		if (local_entries > 0)
		{
			ASSERT(0);
		}

		int fi = ptr;

		cur_f->code_start_idx = bcs.size();

		stmnt_dbg* cur_st = &cur_f->wasm_stmnts[0];



		cur_f->wasm_code_sect_idx = bcs.size();
#ifdef WASM_DBG
		cur_f->wasm_stmnts.clear();
#endif


		while (ptr < next_func)
		{
			wasm_bc bc = {};

			int cur_ptr = (ptr - fi);
			bc.start_code = cur_ptr;
			unsigned int* int_op = (unsigned int*)&code[ptr];
#ifdef WASM_DBG
			if (*int_op == 0x67452301)
			{
				stmnt_dbg st;
				st.start = bcs.size();

				unsigned int* line = (unsigned int*)(int_op + 1);
				st.line = *line;
				cur_f->wasm_stmnts.emplace_back(st);
				ptr += 8;
				continue;

			}
			if (*int_op == 0x86543210)
			{
				stmnt_dbg &st = cur_f->wasm_stmnts.back();
				st.end = bcs.size();
				st.start_ir = *(int_op + 1);
				st.end_ir = *(int_op + 2);

				ASSERT(st.start_ir >= 0 && st.end_ir >= 0);

				ptr += 12;
				continue;

			}
#endif


			int op = code[ptr];
			ptr++;
			switch (op)
			{
			case 0x1:
			{
				bc.type = WASM_INST_NOP;
			}break;
			case 0x11:
			{
				bc.type = WASM_INST_INDIRECT_CALL;

				// type idx
				decodeSLEB128(&code[ptr], &consumed);
				ptr += consumed;
				// table idx idx
				decodeSLEB128(&code[ptr], &consumed);
				ptr += consumed;
			}break;
			case 0x10:
			{
				bc.type = WASM_INST_CALL;
				bc.i = decodeSLEB128(&code[ptr], &consumed);
				ptr += consumed;
			}break;
			case 0x6c:
			{
				bc.type = WASM_INST_I32_MUL;
			}break;
			case 0x47:
			{
				bc.type = WASM_INST_I32_NE;
			}break;
			case 0x4e:
			{
				bc.type = WASM_INST_I32_GE_S;
			}break;
			case 0x4f:
			{
				bc.type = WASM_INST_I32_GE_U;
			}break;
			case 0xd:
			{
				bc.type = WASM_INST_BREAK_IF;
				bc.i = decodeSLEB128(&code[ptr], &consumed);
				ptr += consumed;
			}break;
			case 0xc:
			{
				bc.type = WASM_INST_BREAK;
				bc.i = decodeSLEB128(&code[ptr], &consumed);
				ptr += consumed;
			}break;
			case 0x3:
			{
				bc.type = WASM_INST_LOOP;
				// block return type
				ptr++;
			}break;
			case 0xb:
			{
				bc.type = WASM_INST_END;
			}break;
			case 0x2:
			{
				bc.type = WASM_INST_BLOCK;
				// block return type
				ptr++;
			}break;
			case 0x43:
			{
				bc.type = WASM_INST_F32_CONST;
				//ptr++;
				unsigned int* int_ptr = (unsigned int*)&code[ptr];
				//*(int*)&bc.f32 = (*int_ptr << 24) | (((*int_ptr) & 0xff00) << 8) | (((*int_ptr) & 0xff0000) >> 8) | (((*int_ptr) >> 24));
				*(int*)&bc.f32 = *int_ptr;

				ptr += 4;
			}break;
			case 0x93:
			{
				bc.type = WASM_INST_F32_SUB;
			}break;
			case 0x92:
			{
				bc.type = WASM_INST_F32_ADD;
			}break;
			case 0x6a:
			{
				bc.type = WASM_INST_I32_ADD;
			}break;
			case 0x6f:
			{
				bc.type = WASM_INST_I32_REMAINDER_S;
			}break;
			case 0x70:
			{
				bc.type = WASM_INST_I32_REMAINDER_U;
			}break;
			case 0x6b:
			{
				bc.type = WASM_INST_I32_SUB;
			}break;
			case 0x3a:
			{
				bc.type = WASM_INST_I32_STORE8;
				// alignment
				ptr++;
				// offset
				ptr++;

			}break;
			case 0x38:
			{
				bc.type = WASM_INST_F32_STORE;
				// alignment
				ptr++;
				// offset
				ptr++;

			}break;
			case 0x36:
			{
				bc.type = WASM_INST_I32_STORE;
				// alignment
				ptr++;
				// offset
				ptr++;

			}break;
			case 0x37:
			{
				bc.type = WASM_INST_I64_STORE;
				// alignment
				ptr++;
				// offset
				ptr++;

			}break;
			case 0x29:
			{
				bc.type = WASM_INST_I64_LOAD;
				// alignment
				ptr++;
				// offset
				ptr++;

			}break;
			case 0xf:
			{
				bc.type = WASM_INST_RET;
			}break;
			case 0x2c:
			{
				bc.type = WASM_INST_I32_LOAD_8_S;
				// alignment
				ptr++;
				// offset
				ptr++;

			}break;
			case 0x2a:
			{
				bc.type = WASM_INST_F32_LOAD;
				// alignment
				ptr++;
				// offset
				ptr++;

			}break;
			case 0x28:
			{
				bc.type = WASM_INST_I32_LOAD;
				// alignment
				ptr++;
				// offset
				ptr++;

			}break;
			case 0x6d:
			{
				bc.type = WASM_INST_I32_DIV_S;

			}break;
			case 0x6e:
			{
				bc.type = WASM_INST_I32_DIV_U;

			}break;
			case 0x46:
			{
				bc.type = WASM_INST_I32_EQ;

			}break;
			case 0x72:
			{
				bc.type = WASM_INST_I32_OR;

			}break;
			case 0x71:
			{
				bc.type = WASM_INST_I32_AND;

			}break;
			case 0x4a:
			{
				bc.type = WASM_INST_I32_GT_S;

			}break;
			case 0x4b:
			{
				bc.type = WASM_INST_I32_GT_U;

			}break;
			case 0x4c:
			{
				bc.type = WASM_INST_I32_LE_S;

			}break;
			case 0x4d:
			{
				bc.type = WASM_INST_I32_LE_U;

			}break;
			case 0x48:
			{
				bc.type = WASM_INST_I32_LT_S;

			}break;
			case 0x49:
			{
				bc.type = WASM_INST_I32_LT_U;

			}break;
			case 0x5c:
			{
				bc.type = WASM_INST_F32_NE;

			}break;
			case 0x5b:
			{
				bc.type = WASM_INST_F32_EQ;

			}break;
			case 0x5f:
			{
				bc.type = WASM_INST_F32_LE;

			}break;
			case 0x5d:
			{
				bc.type = WASM_INST_F32_LT;

			}break;
			case 0x95:
			{
				bc.type = WASM_INST_F32_DIV;

			}break;
			case 0xb2:
			{
				bc.type = WASM_INST_CAST_S32_2_F32;

			}break;
			case 0xa8:
			{
				bc.type = WASM_INST_CAST_F32_2_S32;

			}break;
			case 0xb9:
			{
				bc.type = WASM_INST_CAST_S64_2_F32;

			}break;
			case 0xb3:
			{
				bc.type = WASM_INST_CAST_U32_2_F32;
			}break;
			case 0xba:
			{
				bc.type = WASM_INST_CAST_U32_2_F32;

			}break;
			case 0x60:
			{
				bc.type = WASM_INST_F32_GT;

			}break;
			case 0x94:
			{
				bc.type = WASM_INST_F32_MUL;

			}break;
			case 0x5e:
			{
				bc.type = WASM_INST_F32_GE;

			}break;
			case 0xff:
			{
				bc.type = WASM_INST_DBG_BREAK;
			}break;
			case 0x41:
			{
				bc.type = WASM_INST_I32_CONST;
				bc.i = decodeSLEB128(&code[ptr], &consumed);
				ptr += consumed;

			}break;
			default:
				ASSERT(0);
			}
			// decrementing because the statement counts only the end of the instructions, and not the beginning of the next one
			cur_ptr = (ptr - fi) - 1;
			bc.end_code = cur_ptr + 1;
			bcs.emplace_back(bc);

		}

		own_std::vector<ir_rep>* ir_ar = (own_std::vector<ir_rep> *) & cur_f->ir;
		ir_rep* cur_ir = ir_ar->begin();
		for (int i = cur_f->wasm_code_sect_idx; i < bcs.size(); i++)
		{
			while (cur_ir->start == cur_ir->end && cur_ir < ir_ar->end())
			{
				cur_ir->start = i;
				cur_ir->end = i;
				cur_ir++;
			}
			wasm_bc* cur_bc = bcs.begin() + i;
			if (cur_ir->start == cur_bc->start_code)
				cur_ir->start = i;
			if (cur_ir->end == cur_bc->end_code)
			{
				cur_ir->end = max(cur_ir->start, max(i - 1, 0));
				cur_ir++;
			}
		}

		ptr = next_func;

		//funcs[i] = wf;
		i++;
	}

	//wasm_func *wf = &funcs[func_idx];

	/*
	own_std::vector<func_decl*> &func_stack = dbg.func_stack;
	FOR_VEC(func, winterp->funcs)
	{
		func_decl* f = *func;
		if (f->name == func_start_name)
		{
			func_stack.emplace_back(f);
			break;
		}
	}
	ASSERT(func_stack.size() > 0);
	func_decl* cur_func = func_stack.back();
	*/

	block_linked* cur = nullptr;
	FOR_VEC(bc, bcs)
	{
		switch (bc->type)
		{
		case WASM_INST_LOOP:
		case WASM_INST_BLOCK:
		{
			cur = NewBlock(cur);
			cur->wbc = bc;
		}break;
		case WASM_INST_END:
		{
			if (cur)
			{
				FreeBlock(cur);
				cur->wbc->block_end = bc;
				cur = cur->parent;
			}

		}break;
		}
	}
	__lang_globals.cur_block = 0;
	cur = nullptr;
	FOR_VEC(bc, bcs)
	{
		switch (bc->type)
		{
		case WASM_INST_LOOP:
		case WASM_INST_BLOCK:
		{
			cur = NewBlock(cur);
			cur->wbc = bc;
		}break;
		case WASM_INST_BREAK_IF:
		case WASM_INST_BREAK:
		{
			int i = 0;
			wasm_bc* label;
			block_linked *aux_block = cur;
			while (i < bc->i)
			{
				aux_block = aux_block->parent;
				i++;
			}
			if (aux_block)
			{
				if (aux_block->wbc->type == WASM_INST_LOOP && bc->type == WASM_INST_BREAK)
				{
					bc->jmps_to = aux_block->wbc;
				}
				else
				{
					bc->jmps_to = aux_block->wbc->block_end;
				}
			}
		}break;
		case WASM_INST_END:
		{
			if (cur)
			{
				FreeBlock(cur);
				//cur->wbc->block_end = bc;
				cur = cur->parent;
			}

		}break;
		}
	}
}
void WasmIrInterp(dbg_state* dbg, GLFWwindow *window, func_decl *start)
{
	auto ir_ar = (own_std::vector<ir_rep>*)&start->ir;
	ir_rep* ir = ir_ar->begin() + start->ir_stack_begin_idx;
	ir_rep* start_ir = ir_ar->begin();

	scope* cur_scp = nullptr;
	stmnt_dbg* cur_st;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	int irs_to_show = 50;
	bool show_ir = false;
	bool activate_dbg_break_on_next_ir = false;

	int mem_wnd_offset = 0;
	int mem_wnd_show_type = 0;
	char* mem_wnd_items[] = {"u8", "s8", "u16", "s16", "u32", "s32", "u64", "s64", "f32", "char"};
	int total_items = IM_ARRAYSIZE(mem_wnd_items);

	int total_irs = 0;

	func_decl* cur_func = nullptr;

	while (dbg->func_stack.size() > 0)
	{
		//dbg->cur_st = GetStmntBasedOnOffsetIr(&dbg->cur_func->wasm_stmnts,ir->idx);
		if(cur_func != dbg->cur_func)
		{
			ir_ar = (own_std::vector<ir_rep>*) & dbg->cur_func->ir;
			start_ir = ir_ar->begin();
			cur_func = dbg->cur_func;
		}
		if (dbg->break_type == DBG_BREAK_ON_DIFF_IR && dbg->prev_break_ir != ir && IsExecutableIr(ir))
			ir->one_dbg_break = true;
		if (dbg->break_type == DBG_BREAK_ON_DIFF_IR_BUT_SAME_FUNC && dbg->cur_func == dbg->next_stat_break_func && dbg->prev_break_ir != ir && IsExecutableIr(ir))
			ir->one_dbg_break = true;
		/*
		if (ir->type == IR_DBG_BREAK || ir->dbg_break || ir->one_dbg_break)
		{
			if (!cur_scp)
			{
				cur_st = GetStmntBasedOnOffsetIr(&dbg->cur_func->wasm_stmnts,ir->idx);
				if (cur_st)
				{
					cur_scp = FindScpWithLine(dbg->cur_func, cur_st->line);
					auto a = 0;
					dbg->cur_st = cur_st;
				}
			}

			glfwPollEvents();
			if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
			{
				ImGui_ImplGlfw_Sleep(10);
				continue;
			}

			//ImGui::ImGuiContext& g = *ImGui::GImGui;
			//if(g.WithinFrameScope)
			if(ImGui::WithinFrame())
				ImGui::Render();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Selectable("show ir", &show_ir);
			
			ImGui::BeginChild("code", ImVec2(500, 300));
			if (cur_st)
			{
				ir_rep* aux_ir =  ir_ar->begin() + cur_st->start_ir;
				stmnt_dbg* next_st = cur_st + 1;
				stmnt_dbg* aux_cur_st = cur_st;
				bool first_stat = true;
				int max_ir = min(irs_to_show, ir_ar->size());
				ir_rep* end = ir_ar->end();
				for (; aux_ir < end; aux_ir++)
				{
					AdvanceIrToNext(&aux_ir);
					if (aux_ir >= end)
						break;
					bool new_stat = false;
					if (aux_ir->idx >= next_st->start_ir)
					{
						aux_cur_st++;
						next_st++;
						new_stat = true;
					}
					if ((new_stat || first_stat) && aux_cur_st->line != 0)
					{
						if(first_stat)
							ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "%d: %s", aux_cur_st->line, GetFileLn(dbg->lang_stat, aux_cur_st->line - 1, dbg->cur_func->from_file));
						else
							ImGui::Text("%d: %s", aux_cur_st->line, GetFileLn(dbg->lang_stat, aux_cur_st->line - 1, dbg->cur_func->from_file));
						first_stat = false;
					}
					if (show_ir)
					{
						if(ir == aux_ir)
							ImGui::TextColored(ImVec4(0.8, 0.5, 0.5, 1.0), "\t%s", WasmIrToString(dbg, aux_ir).c_str());
						else
							ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1.0), "\t%s", WasmIrToString(dbg, aux_ir).c_str());
					}
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();
			SHowMemWindow(*dbg, (char **)&mem_wnd_items, mem_wnd_show_type, mem_wnd_offset, total_items);

			int base_ptr = WasmGetRegVal(dbg, BASE_STACK_PTR_REG);
			BeginLocalsChild(*dbg, base_ptr, cur_scp);
			if (IsKeyRepeat(dbg->data, GLFW_KEY_F10))
			{
				if (show_ir)
				{
					cur_scp = nullptr;
					dbg->prev_break_ir = ir;
					dbg->break_type = DBG_BREAK_ON_DIFF_IR_BUT_SAME_FUNC;
					dbg->next_stat_break_func = dbg->cur_func;
					if (!ir->one_dbg_break && !ir->dbg_break)
					{
						ir++;
					}
					else
					{
						//ir_rep* next_ir = ir + 1;
						//AdvanceIrToNext(&next_ir);
						//next_ir->one_dbg_break = true;

						if (ir->dbg_break)
							activate_dbg_break_on_next_ir = true;
					}
					ir->one_dbg_break = false;
				}
			}


			auto  a = 0;

			// Rendering
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window);
			ClearKeys(dbg->data);
			//ir++;
		}
		*/
		//else
		{
			if (activate_dbg_break_on_next_ir)
				ir->dbg_break = true;
			bool res = IrLogic2(dbg, &ir, start_ir);
			ir++;
		}
	}
}
#endif
func_decl* WasmInterpFindFunc(wasm_interp* winterp, std::string func_name)
{
	FOR_VEC(func, winterp->funcs)
	{
		func_decl* f = *func;
		if (f->name == func_name)
		{
			return f;
		}
	}
	return nullptr;
}



std::string WasmCmdPrintWasmFuncAutoComplete(dbg_state* dbg, command_info_args *info)
{
	std::string ret = "";

	own_std::vector<std::string> names_found;
		
	FOR_VEC(func, dbg->lang_stat->winterp->funcs)
	{
		std::string n = (*func)->name;
		bool is_equal = true;
		for (int j = 0; j < info->incomplete_str.size(); j++)
		{
			if (info->incomplete_str[j] != n[j])
			{
				is_equal = false;
				break;
			}
		}
		if (is_equal)
		{
			names_found.emplace_back(n);
		}
	}
	if (names_found.size() == 1)
		return names_found[0];

	int shortest_str_idx = -1;
	int shortest_str_count = 60000;

	int second_shortest_str_idx = -1;
	
	int i = 0;
	FOR_VEC(str, names_found)
	{
		if (str->size() <= shortest_str_count)
		{
			second_shortest_str_idx = shortest_str_idx;
			shortest_str_idx = i;
			shortest_str_count = str->size();
		}
		i++;
	}

	std::string shortest_str = names_found[shortest_str_idx];
	std::string second_shortest_str = names_found[second_shortest_str_idx];

	int j = 0;
	for (; j < shortest_str.size(); j++)
	{
		if (shortest_str[j] != second_shortest_str[j])
		{
			break;
		}
	}

	ret = second_shortest_str.substr(0, j);

	return ret;
}




void WasmPushNameIntoArray(own_std::vector<unsigned char>* out, std::string name)
{
	own_std::vector<unsigned char> uleb;
	int name_len = name.size();
	uleb.clear();
	encodeSLEB128(&uleb, name_len);
	out->insert(out->end(), uleb.begin(), uleb.end());
	unsigned char* name_ptr = (unsigned char*)name.data();
	out->insert(out->end(), name_ptr, name_ptr + name_len);

}
void WasmInsertSectSizeAndType(own_std::vector<unsigned char>* out, char type)
{
	own_std::vector<unsigned char> uleb;
	uleb.clear();
	//encodeSLEB128(&uleb, out->size());
	GenUleb128(&uleb, out->size());

	// first inserting the size of the sect and then the type
	// so that, in memory, the type will come fircs
	out->insert(out->begin(), uleb.begin(), uleb.end());
	unsigned char sect_type = type;
	out->insert(0, sect_type);
}
void WasmAppendFunc(own_std::vector<unsigned char> &type_sect, func_decl*fdecl)
{
	// func type
	type_sect.emplace_back(0x60);

	int num_of_args = 0;//fdecl->args.size();

	type_sect.emplace_back(num_of_args);
	/*
	FOR_VEC(arg, fdecl->args)
	{
		decl2* a = *arg;
		// for now we pushing all args as int
		type_sect.emplace_back(0x7f);
	}
	*/
	// no return type
	type_sect.emplace_back(0);
	/*
	if (fdecl->ret_type.type != TYPE_VOID)
	{
		// only one return type
		type_sect.emplace_back(0x01);
		// an int
		type_sect.emplace_back(0x7f);
	}
	*/
}

void PreX64ImmToReg(own_std::vector<byte_code>& ret, long long imm, char imm_sz, short reg, char reg_sz)
{
	EmplaceLeaInst2(reg, reg, imm, reg_sz, ret);
}
void FromIrValToBytecodeReg(ir_val *val, byte_code::operand*o) 
{
	o->reg = val->reg;
	o->reg_sz = val->reg_sz;
}

void PreX64Deref(lang_state* lang_stat, short reg, char deref, own_std::vector<byte_code>& ret)
{
	byte_code bc;
	while (deref > 0)
	{
		bc.type = MOV_M;
		bc.bin.lhs.reg = reg;
		bc.bin.lhs.reg_sz = 8;
		bc.bin.rhs.reg = reg;
		bc.bin.rhs.reg_sz = 8;
		ret.emplace_back(bc);
		deref--;
	}
}

void GenX64RegToReg(own_std::vector<byte_code>& ret, short reg, char reg_sz, short reg_dst, char reg_sz_dst, byte_code_enum inst)
{
	byte_code bc;
	bc.type = inst;
	bc.bin.lhs.reg = reg;
	bc.bin.lhs.reg_sz = reg_sz;
	bc.bin.rhs.reg = reg_dst;
	bc.bin.rhs.reg_sz = reg_sz_dst;
	ret.emplace_back(bc);
}
void GenX64ImmToMem(own_std::vector<byte_code>& ret, short reg, int mem_offset, char reg_sz, int imm, byte_code_enum inst)
{
	byte_code bc;
	bc.type = inst;
	bc.bin.lhs.reg = reg;
	bc.bin.lhs.voffset = mem_offset;
	bc.bin.lhs.reg_sz = reg_sz;
	bc.bin.rhs.i = imm;
	ret.emplace_back(bc);
}
void GenX64ImmToReg(own_std::vector<byte_code>& ret, short reg, char reg_sz, int imm, byte_code_enum inst)
{
	byte_code bc;
	bc.type = inst;
	bc.bin.lhs.reg = reg;
	bc.bin.lhs.reg_sz = reg_sz;
	bc.bin.rhs.i = imm;
	ret.emplace_back(bc);
}

void GenX64MemToReg(own_std::vector<byte_code>& ret, short reg, char reg_sz, int mem_offset, short mem_reg, byte_code_enum inst)
{
	byte_code bc;
	bc.type = inst;
	bc.bin.lhs.reg = reg;
	bc.bin.lhs.reg_sz = reg_sz;
	bc.bin.rhs.reg = mem_reg;
	bc.bin.rhs.reg_sz = reg_sz;
	bc.bin.rhs.voffset = mem_offset;
	ASSERT(reg_sz> 0)
	ret.emplace_back(bc);
}

void GenX64RegToMem(own_std::vector<byte_code>& ret, short reg, char reg_sz, int mem_offset, short mem_reg, byte_code_enum inst)
{
	ASSERT(reg_sz != 0);
	byte_code bc;
	bc.type = inst;
	bc.bin.lhs.reg = mem_reg;
	bc.bin.lhs.voffset = mem_offset;
	bc.bin.lhs.reg_sz = reg_sz;
	bc.bin.rhs.reg = reg;
	bc.bin.rhs.reg_sz = reg_sz;
	ret.emplace_back(bc);
}
void GenX64RegToReg(lang_state* lang_stat, own_std::vector<byte_code>& ret, short reg_dst, char reg_sz, short reg_src, byte_code_enum inst)
{
	byte_code bc;
	bc.type = inst;
	bc.bin.lhs.reg = reg_dst;
	bc.bin.lhs.reg_sz = reg_sz;
	bc.bin.rhs.reg = reg_src;
	bc.bin.rhs.reg_sz = reg_sz;
	ret.emplace_back(bc);
}
void GenX64DeclWithDeref(lang_state *lang_stat, own_std::vector<byte_code>& ret, short reg, char reg_sz, int var_offset, char deref)
{
	byte_code bc;
	bc.type = MOV_M;
	bc.bin.rhs.reg = PRE_X64_RSP_REG;
	bc.bin.rhs.voffset = var_offset;
	bc.bin.rhs.reg_sz = 8;
	bc.bin.lhs.reg = reg;
	bc.bin.lhs.reg_sz = 8;
	ret.emplace_back(bc);

	PreX64Deref(lang_stat, reg, deref, ret);
	deref--;
}
/*
int GenX64DeclAndRegOperationToReg(lang_state *lang_stat, own_std::vector<byte_code>& ret, ir_rep * ir, byte_code_enum mem_reg_inst, byte_code_enum reg_reg_inst, short reg_dst, char reg_sz, char deref)
{
	byte_code bc;
	char reg = 0;
	// lea
	if (ir->bin.lhs.deref == 0)
	{
		reg = reg_dst;
		char reg_sz = reg_sz;
		bc.type = INST_LEA;
		bc.bin.rhs.lea.reg_base = PRE_X64_RSP_REG;
		bc.bin.rhs.lea.offset = ir->bin.lhs.decl->offset;
		bc.bin.rhs.lea.reg_dst = reg;
		bc.bin.rhs.lea.size = reg_sz;
		
		ret.emplace_back(bc);

		//GenX64RegToMem(ret, reg, reg_sz, mem_offset, short mem_reg, byte_code_enum inst)
		GenX64ImmToReg(ret, reg, reg_sz, ir->bin.rhs.i, reg_int_inst);
		//FreeSpecificReg(lang_stat, reg);
	}
	else if (ir->bin.lhs.deref == 1)
	{
		GenX64MemToReg(ret, reg, reg_sz, ir->assign.to_assign.decl->offset, PRE_X64_RSP_REG, mem_reg_inst);
		GenX64ImmToReg(ret, reg, reg_sz, ir->assign.rhs.i, reg_int_inst);
	}
	else
	{
		reg = reg_dst;
		char reg_sz = reg_sz;
		GenX64DeclWithDeref(lang_stat, ret, reg, reg_sz, ir->bin.lhs.decl->offset, ir->ret.assign.lhs.deref - 1);
		GenX64ImmToReg(ret, reg, reg_sz, ir->bin.rhs.i, reg_int_inst);
		//FreeSpecificReg(lang_stat, reg);
	}

	return reg;
}
*/
int GenX64DeclAndIntOperationToReg(lang_state *lang_stat, own_std::vector<byte_code>& ret, ir_rep * ir, byte_code_enum mem_int_inst, byte_code_enum mem_reg_inst, byte_code_enum reg_int_inst, short reg_dst, char reg_sz, char deref)
{
	byte_code bc;
	char reg = 0;
	// lea
	if (ir->bin.lhs.deref == 0)
	{
		reg = reg_dst;
		//char reg_sz = reg_sz;
		bc.type = INST_LEA;
		bc.bin.rhs.lea.reg_base = PRE_X64_RSP_REG;
		bc.bin.rhs.lea.offset = ir->bin.lhs.decl->offset;
		bc.bin.rhs.lea.reg_dst = reg;
		bc.bin.rhs.lea.size = reg_sz;
		
		ret.emplace_back(bc);

		//GenX64RegToMem(ret, reg, reg_sz, mem_offset, short mem_reg, byte_code_enum inst)
		GenX64ImmToReg(ret, reg, reg_sz, ir->bin.rhs.i, reg_int_inst);
		//FreeSpecificReg(lang_stat, reg);
	}
	else if (ir->bin.lhs.deref == 1)
	{
		GenX64MemToReg(ret, reg, reg_sz, ir->assign.to_assign.decl->offset, PRE_X64_RSP_REG, mem_reg_inst);
		GenX64ImmToReg(ret, reg, reg_sz, ir->assign.rhs.i, reg_int_inst);
	}
	else
	{
		reg = reg_dst;
		//char reg_sz = reg_sz;
		GenX64DeclWithDeref(lang_stat, ret, reg, reg_sz, ir->bin.lhs.decl->offset, ir->ret.assign.lhs.deref - 1);
		GenX64ImmToReg(ret, reg, reg_sz, ir->bin.rhs.i, reg_int_inst);
		//FreeSpecificReg(lang_stat, reg);
	}

	return reg;
}
int GenX64DeclAndIntOperation(lang_state *lang_stat, own_std::vector<byte_code>& ret, ir_rep * ir, byte_code_enum mem_int_inst, byte_code_enum reg_int_inst)
{
	byte_code bc;
	char reg = 0;
	// lea
	if (ir->bin.lhs.deref == 0)
	{
		reg = AllocReg(lang_stat);
		char reg_sz = ir->bin.lhs.reg_sz;
		bc.type = INST_LEA;
		bc.bin.rhs.lea.reg_base = PRE_X64_RSP_REG;
		bc.bin.rhs.lea.offset = ir->bin.lhs.decl->offset;
		bc.bin.rhs.lea.reg_dst = reg;
		bc.bin.rhs.lea.size = reg_sz;
		
		ret.emplace_back(bc);

		//GenX64RegToMem(ret, reg, reg_sz, mem_offset, short mem_reg, byte_code_enum inst)
		GenX64ImmToReg(ret, reg, reg_sz, ir->bin.rhs.i, reg_int_inst);
		FreeSpecificReg(lang_stat, reg);
	}
	else if (ir->bin.lhs.deref == 1)
	{
		bc.type = mem_int_inst;
		bc.bin.lhs.reg = PRE_X64_RSP_REG;
		bc.bin.lhs.voffset = ir->bin.lhs.decl->offset;
		bc.bin.lhs.reg_sz = ir->bin.lhs.reg_sz;
		bc.bin.rhs.i = ir->bin.rhs.i;
		ret.emplace_back(bc);
	}
	else
	{
		reg = AllocReg(lang_stat);
		char reg_sz = ir->bin.lhs.reg_sz;
		GenX64DeclWithDeref(lang_stat, ret, reg, reg_sz, ir->bin.lhs.decl->offset, ir->ret.assign.lhs.deref - 1);
		GenX64ImmToReg(ret, reg, reg_sz, ir->bin.rhs.i, reg_int_inst);
		FreeSpecificReg(lang_stat, reg);
	}

	return reg;
}
void GenX64RetGroup(lang_state *lang_stat, int stack_size, own_std::vector<byte_code>& ret)
{

	byte_code bc;
	GenX64ImmToReg(ret, PRE_X64_RSP_REG, 8, stack_size, ADD_I_2_R);
	bc.type = RET;
	ret.emplace_back(bc);
}

void GenX64AutomaticAddress(lang_state* lang_stat, own_std::vector<byte_code>& ret, char deref, short* reg, int* voffset, char reg_sz, bool is_float, bool address, bool is_packed_float, short reg_dst = -1)
{
	byte_code bc = {};
	if (!address)
	{
		if (is_float)
		{
			short sse_reg = reg_dst;
			if (reg_dst == -1)
			{
				reg_dst = 0;
				sse_reg = AllocFloatReg(lang_stat);
			}
			else
				AllocSpecificFloatReg(lang_stat, sse_reg);
			
			if(is_packed_float)
				bc.type = MOV_M_2_PCKD_SSE;
			else
				bc.type = MOV_M_2_SSE;
			bc.bin.lhs.reg = sse_reg;
			bc.bin.rhs.reg = *reg;
			bc.bin.rhs.reg_sz = 8;
			bc.bin.rhs.voffset = *voffset;
			*reg = sse_reg;
		}
		else
		{
			if (reg_dst == -1)
			{
				if (*reg == PRE_X64_RSP_REG)
				{
					reg_dst = AUX_DECL_REG;
				}
				else
					reg_dst = *reg;
			}
			else
				AllocSpecificReg(lang_stat, reg_dst);

			bc.type = MOV_M;
			bc.bin.lhs.reg = reg_dst;
			bc.bin.lhs.reg_sz = 8;
			bc.bin.rhs.reg = *reg;
			bc.bin.rhs.reg_sz = 8;
			bc.bin.rhs.voffset = *voffset;
			*reg = reg_dst;
		}
		ret.emplace_back(bc);
		*voffset = 0;
	}
}

void GenX64AutomaticRegDeref(lang_state* lang_stat, own_std::vector<byte_code>& ret, char deref, short* reg, char reg_sz, bool is_float, bool address, bool is_packed_float, short reg_dst = -1)
{
	int offset = 0;
	while (deref >= 0)
	{
		if (deref > 0)
		{
			GenX64AutomaticAddress(lang_stat, ret, deref, reg, &offset, reg_sz, false, false, false, reg_dst);
		}
		else
		{
			 if(!address)
				GenX64AutomaticAddress(lang_stat, ret, deref, reg, &offset, reg_sz, is_float, address, is_packed_float, reg_dst);
			 else
				GenX64AutomaticAddress(lang_stat, ret, deref, reg, &offset, reg_sz, false, false, false, reg_dst);
		}

		deref--;
	}
	/*
	byte_code bc;
	if (deref > 1)
	{
		PreX64Deref(lang_stat, *reg, deref, ret);
	}
	if(deref >= 0)
		GenX64AutomaticAddress(lang_stat, ret, deref, reg, &offset, reg_sz, is_float, address, reg_dst);
	*/
}
void GenX64AutomaticDeclDeref(lang_state* lang_stat, own_std::vector<byte_code>& ret, char deref, short *reg, int *voffset, char reg_sz, bool is_float, bool address, bool is_packed_float, short reg_dst = -1)
{
	while (deref >= 0)
	{
		if (deref > 0)
		{
			GenX64AutomaticAddress(lang_stat, ret, deref, reg, voffset, reg_sz, false, false, reg_dst);
		}
		else
		{
			 if(!address)
				GenX64AutomaticAddress(lang_stat, ret, deref, reg, voffset, reg_sz, is_float, address, is_packed_float, reg_dst);
			 else
				GenX64AutomaticAddress(lang_stat, ret, deref, reg, voffset, reg_sz, false, false, false, reg_dst);
		}

		deref--;
	}
}

byte_code_enum DetermineMovBcBasedOnDeref(char deref, bool is_float, bool is_packed_float)
{
	byte_code_enum ret;
	if (deref > 0)
	{
		if (is_float)
			if(is_packed_float)
				ret = MOV_PCKD_SSE_2_M;
			else
				ret = MOV_SSE_2_MEM;
		else
			ret = STORE_R_2_M;
	}
	else
	{
		if (is_float)
			if(is_packed_float)
				ret = MOV_PCKD_SSE_2_PCKD_SSE;
			else
				ret = MOV_SSE_2_SSE;
		else
			ret = MOV_R;
	}
	return ret;
}

struct ir_val_aux
{
	ir_val_type type;
	short reg;
	char reg_sz;
	int voffset;
	char deref;
	bool is_float;
	bool is_packed_float;
	bool is_unsigned;
	union
	{
		float f;
		int i;
	};
};
void GenX64ToIrValImm(lang_state *lang_stat, own_std::vector<byte_code>& ret, ir_val_aux *aux, ir_val *ir)
{
	aux->type = ir->type;
	if (ir->is_float)
		aux->f = ir->f32;
	else
		aux->i = ir->i;
	aux->is_float = ir->is_float;
}
void GenX64ToIrValReg2(lang_state *lang_stat, own_std::vector<byte_code>& ret, ir_val_aux *aux, ir_val *ir, bool address, char reg_dst = -1)
{
	aux->type = ir->type;
	aux->reg_sz = ir->reg_sz;
	aux->deref = ir->deref;
	aux->reg = ir->reg;
	aux->is_unsigned = ir->is_unsigned;
	aux->voffset = 0;
	aux->is_float = ir->is_float;
	aux->is_packed_float = ir->is_packed_float;
	char out_deref = aux->deref;
	if (address && aux->deref > 0 || !address)
	{
		if (address)
			aux->deref--;
		GenX64AutomaticRegDeref(lang_stat, ret, aux->deref, &aux->reg, aux->reg_sz, aux->is_float, aux->is_packed_float, address, reg_dst);
		//out_deref--;
	}
	//aux->deref = out_deref;
}
void GenX64ToIrValReg(lang_state *lang_stat, own_std::vector<byte_code>& ret, ir_val_aux *aux, ir_val *ir, bool address)
{
	aux->type = ir->type;
	aux->reg = ir->reg;
	aux->reg_sz = ir->reg_sz;
	aux->deref = ir->deref;
	aux->is_float = ir->is_float;
	aux->is_unsigned = ir->is_unsigned;
	GenX64AutomaticRegDeref(lang_stat, ret, aux->deref, &aux->reg, aux->reg_sz, aux->is_float, address, aux->reg);
}
void GenX64ToIrValFloatRaw(lang_state* lang_stat, own_std::vector<byte_code>& ret, ir_val_aux* aux, ir_val* ir, bool packed, short reg_dst = -1)
{
	aux->type = ir->type;
	aux->reg = reg_dst;
	aux->reg_sz = ir->reg_sz;
	aux->is_float = ir->is_float;
	aux->is_packed_float = ir->is_packed_float;

	MovFloatToSSEReg2(lang_stat, reg_dst, ir->f32, &ret, packed);
}

int GetOnStackOffsetWithIrVal(lang_state* lang_stat, ir_val* ir)
{
	int ret = 0;
	if (ir->type == IR_TYPE_ON_STACK)
	{
		switch (ir->on_stack_type)
		{
		case ON_STACK_SPILL:
			ret = ir->i + lang_stat->cur_func->to_spill_offset;
			break;
		case ON_STACK_STRUCT_CONSTR:
			ret = ir->i + lang_stat->cur_func->strct_constrct_at_offset;
			break;
		case ON_STACK_STRUCT_RET:
			ret = ir->i + lang_stat->cur_func->strct_ret_size_per_statement_offset;
			break;
		default:
			ASSERT(false)
		}
		
	}
	else
		ret = ir->decl->offset;
	return ret;
}
void GenX64DeclGlobal(lang_state* lang_stat, own_std::vector<byte_code>& ret, ir_val_aux* aux, int offset)
{
	byte_code bc;
	aux->type = IR_TYPE_REG;
	aux->reg = AllocReg(lang_stat);
	aux->voffset = 0;
	bc.type = MOV_I;
	bc.bin.lhs.reg = aux->reg;
	bc.bin.lhs.reg_sz = 4;
	bc.bin.rhs.i = GLOBALS_OFFSET + offset;
	ret.emplace_back(bc);
}
void GenX64ToIrValDecl2(lang_state *lang_stat, own_std::vector<byte_code>& ret, ir_val_aux *aux, ir_val *ir, bool address, short reg_dst = -1)
{
	aux->type = ir->type;
	aux->reg = PRE_X64_RSP_REG;
	aux->reg_sz = ir->reg_sz;
	aux->is_unsigned = ir->is_unsigned;
	aux->voffset = GetOnStackOffsetWithIrVal(lang_stat, ir);

	if (ir->type == IR_TYPE_DECL)
		ASSERT(ir->decl->type.type != TYPE_FUNC);
	aux->deref = ir->deref;
	aux->is_float = ir->is_float;
	aux->is_packed_float = ir->is_packed_float;
	byte_code bc;

	bool decl_is_global = false;

	if (ir->type == IR_TYPE_DECL && IS_FLAG_ON(ir->decl->flags, DECL_IS_GLOBAL))
	{
		decl_is_global = true;
		GenX64DeclGlobal(lang_stat, ret, aux, ir->decl->offset);

	}

	if (aux->is_packed_float)
	{
		address = false;
	}

	if (ir->ptr == -1 || ir->deref < 0)
	{
		if (!decl_is_global)
		{
			aux->reg = 32;
			EmplaceLeaInst2(aux->reg, PRE_X64_RSP_REG, aux->voffset, 8, ret);
		}
		else
			EmplaceLeaInst2(aux->reg, aux->reg, aux->voffset, 8, ret);
		aux->voffset = 0;
	}
	else if(address && aux->deref > 0 || !address)
	{
		if (address)
			aux->deref--;
		GenX64AutomaticDeclDeref(lang_stat, ret, aux->deref, &aux->reg, &aux->voffset, aux->reg_sz, aux->is_float, address, aux->is_packed_float, reg_dst);
	}
	if (aux->reg != PRE_X64_RSP_REG)
		aux->type = IR_TYPE_REG;
}
void GenX64ToIrValDecl(lang_state *lang_stat, own_std::vector<byte_code>& ret, ir_val_aux *aux, ir_val *ir, bool address, short reg_dst = -1)
{
	aux->type = ir->type;
	aux->reg = PRE_X64_RSP_REG;
	aux->reg_sz = ir->reg_sz;
	aux->voffset = GetOnStackOffsetWithIrVal(lang_stat, ir);
	aux->deref = ir->deref;
	aux->is_float = ir->is_float;
	byte_code bc;
	if (ir->ptr == -1)
	{
		aux->reg = AllocReg(lang_stat);
		EmplaceLeaInst2(aux->reg, PRE_X64_RSP_REG, aux->voffset, 8, ret);
		aux->voffset = 0;
	}
	else
	{
		GenX64AutomaticDeclDeref(lang_stat, ret, aux->deref, &aux->reg, &aux->voffset, aux->reg_sz, aux->is_float, address, reg_dst);
	}
	if (aux->reg != PRE_X64_RSP_REG)
		aux->type = IR_TYPE_REG;
}
#pragma optimize("", off)
void GenX64BinInst(lang_state *lang_stat, own_std::vector<byte_code>& ret, ir_val_aux *lhs, ir_val_aux *rhs, byte_code_enum inst)
{
	byte_code bc = {};
	if (lhs->is_packed_float)
	{
		inst = (byte_code_enum)((u32)inst + 3);
	}
	bc.type = inst;
	bc.bin.is_unsigned = lhs->is_unsigned;

	switch (inst)
	{
	case SUB_I_2_R:
	case SUB_I_2_M:
	case ADD_I_2_R:
	case ADD_I_2_M:
	{
		if(rhs->i == 0)
			return;
	}
	}
	switch (lhs->type)
	{
	case IR_TYPE_F32:
	{
		bc.bin.lhs.reg = lhs->reg;
		bc.bin.lhs.reg_sz = lhs->reg_sz;
		switch (rhs->type)
		{
		case IR_TYPE_INT:
		{
			bc.bin.rhs.i = rhs->i;
		}break;
		case IR_TYPE_F32:
		case IR_TYPE_REG:
		case IR_TYPE_RET_REG:
		case IR_TYPE_ARG_REG:
		{
			bc.bin.rhs.reg = rhs->reg;
			bc.bin.rhs.reg_sz = lhs->reg_sz;
		}break;
		case IR_TYPE_DECL:
		{
			bc.bin.rhs.reg = rhs->reg;
			bc.bin.rhs.reg_sz = rhs->reg_sz;
			bc.bin.rhs.voffset = rhs->voffset;
		}break;
		default:
			ASSERT(false)
		}
		//TODO
	}break;
	case IR_TYPE_ARG_REG:
	case IR_TYPE_RET_REG:
	case IR_TYPE_REG:
	{
		switch (rhs->type)
		{
		case IR_TYPE_INT:
		{
			bc.bin.lhs.reg = lhs->reg;
			bc.bin.lhs.reg_sz = lhs->reg_sz;
			bc.bin.rhs.i = rhs->i;
		}break;
		case IR_TYPE_F32:
		case IR_TYPE_REG:
		case IR_TYPE_RET_REG:
		case IR_TYPE_ARG_REG:
		{
			bc.bin.lhs.reg = lhs->reg;
			bc.bin.lhs.reg_sz = lhs->reg_sz;
			bc.bin.rhs.reg = rhs->reg;
			bc.bin.rhs.reg_sz = lhs->reg_sz;
		}break;
		case IR_TYPE_DECL:
		{
			bc.bin.lhs.reg = lhs->reg;
			bc.bin.lhs.reg_sz = lhs->reg_sz;
			bc.bin.rhs.reg = rhs->reg;
			bc.bin.rhs.reg_sz = rhs->reg_sz;
			bc.bin.rhs.voffset = rhs->voffset;
		}break;
		default:
			ASSERT(false)
		}
	}break;
	case IR_TYPE_DECL:
	{
		switch (rhs->type)
		{
		case IR_TYPE_INT:
		{
			bc.bin.lhs.reg = lhs->reg;
			bc.bin.lhs.reg_sz = lhs->reg_sz;
			bc.bin.lhs.voffset = lhs->voffset;
			bc.bin.rhs.i = rhs->i;
		}break;
		case IR_TYPE_DECL:
		{
			bc.bin.lhs.reg = lhs->reg;
			bc.bin.lhs.reg_sz = lhs->reg_sz;
			bc.bin.lhs.voffset = lhs->voffset;
			bc.bin.rhs.reg = rhs->reg;
			bc.bin.rhs.reg_sz = rhs->reg_sz;
			bc.bin.rhs.voffset = rhs->voffset;
		}break;
		case IR_TYPE_REG:
		{
			bc.bin.lhs.reg = lhs->reg;
			bc.bin.lhs.reg_sz = lhs->reg_sz;
			bc.bin.lhs.voffset = lhs->voffset;

			bc.bin.rhs.reg = rhs->reg;
			bc.bin.rhs.reg_sz = rhs->reg_sz;
		}break;
		default:
			ASSERT(false)
		}
	}break;
	default:
		ASSERT(false)
	}
	ret.emplace_back(bc);
}
#pragma optimize("", on)

byte_code_enum GenX64GetCorrectBinInst(ir_val_aux* lhs, ir_val_aux* rhs, byte_code_enum correct_inst, byte_code_enum base_inst_sse)
{
	if (rhs->is_float)
	{
		/*
		if (lhs->is_packed_float)
		{
			correct_inst = (byte_code_enum)((u32)base_inst_sse + 3);
		}
		*/
		if (rhs->reg == PRE_X64_RSP_REG)
			correct_inst = (byte_code_enum)(base_inst_sse + 1);
		// by default, base_inst_sse is already SSE_2_SSE
		else
			correct_inst = (byte_code_enum)(base_inst_sse);
	}
	else
	{
		if (rhs->reg == PRE_X64_RSP_REG)
			correct_inst = (byte_code_enum)(correct_inst + 2);
		// by default, base_inst_sse is already SSE_2_SSE
		else
			correct_inst = (byte_code_enum)(correct_inst + 7);
	}
	return correct_inst;
}
void GenX64PointLhs(lang_state *lang_stat, own_std::vector<byte_code>& ret, ir_val_aux *out, short reg, char reg_sz, int mem_offset, char deref)
{
	if (deref >= 0)
	{
		while (deref >= 0)
		{
			GenX64MemToReg(ret, AUX_DECL_REG, 8, mem_offset, reg, MOV_M);
			deref--;
			reg = AUX_DECL_REG;
			mem_offset = 0;
		}
	}
	else
	{

		EmplaceLeaInst2(AUX_DECL_REG, reg, mem_offset, 8, ret);
		reg = AUX_DECL_REG;
		mem_offset = 0;
	}

	out->type = IR_TYPE_REG;
	out->reg = reg;
	out->voffset = mem_offset;
	out->reg_sz = 8;

}

void GenX64AddGetFuncAddrReloc(lang_state* lang_stat, own_std::vector<byte_code>& ret, ir_val_aux *lhs, func_decl *fdecl)
{
	byte_code bc;
	bc.type = RELOC;
	bc.rel.type = REL_GET_FUNC_ADDR;
	bc.rel.call_func = fdecl;
	bc.rel.reg_dst = AllocReg(lang_stat);
	ret.emplace_back(bc);

	lhs->type = IR_TYPE_REG;	
	lhs->reg = bc.rel.reg_dst;
	lhs->reg_sz = 4;
}

void GenX64BytecodeFromAssignIR(lang_state* lang_stat,
	own_std::vector<byte_code>& ret,
	own_std::vector<ir_rep>& irs,
	wasm_gen_state* gen_state,
	assign_info &assign, int line)
{
	byte_code bc = {};
	if (assign.only_lhs)
	{
		switch (assign.to_assign.type)
		{
		case IR_TYPE_ARG_REG:
		{
			FreeSpecificFloatReg(lang_stat, assign.to_assign.reg);
			switch (assign.lhs.type)
			{
			case IR_TYPE_STR_LIT:
			{
				char str_on_reg = AllocReg(lang_stat);
				bc.type = RELOC;
				bc.rel.type = REL_DATA;
				bc.rel.reg_dst = str_on_reg;
				bc.rel.offset = assign.lhs.on_data_sect_offset;
				ret.emplace_back(bc);
				bc.type = MOV_R_2_REG_PARAM;
				bc.bin.lhs.reg = assign.to_assign.reg;
				bc.bin.lhs.reg_sz = assign.to_assign.reg_sz;
				bc.bin.rhs.reg = str_on_reg;
				bc.bin.rhs.reg_sz = 8;
				ret.emplace_back(bc);
				FreeSpecificReg(lang_stat, str_on_reg);

				//TODO
			}break;
			case IR_TYPE_ON_STACK:
			case IR_TYPE_DECL:
			{
				ir_val_aux lhs;
				if (assign.lhs.type == IR_TYPE_DECL && (assign.lhs.decl->type.type == TYPE_FUNC || assign.lhs.decl->type.type == TYPE_FUNC_PTR))
				{
					GenX64AddGetFuncAddrReloc(lang_stat, ret, &lhs, assign.lhs.decl->type.fdecl);
				}
				else
				{
					GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false);
				}
				if (lhs.reg == PRE_X64_RSP_REG)
					bc.type = MOV_M_2_REG_PARAM;
				else
				{
					if (lhs.is_float)
						bc.type = MOV_SSE_2_REG_PARAM;
					else
						bc.type = MOV_R_2_REG_PARAM;
				}
				bc.bin.lhs.reg = assign.to_assign.reg;
				bc.bin.lhs.reg_sz = assign.to_assign.reg_sz;
				bc.bin.rhs.reg = lhs.reg;
				bc.bin.rhs.reg_sz = lhs.reg_sz;
				bc.bin.rhs.voffset = lhs.reg_sz;

				ret.emplace_back(bc);

			}break;
			case IR_TYPE_F32:
			{
				char sse_reg = 4;
				MovFloatToSSEReg2(lang_stat, sse_reg, assign.lhs.f32, &ret, false);
				bc.type = MOV_SSE_2_REG_PARAM;
				bc.bin.lhs.reg = assign.to_assign.reg;
				bc.bin.lhs.reg_sz = assign.to_assign.reg_sz;
				bc.bin.rhs.reg = sse_reg;

				ret.emplace_back(bc);
			}break;
			case IR_TYPE_INT:
			{
				bc.type = MOV_I_2_REG_PARAM;
				bc.bin.lhs.reg = assign.to_assign.reg;
				bc.bin.lhs.reg_sz = assign.to_assign.reg_sz;
				bc.bin.rhs.i = assign.lhs.i;

				ret.emplace_back(bc);
			}break;
			case IR_TYPE_RET_REG:
			case IR_TYPE_REG:
			{
				if (assign.lhs.reg == 5)
					assign.lhs.reg++;
				ir_val_aux lhs;
				if (assign.lhs.is_float)
				{
					GenX64ToIrValReg2(lang_stat, ret, &lhs, &assign.lhs, false);
					bc.type = MOV_SSE_2_REG_PARAM;
					bc.bin.lhs.reg = assign.to_assign.reg;
					bc.bin.lhs.reg_sz = assign.to_assign.reg_sz;
					bc.bin.rhs.reg = lhs.reg;
					bc.bin.rhs.reg_sz = lhs.reg_sz;
					ret.emplace_back(bc);
					byte_code* last = &ret.back();
					last->bin.is_float_param = true;
					//ASSERT(last->type == MOV_SSE_2_SSE);
					//last->type = MOV_SSE_2_REG_PARAM;
				}
				else
				{
					GenX64ToIrValReg2(lang_stat, ret, &lhs, &assign.lhs, false);
					bc.type = MOV_R_2_REG_PARAM;
					bc.bin.lhs.reg = assign.to_assign.reg;
					bc.bin.lhs.reg_sz = assign.to_assign.reg_sz;
					bc.bin.rhs.reg = lhs.reg;
					bc.bin.rhs.reg_sz = lhs.reg_sz;
					ret.emplace_back(bc);
				}
				if (assign.lhs.reg == 5)
					assign.lhs.reg--;

			}break;
			default:
				ASSERT(false)
			}
			//if()
		}break;
		case IR_TYPE_ON_STACK:
		case IR_TYPE_DECL:
		{
			//PreX64Deref(lang_stat, assign.to_assign.reg, assign.to_assign.deref - 1, ret);

			short reg = PRE_X64_RSP_REG;
			char reg_sz = assign.to_assign.reg_sz;
			int voffset = 0;
			char deref = assign.to_assign.deref;

			voffset = GetOnStackOffsetWithIrVal(lang_stat, &assign.to_assign);
			
			GenX64AutomaticDeclDeref(lang_stat, ret, deref, &reg, &voffset, reg_sz, false, true, false);


			//GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false, 4);


			switch (assign.lhs.type)
			{
			case IR_TYPE_GET_FUNC_BC:
			{

			}break;
			case IR_TYPE_DECL:
			{
				ir_val_aux lhs;
				if (assign.lhs.type == IR_TYPE_DECL && (assign.lhs.decl->type.type == TYPE_FUNC || assign.lhs.decl->type.type == TYPE_FUNC_PTR))
				{
					GenX64AddGetFuncAddrReloc(lang_stat, ret, &lhs, assign.lhs.decl->type.fdecl);
				}
				else
					GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false, 4);
				if (assign.lhs.is_float)
				{
					bc.type = MOV_SSE_2_MEM;
					bc.bin.lhs.voffset = voffset;
					bc.bin.lhs.reg = reg;
					bc.bin.lhs.reg_sz = reg_sz;
					bc.bin.rhs.reg = lhs.reg;
					ret.emplace_back(bc);
				}
				else
				{
					GenX64RegToMem(ret, lhs.reg, reg_sz, voffset, reg, STORE_R_2_M);
				}
			}break;
			case IR_TYPE_RET_REG:
			case IR_TYPE_REG:
			{
				short src_reg = assign.lhs.reg;
				if (assign.lhs.is_float)
				{
					ir_val_aux lhs;
					GenX64ToIrValReg2(lang_stat, ret, &lhs, &assign.lhs, false);
					if(lhs.is_packed_float)
						bc.type = MOV_PCKD_SSE_2_M;
					else
						bc.type = MOV_SSE_2_MEM;
					bc.bin.lhs.voffset = voffset;
					bc.bin.lhs.reg = reg;
					bc.bin.lhs.reg_sz = reg_sz;
					bc.bin.rhs.reg = lhs.reg;
					ret.emplace_back(bc);

				}
				else
				{
					ir_val_aux lhs;
					GenX64ToIrValReg(lang_stat, ret, &lhs, &assign.lhs, false);
					GenX64RegToMem(ret, lhs.reg, lhs.reg_sz, voffset, reg, STORE_R_2_M);
				}
			}break;
			case IR_TYPE_INT:
			{
				bc.type = STORE_I_2_M;
				bc.bin.lhs.reg = reg;
				bc.bin.lhs.reg_sz = reg_sz;
				bc.bin.lhs.voffset = voffset;
				bc.bin.rhs.i = assign.lhs.i;

				ret.emplace_back(bc);
			}break;
			case IR_TYPE_F32:
			{
				char sse_reg = 4;
				MovFloatToSSEReg2(lang_stat, sse_reg, assign.lhs.f32, &ret, assign.to_assign.is_packed_float);

				bc.type = MOV_SSE_2_MEM;
				bc.bin.lhs.reg = reg;
				bc.bin.lhs.reg_sz = reg_sz;
				bc.bin.lhs.voffset = voffset;
				bc.bin.rhs.reg = sse_reg;
				bc.bin.rhs.reg_sz = 8;

				ret.emplace_back(bc);
			}break;
			case IR_TYPE_STR_LIT:
			{
				char str_on_reg = AllocReg(lang_stat);
				bc.type = RELOC;
				bc.rel.type = REL_DATA;
				bc.rel.reg_dst = str_on_reg;
				bc.rel.offset = assign.lhs.on_data_sect_offset;
				ret.emplace_back(bc);
				bc.type = STORE_R_2_M;
				bc.bin.lhs.reg = reg;
				bc.bin.lhs.reg_sz = 8;
				bc.bin.lhs.voffset = voffset;
				bc.bin.rhs.reg = str_on_reg;
				bc.bin.rhs.reg_sz = 8;
				ret.emplace_back(bc);
				FreeSpecificReg(lang_stat, str_on_reg);
			}break;
			default:
				ASSERT(false);
			}
		}break;
		case IR_TYPE_RET_REG:
		case IR_TYPE_REG:
		{
			if (assign.to_assign.reg == 5)
				assign.to_assign.reg++;

			AllocSpecificReg(lang_stat, assign.to_assign.reg);
			if (assign.to_assign.is_float)
				AllocSpecificFloatReg(lang_stat, assign.to_assign.reg);
			switch (assign.lhs.type)
			{
			case IR_TYPE_STR_LIT:
			{
				ir_val_aux lhs;
				//assign.to_assign.deref--;
				GenX64ToIrValReg2(lang_stat, ret, &lhs, &assign.to_assign, true);

				char str_on_reg = AllocReg(lang_stat);
				bc.type = RELOC;
				bc.rel.type = REL_DATA;
				bc.rel.reg_dst = str_on_reg;
				bc.rel.offset = assign.lhs.on_data_sect_offset;
				ret.emplace_back(bc);
				if(lhs.deref >= 0)
					bc.type = STORE_R_2_M;
				else
					bc.type = MOV_R;
				bc.bin.lhs.reg = lhs.reg;
				bc.bin.lhs.reg_sz = 8;
				bc.bin.lhs.voffset = lhs.voffset;
				bc.bin.rhs.reg = str_on_reg;
				bc.bin.rhs.reg_sz = 8;
				ret.emplace_back(bc);
				FreeSpecificReg(lang_stat, str_on_reg);
				//TODO
			}break;
			case IR_TYPE_F32:
			{
				ir_val_aux rhs;

				short sse_reg = 4;
				GenX64ToIrValFloatRaw(lang_stat, ret, &rhs, &assign.lhs, assign.to_assign.is_packed_float, sse_reg);

				ir_val_aux lhs;
				assign.to_assign.deref--;
				GenX64ToIrValReg(lang_stat, ret, &lhs, &assign.to_assign, true);
				assign.to_assign.deref++;

				byte_code_enum inst;
				if (assign.to_assign.deref >= 0)
					inst = MOV_SSE_2_MEM;
				else
					inst = MOV_SSE_2_SSE;

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, inst);

			}break;
			case IR_TYPE_INT:
			{
				ir_val_aux lhs;
				//assign.to_assign.deref--;
				GenX64ToIrValReg2(lang_stat, ret, &lhs, &assign.to_assign, true);
				//assign.to_assign.deref++;

				ir_val_aux rhs;
				GenX64ToIrValImm(lang_stat, ret, &rhs, &assign.lhs);


				byte_code_enum inst;
				if (lhs.deref >= 0)
					inst = STORE_I_2_M;
				else
					inst = MOV_I;

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, inst);

				//GenX64ImmToReg(ret, assign.to_assign.reg, assign.to_assign.reg_sz, assign.lhs.i, MOV_I);
			}break;
			case IR_TYPE_ON_STACK:
			case IR_TYPE_DECL:
			{
				short reg_dst = assign.to_assign.reg;
				short reg_dst_sz = assign.to_assign.reg_sz;
				short reg_dst_deref = assign.to_assign.deref;
				bool reg_dst_float = assign.to_assign.is_float;
				reg_dst_deref--;
				GenX64AutomaticRegDeref(lang_stat, ret, reg_dst_deref, &reg_dst, reg_dst_sz, assign.to_assign.is_float, true, reg_dst);
				reg_dst_deref++;
				AllocSpecificReg(lang_stat, reg_dst);

				ir_val_aux lhs;

				if (assign.lhs.type == IR_TYPE_DECL && (assign.lhs.decl->type.type == TYPE_FUNC || assign.lhs.decl->type.type == TYPE_FUNC_PTR))
				{
					decl2* decl = assign.lhs.decl;
					GenX64AddGetFuncAddrReloc(lang_stat, ret, &lhs, decl->type.fdecl);
				}
				else
					GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false);
				//ASSERT(lhs.type == IR_TYPE_REG)

				if (reg_dst_deref < 0)
				{
					if(assign.to_assign.is_float)
						GenX64RegToReg(lang_stat, ret, reg_dst, reg_dst_sz, lhs.reg, MOV_SSE_2_SSE);
					else
						GenX64RegToReg(lang_stat, ret, reg_dst, reg_dst_sz, lhs.reg, MOV_R);
				}
				else
				{
					if(assign.to_assign.is_float)
						GenX64RegToReg(lang_stat, ret, reg_dst, reg_dst_sz, lhs.reg, MOV_SSE_2_MEM);
					else
						GenX64RegToMem(ret, lhs.reg, lhs.reg_sz, 0, reg_dst, STORE_R_2_M);
				}
				FreeSpecificReg(lang_stat, lhs.reg);
			}break;
			case IR_TYPE_RET_REG:
			case IR_TYPE_REG:
			{
				if (assign.lhs.reg == 5)
					assign.lhs.reg++;
				ir_val_aux lhs;
				//assign.to_assign.deref--;
				GenX64ToIrValReg2(lang_stat, ret, &lhs, &assign.to_assign, true);
				//assign.to_assign.deref++;

				ir_val_aux rhs;
				GenX64ToIrValReg(lang_stat, ret, &rhs, &assign.lhs, false);

				byte_code_enum inst;
				if (assign.to_assign.is_float)
				{
					if (assign.to_assign.deref >= 0)
					{
						if (assign.to_assign.is_packed_float)
							inst = MOV_PCKD_SSE_2_M;
						else
							inst = MOV_SSE_2_MEM;
					}
					else
					{
						if (assign.to_assign.is_packed_float)
							inst = MOV_PCKD_SSE_2_PCKD_SSE;
						else
							inst = MOV_SSE_2_SSE;
					}
				}
				else
				{
					if (lhs.deref >= 0)
						inst = STORE_R_2_M;
					else
						inst = MOV_R;
				}

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, inst);
				if (assign.lhs.reg == 5)
					assign.lhs.reg--;
			}break;
			default:
				ASSERT(false);
			}
			if (assign.to_assign.reg == 5)
				assign.to_assign.reg--;
		}break;
		default:
			ASSERT(false);
		}
	}
	else
	{
		//if(AreIRValsEqual(&assign.to_assign, assign))
		// lea inst shortcut
		//if(assign.lhs.type == IR_TYPE_REG)

		byte_code_enum base_inst;
		byte_code_enum base_inst_sse;
		bool is_point = false;
		switch (assign.op)
		{
		case T_DIV:
			base_inst = DIV_M_2_M;
			base_inst_sse = DIV_SSE_2_SSE;
			break;
		case T_MUL:
			base_inst = MUL_M_2_M;
			base_inst_sse = MUL_SSE_2_SSE;
		break;
		case T_PERCENT:
			base_inst = MOD_M_2_M;
		break;
		case T_POINT:
		{
			ir_val_aux lhs = {};
			switch (assign.lhs.type)
			{
			case IR_TYPE_REG:
			{
				GenX64PointLhs(lang_stat, ret, &lhs, assign.lhs.reg, 8, 0, assign.lhs.deref);
			}break;
			case IR_TYPE_ON_STACK:
			{
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, true);
			}break;
			case IR_TYPE_DECL:
			{
				if (IS_FLAG_ON(assign.lhs.decl->flags, DECL_IS_GLOBAL))
				{
					GenX64DeclGlobal(lang_stat, ret, &lhs, assign.lhs.decl->offset);
				}
				else
				{
					GenX64PointLhs(lang_stat, ret, &lhs, PRE_X64_RSP_REG, 8, assign.lhs.decl->offset, assign.lhs.deref);
				}
			}break;
			default:
				lhs.type = IR_TYPE_ARG_REG;
				ASSERT(false)
			}
			
			if(assign.rhs.i != 0)
				GenX64ImmToReg(ret, lhs.reg, 8, assign.rhs.i, ADD_I_2_R);


			switch (assign.to_assign.type)
			{
			case IR_TYPE_REG:
			{
				ir_val_aux dst;
				GenX64ToIrValReg(lang_stat, ret, &dst, &assign.to_assign, false);
				byte_code_enum inst;
				if (dst.deref > 0)
					inst = STORE_R_2_M;
				else
				{
					inst = MOV_R;
				}
				GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);
				AllocSpecificReg(lang_stat, dst.reg);
			}break;
			default:
				ASSERT(false)
			}

			is_point = true;

		}break;
		case T_PIPE:
			base_inst = OR_M_2_M;

		break;
		case T_AMPERSAND:
			base_inst = AND_M_2_M;

		break;
		case T_PLUS:
			base_inst = ADD_M_2_M;
			base_inst_sse = ADD_SSE_2_SSE;

		break;
		case T_MINUS:
			base_inst = SUB_M_2_M;
			base_inst_sse = SUB_SSE_2_SSE;
			break;
		default:
			ASSERT(false);
		}
		byte_code_enum correct_inst = base_inst;
		if (assign.lhs.is_float)
			correct_inst = base_inst_sse;

		
		bool saved_float = assign.rhs.is_float;
		if (is_point)
			return;

		switch (assign.to_assign.type)
		{
		// 64 D
		case IR_TYPE_DECL:
		{
			ir_val_aux decl;
			if (assign.lhs.type == IR_TYPE_DECL && assign.rhs.type == IR_TYPE_INT)
			{
				ir_val_aux imm;
				GenX64ToIrValImm(lang_stat, ret, &imm, &assign.rhs);

				ir_val_aux lhs;
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false);

				GenX64BinInst(lang_stat, ret, &lhs, &imm, (byte_code_enum)(correct_inst + 3));

				GenX64ToIrValDecl2(lang_stat, ret, &decl, &assign.to_assign, true);
				byte_code_enum inst = STORE_R_2_M;
				GenX64BinInst(lang_stat, ret, &decl, &lhs, inst);


			}
			else if (assign.lhs.type == IR_TYPE_DECL && (assign.rhs.type == IR_TYPE_REG || assign.rhs.type == IR_TYPE_RET_REG))
			{
				ir_val_aux lhs;
				//assign.lhs.deref++;
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false);
				//assign.lhs.deref--;

				ir_val_aux rhs;
				GenX64ToIrValReg(lang_stat, ret, &rhs, &assign.rhs, false);

				correct_inst = GenX64GetCorrectBinInst(&lhs, &rhs, correct_inst, base_inst_sse);

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst));
				
				GenX64ToIrValDecl2(lang_stat, ret, &decl, &assign.to_assign, true);
				byte_code_enum inst = STORE_R_2_M;
				if (lhs.is_float)
					inst = MOV_SSE_2_MEM;
				GenX64BinInst(lang_stat, ret, &decl, &lhs, inst);
			}
			// D D D
			else if (assign.lhs.type == IR_TYPE_DECL && assign.rhs.type == IR_TYPE_DECL)
			{
				ir_val_aux lhs;
				//assign.lhs.deref++;
				AllocSpecificReg(lang_stat, 0);

				if(assign.lhs.is_float)
					GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false, AllocFloatReg(lang_stat));
				else
					GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false, AllocReg(lang_stat));
				//assign.lhs.deref--;
				//AllocSpecificReg(lang_stat, lhs.reg);

				// GenX64ToIrValDecl2 use the rbx register by default
				// if none is provided
				/*
				ir_val_aux aux_reg_dst = lhs;
				aux_reg_dst.type = IR_TYPE_REG;
				if (aux_reg_dst.is_float)
					aux_reg_dst.reg = AllocFloatReg(lang_stat);
				else
					aux_reg_dst.reg = AllocReg(lang_stat);

				GenX64BinInst(lang_stat, ret, &aux_reg_dst, &lhs, MOV_R);
				*/

				ir_val_aux rhs;
				//assign.rhs.deref--;
				GenX64ToIrValDecl2(lang_stat, ret, &rhs, &assign.rhs, false, 0);
				//assign.rhs.deref++;

				correct_inst = GenX64GetCorrectBinInst(&lhs, &rhs, correct_inst, base_inst_sse);

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst));

				ir_val_aux dst;
				GenX64ToIrValDecl2(lang_stat, ret, &dst, &assign.to_assign, true);

				bool dst_float = assign.to_assign.is_float;
				bc = {};
				//byte_code_enum inst = GenX64GetCorrectBinInst(&dst, &lhs, correct_inst, base_inst_sse);
				//if(lhs.reg == PRE_X64_RSP_REG)
				GenX64RegToMem(ret, lhs.reg, lhs.reg_sz, dst.voffset, dst.reg, STORE_R_2_M);
				//GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);

				if (rhs.reg != PRE_X64_RSP_REG)
					FreeSpecificReg(lang_stat, rhs.reg);

				if (lhs.reg != PRE_X64_RSP_REG)
					FreeSpecificReg(lang_stat, lhs.reg);
			}
			else if (assign.lhs.type == IR_TYPE_DECL && assign.rhs.type == IR_TYPE_F32)
			{
				short reg = PRE_X64_RSP_REG;
				char reg_sz = assign.lhs.reg_sz;
				int voffset = assign.lhs.decl->offset;
				char deref = assign.lhs.deref;
				GenX64AutomaticDeclDeref(lang_stat, ret, deref, &reg, &voffset, reg_sz, assign.lhs.is_float, assign.lhs.is_packed_float, false);

				short sse_reg = 1;
				MovFloatToSSEReg2(lang_stat, sse_reg, assign.rhs.f32, &ret, assign.lhs.is_packed_float);
				byte_code_enum base_inst;
				switch (assign.op)
				{
				case T_PLUS:
					base_inst = ADD_SSE_2_SSE;
					break;
				default:
					ASSERT(false)
				}
				GenX64ToIrValDecl2(lang_stat, ret, &decl, &assign.to_assign, true);

				bc.type = base_inst;
				bc.bin.lhs.reg = reg;
				bc.bin.lhs.reg_sz = 8;
				bc.bin.rhs.reg = sse_reg;
				bc.bin.rhs.reg_sz = 8;
				ret.emplace_back(bc);

				bc.type = MOV_SSE_2_MEM;
				bc.bin.lhs.reg = decl.reg;
				bc.bin.lhs.voffset = decl.voffset;
				bc.bin.lhs.reg_sz = decl.reg_sz;
				bc.bin.rhs.reg = reg;
				bc.bin.rhs.reg_sz = decl.reg_sz;
				ret.emplace_back(bc);

			}
			else
				ASSERT(false);
		}break;
		// 64 R
		case IR_TYPE_REG:
		{
			if (assign.to_assign.is_float)
				AllocSpecificFloatReg(lang_stat, assign.to_assign.reg);
			
			auto prev_lhs_reg = assign.lhs.reg;
			// R D I
			if (assign.lhs.type == IR_TYPE_DECL && assign.rhs.type == IR_TYPE_INT)
			{

				ir_val_aux lhs;
				//assign.lhs.deref++;
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false);
				//assign.lhs.deref--;
				ASSERT(lhs.reg != PRE_X64_RSP_REG)
				//if()
				AllocSpecificReg(lang_stat, lhs.reg);


				ir_val_aux rhs;
				GenX64ToIrValImm(lang_stat, ret, &rhs, &assign.rhs);

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst + 3));
				ir_val_aux dst;
				GenX64ToIrValReg(lang_stat, ret, &dst, &assign.to_assign, true);

				bool dst_float = assign.to_assign.is_float;
				bc = {};
				byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst_float, lhs.is_packed_float);
				GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);
				FreeSpecificReg(lang_stat, lhs.reg);

			}
			// R I R
			else if (assign.lhs.type == IR_TYPE_INT && assign.rhs.type == IR_TYPE_REG)
			{
			
				AllocSpecificReg(lang_stat, assign.rhs.reg);
				short reg = AllocReg(lang_stat);

				GenX64ImmToReg(ret, reg, 4, assign.lhs.i, MOV_I);
				ir_val_aux lhs;
				lhs.type = IR_TYPE_REG;
				lhs.reg = reg;
				lhs.reg_sz = 4;


				ir_val_aux rhs;
				GenX64ToIrValReg(lang_stat, ret, &rhs, &assign.rhs, false);

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst + 7));
				ir_val_aux dst;
				GenX64ToIrValReg(lang_stat, ret, &dst, &assign.to_assign, true);

				bool dst_float = assign.to_assign.is_float;
				bc = {};
				byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst_float, lhs.is_packed_float);
				GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);
				FreeSpecificReg(lang_stat, lhs.reg);

			}
			// R R I
			else if (assign.lhs.type == IR_TYPE_REG && assign.rhs.type == IR_TYPE_INT)
			{

				ir_val_aux lhs;
				GenX64ToIrValReg2(lang_stat, ret, &lhs, &assign.lhs, false);
				AllocSpecificReg(lang_stat, lhs.reg);


				ir_val_aux rhs;
				GenX64ToIrValImm(lang_stat, ret, &rhs, &assign.rhs);

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst + 3));
				ir_val_aux dst = {};
				GenX64ToIrValReg2(lang_stat, ret, &dst, &assign.to_assign, true);


				if (dst.deref >= 0)
				{
					GenX64RegToMem(ret, lhs.reg, lhs.reg_sz, dst.voffset, dst.reg, STORE_R_2_M);
				}
				else
				{
					bool dst_float = assign.to_assign.is_float;
					bc = {};
					byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst_float, lhs.is_packed_float);
					dst.is_unsigned = lhs.is_unsigned;
					GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);
					FreeSpecificReg(lang_stat, lhs.reg);
				}

			}
			// R R R
			else if ((assign.lhs.type == IR_TYPE_REG && assign.rhs.type == IR_TYPE_REG) ||
					 (assign.lhs.type == IR_TYPE_ARG_REG && assign.rhs.type == IR_TYPE_REG))
			{
				ir_val_aux lhs;
				//assign.lhs.deref += assign.lhs.ptr;
				GenX64ToIrValReg2(lang_stat, ret, &lhs, &assign.lhs, false);
				//assign.lhs.deref -= assign.lhs.ptr;
				//assign.lhs.deref--;

				ir_val_aux rhs;
				GenX64ToIrValReg2(lang_stat, ret, &rhs, &assign.rhs, false);
				
				correct_inst = GenX64GetCorrectBinInst(&lhs, &rhs, correct_inst, base_inst_sse);
				GenX64BinInst(lang_stat, ret, &lhs, &rhs, correct_inst);

				ir_val_aux dst;
				GenX64ToIrValReg2(lang_stat, ret, &dst, &assign.to_assign, true);

				byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst.is_float, lhs.is_packed_float);
				if (dst.deref >= 0)
				{
					if(dst.is_float)
						GenX64RegToMem(ret, lhs.reg, dst.reg_sz, 0, dst.reg, MOV_SSE_2_MEM);
					else
						GenX64RegToMem(ret, lhs.reg, dst.reg_sz, 0, dst.reg, STORE_R_2_M);
				}
				else
					GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);

				if (lhs.is_float && assign.to_assign.reg != lhs.reg)
					FreeSpecificFloatReg(lang_stat, lhs.reg);
				if (rhs.is_float && assign.to_assign.reg != rhs.reg)
					FreeSpecificFloatReg(lang_stat, rhs.reg);

			}
			// R R D
			else if (assign.lhs.type == IR_TYPE_REG && assign.rhs.type == IR_TYPE_DECL)
			{
				ir_val_aux lhs;
				GenX64ToIrValReg2(lang_stat, ret, &lhs, &assign.lhs, false);

				ir_val_aux rhs;
				if (assign.rhs.is_float)
				{
					GenX64ToIrValDecl2(lang_stat, ret, &rhs, &assign.rhs, false);
				}
				else
					GenX64ToIrValDecl2(lang_stat, ret, &rhs, &assign.rhs, true);

				correct_inst = GenX64GetCorrectBinInst(&lhs, &rhs, correct_inst, base_inst_sse);

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst));

				ir_val_aux dst;
				GenX64ToIrValReg2(lang_stat, ret, &dst, &assign.to_assign, true);

				bool dst_float = assign.to_assign.is_float;
				bc = {};
				byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst_float, lhs.is_packed_float);
				if (dst.deref >= 0)
				{
					if (lhs.is_float)
						inst = MOV_SSE_2_MEM;
					else
						inst = STORE_R_2_M;
				}
				GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);
				FreeSpecificReg(lang_stat, lhs.reg);
				
			}
			else if (assign.lhs.type == IR_TYPE_INT && assign.rhs.type == IR_TYPE_INT)
			{
				int val = GetExpressionValT<int>(assign.op, assign.lhs.i, assign.rhs.i);

				GenX64ImmToReg(ret, assign.to_assign.reg, assign.to_assign.reg_sz, val, MOV_I);
			}
			// R D D
			else if ((assign.lhs.type == IR_TYPE_DECL && assign.rhs.type == IR_TYPE_DECL) || 
				(assign.lhs.type == IR_TYPE_ON_STACK && assign.rhs.type == IR_TYPE_ON_STACK))
			{
				ir_val_aux lhs;
				//assign.lhs.deref++;
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false);
				//assign.lhs.deref--;
				AllocSpecificReg(lang_stat, lhs.reg);

				ir_val_aux rhs;
				//assign.rhs.deref--;
				GenX64ToIrValDecl2(lang_stat, ret, &rhs, &assign.rhs, true);
				//assign.rhs.deref++;

				correct_inst = GenX64GetCorrectBinInst(&lhs, &rhs, correct_inst, base_inst_sse);

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst));

				ir_val_aux dst;
				GenX64ToIrValReg(lang_stat, ret, &dst, &assign.to_assign, true);

				bool dst_float = assign.to_assign.is_float;
				bc = {};
				byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst_float, lhs.is_packed_float);
				GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);


				if (rhs.reg != PRE_X64_RSP_REG)
					FreeSpecificReg(lang_stat, rhs.reg);

				if (lhs.reg != PRE_X64_RSP_REG)
					FreeSpecificReg(lang_stat, lhs.reg);
			}
			// R F R
			else if (assign.lhs.type == IR_TYPE_F32  && assign.rhs.type == IR_TYPE_REG)
			{
				char sse_reg = 0;
				ir_val_aux lhs;
				GenX64ToIrValFloatRaw(lang_stat, ret, &lhs, &assign.lhs, assign.rhs.is_packed_float, sse_reg);
				lhs.type = IR_TYPE_REG;

				if (lhs.reg == 0)
					sse_reg++;
				ir_val_aux rhs;
				GenX64ToIrValReg(lang_stat, ret, &rhs, &assign.rhs, false);
				//AllocSpecificReg(lang_stat, lhs.reg);


				correct_inst = base_inst_sse;

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst));

				ir_val_aux dst;
				GenX64ToIrValReg(lang_stat, ret, &dst, &assign.to_assign, true);
				
				byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst.is_float, lhs.is_packed_float);
				GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);
				//FreeSpecificReg(lang_stat, lhs.reg);

			}
			// R I D
			else if (assign.lhs.type == IR_TYPE_INT && assign.rhs.type == IR_TYPE_DECL)
			{
				ir_val_aux lhs;
				char reg = AllocReg(lang_stat);
				GenX64ImmToReg(ret, reg, 4, assign.lhs.i, MOV_I);
				lhs.type = IR_TYPE_REG;
				lhs.reg = reg;
				lhs.reg_sz = 4;

				ir_val_aux rhs;
				//assign.rhs.deref--;
				GenX64ToIrValDecl2(lang_stat, ret, &rhs, &assign.rhs, false);

				correct_inst = GenX64GetCorrectBinInst(&lhs, &rhs, correct_inst, base_inst_sse);

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst));

				ir_val_aux dst;
				GenX64ToIrValReg(lang_stat, ret, &dst, &assign.to_assign, true);

				bool dst_float = assign.to_assign.is_float;
				bc = {};
				byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst_float, lhs.is_packed_float);
				GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);

				FreeSpecificReg(lang_stat, reg);
			}
			// R R F
			else if (assign.lhs.type == IR_TYPE_REG && assign.rhs.type == IR_TYPE_F32)
			{
				ir_val_aux lhs;
				GenX64ToIrValReg(lang_stat, ret, &lhs, &assign.lhs, false);
				//AllocSpecificReg(lang_stat, lhs.reg);

				char sse_reg = 0;
				if (lhs.reg == 0)
					sse_reg++;
				ir_val_aux rhs;
				GenX64ToIrValFloatRaw(lang_stat, ret, &rhs, &assign.rhs, assign.lhs.is_packed_float, sse_reg);

				correct_inst = base_inst_sse;

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst));

				ir_val_aux dst;
				GenX64ToIrValReg2(lang_stat, ret, &dst, &assign.to_assign, true);
				
				byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst.is_float, lhs.is_packed_float);
				if (lhs.is_float && dst.deref >= 0)
					inst = MOV_SSE_2_MEM;
				GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);
				//FreeSpecificReg(lang_stat, lhs.reg);

			}
			else if (assign.lhs.type == IR_TYPE_F32 && assign.rhs.type == IR_TYPE_DECL)
			{
				ir_val_aux rhs;
				GenX64ToIrValDecl2(lang_stat, ret, &rhs, &assign.rhs, false);
				AllocSpecificReg(lang_stat, rhs.reg);

				char sse_reg = 0;
				if (rhs.reg == 0)
					sse_reg++;
				ir_val_aux lhs;
				GenX64ToIrValFloatRaw(lang_stat, ret, &lhs, &assign.lhs, assign.rhs.is_packed_float, sse_reg);

				correct_inst = base_inst_sse;

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst));

				ir_val_aux dst;
				GenX64ToIrValReg(lang_stat, ret, &dst, &assign.to_assign, true);
				
				byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst.is_float, lhs.is_packed_float);
				GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);
				FreeSpecificReg(lang_stat, lhs.reg);

			}
			// R D F
			else if (assign.lhs.type == IR_TYPE_DECL && assign.rhs.type == IR_TYPE_F32)
			{
				ir_val_aux lhs;
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false);
				AllocSpecificReg(lang_stat, lhs.reg);

				char sse_reg = 0;
				if (lhs.reg == 0)
					sse_reg++;
				ir_val_aux rhs;
				GenX64ToIrValFloatRaw(lang_stat, ret, &rhs, &assign.rhs, assign.lhs.is_packed_float, sse_reg);

				correct_inst = base_inst_sse;

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst));

				ir_val_aux dst;
				GenX64ToIrValReg(lang_stat, ret, &dst, &assign.to_assign, true);
				
				byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst.is_float, lhs.is_packed_float);
				GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);
				FreeSpecificReg(lang_stat, lhs.reg);

			}
			// R D R
			else if (assign.lhs.type == IR_TYPE_DECL && (assign.rhs.type == IR_TYPE_REG || assign.rhs.type == IR_TYPE_RET_REG))
			{
		
				ir_val_aux lhs;
				//assign.lhs.deref++;
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &assign.lhs, false);
				//assign.lhs.deref--;

				ir_val_aux rhs;
				GenX64ToIrValReg2(lang_stat, ret, &rhs, &assign.rhs, false);

				correct_inst = GenX64GetCorrectBinInst(&lhs, &rhs, correct_inst, base_inst_sse);

				GenX64BinInst(lang_stat, ret, &lhs, &rhs, (byte_code_enum)(correct_inst));
				
				ir_val_aux dst;
				GenX64ToIrValReg(lang_stat, ret, &dst, &assign.to_assign, true);
				
				byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst.is_float, lhs.is_packed_float);
				GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);
				if(lhs.reg != PRE_X64_RSP_REG)
					FreeSpecificReg(lang_stat, lhs.reg);
			}
			else
				ASSERT(false);
			
			if (assign.lhs.type == IR_TYPE_REG && assign.to_assign.reg != assign.lhs.reg)
				FreeSpecificReg(lang_stat, assign.lhs.reg);
			if (assign.rhs.type == IR_TYPE_REG && assign.to_assign.reg != assign.rhs.reg)
				FreeSpecificReg(lang_stat, assign.rhs.reg);


			AllocSpecificReg(lang_stat, assign.to_assign.reg);

		}break;
		default:
			ASSERT(false)
		}
		if (is_point)
			assign.rhs.is_float = saved_float;

	}

}
void PatchJmps(own_std::vector<byte_code>& ret, block_linked* cur)
{
	FOR_VEC(r, cur->rels)
	{
		byte_code* bc = &ret[r->bc];
		ASSERT(bc->type >= JMP && bc->type <= JMP_LE);
		bc->val = (ret.size()-1) - r->bc;
	}
}

void GenX64AsmGetReg(lang_state* lang_stat, std::string& reg_str, byte_code::operand* op)
{
	char reg = reg_str[1] -'0';
	char reg_sz_char = reg_str[2];
	char reg_sz = 0;
	switch (reg_sz_char)
	{
	case 'b':
	{
		reg_sz = 1;
	}break;
	case 'w':
	{
		reg_sz = 2;
	}break;
	case 'd':
	{
		reg_sz = 4;
	}break;
	case 'q':
	{
		reg_sz = 8;
	}break;
	default:
		ASSERT(0);
	}
	op->reg = reg;
	op->reg_sz = reg_sz;
}
bool GenX64AsmIsMemoryAssign(lang_state* lang_stat, token2** t, byte_code::operand *op)
{


	if ((*t)->type == T_WORD && ((*t) + 1)->type == T_OPEN_BRACKETS)
	{
		char sz = 0;
		if ((*t)->str == "byte")
		{
			sz = 1;
		}
		else if ((*t)->str == "word")
		{
			sz = 2;
		}
		else if ((*t)->str == "dword")
		{
			sz = 4;
		}
		else if ((*t)->str == "qword")
		{
			sz = 8;
		}
		else
			ASSERT(0);
		*t = *t + 2;
		ASSERT((*t)->type == T_WORD);
		GenX64AsmGetReg(lang_stat, (*t)->str, op);

		*t = *t + 1;

		if ((*t)->type != T_CLOSE_BRACKETS)
		{
			tkn_type2 t_op = (*t)->type;
			*t = *t + 1;
			ASSERT((*t)->type == T_INT);

			if (t_op == T_MINUS)
			{
				op->voffset = -(*t)->i;
			}
			else
				op->voffset = (*t)->i;
			*t = *t + 1;

		}
		ASSERT((*t)->type == T_CLOSE_BRACKETS);
		*t = *t + 1;
		return true;

	}
	else
	{
		GenX64AsmGetReg(lang_stat, (*t)->str, op);
		(*t)++;
	}
	return false;
}


token2 *GenX64AsmBin(lang_state* lang_stat, byte_code &bc, token2 *t,
	byte_code_enum m_to_m, 
	byte_code_enum r_to_m, 
	byte_code_enum r_to_r, 
	byte_code_enum m_to_i, 
	byte_code_enum r_to_i)
{
	char lhs_type = 0;
	char rhs_type = 0;
	if (GenX64AsmIsMemoryAssign(lang_stat, &t, &bc.bin.lhs))
		lhs_type = 1;

	ASSERT(t->type == T_COMMA);
	t++;

	if(t->type == T_INT)
	{
		bc.bin.rhs.i = t->i;
		rhs_type = 2;
	}
	else if (t->type == T_STR_LIT)
	{
		
		rhs_type = 3;
	}
	else if (GenX64AsmIsMemoryAssign(lang_stat, &t, &bc.bin.rhs))
	{
		rhs_type = 1;
	}


	switch (lhs_type | (rhs_type < 1))
	{
	// both regs
	case 0:
	{
		bc.type = MOV_R;
	}break;
	// lhs m, rhs r
	case 1:
	{
		bc.type = MOV_M;
	}break;
	// lhs r, rhs m
	case 2:
	{
		bc.type = STORE_R_2_M;
	}break;
	// lhs r, rhs i
	case 4:
	{
		bc.type = MOV_I;
	}break;
	// lhs m, rhs i
	case 5:
	{
		bc.type = STORE_I_2_M;
	}break;
	default:
		ASSERT(0);
	}
	t--;

	return t;
}

void GenX64ByteCodeFromStr(lang_state* lang_stat,
	own_std::vector<byte_code>& ret,
	std::string& str,
	scope *scp)
{
	type2 dummy_tp;
	own_std::vector<token2> tkns;
	Tokenize2((char*)str.data(), str.size(), &tkns);
	token2* t = &tkns[0];
	// the very last token is an end of file, so were ignoring it, by decrementing the ptr
	token2* end = tkns.end() - 1;


	for (; t < end; t++)
	{
		byte_code bc;
		if (t->type == T_WORD)
		{
			if (t->str == "mov")
			{
				t++;
				if ((t + 2)->type == T_STR_LIT)
				{

					byte_code::operand dummy_op = {};
					GenX64AsmGetReg(lang_stat, t->str, &dummy_op);
					bc.type = RELOC;
					bc.rel.type = REL_DATA;
					bc.rel.offset = lang_stat->data_sect.size();
					bc.rel.reg_dst = dummy_op.reg;
					InsertIntoDataSect(lang_stat, (char *) t->str.c_str(), t->str.size() + 1);
					t++;
				}
				else
				{
					t = GenX64AsmBin(lang_stat, bc, t,
						STORE_M_2_M,
						STORE_R_2_M,
						MOV_R,
						STORE_I_2_M,
						MOV_I);
				}
			}
			if (t->str == "mov_param")
			{
				t++;
				char reg = 0;
				// arg reg should be a number
				ASSERT(t->type == T_INT);
				reg = t->i;

				t++;
				ASSERT(t->type == T_COMMA);
				t++;

				char rhs_type = 0;
				if (t->type == T_INT)
				{
					rhs_type = 2;
				}
				if (GenX64AsmIsMemoryAssign(lang_stat, &t, &bc.bin.rhs))
					rhs_type = 1;

				if(rhs_type == 0)
				{
					bc.type = MOV_R_2_REG_PARAM;
				}
				else if (rhs_type == 1)
				{
					bc.type = MOV_M_2_REG_PARAM;
				}
				else if (rhs_type == 2)
				{
					bc.type = MOV_I_2_REG_PARAM;
				}

			}
			else if (t->str == "call")
			{
				t++;
				decl2 *func = FindIdentifier(t->str, scp, &dummy_tp);
				bc.type = RELOC;
				bc.rel.type = REL_FUNC;
				bc.rel.call_func = func->type.fdecl;
			}
			else
				ASSERT(0);

			ret.emplace_back(bc);
		}
		else
			ASSERT(0);
	}
}
// $IrX64
void GenX64BytecodeFromIR(lang_state *lang_stat, 
						  own_std::vector<byte_code>& ret,
						  own_std::vector<ir_rep>& irs,
						  wasm_gen_state *gen_state)
{
	auto cur = (block_linked*)malloc(sizeof(block_linked));
	cur->parent = nullptr;
	memset(cur, 0, sizeof(block_linked));
	int args = gen_state->cur_func->biggest_call_args - 4;
	int on_stack_args = max(args, 0);

	bool print_ir = false;

	unsigned int stack_size = 32 + on_stack_args * 8;
	int total_args = 0;
	int cur_line = 0;
	int start = ret.size();
	lang_stat->dstate->cur_func = gen_state->cur_func;
	lang_stat->cur_func = gen_state->cur_func;
	ret.emplace_back(byte_code(byte_code_enum::BEGIN_FUNC_FOR_INTERPRETER, gen_state->cur_func));

	func_decl* fdecl = lang_stat->cur_func;
	
	
	// moving dbg_start to rax
	GenX64RegToReg(lang_stat, ret, 0, 8, 1, MOV_R);
	char rbx_reg = 3;
	char rsi_reg = 4;
	int float_reg = 0;
	// moving args to rbx
	GenX64RegToReg(lang_stat, ret, rbx_reg, 8, 2, MOV_R);
	//GenX64ImmToReg(ret, PRE_X64_RSP_REG, 8, 8, SUB_I_2_R);
	for (int a = 0; a < fdecl->args.size(); a++)
	{
		byte_code bc = {};
		int i = a;
		decl2* d = fdecl->args[i];
		if (i >= 4)
			i++;
		if (d->type.ptr <= 0)
		{
			if (d->type.IsFloat())
			{
				bc.type = MOV_M_2_SSE;
				bc.bin.lhs.reg = float_reg;
				bc.bin.lhs.reg_sz = 4;
			}
			else
			{
				bc.type = MOV_M_2_REG_PARAM;
				bc.bin.lhs.reg = i;
				bc.bin.lhs.reg_sz = 8;
			}
			bc.bin.rhs.reg = rbx_reg;
			bc.bin.rhs.reg_sz = 8;
			bc.bin.rhs.voffset = a * 8;
			ret.emplace_back(bc);
		}
		else
		{
			bc.type = MOV_M;
			bc.bin.lhs.reg = rsi_reg;
			bc.bin.lhs.reg_sz = 8;
			bc.bin.rhs.reg = rbx_reg;
			bc.bin.rhs.reg_sz = 8;
			bc.bin.rhs.voffset = a * 8;

			ret.emplace_back(bc);

			GenX64RegToReg(lang_stat, ret, rsi_reg, 8, 0, ADD_R_2_R);

			bc.type = MOV_R_2_REG_PARAM;
			bc.bin.lhs.reg = i;
			bc.bin.lhs.reg_sz = 8;
			bc.bin.rhs.reg = rsi_reg;
			bc.bin.rhs.reg_sz = 8;
			ret.emplace_back(bc);
		}
	}
	//GenX64ImmToReg( ret, PRE_X64_RSP_REG, 8, 8, ADD_I_2_R);
	ret.emplace_back(byte_code(byte_code_enum::BEGIN_FUNC, gen_state->cur_func));
	gen_state->cur_func->bcs2_start += ret.size() - start;

	int idx = 0;
	stmnt_dbg* cur_st = gen_state->cur_func->wasm_stmnts.begin();
	int start_bc = ret.size();
	FOR_VEC(ir, irs)
	{
		ir_rep* cur_ir = ir;
		cur_ir->start = ret.size();
		byte_code bc;
		/*
		if (cur_line == 293 && IsExecutableIr(ir))
		{
			//bc.type = NOP;
			//ret.emplace_back(bc);
			printf("%d|%s\n", ir->idx, WasmIrToString(lang_stat->dstate, ir).c_str());
		}
		*/
		switch (ir->type)
		{
		case IR_STACK_END:
		{
			GenX64RetGroup(lang_stat, stack_size, ret);
		}break;
		case IR_STACK_BEGIN:
		{
			//stack_size += 32 + on_stack_args * 8;
			//gen_state->strcts_construct_stack_offset = stack_size;
			cur_ir->fdecl->strct_constrct_at_offset = stack_size;
			stack_size += cur_ir->fdecl->strct_constrct_size_per_statement;

			//gen_state->to_spill_offset = stack_size;
			cur_ir->fdecl->to_spill_offset = stack_size;
			stack_size += cur_ir->fdecl->to_spill_size * 8;

			//gen_state->strcts_ret_stack_offset = stack_size;
			cur_ir->fdecl->strct_ret_size_per_statement_offset = stack_size;
			stack_size += cur_ir->fdecl->strct_ret_size_per_statement;

			stack_size += cur_ir->fdecl->biggest_call_args * 8;
			//cur_ir->fdecl->stack_size = stack_size;
			ParametersToStack(cur_ir->fdecl, &ret);

			GenX64ImmToReg(ret, PRE_X64_RSP_REG, 8, stack_size, SUB_I_2_R);

			cur_ir->fdecl->stack_size = stack_size;


			if (IS_FLAG_ON(lang_stat->cur_func->flags, FUNC_DECL_COROUTINE))
			{
			}

		}break;
		case IR_BEGIN_STMNT:
		{
			FreeAllRegs(lang_stat);
			FreeAllFloatRegs(lang_stat);
			cur_line = ir->block.stmnt.line;
#ifndef WASM_DBG
			if(lang_stat->is_x64_bc_backend)
				cur_st->start = ret.size();
#endif

		}break;
		case IR_END_STMNT:
		{
			FreeAllRegs(lang_stat);
			FreeAllFloatRegs(lang_stat);
#ifndef WASM_DBG
			if(lang_stat->is_x64_bc_backend)
				cur_st->end = ret.size();
			cur_st++;
#endif
		}break;
		case IR_DECLARE_LOCAL:
		{
			if (IS_FLAG_ON(cur_ir->decl->flags, DECL_IS_GLOBAL))
				break;
			int to_sum = GetTypeSize(&cur_ir->decl->type);
			cur_ir->decl->offset = stack_size;
			stack_size += to_sum <= 4 ? 4 : to_sum;
		}break;
		case IR_DECLARE_ARG:
		{
			cur_ir->decl->offset = stack_size + total_args * 8 + 8;
			total_args++;
		}break;
		case IR_RET:
		{
			if (!ir->ret.no_ret_val)
			{
				switch (ir->ret.assign.lhs.type)
				{
				case IR_TYPE_F32:
				{
					MovFloatToSSEReg2(lang_stat, ir->ret.assign.to_assign.reg, ir->ret.assign.lhs.f32, &ret, false);
				}break;
				case IR_TYPE_INT:
				{
					bc.type = MOV_I;
					bc.bin.lhs.reg = ir->ret.assign.to_assign.reg;
					bc.bin.lhs.reg_sz = 8;
					bc.bin.rhs.i = ir->ret.assign.lhs.i;
					ret.emplace_back(bc);
				}break;
				case IR_TYPE_DECL:
				{
					ir_val* d = &ir->ret.assign.lhs;
					ir_val_aux lhs;
					GenX64ToIrValDecl2(lang_stat, ret, &lhs, d, false);

					if (lhs.reg == PRE_X64_RSP_REG)
						GenX64MemToReg(ret, 0, lhs.reg_sz, lhs.voffset, lhs.reg, MOV_M);
					else
						GenX64RegToReg(lang_stat, ret, 0, lhs.reg_sz, lhs.reg, MOV_R);
					//GenX64AutomaticDeclDeref(lang_stat, ret, d->deref, &d-> )
				}break;
				case IR_TYPE_RET_REG:
				case IR_TYPE_REG:
				{
					ir_val_aux lhs;
					GenX64ToIrValReg(lang_stat, ret, &lhs, &ir->ret.assign.lhs, true);

					ir_val_aux dst = {};
					dst.type = IR_TYPE_RET_REG;;
					dst.reg = ir->ret.assign.to_assign.reg;
					dst.reg_sz = ir->ret.assign.to_assign.reg_sz;


					bool dst_float = ir->ret.assign.to_assign.is_float;
					bc = {};
					byte_code_enum inst = DetermineMovBcBasedOnDeref(dst.deref, dst_float, lhs.is_packed_float);
					GenX64BinInst(lang_stat, ret, &dst, &lhs, inst);

					/*
					FreeSpecificReg(lang_stat, lhs.reg);
					if(ir->assign.to_assign.is_float)
						GenX64RegToReg(lang_stat, ret, ir->assign.to_assign.reg, ir->assign.to_assign.reg_sz, ir->assign.lhs.reg_sz, )
					PreX64Deref(lang_stat, ir->ret.assign.lhs.reg, ir->ret.assign.lhs.deref - 1, ret);
					PreX64Deref(lang_stat, ir->ret.assign.rhs.reg, ir->ret.assign.rhs.deref - 1, ret);
					bc.type = MOV_R;
					bc.bin.lhs.reg_sz = ir->ret.assign.lhs.reg_sz;
					bc.bin.lhs.reg = ir->ret.assign.to_assign.reg;

					FromIrValToBytecodeReg(&ir->ret.assign.lhs, &bc.bin.rhs);
					ret.emplace_back(bc);
					*/
				}break;
				case IR_TYPE_STR_LIT:
				{
					char str_on_reg = AllocReg(lang_stat);
					bc.type = RELOC;
					bc.rel.type = REL_DATA;
					bc.rel.reg_dst = str_on_reg;
					bc.rel.offset = ir->assign.lhs.on_data_sect_offset;
					ret.emplace_back(bc);
					bc.type = MOV_R;
					bc.bin.lhs.reg = ir->assign.to_assign.reg;
					bc.bin.lhs.reg_sz = ir->assign.to_assign.reg_sz;
					bc.bin.rhs.reg = str_on_reg;
					bc.bin.rhs.reg_sz = 8;
					ret.emplace_back(bc);
					FreeSpecificReg(lang_stat, str_on_reg);

					//TODO
				}break;
				default:
					ASSERT(false);
				}
			}

			GenX64RetGroup(lang_stat, stack_size, ret);
		}break;
		case IR_CMP_LT:
		case IR_CMP_LE:
		case IR_CMP_GE:
		case IR_CMP_GT:
		case IR_CMP_EQ:
		case IR_CMP_NE:
		{
			// CMP D I
			if (ir->bin.lhs.type == IR_TYPE_DECL && ir->bin.rhs.type == IR_TYPE_INT)
			{
				ir_val_aux lhs;
				//GenX64ToIrValDecl(lang_stat, ret, &lhs, &ir->bin.lhs, true);
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &ir->bin.lhs, true);

				ir_val_aux rhs;
				GenX64ToIrValImm(lang_stat, ret, &rhs, &ir->bin.rhs);

				if (lhs.deref >= 0)
				{
					GenX64ImmToMem(ret, lhs.reg, lhs.voffset, lhs.reg_sz, rhs.i, CMP_I_2_M);
				}
				else
				{
					FreeSpecificReg(lang_stat, lhs.reg);
					GenX64ImmToReg(ret, lhs.reg, lhs.reg_sz, rhs.i, CMP_I_2_R);
				}
			}
			//CMP R F
			else if (ir->bin.lhs.type == IR_TYPE_REG && ir->bin.rhs.type == IR_TYPE_F32)
			{
				ir_val_aux lhs;
				//ir->bin.lhs.deref++;
				GenX64ToIrValReg2(lang_stat, ret, &lhs, &ir->bin.lhs, false);
				//ir->bin.lhs.deref--;

				char sse_reg = 0;
				if (lhs.reg == 0)
					sse_reg++;
				MovFloatToSSEReg2(lang_stat, sse_reg, ir->assign.rhs.f32, &ret, ir->bin.lhs.is_packed_float);


				bc.type = CMP_SSE_2_SSE;
				bc.bin.lhs.reg = lhs.reg;
				bc.bin.rhs.reg = sse_reg;
				ret.emplace_back(bc);
			}
			// CMP D F
			else if (ir->bin.lhs.type == IR_TYPE_DECL && ir->bin.rhs.type == IR_TYPE_F32)
			{
				ir_val_aux lhs;
				//ir->bin.lhs.deref++;
				//GenX64ToIrValDecl(lang_stat, ret, &lhs, &ir->bin.lhs, false);
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &ir->bin.lhs, false);
				//ir->bin.lhs.deref--;
				AllocSpecificReg(lang_stat, lhs.reg);

				char sse_reg = 0;
				if (lhs.reg == 0)
					sse_reg++;
				MovFloatToSSEReg2(lang_stat, sse_reg, ir->assign.rhs.f32, &ret, ir->bin.lhs.is_packed_float);


				bc.type = CMP_SSE_2_SSE;
				bc.bin.lhs.reg = lhs.reg;
				bc.bin.rhs.reg = sse_reg;
				ret.emplace_back(bc);

				FreeSpecificReg(lang_stat, lhs.reg);
			}
			// CMP R RARG
			else if (ir->bin.lhs.type == IR_TYPE_REG && ir->bin.rhs.type == IR_TYPE_ARG_REG)
			{
				ir_val_aux lhs;
				GenX64ToIrValReg(lang_stat, ret, &lhs, &ir->bin.lhs, true);

				ir_val_aux rhs;
				GenX64ToIrValReg(lang_stat, ret, &rhs, &ir->bin.rhs, true);

				GenX64RegToReg(lang_stat, ret, lhs.reg, lhs.reg_sz, rhs.reg, CMP_R_2_R);
			}
			// CMP R I
			else if ((ir->bin.lhs.type == IR_TYPE_RET_REG || ir->bin.lhs.type == IR_TYPE_REG) && ir->bin.rhs.type == IR_TYPE_INT)
			{
				ir_val_aux lhs;
				ir->bin.lhs.deref--;
				GenX64ToIrValReg(lang_stat, ret, &lhs, &ir->bin.lhs, true);
				ir->bin.lhs.deref++;

				if (ir->bin.lhs.deref >= 0)
					GenX64ImmToMem(ret, lhs.reg, 0, lhs.reg_sz, ir->bin.rhs.i, CMP_I_2_M);
				else
					GenX64ImmToReg(ret, lhs.reg, lhs.reg_sz, ir->bin.rhs.i, CMP_I_2_R);
			}
			else if (ir->bin.lhs.type == IR_TYPE_RET_REG && ir->bin.rhs.type == IR_TYPE_F32)
			{
				ir_val_aux lhs;
				GenX64ToIrValReg(lang_stat, ret, &lhs, &ir->bin.lhs, true);

				ir_val_aux rhs;
				GenX64ToIrValFloatRaw(lang_stat, ret, &rhs, &ir->bin.rhs, ir->bin.lhs.is_packed_float, true);

				GenX64MemToReg(ret, lhs.reg, lhs.reg_sz, rhs.voffset, rhs.reg, CMP_SSE_2_SSE);
			}
			// CMP R D
			else if (ir->bin.lhs.type == IR_TYPE_REG && ir->bin.rhs.type == IR_TYPE_DECL)
			{
				ir_val_aux lhs;
				GenX64ToIrValReg2(lang_stat, ret, &lhs, &ir->bin.lhs, false);

				ir_val_aux rhs;
				GenX64ToIrValDecl2(lang_stat, ret, &rhs, &ir->bin.rhs, true);
				AllocSpecificReg(lang_stat, rhs.reg);



				byte_code_enum inst = CMP_M_2_R;
				if (rhs.reg == PRE_X64_RSP_REG)
				{
					if (lhs.is_float)
						inst = CMP_MEM_2_SSE;
					GenX64MemToReg(ret, lhs.reg, lhs.reg_sz, rhs.voffset, rhs.reg, inst);
				}
				else
				{
					if (lhs.is_float)
						inst = CMP_SSE_2_SSE;
					GenX64RegToReg(lang_stat, ret, lhs.reg, lhs.reg_sz, rhs.reg, CMP_R_2_R);
				}

				FreeSpecificReg(lang_stat, lhs.reg);
				FreeSpecificReg(lang_stat, rhs.reg);
			}
			else if (ir->bin.lhs.type == IR_TYPE_DECL && ir->bin.rhs.type == IR_TYPE_REG)
			{
				ir_val_aux lhs;
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &ir->bin.lhs, true);
				AllocSpecificReg(lang_stat, lhs.reg);

				ir_val_aux rhs;
				GenX64ToIrValReg(lang_stat, ret, &rhs, &ir->bin.rhs, false);


				byte_code_enum inst = CMP_R_2_M;
				if (ir->bin.lhs.is_float)
					inst = CMP_SSE_2_MEM;
				GenX64RegToMem(ret, rhs.reg, rhs.reg_sz, lhs.voffset, lhs.reg, inst);

				FreeSpecificReg(lang_stat, lhs.reg);
				FreeSpecificReg(lang_stat, rhs.reg);
			}
			else if (ir->bin.lhs.type == IR_TYPE_INT && ir->bin.rhs.type == IR_TYPE_INT)
			{
				ir_val_aux lhs;
				lhs.reg = AllocReg(lang_stat);
				GenX64ImmToReg(ret, lhs.reg, 4, ir->bin.lhs.i, MOV_I);

				ir_val_aux rhs;
				GenX64ToIrValImm(lang_stat, ret, &rhs, &ir->bin.rhs);

				GenX64ImmToReg(ret, lhs.reg, 4, ir->bin.rhs.i, CMP_I_2_R);
				FreeSpecificReg(lang_stat, lhs.reg);
			}
			else if (ir->bin.lhs.type == IR_TYPE_REG && ir->bin.rhs.type == IR_TYPE_REG)
			{
				ir_val_aux lhs;
				GenX64ToIrValReg(lang_stat, ret, &lhs, &ir->bin.lhs, false);

				ir_val_aux rhs;
				GenX64ToIrValReg(lang_stat, ret, &rhs, &ir->bin.rhs, false);
				byte_code_enum inst = CMP_R_2_R;
				if (lhs.is_float)
					inst = CMP_SSE_2_SSE;

				GenX64RegToReg(lang_stat, ret, lhs.reg, lhs.reg_sz, rhs.reg, inst);
			}
			// CMP D D
			else if (ir->bin.lhs.type == IR_TYPE_DECL && ir->bin.rhs.type == IR_TYPE_DECL)
			{
				ir_val_aux lhs;
				//ir->bin.lhs.deref++;
				char reg = -1;
				if (ir->bin.lhs.deref > -1)
					reg = AllocReg(lang_stat);
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &ir->bin.lhs, true, reg);
				//ir->bin.lhs.deref--;
				AllocSpecificReg(lang_stat, lhs.reg);

				ir_val_aux rhs;
				//ir->bin.rhs.deref--;
				GenX64ToIrValDecl2(lang_stat, ret, &rhs, &ir->bin.rhs, false);
				//ir->bin.rhs.deref++;


				byte_code_enum inst = CMP_R_2_M;
				if (lhs.is_float)
					inst = CMP_SSE_2_MEM;
				GenX64RegToMem(ret, rhs.reg, rhs.reg_sz, lhs.voffset, lhs.reg, inst);

				FreeSpecificReg(lang_stat, lhs.reg);
				FreeSpecificReg(lang_stat, rhs.reg);
			}
			else
				ASSERT(false);

			ir_rep* cur_ir = ir;
			ir_rep* next = cur_ir + 1;
			bool val = cur_ir->bin.it_is_jmp_if_true;

			if (cur_ir->bin.it_is_jmp_if_true)
			{
				if (next->type == IR_END_COND_BLOCK || next->type == IR_END_AND_BLOCK)
				{
					val = false;
				}
			}
			else
			{
				if (next->type == IR_END_OR_BLOCK)
				{
					val = true;
				}
			}

			tkn_type2 opposite = cur_ir->bin.op;
			if (cur_ir->bin.it_is_jmp_if_true != val)
				opposite = OppositeCondCmp(opposite);


			int depth = 0;

			block_linked* aux = cur;
			while (aux->parent)
			{
				if (!val && (aux->ir->type == IR_BEGIN_LOOP_BLOCK))
					depth++;
				//break;
				if (!val && (aux->ir->type == IR_BEGIN_OR_BLOCK || aux->ir->type == IR_BEGIN_SUB_IF_BLOCK || aux->ir->type == IR_BEGIN_IF_BLOCK || aux->ir->type == IR_BEGIN_LOOP_BLOCK))
					break;
				if (val && (aux->ir->type == IR_BEGIN_AND_BLOCK || aux->ir->type == IR_BEGIN_COND_BLOCK))
					break;
				depth++;
				aux = aux->parent;
			}
			byte_code* cmp_inst = &ret.back();
			cmp_inst->bin.is_unsigned = ir->bin.lhs.is_unsigned;

			EmplaceCondJmpInst2(opposite, 0, ret, ir->bin.lhs.is_unsigned | ir->bin.lhs.is_float);
			x64_rel_info rel;
			rel.bc = ret.size() - 1;
			aux->rels.emplace_back(rel);
			//byte_code* assert_bc = ret.back();
			//ASSERT(assert_bc->type >= JMP && assert_bc->type <= JMP_LE);
		}break;
		// X64 =
		case IR_ASSIGNMENT:
		{
			GenX64BytecodeFromAssignIR(lang_stat,
				ret,
				irs,
				gen_state,
				ir->assign, cur_line);
			break;

		}break;

		case IR_CAST_INT_TO_F32:
		{
			int a = 0;
			switch (ir->bin.rhs.type)
			{
			case IR_TYPE_INT:
			case IR_TYPE_REG:
			{
				ir_val_aux rhs;
				byte_code_enum inst = CVTSD_REG_2_SS;
				if(ir->bin.rhs.type == IR_TYPE_INT)
				{
					char reg = AllocReg(lang_stat);
					rhs.reg = reg;
					rhs.reg_sz = 4;
					GenX64ImmToReg(ret, reg, 4, ir->bin.rhs.i, MOV_I);
					FreeSpecificReg(lang_stat, reg);
				}
				else
				{
					GenX64ToIrValReg2(lang_stat, ret, &rhs, &ir->bin.rhs, true);
					if (rhs.deref >= 0)
						inst = CVTSD_MEM_2_SS;
				}
				


				bc.type = inst;
				FromIrValToBytecodeReg(&ir->bin.lhs, &bc.bin.lhs);
				bc.bin.lhs.reg_sz = 4;
				bc.bin.rhs.reg = rhs.reg;
				bc.bin.rhs.voffset = rhs.voffset;
				bc.bin.rhs.reg_sz = 1;
				ret.emplace_back(bc);
			}break;
			case IR_TYPE_DECL:
			{
				ir_val_aux rhs;
				GenX64ToIrValDecl2(lang_stat, ret, &rhs, &ir->bin.rhs, true);


				bc.type = CVTSD_MEM_2_SS;
				FromIrValToBytecodeReg(&ir->bin.lhs, &bc.bin.lhs);
				bc.bin.lhs.reg_sz = 4;
				bc.bin.rhs.reg = rhs.reg;
				bc.bin.rhs.voffset = rhs.voffset;
				bc.bin.rhs.reg_sz = 1;
				ret.emplace_back(bc);
			}break;
			default:
				ASSERT(false);
			}
		}break;

		case IR_BREAK_OUT_IF_BLOCK:
		{
			int depth = 0;

			block_linked* aux = cur;
			while (aux->parent)
			{
				if (aux->ir->type == IR_BEGIN_IF_BLOCK || aux->ir->type == IR_BEGIN_LOOP_BLOCK)
					break;
				depth++;
				aux = aux->parent;
			}
		}break;

		case IR_CAST_INT_TO_INT:
		{
			switch (ir->bin.lhs.type)
			{
			case IR_TYPE_REG:
			{
				ir_val_aux dst;
				GenX64ToIrValReg2(lang_stat, ret, &dst, &ir->bin.lhs, true);
				bc.bin.lhs.reg = dst.reg;
				bc.bin.lhs.reg_sz = ir->bin.lhs.reg_sz;

				ir_val_aux src;
				switch (ir->bin.rhs.type)
				{
				case IR_TYPE_REG:
				{
					GenX64ToIrValReg2(lang_stat, ret, &src, &ir->bin.rhs, false);
					bc.type = MOVZX_R;
					bc.ir = ir;
					bc.bin.rhs.reg = ir->bin.rhs.reg;
					bc.bin.rhs.reg_sz = ir->bin.rhs.reg_sz;
					ret.emplace_back(bc);
				}break;
				case IR_TYPE_STR_LIT:
				{
					char str_on_reg = AllocReg(lang_stat);
					bc.type = RELOC;
					bc.rel.type = REL_DATA;
					bc.rel.reg_dst = str_on_reg;
					bc.rel.offset = ir->bin.rhs.on_data_sect_offset;
					ret.emplace_back(bc);
					bc.type = MOVZX_R;
					bc.bin.lhs.reg = ir->bin.lhs.reg;
					bc.bin.lhs.reg_sz = 8;
					bc.bin.rhs.reg = str_on_reg;
					bc.bin.rhs.reg_sz = 4;
					ret.emplace_back(bc);
				}break;
				case IR_TYPE_DECL:
				{
					GenX64ToIrValDecl2(lang_stat, ret, &src, &ir->bin.rhs, true);
					bc.type = MOVZX_M;
					bc.bin.lhs.reg = ir->bin.lhs.reg;
					bc.bin.rhs.reg = src.reg;
					bc.bin.rhs.reg_sz = ir->bin.rhs.reg_sz;
					bc.bin.rhs.voffset = src.voffset;
					ret.emplace_back(bc);
				}break;
				default:
					ASSERT(false);
				}
			}break;
			default:
				ASSERT(false);
			}
		}break;
		case IR_BEGIN_SUB_IF_BLOCK:
		case IR_BEGIN_AND_BLOCK:
		case IR_BEGIN_OR_BLOCK:
		case IR_BEGIN_LOOP_BLOCK:
		case IR_BEGIN_BLOCK:
		case IR_BEGIN_COND_BLOCK:
		case IR_BEGIN_IF_BLOCK:
		{
			cur = NewBlock(cur);
			cur->ir = ir;
			cur->bc_idx = ret.size();
		}break;
		case IR_BREAK:
		{
			int depth = 0;

			block_linked* aux = cur;
			while (aux->parent)
			{
				if (aux->ir->type == IR_BEGIN_LOOP_BLOCK)
					break;
				depth++;
				aux = aux->parent;
			}

			bc.type = JMP;
			ret.emplace_back(bc);
			x64_rel_info rel;
			rel.bc = ret.size() - 1;
			aux->rels.emplace_back(rel);
		}break;
		case IR_INDIRECT_CALL:
		{
			ir_val_aux lhs;
			switch (ir->bin.lhs.type)
			{
			case IR_TYPE_DECL:
			{
				GenX64ToIrValDecl2(lang_stat, ret, &lhs, &ir->bin.lhs, false);
			}break;
			case IR_TYPE_REG:
			{
				GenX64ToIrValReg2(lang_stat, ret, &lhs, &ir->bin.lhs, false);
			}break;
			}
			bc.type = INST_CALL_REG;
			bc.bin.lhs.reg = lhs.reg;
			bc.bin.lhs.reg_sz = lhs.reg;
			ret.emplace_back(bc);
		}break;
		case IR_CONTINUE:
		{
			int depth = 0;

			block_linked* aux = cur;
			while (aux->parent)
			{
				if (aux->parent->ir && aux->parent->ir->type == IR_BEGIN_LOOP_BLOCK)
					break;
				aux = aux->parent;
			}
			

			ASSERT(aux);
			bc.type = JMP;
			ret.emplace_back(bc);
			x64_rel_info rel;
			rel.bc = ret.size() - 1;
			aux->rels.emplace_back(rel);
		}break;
		case IR_END_BLOCK:
		{
			PatchJmps(ret, cur);
			//EmplaceJmp
			FreeBlock(cur);
			cur = cur->parent;
		}break;
		case IR_END_LOOP_BLOCK:
		{
			bc.type = JMP;
			bc.val = cur->bc_idx - (ret.size() + 1);
			ret.emplace_back(bc);
			PatchJmps(ret, cur);
			//EmplaceJmp
			FreeBlock(cur);
			cur = cur->parent;

		}break;

		case IR_DBG_BREAK:
		{
			bc.type = byte_code_enum::INT3;
			ret.emplace_back(bc);
		}break;
		case IR_END_SUB_IF_BLOCK:
		{
			int depth = 0;

			block_linked* aux = cur;
			while (aux->parent)
			{
				if (aux->ir->type == IR_BEGIN_IF_BLOCK)
					break;
				aux = aux->parent;
			}


			bc.type = JMP;
			ret.emplace_back(bc);
			x64_rel_info rel;
			rel.bc = ret.size() - 1;
			aux->rels.emplace_back(rel);

			PatchJmps(ret, cur);

			FreeBlock(cur);
			cur = cur->parent;
		}break;
		case IR_END_AND_BLOCK:
		case IR_END_OR_BLOCK:
		case IR_END_COND_BLOCK:
		case IR_END_IF_BLOCK:
		{
			PatchJmps(ret, cur);
			FreeBlock(cur);
			cur = cur->parent;
			//code_sect.emplace_back(0xb);
		}break;
		case IR_CALL:
		{
			if (IS_FLAG_ON(ir->call.fdecl->flags, FUNC_DECL_INTRINSIC))
			{
				if (ir->call.fdecl->name == "__sqrt")
				{
					bc.type = SQRT_SSE;
					bc.bin.lhs.reg = 0;
					bc.bin.rhs.reg = 0;
					ret.emplace_back(bc);
				}
				else if (ir->call.fdecl->name == "GetFuncStackSize")
				{
					bc.type = MOV_I;
					bc.bin.lhs.reg = 0;
					bc.bin.lhs.reg_sz = 8;
					bc.bin.rhs.i = fdecl->stack_size;
					ret.emplace_back(bc);
				}
				else
					ASSERT(false)
			}
			else
			{
				ret.emplace_back(byte_code(rel_type::REL_FUNC, (char*)ir->call.fdecl->name.c_str(), (int)0, (char)0, ir->call.fdecl));
			}
		}break;
		case IR_CAST_F32_TO_INT:
		{
			int a = 0;
			switch (ir->bin.rhs.type)
			{
			case IR_TYPE_INT:
			{
				ASSERT(0);
				//TODO
			}break;
			case IR_TYPE_REG:
			{
				ir_val_aux rhs;
				GenX64ToIrValReg2(lang_stat, ret, &rhs, &ir->bin.rhs, false);


				bc.type = MOV_SSE_2_R;
				FromIrValToBytecodeReg(&ir->bin.lhs, &bc.bin.lhs);
				bc.bin.lhs.reg_sz = 4;
				bc.bin.rhs.reg = rhs.reg;
				bc.bin.rhs.voffset = rhs.voffset;
				bc.bin.rhs.reg_sz = 1;
				ret.emplace_back(bc);
			}break;
			case IR_TYPE_DECL:
			{
				ir_val_aux rhs;
				GenX64ToIrValDecl2(lang_stat, ret, &rhs, &ir->bin.rhs, false);


				bc.type = MOV_SSE_2_R;
				FromIrValToBytecodeReg(&ir->bin.lhs, &bc.bin.lhs);
				bc.bin.lhs.reg_sz = 4;
				bc.bin.rhs.reg = rhs.reg;
				bc.bin.rhs.voffset = rhs.voffset;
				bc.bin.rhs.reg_sz = 1;
				ret.emplace_back(bc);
			}break;
			default:
				ASSERT(false);
			}
			//TODO:
		}break;
		case IR_LABEL:
		{
			ir->decl->type.fdecl->bcs2_start = ret.size();
		}break;
		default:
			ASSERT(false)
		}
		idx++;
		cur_ir->end = ret.size();
		int aux_bc = 0;
	}
	free(cur);
}
void FromBcToBc2(web_assembly_state *wasm_state, own_std::vector<byte_code> *from, own_std::vector<byte_code2> *to)
{
	int idx = 0;
	FOR_VEC(from_bc, *from)
	{
		byte_code2 bc = {};
		bc.bc_type = from_bc->type;
		if (idx >= 19258)
			auto a = 0;
		switch (from_bc->type)
		{
		case NOP:
		case INT3:
		{
		}break;
		case INST_LEA:
		{
			bc.regs = from_bc->bin.rhs.lea.reg_dst;
			bc.regs |= from_bc->bin.rhs.lea.reg_base << RHS_REG_BIT;
			bc.regs |= GetSizeMin(from_bc->bin.rhs.lea.size)<<REG_SZ_BIT;
			bc.mem_offset = from_bc->bin.rhs.lea.offset;
		}break;
		case RELOC:
		{
			auto a = 0;
			bc.rel_type = from_bc->rel.type;
			switch (bc.rel_type)
			{
			case REL_DATA:
			{
				bc.regs = from_bc->rel.reg_dst;
				if (from_bc->rel.is_float)
				{
					if(from_bc->rel.is_packed_float)
						bc.type = MOV_F_2_PCKED_SSE;
					else
						bc.type = MOV_F_2_SSE;
					bc.f32 = from_bc->rel.f;
				}
				else
				{
					bc.type = MOV_I;
					bc.i = DATA_SECT_OFFSET + from_bc->rel.offset;
					bc.regs |= GetSizeMin(4)<<REG_SZ_BIT;
				}
			}break;
			case REL_GET_FUNC_ADDR:
			{
				char reg_dst = from_bc->rel.reg_dst;

				bc.type = MOV_I;
				bc.regs = reg_dst;
				bc.regs |= 2<<REG_SZ_BIT;
				func_decl* f = from_bc->rel.call_func;
				ASSERT(IS_FLAG_OFF(f->flags, FUNC_DECL_IS_OUTSIDER));
				bc.i = f->bcs2_start;
			}break;
			case REL_FUNC:
			{
				func_decl *f = from_bc->rel.call_func;
				if(IS_FLAG_ON(f->flags, FUNC_DECL_IS_OUTSIDER | FUNC_DECL_X64))
				{
					bc.type = INST_CALL_OUTSIDER;
					FuncAddedWasm(wasm_state, f->name, &bc.i);
				}
				else
				{
					bc.type = INST_CALL;
					bc.i = f->bcs2_start - (idx);
				}
			}break;
			default:
				ASSERT(0);
			}
		}break;
		case BEGIN_FUNC:
		case RET:
		case BEGIN_FUNC_FOR_INTERPRETER:
		{
		}break;
		case MOV_M_2_REG_PARAM:
		case MOV_M_2_SSE:
		case MOVSX_M:
		case ADD_M_2_R:
		case SUB_M_2_R:
		case CMP_M_2_R:
		case AND_M_2_R:
		case DIV_M_2_R:
		case MUL_M_2_R:
		case MOD_M_2_R:
		case CVTSD_MEM_2_SS:
		case ADD_MEM_2_SSE:
		case SUB_MEM_2_SSE:
		case MUL_MEM_2_SSE:
		case DIV_MEM_2_SSE:
		case CMP_MEM_2_SSE:
		case MOV_M_2_PCKD_SSE:
		case MOV_M:
		{
			bc.regs = from_bc->bin.lhs.reg;
			bc.regs |= from_bc->bin.rhs.reg << RHS_REG_BIT;
			bc.regs |= GetSizeMin(from_bc->bin.lhs.reg_sz)<<REG_SZ_BIT;
			bc.mem_offset = from_bc->bin.rhs.voffset;
		}break;
		case MOVZX_M:
		{
			bc.regs = from_bc->bin.lhs.reg;
			bc.regs |= from_bc->bin.rhs.reg << RHS_REG_BIT;
			bc.mem_offset = from_bc->bin.rhs.voffset;
			bc.rhs_and_lhs_reg_sz = GetSizeMin(from_bc->bin.lhs.reg_sz) | GetSizeMin(from_bc->bin.rhs.reg_sz) << 4;
		}break;
		case ADD_R_2_R:
		case CMP_R_2_R:
		case SUB_R_2_R:
		case DIV_R_2_R:
		case MUL_R_2_R:
		case SQRT_SSE:
		case AND_R_2_R:
		case OR_R_2_R:
		case MOV_SSE_2_SSE:
		case ADD_SSE_2_SSE:
		case ADD_PCKD_SSE_2_PCKD_SSE:
		case SUB_PCKD_SSE_2_PCKD_SSE:
		case MUL_PCKD_SSE_2_PCKD_SSE:
		case DIV_PCKD_SSE_2_PCKD_SSE:
		case MOV_PCKD_SSE_2_PCKD_SSE:
		case SUB_SSE_2_SSE:
		case DIV_SSE_2_SSE:
		case MUL_SSE_2_SSE:
		case CMP_SSE_2_SSE:
		case CVTSD_REG_2_SS:
		case MOV_R_2_REG_PARAM:
		case MOV_SSE_2_REG_PARAM:
		case MOV_SSE_2_R:
		case MOV_R:
		case MOVSX_R:
		case MOD_R_2_R:
		{
			bc.regs = from_bc->bin.lhs.reg;
			bc.regs |= from_bc->bin.rhs.reg << RHS_REG_BIT;
			bc.regs |= GetSizeMin(from_bc->bin.lhs.reg_sz)<<REG_SZ_BIT;
		}break;
		case MOVZX_R:
		{
			bc.regs = from_bc->bin.lhs.reg;
			bc.regs |= from_bc->bin.rhs.reg << RHS_REG_BIT;
			bc.rhs_and_lhs_reg_sz = GetSizeMin(from_bc->bin.lhs.reg_sz) | GetSizeMin(from_bc->bin.rhs.reg_sz) << 4;
		}break;
		case CMP_I_2_M:
		case ADD_I_2_M:
		case SUB_I_2_M:
		case MUL_I_2_M:
		case OR_I_2_M:
		case AND_I_2_M:
		case DIV_I_2_M:
		case STORE_I_2_M:
		case MOD_I_2_M:
		{
			bc.regs = from_bc->bin.lhs.reg;
			//bc.regs |= from_bc->bin.rhs.reg << RHS_REG_BIT;
			bc.regs |= GetSizeMin(from_bc->bin.lhs.reg_sz)<<REG_SZ_BIT;
			bc.mem_offset = from_bc->bin.lhs.voffset;
			bc.i = from_bc->bin.rhs.i;
		}break;
		case STORE_REG_PARAM:
		case MOV_SSE_2_MEM:
		case ADD_R_2_M:
		case SUB_R_2_M:
		case MUL_R_2_M:
		case CMP_R_2_M:
		case CMP_SSE_2_MEM:
		case ADD_SSE_2_MEM:
		case SUB_SSE_2_MEM:
		case MUL_SSE_2_MEM:
		case DIV_SSE_2_MEM:
		case MOV_PCKD_SSE_2_M:
		case DIV_R_2_M:
		case STORE_R_2_M:
		case AND_R_2_M:
		case OR_R_2_M:
		case MOD_R_2_M:
		{
			bc.regs = from_bc->bin.lhs.reg;
			bc.regs |= from_bc->bin.rhs.reg << RHS_REG_BIT;
			bc.regs |= GetSizeMin(from_bc->bin.lhs.reg_sz)<<REG_SZ_BIT;
			bc.mem_offset = from_bc->bin.lhs.voffset;
		}break;
		case ADD_M_2_M:
		{
			bc.regs = from_bc->bin.lhs.reg;
			bc.regs |= from_bc->bin.rhs.reg << RHS_REG_BIT;
			bc.regs |= GetSizeMin(from_bc->bin.lhs.reg_sz)<<REG_SZ_BIT;
			bc.mem_offset = from_bc->bin.lhs.voffset;
			bc.mem_offset2 = from_bc->bin.rhs.voffset;
		}break;
		case INST_CALL_REG:
		{
			bc.regs = from_bc->bin.lhs.reg;
		}break;
		case JMP_A:
		case JMP_AE:
		case JMP_B:
		case JMP_BE:
		case JMP:
		case JMP_L:
		case JMP_LE:
		case JMP_E:
		case JMP_GE:
		case JMP_G:
		case JMP_NE:
		{
			bc.i = from_bc->val;
		}break;
		case ADD_I_2_R:
		case MOV_I_2_REG_PARAM:
		case SUB_I_2_R:
		case CMP_I_2_R:
		case AND_I_2_R:
		case OR_I_2_R:
		case MUL_I_2_R:
		case DIV_I_2_R:
		case MOD_I_2_R:
		case MOV_I:
		{
			bc.regs = from_bc->bin.lhs.reg;
			bc.regs |= GetSizeMin(from_bc->bin.lhs.reg_sz)<<REG_SZ_BIT;
			bc.i = from_bc->bin.rhs.i;
		}break;
		default:
			ASSERT(0);
		}
		bc.regs |= (1 << FLAG_UNSIGNED) * from_bc->bin.is_unsigned;

		/*
		char lhs_reg = bc.regs & 0xf;
		char rhs_reg = (bc.regs  >> RHS_REG_BIT) & 0xf;
		if (lhs_reg == PRE_X64_RSP_REG)
		{
			bc.regs &= ~0xf;
			bc.regs |= BC2_RSP_REG;
		}
		if (rhs_reg == PRE_X64_RSP_REG)
		{
			bc.regs &= ~(0xf << RHS_REG_BIT);
			bc.regs |= (BC2_RSP_REG) << RHS_REG_BIT;
		}
		*/

		to->emplace_back(bc);
		idx++;
	}
}
#pragma optimize("", off)
void GenWasm(web_assembly_state* wasm_state)
{
	//ASSERT(func->func_bcode);
	own_std::vector<unsigned char>* ret = &wasm_state->final_buf;
	own_std::vector<unsigned char> type_sect;
	own_std::vector<unsigned char> func_sect;
	own_std::vector<unsigned char> import_sect;
	own_std::vector<unsigned char> element_sect;
	own_std::vector<unsigned char> table_sect;
	own_std::vector<unsigned char> exports_sect;
	own_std::vector<unsigned char> memory_sect;

	// magic number
	ret->emplace_back(0);
	ret->emplace_back(0x61);
	ret->emplace_back(0x73);
	ret->emplace_back(0x6d);
	// version
	ret->emplace_back(01);
	ret->emplace_back(0);
	ret->emplace_back(0);
	ret->emplace_back(0);

	// number of elem segmentns
	element_sect.emplace_back(1);
	// segment flags
	element_sect.emplace_back(0);
	// number of func ptrs
	WasmPushConst(WASM_TYPE_INT, 0, 0, &element_sect);
	element_sect.emplace_back(0xb);
	own_std::vector<unsigned char> uleb;

	uleb.clear();
	GenUleb128(&uleb, wasm_state->funcs.size() + wasm_state->imports.size());
	INSERT_VEC(element_sect, uleb);
	FOR_VEC(func, wasm_state->imports)
	{
		func_decl* f = (*func)->type.fdecl;
		int idx = 0;
		FuncAddedWasm(wasm_state, f->name, &idx);

		uleb.clear();
		GenUleb128(&uleb, idx);
		INSERT_VEC(element_sect, uleb);

	}
	FOR_VEC(func, wasm_state->funcs)
	{
		func_decl* f = *func;
		int idx = 0;
		FuncAddedWasm(wasm_state, f->name, &idx);

		uleb.clear();
		GenUleb128(&uleb, idx);
		INSERT_VEC(element_sect, uleb);

	}
	WasmInsertSectSizeAndType(&element_sect, 9);



	// ****
	// type of the section (type section)
	// ****
	int inserted = 0;
	//type sect
	FOR_VEC(t, wasm_state->types)
	{
		decl2* tp = *t;
		switch (tp->type.type)
		{
		case TYPE_OVERLOADED_FUNCS:
		{
		}break;
		case TYPE_FUNC_PTR:
		case TYPE_FUNC:
		{
			WasmAppendFunc(type_sect, tp->type.fdecl);
		}break;
		default:
			ASSERT(0)
		}
		tp->wasm_type_idx = inserted;
		inserted++;
	}

	uleb.clear();
	encodeSLEB128(&uleb, wasm_state->types.size());
	type_sect.insert(type_sect.begin(), uleb.begin(), uleb.end());

	uleb.clear();
	encodeSLEB128(&uleb, type_sect.size());

	// first inserting the size of the sect and then the type
	// so that, in memory, the type will come fircs
	type_sect.insert(type_sect.begin(), uleb.begin(), uleb.end());
	unsigned char sect_type = 1;
	type_sect.insert(0, sect_type);

	//****
	// import section
	//****
	inserted = 0;
	FOR_VEC(decl, wasm_state->imports)
	{
		decl2* d = *decl;
		switch (d->type.type)
		{
		case TYPE_FUNC:
		{
			func_decl* f = d->type.fdecl;
			WasmPushNameIntoArray(&import_sect, "funcs_import");
			WasmPushNameIntoArray(&import_sect, f->name);
			// import kind
			import_sect.emplace_back(0);
			// signature idx
			import_sect.emplace_back(0);

			inserted++;
		}break;
		default:
			ASSERT(0)
		}
	}
	uleb.clear();
	encodeSLEB128(&uleb, inserted);
	import_sect.insert(import_sect.begin(), uleb.begin(), uleb.end());
	WasmInsertSectSizeAndType(&import_sect, 2);

	//import_sect.emplace_back(2);

	//****
	//func section
	//****
	int funcs_inserted = 0;
	FOR_VEC(f, wasm_state->funcs)
	{
		decl2* d = (*f)->this_decl;

		// inserting idx of the type section
		uleb.clear();
		encodeSLEB128(&uleb, d->wasm_type_idx);

		func_sect.insert(func_sect.end(), uleb.begin(), uleb.end());

		(*f)->wasm_func_sect_idx = funcs_inserted + wasm_state->imports.size();
		funcs_inserted++;

	}
	uleb.clear();
	encodeSLEB128(&uleb, funcs_inserted);
	// same scheme as the type sect, but here we have to insert
	// to maintain the order in memory: first type, bytes then total funcs
	func_sect.insert(func_sect.begin(), uleb.begin(), uleb.end());

	uleb.clear();
	encodeSLEB128(&uleb, func_sect.size());

	func_sect.insert(func_sect.begin(), uleb.begin(), uleb.end());

	sect_type = 3;
	func_sect.insert(0, sect_type);


	// ******
	// export section
	// ******

	uleb.clear();
	encodeSLEB128(&uleb, wasm_state->exports.size());
	exports_sect.insert(exports_sect.begin(), uleb.begin(), uleb.end());

	FOR_VEC(exp, wasm_state->exports)
	{
		decl2* ex = *exp;

		WasmPushNameIntoArray(&exports_sect, ex->name);
		/*
		int name_len = ex->name.size();
		uleb.clear();
		encodeSLEB128(&uleb, name_len);
		exports_sect.insert(exports_sect.end(), uleb.begin(), uleb.end());
		unsigned char* name_ptr = (unsigned char*)ex->name.data();
		exports_sect.insert(exports_sect.end(), name_ptr, name_ptr + name_len);
		*/

		switch (ex->type.type)
		{
		case TYPE_WASM_MEMORY:
		{
			exports_sect.emplace_back(2);
			// func type
			exports_sect.emplace_back(0);
		}break;
		case TYPE_FUNC:
		{
			uleb.clear();
			if (ex->name == "_own_memset")
				auto a = 0;
			encodeSLEB128(&uleb, ex->type.fdecl->wasm_func_sect_idx);
			// func type
			exports_sect.emplace_back(0);
			// idx, i stiil dont know if this is in the type sect. or in the funct sect
			exports_sect.insert(exports_sect.end(), uleb.begin(), uleb.end());
		}break;
		default:
			ASSERT(0);
		}
	}
	// sect size
	uleb.clear();
	encodeSLEB128(&uleb, exports_sect.size());
	exports_sect.insert(exports_sect.begin(), uleb.begin(), uleb.end());
	sect_type = 7;
	exports_sect.insert(0, sect_type);


	table_sect.emplace_back(4);
	table_sect.emplace_back(5);
	table_sect.emplace_back(1);
	table_sect.emplace_back(0x70);
	table_sect.emplace_back(0x1);
	/*
	uleb.clear();
	//encodeSLEB128(&uleb, out->size());
	GenUleb128(&uleb, 0x90);
	INSERT_VEC(table_sect, uleb);
	uleb.clear();
	//encodeSLEB128(&uleb, out->size());
	GenUleb128(&uleb, 0x90);
	INSERT_VEC(table_sect, uleb);
	*/
	table_sect.emplace_back(0x70);
	table_sect.emplace_back(0x70);

	// ******
	// memory sect
	// ******
	memory_sect.emplace_back(5);
	memory_sect.emplace_back(3);
	// num of memories
	memory_sect.emplace_back(1);
	// flags
	memory_sect.emplace_back(0);
	// initial
	memory_sect.emplace_back(16);


	auto cur = NewBlock(nullptr);

	own_std::vector<unsigned char> final_code_sect;
	ASSERT(wasm_state->funcs.size() > 0);
	
	own_std::vector<func_decl*> x64_funcs;

	machine_code mcode;
	own_std::vector<byte_code2> bcs2;
	func_decl* asm_test = nullptr;
	// wasm
	FOR_VEC(f, wasm_state->funcs)
	{
		own_std::vector<unsigned char> code_sect;
		func_decl* func = *f;
		if (IS_FLAG_ON(func->flags, FUNC_DECL_INTRINSIC))
			continue;
		int prev_size = code_sect.size();

		own_std::vector<ir_rep>* ir = (own_std::vector<ir_rep> *) & func->ir;
		bool use_ir = true;
		int total_of_locals = 0;

		func->wasm_code_sect_idx = final_code_sect.size();

		std::unordered_map<decl2*, int> decl_to_local_idx;
		int total_of_args = 0;
		int stack_size = 0;
		ir_val last_on_stack;
		wasm_gen_state gen;
		gen.wasm_state = wasm_state;
		gen.cur_func = func;
		gen.similar.reserve(8);
		ir_rep* cur_ir = ir->begin();
		int i = 0;
		while (cur_ir < ir->end())
		{
			WasmFromSingleIR(decl_to_local_idx, total_of_args, total_of_locals, cur_ir, code_sect, &stack_size, ir, &cur, &last_on_stack, &gen, i);
			if (cur_ir->type == IR_STACK_BEGIN)
			{
				func->ir_stack_begin_idx = i;
			}

			cur_ir = cur_ir + (1 + (gen.advance_ptr));
			i++;
		}


		func->stack_size = stack_size;
		func->bcs2_start = mcode.bcs.size();
		//WasmPushConst(WASM_TYPE_INT, 0, 3, &code_sect);
		code_sect.emplace_back(0x0b);
		// end
		total_of_locals = 0;
		if (total_of_locals == 0)
		{
			// total of locals
			sect_type = total_of_locals;
			code_sect.insert(0, sect_type);
		}
		else
		{
			sect_type = 1;
			own_std::vector<unsigned char> aux;

			// only i32 type
			aux.insert(0, sect_type);
			//number of locals of type 32
			WasmPushImm(total_of_locals, &aux);
			// i32 type
			aux.emplace_back(0x7f);

			code_sect.insert(code_sect.begin(), aux.begin(), aux.end());
		}
		uleb.clear();
		// function size
		encodeSLEB128(&uleb, code_sect.size() - prev_size);
		code_sect.insert(code_sect.begin(), uleb.begin(), uleb.end());
		INSERT_VEC(final_code_sect, code_sect);

#ifndef WASM_DBG
		if (IS_FLAG_OFF(func->flags, FUNC_DECL_X64))
		{
			wasm_state->lang_stat->is_x64_bc_backend = true;
			GenX64BytecodeFromIR(wasm_state->lang_stat, mcode.bcs, *ir, &gen);
		}
#endif
		func->bcs2_end = mcode.bcs.size();
		x64_funcs.emplace_back(func);
		wasm_state->lang_stat->global_funcs.emplace_back(func);

		func->flags |= FUNC_DECL_CODE_WAS_GENERATED;

	}
	/*
	for (int j = 0; j < mcode.bcs.size(); j++)
	{
		byte_code* cur_bc = &mcode.bcs[j];
		if (cur_bc->type == MOVZX_R)
		{
			if (cur_bc->bin.rhs.reg == 0x80)
			{
				ASSERT(0);
			}
		}
	}
	*/
	timer tmr;
	InitTimer(&tmr);
	StartTimer(&tmr);
	FOR_VEC(f, wasm_state->funcs)
	{
		func_decl* func = *f;
		own_std::vector<ir_rep>* ir = (own_std::vector<ir_rep> *) & func->ir;
		if (IS_FLAG_ON(func->flags, FUNC_DECL_X64))
		{
			//func->code = (machine_code *)AllocMiscData(wasm_state->lang_stat, sizeof(machine_code));

			wasm_gen_state state = {};
			state.cur_func = func;
			wasm_state->lang_stat->is_x64_bc_backend = false;
			GenX64BytecodeFromIR(wasm_state->lang_stat, mcode.bcs, *ir, &state);
			x64_funcs.emplace_back(func);
			wasm_state->lang_stat->global_funcs.emplace_back(func);

			func->flags |= FUNC_DECL_CODE_WAS_GENERATED;

			if (func->name == "SimpleAsm")
				asm_test = func;
		}
	}
	FromBcToBc2(wasm_state,	&mcode.bcs, &bcs2);
	GenX64(wasm_state->lang_stat, mcode.bcs, mcode);
	CompleteMachineCode(wasm_state->lang_stat, mcode);
	EndTimer(&tmr);
	printf("x64 code gen %d\n", GetTimerMS(&tmr));

	//ResolveJmpInsts(&mcode);
	/*
	FOR_VEC(f, x64_funcs)
	{
		func_decl* func = *f;

		wasm_state->lang_stat->cur_func = func;
		GenX64(wasm_state->lang_stat, mcode.bcs, mcode);


	}
	*/
	if (asm_test)
	{
		int reached = 0;
		int func_addr_int = asm_test->code_start_idx;
		char* func_addr = (char*)(wasm_state->lang_stat->code_sect.data() + wasm_state->lang_stat->type_sect.size() + func_addr_int);
		uleb.clear();
		/*
		int a = ((int(__cdecl*)(int*))func_addr)(&reached);
		uleb.clear();
		if (a == -1)
		{
			ASSERT(false)
		}
		*/
	}
	uleb.clear();
	// total funcs size
	encodeSLEB128(&uleb, wasm_state->funcs.size());
	final_code_sect.insert(final_code_sect.begin(), uleb.begin(), uleb.end());

	uleb.clear();
	// sect size
	encodeSLEB128(&uleb, final_code_sect.size());
	final_code_sect.insert(final_code_sect.begin(), uleb.begin(), uleb.end());

	sect_type = 10;
	final_code_sect.insert(0, sect_type);

	/*
	own_std::vector<unsigned char> type_sect;
	own_std::vector<unsigned char> code_sect;
	own_std::vector<unsigned char> func_sect;
	own_std::vector<unsigned char> exports_sect;
	own_std::vector<unsigned char> memory_sect;
	*/

	INSERT_VEC((*ret), type_sect);
	INSERT_VEC((*ret), import_sect);
	INSERT_VEC((*ret), func_sect);
	INSERT_VEC((*ret), table_sect);
	INSERT_VEC((*ret), memory_sect);
	INSERT_VEC((*ret), exports_sect);
	INSERT_VEC((*ret), element_sect);
	INSERT_VEC((*ret), final_code_sect);

	lang_state* lang_stat = wasm_state->lang_stat;
	WriteFileLang((char*)(wasm_state->wasm_dir + wasm_state->folder_name + ".wasm").c_str(), ret->begin(), ret->size());
	WriteFileLang((char*)(wasm_state->wasm_dir + wasm_state->folder_name + "_data_sect").c_str(), lang_stat->data_sect.begin(), lang_stat->data_sect.size());

	if (wasm_state->lang_stat->release)
	{
		int read = 0;
		char* images_data = ReadEntireFileLang("../web/images.data", &read);

		std::string code_str((char*)ret->begin(), ret->size());
		std::string encoded_str = base64_encode(code_str);
		std::string data_sect_str((char*)lang_stat->data_sect.begin(), lang_stat->data_sect.size());
		std::string data_sect_encoded_str = base64_encode(data_sect_str);

		std::string page =
			std::string(
				"<!DOCTYPE html>\n\
<html>\
<script src = \"main.js\"></script>\
<script src = \"https://greggman.github.io/webgl-lint/webgl-lint.js\" crossorigin></script>\
<audio id = \"music\">\
<source src = \"10.mp3\" type = \"audio/mp3\">\
Your browser does not support the audio element.\
</audio>\
<canvas id = \"glcanvas\" width = \"640\" height = \"480\">< / canvas>\
<p id = \"dt\">< / p>\
<p id = \"wasm_module\">") + encoded_str + std::string("</p>\
<p id = \"wasm_data_sect\">") + data_sect_encoded_str + std::string("</p>\
<p id = \"wasm_images\">") + images_data + std::string("</p>\
</html>\
");

		WriteFileLang((char *)(wasm_state->wasm_dir+"index.html").c_str(), (char*)page.data(), page.size());
	}

	for (int i = wasm_state->imports.size() - 1; i >=0 ; i--)
	{
		decl2* d = *(wasm_state->imports.begin() + i);

		if (d->type.type == TYPE_FUNC)
			wasm_state->funcs.insert(0, d->type.fdecl);
	}

	int i = 0;
	FOR_VEC(decl, wasm_state->funcs)
	{
		func_decl* d = *decl;
		
		std::string st = d->name;
		//printf("%d: %s\n", i, st.c_str());

		i++;
	}


#ifndef LANG_NO_ENGINE
	if(!wasm_state->lang_stat->release)
		WasmSerialize(wasm_state, final_code_sect, bcs2);
#endif

	//WasmInterp(final_code_sect, buffer, mem_size, "wasm_test_func_ptr", wasm_state, args, 3);

	//WriteFileLang("../../wabt/test.html", (void*)page.data(), page.size());
	//void FromIRToAsm()
}
#pragma optimize("", on)

void CreateAstFromFunc(lang_state* lang_stat, func_decl* f)
{
	f->func_node->fdecl = f;
	ast_rep* ast = AstFromNode(lang_stat, f->func_node, f->scp);
	f->ast = ast;
	ASSERT(ast->type == AST_FUNC);
	own_std::vector<ir_rep>* ir = (own_std::vector<ir_rep> *) & f->ir;
	ir->reserve(128);
	GetIRFromAst(lang_stat, ast, ir);
}

struct compile_options
{
	std::string file;
	std::string wasm_dir;
	std::string folder_name;
	bool release;
	bool run;
};

void AssignDbgFile(lang_state* lang_stat, std::string file_name)
{
	int read;
	web_assembly_state *wasm_state = lang_stat->wasm_state;
	lang_stat->data_sect.clear();
	lang_stat->globals_sect.clear();
	wasm_state->lang_stat = lang_stat;
	wasm_state->funcs.clear();
	wasm_state->imports.clear();
	wasm_state->outsiders.clear();
	auto file = (unsigned char *)ReadEntireFileLang((char *)file_name.c_str(), &read);

	

	auto dfile = (dbg_file_seriealize*)(file);
	wasm_interp &winterp = *lang_stat->winterp;
	lang_stat->bcs2_start = (byte_code2 *)(((char*)(dfile + 1)) + dfile->bc2_sect);
	lang_stat->bcs2_end = (byte_code2 *)(((char*)(dfile + 1)) + dfile->bc2_sect) + dfile->bc2_sect_size;

#ifndef LANG_NO_ENGINE
	WasmInterpInit(&winterp, file, read, lang_stat);
#endif

}
void AssignOutsiderFunc(lang_state* lang_stat, std::string name, OutsiderFuncType func)
{
	lang_stat->winterp->outsiders[name] = func;
}

char* heap_alloc(mem_alloc* alloc, int size);//, mem_chunk **out = nullptr)
void AddFolder(lang_state* lang_stat, std::string folder)
{
	own_std::vector<std::string> file_contents;
	own_std::vector<char *> file_names;
	GetFilesInDirectory(lang_stat->exe_dir+folder, nullptr, &file_names);

	if (file_names.size() == 0)
	{
		printf("no files found in the specified directory");
		ExitProcess(0);
	}


	FOR_VEC(str, file_names)
	{
		TCHAR name_buffer[MAX_PATH];
		std::string dir = lang_stat->exe_dir + folder+"\\";
		for(int i= 0; i < dir.size(); i++)
		{
			if (dir[i] == '/')
				dir[i] = '\\';
		}
		int res = GetFullPathName((char*)dir.c_str(), MAX_PATH, name_buffer, nullptr);
		dir = name_buffer;
		
		AddNewFile(lang_stat, *str, dir);
	}
}
struct code_info
{
	lang_state* lang_stat;
	wasm_gen_state* gen_state;
	own_std::vector<ir_rep> ir;
	own_std::vector<byte_code> bcs;
	machine_code mcode;
};

int ExecuteString(code_info *info, std::string str, int param)
{

	printf("NEW TASK");
	scope* scp = NewScope(info->lang_stat, info->lang_stat->root);
	scp->flags |= SCOPE_IS_GLOBAL;
	scope* root = scp;
	node *stat1 = ParseString(info->lang_stat, str);
	info->lang_stat->cur_file->global = scp;

	int its = 0;
	while (true)
	{
		DescendNameFinding(info->lang_stat, stat1, scp);
		its++;
		if (its > 2)
			break;
	}
	DescendNode(info->lang_stat, stat1, scp);

	func_decl* start_func = nullptr;

	FOR_VEC(decl, scp->vars)
	{
		auto d = *decl;
		if (d->type.type != TYPE_FUNC)
			continue;

		func_decl* fdecl = d->type.fdecl;
		ast_rep* ast = AstFromNode(info->lang_stat, fdecl->func_node, scp);
		auto ir = (own_std::vector<ir_rep>*) &fdecl->ir;
		GetIRFromAst(info->lang_stat, ast, ir);

		info->lang_stat->global_funcs.emplace_back(fdecl);

		if (fdecl->name == "start")
			start_func = fdecl;
	}
	FOR_VEC(decl, scp->vars)
	{
		auto d = *decl;
		if (d->type.type != TYPE_FUNC)
			continue;

		func_decl* fdecl = d->type.fdecl;
		info->gen_state->cur_func = fdecl;

		auto ir = (own_std::vector<ir_rep>*) &fdecl->ir;

		info->lang_stat->dstate->print_numbers_format = dbg_print_numbers_format::DBG_PRINT_HEX;
		info->lang_stat->dstate->cur_func = fdecl;
		std::string all;
		for (int i = 0; i < ir->size(); i++)
		{
			char buffer[512];
			ir_rep* ir_cur = ir->data() + i;
			ir_cur->idx = info->lang_stat->cur_idx;
			std::string ir_str;
			WasmIrToString(info->lang_stat->dstate, ir_cur, ir_str);
			if (ir_str.size() != 0)
			{
				snprintf(buffer, 512, "\t%d: %s", ir_cur->idx, ir_str.c_str());
				all += buffer;
			}
			if (ir_cur->type == IR_BEGIN_STMNT)
				all += "\n";
			info->lang_stat->cur_idx++;
		}
		printf("\n\n*******\nfunc_name: %s\n%s", fdecl->name.c_str(), all.c_str());

		GenX64BytecodeFromIR(info->lang_stat, info->bcs, *(own_std::vector<ir_rep>*)&fdecl->ir, info->gen_state);
		fdecl->flags |= FUNC_DECL_CODE_WAS_GENERATED;

	}
	GenX64(info->lang_stat, info->bcs, info->mcode);

	CompleteMachineCode(info->lang_stat, info->mcode);
	int tsize = info->lang_stat->type_sect.size();
	int reached = 0;
	int func_addr_int = start_func->code_start_idx;
	char* func_addr = (char *)(info->lang_stat->code_sect.data() + info->lang_stat->type_sect.size() + func_addr_int);
	info->bcs.clear();
	info->ir.clear();
	info->mcode.call_rels.clear();
	info->mcode.code.clear();
	info->mcode.rels.clear();
	info->mcode.symbols.clear();
	info->mcode.jmp_rels.clear();
	info->lang_stat->data_sect.clear();
	info->lang_stat->type_sect.clear();
	info->lang_stat->code_sect.clear();
	info->lang_stat->global_funcs.clear();
	info->lang_stat->funcs_scp->ClearDecls();
	info->lang_stat->global_funcs.clear();
	return ((int(*)(int))func_addr)(param);
}


void AssertFuncByteCode(lang_state* lang_stat)
{
	code_info info;
	wasm_gen_state gen_state;
	info.gen_state = &gen_state;
	info.lang_stat = lang_stat;
	int val = 0;

	lang_stat->cur_idx = 0;
	lang_stat->gen_type = gen_enum::GEN_X64;

	lang_stat->cur_file = new unit_file();
	lang_stat->cur_file->funcs_scp = NewScope(lang_stat, nullptr);


	val = ExecuteString(&info, "start::fn(a : s32) ! s32{\n\
		d := 2;\n\
		d = d * 2;\n\
		if d != 0x4\n\
		{\n\
			return -1;\n\
		}\n\
		d = d * d;\n\
		if d != 16\n\
		{\n\
			return -1;\n\
		}\n\
		return a + d;\n\
	}", 2);
	ASSERT(val == 18)

		val = ExecuteString(&info, "start::fn(a : s32) ! s32{\n\
			d: = 0;\n\
			sz : = 4;\n\
			while d < sz{\n\
				d++;\n\
			}\n\
			if !(d == sz && d == 4){\n\
				return -1;\n\
			}\n\
		return a;\n\
	}", 1);
	ASSERT(val == 1)

		val = ExecuteString(&info, "start::fn(a : s32) ! s32{\n\
		a = 0x100;\n\
		c: = &a;\n\
		e: = *cast(*u8)(cast(u64)(c) + 1);\n\
		return cast(s32)e;\n\
	}", 0);

	ASSERT(val == 1)

		val = ExecuteString(&info, "start::fn(a : s32) ! s32{\n\
		c := 0;\n\
		ptr := &c;\n\
		*cast(*s32) (cast(u64)ptr + 2 * 1) = 4;\n\
		if c != 0x40000{\n\
			return -1;\n\
		}\n\
		return a + 4;\n\
	}\n\
", 1);
	ASSERT(val == 5)


		val = ExecuteString(&info, "start::fn(a : s32) ! s32{\
		ptr := &a;\
		*ptr = 5;\
		return a;\
	}", 1);
	ASSERT(val == 5)

		val = ExecuteString(&info, "start::fn(a : s32) ! s32{\
		ptr := &a;\
		b := 3;\
		*ptr = b;\
		return a;\
	}", 1);
	ASSERT(val == 3)

		val = ExecuteString(&info, "start::fn(a : s32) ! s32{\
		ptr := &a;\
		b := 3;\
		*ptr = b;\
		if *ptr != 3\n\
			return -1;\n\
		return a;\
	}", 1);

	val = ExecuteString(&info, "start::fn(a : s32) ! s32{\
		ptr := &a;\
		if ptr != &a\n\
			return -1;\n\
		return a;\
	}", 1);

	ASSERT(val == 1)

		val = ExecuteString(&info, "start::fn(a : s32) ! s32{return a;}", 1);
	ASSERT(val == 1)

		val = ExecuteString(&info, "start::fn(a : s32) ! s32{\
		b := a + 1;\
		return b;\
	}", 1);
	ASSERT(val == 2)

		info.lang_stat->execute_id = 4;

	val = ExecuteString(&info, "start::fn(a : s32) ! s32{\n\
		if a == 0{\n\
			a++;\n\
		}\n\
		if a == 2{a = 5;}\n\
		else{ a = 4;}\n\
		return a;\n\
	}", 0);
	ASSERT(val == 4)

		val = ExecuteString(&info, "start::fn(a : s32) ! s32{\n\
		f:=1.0;\n\
		if f < 0.9 || f > 1.0{\n\
			return -1;\n\
		}\n\
		return 1;\n\
	}", 0);
	ASSERT(val == 1)

		val = ExecuteString(&info, "start::fn(a : s32) ! s32{\n\
		i:=0;\n\
		while i < a{\n\
			i++;\n\
		}\n\
		return i;\n\
	}", 4);
	ASSERT(val == 4)
		val = ExecuteString(&info, "start::fn(a : s32) ! s32{\n\
		i:=0;\n\
		d:= 1;\n\
		c:= a + 1;\n\
		while i < c{\n\
			i++;\n\
		}\n\
		return i;\n\
	}", 4);
	ASSERT(val == 5)

		val = ExecuteString(&info, "start::fn(a : s32) ! s32{\n\
		c := cast(*s32)7;\n\
		if cast(u64)c != 7{\n\
			return -1;\n\
		}\n\
		return cast(s32)cast(u64)(c) + a;\n\
	}", 1);
	ASSERT(val == 8)

		val = ExecuteString(&info, "\
		start::fn(a : s32) ! s32{\n\
			d := 2;\n\
			ModifyPtr(&d);\n\
			return d;\n\
		}\n\
		ModifyPtr::fn(s : *s32)\n\
		{\n\
			*s = 4;\n\
		}\n\
		", 1);
	ASSERT(val == 4)

		val = ExecuteString(&info, "\
		start::fn(a : s32) ! s32{\n\
			d := 0;\n\
			c := &d;\n\
			c = cast(*s32)(cast(u64)c + 4);\n\
			if cast(u64)c != (cast(u64)(&d) + 4)\n\
			{\n\
				return -1;\n\
			}\n\
			return 1;\n\
		}\n\
		", 1);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		start::fn(a : s32) ! s32{\n\
			d:= 0x100;\n\
			g:= 0x100;\n\
			ptr: = &d;\n\
			c := &g;\n\
			if (*cast(*u8)(cast(u64)ptr + 1)) != (*cast(*u8)(cast(u64)c + 1))\n\
			{\n\
				return -1;\n\
			}\n\
			return 1;\n\
		}\n\
		", 1);
	ASSERT(val == 1)

		// HERE
		val = ExecuteString(&info, "\
		start::fn(a : s32) ! s32{\n\
			d:= 0;\n\
			g:= 4;\n\
			ptr: = &d;\n\
			c := &g;\n\
			*cast(*u8)(cast(u64)(ptr) + 2) = *cast(*u8)(cast(u64)(c) + 0);\n\
			if *cast(*u8)(cast(u64)(ptr) + 2) != 4\n\
			{\n\
				return -1;\n\
			}\n\
			return 1;\n\
		}\n\
		", 1);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		start::fn(a : s32) ! s32{\n\
			d := 2;\n\
			ModifyPtrArbitraryByte(&d, 1, 3);\n\
			return a + d;\n\
		}\n\
		ModifyPtrArbitraryByte::fn x64(s : *s32, idx : u32, val : u8)\n\
		{\n\
			*cast(*u8)(cast(u64)s + idx * 1) = val;\n\
		}\n\
		", 1);
	ASSERT(val == 0x303)

		val = ExecuteString(&info, "\
		test_strct:struct\n\
		{\n\
			f : f32,\n\
		}\n\
		ModifyStrctPtr::fn x64(s : *test_strct)\n\
		{\n\
			s.f = 3.0;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			s:test_strct= ?;\n\
			s.f = 2.0;\n\
			if s.f < 1.9 || s.f > 2.1\n\
			{\n\
				return -1;\n\
			}\n\
			return a;\n\
		}\n\
		", 1);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		other_strct:struct\n\
		{\n\
			b : f32,\n\
		}\n\
		test_strct:struct\n\
		{\n\
			f : f32,\n\
			a : other_strct,\n\
			ptr : *other_strct,\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			s:test_strct= ?;\n\
			s.ptr = &s.a;\n\
			s.ptr.b = 6.0;\n\
			if s.a.b < 5.9 || s.a.b > 6.1\n\
			{\n\
				return -1;\n\
			}\n\
			s.a.b = 5.0;\n\
			if s.a.b < 4.9 || s.a.b > 5.1\n\
			{\n\
				return -1;\n\
			}\n\
			s.ptr = nil;\n\
			if s.ptr != nil\n\
			{\n\
				return -1;\n\
			}\n\
			return a;\n\
		}\n\
		", 1);
	ASSERT(val == 1)



		val = ExecuteString(&info, "\
		memcpy::fn(dst : *void, src : *void, sz : u64) !void\n\
		{\n\
			i:u64 = 0;\n\
			while i < sz\n\
			{\n\
				*cast(*u8)(cast(u64)(dst) + i) = *cast(*u8)(cast(u64)(src) + i);\n\
				i++;\n\
			}\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			f32_ar: = []f32{ 1.0, 2.0, 3.0 };\n\
			f := *f32_ar[0] + *f32_ar[1] + *f32_ar[2];\n\
			if f < 5.9 || f > 6.1\n\
			{\n\
				return -1;\n\
			}\n\
			return 1;\n\
		}\n\
		", 1);
	ASSERT(val == 1)


		val = ExecuteString(&info, "\
		st :struct\n\
		{\n\
			a : f32,\n\
			b : f32,\n\
			c : f32,\n\
			d : s32,\n\
		}\n\
		test::fn(a : *st, b : *st, c : *st) !s32\n\
		{\n\
			a.d = 1;\n\
			return 2;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			b :st= ?;\n\
			ret := test(&b, &b, &b);\n\
			ptr := &b;\n\
			ret += test(ptr, ptr, &b);\n\
			return ret + ptr.d;\n\
		}\n\
		", 1);
	ASSERT(val == 5)

		val = ExecuteString(&info, "\
		st :struct\n\
		{\n\
			a : s32,\n\
			ptr : *u8,\n\
		}\n\
		test::fn() !*s32\n\
		{\n\
			return cast(*s32)2;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			b :st= ?;\n\
			(cast(*st)(cast(u64)&b)).ptr = cast(*u8)test();\n\
			if cast(u64)b.ptr != 2\n\
			{\n\
				return -1;\n\
			}\n\
			return a;\n\
		}\n\
		", 3);
	ASSERT(val == 3)

		val = ExecuteString(&info, "\
		test::fn(f : f32) ! f32\n\
		{\n\
			return f + 1.0;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			f := test(1.0);\n\
			if f < 1.9 || f > 2.1\n\
				return -1;\n\
			f = test(f);\n\
			if f < 2.9 || f > 3.1\n\
				return -1;\n\
			return 1;\n\
		}\n\
		", 0);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		st :struct\n\
		{\n\
			f1 : f32,\n\
			f2 : f32,\n\
		}\n\
		test::fn(v : *st) ! f32\n\
		{\n\
			return v.f1 * v.f2;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			v : st=?;\n\
			v.f1 = 2.0;\n\
			v.f2 = 2.0;\n\
			f: = test(&v);\n\
			if f < 3.9 || f > 4.0\n\
				return -1;\n\
			return 1;\n\
		}\n\
		", 0);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		test::fn(v : *f32) ! f32\n\
		{\n\
			return *v + 1.0;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			f1: = 2.0;\n\
			f1 = test(&f1);\n\
			if f1 < 2.9 || f1 > 3.1\n\
				return -1;\n\
			return 1;\n\
		}\n\
		", 0);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		test::fn(v : *f32)\n\
		{\n\
			*v = 1.0;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			f1: = 2.0;\n\
			test(&f1);\n\
			if f1 < 0.9 || f1 > 1.1\n\
				return -1;\n\
			return 1;\n\
		}\n\
		", 0);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		st :struct\n\
		{\n\
			a : s32,\n\
			ptr : **st,\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			b :st= ?;\n\
			c :st= ?;\n\
			buffer : u64= cast(u64)&b;\n\
			c.ptr = cast(**st)&buffer;\n\
			c.ptr.a = 3;\n\
			if b.a != 3\n\
			{\n\
				return -1;\n\
			}\n\
			return a + c.ptr.a;\n\
		}\n\
		", 1);
	ASSERT(val == 4)

		val = ExecuteString(&info, "\
		st :struct\n\
		{\n\
			a : s32,\n\
			ptr : **st,\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			b :st= ?;\n\
			c :st= ?;\n\
			buffer : u64= cast(u64)&b;\n\
			c.ptr = cast(**st)&buffer;\n\
			b.a = 5;\n\
			(cast(**st)(cast(u64)c.ptr)).a = 3;\n\
			if b.a != 3\n\
			{\n\
				return -1;\n\
			}\n\
			return a + c.ptr.a;\n\
		}\n\
		", 1);
	ASSERT(val == 4)

		val = ExecuteString(&info, "\
		start::fn(a : s32) ! s32{\n\
		fsum := 0.0;\n\
		*cast(*s32)&fsum = 0x40000000;\n\
		if fsum < 1.9 || fsum > 2.1\n\
			return -1;\n\
		return a;\n\
		}\n\
		", 1);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		start::fn(a : s32) ! s32{\n\
		fsum := 0.0;\n\
		fptr := &fsum;\n\
		*cast(*s32)fptr = 0x40000000;\n\
		if fsum < 1.9 || fsum > 2.1\n\
			return -1;\n\
		return a;\n\
		}\n\
		", 2);
	ASSERT(val == 2)


		val = ExecuteString(&info, "\
		memcpy::fn(dst : *void, src : *void, sz : u64) !void\n\
		{\n\
			i:u64 = 0;\n\
			while i < sz\n\
			{\n\
				*cast(*u8)(cast(u64)(dst) + i) = *cast(*u8)(cast(u64)(src) + i);\n\
				i++;\n\
			}\n\
		}\n\
		st :struct\n\
		{\n\
			a : s32,\n\
		}\n\
		ret_strct::fn() ! st\n\
		{\n\
			ret : st= ?;\n\
			ret.a = 5;\n\
			return ret;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			ret := ret_strct();\n\
			return ret.a + a;\n\
		}\n\
		", 1);
	ASSERT(val == 6)

		val = ExecuteString(&info, "\
		test_strct:struct\n\
		{\n\
			f : f32,\n\
			g : f32,\n\
		}\n\
		ModifyStrctPtr::fn x64(v1 : *test_strct, v2 : *test_strct)\n\
		{\n\
			v1.f = v1.f * v2.g + v1.g * v2.f + v1.g * v2.f;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			s:test_strct= ?;\n\
			ModifyStrctPtr(&s, &s);\n\
			return a;\n\
		}\n\
		", 1);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		memcpy::fn(dst : *void, src : *void, sz : u64) !void\n\
		{\n\
			i:u64 = 0;\n\
			while i < sz\n\
			{\n\
				*cast(*u8)(cast(u64)(dst) + i) = *cast(*u8)(cast(u64)(src) + i);\n\
				i++;\n\
			}\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			ar:= []s32{1, 2, 3, 4};\n\
			ret:= 0;\n\
			for i in ar\n\
			{\n\
				ret += *i;\n\
			}\n\
			return ret;\n\
		}\n\
		", 1);
	ASSERT(val == 10)

		val = ExecuteString(&info, "\
		start::fn(a : s32) ! s32{\n\
			v1:= 1;\n\
			v2:= 2;\n\
			v3 := cast(u64)v1;\n\
			if v3 != 1\n\
				return -1;\n\
			return a;\n\
		}\n\
		", 1);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		v3 : struct\n\
		{\n\
			x : f32,\n\
			y : f32,\n\
			z : f32,\n\
		}\n\
		operator + x64(self : *v3, rhs : *v3) !v3\n\
		{\n\
			ret:v3=?;\n\
			ret.x = self.x + rhs.x;\n\
			ret.y = self.y + rhs.y;\n\
			ret.z = self.z + rhs.z;\n\
			return ret;\n\
		}\n\
		operator * x64(self : *v3, f : f32) !v3\n\
		{\n\
			ret:v3=?;\n\
			ret.x = self.x * f;\n\
			ret.y = self.y * f;\n\
			ret.z = self.z * f;\n\
			return ret;\n\
		}\n\
		memcpy::fn(dst : *void, src : *void, sz : u64) !void\n\
		{\n\
			i:u64 = 0;\n\
			while i < sz\n\
			{\n\
				*cast(*u8)(cast(u64)(dst) + i) = *cast(*u8)(cast(u64)(src) + i);\n\
				i++;\n\
			}\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			v1:v3=?;\n\
			v2:v3=?;\n\
			v1.x = 1.0;\n\
			v2.x = 1.0;\n\
			v1_ptr:= &v1;\n\
			v2_ptr:= &v2;\n\
			v3_:v3=?;\n\
			v3_= *v1_ptr + *v2_ptr;\n\
			if v3_.x < 1.9 || v3_.x > 2.1\n\
				return -2;\n\
			__dbg_break;\n\
			v3_= v1 + v2 * 3.0;\n\
			if v3_.x < 3.9 || v3_.x > 4.1\n\
				return -1;\n\
			return a + 1;\n\
		}\n\
		", 1);
	ASSERT(val == 2)

		val = ExecuteString(&info, "\
		ModifyStrctPtr::fn x64(a : u32, b : u32, c : u32, d : u32, e : u32, f : *f32)\n\
		{\n\
			*f = 3.0 + cast(f32)e;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			f : f32;\n\
			ModifyStrctPtr(1, 2, 3, 4, 5, &f);\n\
			if f < 7.9 || f > 8.1\n\
			{\n\
				return -1;\n\
			}\n\
			return a;\n\
		}\n\
		", 1);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		ModifyStrctPtr::fn x64(a : u32, b : u32, c : u32, d : u32, e : *u32, f : *f32)\n\
		{\n\
			*f = 3.0 + cast(f32)*e;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			f : f32;\n\
			i :u32= 5;\n\
			ModifyStrctPtr(1, 2, 3, 4, &i, &f);\n\
			if f < 7.9 || f > 8.1\n\
			{\n\
				return -1;\n\
			}\n\
			return a;\n\
		}\n\
		", 1);
	ASSERT(val == 1)

		val = ExecuteString(&info, "\
		ModifyStrctPtr::fn x64(a : *u32) ! u32\n\
		{\n\
			if *a == 5\n\
				return 1;\n\
			return 2;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			i :u32= 5;\n\
			ret := ModifyStrctPtr(&i);\n\
			if ret != 1\n\
			{\n\
				return -1;\n\
			}\n\
			i = 2;\n\
			ret = ModifyStrctPtr(&i);\n\
			if ret != 2\n\
			{\n\
				return -2;\n\
			}\n\
			return a;\n\
		}\n\
		", 3);
	ASSERT(val == 3)

		val = ExecuteString(&info, "\
		v3 : struct\n\
		{\n\
			x : f32,\n\
			y : f32,\n\
			z : f32,\n\
		}\n\
		absf::fn x64(a :f32) !f32\n\
		{\n\
			if a < 0.0 return -a;\n\
			return a;\n\
		}\n\
		NearlyEqualV3::fn x64(a: *v3, b : *v3) !bool\n\
		{\n\
			return NearlyEqualF32(a.x, b.x) && NearlyEqualF32(a.y, b.y) && NearlyEqualF32(a.z, b.z);\n\
		}\n\
		NearlyEqualF32::fn x64(a:f32, b : f32) !bool\n\
		{\n\
			return absf(a - b) < 0.005;\n\
		}\n\
		start::fn(arg : s32) ! s32{\n\
			a :v3=?;\n\
			b :v3=?;\n\
			a.x = 0.1;\n\
			a.y = 0.1;\n\
			a.z = 0.1;\n\
			b.x = 0.100002;\n\
			b.y = 0.1;\n\
			b.z = 0.1;\n\
			if !NearlyEqualF32(a.x, b.x)\n\
				return -1;\n\
			if !NearlyEqualF32(a.y, b.y)\n\
				return -2;\n\
			if !NearlyEqualV3(&a, &b)\n\
				return -3;\n\
			return arg;\n\
		}\n\
		", 3);
	ASSERT(val == 3)

		val = ExecuteString(&info, "\
		v3 : struct\n\
		{\n\
			x : f32,\n\
			y : f32,\n\
			z : f32,\n\
		}\n\
		DoTwoLinesIntersect::fn(startA : *v3, endA : *v3, startB : *v3, endB : *v3, p : *v3) !bool\n\
		{\n\
			x1:f32;\n\
			x2:f32; \n\
			y1:f32; \n\
			y2:f32; \n\
			x3:f32;\n\
			x4:f32;\n\
			y3:f32; \n\
			y4:f32;\n\
		\n\
			x1 = startA.x;\n\
			x2 = endA.x;\n\
			y1 = startA.y;\n\
			y2 = endA.y;\n\
		\n\
			x3 = startB.x;\n\
			x4 = endB.x;\n\
			y3 = startB.y;\n\
			y4 = endB.y;\n\
		\n\
			__dbg_break;\n\
			den:= (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);\n\
			if (den == 0.0)\n\
			{\n\
				return false;\n\
			}\n\
		\n\
			t := ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;\n\
		\n\
			u := ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / den;\n\
		\n\
			if (t > 0.0 && t < 1.0 && u > 0.0 && u < 1.0)\n\
			{\n\
				p.x = startA.x + (endA.x - startA.x) * t;\n\
				p.y = startA.y + (endA.y - startA.y) * t;\n\
				return true;\n\
			}\n\
			return false;\n\
		}\n\
		start::fn(a : s32) ! s32{\n\
			i :u32= 5;\n\
			startA :v3=?;\n\
			endA :v3=?;\n\
			startB :v3=?;\n\
			endB :v3=?;\n\
			p :v3=?;\n\
			startA.x = 0.0;\n\
			startA.y = 0.0;\n\
			endA.x = 0.0;\n\
			endA.y = 1.0;\n\
			startB.x = 0.5;\n\
			startB.y = 0.5;\n\
			endB.x = -1.0;\n\
			endB.y = 0.5;\n\
			__dbg_break;\n\
			ret :=DoTwoLinesIntersect(&startA, &endA, &startB, &endB, &p);\n\
			if ret != true\n\
			{\n\
				return -1;\n\
			}\n\
			return a;\n\
		}\n\
		", 3);
	ASSERT(val == 3)
	ASSERT(false);
	int a = 0;
	lang_stat->gen_type = gen_enum::GEN_WASM;
}
int Compile(lang_state* lang_stat, compile_options *opts)
{
	//own_std::vector<std::string> args;
	//std::string aux;
	//split(args_str, ' ', args, &aux);
	
	//AssertFuncByteCode(lang_stat);
	
	int i = 0;
	std::string file = opts->file;
	std::string wasm_dir = opts->wasm_dir;
	std::string target = "";

	lang_stat->gen_wasm = true;
	lang_stat->release = opts->release;
	lang_stat->wasm_state->folder_name = std::string(opts->folder_name);
	lang_stat->wasm_state->wasm_dir = std::string(opts->wasm_dir);
	//auto base_fl = AddNewFile(lang_stat, "Core/base.lng");
	//tp.type = enum_type2::TYPE_IMPORT;
	//tp.imp = NewImport(lang_stat, import_type::IMP_IMPLICIT_NAME, "", base_fl);
	//lang_stat->base_lang = NewDecl(lang_stat, "base", tp);

	type2 dummy_type;
	decl2* release = FindIdentifier("RELEASE", lang_stat->root, &dummy_type);
	release->type.i = lang_stat->release;



	

	timer tmr;
	InitTimer(&tmr);
	StartTimer(&tmr);
	TCHAR buffer[MAX_PATH] = { 0 };
	GetFullPathName(file.c_str(), MAX_PATH, buffer, nullptr);
	lang_stat->work_dir = buffer;

	AddFolder(lang_stat, file);

	if (lang_stat->files.size() == 0)
	{
		printf("no files to be compiled, will exit\n");
		ExitProcess(0);
	}
	//printf("files added");
	FOR_VEC(i1, lang_stat->files)
	{
		type2 tp;
		tp.type = enum_type2::TYPE_IMPORT;
		tp.imp = NewImport(lang_stat, import_type::IMP_IMPLICIT_NAME, "", *i1);
		FOR_VEC(i2, lang_stat->files)
		{
			if (*i1 == *i2)
				continue;


			(*i2)->global->imports.emplace_back(NewDecl(lang_stat, "__import", tp));
		}
		//LookingForLabels(lang_stat, (*i1)->s, (*i1)->global);
	}
	//AddNewFile(lang_stat, "Core/tests.lng");
	//AddNewFile(lang_stat, "Core/player.lng");
	//AddNewFile(file_name);
	

	int cur_f = 0;

	int iterations = 0;
	bool can_continue = false;
	struct info_not_found
	{
		node* nd;
		scope* scp;
	};
	own_std::vector<info_not_found>names_not_found;
	type2 dummy;
	EndTimer(&tmr);
	printf("initial parsing %d\n", GetTimerMS(&tmr));
	StartTimer(&tmr);
	while(true)
	{
		
		lang_stat->something_was_declared = false;
		
		for(cur_f = 0; cur_f < lang_stat->files.size(); cur_f++)
		{
			can_continue = false;
			lang_stat->flags &= ~PSR_FLAGS_SOMETHING_IN_GLOBAL_NOT_FOUND;
			lang_stat->global_decl_not_found.clear();
			auto f = lang_stat->files[cur_f];
			lang_stat->work_dir = f->path;
			lang_stat->cur_file = f;
			if(!DescendNameFinding(lang_stat, f->s, f->global))
				can_continue = true;


#ifdef DEBUG_GLOBAL_NOT_FOUND
	
			if (IS_FLAG_ON(lang_stat->flags, PSR_FLAGS_SOMETHING_IN_GLOBAL_NOT_FOUND))
			{
				FOR_VEC(global_nd, lang_stat->global_decl_not_found)
				{
					node* cur = *global_nd;
					info_not_found info;
					info.nd = cur->l;
					info.scp = f->global;
					names_not_found.emplace_back(info);
				}
				can_continue = true;
			}
#endif

			int a = 0;
			iterations++;
			if (iterations >= 99)
			{
				FOR_VEC(cur, names_not_found)
				{
					decl2* d = FindIdentifier(cur->nd->t->str, cur->scp, &dummy);

					printf("\nglobal decl not found line %d\nstr: %s\n", cur->nd->t->line,
						cur->nd->t->line_str);
					if (d == nullptr)
					{
						printf("\nglobal decl not found line %d\nstr: %s\n was found %d", cur->nd->t->line,
							cur->nd->t->line_str, d != nullptr);
					}
					if (d && IS_FLAG_ON(d->flags, DECL_NOT_DONE))
					{
						printf("\nglobal decl not done line %d\nstr: %s\n was found %d", cur->nd->t->line,
							cur->nd->t->line_str, d != nullptr);
					}
					if (d && d->type.type == TYPE_STRUCT_TYPE && IS_FLAG_ON(d->type.strct->flags, TP_STRCT_STRUCT_NOT_NODE))
					{
						printf("\nglobal struct not done line %d\nstr: %s\n was found %d", cur->nd->t->line,
							cur->nd->t->line_str, d != nullptr);
					}

				}
				can_continue = false;
				break;
			}
		}
		if(!lang_stat->something_was_declared && !can_continue)
			break;
	}
#ifdef DEBUG_GLOBAL_NOT_FOUND
	printf("names not found!\n");
	FOR_VEC(cur, names_not_found)
	{
		decl2* d = FindIdentifier(cur->nd->t->str, cur->scp, &dummy);

		if (d == nullptr)
		{
			printf("\nglobal decl not found line %d\nstr: %s\n was found %d", cur->nd->t->line,
				cur->nd->t->line_str, d != nullptr);
		}

	}
#endif
	lang_stat->flags = PSR_FLAGS_REPORT_UNDECLARED_IDENTS;

	for(cur_f = 0; cur_f < lang_stat->files.size(); cur_f++)
	{
		auto f = lang_stat->files[cur_f];
		lang_stat->cur_file = f;
		DescendNameFinding(lang_stat, f->s, f->global);
	}
	//DescendIndefinedIdents(s, &global);
	
	if(IS_FLAG_ON(lang_stat->flags, PSR_FLAGS_ERRO_REPORTED))
		ExitProcess(1);

	EndTimer(&tmr);
	printf("name finding parsing %d, find ident %d\n", GetTimerMS(&tmr), (u32)__lang_globals.find_ident_timer);
	__lang_globals.find_ident_timer = 0;


	lang_stat->flags |= PSR_FLAGS_ASSIGN_SAVED_REGS;
	lang_stat->flags |= PSR_FLAGS_AFTER_TYPE_CHECK;

	//CreateBaseFileCode(lang_stat);

    // comp time function 
    for ( auto it = lang_stat->comp_time_funcs.begin(); it != lang_stat->comp_time_funcs.end(); ++it )
    {

        func_decl *fdecl = it->second;
		auto f = fdecl->from_file;
		lang_stat->cur_file = f;
		lang_stat->lhs_saved = 0;
		lang_stat->call_regs_used = 0;
		DescendNode(lang_stat, fdecl->func_node, fdecl->scp);

		ast_rep * ast = AstFromNode(lang_stat, fdecl->func_node, fdecl->scp);
		GetIRFromAst(lang_stat, ast, (own_std::vector<ir_rep> *)& fdecl->ir);


        //print_key_value(key, value);
    }
	
	StartTimer(&tmr);
	__lang_globals.find_ident_timer = 0;
	for(cur_f = 0; cur_f < lang_stat->files.size(); cur_f++)
	{
		auto f = lang_stat->files[cur_f];
		lang_stat->cur_file = f;
		lang_stat->lhs_saved = 0;
		lang_stat->call_regs_used = 0;
		//std::string scp_str = lang_stat->root->Print(0);
		//printf("file: %s, scp: \n %s", f->name.c_str(), scp_str.c_str());
		DescendNode(lang_stat, f->s, f->global);

		/*
		TypeCheckTree(f->s, f->global, []()
		{
			while (true)
			{
				auto msg = works.GetMessageA();
				if (!msg) continue;
				if (msg->type == msg_type::MSG_DONE) break;
			}
		});
		*/
		lang_stat->call_regs_used = 0;
		//own_std::vector<func_byte_code*>all_funcs = GetFuncs(lang_stat, lang_stat->funcs_scp);
		machine_code code;
		//FromByteCodeToX64(lang_stat, &all_funcs, code);

		//auto exec_funcs = CompleteMachineCode(lang_stat, code);
	}

	EndTimer(&tmr);

	printf("type checking %d\n", GetTimerMS(&tmr));


	char* start_dbg_global_buffer = &lang_stat->dstate->mem_buffer[GLOBALS_OFFSET];
	char* start_vector_global_buffer = lang_stat->globals_sect.data();
	memcpy(start_vector_global_buffer, start_dbg_global_buffer, lang_stat->globals_sect.size());
	

	char msg_hdr[256];
    web_assembly_state &wasm_state = *lang_stat->wasm_state ;
	wasm_state.lang_stat = lang_stat;
	wasm_state.wasm_dir = opts->wasm_dir;

    auto mem_decl = (decl2 *)AllocMiscData(lang_stat, sizeof(decl2));
    mem_decl->name = std::string("mem");
    mem_decl->type.type = TYPE_WASM_MEMORY;
    wasm_state.exports.emplace_back(mem_decl);

	FOR_VEC(f_ptr, lang_stat->outsider_funcs)
	{
		auto f = *f_ptr;
		auto fdecl = (decl2 *)AllocMiscData(lang_stat, sizeof(decl2));
		fdecl->type.type = TYPE_FUNC;
		fdecl->name = std::string(f->name);
		fdecl->type.fdecl = f;
		f->this_decl = fdecl;
		wasm_state.imports.emplace_back(f->this_decl);
	}

	StartTimer(&tmr);
	__lang_globals.find_ident_timer = 0;
	FOR_VEC(cur_f, lang_stat->funcs_scp->vars)
	{
		auto f = *cur_f;
		if (f->type.type != TYPE_FUNC)
			continue;
		auto fdecl = f->type.fdecl;
		if (IS_FLAG_ON(fdecl->flags, FUNC_DECL_MACRO | FUNC_DECL_IS_OUTSIDER | FUNC_DECL_TEMPLATED |FUNC_DECL_INTRINSIC))
			continue;

		if (FuncAddedWasm(&wasm_state, f->name))
			continue;

		AddFuncToWasm(&wasm_state, f->type.fdecl);
		CreateAstFromFunc(lang_stat, f->type.fdecl);
		int  a = 0;

	}
	EndTimer(&tmr);
	printf("Ast gen %d, find ident %d\n", GetTimerMS(&tmr), (u32)__lang_globals.find_ident_timer);

	/*
	FOR_VEC(f_ptr, lang_stat->func_ptrs_decls)
	{
		AddFuncToWasm(&wasm_state, *f_ptr, false);
		CreateAstFromFunc(lang_stat, &wasm_state, *f_ptr);
	}
	*/
	FOR_VEC(f_ptr, lang_stat->global_funcs)
	{
		auto f = *f_ptr;
        //if(f->name == "wasm_test_func_ptr")
        //{
		if (IS_FLAG_ON(f->flags, FUNC_DECL_MACRO | FUNC_DECL_IS_OUTSIDER | FUNC_DECL_TEMPLATED |FUNC_DECL_INTRINSIC))
			continue;
		if (FuncAddedWasm(&wasm_state, f->name))
			continue;

		AddFuncToWasm(&wasm_state, f);
		CreateAstFromFunc(lang_stat, f);
        //}

		if (IS_FLAG_ON(f->flags, NODE_FLAGS_FUNC_TEST))
		{
			auto func = (int(*)(char*, long long*)) lang_stat->GetCodeAddr(f->code_start_idx);
			GetFuncBasedOnAddr(lang_stat, (unsigned long long)func);
			GetFuncAddrBasedOnName(lang_stat, "_own_memset");
			char ret_success = 0;
			long long add_info = 0;
			int reached = func(&ret_success, &add_info);

			lang_stat->cur_file = f->from_file;

			if (ret_success == 0)
			{
				token2* t = f->func_node->t;
				REPORT_ERROR(t->line, t->line_offset, VAR_ARGS("test failed! reached %d, info %Ix\n", reached, add_info))
                ASSERT(0);
			}
		}
	}

    GenWasm(&wasm_state);
	/*
	FOR_VEC(i1, lang_stat->files)
	{
		(*i1)->s->FreeTree();
	}
	*/
}
int InitLang(lang_state *lang_stat, AllocTypeFunc alloc_addr, FreeTypeFunc free_addr, void *data)
{
    __lang_globals.alloc =  alloc_addr;
    __lang_globals.data = data;
    __lang_globals.free = free_addr;
	__lang_globals.total_blocks = 258;
	__lang_globals.blocks = (block_linked*)AllocMiscData(lang_stat, sizeof(block_linked) * __lang_globals.total_blocks);
	__lang_globals.cur_block = 0;
	//auto test = lang_state();
	//*lang_stat = test;
	new(lang_stat)lang_state();
	lang_stat->code_sect.reserve(256);
	//lang_stat->jump_buffer = (jmp_buf*)AllocMiscData(lang_stat, sizeof(jmp_buf));

	lang_stat->winterp = (wasm_interp *) AllocMiscData(lang_stat, sizeof(wasm_interp));
	new(&lang_stat->winterp->outsiders) std::unordered_map<std::string, OutsiderFuncType>();
	lang_stat->wasm_state = (web_assembly_state *) AllocMiscData(lang_stat, sizeof(web_assembly_state));
	lang_stat->dstate = (dbg_state *) AllocMiscData(lang_stat, sizeof(dbg_state));
	lang_stat->dstate->mem_size = 128000;
	lang_stat->dstate->mem_buffer = AllocMiscData(lang_stat, lang_stat->dstate->mem_size);
	lang_stat->dstate->mem_buffer = AllocMiscData(lang_stat, lang_stat->dstate->mem_size);
	int stack_offset = 10000;
	*(int*)&lang_stat->dstate->mem_buffer[STACK_PTR_REG * 8] = stack_offset;
	*(int*)&lang_stat->dstate->mem_buffer[BASE_STACK_PTR_REG * 8] = stack_offset;
	//lang_stat->wasm_state->folder_name = 


	//CreateWindowEx()
	//glfwInit();
	//HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	//SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | BACKGROUND_BLUE);
	//printf("hello");

    
	lang_stat->max_nd = 80000;
	lang_stat->cur_nd = 0;
	lang_stat->node_arena = (node*)AllocMiscData(lang_stat, lang_stat->max_nd * sizeof(node));

	lang_stat->max_decl = 4000;
	lang_stat->cur_decl = 0;
	lang_stat->decl_arena = (decl2*)AllocMiscData(lang_stat, lang_stat->max_decl * sizeof(decl2));
	//lang_stat->max_misc = 16 * 1024 * 1024;
	//lang_stat->misc_arena = (char*)VirtualAlloc(0, lang_stat->max_misc, MEM_COMMIT, PAGE_READWRITE);
    
	/*
	lang_stat->structs = (LangLangArray<type_struct2> *)malloc(sizeof(LangLangArray<int>));
	lang_stat->template_strcts = (LangLangArray<type_struct2> *)malloc(sizeof(LangLangArray<int>));
	memset(lang_stat->structs, 0, sizeof(LangArray<int>));
	memset(lang_stat->template_strcts, 0, sizeof(LangArray<int>));
	*/


	//auto a = ParseString(lang_stat, "a = a * a + a * a + a * a");

	auto dummy_decl = new decl2();
	memset(dummy_decl, 0, sizeof(decl2));

	// adding sizeof builtin
	lang_stat->root = NewScope(lang_stat, nullptr);
	lang_stat->funcs_scp = NewScope(lang_stat, nullptr);

	type2 tp;
	tp.type = enum_type2::TYPE_FUNC;
	func_decl* sz_of_fdecl = (func_decl*)AllocMiscData(lang_stat, sizeof(func_decl));
	memset(sz_of_fdecl, 0, sizeof(func_decl));
	sz_of_fdecl->ret_type.type = enum_type2::TYPE_INT;
	tp.fdecl = sz_of_fdecl;
	tp.fdecl->flags |= FUNC_DECL_INTERNAL;
	tp.fdecl->name = std::string("sizeof");
	tp.fdecl->args.push_back(dummy_decl);

	lang_stat->root->vars.push_back(NewDecl(lang_stat, "sizeof", tp));


	sz_of_fdecl = (func_decl*)AllocMiscData(lang_stat, sizeof(func_decl));
	memset(sz_of_fdecl, 0, sizeof(func_decl));
	tp.fdecl = sz_of_fdecl;
	sz_of_fdecl->flags |= FUNC_DECL_INTERNAL;
	tp.fdecl->name = std::string("enum_count");
	lang_stat->root->vars.push_back(NewDecl(lang_stat, "enum_count", tp));

	sz_of_fdecl = (func_decl*)AllocMiscData(lang_stat, sizeof(func_decl));
	memset(sz_of_fdecl, 0, sizeof(func_decl));
	tp.fdecl = sz_of_fdecl;
	sz_of_fdecl->flags |= FUNC_DECL_INTERNAL;
	tp.fdecl->name = std::string("__is_struct");
	lang_stat->root->vars.push_back(NewDecl(lang_stat, "enum_count", tp));

	lang_stat->root->vars.push_back(NewDecl(lang_stat, "get_type_data", tp));


	tp.type = enum_type2::TYPE_VECTOR_TYPE;
	node *n = ParseString(lang_stat, "__vec_struct : struct{x : f32, y : f32, z : f32, w : f32}");
	decl2 *decl_strct = DescendNameFinding(lang_stat, n, lang_stat->root);
	tp.strct = decl_strct->type.strct;
	lang_stat->root->vars.push_back(NewDecl(lang_stat, "_vec", tp));
	lang_stat->_vec_strct = decl_strct;


	tp.type = enum_type2::TYPE_VOID;
	lang_stat->void_decl = NewDecl(lang_stat, "void", tp);

	tp.type = enum_type2::TYPE_U64;
	lang_stat->i64_decl = NewDecl(lang_stat, "i64", tp);

	tp.type = enum_type2::TYPE_U64;
	lang_stat->u64_decl = NewDecl(lang_stat, "u64", tp);

	lang_stat->func_ptr_decl = NewDecl(lang_stat, "func_ptr_decl", tp);

	tp.type = enum_type2::TYPE_S64;
	lang_stat->s64_decl = NewDecl(lang_stat, "s64", tp);

	tp.type = enum_type2::TYPE_U32;
	lang_stat->u32_decl = NewDecl(lang_stat, "u32", tp);

	tp.type = enum_type2::TYPE_S32;
	lang_stat->s32_decl = NewDecl(lang_stat, "s32", tp);

	tp.type = enum_type2::TYPE_U16;
	lang_stat->s16_decl = NewDecl(lang_stat, "u16", tp);

	tp.type = enum_type2::TYPE_S16;
	lang_stat->u16_decl = NewDecl(lang_stat, "s16", tp);

	tp.type = enum_type2::TYPE_U8;
	lang_stat->u8_decl = NewDecl(lang_stat, "u8", tp);

	tp.type = enum_type2::TYPE_S8;
	lang_stat->s8_decl = NewDecl(lang_stat, "s8", tp);

	tp.type = enum_type2::TYPE_BOOL;
	lang_stat->bool_decl = NewDecl(lang_stat, "bool", tp);

	tp.type = enum_type2::TYPE_F32;
	lang_stat->f32_decl = NewDecl(lang_stat, "f32", tp);

	tp.type = enum_type2::TYPE_F64;
	lang_stat->f64_decl = NewDecl(lang_stat, "f64", tp);

	tp.type = enum_type2::TYPE_CHAR;
	lang_stat->char_decl = NewDecl(lang_stat, "char", tp);

	tp.type = TYPE_U32;
	tp.i = lang_stat->release;
	decl2* glb_dummy = NewDecl(lang_stat, "__global_dummy", tp);
	glb_dummy->flags = DECL_IS_GLOBAL;
	glb_dummy->offset = -8;
	lang_stat->root->vars.push_back(glb_dummy);


	lang_stat->dbg_equal = CreateDbgEqualStmnt(lang_stat);

	

	tp.type = TYPE_INT;
	tp.i = lang_stat->release;
	lang_stat->root->vars.push_back(NewDecl(lang_stat, "RELEASE", tp));
	// inserting builtin types
	{
		NewTypeToSection(lang_stat, "s64", TYPE_S64);
		NewTypeToSection(lang_stat, "s32", TYPE_S32);
		NewTypeToSection(lang_stat, "s16", TYPE_S16);
		NewTypeToSection(lang_stat, "s8", TYPE_S8);
		NewTypeToSection(lang_stat, "u64", TYPE_U64);
		NewTypeToSection(lang_stat, "u32", TYPE_U32);
		NewTypeToSection(lang_stat, "u16", TYPE_U16);
		NewTypeToSection(lang_stat, "u8", TYPE_U8);
		NewTypeToSection(lang_stat, "bool", TYPE_BOOL);
		NewTypeToSection(lang_stat, "void", TYPE_VOID);
		NewTypeToSection(lang_stat, "str_lit", TYPE_STR_LIT);
	}
	
	srand(time(0));
	// getting the compiler exe path 
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);

	//printf("full exe path: %s", buffer);

	// addind the file name to the path
	std::string exe_dir = buffer;
	int last_bar_comp = exe_dir.find_last_of('\\');
	/*
	std::string file_name_dir = std::string(argv[1]);
	int last_bar_main = file_name_dir.find_last_of('/');
	*/
	//std::string dir = exe_dir.substr(0, last_bar_comp) + "\\" + file_name_dir.substr(0, last_bar_main);
	std::string dir = exe_dir.substr(0, last_bar_comp + 1);
	lang_stat->exe_dir = dir;


	//std::string file_name = file_name_dir.substr(last_bar_main+1);

    return 0;
}

func_decl *GetFuncWithLine(lang_state *lang_stat, int line)
{

	FOR_VEC(f_ptr, lang_stat->funcs_scp->vars)
	{
		if ((*f_ptr)->type.type != TYPE_FUNC)
			continue;
		func_decl* f = (*f_ptr)->type.fdecl;
		if(line >= f->scp->line_start && line <= f->scp->line_end)
		{
			return f;
		}
	}
	return nullptr;
}

bool GetDeclOnCursor(lang_state *lang_stat, int line, int offset, type2 *out)
{
	func_decl* found = GetFuncWithLine(lang_stat, line);
	if (!found)
		return false;

	ast_rep* ast_stats = found->ast->ast;
	ASSERT(found->ast && ast_stats->type == AST_STATS);
	scope *scp = FindScpWithLine(found, line);
	ASSERT(scp);

	ast_rep* stat = nullptr;
	FOR_VEC(ast, ast_stats->stats)
	{
		ast_rep* s = *ast;
		if(s->line_number == line)
		{
			stat = s;
			break;
		}
	}

	if (!stat)
		return false;

	ast_rep* exp = nullptr;

	FOR_VEC(ex, stat->expr)
	{
		ast_rep* e = *ex;
		if(offset <= e->line_offset_start)
		{
			exp = e;
		}
	}
	switch (exp->type)
	{
	case AST_IDENT:
	{
		*out = exp->decl->type;
	}break;
	default:
		ASSERT(0);
	}
	//ASSERT(exp->type == AST_IDENT)
	auto a = 0;

	return true;

}

void LspAddFolder(lang_state *lang_stat, std::string folder)
{
	type2 dummy_type;
	decl2* release = FindIdentifier("RELEASE", lang_stat->root, &dummy_type);
	release->type.i = lang_stat->release;



	TCHAR buffer[MAX_PATH] = { 0 };
	GetFullPathName(folder.c_str(), MAX_PATH, buffer, nullptr);
	lang_stat->work_dir = buffer;

	AddFolder(lang_stat, folder);

	if (lang_stat->files.size() == 0)
	{
		printf("no files to be compiled, will exit\n");
		ExitProcess(0);
	}
	//printf("files added");
	FOR_VEC(i1, lang_stat->files)
	{
		type2 tp;
		tp.type = enum_type2::TYPE_IMPORT;
		tp.imp = NewImport(lang_stat, import_type::IMP_IMPLICIT_NAME, "", *i1);
		FOR_VEC(i2, lang_stat->files)
		{
			if (*i1 == *i2)
				continue;


			(*i2)->global->imports.emplace_back(NewDecl(lang_stat, "__import", tp));
		}
	}
}
void LspCompile(lang_state *lang_stat, std::string folder, int line, int line_offset, own_std::vector<decl2> *out_decls)
{
	switch (lang_stat->lsp_stage)
	{
	case LSP_STAGE_ADD_FILES:
	{
		lang_stat->lsp_stage = LSP_STAGE_NAME_FINDING;
	}break;
	case LSP_STAGE_NAME_FINDING:
	{
		int cur_f = 0;

		int iterations = 0;
		bool can_continue = false;
		struct info_not_found
		{
			node* nd;
			scope* scp;
		};
		type2 dummy;
		while (true)
		{

			lang_stat->something_was_declared = false;

			for (cur_f = 0; cur_f < lang_stat->files.size(); cur_f++)
			{
				can_continue = false;
				lang_stat->flags &= ~PSR_FLAGS_SOMETHING_IN_GLOBAL_NOT_FOUND;
				lang_stat->global_decl_not_found.clear();
				auto f = lang_stat->files[cur_f];
				lang_stat->work_dir = f->path;
				lang_stat->cur_file = f;
				if (!DescendNameFinding(lang_stat, f->s, f->global))
					can_continue = true;

			}
			if (!lang_stat->something_was_declared && !can_continue || iterations > 4)
				break;
		}
		if (lang_stat->something_was_declared)
			break;
		lang_stat->flags |= PSR_FLAGS_REPORT_UNDECLARED_IDENTS;

		int val = setjmp(lang_stat->jump_buffer);
		if (val == 0)
		{
			lang_stat->flags |= PSR_FLAGS_ON_JMP_WHEN_ERROR;
			bool error = false;
			for (cur_f = 0; cur_f < lang_stat->files.size(); cur_f++)
			{
				auto f = lang_stat->files[cur_f];
				lang_stat->cur_file = f;
				if (!DescendNameFinding(lang_stat, f->s, f->global))
					error = true;
			}
			//DescendIndefinedIdents(s, &global);

			if (IS_FLAG_ON(lang_stat->flags, PSR_FLAGS_ERRO_REPORTED))
			{
				break;
			}
			lang_stat->flags &= ~PSR_FLAGS_ON_JMP_WHEN_ERROR;
			if(!error)
				lang_stat->lsp_stage = LSP_STAGE_TYPE_CHECKING;
		}
		else
		{
			lang_stat->lsp_stage = LSP_STAGE_PAUSE;
		}

		lang_stat->flags |= PSR_FLAGS_ASSIGN_SAVED_REGS;
		lang_stat->flags |= PSR_FLAGS_AFTER_TYPE_CHECK;

	}break;
	case LSP_STAGE_TYPE_CHECKING:
	{
		//CreateBaseFileCode(lang_stat);

		int val = setjmp(lang_stat->jump_buffer);
		if (val == 0)
		{
			lang_stat->flags |= PSR_FLAGS_ON_JMP_WHEN_ERROR;
			for (int cur_f = 0; cur_f < lang_stat->files.size(); cur_f++)
			{
				auto f = lang_stat->files[cur_f];
				lang_stat->cur_file = f;
				lang_stat->lhs_saved = 0;
				lang_stat->call_regs_used = 0;
				//std::string scp_str = lang_stat->root->Print(0);
				//printf("file: %s, scp: \n %s", f->name.c_str(), scp_str.c_str());
				DescendNode(lang_stat, f->s, f->global);
				lang_stat->call_regs_used = 0;
			}
			lang_stat->flags &= ~PSR_FLAGS_ON_JMP_WHEN_ERROR;
		}



		/*
		FOR_VEC(cur_f, lang_stat->funcs_scp->vars)
		{
			auto f = *cur_f;
			if (f->type.type != TYPE_FUNC)
				continue;
			auto fdecl = f->type.fdecl;
			if (IS_FLAG_ON(fdecl->flags, FUNC_DECL_MACRO | FUNC_DECL_IS_OUTSIDER | FUNC_DECL_TEMPLATED | FUNC_DECL_INTRINSIC))
				continue;

			CreateAstFromFunc(lang_stat, f->type.fdecl);
			int  a = 0;
		}
		*/
		lang_stat->lsp_stage = LSP_STAGE_DONE;

	}break;
	}
}

func_decl *GetFuncWithLine2(lang_state *lang_stat, int line, unit_file *file)
{

	FOR_VEC(f_ptr, file->funcs_scp->vars)
	{
		if ((*f_ptr)->type.type != TYPE_FUNC)
			continue;
		func_decl* f = (*f_ptr)->type.fdecl;
		if(line >= f->scp->line_start && line <= f->scp->line_end)
		{
			return f;
		}
	}
	return nullptr;
}
