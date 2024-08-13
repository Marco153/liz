#include "node.h"
#include "token.h"

enum ast_type
{
    AST_STATS,
    AST_FUNC,
    AST_INT,
    AST_IDENT,
    AST_RET,
    AST_BINOP,
    AST_UNOP,
    AST_ADDRESS_OF,
    AST_EXPR,
    AST_IF,
    AST_ELSE_IF,
    AST_ELSE,
    AST_WHILE,
    AST_CALL,
    AST_CAST,
    AST_STRUCT_COSTRUCTION,

    AST_DEREF,

    AST_FLOAT,

    AST_TYPE,

};

struct ast_rep;
struct ir_rep;
struct dbg_state;

typedef  void(*OutsiderFuncType)(dbg_state*);

struct ast_stats
{
};
struct ast_bool_exp
{
    own_std::vector<ir_rep *> false_conds;
    own_std::vector<ir_rep *> true_conds;

};
struct ast_loop
{
    ast_rep *cond;
    ast_rep *scope;
};
struct ast_call
{
    own_std::vector<ast_rep *>args;
	bool indirect;
    func_decl *fdecl;
    func_decl *in_func;
};
struct ast_struct_construct_info
{
    decl2 *var;
    ast_rep *exp;
};
struct ast_struct_construct
{
    type_struct2 *strct;

    int at_offset;
    own_std::vector<ast_struct_construct_info>commas;
};
struct ast_func
{
    ast_rep *stats;
    func_decl *fdecl;
};
struct ast_cast
{
    ast_rep * casted;
    type2 type;
};
struct ast_point
{
    ast_rep * exp;
    decl2 *decl_strct;
};
struct ast_deref
{
    ast_rep *exp;
    char times;
};
struct ast_if
{
    ast_rep * cond;
    own_std::vector<ast_rep *> elses;
    ast_rep *scope;
};
struct ast_rep
{
    ast_type type;
    tkn_type2 op;

    union
    {
        int line_number;
    };

    union
    {
        float f32;
        long long num;
        ast_rep *ast;
        decl2 *decl;
        std::string str;
		struct
		{
			own_std::vector<ast_rep*> stats;
			bool zero_initialized;
		};
        own_std::vector<ast_rep *> expr;
        own_std::vector<ast_point> points;
        ast_if cond;
        ast_func func;
        ast_loop loop;
        ast_call call;
        ast_cast cast;
        ast_deref deref;
        ast_struct_construct strct_constr;
        type2 tp;

    };
    ~ast_rep()
    {
    }
};

enum ir_type
{
    IR_NONE,
    IR_NOP,

    IR_ASSIGNMENT,
    IR_RET,
    IR_CMP_EQ,
    IR_CMP_NE,
    IR_CMP_LT,
    IR_CMP_GE,
    IR_BREAK_IF,

    IR_SPILL_REG,
    IR_UNSPILL_REG,

	IR_SKIPABLE,

    IR_BEGIN_CALL,
    IR_END_CALL,

    IR_STACK_BEGIN,
    IR_STACK_END,
    IR_DECLARE_LOCAL,
    IR_DECLARE_ARG,

    IR_BEGIN_LOOP_BLOCK,
    IR_END_LOOP_BLOCK,

    IR_BEGIN_OR_BLOCK,
    IR_END_OR_BLOCK,

    IR_BEGIN_AND_BLOCK,
    IR_END_AND_BLOCK,

    IR_BEGIN_COND_BLOCK,
    IR_END_COND_BLOCK,

    IR_BEGIN_IF_BLOCK,
    IR_END_IF_BLOCK,

    IR_BEGIN_SUB_IF_BLOCK,
    IR_END_SUB_IF_BLOCK,

    IR_BEGIN_COMPLEX,
    IR_END_COMPLEX,

    IR_BEGIN_STMNT,
    IR_END_STMNT,

    IR_CALL,
    IR_INDIRECT_CALL,

};
enum ir_val_type
{
    IR_TYPE_F32_REG,
    IR_TYPE_REG,
    IR_TYPE_INT,
    IR_TYPE_F32,
    IR_TYPE_PARAM_REG,
    IR_TYPE_ARG_REG,
    IR_TYPE_RET_REG,
    IR_TYPE_DECL,
    IR_TYPE_ON_STACK,
};

struct ir_val
{
    ir_val_type type;
    bool is_unsigned;
	bool is_float;
    char ptr;
    char reg_sz;
    union
    {
        decl2 *decl;
        int i;
        float f32;
        struct
        {
            char reg;
        };
    };
};

struct ir_rep
{
    ir_type type;
    int idx;
    int start;
    int end;
    union
    {
        struct
        {
            ir_val dst;
            int offset;
        }spill;
        struct
        {
            ir_val dst;
        }complx;
        struct
        {
			union
			{
				func_decl* fdecl;
				OutsiderFuncType outsider;
			};
			bool is_outsider;
			int i;
        }call;
        struct
        {
            struct
            {
                int code_start;
                int line;
            }stmnt;
            int other_idx;
        }block;
        struct
        {
            decl2 *decl;
            func_decl *fdecl;
        };
        int num;
        struct
        {
            ir_val lhs;
            ir_val rhs;
            bool only_lhs;

            tkn_type2 op;
            bool it_is_jmp_if_true;
        }bin;
        struct
        {
            ir_val lhs;
            ir_val rhs;
            bool only_lhs;
        }ret;
        struct
        {
            ir_val lhs;
            ir_val rhs;
            bool only_lhs;

            ir_val to_assign;
            tkn_type2 op;
        }assign;
    };

};
