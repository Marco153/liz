#include "node.h"
#include "token.h"

enum ast_type
{
    AST_EMPTY,
    AST_BREAK,
    AST_DBG_BREAK,
    AST_STATS,
    AST_CONTINUE,
    AST_FUNC,
    AST_INT,
    AST_TYPE_DATA,
    AST_GET_FUNC_BC,
    AST_CHAR,
    AST_IDENT,
    AST_RET,
    AST_BINOP,
    AST_UNOP,
    AST_DEFER,
    AST_STR_LIT,
    AST_ADDRESS_OF,
    AST_EXPR,
    AST_IF,
    AST_ELSE_IF,
    AST_ELSE,
    AST_INDEX,
    AST_WHILE,
    AST_FOR,
    AST_CALL,
    AST_CAST,
    AST_ON,
    AST_STRUCT_COSTRUCTION,
    AST_ARRAY_COSTRUCTION,

    AST_DEREF,
    AST_LABEL,

    AST_FLOAT,
    AST_F64,

    AST_TYPE,
    AST_NEGATIVE,
    AST_NEGATE,
    AST_OPPOSITE,
    AST_PLUS_PLUS,
    AST_MINUS_MINUS,

};
enum ast_index_type
{
	AST_INDEX_TP_NORMAL,
};

struct ast_rep;
struct ir_rep;
struct dbg_state;

typedef  void(*OutsiderFuncType)(int, dbg_state*);

struct ast_stats
{
};
struct ast_bool_exp
{
    own_std::vector<ir_rep *> false_conds;
    own_std::vector<ir_rep *> true_conds;

};
struct ast_for
{
	ast_rep* start_stat;
	ast_rep* cond_stat;
	ast_rep* at_loop_end_stat;
	ast_rep* scope;
};
struct ast_index
{
	ast_index_type type;
	type2 lhs_type;
	ast_rep* lhs;
	ast_rep* rhs;
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
	decl2 *func_ptr_var;
    func_decl *fdecl;
    func_decl *in_func;
    ast_rep* lhs;
};
struct ast_struct_construct_info
{
    decl2 *var;
    ast_rep *exp;
};
struct ast_array_construct
{
    type2 type;

    int at_offset;

	own_std::vector<ast_rep *>commas;
};
struct ast_struct_construct
{
    type_struct2 *strct;
    bool is_vector;

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
	type2 type;
    char times;
};
struct ast_opposite
{
    ast_rep *exp;
	tkn_type2 op;
};
struct ast_if
{
    ast_rep * cond;
    own_std::vector<ast_rep *> elses;
    ast_rep *scope;
    bool from_on_ast;
	type2 expr_type;
};
struct on_cond_ast
{
    ast_rep *cond;
    ast_rep *scp;
};
struct ast_rep
{
  ast_type type;
  tkn_type2 op;
  bool stmnt_without_semicolon : 1;
  bool dont_make_dbg_stmnt : 1;
  bool goes_onto_stack : 1;
  bool is_import : 1;
  bool ir_generated : 1;

  int line_number;

  int line_offset_start;
  union
  {
    int line_offset_end;
    int at_stack_offset;
  };
  type2 lhs_tp;

  union
  {
    double f64;
    float f32;
    long long num;
    ast_rep *ast;
    decl2 *decl;
    own_std::string str;
    struct
    {
      own_std::vector<ast_rep*> stats;
      bool zero_initialized;
    };
    struct
    {
      own_std::vector<ast_rep*> expr;
    }e_holder;
    struct
    {
      ast_rep *main;
      ast_rep *def;
      own_std::vector<on_cond_ast> exprs;
    }on;

    struct
    {
      own_std::vector<ast_point> points;
      bool point_get_last_val;
    };
    struct
    {
      ast_rep* ast;
      type2 tp;
    }unop_assign;
    struct
    {
      ast_rep* ast;
      type2 tp;
    }ret;
    ast_if cond;
    ast_func func;
    ast_loop loop;
    ast_call call;
    ast_cast cast;
    ast_for for_info;
    ast_opposite opposite;
    ast_deref deref;
    ast_struct_construct strct_constr;
    ast_array_construct ar_constr;
    type2 tp;
    ast_index index;

  };
  ~ast_rep()
  {
  }
};

enum ir_type
{
    IR_NONE,
    IR_NOP,

    IR_LOAD,
    IR_STORE,
    IR_ADDRESS_OF,

    IR_ASSIGNMENT,
    IR_RET,
    IR_REPMOVSB,
    IR_LABEL,
    IR_BIN,
    IR_CMP,
    IR_CMP_EQ,
    IR_CMP_NE,
    IR_CMP_LT,
    IR_CMP_LE,
    IR_CMP_GE,
    IR_CMP_GT,
    IR_JMP,
    IR_BREAK_OUT_IF_BLOCK,

    IR_CAST_INT_TO_F32,
    IR_CAST_F32_TO_INT,
    IR_CAST_INT_TO_INT,
    IR_CAST_FLOAT_TO_FLOAT,

    IR_SPILL_REG,
    IR_UNSPILL_REG,

    IR_SKIPABLE,

    IR_BEGIN_CALL,
    IR_END_CALL,

    IR_STACK_BEGIN,
    IR_STACK_END,
    IR_PROLOGUE_END,
    IR_DECLARE_LOCAL,
    IR_DECLARE_ARG,

    IR_BREAK,

    IR_SPILL,
    IR_UNSPILL,

    IR_BEGIN_BLOCK,
    IR_END_BLOCK,

    IR_BEGIN_LOOP_BLOCK,
    IR_END_LOOP_BLOCK,

    IR_BEGIN_OR_BLOCK,
    IR_END_OR_BLOCK,

    IR_BEGIN_AND_BLOCK,
    IR_END_AND_BLOCK,

    IR_BEGIN_COND_BLOCK,
    IR_END_COND_BLOCK,

    IR_CONTINUE,

	IR_BEGIN_IF_EXPR_BLOCK,
	IR_END_IF_EXPR_BLOCK,

	IR_BEGIN_ON_BLOCK,
	IR_END_ON_BLOCK,

	IR_BEGIN_IF_BLOCK,
	IR_END_IF_BLOCK,

	IR_BEGIN_SUB_IF_BLOCK,
	IR_END_SUB_IF_BLOCK,

	IR_DBG_BREAK,

    IR_BEGIN_COMPLEX,
    IR_END_COMPLEX,

    IR_BEGIN_STMNT,
    IR_END_STMNT,

    IR_CALL,
    IR_INDIRECT_CALL,

    IR_GET_FLOAT,
    IR_CLEAR_SSE,
};
enum ir_val_type
{
    IR_TYPE_NONE,
    IR_TYPE_REG,
    IR_TYPE_REG_MEM,
    IR_TYPE_INT,
    IR_TYPE_INT64,
    IR_TYPE_F32,
    IR_TYPE_F64,
    IR_TYPE_STR_LIT,
    IR_TYPE_PARAM_REG,
    IR_TYPE_ARG_REG,
    IR_TYPE_RET_REG,
    IR_TYPE_DECL,
    IR_TYPE_GET_FUNC_BC,
    IR_TYPE_ON_STACK,
    IR_TYPE_TYPE_DATA,
};
enum on_stack_type
{
	ON_STACK_STRUCT_CONSTR,	
	ON_STACK_STRUCT_RET,	
	ON_STACK_SPILL,	
	ON_STACK_SPILL_FLOAT,	
};
#define IR_VAL_ADDR 0 
#define IR_VAL_VALUE 1
struct ir_val
{
  ir_val_type type;
  int kind;
  union
  {
    decl2* decl;
    func_decl* fdecl;
  };
  union
  {
    int decl_offset;
    struct
    {
      on_stack_type on_stack_type;
      int i;
    }stack;
    float f32;
    double f64;
    char* str;
    u64 val;
    int i;
    u64* val_ptr;
    struct
    {
      union
      {
        char reg;
      };
    };
  };
  int voffset;
  short reg_ex;
  int on_data_sect_offset;
  bool is_unsigned : 1;
  bool is_float:1;
  bool is_packed_float:1;
  char ptr;
  char deref;
  char reg_sz;
};

struct assign_info
{
  ir_val lhs;
  ir_val rhs;
  bool only_lhs;

  ir_val to_assign;
  tkn_type2 op;
};
struct ir_rep
{
  ir_type type;
  int idx;
  union
  {
    int start;
    int dst_ir_rel_idx;
  };
  union
  {
    int end;
  };
  union
  {
    int dbg_int;
    struct
    {
      bool dbg_break;
      bool one_dbg_break;
    };
  };
  union
  {
    type_struct2 *strct;
    struct
    {
      ir_val dst;
    }complx;
    struct
    {
      union
      {
        func_decl* fdecl;
        decl2 *func_ptr_var;
        OutsiderFuncType outsider;
      };
      bool is_outsider;
      int i;
    }call;
    int i;
    struct
    {
      struct
      {
        int code_start;
        int line;
      }stmnt;
      bool is_for_loop;
      int other_idx;
      int for_loop_end_stat;
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
      u32 block_id;
    }bin;
    assign_info assign;
    struct
    {
      ir_val spilled;
      int offset;
    }spill;
    struct
    {
      assign_info assign;
      bool no_ret_val;
    }ret;
  };

};
struct block2
{
  u32 id;
  int code_start;
  int code_end;
  bool generated;
  bool emitted;
  own_std::vector<ir_rep> irs;
  own_std::vector<u32> jmp_rels;
};


template<class T>
struct tracker_stack
{
  T *ptr;
  u32 cur;
  u32 max_cur_gotten;
  u32 limit;
};


template<class T>
void init_tracker_stack(tracker_stack<T> *ar, u32 limit)
{
  ar->ptr = (T *)__lang_globals.alloc(__lang_globals.data, limit * sizeof(T));
  ar->limit = limit;
}
template<class T>
T *pop_tracker_stack(tracker_stack<T> *ar)
{
  if(ar->cur == 0) return nullptr;
  ar->cur--;
  T *ret = (ar->ptr + ar->cur);

  return ret;
}

template<class T>
T *top_tracker_stack(tracker_stack<T> *ar)
{
  if(ar->cur == 0 ) return nullptr;
  return ar->ptr + (ar->cur - 1);
}
template<class T>
void push_tracker_stack(tracker_stack<T> *ar, T val)
{
  memcpy((ar->ptr + ar->cur), &val, sizeof(T));

  ar->cur++;

  ASSERT(ar->cur < ar->limit);

  if(ar->max_cur_gotten < ar->cur)
    ar->max_cur_gotten = ar->cur;
}

struct thread_ir_state
{
  block2 *cur_block;
  func_decl *cur_func;

	void *blocks_ptr;
	int blocks_cur;
	int blocks_max;

  tracker_stack<char> spilled_regs;
  int strct_ret_size_per_statement_cur;
  int strct_ret_size_per_statement_max_gotten;
  tracker_stack<block2 *> continue_start_block;
  tracker_stack<block2 *> break_end_block;
};
int get_strct_ret(thread_ir_state *state)
{
  return state->strct_ret_size_per_statement_cur;
}
void clear_strct_ret(thread_ir_state *state)
{
  state->strct_ret_size_per_statement_cur = 0;
}
void add_strct_ret(thread_ir_state *state, int sz)
{
  state->strct_ret_size_per_statement_cur += sz;
  if(state->strct_ret_size_per_statement_cur > state->strct_ret_size_per_statement_max_gotten)
  {
    state->strct_ret_size_per_statement_max_gotten = state->strct_ret_size_per_statement_cur;
  }
}
