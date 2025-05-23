#pragma once
#include<vector>
#include<string>
#include "machine_rel.h"
#include "rel_utils.h"
#include "token_common.h"



struct decl2;
struct node;
struct type_struct2;
struct type2;
struct func_decl;
struct comma_ret;
struct scope;
struct token2;
struct import_strct;
struct lang_state;
struct func_byte_code;
struct machine_code;
struct unit_file;

void NewFuncToCompile(func_decl * fedcl);
void NewDeclToCurFilseGlobalsScope(lang_state *, decl2 * decl);
func_decl* IsThereAFunction(lang_state *, char* name);
type_struct2 *SearchSerializedStruct(own_std::string name);
void AddNewDeclToFileGlobalScope(lang_state *, decl2 * d);
bool GetTypeFromTkns(token2 *tkns, type2 &tp);
bool GetDeclFromTkns(own_std::vector<token2> *tkns, int *i, decl2 *);
bool GetDeclFromTkns(own_std::vector<token2> *tkns, int *i, decl2 *);
type2 DescendNode(lang_state *, node *n, scope *scp);
void InsertIntoCharVector(own_std::vector<char> *vec, void *src, int size);
own_std::string FuncNameWithTempls(lang_state *,  own_std::string fname, own_std::vector<type2> *types);
own_std::string MangleFuncNameWithArgs(lang_state *, func_decl *fdecl, own_std::string original_name, int start_arg);
decl2 *NewDecl(lang_state *, own_std::string name, type2 tp);
decl2 *GetDeclFromStruct(type_struct2*);
void TransformSingleFuncToOvrlStrct(lang_state *, decl2 *decl_exist);
int GetTypeSize(type2* tp);



enum overload_op
{
	INDEX_OP,
	ASSIGN_OP,
	PLUS_OP,
	MUL_OP,
	MINUS_OP,
	DEREF_OP,
	COND_EQ_OP,
	FOR_OP,
};
enum enum_type2
{
	TYPE_AUTO,
	TYPE_ARRAY_TYPE,
	TYPE_STATIC_ARRAY,
	TYPE_ARRAY,
	TYPE_ARRAY_DYN,

	TYPE_ENUM_IDX_32,

	TYPE_OVERLOADED_FUNCS,

	TYPE_TYPEDEF,

	TYPE_WASM_MEMORY,

	TYPE_VOID,
	TYPE_VOID_TYPE,

	TYPE_RANGE,

	//
	TYPE_STRUCT_TYPE,
	TYPE_CHAR_TYPE,
	TYPE_STR_LIT,
	TYPE_STATIC_ARRAY_TYPE,

	TYPE_U8_TYPE,
	TYPE_S8_TYPE,
	TYPE_U16_TYPE,
	TYPE_S16_TYPE,
	TYPE_U32_TYPE,
	TYPE_S32_TYPE,
	TYPE_U64_TYPE,
	TYPE_S64_TYPE,

	TYPE_BOOL_TYPE,
	TYPE_UNION_TYPE,
	TYPE_FUNC_TYPE,
	TYPE_ENUM_TYPE,
	TYPE_F32_TYPE,
	TYPE_F64_TYPE,
	//

	TYPE_INT,

	TYPE_IMPORT,

	TYPE_REL_TYPE,
	TYPE_REL,

	TYPE_U8,
	TYPE_S8,
	TYPE_U16,
	TYPE_S16,
	TYPE_U32,
	TYPE_S32,
	TYPE_U64,
	TYPE_S64,

	TYPE_F32,
	TYPE_CHAR,
	TYPE_F64,
	TYPE_BOOL,
	TYPE_F32_RAW,
	TYPE_F64_RAW,

	TYPE_UNION_DECL,
	TYPE_UNION,

	TYPE_ENUM_DECL,
	TYPE_ENUM,

	TYPE_MACRO_EXPR,

	TYPE_STRUCT_DECL,
	TYPE_STRUCT,
	TYPE_STRUCT_ANON,
	TYPE_BUILT_IN_MACRO,
	TYPE_FUNC_DECL,
	TYPE_FUNC_PTR,
	TYPE_FUNC_DEF,
	TYPE_FUNC_EXTERN,
	TYPE_FUNC,
	TYPE_COMMA,
	TYPE_TEMPLATE,
	TYPE_VECTOR_TYPE,
	TYPE_VECTOR,
	TYPE_MAX,
};
struct type_data
{
	union
	{
		// offset holds a ptr to the another strct type_data
		long long _u64;
		// entries is how many members a strct has
		int entries;
	};
	long long strct_sz;
	long long offset;
	char ptr;
	decl2* serializable_func;
	enum_type2 tp;
	int flags;
	int name;
	int name_len;
	int decl_offset_for_ptr_len;

};
struct template_expr
{
	union
	{
		own_std::string name;
	};
	int type;
	type2 *final_type;
	node *expr;
	node *func;
	scope *scp;

	template_expr()
	{
	}
	~template_expr()
	{
	}
};
struct func_overload_strct
{
	own_std::string name;

	bool templated;
	overload_op ovrld_op;
	own_std::vector<func_decl *> fdecls;
	own_std::hash_map<node *, func_decl *> cached;
};
#define TYPE_CONST_WAS_DECLARED 1
#define TYPE_IS_REL   2
#define TYPE_SELF_REF 4
#define TYPE_STRCT_WAS_FROM_VAL 8
#define TYPE_VAR_ARGS_STRCT 0x10
#define TYPE_NOT_INSTANTIATED_YET 0x20
#define TYPE_FLAGS_AUTO_CAST 0x40
#define TYPE_FUNC_RECURSIVE 8
#define TYPE_IS_GLOBAL 0x10
struct type2
{
	enum_type2 type;
	char ptr;
#ifndef COMPILER
	rel_ptr<type_struct2> strct;
#else
	bool is_const;
	int flags;
	union
	{
		int i;
		float f;

		unsigned char u8;
		unsigned short u16;
		unsigned int u32;
		unsigned long long u64;

		int str_len;

		char s8;
		short s16;
		int s32;
		long long s64;
		type_struct2 *strct;
		own_std::string template_name;
		void (*macro_builtin)(node *, node *, node *, void *);
        decl2 *type_def_decl;

		func_decl *fdecl;
		func_overload_strct *overload_funcs;
		import_strct *imp;

		node *nd;

		struct
		{
			scope *scp;
			decl2 *e_decl;
			own_std::vector<char*>* enum_names;
			own_std::string templ_name;
		};

		//template_expr templ;

		struct
		{
			enum_type2 rel_lhs;
			type2 *rel_rhs;
		};
		struct
		{
			union
			{
				int ar_size;
			};
			int *decl_offset;
			
			type2 *tp;
		};
		struct
		{
		};
	};

	decl2 *from_enum;
	int e_idx;
#endif
	decl2 *GetEnumDecl(own_std::string name);
	func_decl *ChooseFuncOverload(lang_state *, own_std::vector<type2> *tps);
	bool IsStrct(decl2 **decl)
	{
		if(type == enum_type2::TYPE_STRUCT || type == enum_type2::TYPE_STRUCT_TYPE)
        {   
            if(decl)
                *decl = GetDeclFromStruct(strct);
            return true;
        }
        return false;
	}
	bool IsFloat()
	{
		return type == TYPE_F32 || type == TYPE_F64;
	}
	type2()
	{
		memset(this, 0, sizeof(*this));
	}
	type2 operator=(const type2 &other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
	type2(const type2 &other)
	{
		memcpy(this, &other, sizeof(*this));
	}
	~type2(){}
};
struct template_to_be_assigned
{
	own_std::string name;
	type2 **ptr;
};
#define FUNC_DECL_IS_LAMBDA   0x1
#define FUNC_DECL_IS_OUTSIDER 0x2
#define FUNC_DECL_IS_OP_OVERLOAD 0x4
#define FUNC_DECL_IS_DONE 0x8
#define FUNC_DECL_TEMPLATES_DECLARED_TO_SCOPE 0x10
#define FUNC_DECL_EXTERN 0x20
#define FUNC_DECL_TEMPLATED 0x40
#define FUNC_DECL_VAR_ARGS 0x80
#define FUNC_DECL_CONSTRUCTOR 0x100
#define FUNC_DECL_INTERNAL 0x200
#define FUNC_DECL_LINK_NAME 0x400
#define FUNC_DECL_TEST 0x800
#define FUNC_DECL_ARGS_GOTTEN 0x1000
#define FUNC_DECL_MACRO 0x2000
#define FUNC_DECL_NAME_INSERTED 0x4000
#define FUNC_DECL_ALIGN_STACK_WHEN_CALL 0x8000
#define FUNC_DECL_COMP 0x10000
#define FUNC_DECL_CODE_WAS_GENERATED 0x20000
#define FUNC_DECL_COROUTINE 0x40000
#define FUNC_DECL_SERIALIZED 0x80000
#define FUNC_DECL_X64 0x100000
#define FUNC_DECL_INTRINSIC 0x200000
#define FUNC_DECL_LABEL 0x400000
#define FUNC_DECL_LABELS_GOTTEN 0x800000
#define FUNC_DECL_INSTANTIATED 0x1000000
#define FUNC_DECL_CAST 0x2000000

struct stmnt_dbg
{
	int start;
	int end;
	int start_ir;
	int end_ir;
	int line;
};
struct ast_rep;
struct func_decl
{
	own_std::string name;
	own_std::string link_name;

	own_std::vector<decl2 *> args;
	own_std::vector<decl2 *> vars;
	own_std::vector<node *> stmnts;
	own_std::vector<template_expr> templates;
	own_std::vector<template_to_be_assigned> temps_to_be_assigned;
	own_std::vector<func_decl *> plugins;
	own_std::vector<func_decl *> this_functions;
	own_std::vector<stmnt_dbg> wasm_stmnts;
	own_std::vector<int> bcs2;

	own_std::vector<int> ir;

	func_decl* call_not_found;

	node* coroutine_prologue_tree;
	long long flags;
	//tkn_type2 op_overload;
	overload_op op_overload;
	type2 ret_type;
	node *func_node;

	scope *scp;
	scope *wasm_scp;


    func_byte_code *func_bcode;
	node *reached_nd;

	unit_file *from_file;

	unit_file *templated_in_file;
	int templated_in_file_line;

	decl2 *this_decl;

	decl2 *from_overload;

	int ir_stack_begin_idx;

	ast_rep* ast;

    machine_code *code;

	int references;

	int code_start_idx;
	int for_interpreter_code_start_idx;
	int code_size;

    int wasm_func_sect_idx;
    int wasm_code_sect_idx;
	int stack_size;

	int func_dbg_idx;

	int func_idx;

	int biggest_saved_regs = 2;
	int biggest_call_args;

	int to_spill_size;
	int to_spill_offset;
	//int biggest_call_args;

	int biggest_saved_lhs;

	int bcs2_start;
	int bcs2_end;


	int strct_ret_size_per_statement;
	int strct_ret_size_per_statement_offset;

	int strct_constrct_size_per_statement;
	int strct_constrct_at_offset;

	int saved_regs_offset;

	int saved_rsp_offset;

	int saved_lhs_offset;

	int per_stmnt_strct_val_sz;
	int per_stmnt_strct_val_offset;

	int total_of_var_args;

	// strct_vals holds the func's args that are struct vals
	int strct_vals_offset;
	int strct_vals_sz;

	int strct_val_ret_offset;

	int call_strcts_val_sz;
	int call_strcts_val_offset;

	int array_literal_offset;
	int array_literal_sz;

	int total_hidden_ret_labels;

	int var_args_start_offset;
	int line;
	/*
	func_decl *NewFuncComplete()
	{
		auto ret = this->new_func();
		ret->func_node = this->func_node->NewTree();
		ret->scp   = NewScope(this->scp);
		return ret;

	}
	*/
	func_decl *new_func()
	{
		auto ret = (func_decl*)__lang_globals.alloc(__lang_globals.data, sizeof(func_decl));
		memset(ret, 0, sizeof(func_decl));
		//memcpy(ret, this, sizeof(func_decl));
		//ret->args.assign(args.begin(), args.end());
		ret->func_node = this->func_node;
		//ret->args = this->args;
		ret->templates.assign(templates.begin(), templates.end());
		ret->temps_to_be_assigned.assign(temps_to_be_assigned.begin(), temps_to_be_assigned.end());
		ret->scp = nullptr;
		ret->from_file = from_file;
		return ret;
	}
};
#define DECL_NOT_DONE  0x1
// this is used for vars inside structs that are after a using
#define DECL_FROM_USING  0x2
#define DECL_IS_ARG  0x4
#define DECL_INSERT_VAR_ARGS_AR  0x8
#define DECL_IS_GLOBAL  0x10
#define DECL_IS_SERIALIZED  0x20
#define DECL_ABSOLUTE_ADDRESS  0x40
#define DECL_PTR_HAS_LEN  0x80
#define DECL_IS_VAR_ARG  0x100
#define DECL_SERIALIZABLE  0x200

struct decl2
{
#ifndef COMPILER
	rel_ptr<char> name;
#else
	own_std::string name;
#endif
	type2 type;
    union
    {
        int offset;
        int type_sect_offset;
    };
	int flags;
    int wasm_type_idx;
    int func_arg_idx;
	int serialized_type_idx;
	own_std::vector<int>* when_used_code;

	node *using_node;
	node *bottom_n;

	node *decl_nd;
	union
	{
		decl2* len_for_ptr;
		u64 len_for_ptr_offset;
		own_std::string len_for_ptr_name;
	};

	unit_file *from_file;

	bool AssignTemplate(lang_state *, own_std::string tname, type2 *tp, comma_ret *);
	decl2()
	{
		memset(this, 0, sizeof(*this));
	}
	~decl2(){}
};


decl2 *FindDeclFunc(own_std::vector<decl2> *vec, bool(*func)(decl2 *))
{
	FOR_VEC(it, *vec)
	{
		if(func(&*it))
			return &*it;
	}
	return nullptr;
}

own_std::string OvrldOpToStr(overload_op op);

#define TP_STRCT_TEMPLATED 1
#define TP_STRCT_STRUCT_NOT_NODE 2
#define TP_STRCT_STRUCT_SERIALIZED 4
#define TP_STRCT_TUPLE 8
#define TP_STRCT_ETRUCT 0x10
#define TP_STRCT_IS_DYN_ARRAY 0x20
#define TP_STRCT_ACTUAL_SERIALIZED 0x40
//#define TP_STRCT_DONE 8
struct type_struct2
{
#ifndef COMPILER
	rel_ptr<char> name;
	rel_array<decl2> vars;
#else
	own_std::string name;
	own_std::vector<decl2 *> vars;

	decl2 *this_decl;

	unit_file *from_file;

	own_std::vector<template_expr> templates;
	own_std::vector<template_to_be_assigned> temps_to_be_assigned;
	own_std::vector<func_decl *> op_overloads;
	own_std::vector<func_decl *> this_funcs;
	own_std::vector<decl2 *> op_overloads_funcs;
	own_std::vector<func_overload_strct> op_overloads_intern;
	own_std::vector<func_overload_strct> constructors;
	own_std::vector<int> depends_on_me;

	own_std::vector<type_struct2 *> instantiated_strcts;
	decl2* ser_func;
	type_struct2 *original_strct;
	node *strct_node;
	scope *scp;

	int size;
	int flags;
	int biggest_type;
	int type_sect_offset;
	int serialized_type_idx;
	type_struct2()
	{
		memset(this, 0, sizeof(type_struct2));
	}
	//own_std::vector<own_std::string> templates;
	//own_std::vector<type2> templates_insantiated;
	func_decl *CreateNewOpOverload(lang_state *lang_stat, func_decl *original, overload_op tp);
	void AddNewConstrctor(func_decl *fdecl)
	{
		constructors[0].fdecls.emplace_back(fdecl);
	}
	void AddOpOverload(lang_state *lang_stat, func_decl *fdecl, overload_op op, int line, int line_offset)
	{
		op_overloads.push_back(fdecl);


		decl2 *found_op = nullptr;
		FOR_VEC(f, op_overloads_funcs)
		{
			if ((*f)->type.type == TYPE_OVERLOADED_FUNCS && (*f)->type.overload_funcs->ovrld_op == op)
			{
				found_op = (*f);
				break;
			}
			else
			{
				if (op == (*f)->type.fdecl->op_overload)
				{
					found_op = (*f);
					break;
				}
			}
		}
	
		if(found_op)
		{
			if(found_op->type.type == TYPE_FUNC)
				TransformSingleFuncToOvrlStrct(lang_stat, found_op);
				
			if (IS_FLAG_OFF(fdecl->flags, FUNC_DECL_NAME_INSERTED))
			{
				fdecl->name = own_std::string(found_op->name);
				fdecl->name = MangleFuncNameWithArgs(lang_stat, fdecl, fdecl->name, 0);
			}

			type2 tp = {};
			tp.type = TYPE_FUNC;
			tp.fdecl = fdecl;
			AddNewDeclToFileGlobalScope(lang_stat, NewDecl(lang_stat, fdecl->name, tp));
			found_op->type.overload_funcs->fdecls.push_back(fdecl);
			//lang_stat->fu
		}
		else
		{
			type2 f_tp;
			f_tp.type  = TYPE_FUNC;
			f_tp.fdecl = fdecl;

			own_std::string fname = this->name + OvrldOpToStr(fdecl->op_overload);

			//if (fdecl->op_overload == COND_EQ_OP)
			if (IS_FLAG_OFF(fdecl->flags, FUNC_DECL_NAME_INSERTED))
				fdecl->name = fname;

			if (!fdecl->this_decl)
				fdecl->this_decl = NewDecl(lang_stat, "", f_tp);

			fdecl->this_decl->name = fname;
			fdecl->this_decl->type = f_tp;
			fdecl->templated_in_file = lang_stat->cur_file;
			fdecl->templated_in_file_line = line;
			if (fdecl->this_decl->name == "string==")
				auto a = 0;
			op_overloads_funcs.push_back(fdecl->this_decl);

			NewDeclToCurFilseGlobalsScope(lang_stat, fdecl->this_decl);
		}
		fdecl->flags |= FUNC_DECL_NAME_INSERTED;
	}
#endif
	func_decl *FindOpOverload(lang_state *, overload_op tp, node *, own_std::vector<type2> * = nullptr);
	func_decl *FindExistingOverload(lang_state*, own_std::vector<func_overload_strct> *funcs, void * op, own_std::vector<type2> *tps, bool search_operator_ovrld, node *);
	decl2 *FindDecl(own_std::string &name)
	{
#ifdef COMPILER
		for(int i = 0; i < this->vars.size(); i++)
		{
			auto decl = this->vars[i];
			if(decl->name == name)
			{
				return decl;
			}
		}
		return nullptr;
#endif
	}

#define NEXT_TYPE_DATA(st, name_sz) (type_data *)(((char*)(st + 1)) + (name_sz))
	void ToTypeSect(lang_state *lang_stat, own_std::vector<char> *type_sect, int *str_tbl_sz)
	{
		// the name of the struct and variables go at the of the struct
		own_std::vector<char> buffer;
		own_std::vector<char> strct_str_tbl;
		//buffer.resize(128);
		//strct_str_tbl.resize(128);

        type_sect_offset = type_sect->size();
		buffer.insert(buffer.end(), sizeof(type_data), 0);
		
		type_data *strct_ptr = (type_data *)buffer.data();

		int offset_to_str_tbl = sizeof(type_data) *(vars.size() + 1);
		offset_to_str_tbl    -= offsetof(type_data, name);

		strct_ptr->name = offset_to_str_tbl;
		strct_ptr->name_len = this->name.length();
		strct_ptr->tp = enum_type2::TYPE_STRUCT_DECL;
		strct_ptr->entries = vars.size();
		strct_ptr->flags = flags;
		strct_ptr->strct_sz = size;


		if (ser_func)
		{
			strct_ptr->serializable_func = ser_func;
			lang_stat->ser_funcs_type_data.emplace_back(type_sect->size());
		}

		if (original_strct && original_strct->name == "dyn_array")
		{
			strct_ptr->flags |= TP_STRCT_IS_DYN_ARRAY;
			if (name == "dyn_array_1layer_sprites")
				auto a = 0;
		}
		if (name == "layer_sprites")
			auto a = 0;

		InsertIntoCharVector(&strct_str_tbl, (void *)this->name.data(), this->name.size());

		type_data *var_ptr = nullptr;

		int total_vars = 0;
		FOR_VEC(v, vars)
		{
			if (IS_FLAG_ON((*v)->flags, DECL_FROM_USING))
				continue;
			total_vars++;
		}
		int i = 0;
		FOR_VEC(v, vars)
		{
			if (IS_FLAG_ON((*v)->flags, DECL_FROM_USING))
				continue;


			int last_size = buffer.size();
			buffer.insert(buffer.end(), sizeof(type_data), 0);

			var_ptr = (type_data*)&buffer[last_size];
			
			offset_to_str_tbl  = (total_vars - i) * sizeof(type_data);
			offset_to_str_tbl += strct_str_tbl.size();
			offset_to_str_tbl -= offsetof(type_data, name);

			if (IS_FLAG_ON((*v)->flags, DECL_PTR_HAS_LEN))
			{
				var_ptr->flags = (*v)->flags;
				var_ptr->decl_offset_for_ptr_len = (*v)->len_for_ptr->offset;
			}
			var_ptr->offset = (*v)->offset;
			var_ptr->tp = (*v)->type.type;
			var_ptr->ptr = (*v)->type.ptr;
			var_ptr->name = offset_to_str_tbl;
			var_ptr->strct_sz = GetTypeSize(&(*v)->type);
			var_ptr->name_len = (*v)->name.length();
			var_ptr->flags = (*v)->flags;

			InsertIntoCharVector(&strct_str_tbl, (void *)(*v)->name.data(), (*v)->name.size());

			if ((*v)->type.type == enum_type2::TYPE_STRUCT)
			{
				var_ptr->flags |= IS_FLAG_ON((*v)->type.strct->flags, TP_STRCT_ETRUCT) * TP_STRCT_ETRUCT;
				var_ptr->_u64 = (*v)->type.strct->type_sect_offset + MEM_PTR_START_ADDR;
				if(IS_FLAG_OFF((*v)->type.strct->flags, TP_STRCT_ACTUAL_SERIALIZED))
				{
					(*v)->type.strct->depends_on_me.emplace_back(type_sect_offset + last_size);
				}
			}
			i++;
		}
		

		strct_ptr = (type_data *)buffer.data();
		strct_ptr->entries = total_vars;
		strct_ptr->name = sizeof(type_data) * (total_vars + 1);
		strct_ptr->name    -= offsetof(type_data, name);

		type_sect->insert(type_sect->end(), buffer.begin(), buffer.end());
		type_sect->insert(type_sect->end(), strct_str_tbl.begin(), strct_str_tbl.end());

		FOR_VEC(cur, depends_on_me)
		{
			var_ptr = (type_data*)&(*type_sect)[*cur];
			var_ptr->_u64 = type_sect_offset + MEM_PTR_START_ADDR;
			auto a = 0;

		}

		flags |= TP_STRCT_ACTUAL_SERIALIZED;

	}
};

decl2 *GetDeclFromStruct(type_struct2 *st)
{
    return st->this_decl;
}
token2 *token2::NewTkn(lang_state *lang_stat)
{
	auto ret = (token2 *)AllocMiscData(lang_stat, sizeof(token2));
	memset(ret, 0, sizeof(token2));
	
	if (type == T_WORD)
	{
		ret->line = line;
		ret->line_str = line_str;
		ret->line_offset = line_offset;
		ret->str = str;
	}
	else
	{
		memcpy(ret, this, sizeof(token2));
	}
	
	return ret;
}

#define TKN_FLAGS_IS_NEW_LINE 1
int IsTknWordStr(token2 *tkn, const char *str)
{
	return tkn->type == tkn_type2::T_WORD && tkn->str == str;
}
int IsTknWordStr(token2 *tkn, own_std::string str)
{
	return tkn->type == tkn_type2::T_WORD && tkn->str == str;
}
int FindToken(own_std::vector<token2> *to_search, int start, bool(*func)(tkn_type2))
{
	int i = 0;
	int max = to_search->size();
	tkn_type2 cur = tkn_type2::T_PLUS;
	while((start + i) < max)
	{
		cur = (*to_search)[start + i].type;
		if(func(cur))
			break;
		i++;
	}
	return start + i;
}
int FindWordAtSameScope(own_std::vector<token2> *to_search, int start, int end, own_std::string target)
{
	int level = 0;
	tkn_type2 ch = (tkn_type2)0;
	int i = 0;
	while (level >= 0 && (start + i) < end)
	{
		auto cur_tkn = &(*to_search)[start + i];
		ch = cur_tkn->type;

		if (ch == tkn_type2::T_CLOSE_CURLY)
		{
			level--;
		}
		if (ch == tkn_type2::T_OPEN_CURLY)
		{
			level++;
		}
		if(IsTknWordStr(cur_tkn, target) && level == 0)
		{
			return start + i;
		}
		i++;
	}
	return start + i;
}
int FindTokenClose(own_std::vector<token2> *to_search, int start, tkn_type2 target)
{
	int level = 0;
	tkn_type2 ch = (tkn_type2)0;
	int i = 0;
	while (level >= 0)
	{
		ch = (*to_search)[start + i].type;

		if (target == tkn_type2::T_OPEN_CURLY && ch == tkn_type2::T_CLOSE_CURLY)
		{
			level--;
		}
		if (target == tkn_type2::T_OPEN_CURLY && ch == tkn_type2::T_OPEN_CURLY)
		{
			level++;
		}
		if (target == tkn_type2::T_LESSER_THAN && ch == tkn_type2::T_GREATER_THAN)
		{
			level--;
		}
		if (target == tkn_type2::T_LESSER_THAN && ch == tkn_type2::T_LESSER_THAN)
		{
			level++;
		}
		if (target == tkn_type2::T_OPEN_PARENTHESES && ch == tkn_type2::T_CLOSE_PARENTHESES)
		{
			level--;
		}
		if (target == tkn_type2::T_OPEN_PARENTHESES && ch == tkn_type2::T_OPEN_PARENTHESES)
		{
			level++;
		}
		i++;
	}
	i--;
	return start + i;
}
own_std::string OperatorToString(tkn_type2 type)
{
	switch(type)
	{
		case tkn_type2::T_PIPE:
		{
			return own_std::string("|");
		}break;
		case tkn_type2::T_EXCLAMATION:
		{
			return own_std::string("!");
		}break;
		case tkn_type2::T_PERCENT:
		{
			return own_std::string("%");
		}break;
		case tkn_type2::T_COLON:
		{
			return own_std::string(":");
		}break;
		case tkn_type2::T_PLUS:
		{
			return own_std::string("+");
		}break;
		case tkn_type2::T_COND_OR:
		{
			return own_std::string("|");
		}break;
		case tkn_type2::T_NEW_LINE:
		{
			return own_std::string("\n");
		}break;
		case tkn_type2::T_AT:
		{
			return own_std::string("@");
		}break;
		case tkn_type2::T_POINT:
		{
			return own_std::string(".");
		}break;
		case tkn_type2::T_COMMA:
		{
			return own_std::string(",");
		}break;
		case tkn_type2::T_CLOSE_CURLY:
		{
			return own_std::string("\n}\n");
		}break;
		case tkn_type2::T_OPEN_CURLY:
		{
			return own_std::string("\n{\n");
		}break;
		case tkn_type2::T_SHIFT_RIGHT:
		{
			return own_std::string(">>");
		}break;
		case tkn_type2::T_SHIFT_LEFT:
		{
			return own_std::string("<<");
		}break;
		case tkn_type2::T_LESSER_THAN:
		{
			return own_std::string("<");
		}break;
		case tkn_type2::T_LESSER_EQ:
		{
			return own_std::string("<=");
		}break;
		case tkn_type2::T_GREATER_THAN:
		{
			return own_std::string(">");
		}break;
		case tkn_type2::T_OPEN_BRACKETS:
		{
			return own_std::string("[");
		}break;
		case tkn_type2::T_CLOSE_BRACKETS:
		{
			return own_std::string("]");
		}break;
		case tkn_type2::T_OPEN_PARENTHESES:
		{
			return own_std::string("(");
		}break;
		case tkn_type2::T_CLOSE_PARENTHESES:
		{
			return own_std::string(")");
		}break;
		case tkn_type2::T_MINUS:
		{
			return own_std::string("-");
		}break;
		case tkn_type2::T_EQUAL:
		{
			return own_std::string("=");
		}break;
		case tkn_type2::T_AMPERSAND:
		{
			return own_std::string("&");
		}break;
		case tkn_type2::T_SEMI_COLON:
		{
			return own_std::string(";\n");
		}break;
		case tkn_type2::T_COND_NE:
		{
			return own_std::string("!=");
		}break;
		case tkn_type2::T_COND_EQ:
		{
			return own_std::string("==");
		}break;
		case tkn_type2::T_GREATER_EQ:
		{
			return own_std::string(">=");
		}break;
		case tkn_type2::T_DIV:
		{
			return own_std::string("/");
		}break;
		case tkn_type2::T_MUL:
		{
			return own_std::string("*");
		}break;
		default:
			ASSERT(0);
	}
	return own_std::string("");
}
own_std::string token2::ToString()
{
	switch(type)
	{
		case tkn_type2::T_PIPE:
		{
			return own_std::string("|");
		}break;
		case tkn_type2::T_EXCLAMATION:
		{
			return own_std::string("!");
		}break;
		case tkn_type2::T_COLON:
		{
			return own_std::string(":");
		}break;
		case tkn_type2::T_PLUS:
		{
			return own_std::string("+");
		}break;
		case tkn_type2::T_NEW_LINE:
		{
			return own_std::string("\n");
		}break;
		case tkn_type2::T_AT:
		{
			return own_std::string("@");
		}break;
		case tkn_type2::T_FLOAT:
		{
			return own_std::to_string(this->f).c_str();
		}break;
		case tkn_type2::T_INT:
		{
			return own_std::to_string(this->i).c_str();
		}break;
		case tkn_type2::T_WORD:
		{
			return this->str;
		}break;
		case tkn_type2::T_POINT:
		{
			return own_std::string(".");
		}break;
		case tkn_type2::T_COMMA:
		{
			return own_std::string(",");
		}break;
		case tkn_type2::T_CLOSE_CURLY:
		{
			return own_std::string("\n}\n");
		}break;
		case tkn_type2::T_OPEN_CURLY:
		{
			return own_std::string("\n{\n");
		}break;
		case tkn_type2::T_LESSER_THAN:
		{
			return own_std::string("<");
		}break;
		case tkn_type2::T_GREATER_THAN:
		{
			return own_std::string(">");
		}break;
		case tkn_type2::T_OPEN_BRACKETS:
		{
			return own_std::string("[");
		}break;
		case tkn_type2::T_CLOSE_BRACKETS:
		{
			return own_std::string("]");
		}break;
		case tkn_type2::T_OPEN_PARENTHESES:
		{
			return own_std::string("(");
		}break;
		case tkn_type2::T_CLOSE_PARENTHESES:
		{
			return own_std::string(")");
		}break;
		case tkn_type2::T_MINUS:
		{
			return own_std::string("-");
		}break;
		case tkn_type2::T_EQUAL:
		{
			return own_std::string("=");
		}break;
		case tkn_type2::T_AMPERSAND:
		{
			return own_std::string("&");
		}break;
		case tkn_type2::T_SEMI_COLON:
		{
			return own_std::string(";\n");
		}break;
		case tkn_type2::T_MUL:
		{
			return own_std::string("*");
		}break;
	}
	return own_std::string("");
}
own_std::string StringifyTkns(own_std::vector<token2> *tkns, int start, int amount)
{
	own_std::string final_ret;
	final_ret.reserve(64);
	for(int i = 0; i < amount; i++)
	{
		final_ret.append((*tkns)[start + i].ToString());
		if (i < (amount - 1))
		{
			final_ret.append(" ");
		}
	}
	return final_ret;
}
bool is_type_unsigned(enum_type2 tp)
{
	switch(tp)
	{
	case enum_type2::TYPE_U64:
	case enum_type2::TYPE_U32:
	case enum_type2::TYPE_U16:
	case enum_type2::TYPE_U8:
		return true;

	case enum_type2::TYPE_S64:
	case enum_type2::TYPE_S32:
	case enum_type2::TYPE_S16:
	case enum_type2::TYPE_S8:
		return false;
	}

	return false;
}
struct comma_ret
{
	int type;
	union
	{
		decl2 decl;
	};
	node *n;
	comma_ret()
	{
		memset(this, 0, sizeof(comma_ret));
	}
	comma_ret operator=(const comma_ret& other) 
	{
		comma_ret c;
		memcpy(this, &other, sizeof(other));

		return c;
	}
	comma_ret(const comma_ret &other)
	{
		memcpy(this, &other, sizeof(comma_ret));
	}
	~comma_ret(){
	}
};

bool CompareTypes(type2* lhs, type2* rhs, bool assert);
func_decl *type2::ChooseFuncOverload(lang_state *lang_stat, own_std::vector<type2> *tps)
{
	ASSERT(overload_funcs);
	FOR_VEC(f, overload_funcs->fdecls)
	{
		if ((*f)->args.size() == tps->size())
		{
			int i = 0;
			bool matches = true;
			FOR_VEC(t, (*tps))
			{
				if (!CompareTypes(&(*f)->args[i]->type, t, false))
				{
					matches = false;
					break;
				}
			}
			if(matches)
				return (*f);

		}
	}
	return nullptr;
} 
