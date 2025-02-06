#pragma once
#include "token.h"
#include <queue>

#define NODE_FLAGS_IS_PARENTHESES 0x1
#define NODE_FLAGS_IS_SCOPE 0x2
#define NODE_FLAGS_IS_PROCESSED 0x4
#define NODE_FLAGS_IS_BRACKETS 0x8
#define NODE_FLAGS_INDEX_IS_TYPE 0x10
#define NODE_FLAGS_COLON_INFER 0x20
#define NODE_FLAGS_FUNC_OUTSIDER 0x40
#define NODE_FLAGS_FUNC_CONSTRUCTOR 0x80
#define NODE_FLAGS_IS_PROCESSED2 0x100
#define NODE_FLAGS_FUNC_INTERNAL 0x200
#define NODE_FLAGS_FUNC_LINK_NAME 0x400
#define NODE_FLAGS_FUNC_TEST      0x800
#define NODE_FLAGS_AR_LIT_ANON   0x1000
#define NODE_FLAGS_CALL_RET_ANON   0x2000
#define NODE_FLAGS_FUNC_MACRO      0x4000
#define NODE_FLAGS_ALIGN_STACK_WHEN_CALL 0x8000
#define NODE_FLAGS_IS_PROCESSED3 0x10000
#define NODE_FLAGS_FUNC_COMP 0x20000
#define NODE_FLAGS_WAS_MODIFIED 0x40000
#define NODE_FLAGS_FUNC_COROUTINE 0x80000
#define NODE_FLAGS_STMNT_ZERO_INITIALIZED 0x100000
#define NODE_FLAGS_POINT_FROM_USING 0x200000
#define NODE_FLAGS_STMNT_WITHOUT_SEMICOLON 0x400000
#define NODE_FLAGS_CALL_WAS_MACRO 0x800000
#define NODE_FLAGS_COMMA_INSIDE_PARENTHESES 0x1000000
#define NODE_FLAGS_NO_ZERO_INITIALIZATION 0x2000000
#define NODE_FLAGS_FUNC_X64 0x4000000
#define NODE_FLAGS_FUNC_INTRINSIC 0x8000000

#define ASSIGN_VEC(v1, v2) v1.assign(v2.begin(), v2. end())
#define INSERT_VEC(v1, v2) v1.insert(v1.end(), v2.begin(), v2.end())

#define DEBUG_NAME

struct lang_state;
node *new_node(lang_state* );
node *new_node(lang_state *, node *src);
enum tkn_type2 : unsigned char;
struct import_strct;
struct func_byte_code;
struct node;

enum parser_cond
{
	GREATER,
	GREATER_EQUAL,
	LESSER,
	LESSER_EQUAL,
	EQUAL,
	NEQUAL,
};

struct node;
struct node_iter
{
	own_std::vector<token2> *tkns;
	int cur_idx;
	int cur_scope_count;
	bool rev;

    lang_state *lang_stat;

	node_iter(own_std::vector<token2> *v, lang_state *l)
	{
		memset(this, 0, sizeof(node_iter));
		tkns = v;
		cur_idx = 0;
		lang_stat = l;
	}

	void CheckTwoBinaryOparatorsTogether(node *);
	bool node_iter::IsOpUnary(token2* tkn, node*);
	void node_iter::SetNodeScopeIdx(lang_state* lang_stat, node** nd, unsigned char val, int, int);

	void node_iter::ExpectTkn(tkn_type2);
	node *node_iter::parse_func_like();
	node *node_iter::parse_strct_like();
	node *node_iter::parse_all();
	node *node_iter::parse_stmnts();
	node *node_iter::parse_sub_expr(int prec);
	node *node_iter::parse_expr();
	node *node_iter::parse_str(std::string &, int *);
	node *node_iter::parse_(int prec,  parser_cond);
	void EatNewLine();
	void CreateCondAndScope(node **n);

	//Znode *parse_expression();
	node* parse_expression(int);

	node *node_iter::parse_sub_expression();
	node *parse(tkn_type2 target);

	token2 *peek_tkn();
	token2 *get_tkn();

	bool is_operator(token2 *tkn, int *precedence);
	bool is_unary(tkn_type2 *tp);
	node *grow_unary_tree(bool left, node **top);
};
enum keyword
{
	KW_DBG_BREAK,
	KW_RETURN,
	KW_TRUE,
	KW_FALSE,
	KW_FN,
	KW_MATCH,
	KW_RET_TYPE,
	KW_BREAK,
	KW_CONTINUE,
	KW_NIL,
	KW_USING,
	KW_REL,
	KW_TYPEDEF,
	KW_CONSTRUCTOR,
	KW_REV,
};
enum node_type
{
	N_EMPTY,
	N_BINOP,
	N_TUPLE,
	N_SCOPE,
	N_STMNT,
	N_UNOP,
	N_IDENTIFIER,
	N_WHEN_USED,
	N_MAKE_PTR_LEN,
	N_CONST_DECL,
	N_APOSTROPHE,
	N_FOR,
	N_WHILE,
	N_IMPORT,
	N_TYPEDEF,
	N_IMPORT_LIB,
	N_TYPE,
	N_CAST,
	N_INT,
	N_FLOAT,
	N_CONST,
	N_KEYWORD,
	N_INDEX,
	N_IF,
	N_ELSE,
	N_ELSE_IF,
	N_IF_BLOCK,
	N_DECL,
	N_CALL,
	N_FUNC_DECL,
	N_OP_OVERLOAD,
	N_FUNC_DEF,
	N_STRUCT_DECL,
	N_SIGNATURE,
	N_VAR_ARGS,
	N_LAMBDA,

	N_QUESTION_MARK,

	N_HASHTAG,

	N_STRUCT_CONSTRUCTION,
	N_ARRAY_CONSTRUCTION,

	N_PLUGIN,

	N_ENUM_DECL,
	N_UNION_DECL,
	N_ETRUCT_DECL,

	N_DESUGARED,

	N_STR_LIT,
	N_TEMPLATES,
};
struct stmnt_nd
{
	node *n;
	scope *scp;
};
struct node
{
	node *l;
	node *r;

	bool modified;
	node* original;

    union
    {
        own_std::vector<node *> *extra;
        own_std::vector<comma_ret> *exprs;
    };
	decl2* decl;

	token2 *t;

	node_type type;

#ifdef DEBUG_NAME
	node *not_found_nd;
#endif
	union
	{

		tkn_type2 op_type;
		overload_op ovrld_op;
		node_type extra_type;
	};

	struct
	{
		int scope_line_start;
		int scope_line_end;
	};
	union
	{
		scope *scp;
		node *call_templates;
		type_struct2 *tstrct;
		bool is_unsigned;

		struct
		{
			func_decl *fdecl;
			std::string *str;
		};

		type2 decl_type;
		type2 *ptr_tp;
		struct
		{
			decl2 *ar_lit_decl;
			type2 *ar_lit_tp;
			int ar_byte_sz;
		};
		keyword kw;
		struct
		{
			int i_var_idx;
		}for_strct;
	};
		
	int flags;
	~node()
	{
	}
	void Free()
	{
		if(l != nullptr)
			l->Free();
		if(r != nullptr)
			r->Free();
		free(this);
	}
	node *NewTree(lang_state *lang_stat)
	{
		auto ret = new_node(lang_stat, this);
		if (this->t != nullptr)
		{
			ret->t = this->t->NewTkn(lang_stat);
		}


		if(this->l != nullptr)
			ret->l = this->l->NewTree(lang_stat);

		if(this->r!= nullptr)
			ret->r = this->r->NewTree(lang_stat);

		return ret;
		
	}
	void FreeTree()
	{
		if(this->l)
			this->l->FreeTree();
		if(this->r)
			this->r->FreeTree();

		__lang_globals.free(__lang_globals.data, this);
		
	}
};
enum msg_type
{
	MSG_BINOP,
	MSG_STMNT,
	MSG_UNOP,
	MSG_IDENTIFIER,
	MSG_INT,
	MSG_FLOAT,
	MSG_KEYWORD,
	MSG_INDEX,
	MSG_IF,
	MSG_ELSE,
	MSG_IF_BLOCK,
	MSG_DECL,
	MSG_CALL,
	MSG_CAST,
	MSG_RETURN,
	MSG_FUNC_DECL,
	MSG_MATCH,
	MSG_DONE,
};
 struct variable
 {
	std::string name;
	type2 type;
 };



#define SCOPE_INSIDE_FUNCTION 1
#define SCOPE_INSIDE_STRUCT   2
#define SCOPE_IS_GLOBAL   4
#define SCOPE_SERIALIZED   8
#define SCOPE_SKIP_SERIALIZATION   16

 enum scp_type
 {
	 SCP_TYPE_UNDEFINED,
	 SCP_TYPE_STRUCT,
	 SCP_TYPE_ENUM,
	 SCP_TYPE_FUNC,
	 SCP_TYPE_FILE,
 };
 struct cached_decl
 {
	 int hit;
	 decl2* d;
 };
#define CACHED_DECLS_MAX  128
struct scope
{
	scope *parent;
	own_std::vector<decl2 *> vars;
	std::unordered_map<std::string, decl2 *> vars_map;
	own_std::vector<scope *> children;
	own_std::vector<template_to_be_assigned> templs_to_be_assigned;

	own_std::vector<decl2 *> imports;

	cached_decl cached_decls[CACHED_DECLS_MAX];
	

	int flags;
	//int flags;
	scp_type type;
	int serialized_offset;
	int line_start;
	int line_end;

	func_decl *fdecl;
	type_struct2 *tstrct;
	decl2* e_decl;
	unit_file *file;

	decl2 *FindVariable(std::string &name);
	decl2 *FindVariableCached(std::string &name);
	std::string Print(int);

	void AssignDecls(decl2 **start, decl2 **end)
	{
		vars.assign(start, end);
	}
	void ClearDecls()
	{
		vars.clear();
		memset(cached_decls, 0, sizeof(cached_decls));
	}
	void AddDecl(decl2 *d)
	{
		vars.emplace_back(d);
		//vars_map[d->name] = d;
	}
	int GetNameSimpleHash(std::string &str)
	{
		int sz = str.size();
		int sum = sz;
		for(int i = 0; i < sz; i++)
		{
			sum += (str[i] * 2) << 1;
		}
		return sum;
	}
	void CacheDecl(decl2 *d)
	{
		bool cached = false;
		cached_decl* cached_ptr;
		u32 cur_min = INT_MAX;
		u32 cur_min_idx = INT_MAX;

		int simple_hash = GetNameSimpleHash(d->name);
		for(int i = 0; i < CACHED_DECLS_MAX; i++)
		{
			int idx = (i + simple_hash) % CACHED_DECLS_MAX;
			cached_ptr = &cached_decls[idx];
			decl2* d_aux = cached_ptr->d;

			if (d_aux == nullptr)
			{
				cached_ptr->d = d;
				cached = true;
				break;
			}
			if (cached_ptr->hit < cur_min)
			{
				cur_min = cached_ptr->hit;
				cur_min_idx = i;
			}
		}
		if (cached)
			return;

		cached_decls[cur_min_idx].d = d;
		cached_decls[cur_min_idx].hit = 1;
	}

};
struct message
{
	msg_type type;
	node *n;
	type2 lhs_type;
	type2 rhs_type;
	node *stmnt;
	scope *scp;
};
struct unit_file
{
	std::string name;
	std::string path;
//	std::string contents;
	char* contents;
	unsigned long long contents_sz;
	own_std::vector<token2> tkns;
	own_std::vector<func_byte_code *> bc_funcs;

	own_std::vector<char *> lines;

	scope *global;
	scope *funcs_scp;
	node *s;

	int file_dbg_idx;

	bool is_done;
};

bool decl2::AssignTemplate(lang_state *lang_stat, std::string tname, type2 *tp, comma_ret *given_arg)
{
	switch(type.type)
	{
	case enum_type2::TYPE_AUTO:
	{
		type = *tp;
		return true;
	}break;
	case enum_type2::TYPE_TEMPLATE:
	{
		if(type.templ_name == tname)
		{
			type.tp = tp;
			return true;
		}
	}break;
	case enum_type2::TYPE_FUNC_PTR:
	{
		auto func_ptr = type.fdecl;
		int cur_arg = 0;
		FOR_VEC(t, func_ptr->args)
		{
			//original function parameter signature args
			if ((*t)->AssignTemplate(lang_stat, tname, tp, given_arg))
			{
				auto func_ptr_in_call_arg = given_arg->decl.type.fdecl->args[cur_arg];
				ASSERT(func_ptr_in_call_arg->type.type == enum_type2::TYPE_AUTO)
				// func passed to func call
				func_ptr_in_call_arg->AssignTemplate(lang_stat, tname, tp, given_arg);
			}

			cur_arg++;
		}

		auto func_ptr_in_call= given_arg->decl.type.fdecl;

		if (func_ptr_in_call->ret_type.type == enum_type2::TYPE_AUTO)
		{
			DescendNode(lang_stat, func_ptr_in_call->func_node->r, func_ptr_in_call->scp);
			func_ptr->ret_type = func_ptr_in_call->ret_type;
		}
	}break;
	}
	return false;
}
enum import_type
{
	IMP_IMPLICIT_NAME,
	IMP_BY_ALIAS,
};
struct import_strct
{
	import_type type;
	std::string alias;
	unit_file *fl;
	decl2 *FindDecl(std::string name)
	{

		ASSERT(fl->global);
		scope* imp_scp = fl->global;
		// searching import scopes vars
		FOR_VEC(var, imp_scp->vars)
		{
			if ((*var)->name == name)
				return *var;
		}
		return nullptr;
	}
};
/*
bool IsCondOp(tkn_type2 t)
{
	switch(t)
	{
	case tkn_type2::T_EXCLAMATION:
	case tkn_type2::T_COND_EQ: return tkn_type2::T_COND_NE; break;
	case tkn_type2::T_COND_NE: return tkn_type2::T_COND_EQ; break;
	case tkn_type2::T_LESSER_THAN: return tkn_type2::T_GREATER_EQ; break;
	case tkn_type2::T_LESSER_EQ: return tkn_type2::T_GREATER_THAN; break;
	case tkn_type2::T_GREATER_THAN: return tkn_type2::T_LESSER_EQ; break;
	case tkn_type2::T_GREATER_EQ: return tkn_type2::T_LESSER_THAN; break;
	default: ASSERT(false)
	}
    return false;
}
*/
tkn_type2 OppositeCondCmp(tkn_type2 t)
{
	switch(t)
	{
	case tkn_type2::T_EXCLAMATION:
	case tkn_type2::T_COND_EQ: return tkn_type2::T_COND_NE; break;
	case tkn_type2::T_COND_NE: return tkn_type2::T_COND_EQ; break;
	case tkn_type2::T_LESSER_THAN: return tkn_type2::T_GREATER_EQ; break;
	case tkn_type2::T_LESSER_EQ: return tkn_type2::T_GREATER_THAN; break;
	case tkn_type2::T_GREATER_THAN: return tkn_type2::T_LESSER_EQ; break;
	case tkn_type2::T_GREATER_EQ: return tkn_type2::T_LESSER_THAN; break;
	default: ASSERT(false)
	}
	return tkn_type2::T_LESSER_THAN;
}
