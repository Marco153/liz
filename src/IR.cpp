#include "IR.h"
#include "bytecode.h"
#include "error_report.h"
#include "node.h"
#include "token.h"
#include "token_common.h"
#include <algorithm>

bool IsAstSimple(lang_state *lang_stat, ast_rep *ast);
decl2 *PointLogic(lang_state *lang_stat, node *n, scope *scp, type2 *ret_tp);
bool NameFindingGetType(lang_state *lang_stat, node *n, scope *scp,
                        type2 &ret_type, int);
void GenStackThenIR(lang_state *lang_stat, ast_rep *ast,
                    own_std::vector<ir_rep> *out, ir_val *dst_val,
                    ir_val *i = nullptr);
void CreateOppositeRegAssigmentAfterCondChecking(
    lang_state *lang_stat, own_std::vector<ir_rep> *out, int sub_if_idx,
    int if_idx, int reg, char true_cond_final_reg_val = 1,
    char false_cond_final_reg_val = 0);
bool IsNodeOperator(node *nd, tkn_type2 tkn);
ir_val GetIRFromAst2(lang_state *lang_stat, ast_rep *ast, thread_ir_state *, bool is_lhs);

int clamp(int, int, int);

#define MAKE_DST_IR_VAL(ir_tp, ptr) (((short)ir_tp) | (((int)ptr) << 16))
#define MAKE_DST_IR_VAL(ir_tp, ptr) (((short)ir_tp) | (((int)ptr) << 16))

#define IR_VAL_FROM_POINT 0x100
#define IR_VAL_ALREADY_ON_PARAM_REG 0x200
#define IR_VAL_FROM_DEREF 0x400
#define IR_VAL_FROM_POINT_BUT_IS_STATIC_ARRAY 0x800
#define IR_VAL_FROM_AST_INDEX 0x1000

#define IR_BEGIN_ARG 25
#define IR_END_ARG 26
#define IR_BEGIN_AND 20
#define IR_END_AND 21
#define IR_BEGIN_OR 22
#define IR_END_OR 23
#define IR_BEGIN_INSIDE_IF 24
#define IR_END_INSIDE_IF 25
#define IR_BEGIN_FUNC_CALL 26
#define IR_END_FUNC_CALL 27

bool CheckIrValIsPointIncDeref(ir_val *val) {
  if (IS_FLAG_ON(val->reg_ex, IR_VAL_FROM_POINT) && val->type != IR_TYPE_DECL &&
      val->deref < 0) {
    val->deref++;
  }
  return false;
}
ast_rep *NewAstLinear(lang_state *lang_stat) {
  auto ret = (ast_rep *)lang_stat->start_ast + lang_stat->cur_ast;
  memset(ret, 0, sizeof(ast_rep));
  lang_stat->cur_ast++;
  ASSERT(lang_stat->cur_ast < lang_stat->max_ast);
  return ret;
}
ast_rep *NewAst() {
  auto ret =
      (ast_rep *)__lang_globals.alloc(__lang_globals.data, sizeof(ast_rep));
  memset(ret, 0, sizeof(ast_rep));
  return ret;
}

bool CheckStmntWithoutSemicolon(lang_state *lang_stat,
                                own_std::vector<ast_rep *> *ar) {
  FOR_VEC(stmnt, (*ar)) {
    ast_rep *st = *stmnt;
    bool is_stmnt_without_semicolon = st->stmnt_without_semicolon;
    if (is_stmnt_without_semicolon) {
      // stmnt without semicolon should be at the end of the scope, because they
      // are like a return
      ASSERT((stmnt + 1) == ar->end());
      return true;
    }
  }
  return false;
}

ast_rep *CreateAstBin(lang_state *lang_stat, tkn_type2 op, ast_rep *lhs,
                      ast_rep *rhs, node *lhs_n, scope *scp) {
  ast_rep *ret = NewAst();
  ret->type = AST_BINOP;
  ret->op = op;
  ret->lhs_tp = DescendNode(lang_stat, lhs_n, scp);
  ret->e_holder.expr.emplace_back(lhs);
  ret->e_holder.expr.emplace_back(rhs);

  return ret;
}
void InsertDeferd(ast_rep **out, scope *scp, bool recursive, bool is_break) {
  auto start_func = scp->fdecl;
  while (scp && scp->fdecl == start_func) {
    if (scp->defered.size() > 0) {
      if ((*out)->type != AST_STATS) {
        ast_rep *new_ret = NewAst();
        new_ret->type = AST_STATS;
        INSERT_VEC(new_ret->stats, scp->defered);
        new_ret->stats.emplace_back(*out);
        *out = new_ret;
      } else {
        INSERT_VEC((*out)->stats, scp->defered);
      }
    }
    if (!recursive)
      return;
    if(is_break && scp->is_loop)
    {
      return;
    }
    scp = scp->parent;
  }
}
ast_rep *AstFromNode(lang_state *lang_stat, node *n, scope *scp) {
  ast_rep *ret = NewAst();
  ret->line_number = n->t->line;
  ret->line_offset_start = n->t->line_offset;
  type2 dummy_type;
  switch (n->type) {
  case N_TYPE: {
    ret->type = AST_TYPE;
    ret->tp = n->decl_type;
  } break;
  case node_type::N_UNOP: {
    switch (n->t->type) {
    case T_PERCENT: {
      ret->type = AST_REG;
      ret->reg.reg_sz = 8;
      ret->reg.type = IR_TYPE_REG;
      ret->reg.kind = IR_VAL_VALUE;

      if(n->r->t->str == "rax")
      {
        ret->reg.reg = (char)regs_enum::RAX;
      }
      else if(n->r->t->str == "rcx")
      {
        ret->reg.reg = (char)regs_enum::RCX;
      }
      else if(n->r->t->str == "rdx")
      {
        ret->reg.reg = (char)regs_enum::RDX;
      }
      else if(n->r->t->str == "rbp")
      {
        ret->reg.reg = (char)regs_enum::RBP;
      }
      else if(n->r->t->str == "rsp")
      {
        ret->reg.reg = (char)regs_enum::RSP;
      }
      else if(n->r->t->str == "rdi")
      {
        ret->reg.reg = (char)regs_enum::RDI;
      }
      else if(n->r->t->str == "rsi")
      {
        ret->reg.reg = (char)regs_enum::RSI;
      }
      else if(n->r->t->str == "rbx")
      {
        ret->reg.reg = (char)regs_enum::RBX;
      }
      else if(n->r->t->str[0] == 'x')
      {
        char reg;
        if(n->r->t->str.size() == 5)
        {
          char dezena = n->r->t->str[3] - '0';
          char unidade = n->r->t->str[4] - '0';

          reg = dezena * 10 + unidade;
        }
        else
        {
          char unidade = n->r->t->str[3] - '0';
          reg = unidade;
        }
        ret->reg.reg = reg;
        ret->reg.is_float = true;
      }
      else
      {
        if(n->r->t->str[0]=='r' && IsNumber(n->r->t->str[1]))
        {

          char reg;
          if(n->r->t->str.size() == 3)
          {
            char dezena = n->r->t->str[1] - '0';
            char unidade = n->r->t->str[2] - '0';

            reg = dezena * 10 + unidade;
          }
          else
          {
            char unidade = n->r->t->str[1] - '0';
            reg = unidade;
          }
          ret->reg.reg = (char)regs_enum::R8 + (reg - 8);
          ret->reg.type = IR_TYPE_REG;
          ret->reg.reg_sz = 8;
        }
      }
      //if()
      //ret->unop_assign.tp = DescendNode(lang_stat, n->r, scp);
      //ret->unop_assign.ast = AstFromNode(lang_stat, n->r, scp);
    } break;
    case T_MINUS_MINUS: {
      ret->type = AST_MINUS_MINUS;
      ret->unop_assign.tp = DescendNode(lang_stat, n->r, scp);
      ret->unop_assign.ast = AstFromNode(lang_stat, n->r, scp);
    } break;
    case T_PLUS_PLUS: {
      ret->type = AST_PLUS_PLUS;
      ret->unop_assign.tp = DescendNode(lang_stat, n->r, scp);
      ret->unop_assign.ast = AstFromNode(lang_stat, n->r, scp);
    } break;
    case T_EXCLAMATION: {
      ret->type = AST_OPPOSITE;
      ret->ast = AstFromNode(lang_stat, n->r, scp);
    } break;
    case T_TILDE: {
      ret->type = AST_NEGATE;
      ret->ast = AstFromNode(lang_stat, n->r, scp);
    } break;
    case T_MINUS: {
      ret->type = AST_NEGATIVE;
      ret->ast = AstFromNode(lang_stat, n->r, scp);
      ret->lhs_tp = DescendNode(lang_stat, n->r, scp);
    } break;
    case T_AMPERSAND: {
      ret->type = AST_ADDRESS_OF;
      ret->ast = AstFromNode(lang_stat, n->r, scp);
      type2 *tp = &ret->ast->lhs_tp;
      if (tp->type == TYPE_VECTOR && !CMP_NTYPE_BIN(n->r, T_POINT)) {
        ret->goes_onto_stack = true;
        ret->at_stack_offset = lang_stat->cur_strct_constrct_size_per_statement;

        lang_stat->cur_strct_constrct_size_per_statement += 16;

        int cur_sz = lang_stat->cur_func->strct_constrct_size_per_statement;
        lang_stat->cur_func->strct_constrct_size_per_statement =
            max(cur_sz, lang_stat->cur_strct_constrct_size_per_statement);
      }
    } break;
    case T_MUL: {
      ret->type = AST_DEREF;
      node *cur = n;
      while (cur->type == N_UNOP && cur->t->type == T_MUL) {
        ret->deref.times++;
        cur = cur->r;
      }
      ret->deref.exp = AstFromNode(lang_stat, cur, scp);
      ret->deref.type = DescendNode(lang_stat, n, scp);

      ASSERT(ret->deref.exp->type != AST_EMPTY);
    } break;
    case T_DOLLAR: {
      ret->type = AST_LABEL;
      ret->decl = FindIdentifier(n->r->t->str, scp, &dummy_type);
    } break;
    default:
      ASSERT(0)
    }
  } break;
  case node_type::N_CAST: {
    ret->type = AST_CAST;
    NameFindingGetType(lang_stat, n->l, scp, ret->cast.type);
    ret->cast.type.type = FromTypeToVarType(ret->cast.type.type);
    ret->cast.casted = AstFromNode(lang_stat, n->r, scp);
  } break;
  case node_type::N_ON: {
    ret->type = AST_ON;
    ret->on.main = AstFromNode(lang_stat, n->on->main, scp);
    FOR_VEC(cur_cond, n->on->exprs) {
      on_cond_ast cur = {};
      cur.cond = AstFromNode(lang_stat, cur_cond->cond, scp);
      cur.scp = AstFromNode(lang_stat, cur_cond->scp, scp);
      ret->on.exprs.emplace_back(cur);
    }
    if (n->on->def) {
      ret->on.def = AstFromNode(lang_stat, n->on->def, scp);
    }
    // ret->cast.type = AstFromNode(lang_stat, n->l, scp);
  } break;
  case node_type::N_INDEX: {
    ret->type = AST_INDEX;
    BREAK(n->t->line == 18)
    ret->index.type = AST_INDEX_TP_NORMAL;
    ret->index.lhs_type = DescendNode(lang_stat, n->l, scp);
    ret->index.lhs = AstFromNode(lang_stat, n->l, scp);
    ret->index.rhs = AstFromNode(lang_stat, n->r, scp);
  } break;
  case node_type::N_ELSE: {
    ret->type = AST_ELSE;
    ret->cond.scope = AstFromNode(lang_stat, n->r, scp);
  } break;
  case node_type::N_ELSE_IF: {
    ret->type = AST_ELSE_IF;
    ret->cond.cond = AstFromNode(lang_stat, n->l->l, scp);
    ret->cond.scope = AstFromNode(lang_stat, n->l->r, scp);
  } break;
  case node_type::N_WHILE: {
    ret->type = AST_WHILE;
    ret->loop.cond = AstFromNode(lang_stat, n->l, scp);
    ret->loop.scope = AstFromNode(lang_stat, n->r, scp);
  } break;
  case node_type::N_IF: {
    ret->type = AST_IF;

    ret->cond.cond = AstFromNode(lang_stat, n->l->l, scp);

    ret->cond.scope = AstFromNode(lang_stat, n->l->r, scp);
    node *cur = n->r;
    while (cur && (cur->type == N_ELSE_IF || cur->type == N_ELSE)) {
      ret->cond.elses.emplace_back(AstFromNode(lang_stat, cur, scp));
      cur = cur->r;
    }


    if(IS_FLAG_ON(n->flags, NODE_FLAGS_IF_EXPR))
    {
      //HERE()
      ASSERT(n->decl_type.type != TYPE_AUTO);
      ret->cond.expr_type = n->decl_type;
    }

    if (CheckStmntWithoutSemicolon(lang_stat, &ret->cond.scope->stats))
      GetLastStmntType(lang_stat, n->l->r->r, scp, ret->cond.expr_type);

    FOR_VEC(cond, ret->cond.elses) {
      ast_rep *c = *cond;
      CheckStmntWithoutSemicolon(lang_stat, &c->cond.scope->stats);
    }

  } break;
  case node_type::N_BINOP: {
    if (n->r->type == N_QUESTION_MARK)
      return nullptr;
    // ret->t = n->l->t;
    if (n->r->type == N_SCOPE)
      return AstFromNode(lang_stat, n->r, scp);
    if (n->t->type == T_COLON || n->r->type == N_SCOPE)
      return AstFromNode(lang_stat, n->l, scp);

    ret->type = AST_BINOP;
    ret->op = n->t->type;

    own_std::vector<node *> node_stack;
    node *cur_node = n;
    node_stack.emplace_back(n);

    // putting all equal operators in a single array
    while (cur_node->l && cur_node->l->type == node_type::N_BINOP &&
           cur_node->l->t->type == n->t->type) {
      node_stack.emplace_back(cur_node->l);
      cur_node = cur_node->l;
    }
    int last_idx = node_stack.size() - 1;
    node *cur = *(node_stack.begin() + last_idx);

    ret->lhs_tp = DescendNode(lang_stat, cur->l, scp);

    ast_rep *lhs = AstFromNode(lang_stat, cur->l, scp);
    lhs->lhs_tp = ret->lhs_tp;
    ast_rep *rhs = AstFromNode(lang_stat, cur->r, scp);
    if (!CMP_NTYPE_BIN(cur, T_POINT))
      rhs->lhs_tp = DescendNode(lang_stat, cur->r, scp);

    ret->e_holder.expr.emplace_back(lhs);
    ret->e_holder.expr.emplace_back(rhs);

    for (int i = node_stack.size() - 2; i >= 0; i--) {
      cur = *(node_stack.begin() + i);
      rhs = AstFromNode(lang_stat, cur->r, scp);
      if (!CMP_NTYPE_BIN(cur, T_POINT))
        rhs->lhs_tp = DescendNode(lang_stat, cur->r, scp);
      ret->e_holder.expr.emplace_back(rhs);
    }

    switch (n->t->type) {
    case T_POINT: {
      ret->point_get_last_val = true;

      node *first_node = *(node_stack.begin() + last_idx);

      dummy_type = DescendNode(lang_stat, first_node->l, scp);

      if (dummy_type.type == TYPE_ENUM_TYPE) {
        decl2 *d = dummy_type.e_decl->type.GetEnumDecl(n->r->t->str);
        ret->type = AST_INT;
        ret->num = d->type.e_idx;
        ;
      } else if (dummy_type.type == TYPE_STRUCT_TYPE &&
                 IS_FLAG_ON(dummy_type.strct->flags, TP_STRCT_ETRUCT)) {
        decl2 *d = dummy_type.strct->FindDecl(n->r->t->str);
        ret->type = AST_INT;
        ret->num = d->type.e_idx;
        ;
      } else {
        if(dummy_type.type == TYPE_IMPORT)
        {
          ASSERT(first_node->r->t->str.size() != 0)
          decl2 *d = dummy_type.imp->FindDecl(first_node->r->t->str);
          ret->is_import = true;

          switch(d->type.type)
          {
          case TYPE_INT:
          {
            ret->type = AST_INT;
            ret->num = d->type.i;
          }break;
          case TYPE_FUNC_EXTERN:
          {
            ret->type = AST_IDENT;
            ret->decl = d;
          }break;
          default: ASSERT(false)
          }

        }
        else
        {
          auto new_ar = (own_std::vector<ast_point> *)AllocMiscData(
              lang_stat, sizeof(own_std::vector<ast_point>));
          memset(new_ar, 0, sizeof(*new_ar));

          ast_point aux;
          aux.decl_strct = dummy_type.strct->this_decl;
          aux.exp = ret->e_holder.expr[0];
          new_ar->emplace_back(aux);

          decl2 *strct = aux.decl_strct;
          for (int i = node_stack.size() - 1; i >= 0; i--) {
            first_node = *(node_stack.begin() + i);

            decl2 *is_struct = nullptr;
            bool is_tuple = IS_FLAG_ON(strct->type.strct->flags, TP_STRCT_TUPLE);
            if (is_tuple) {
              is_struct = strct->type.strct->scp->vars[first_node->t->i];
            } else
              is_struct = FindIdentifier(first_node->r->t->str,
                                        strct->type.strct->scp, &dummy_type);
            ASSERT(is_struct);
            if (is_struct) {
              aux.decl_strct = is_struct;
              aux.exp =
                  AstFromNode(lang_stat, first_node->r, strct->type.strct->scp);
              // aux.
              if (is_tuple) {
                aux.exp->type = AST_IDENT;
                aux.exp->decl = is_struct;
              }

              strct = aux.decl_strct;
              rhs->decl = is_struct;
              ret->lhs_tp = is_struct->type;
            }
            new_ar->emplace_back(aux);
          }
          // THIS CAUSES A LEAK
          memcpy(&ret->points, new_ar, sizeof(*new_ar));
        }
      }
      // ret->points = *new_ar;

    } break;
    case T_COND_OR:
    case T_PIPE:
    case T_COND_AND:
    case T_COMMA:
    case T_MUL:
    case T_COND_NE:
    case T_LESSER_EQ:
    case T_PLUS_EQUAL:
    case T_MINUS_EQUAL:
    case T_COND_EQ:
    case T_AMPERSAND:
    case T_HAT:
    case T_LESSER_THAN:
    case T_GREATER_EQ:
    case T_GREATER_THAN:
    case T_MINUS:
    case T_SHIFT_LEFT:
    case T_SHIFT_RIGHT:
    case T_DIV:
    case T_PERCENT: {
    } break;
    case T_EQUAL: {

    } break;
    case T_PLUS: {

    } break;
    default:
      ASSERT(0);
    }
  } break;
  case node_type::N_INT64: {
    ret->type = AST_INT64;
    ret->num = n->t->i64;
  } break;
  case node_type::N_INT: {
    ret->type = AST_INT;
    ret->num = n->t->i;
  } break;
  case N_CONST_DECL: {
    ret->type = AST_IDENT;
    type2 ret_type;
    ret->decl = FindIdentifier(n->l->t->str, scp, &ret_type);
    if (!ret->decl) {
      ret->str = n->t->str;
    }
  } break;
  case node_type::N_IDENTIFIER: {
    ret->type = AST_IDENT;
    type2 ret_type;
    if (n->decl) {
      ret->decl = n->decl;
      memcpy(&ret_type, &ret->decl->type, sizeof(type2));
      // ret_type = ret->decl->type;
    } else
      ret->decl = FindIdentifier(n->t->str, scp, &ret_type);
    if (!ret->decl) {
      ret->str = n->t->str;
    }
  } break;
  case node_type::N_KEYWORD: {
    switch (n->kw) {
    case KW_CONTINUE: {
      ret->type = AST_CONTINUE;
      InsertDeferd(&ret, scp, true, true);
    } break;
    case KW_DBG_BREAK: {
      ret->type = AST_DBG_BREAK;
    } break;
    case KW_BREAK: {
      ret->type = AST_BREAK;
      //BREAK(n->t->line == 7400)
      InsertDeferd(&ret, scp, true, true);
    } break;
    case KW_USING: {
      ret->type = AST_EMPTY;
    } break;
    case KW_TRUE: {
      ret->type = AST_INT;
      ret->num = 1;
    } break;
    case KW_NIL:
    case KW_FALSE: {
      ret->type = AST_INT;
      ret->num = 0;
    } break;
    case KW_RETURN: {
      ret->type = AST_RET;
      if (IS_FLAG_ON(scp->fdecl->flags, FUNC_DECL_COROUTINE)) {
        own_std::string str("CoroutineEnding");
        decl2 *d = FindIdentifier(str, scp, &dummy_type);

        decl2 *first_arg = d->type.fdecl->args[0];

        node *label_name = new_node(lang_stat, n->t);
        label_name->type = N_IDENTIFIER;
        label_name->t->str =
            own_std::string("_ret_label") +
            own_std::to_string(scp->fdecl->total_hidden_ret_labels);

        scp->fdecl->total_hidden_ret_labels++;

        first_arg->type.nd = label_name;

        node *new_tree = d->type.fdecl->func_node->r->r->NewTree(lang_stat);
        BuildMacroTree(lang_stat, d->type.fdecl->scp, new_tree, n->t->line);
        LookingForLabels(lang_stat, new_tree, scp);

        decl2 *res = DescendNameFinding(lang_stat, new_tree, scp);
        ASSERT(res);

        DescendNode(lang_stat, new_tree, scp);

        // disabling the flag, because it will cause this trigger this return
        // again on the other return that we have inside the macro
        scp->fdecl->flags &= ~FUNC_DECL_COROUTINE;
        ret = AstFromNode(lang_stat, new_tree, scp);
        scp->fdecl->flags |= FUNC_DECL_COROUTINE;
        ASSERT(ret->type == AST_STATS);
        FOR_VEC(cur_st_ptr, ret->stats) {
          ast_rep *cur_st = *cur_st_ptr;
          cur_st->dont_make_dbg_stmnt = true;
        }
        ret->stats[ret->stats.size() - 2]->dont_make_dbg_stmnt = false;
      }
      if (n->r) {
        ret->ret.ast = AstFromNode(lang_stat, n->r, scp);
        ret->ret.tp = DescendNode(lang_stat, n->r, scp);
      }
      InsertDeferd(&ret, scp, true, false);
    } break;
    default:
      ASSERT(0);
    }
  } break;
  case node_type::N_STR_LIT: {
    ret->type = AST_STR_LIT;
    ret->str = own_std::string(n->t->str);
  } break;
  case node_type::N_SCOPE: {
    ret->type = AST_STATS;
    ret->line_number = n->r->t->line;
    ast_rep *rhs = AstFromNode(lang_stat, n->r, n->scp);
    if (rhs->type == AST_STATS)
      INSERT_VEC(ret->stats, rhs->stats);
    else
      ret->stats.emplace_back(rhs);
    InsertDeferd(&ret, n->scp, false, false);
    /*
    if (n->r && n->r->type != N_STMNT && n->r->type != N_IF)
    {
    }
    else
            ret = AstFromNode(lang_stat, n->r, n->scp);
            */
  } break;
  case node_type::N_ARRAY_CONSTRUCTION: {
    auto last = lang_stat->cur_func;
    ret->type = AST_ARRAY_COSTRUCTION;
    type2 tp_ar;
    NameFindingGetType(lang_stat, n->l, scp, tp_ar);

    ret->ar_constr.type = *tp_ar.tp;

    int tp_size = GetTypeSize(&tp_ar);
    ret->ar_constr.at_offset = lang_stat->cur_strct_constrct_size_per_statement;
    lang_stat->cur_strct_constrct_size_per_statement += tp_size;

    int cur_sz = lang_stat->cur_func->strct_constrct_size_per_statement;
    lang_stat->cur_func->strct_constrct_size_per_statement =
        max(cur_sz, lang_stat->cur_strct_constrct_size_per_statement);

    // type_struct2* strct =  ret->strct_constr.strct;
    FOR_VEC(c, *n->exprs) {
      // ast_struct_construct_info info;
      // info.var = strct->FindDecl(c->n->l->t->str);
      ast_rep *a = AstFromNode(lang_stat, c->n, scp);
      ret->ar_constr.commas.emplace_back(a);
    }
    lang_stat->cur_strct_constrct_size_per_statement -= tp_size;
  } break;
  case node_type::N_STRUCT_CONSTRUCTION: {
    auto last = lang_stat->cur_func;
    ret->type = AST_STRUCT_COSTRUCTION;
    FindIdentifier(n->l->t->str, scp, &dummy_type);
    int tp_size = 0;
    if (dummy_type.type == TYPE_STRUCT_TYPE) {
      ret->strct_constr.strct = dummy_type.strct;
    } else {
      tp_size = 16;
      ret->strct_constr.is_vector = true;
      ret->strct_constr.vec_type = dummy_type.vec_type;
      // ASSERT(0)
    }
    // tp_size = ret->strct_constr.strct->size;

    ret->strct_constr.at_offset =
        lang_stat->cur_strct_constrct_size_per_statement;

    lang_stat->cur_strct_constrct_size_per_statement += tp_size;

    int cur_sz = lang_stat->cur_func->strct_constrct_size_per_statement;
    lang_stat->cur_func->strct_constrct_size_per_statement =
        max(cur_sz, lang_stat->cur_strct_constrct_size_per_statement);

    type_struct2 *strct = ret->strct_constr.strct;
    int i = 0;
    if (dummy_type.type == TYPE_VECTOR_TYPE) {
      ASSERT(n->exprs->size() <= 4)
      while (n->exprs->size() < 4) {
        comma_ret f;
        f.n = lang_stat->zero_float;
        n->exprs->emplace_back(f);
      }
    }

    FOR_VEC(c, *n->exprs) {
      ast_struct_construct_info info;
      if (dummy_type.type == TYPE_VECTOR_TYPE) {
        type_struct2 *v;
        if(dummy_type.vec_type == 4)
        {
          v = lang_stat->_vec_strct->type.strct;
        }
        else if(dummy_type.vec_type == 8) 
        {
          v = lang_stat->_vec2_strct->type.strct;
        }
        else
        {
          ASSERT(false)
        }
        info.var = v->vars[i];
        info.exp = AstFromNode(lang_stat, c->n, scp);
      } else if (IS_FLAG_ON(strct->flags, TP_STRCT_TUPLE)) {
        info.var = strct->scp->vars[i];
        info.exp = AstFromNode(lang_stat, c->n, scp);
      } else {
        info.var = strct->FindDecl(c->n->l->t->str);
        info.exp = AstFromNode(lang_stat, c->n->r, scp);
      }
      ret->strct_constr.commas.emplace_back(info);
      i++;
    }
    // lang_stat->cur_strct_constrct_size_per_statement -= tp_size;
  } break;
  case node_type::N_OP_OVERLOAD:
  case node_type::N_FUNC_DECL: {
    ret->type = AST_FUNC;
    ASSERT(n->fdecl);
    ret->func.fdecl = n->fdecl;

    auto last = lang_stat->cur_func;
    lang_stat->cur_func = n->fdecl;

    ret->func.fdecl = n->fdecl;

    ast_rep *coroutine_prolegue = nullptr;
    if (IS_FLAG_ON(n->fdecl->flags, FUNC_DECL_COROUTINE)) {
      own_std::string str("CoroutinePrologue");
      decl2 *d = FindIdentifier(str, scp, &dummy_type);
      node *new_tree = n->fdecl->coroutine_prologue_tree;
      // BuildMacroTree(lang_stat, n->fdecl->scp, new_tree, n->t->line);

      lang_stat->flags |= PSR_FLAGS_REPORT_UNDECLARED_IDENTS;
      unit_file *prev_file = lang_stat->cur_file;
      lang_stat->cur_file = n->fdecl->from_file;
      lang_stat->flags |= PSR_FLAGS_DONT_DECLARE_VARIABLES;
      decl2 *res = DescendNameFinding(lang_stat, new_tree, scp);
      lang_stat->flags &= ~PSR_FLAGS_DONT_DECLARE_VARIABLES;
      lang_stat->cur_file = prev_file;
      lang_stat->flags &= ~PSR_FLAGS_REPORT_UNDECLARED_IDENTS;

      ASSERT(res);
      DescendNode(lang_stat, new_tree, scp);
      coroutine_prolegue = AstFromNode(lang_stat, new_tree, scp);
      if (coroutine_prolegue->type == AST_STATS) {
        FOR_VEC(cur_st_ptr, coroutine_prolegue->stats) {
          ast_rep *cur_st = *cur_st_ptr;
          cur_st->dont_make_dbg_stmnt = true;
        }
      }
      // ret->func.stats->stats.insert(ret->func.stats->stats.begin(),
      // prologue_ast->stats.begin(), prologue_ast->stats.end());
    }

    ret->func.stats = AstFromNode(lang_stat, n->r->r, n->fdecl->scp);
    if (ret->func.stats->type != AST_STATS) {
      ast_rep *new_ast = NewAst();
      new_ast->type = AST_STATS;
      new_ast->stats.emplace_back(ret->func.stats);
      ret->func.stats = new_ast;
    }
    if (IS_FLAG_ON(n->fdecl->flags, FUNC_DECL_COROUTINE)) {
      ASSERT(coroutine_prolegue);
      ret->func.stats->stats.insert(ret->func.stats->stats.begin(),
                                    coroutine_prolegue->stats.begin(),
                                    coroutine_prolegue->stats.end());
    }

    InsertDeferd(&ret->func.stats, scp, false, false);

    lang_stat->cur_func = last;

  } break;
  case node_type::N_DEFER: {
    ret = AstFromNode(lang_stat, n->r, scp);
    //BREAK(n->t->line 9032)
    if (ret->type == AST_STATS) {
      INSERT_VEC(scp->defered, ret->stats);
    } else {
      scp->defered.emplace_back(ret);
    }
    return nullptr;
  } break;
  case node_type::N_CALL: {
    ret->type = AST_CALL;

    ret->call.in_func = scp->fdecl;

    if (n->l->type != N_IDENTIFIER) {
      ret->call.lhs = AstFromNode(lang_stat, n->l, scp);
      if(ret->call.lhs->is_import)
        ret->call.indirect = false;
      else
        ret->call.indirect = true;
      ret->call.fdecl = DescendNode(lang_stat, n->l, scp).fdecl;
    } else {
      decl2 *decl = FindIdentifier(n->l->t->str, scp, &dummy_type);
      ret->call.fdecl = decl->type.fdecl;

      ret->call.fdecl->references++;

      ret->call.indirect = false;
      if (decl->type.type == TYPE_FUNC_PTR) {
        ret->call.indirect = true;
        ret->call.func_ptr_var = decl;
      }
    }

    func_decl *f = ret->call.fdecl;
    ret->lhs_tp = ret->call.fdecl->ret_type;

    if (IS_FLAG_ON(f->flags, FUNC_DECL_INTERNAL)) {
      if (f->name == "sizeof") {
        dummy_type = DescendNode(lang_stat, n->r, scp);
        ret->type = AST_INT;
        ret->num = GetTypeSize(&dummy_type);

      } else if (f->name == "GetTypeData") {
        // BREAK(n->t->line == 3360)
        dummy_type = DescendNode(lang_stat, n->r, scp);
        ret->type = AST_TYPE_DATA;
        if (dummy_type.type == TYPE_STRUCT_TYPE ||
            dummy_type.type == TYPE_STRUCT) {
          ret->decl = dummy_type.strct->this_decl;
        } else if (dummy_type.type == TYPE_ENUM_TYPE ||
                   dummy_type.type == TYPE_ENUM) {
          ret->decl =
              dummy_type.e_decl;
        } else {
          ASSERT(0)
        }
      } else if (f->name == "get_func_bc") {
        dummy_type = DescendNode(lang_stat, n->r, scp);
        ret->type = AST_GET_FUNC_BC;
        ret->call.fdecl = dummy_type.fdecl;
      } else if (f->name == "__is_struct") {
        dummy_type = DescendNode(lang_stat, n->r, scp);
        ret->type = AST_INT;
        ret->num = dummy_type.type == TYPE_STRUCT;
      } else
        ASSERT(0);
    } else if (n->r) {

      if (n->r->type == N_BINOP && n->r->t->type == T_COMMA) {
        ast_rep *args = AstFromNode(lang_stat, n->r, scp);
        ret->call.args = args->e_holder.expr;
      } else {
        ret->call.args.emplace_back(AstFromNode(lang_stat, n->r, scp));
      }
    }
    if (IS_FLAG_ON(f->flags, FUNC_DECL_VAR_ARGS)) {
      // we will create an array of the fixed args, and after the type info
      // and the value of the var args
      // inserting fixed args
      int var_arg_start_idx = f->args.size() - 1;
      own_std::vector<ast_rep *> var_arg_info;
      var_arg_info.insert(var_arg_info.begin(), ret->call.args.begin(),
                          ret->call.args.begin() + var_arg_start_idx);

      // rel_array
      // filling rel_array variambles, ptr to args, and total of varags
      ast_rep *offset_to_var_args = NewAst();
      offset_to_var_args->type = AST_INT;
      // 16 is the size of rel_array struct
      offset_to_var_args->num = 16;
      var_arg_info.emplace_back(offset_to_var_args);

      int total_var_args = ret->call.args.size() - var_arg_start_idx;
      
      ast_rep *total_var_args_ast = NewAst();
      total_var_args_ast->type = AST_INT;
      total_var_args_ast->num = total_var_args;
      var_arg_info.emplace_back(total_var_args_ast);
      //---------

      ast_rep *zero = NewAst();
      zero->type = AST_INT;
      zero->num = 0;

      for (int i = var_arg_start_idx; i < ret->call.args.size(); i++) {
        // now filling data for this struct
        /*
        var_arg:struct
        {
          type : type_enum,
          type_info : *type_data,
          ptr : u8,
          val : *void,
        }
        */
        ast_rep *cur_arg = ret->call.args[i];

        //***** type : type_enum
        ast_rep *type_int = NewAst();
        type_int->type = AST_INT;
        type_int->num = cur_arg->lhs_tp.type;
        var_arg_info.emplace_back(type_int);
        //***

        //***** type_info : *type_data
        if (cur_arg->lhs_tp.type == TYPE_STRUCT) {
          type_int = NewAst();
          type_int->type = AST_TYPE_DATA;
          type_int->decl = cur_arg->lhs_tp.strct->this_decl;
          var_arg_info.emplace_back(type_int);
        } else
          var_arg_info.emplace_back(zero);
        //*****

        //***** ptr : u8
        ast_rep *ptr = NewAst();
        ptr->type = AST_INT;
        ptr->num = cur_arg->lhs_tp.ptr;
        var_arg_info.emplace_back(ptr);
        //***** 

        // casting to the highest type so that we zero the
        // rest of the register when generating the code

        //***** val : *void
        ast_rep *arg = cur_arg;
        if (cur_arg->lhs_tp.type == TYPE_STRUCT) {
          ast_rep *ref = NewAst();
          ref->type = AST_ADDRESS_OF;
          ref->ast = cur_arg;
          arg = ref;
        } else if (cur_arg->lhs_tp.type == TYPE_F32) {

          ast_rep *ref = NewAst();
          ref->type = AST_ADDRESS_OF;
          ref->ast = cur_arg;

          ast_rep *cast = NewAst();
          cast->type = ast_type::AST_CAST;
          cast->cast.type.type = TYPE_S32;
          cast->cast.type.ptr = 1;
          cast->cast.casted = ref;

          ast_rep *deref = NewAst();
          deref->type = ast_type::AST_DEREF;
          deref->deref.times = 1;
          deref->deref.exp = cast;
          deref->deref.type = cast->cast.type;

          arg = deref;
        }

        ast_rep *cast_to_s64 = NewAst();
        cast_to_s64->type = AST_CAST;
        cast_to_s64->cast.casted = arg;
        cast_to_s64->cast.type.type = TYPE_S64;
        var_arg_info.emplace_back(cast_to_s64);
        auto a = 0;
      }
      ret->call.args = var_arg_info;
      // ret->call.args.erase()
    }
  } break;
  case node_type::N_FLOAT64: {
    ret->type = AST_F64;
    ret->f64 = n->t->f64;

  } break;
  case node_type::N_FLOAT: {
    ret->type = AST_FLOAT;
    ret->f32 = n->t->f;

  } break;
  case node_type::N_STMNT: {
    lang_stat->cur_strct_constrct_size_per_statement = 0;
    ret->type = AST_STATS;
    ret->line_number = n->t->line;
    own_std::vector<node *> node_stack;
    node *cur_node = n;
    node_stack.emplace_back(cur_node);
    while (cur_node->l->type == node_type::N_STMNT) {
      node_stack.emplace_back(cur_node->l);
      cur_node = cur_node->l;
    }
    int size = node_stack.size();

    ast_rep *lhs = AstFromNode(lang_stat, cur_node->l, scp);
    if (IS_FLAG_ON(cur_node->l->flags, NODE_FLAGS_STMNT_WITHOUT_SEMICOLON))
      lhs->stmnt_without_semicolon = true;

    if (lhs->type == AST_STATS)
      INSERT_VEC(ret->stats, lhs->stats);
    else
      ret->stats.emplace_back(lhs);

    for (int i = node_stack.size() - 1; i >= 0; i--) {
      node *s = *(node_stack.begin() + i);
      if (!s->r)
        continue;
      ast_rep *rhs = AstFromNode(lang_stat, s->r, scp);
      if (!rhs)
        continue;

      if (IS_FLAG_ON(s->r->flags, NODE_FLAGS_STMNT_WITHOUT_SEMICOLON))
        rhs->stmnt_without_semicolon = true;

      if (rhs->type == AST_STATS)
        INSERT_VEC(ret->stats, rhs->stats);
      else
        ret->stats.emplace_back(rhs);

      /*
      if (s->l->type != N_STMNT)
              ret->stats.emplace_back(AstFromNode(lang_stat, s->l, scp));
      if(s->r->type != N_STMNT)
              ret->stats.emplace_back(AstFromNode(lang_stat, s->r, scp));
      */
    }
  } break;
  case N_FOR: {
    ret->type = AST_FOR;
    node *for_nd = n;
    scp = n->r->scp;
    bool is_rev = false;
    if (n->l->type == N_KEYWORD && n->l->kw == KW_REV) {
      is_rev = true;
      n = n->l->r;
    } else
      n = n->l;
    if (IsNodeOperator(n, T_IN)) {
      node *var_name_nd = n->l;
      ast_rep *var_name = AstFromNode(lang_stat, var_name_nd, scp);
      ast_rep *var_name_og = var_name;
      if (IsNodeOperator(n->r, T_TWO_POINTS)) {
        type2 var_type = DescendNode(lang_stat, var_name_nd, scp);

        node *start_val_nd = n->r->l;
        node *end_val_nd = n->r->r;
        ast_rep *start_val = AstFromNode(lang_stat, start_val_nd, scp);
        ast_rep *end_val = AstFromNode(lang_stat, end_val_nd, scp);

        tkn_type2 cmp_op;

        if (var_type.ptr > 0) {
          ast_rep *address = NewAst();
          address->type = AST_ADDRESS_OF;
          address->ast = var_name;
          address->line_number = n->t->line;

          ast_rep *cast_to_ptr_u64 = NewAst();
          cast_to_ptr_u64->type = AST_CAST;
          cast_to_ptr_u64->cast.casted = address;
          cast_to_ptr_u64->cast.type.type = TYPE_U64;
          cast_to_ptr_u64->cast.type.ptr = 1;
          cast_to_ptr_u64->line_number = n->t->line;
          //var_name = cast_to_ptr_u64;

          cast_to_ptr_u64 = NewAst();
          cast_to_ptr_u64->type = AST_CAST;
          cast_to_ptr_u64->cast.casted = start_val;
          cast_to_ptr_u64->cast.type.type = TYPE_U64;
          cast_to_ptr_u64->cast.type.ptr = 1;
          cast_to_ptr_u64->line_number = n->t->line;
          start_val = cast_to_ptr_u64;

          cast_to_ptr_u64 = NewAst();
          cast_to_ptr_u64->type = AST_CAST;
          cast_to_ptr_u64->cast.casted = end_val;
          cast_to_ptr_u64->cast.type.type = TYPE_U64;
          cast_to_ptr_u64->cast.type.ptr = 1;
          cast_to_ptr_u64->line_number = n->t->line;
          end_val = cast_to_ptr_u64;

          // cast(*u64)&var_name = cast(*64)(cast(u64)var_name + sizeof(type))
          ast_rep *cast_to_u64 = NewAst();
          cast_to_u64->type = AST_CAST;
          cast_to_u64->cast.casted = var_name_og;
          cast_to_u64->cast.type.type = TYPE_U64;
          cast_to_u64->cast.type.ptr = 0;

          ast_rep *type_size = NewAst();
          type_size->type = AST_INT;
          var_type.ptr--;
          type_size->num = GetTypeSize(&var_type);
          var_type.ptr++;

          ast_rep *bin_plus = NewAst();
          bin_plus->type = AST_BINOP;
          bin_plus->op = T_PLUS;
          bin_plus->lhs_tp = var_type;
          bin_plus->e_holder.expr.emplace_back(cast_to_u64);
          bin_plus->e_holder.expr.emplace_back(type_size);

          if (is_rev)
            bin_plus->op = T_MINUS;

          ast_rep *equal = NewAst();
          equal->type = AST_BINOP;
          equal->op = T_EQUAL;
          equal->lhs_tp = var_type;
          equal->e_holder.expr.emplace_back(var_name);
          equal->e_holder.expr.emplace_back(bin_plus);
          ret->for_info.at_loop_end_stat = equal;
          equal->line_number = n->t->line;

        } else {
          ast_rep *inc_val = NewAst();

          if (is_rev) {
            inc_val->type = AST_MINUS_MINUS;
          } else {
            inc_val->type = AST_PLUS_PLUS;
          }
          inc_val->unop_assign.ast = var_name;
          inc_val->unop_assign.tp = var_type;
          ret->for_info.at_loop_end_stat = inc_val;
        }

        if (is_rev) {
          cmp_op = T_GREATER_THAN;
        } else {
          cmp_op = T_LESSER_THAN;
        }
        ret->for_info.start_stat = CreateAstBin(lang_stat, T_EQUAL, var_name,
                                                start_val, start_val_nd, scp);

        ret->for_info.cond_stat = CreateAstBin(lang_stat, cmp_op, var_name_og,
                                               end_val, end_val_nd, scp);

      } else {
        ASSERT(0)
      }

    } else {
      ASSERT(0)
    }
    ret->for_info.scope = AstFromNode(lang_stat, for_nd->r, scp);
  } break;
  case N_WHEN_USED:
  case N_HASHTAG: {

  } break;
  case N_APOSTROPHE: {
    ret->type = AST_CHAR;
    ret->num = n->t->i;
  } break;
  case N_EMPTY:
    break;
  default:
    ASSERT(0);
  }
  return ret;
}

void GetIRVal(lang_state *lang_stat, ast_rep *ast, ir_val *val) {
  val->kind = IR_VAL_VALUE;
  switch (ast->type) {
  case AST_STRUCT_COSTRUCTION: {
    val->type = IR_TYPE_ON_STACK;
    val->stack.i = ast->strct_constr.at_offset;
    // val->is_unsigned = IsUnsigned(ast->decl->type.type);
  } break;
  case AST_REG: {
    *val = ast->reg;
  }break;
  case AST_IDENT: {
    val->type = IR_TYPE_DECL;
    val->decl = ast->decl;
    val->ptr = ast->decl->type.ptr;

    val->reg_sz = GetTypeSize(&ast->decl->type, 0);

    if (val->reg_sz <= 0)
      val->reg_sz = 8;

    val->reg_sz = min(val->reg_sz, 8);

    if(ast->decl->type.type == TYPE_VOID && ast->decl->type.ptr > 0)
      val->reg_sz = 8;

    val->is_unsigned = false;
    val->deref = 0;
    type2 *tp = &ast->decl->type;
    if (ast->decl->type.type == TYPE_STATIC_ARRAY) {
      // val->is_unsigned = IsUnsigned(ast->decl->type.tp->type);
      val->is_unsigned = true;
      val->reg_sz = 8;
      tp = ast->decl->type.tp;
      val->reg_sz = GetTypeSize(tp);
      if(val->reg_sz > 8)
      {
        val->reg_sz = 8;
      }
    } else if (ast->decl->type.type == TYPE_STRUCT_TYPE &&
               IS_FLAG_ON(ast->decl->type.strct->flags, TP_STRCT_ETRUCT)) {
      val->is_unsigned = true;
    } else if (ast->decl->type.type != TYPE_STRUCT)
      val->is_unsigned = IsUnsigned(ast->decl->type.type);
    else if (ast->decl->type.type == TYPE_STRUCT &&
             IS_FLAG_ON(ast->decl->type.strct->flags, TP_STRCT_ETRUCT)) {
      val->reg_sz = GetTypeSize(&ast->decl->type.strct->vars[0]->type);
    }
    // if (ast->decl->type.ptr > 0)
    // val->is_unsigned = true;
    val->is_float = tp->IsFloat();
    val->is_packed_float = tp->type == TYPE_VECTOR;
    if(val->is_packed_float)
    {
      val->reg_sz = tp->vec_type;
    }
    val->is_float = val->is_float || val->is_packed_float;
    val->ptr = ast->decl->type.ptr;
    //val->deref += ast->decl->type.ptr;
    val->kind = IR_VAL_ADDR;
    val->reg = (char)regs_enum::RSP;
    val->voffset = 0;

  } break;
  case AST_F64: {
    val->type = IR_TYPE_F64;
    val->f64 = ast->f64;
    val->reg_sz = 8;
    val->is_unsigned = false;
    val->is_float = true;
  }break;
  case AST_FLOAT: {
    val->type = IR_TYPE_F32;
    val->f32 = ast->f32;
    val->reg_sz = 4;
    val->is_unsigned = false;
    val->is_float = true;
  } break;
  case AST_TYPE_DATA:
  {
    val->type = IR_TYPE_TYPE_DATA;
    val->decl = ast->decl;
    val->is_unsigned = ast->num < 0 ? false : true;
    val->reg_sz = 8;
  }break;
  case AST_INT64: {
    val->type = IR_TYPE_INT64;
    val->val = ast->num;
    val->is_unsigned = ast->num < 0 ? false : true;
    val->is_float = false;
    val->is_packed_float = false;
    val->reg_sz = 8;
  } break;
  case AST_CHAR:
  case AST_INT: {
    val->type = IR_TYPE_INT;
    val->val = ast->num;
    val->is_unsigned = ast->num < 0 ? false : true;
    val->is_float = false;
    val->is_packed_float = false;
    val->reg_sz = 4;
  } break;
  default:
    ASSERT(0)
  }
}

void GenIRBinOp(lang_state *lang_stat, ast_rep *ast, ir_val *lhs, ir_val *rhs,
                own_std::vector<ir_rep> *out) {
  switch (ast->op) {
  case T_PLUS: {

  } break;
  default:
    ASSERT(0)
  }
}
bool IsAstSimple(lang_state *lang_stat, ast_rep *ast) {
  if (ast->type == AST_INT || ast->type == AST_IDENT)
    return true;
  return false;
}

void IRCreateEndBlock(lang_state *lang_stat, int begin_sub_if_idx,
                      own_std::vector<ir_rep> *out, ir_type type) {
  int end_idx = out->size();

  ir_rep ir;
  ir.type = type;
  ir.block.other_idx = begin_sub_if_idx;

  out->emplace_back(ir);
  //ir_rep *begin = &(*out)[begin_sub_if_idx];

  //begin->block.other_idx = out->size();
  switch (type) {
  case IR_BEGIN_STMNT: {
    ASSERT(!lang_stat->in_ir_stmnt);
    lang_stat->in_ir_stmnt = true;
  } break;
  case IR_END_STMNT: {
    //ASSERT(begin->type == IR_BEGIN_STMNT);
    lang_stat->in_ir_stmnt = false;
  } break;
  }
}
int IRCreateBeginBlock(lang_state *lang_stat, own_std::vector<ir_rep> *out,
                       ir_type type, void *data = nullptr) {
  ir_rep ir;
  ir.type = type;
  int ret = out->size();

  out->emplace_back(ir);
  if (type == IR_BEGIN_STMNT) {
    auto l = (int)(long long)data;
    out->back().block.stmnt.line = l;
    // ir.type = IR_NOP;
    // out->emplace_back(ir);
  }

  return ret;
}

ir_type FromTokenOpToIRType(tkn_type2 op) {
  switch (op) {
  case T_COND_EQ:
    return IR_CMP_EQ;
  case T_GREATER_EQ:
    return IR_CMP_GE;
  case T_GREATER_THAN:
    return IR_CMP_GT;
  case T_LESSER_THAN:
    return IR_CMP_LT;
  case T_LESSER_EQ:
    return IR_CMP_LE;
  case T_COND_NE:
    return IR_CMP_NE;
  default:
    ASSERT(0)
  }
  return IR_CMP_NE;
}
#define REG_USED_FLAG 0x100
#define REG_SPILLED 0x200
void FreeReg(lang_state *lang_stat, char reg_idx, bool is_float) {
  if(is_float)
    lang_stat->float_regs[reg_idx] &= ~REG_USED_FLAG;
  else
    lang_stat->regs[reg_idx] &= ~REG_USED_FLAG;
}

void FreeAllFloatRegs(lang_state *lang_stat) {
  memset(lang_stat->float_regs, 0, sizeof(lang_stat->float_regs));
  // lang_stat->float_regs[reg_idx] &= ~REG_FREE_FLAG;
}
bool IsRegInUseFloat(lang_state *lang_stat, char idx) {
  // ASSERT(IS_FLAG_ON(lang_stat->float_regs[idx], REG_FREE_FLAG));
  return IS_FLAG_ON(lang_stat->float_regs[idx], REG_USED_FLAG);
}
bool IsRegInUse(lang_state *lang_stat, char idx, bool is_float = false) {
  // ASSERT(IS_FLAG_ON(lang_stat->float_regs[idx], REG_FREE_FLAG));
  if(is_float)
  {
    return IS_FLAG_ON(lang_stat->float_regs[idx], REG_USED_FLAG);
  }
  else
    return IS_FLAG_ON(lang_stat->regs[idx], REG_USED_FLAG);
}
void FreeSpecificFloatReg(lang_state *lang_stat, char idx) {
  // ASSERT(IS_FLAG_ON(lang_stat->float_regs[idx], REG_FREE_FLAG));
  lang_stat->float_regs[idx] &= ~REG_USED_FLAG;
}
void AllocSpecificFloatReg(lang_state *lang_stat, char idx) {
  // ASSERT(IS_FLAG_OFF(lang_stat->float_regs[idx], REG_FREE_FLAG));
  lang_stat->float_regs[idx] |= REG_USED_FLAG;
}
char AllocFloatReg(lang_state *lang_stat, int start = 8) {
  for (int i = start; i < 16; i++) {
    if (IS_FLAG_OFF(lang_stat->float_regs[i], REG_USED_FLAG)) {
      lang_stat->float_regs[i] |= REG_USED_FLAG;
      return i;
    }
  }
  ASSERT(0);
  return -1;
}

void FreeAllRegs(lang_state *lang_stat) {
  memset(lang_stat->regs, 0, sizeof(lang_stat->regs));
  // lang_stat->regs[reg_idx] &= ~REG_FREE_FLAG;
}
void FreeSpecificReg(lang_state *lang_stat, char idx) {
  // ASSERT(IS_FLAG_ON(lang_stat->regs[idx], REG_FREE_FLAG));
  lang_stat->regs[idx] &= ~REG_USED_FLAG;
}
void AllocSpecificReg(lang_state *lang_stat, char idx, bool is_float = false) {
  if(is_float)
  {
    lang_stat->float_regs[idx] |= REG_USED_FLAG;
  }
  else
  {
    lang_stat->regs[idx] |= REG_USED_FLAG;
  }
  // ASSERT(IS_FLAG_OFF(lang_stat->regs[idx], REG_FREE_FLAG));
  if (lang_stat->track_alloc_regs) {
    lang_stat->tracked_regs.emplace_back(idx);
  }
}
char AllocReg2(lang_state *lang_stat, bool on_func_call, int start = 0) {
  if(on_func_call)
  {
    if (IS_FLAG_OFF(lang_stat->regs[0], REG_USED_FLAG)) {
      // lang_stat->regs[i] |= REG_FREE_FLAG;
      AllocSpecificReg(lang_stat, 0);
      return 0;
    }
  }
  else
  {
    for (int i = start; i < 9; i++) {
      if (i == PRE_X64_RSP_REG || i == AUX_DECL_REG)
        continue;
      if (IS_FLAG_OFF(lang_stat->regs[i], REG_USED_FLAG)) {
        // lang_stat->regs[i] |= REG_FREE_FLAG;
        AllocSpecificReg(lang_stat, i);
        return i;
      }
    }
  }
  ASSERT(0);
}
char AllocReg(lang_state *lang_stat, int start = 0) {
  for (int i = start; i < 9; i++) {
    if (i == PRE_X64_RSP_REG || i == AUX_DECL_REG)
      continue;
    if (IS_FLAG_OFF(lang_stat->regs[i], REG_USED_FLAG)) {
      // lang_stat->regs[i] |= REG_FREE_FLAG;
      AllocSpecificReg(lang_stat, i);
      return i;
    }
  }
  ASSERT(0);
}
bool HasCall(lang_state *lang_stat, ast_rep *ast) {
  switch (ast->type) {
  case AST_INT: 
  case AST_F64: 
  case AST_STR_LIT: 
  {
    return false;
  } break;
  case AST_EMPTY: {
    return false;
  } break;
  case AST_CALL: {
    return true;
  } break;
  case AST_STRUCT_COSTRUCTION:
  {
    bool has_call= false;
    FOR_VEC(c, ast->strct_constr.commas)
    {
      has_call = HasCall(lang_stat, c->exp);
      if(has_call) return true;
    }
    return false;
  }break;
  case AST_ARRAY_COSTRUCTION:
  {
    bool has_call= false;
    FOR_VEC(c, ast->ar_constr.commas)
    {
      has_call = HasCall(lang_stat, *c);
      if(has_call) return true;
    }
    return false;
  }break;
  case AST_IDENT:
    return false;
  case AST_INDEX: {
    bool is_static_ar = ast->index.lhs->type == AST_IDENT &&
                        ast->index.lhs->decl->type.type == TYPE_STATIC_ARRAY;
    if (ast->index.lhs_type.type != TYPE_STATIC_ARRAY) {
      return true;
    }
    return false;
  } break;
  case AST_FLOAT: {
    return false;
  } break;
  case AST_DEREF: {
    return HasCall(lang_stat, ast->deref.exp);
  } break;
  case AST_ADDRESS_OF: {
    return HasCall(lang_stat, ast->ast);
  } break;
  case AST_CAST: {
    return HasCall(lang_stat, ast->cast.casted);
  } break;
  case AST_BINOP: {
    if (ast->op == T_POINT) {
      if (HasCall(lang_stat, ast->e_holder.expr[0])) {
        return true;
      }
    } else {
      FOR_VEC(a, ast->e_holder.expr) {
        if (HasCall(lang_stat, *a)) {
          return true;
        }
      }
    }
    return false;
  } break;
  default:
    ASSERT(0);
  }
}

void MaybeUnspillRegisters(lang_state *lang_stat, int *had_spilled_reg,
                           own_std::vector<ir_rep> *out) {
  ir_rep ir;
  if (*had_spilled_reg != -1) {
    ir = {};
    ir.type = IR_ASSIGNMENT;
    ir.assign.to_assign.type = IR_TYPE_REG;
    ir.assign.to_assign.reg_sz = 8;
    ir.assign.to_assign.deref = -1;
    ir.assign.to_assign.reg = *had_spilled_reg;
    ir.assign.only_lhs = true;
    ir.assign.lhs.type = IR_TYPE_ON_STACK;
    ir.assign.lhs.deref = 0;
    ir.assign.lhs.reg_sz = 8;
    ir.assign.lhs.stack.on_stack_type = ON_STACK_SPILL;
    out->emplace_back(ir);
  }
}
int SpillRegisters(lang_state *lang_stat, int spilled_reg,
                   own_std::vector<ir_rep> *out) {
  ir_rep ir;
  int spill_offset = 0;
  ir = {};
  ir.type = IR_ASSIGNMENT;
  ir.assign.to_assign.type = IR_TYPE_ON_STACK;
  ir.assign.to_assign.reg_sz = 8;
  ir.assign.to_assign.deref = -1;
  ir.assign.to_assign.stack.on_stack_type = ON_STACK_SPILL;
  ir.assign.to_assign.stack.i = spill_offset;
  ir.assign.only_lhs = true;
  ir.assign.lhs.type = IR_TYPE_REG;
  ir.assign.lhs.reg_sz = 8;
  ir.assign.lhs.reg = spilled_reg;
  ir.assign.lhs.deref = -1;
  out->emplace_back(ir);
  return spill_offset;
  // we should only have one reg used
}
bool IsComparisonOp(tkn_type2 op) {
    switch (op) {
        case T_COND_EQ:
        case T_COND_NE:
        case T_LESSER_THAN:
        case T_LESSER_EQ:
        case T_GREATER_THAN:
        case T_GREATER_EQ:
            return true;
        default:
            return false;
    }
}
void InsertIr(thread_ir_state *state, ir_rep &ir)
{
  if(ir.type == IR_BIN)
  {
    ASSERT(ir.bin.lhs.reg_sz <= 8 && ir.bin.rhs.reg_sz <= 8)
  }
  state->cur_block->irs.emplace_back(ir);
}
/*
void EmitJmp(lang_state *lang_stat, block2 *dst, block2 * to)
{
  ir_rep ir;
  ir.type = IR_JMP;
  ir.i = to.id;
  dst->irs.emplace_back(ir);
}
*/
void EmitJmp(lang_state *lang_stat, thread_ir_state *state, block2 * block)
{
  ir_rep ir;
  ir.type = IR_JMP;
  ir.i = block->id;
  InsertIr(state, ir);
}
void EmitBlock(lang_state *lang_stat, thread_ir_state *state, block2 * block)
{
  state->cur_func->blocks.emplace_back(block);
  block->emitted = true;
}
void EmitBlockMakeCurrent(lang_state *lang_stat, thread_ir_state *state, block2 * block)
{
  EmitBlock(lang_stat, state, block);
  state->cur_block = block;
}
block2 *CreateBlock(lang_state *lang_stat, thread_ir_state *state)
{
  block2 *ret = &((block2 *)state->blocks_ptr)[state->blocks_cur];

  ret->id = state->blocks_cur;

  state->blocks_cur++;
  ASSERT(state->blocks_cur < state->blocks_max)
  ret->irs.clear();
  ret->jmp_rels.clear();
  ret->generated = false;
  ret->emitted = false;
  return ret;
}
char GetAvailableReg(lang_state *lang_stat)
{
  if (IS_FLAG_OFF(lang_stat->regs[0], REG_USED_FLAG)) 
  {
    lang_stat->regs[0] |= REG_USED_FLAG;
    return 0;
  }
  else if (IS_FLAG_OFF(lang_stat->regs[3], REG_USED_FLAG)) 
  {
    lang_stat->regs[3] |= REG_USED_FLAG;
    return 3;
  }
  else if (IS_FLAG_OFF(lang_stat->regs[14], REG_USED_FLAG)) 
  {
    lang_stat->regs[14] |= REG_USED_FLAG;
    return 14;
  }
  else if (IS_FLAG_OFF(lang_stat->regs[15], REG_USED_FLAG)) 
  {
    lang_stat->regs[15] |= REG_USED_FLAG;
    return 15;
  }
  ASSERT(false)
}
void MakeIrLoadToReg(lang_state *lang_stat, char reg_dst, char reg_sz, ir_val *lhs, ir_rep *out, bool is_float)
{

  if(lhs->type == IR_TYPE_DECL && IS_FLAG_ON(lhs->decl->flags, DECL_IS_GLOBAL))
  {
    out->type = IR_GET_GLOBAL;
    out->bin.lhs.type = IR_TYPE_REG;
    out->bin.lhs.reg = reg_dst;
    out->bin.lhs.reg_sz = reg_sz;
    out->bin.lhs.ptr = 0;
    out->bin.rhs = *lhs;
  }
  else
  {
    if(lhs->ptr > 0) reg_sz = 8;

    out->type = IR_BIN;
    out->bin.op = T_EQUAL;
    out->bin.lhs.type = IR_TYPE_REG;
    out->bin.lhs.reg = reg_dst;
    out->bin.lhs.reg_sz = reg_sz;
    out->bin.lhs.is_float = is_float;
    out->bin.lhs.is_packed_float = is_float && lhs->is_packed_float;
    out->bin.rhs = *lhs;
    out->bin.rhs.voffset = lhs->voffset;
  }
}
char LoadDerefs(lang_state *lang_stat, own_std::vector<ir_rep> *out, ir_val *rhs)
{
  ir_rep ir={};
  char deref = rhs->deref;
  char reg = -1;
  auto prev_type = rhs->type;
  if(rhs->type == IR_TYPE_REG_MEM)
  {
    reg = rhs->reg;
  }
  else if (rhs->type == IR_TYPE_REG)
  {
    rhs->type = IR_TYPE_REG_MEM;
  }

  bool prev_float = rhs->is_float;
  bool prev_packed = rhs->is_packed_float;
  char prev_sz = rhs->reg_sz;
  while(deref > 0)
  {
    if(reg == -1) reg = GetAvailableReg(lang_stat);
    ir.type = IR_BIN;
    ir.bin.op = T_EQUAL;
    ir.bin.lhs.type = IR_TYPE_REG;
    ir.bin.lhs.reg_sz = 8;
    if(deref == 1)
    {
      ir.bin.lhs.is_float = prev_float;
      ir.bin.lhs.is_packed_float = prev_packed;
      ir.bin.lhs.reg_sz = prev_sz;
      if(reg != -1 && prev_float)
      {
        FreeSpecificReg(lang_stat, reg);
        reg = AllocFloatReg(lang_stat);
      }
    }
    else
    {
      ir.bin.lhs.is_float = false;
      ir.bin.lhs.is_packed_float = false;
    }
    ir.bin.lhs.reg = reg;
    ir.bin.rhs = *rhs;
    ir.bin.rhs.voffset = rhs->voffset;
    out->emplace_back(ir);
    *rhs = ir.bin.lhs;
    rhs->kind = IR_VAL_VALUE;
    rhs->reg = reg;
    rhs->voffset = 0;
    deref--;
  }
  //rhs->is_float = prev_float;
  //rhs->is_packed_float = prev_packed;
  if(prev_type == IR_TYPE_REG)
    rhs->type = prev_type;
  return reg;
  /*
  return;

  if(rhs->type == IR_TYPE_INT)
  {
    ir.type = IR_BIN;
    ir.bin.op = T_EQUAL;
    ir.bin.lhs.type = IR_TYPE_REG;
    ir.bin.lhs.reg = GetAvailableReg(lang_stat);
    ir.bin.lhs.reg_sz = 4;
    ir.bin.rhs = *rhs;

    *rhs = ir.bin.lhs;

    out->emplace_back(ir);

    return;
  }
  char reg = 0;
  bool was_float = rhs->is_float;
  char float_reg;
  bool reg_was_allocated = false;
  if(rhs->type == IR_TYPE_REG_MEM || rhs->type == IR_TYPE_REG)
    reg = rhs->reg;
  else
  {
    reg_was_allocated = true;
    reg = GetAvailableReg(lang_stat);
    //rhs->is_float = false;
  }

  char final_reg = reg;
  if(rhs->type == IR_TYPE_DECL)
  {
    final_reg = (char)regs_enum::RSP;
  }

  if(rhs->type == IR_TYPE_DECL && rhs->decl->type.type == TYPE_STATIC_ARRAY && derefs == 0)
  {
    ir.type = IR_ADDRESS_OF;
    if(!reg_was_allocated)
    {
      reg = GetAvailableReg(lang_stat);
    }
    ir.bin.lhs.type = IR_TYPE_REG;
    ir.bin.lhs.reg = reg;
    ir.bin.lhs.reg_sz = 8;
    ir.bin.rhs = *rhs;
    out->emplace_back(ir);
    final_reg = reg;
  }
  
  while(ptr > 0)
  {
    char sz = 8;

    bool is_float = false;
    if(ptr == 1 && derefs_equal){
      is_float = was_float;
      if(is_float){
        if(reg_was_allocated)
        {
          FreeReg(lang_stat, reg, false);
        }
        reg = float_reg;
      } 
      sz = rhs->reg_sz;
      if(sz > 8) sz = 8;

    } 

    MakeIrLoadToReg(lang_stat, reg, sz, rhs, &ir, is_float);

    bool cond = ptr == 1 && derefs_equal;
    if(cond)
    {
      sz = rhs->reg_sz;
      rhs->type = IR_TYPE_REG;
    }
    else
    {
      rhs->type = IR_TYPE_REG_MEM;
    }
    rhs->reg = reg;
    out->emplace_back(ir);

    rhs->voffset = 0;

    final_reg = reg;
    ptr--;
  }


  if(derefs != max_derefs)
  {
    if(rhs->type == IR_TYPE_DECL)
    {
      //rhs->voffset = rhs->decl->offset;
      //rhs->type = IR_TYPE_REG_MEM;
    }
  }
  else
  {
    rhs->type = IR_TYPE_REG;
    rhs->reg = final_reg;
  }
  rhs->kind = IR_VAL_VALUE;
  rhs->deref = 0;
  */

}

void EnsureValue(lang_state *lang_stat, thread_ir_state *state, ir_val *aux)
{
  //BREAK(ast->line_number == 764)
  ir_rep ir={};
  if(aux->kind == IR_VAL_ADDR)
  {
    char reg = -1;
    if(aux->type == IR_TYPE_REG_MEM && aux->reg == 4)
    {
      reg = GetAvailableReg(lang_stat);
    }
    else if(aux->type == IR_TYPE_REG || aux->type == IR_TYPE_REG_MEM)
    {
      aux->type = IR_TYPE_REG_MEM;
      if(aux->is_float)
      {
        reg = AllocFloatReg(lang_stat);
      }
      else
        reg = aux->reg;
    }
    else
    {
      if(aux->is_float && aux->deref == 0)
        reg = AllocFloatReg(lang_stat);
      else
        reg = GetAvailableReg(lang_stat);
    }
    
    if(aux->type == IR_TYPE_DECL && (aux->decl->type.type == TYPE_FUNC))
    {
      ir.type = IR_GET_FUNC_ADDR;
      ir.bin.lhs.type = IR_TYPE_REG;
      ir.bin.lhs.reg_sz = 8;
      ir.bin.lhs.reg = reg;
      ir.bin.rhs.decl = aux->decl;
    }
    else if(aux->type == IR_TYPE_DECL && aux->decl->type.type == TYPE_STATIC_ARRAY)
    {
      ir.type = IR_ADDRESS_OF;
      ir.bin.lhs.type = IR_TYPE_REG;
      ir.bin.lhs.reg_sz = 8;
      ir.bin.lhs.reg = reg;
      ir.bin.rhs.type = IR_TYPE_DECL;
      ir.bin.rhs.decl = aux->decl;
    }
    else
    {
      char prev_sz = aux->reg_sz;
      char prev_float = aux->is_float;
      char prev_packed = aux->is_packed_float;

      if(aux->deref > 0)
      {
        aux->reg_sz = 8;
        aux->is_packed_float = false;
        aux->is_float = false;
      }

      MakeIrLoadToReg(lang_stat, reg, aux->reg_sz, aux, &ir, aux->is_float);

      aux->reg_sz = prev_sz;
      aux->is_float = prev_float;
      aux->is_packed_float = prev_packed;
    }
    InsertIr(state, ir);
    aux->kind = IR_VAL_VALUE;
    aux->type = IR_TYPE_REG;
    aux->reg = reg;
    aux->voffset = 0;
  }
}
void GetIRComparison(lang_state *lang_stat, ast_rep *ast, thread_ir_state *state, tkn_type2 op, block2 *dst) 
{
  char reg = -1;
  ir_rep ir;
  ir.type = IR_CMP;
  ir.bin.op = op;
  //BREAK(ast->line_number == 612)
  ir.bin.rhs = GetIRFromAst2(lang_stat, ast->e_holder.expr[1], state, false);
  ir.bin.block_id = dst->id;

  ir.bin.lhs = GetIRFromAst2(lang_stat, ast->e_holder.expr[0], state, true);
  EnsureValue(lang_stat, state, &ir.bin.lhs);
  LoadDerefs(lang_stat, &state->cur_block->irs, &ir.bin.lhs);
  if(ir.bin.lhs.kind == IR_VAL_ADDR)
  {
  }
  if(ir.bin.lhs.ptr > 0)
    ir.bin.lhs.reg_sz = 8;

  InsertIr(state, ir);

  if(reg != -1)
  {
    FreeReg(lang_stat, reg, ir.bin.lhs.is_float);
  }
}

void FreeSomeRegs(lang_state *lang_stat)
{
  lang_stat->regs[0] = 0;
  lang_stat->regs[4] = 0;
  lang_stat->regs[14] = 0;
  lang_stat->regs[15] = 0;
  for(int i = 8; i < 16;i++)
  {
    lang_stat->float_regs[i] = 0;
  }
}
void GetIRCond2(lang_state *lang_stat, ast_rep *ast, thread_ir_state *state, block2 *cond_true, block2 *cond_false, bool flip_comparison = true) 
{
  ASSERT(cond_true && cond_false)
  //BREAK(ast->line_number == 541)
  if(ast->type == AST_OPPOSITE)
  {
    bool aux_flip = false;
    if(ast->ast->type == AST_BINOP && ast->ast->op == T_POINT || ast->ast->type == AST_IDENT)
    {
      //aux_flip = true;
      auto prev = cond_false;
      cond_false = cond_true;
      cond_true = prev;
    }
    GetIRCond2(lang_stat, ast->ast, state, cond_false, cond_true, aux_flip);
  }
  else if(ast->type == AST_INT)
  {
    if(ast->num == 0)
    {
      EmitJmp(lang_stat, state, cond_false);
    }
    else if(ast->num == 1)
    {

    }
    else
    {
      ASSERT(false)
    }
  }
  else if(ast->type == AST_BINOP && ast->op != T_POINT)
  {
    if(IsComparisonOp(ast->op))
    {
      tkn_type2 final_op = ast->op;
      if(flip_comparison)
      {
        final_op = OppositeCondCmp(final_op);
        GetIRComparison(lang_stat, ast, state, final_op, cond_false);
      }
      else
      {
        GetIRComparison(lang_stat, ast, state, final_op, cond_true);
      }
    }
    else if(ast->op == T_COND_AND)
    {
      int i = 0;
      FOR_VEC(expr, ast->e_holder.expr) 
      {
        ast_rep *e = *expr;
        if((i + 1) == ast->e_holder.expr.size())
        {
          GetIRCond2(lang_stat, e, state, cond_true, cond_false, flip_comparison);
        }
        else
        {
          block2 *mid = CreateBlock(lang_stat, state);
          GetIRCond2(lang_stat, e, state, mid, cond_false, true);
          EmitBlock(lang_stat, state, mid);

          state->cur_block = mid;
        }
        i++;
        FreeSomeRegs(lang_stat);
      }
    }
    else if(ast->op == T_COND_OR)
    {
      int i = 0;
      FOR_VEC(expr, ast->e_holder.expr) 
      {
        ast_rep *e = *expr;
        if((i + 1) == ast->e_holder.expr.size())
        {
          GetIRCond2(lang_stat, e, state, cond_true, cond_false, flip_comparison);
        }
        else
        {
          bool aux_flip = true;
          if(IsComparisonOp(e->op))
            aux_flip = false;
          block2 *mid = CreateBlock(lang_stat, state);
          if(flip_comparison)
            aux_flip = !aux_flip;
          GetIRCond2(lang_stat, e, state, cond_true, mid, false);
          EmitBlock(lang_stat, state, mid);

          state->cur_block = mid;
        }
        i++;
        FreeSomeRegs(lang_stat);
      }
    }
    else
    {
      ASSERT(false)
    }
  }
  else
  {

    ir_rep ir;
    ir.type = IR_CMP;
    if(!flip_comparison)
      ir.bin.op = T_COND_EQ;
    else
      ir.bin.op = T_COND_NE;
    ir.bin.lhs = GetIRFromAst2(lang_stat, ast, state, true);
    ir.bin.rhs.type = IR_TYPE_INT;
    ir.bin.rhs.i = 1;
    ir.bin.block_id = cond_false->id;
    InsertIr(state, ir);
  }
}
bool IsCondAndOr(tkn_type2 t) { return t == T_COND_AND || t == T_COND_OR; }

void GenIRSubBin(lang_state *lang_stat, ast_rep *ast,
                 own_std::vector<ir_rep> *out, ir_val_type to, void *data) {
  ast_rep *lhs_exp = ast->e_holder.expr[0];
  ast_rep *rhs_exp = ast->e_holder.expr[1];

  bool lhs_complex = IsAstSimple(lang_stat, lhs_exp);
  bool rhs_complex = IsAstSimple(lang_stat, rhs_exp);

  // switch(lhs_complex)
}

bool IsIrValPackedFloat(ir_val *val) {
  type2 *decl_type = &val->decl->type;

  bool is_decl = val->type == IR_TYPE_DECL;
  bool is_ar_float = false;
  if (is_decl)
    is_ar_float = decl_type->type == TYPE_STATIC_ARRAY &&
                  decl_type->tp->type == TYPE_VECTOR;

  return is_decl && (val->decl->type.type == TYPE_VECTOR || is_ar_float) ||
         val->is_packed_float;
}
bool IsIrValFloat(ir_val *val) {
  type2 *decl_type = &val->decl->type;

  bool is_decl = val->type == IR_TYPE_DECL;
  bool is_ar_float = false;
  if (is_decl)
    is_ar_float =
        decl_type->type == TYPE_STATIC_ARRAY && decl_type->tp->type == TYPE_F32;

  return is_decl && (val->decl->type.type == TYPE_F32 || is_ar_float) ||
         val->is_float || val->type == IR_TYPE_F32;
}

void CreateOppositeRegAssigmentAfterCondChecking(
    lang_state *lang_stat, own_std::vector<ir_rep> *out, int sub_if_idx,
    int if_idx, int reg, char true_cond_final_reg_val,
    char false_cond_final_reg_val) {
  ir_rep ir = {};
  ir.type = IR_ASSIGNMENT;
  ir.assign.to_assign.type = IR_TYPE_REG;
  ir.assign.to_assign.reg = reg;
  ir.assign.to_assign.deref = -1;
  ir.assign.to_assign.reg_sz = 8;
  ir.assign.only_lhs = true;

  ir.assign.lhs.type = IR_TYPE_INT;
  ir.assign.lhs.i = false_cond_final_reg_val;
  out->emplace_back(ir);

  ir.type = IR_BREAK_OUT_IF_BLOCK;
  out->emplace_back(ir);

  IRCreateEndBlock(lang_stat, sub_if_idx, out, IR_END_SUB_IF_BLOCK);

  sub_if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_SUB_IF_BLOCK);
  ir = {};
  ir.type = IR_ASSIGNMENT;
  ir.assign.to_assign.type = IR_TYPE_REG;
  ir.assign.to_assign.reg = reg;
  ir.assign.to_assign.deref = -1;
  ir.assign.to_assign.reg_sz = 8;
  ir.assign.only_lhs = true;

  ir.assign.lhs.type = IR_TYPE_INT;
  ir.assign.lhs.i = true_cond_final_reg_val;
  out->emplace_back(ir);
  IRCreateEndBlock(lang_stat, sub_if_idx, out, IR_END_SUB_IF_BLOCK);
  IRCreateEndBlock(lang_stat, if_idx, out, IR_END_IF_BLOCK);
}
void GinIRMemCpy(lang_state *lang_stat, own_std::vector<ir_rep> *out) {
  /*
  ir.type = IR_REP_WRITE;
  out->emplace_back(ir);
  return;
  */
  ir_rep ir = {};

  ir.type = IR_ASSIGNMENT;
  ir.assign.only_lhs = false;
  ir.assign.to_assign.type = IR_TYPE_REG;
  ir.assign.to_assign.is_float = false;
  ir.assign.to_assign.reg = 3;
  ir.assign.to_assign.deref = -1;
  ir.assign.to_assign.reg_sz = 8;
  ir.assign.op = T_MINUS;
  ir.assign.lhs.type = IR_TYPE_REG;
  ir.assign.lhs.reg = 3;
  ir.assign.lhs.deref = -1;
  ir.assign.lhs.reg_sz = 8;
  ir.assign.rhs.type = IR_TYPE_INT;
  ir.assign.rhs.i = 1;
  out->emplace_back(ir);

  int block_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_BLOCK);
  int loop_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_LOOP_BLOCK);
  // int stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT, (void
  // *)(long long)ast->line_number); int cond_idx =
  // IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK); GetIRCond(lang_stat,
  // ast->loop.cond, out); IRCreateEndBlock(lang_stat, stmnt_idx, out,
  // IR_END_STMNT);

  // while reg3 >= 0
  ir = {};
  ir.type = IR_CMP_LE;
  ir.bin.op = T_LESSER_THAN;
  ir.bin.only_lhs = false;
  ir.bin.it_is_jmp_if_true = false;
  ir.bin.lhs.type = IR_TYPE_REG;
  ir.bin.lhs.reg = 3;
  ir.bin.lhs.reg_sz = 4;
  ir.bin.lhs.deref = -1;
  ir.bin.rhs.type = IR_TYPE_INT;
  ir.bin.rhs.reg = 0;
  ir.bin.rhs.reg_sz = 4;
  ir.bin.rhs.deref = 0;
  out->emplace_back(ir);

  // reg0 = arg_reg0 + reg0
  ir.type = IR_ASSIGNMENT;
  ir.assign.only_lhs = true;
  ir.assign.to_assign.is_float = false;
  ir.assign.lhs.is_float = false;
  ir.assign.rhs.is_float = false;
  // ir.assign.only_lhs = true;
  ir.assign.to_assign.type = IR_TYPE_REG;
  ir.assign.to_assign.deref = -1;
  ir.assign.to_assign.reg = 0;
  ir.assign.to_assign.reg_sz = 8;
  ir.assign.lhs.type = IR_TYPE_REG;
  ir.assign.lhs.reg = 32;
  ir.assign.lhs.reg_sz = 8;
  ir.assign.lhs.deref = -1;
  out->emplace_back(ir);

  // reg0 = arg_reg0 + reg0
  ir.type = IR_ASSIGNMENT;
  ir.assign.only_lhs = false;
  ir.assign.to_assign.is_float = false;
  ir.assign.lhs.is_float = false;
  ir.assign.rhs.is_float = false;
  // ir.assign.only_lhs = true;
  ir.assign.to_assign.type = IR_TYPE_REG;
  ir.assign.to_assign.deref = -1;
  ir.assign.to_assign.reg = 0;
  ir.assign.to_assign.reg_sz = 8;
  ir.assign.lhs.type = IR_TYPE_REG;
  ir.assign.lhs.reg = 0;
  ir.assign.lhs.reg_sz = 8;
  ir.assign.lhs.deref = -1;
  ir.assign.op = T_PLUS;
  ir.assign.rhs.type = IR_TYPE_REG;
  ir.assign.rhs.reg = 3;
  ir.assign.rhs.reg_sz = 8;
  ir.assign.rhs.deref = -1;
  ir.assign.rhs.is_unsigned = ir.assign.lhs.is_unsigned;
  out->emplace_back(ir);

  // reg1 = arg_reg1 + reg0
  ir.type = IR_ASSIGNMENT;
  // ir.assign.only_lhs = true;
  ir.assign.only_lhs = true;
  ir.assign.to_assign.type = IR_TYPE_REG;
  ir.assign.to_assign.reg = 1;
  ir.assign.to_assign.reg_sz = 8;
  ir.assign.to_assign.deref = -1;
  ir.assign.lhs.type = IR_TYPE_REG;
  ir.assign.lhs.reg = 33;
  ir.assign.lhs.reg_sz = 8;
  ir.assign.lhs.deref = -1;
  out->emplace_back(ir);

  // reg1 = arg_reg1 + reg0
  ir.type = IR_ASSIGNMENT;
  ir.assign.only_lhs = false;
  ir.assign.to_assign.type = IR_TYPE_REG;
  ir.assign.to_assign.reg = 1;
  ir.assign.to_assign.reg_sz = 8;
  ir.assign.to_assign.deref = -1;
  ir.assign.lhs.type = IR_TYPE_REG;
  ir.assign.lhs.reg = 1;
  ir.assign.lhs.reg_sz = 8;
  ir.assign.lhs.deref = -1;
  ir.assign.op = T_PLUS;
  ir.assign.rhs.type = IR_TYPE_REG;
  ir.assign.rhs.reg = 3;
  ir.assign.rhs.reg_sz = 8;
  ir.assign.rhs.deref = -1;
  ir.assign.rhs.is_unsigned = ir.assign.lhs.is_unsigned;
  out->emplace_back(ir);

  // *reg0 = *reg1
  ir.type = IR_ASSIGNMENT;
  ir.assign.only_lhs = true;
  // ir.assign.only_lhs = true;
  ir.assign.to_assign.type = IR_TYPE_REG;
  ir.assign.to_assign.reg = 0;
  ir.assign.to_assign.reg_sz = 1;
  ir.assign.to_assign.deref = 0;
  ir.assign.lhs.type = IR_TYPE_REG;
  ir.assign.lhs.reg = 1;
  ir.assign.lhs.reg_sz = 1;
  ir.assign.lhs.deref = 0;
  out->emplace_back(ir);

  // reg3 = reg3 - 1
  ir.type = IR_ASSIGNMENT;
  ir.assign.only_lhs = false;
  ir.assign.to_assign.type = IR_TYPE_REG;
  ir.assign.to_assign.reg = 3;
  ir.assign.to_assign.deref = -1;
  ir.assign.to_assign.reg_sz = 8;
  ir.assign.lhs.type = IR_TYPE_REG;
  ir.assign.lhs.reg = 3;
  ir.assign.lhs.reg_sz = 8;
  ir.assign.lhs.deref = -1;
  ir.assign.op = T_MINUS;
  ir.assign.rhs.type = IR_TYPE_INT;
  ir.assign.rhs.i = 1;
  ir.assign.rhs.is_unsigned = ir.assign.lhs.is_unsigned;
  out->emplace_back(ir);
  // IRCreateEndBlock(lang_stat, cond_idx, out, IR_END_IF_BLOCK);
  IRCreateEndBlock(lang_stat, loop_idx, out, IR_END_LOOP_BLOCK);
  IRCreateEndBlock(lang_stat, block_idx, out, IR_END_BLOCK);
}
bool IsBeginInsideIf(ast_rep *e) {
  bool is_end_arg = ((long long)e & 0xff) == IR_BEGIN_INSIDE_IF;
  is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
  return is_end_arg;
}
bool IsEndInsideIf(ast_rep *e) {
  bool is_end_arg = ((long long)e & 0xff) == IR_END_INSIDE_IF;
  is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
  return is_end_arg;
}
bool IsBeginOr(ast_rep *e) {
  bool is_end_arg = ((long long)e & 0xff) == IR_BEGIN_OR;
  is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
  return is_end_arg;
}
bool IsEndOr(ast_rep *e) {
  bool is_end_arg = ((long long)e & 0xff) == IR_END_OR;
  is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
  return is_end_arg;
}
bool IsBeginAnd(ast_rep *e) {
  bool is_end_arg = ((long long)e & 0xff) == IR_BEGIN_AND;
  is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
  return is_end_arg;
}
bool IsEndAnd(ast_rep *e) {
  bool is_end_arg = ((long long)e & 0xff) == IR_END_AND;
  is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
  return is_end_arg;
}
bool IsEndFuncCall(ast_rep *e) {
  return ((long long)e & 0xff) == IR_END_FUNC_CALL;
}
bool IsBeginFuncCall(ast_rep *e) {
  return ((long long)e & 0xff) == IR_BEGIN_FUNC_CALL;
}
bool IsEndArg(ast_rep *e) {
  bool is_end_arg = ((long long)e & 0xff) == IR_END_ARG;
  is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
  return is_end_arg;
}
void GenIrToArgReg(lang_state *lang_stat, own_std::vector<ir_val> &stack,
                   int arg_idx, own_std::vector<ast_rep *> &exps,
                   own_std::vector<ir_rep> *out) {
  ir_val *top = &stack[stack.size() - 1];
  // stack.pop_back();
  ir_rep ir = {};
  if (top->ptr > 0 || top->ptr == -1) {
    top->is_float = false;
    top->is_packed_float = false;
  }
  ir.type = IR_ASSIGNMENT;
  ir.assign.to_assign.type = IR_TYPE_ARG_REG;
  ir.assign.to_assign.reg = arg_idx;
  ir.assign.to_assign.reg_sz = 8;
  ir.assign.to_assign.deref = -1;
  ir.assign.to_assign.is_float = top->is_float;
  ir.assign.only_lhs = true;

  if (IS_FLAG_ON(top->reg_ex, IR_VAL_FROM_POINT) &&
      IS_FLAG_ON(top->reg_ex, IR_VAL_FROM_DEREF)) {
    // top->deref--;
  }
  if (top->type == IR_TYPE_REG && top->deref == -1 && top->ptr > 0)
  {
    top->deref++;
  }
  ir.assign.lhs = *top;

  ir.assign.lhs.reg_sz = 8;
  if (top->type == IR_TYPE_DECL && top->decl->type.type == TYPE_STATIC_ARRAY) {
    ir.assign.lhs.deref = -1;
  }
  if (top->is_packed_float) {
    ir.assign.lhs.is_float = false;
    ir.assign.lhs.is_packed_float = false;
  }

  // ir.assign.lhs.deref--;
  // if (IS_FLAG_ON(top->reg_ex, IR_VAL_FROM_POINT) || top->ptr > 0)
  // ir.assign.lhs.deref++;
  out->emplace_back(ir);

  if (top->type == IR_TYPE_REG) {
    FreeReg(lang_stat, top->reg, top->is_float);
  }
  top->reg_ex |= IR_VAL_ALREADY_ON_PARAM_REG;
}
void GinIRCheckNilPtr(lang_state *lang_stat, own_std::vector<ir_rep> *out,
                      ir_val *top_info) {

  int if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);
  int sub_if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_SUB_IF_BLOCK);
  int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_COND_BLOCK);

  ir_rep aux_ir = {};

  aux_ir.type = IR_CMP_LE;
  aux_ir.bin.op = T_GREATER_THAN;
  aux_ir.bin.lhs = *top_info;
  aux_ir.bin.lhs.deref = top_info->ptr - 1;
  aux_ir.bin.lhs.ptr = 0;
  aux_ir.bin.lhs.is_float = false;
  aux_ir.bin.lhs.is_packed_float = false;

  aux_ir.bin.rhs.type = IR_TYPE_INT;
  aux_ir.bin.rhs.i = 2024;
  aux_ir.bin.rhs.is_unsigned = top_info->is_unsigned;
  out->emplace_back(aux_ir);

  IRCreateEndBlock(lang_stat, cond_idx, out, IR_END_COND_BLOCK);

  /*
  aux_ir.type = IR_ASSIGNMENT;
  aux_ir.assign.to_assign.type = IR_TYPE_ARG_REG;
  aux_ir.assign.to_assign.reg = 0;
  aux_ir.assign.to_assign.reg_sz = 8;
  aux_ir.assign.to_assign.deref = -1;
  aux_ir.assign.to_assign.is_float = top->is_float;
  aux_ir.assign.to_assign.lhs.type = IR_TYPE_INT;
  aux_ir.assign.to_assign.lhs.i = 0;

  out->emplace_back(aux_ir);

  aux_ir.assign.to_assign.reg = 1;
  aux_ir.assign.to_assign.is_float = top->is_float;
  aux_ir.assign.to_assign.lhs.type = IR_TYPE_STR_LIT;
  aux_ir.assign.to_assign.lhs.str = "invalid ptr derefed";
  out->emplace_back(aux_ir);

  aux_ir.assign.to_assign.reg = 2;
  aux_ir.assign.to_assign.is_float = top->is_float;
  aux_ir.assign.to_assign.lhs.type = IR_TYPE_STR_LIT;
  aux_ir.assign.to_assign.lhs.str = "invalid ptr derefed";
  out->emplace_back(aux_ir);
  */
  type2 dummy_tp;

  aux_ir.type = IR_CALL;

  aux_ir.call.is_outsider = false;
  aux_ir.call.is_outsider = true;

  auto str = own_std::string("PrintCallStack");
  aux_ir.call.fdecl =
      FindIdentifier(str, lang_stat->funcs_scp, &dummy_tp, 0)->type.fdecl;
  out->emplace_back(aux_ir);

  aux_ir.type = IR_DBG_BREAK;
  out->emplace_back(aux_ir);
  IRCreateEndBlock(lang_stat, sub_if_idx, out, IR_END_SUB_IF_BLOCK);
  IRCreateEndBlock(lang_stat, if_idx, out, IR_END_IF_BLOCK);

  // FreeSpecificReg(lang_stat, reg);
}


void DoStructConstruction(lang_state *lang_stat, ast_rep *ast,
                          ir_val_type dts_type, void *data,
                          own_std::vector<ir_rep> *out) {
  ir_rep ir;
  ir.type = IR_ASSIGNMENT;
  ir.assign.to_assign.type = IR_TYPE_ON_STACK;
  ir.assign.to_assign.reg_sz = 8;
  // ir.assign.to_assign.i = ast->strct_constr.at_offset;
  FOR_VEC(info, ast->strct_constr.commas) {
    ir.assign.to_assign.i = ast->strct_constr.at_offset + info->var->offset;

    if (IsAstSimple(lang_stat, info->exp)) {
      ir.assign.only_lhs = true;
      GetIRVal(lang_stat, info->exp, &ir.assign.lhs);
    } else
      ASSERT(0);

    out->emplace_back(ir);
  }
}


int GetAstTypeSize(lang_state *lang_stat, ast_rep *ast) {
  switch (ast->type) {
  case AST_ARRAY_COSTRUCTION:
  case AST_ADDRESS_OF: {
    return 8;
  } break;
  case AST_FLOAT:
  case AST_INT: {
    return 4;
  } break;
  case AST_CALL: {
    return GetTypeSize(&ast->call.fdecl->ret_type);
  } break;
  case AST_BINOP: {
    if (ast->op == T_POINT) {
      return GetAstTypeSize(lang_stat, ast->points.back().exp);
    } else
      return GetAstTypeSize(lang_stat, ast->e_holder.expr[0]);
  } break;
  case AST_CAST: {
    int prev = ast->cast.type.ptr;

    // ast->cast.type.ptr = max(0, prev - 1);

    int sz = GetTypeSize(&ast->cast.type);
    ast->cast.type.ptr = prev;
    return sz;
  } break;
  case AST_NEGATIVE: {
    return GetAstTypeSize(lang_stat, ast->ast);
  } break;
  case AST_DEREF: {
    return GetAstTypeSize(lang_stat, ast->deref.exp);
  } break;
  case AST_INDEX: {
    return GetTypeSize(&ast->index.lhs_type);
  } break;
  case AST_IDENT: {
    return GetTypeSize(&ast->decl->type);
  } break;
  default:
    ASSERT(0);
  }
  return 0;
}




ast_rep *CreateDbgEqualStmnt(lang_state *lang_stat) {
  ast_rep *bin = NewAst();
  ast_rep *lhs = NewAst();
  ast_rep *rhs = NewAst();

  own_std::string str("__global_dummy");

  lhs->type = AST_IDENT;
  lhs->decl = FindIdentifier(str, lang_stat->root, &rhs->lhs_tp);

  bin->type = AST_BINOP;
  bin->op = T_EQUAL;
  bin->e_holder.expr.emplace_back(lhs);
  bin->e_holder.expr.emplace_back(rhs);
  return bin;
}
void MakeIrStore(lang_state *lang_stat, ir_val *lhs, ir_val *rhs, ir_rep *out)
{
  out->type = IR_BIN;
  out->bin.op = T_EQUAL;
  out->bin.lhs = *lhs;
  out->bin.rhs = *rhs;
}
void MakeIrAssignment(lang_state *lang_stat, ir_val *to_assign, ir_val *lhs, ir_rep *out)
{
  out->type = IR_ASSIGNMENT;
  out->assign.only_lhs = true;
  out->assign.lhs = *lhs;
}
void MakeIrAssignment(lang_state *lang_stat, tkn_type2 bin, ir_val *to_assign, ir_val *lhs, ir_val *rhs, ir_rep *out)
{
  out->type = IR_ASSIGNMENT;
  out->assign.only_lhs = false;
  if(!rhs)
  {
    out->assign.only_lhs = true;
  }
  out->assign.op = bin;
  out->assign.lhs = *lhs;
  out->assign.rhs = *rhs;
}
void FreeRegs2(lang_state *lang_stat)
{
  for(int i = 0; i < 16; i++)
  {
    lang_stat->regs[i] = 0;
    lang_stat->float_regs[i] = 0;
  }
}
void GetIRCallArg(lang_state *lang_stat, ast_rep *arg, thread_ir_state *state, int i, int *float_args, int args_on_stack_start, bool is_sys_call)
{
  ir_rep ir={};
  ir.bin.rhs = GetIRFromAst2(lang_stat, arg, state, true);
  bool was_float = ir.bin.rhs.is_float;

  char reg = -1;

  //BREAK(arg->line_number == 1388)
  if(ir.bin.rhs.type == IR_TYPE_REG) 
  {
    reg = ir.bin.rhs.reg;
  }

  ir.type = IR_BIN;
  ir.bin.op = T_EQUAL;
  if(ir.bin.rhs.is_float)
  {
    char free_reg = -1;
    char deref = ir.bin.rhs.deref;

    EnsureValue(lang_stat, state, &ir.bin.rhs);
    LoadDerefs(lang_stat, &state->cur_block->irs, &ir.bin.rhs);
    if(deref > 1)
    {
      free_reg = ir.bin.rhs.reg;
    }
    ASSERT(*float_args < 7)

    ir.bin.lhs.type = IR_TYPE_REG;
    ir.bin.lhs.is_float = true;
    ir.bin.lhs.reg_sz = ir.bin.rhs.reg_sz;
    ir.bin.lhs.is_packed_float = ir.bin.rhs.is_packed_float;
    ir.bin.lhs.reg = *float_args;
    (*float_args)++;

    if(free_reg != -1)
      FreeReg(lang_stat, free_reg, true);

  }
  else
  {
    char deref = ir.bin.rhs.deref;
    EnsureValue(lang_stat, state, &ir.bin.rhs);
    LoadDerefs(lang_stat, &state->cur_block->irs, &ir.bin.rhs);

    if(i >= args_on_stack_start)
    {
      if(ir.bin.rhs.kind == IR_VAL_ADDR && ir.bin.rhs.deref > 0)
      {
      }
      ir.bin.lhs.type = IR_TYPE_REG_MEM;
      ir.bin.lhs.reg = (char)regs_enum::RSP;
      ir.bin.lhs.reg_sz = 8;
      ir.bin.lhs.voffset = (i - args_on_stack_start) * 8;

    }
    else
    {
      char reg_arg = FromIdxToArgReg(i, is_sys_call);
      AllocSpecificReg(lang_stat, reg_arg);
      ir.bin.lhs.type = IR_TYPE_REG;
      ir.bin.lhs.reg = reg_arg;
      ir.bin.lhs.reg_sz = 8;
    }
  }

  if(reg != -1)
    FreeReg(lang_stat, reg, was_float);

  InsertIr(state, ir);

}

void UnspillReg(thread_ir_state *state, ir_val *aux)
{
  ir_rep ir;
  ir.type = IR_UNSPILL;
  ir.bin.lhs = *aux;
  ir.bin.rhs.reg =  state->spilled_regs.cur;
  InsertIr(state, ir);
}
void SpillReg(thread_ir_state *state, ir_val *aux)
{
  ir_rep ir;
  ir.type = IR_SPILL;
  ir.bin.lhs.reg =  state->spilled_regs.cur;
  ir.bin.lhs.is_float =  aux->is_float;
  ir.bin.lhs.is_packed_float =  aux->is_packed_float;
  ir.bin.lhs.reg_sz =  aux->reg_sz;
  ir.bin.rhs.type =  IR_TYPE_REG;
  ir.bin.rhs = *aux;
  InsertIr(state, ir);
  push_tracker_stack(&state->spilled_regs, *aux);
}
ir_val GetIRCall(lang_state *lang_stat, ast_rep *ast, thread_ir_state *state, bool is_lhs)
{
  func_decl *callf = ast->call.fdecl;
  ir_rep ir;
  ir_val ret;
  int i = 0;

  int call_base = state->spilled_regs.cur;
  bool is_syscall = IS_FLAG_ON(callf->flags, FUNC_DECL_SYSCALL);

  int total_var_args = ast->call.args.size() - callf->args.size();
  int start_on_regs = callf->args.size();

  if(total_var_args > 0) start_on_regs--;


  if(start_on_regs >= MAX_CALL_REGS)
  {
    start_on_regs = MAX_CALL_REGS;
  }

  add_var_args(state, total_var_args);

  for(i = 0; i < MAX_CALL_REGS; i++)
  {
    char reg_arg = FromIdxToArgReg(i);

    ir.type = IR_SPILL;

    if(IsRegInUse(lang_stat, reg_arg))
    {
      ir_val aux = {};
      aux.type = IR_TYPE_REG;
      aux.reg_sz = 8;
      aux.reg = reg_arg;
      SpillReg(state, &aux);
    }
    if(i >= 6) break;
  }
  //BREAK(ast->line_number == 1764)
  for(i = 0; i < 8; i++)
  {
    ir.type = IR_SPILL;
    if(IsRegInUse(lang_stat, i + 8, true))
    {
      ir_val aux = {};
      aux.type = IR_TYPE_REG;
      aux.reg_sz = 8;
      aux.reg = i + 8;
      aux.is_float = true;
      aux.is_packed_float = true;
      SpillReg(state, &aux);
    }
  }
  i = 0;
  int f_args = 0;
  FOR_VEC(arg, ast->call.args)
  {
    if(HasCall(lang_stat, *arg))
    {
      GetIRCallArg(lang_stat, *arg, state, i, &f_args, start_on_regs, is_syscall);
      (*arg)->ir_generated = true;
    }

    i++;
  }


  i=0;
  f_args = 0;
  FOR_VEC(arg, ast->call.args)
  {

    if(!(*arg)->ir_generated)
    {
      GetIRCallArg(lang_stat, *arg, state, i, &f_args, start_on_regs, is_syscall);
    }

    i++;
  }
  ir.type = IR_CALL;

  ir.call.is_outsider = false;
  if (IS_FLAG_ON(ast->call.fdecl->flags, FUNC_DECL_IS_OUTSIDER))
    ir.call.is_outsider = true;

  ir.call.fdecl = ast->call.fdecl;


  if (ast->call.indirect) {
    if (ast->call.lhs) {
      ir_val aux = GetIRFromAst2(lang_stat, ast->call.lhs, state, true);
      ir.bin.lhs.type = IR_TYPE_REG;
      ir.bin.lhs = aux;
      ir.bin.lhs.decl = ast->call.fdecl->this_decl;
    } else {
      ir.type = IR_BIN;
      ir.bin.op = T_EQUAL;
      ir.bin.lhs.type = IR_TYPE_REG;
      ir.bin.lhs.reg = GetAvailableReg(lang_stat);
      ir.bin.lhs.reg_sz = 8;
      ir.bin.lhs.is_float = false;
      ir.bin.lhs.is_packed_float = false;
      ir.bin.rhs.type = IR_TYPE_DECL;
      ir.bin.rhs.decl = ast->call.func_ptr_var;
      ir.bin.rhs.reg = (char)regs_enum::RSP;
      ir.bin.rhs.voffset = 0;
      InsertIr(state, ir);
    }
    ir.type = IR_INDIRECT_CALL;
    ir.bin.lhs.is_float = false;
  }
  InsertIr(state, ir);

  ret.ptr = 0;

  //if(callf->name == "GetMem" && ast->line_number == 28) HERE()


  if(callf->ret_type.type == TYPE_STRUCT && callf->ret_type.ptr == 0)
  {
    //BREAK(ast->line_number == 72)
    int st_size = GetTypeSize(&callf->ret_type);
    int offset = get_strct_ret(state);
    add_strct_ret(state, st_size);
    ir = {};
    ir.type = IR_ADDRESS_OF;
    ir.bin.lhs.type = IR_TYPE_REG;
    ir.bin.lhs.reg = (char)regs_enum::RDI;
    ir.bin.lhs.reg_sz = 8;
    ir.bin.rhs.type = IR_TYPE_ON_STACK;
    ir.bin.rhs.stack.on_stack_type = ON_STACK_STRUCT_RET;
    ir.bin.rhs.stack.i = offset;
    ir.bin.rhs.reg_sz = 8;

    ret = ir.bin.rhs;
    ret.kind = IR_VAL_ADDR;

    InsertIr(state, ir);

    ir = {};
    ir.type = IR_BIN;
    ir.bin.op = T_EQUAL;
    ir.bin.lhs.type = IR_TYPE_REG;
    ir.bin.lhs.reg = (char)regs_enum::RSI;
    ir.bin.lhs.reg_sz = 8;
    ir.bin.rhs.type = IR_TYPE_REG;
    ir.bin.rhs.reg = 0;
    ir.bin.rhs.reg_sz = 0;

    InsertIr(state, ir);

    ir.type = IR_BIN;
    ir.bin.lhs.reg = (char)regs_enum::RCX;
    ir.bin.rhs.type = IR_TYPE_INT;
    ir.bin.rhs.i = st_size;

    InsertIr(state, ir);

    ir.type = IR_REPMOVSB;
    InsertIr(state, ir);



  }
  else
  {
    ret.reg_sz = GetTypeSize(&callf->ret_type, 0);
    ret.type = IR_TYPE_REG;
    ret.reg = 0;
    ret.reg_sz = max(1, ret.reg_sz);
    ret.deref = 0;
    ret.voffset = 0;
    ret.ptr = callf->ret_type.ptr;

    ir = {};

    if(callf->ret_type.IsFloat() && callf->ret_type.ptr == 0)
    {
      ir.type = IR_BIN;
      ir.bin.op = T_EQUAL;
      ir.bin.lhs.type = IR_TYPE_REG;
      ir.bin.lhs.reg = AllocFloatReg(lang_stat);
      ir.bin.lhs.is_float = callf->ret_type.IsFloat();
      ir.bin.lhs.is_packed_float = callf->ret_type.type == TYPE_VECTOR;

      if(ret.reg_sz > 8) ret.reg_sz = 8;

      if(callf->ret_type.type == TYPE_VECTOR)
      {
        ir.bin.lhs.reg_sz = callf->ret_type.vec_type;
      }
      else
      {
        ir.bin.lhs.reg_sz = ret.reg_sz;
      }
      ir.bin.rhs = ret;
      ret = ir.bin.lhs;
      InsertIr(state, ir);
    }
    else
      AllocSpecificReg(lang_stat, 0, false);
    ret.kind = IR_VAL_VALUE;
  }


  while(state->spilled_regs.cur > call_base)
  {
    ir_val unspill = *pop_tracker_stack(&state->spilled_regs);

    UnspillReg(state, &unspill);
  }

  ret.is_float = callf->ret_type.IsFloat();
  ret.is_unsigned = IsUnsigned(callf->ret_type.type);
  ret.is_packed_float = callf->ret_type.type == TYPE_VECTOR;
  if(ret.is_packed_float)
  {
    ret.reg_sz = callf->ret_type.vec_type;

  }


  return ret;
}

void GetIRCondValue(lang_state *lang_stat, ast_rep *ast, thread_ir_state *state, char true_val, char false_val)
{
  ir_rep ir = {};
  block2 *cond_true = CreateBlock(lang_stat, state);
  block2 *cond_false = CreateBlock(lang_stat, state);


  block2 *merge = CreateBlock(lang_stat, state);

  GetIRCond2(lang_stat, ast, state, cond_true, cond_false);

  EmitBlockMakeCurrent(lang_stat, state, cond_true);

  ir.type = IR_BIN;
  ir.bin.op = T_EQUAL;
  ir.bin.lhs.type = IR_TYPE_REG;
  ir.bin.lhs.reg_sz = 8;
  ir.bin.lhs.reg = 0;
  ir.bin.rhs.type = IR_TYPE_INT;
  ir.bin.rhs.i = true_val;

  InsertIr(state, ir);
  EmitJmp(lang_stat, state, merge);

  EmitBlockMakeCurrent(lang_stat, state, cond_false);

  ir.bin.rhs.i = false_val;

  InsertIr(state, ir);
  EmitJmp(lang_stat, state, merge);
  EmitBlockMakeCurrent(lang_stat, state, merge);

}
bool IsIrValLiteral(ir_val_type tp)
{
  return tp == IR_TYPE_INT64 || tp == IR_TYPE_INT || tp == IR_TYPE_F32 || tp == IR_TYPE_F64;
}
ir_val GetDoubleIr(lang_state *lang_stat, thread_ir_state *state, double f)
{
  ir_val ret;
  ir_rep ir={};

  ir.type = IR_GET_FLOAT;
  ir.bin.lhs.type = IR_TYPE_REG;
  ir.bin.lhs.is_float = true;
  ir.bin.lhs.reg = AllocFloatReg(lang_stat);
  ir.bin.lhs.reg_sz = 8 ;
  ir.bin.rhs.f64 = f;
  InsertIr(state, ir);
  ret = ir.bin.lhs;
  ret.kind = IR_VAL_VALUE;

  return ret;
}
ir_val GetFloatIr(lang_state *lang_stat, thread_ir_state *state, float f)
{
  ir_val ret;
  ir_rep ir={};

  ir.type = IR_GET_FLOAT;
  ir.bin.lhs.type = IR_TYPE_REG;
  ir.bin.lhs.is_float = true;
  ir.bin.lhs.kind = IR_VAL_VALUE;
  ir.bin.lhs.reg = AllocFloatReg(lang_stat);
  ir.bin.lhs.reg_sz = 4;
  ir.bin.rhs.f32 = f;

  InsertIr(state, ir);

  ret = ir.bin.lhs;
  return ret;
}
void CheckRegInUseMaybeSpill(lang_state *lang_stat, thread_ir_state *state, char reg)
{
  if(IsRegInUse(lang_stat, reg))
  {
    ir_val aux ={};
    aux.type = IR_TYPE_REG;
    aux.reg = reg;
    aux.reg_sz = 8;
    SpillReg(state, &aux);
  }
}
ir_val GetIRFromAst2(lang_state *lang_stat, ast_rep *ast, thread_ir_state *state, bool is_lhs)
{
  ir_val ret;
  ret.is_float = false;
  ret.is_packed_float = false;
  ir_rep ir= {};
  ir_val lhs = {};
  ir_val rhs = {};
  ret.reg_sz = 1;
  
  switch(ast->type)
  {
  case AST_INDEX: 
  {
    //BREAK(ast->line_number == 1424)
    rhs = GetIRFromAst2(lang_stat, ast->index.rhs, state, false);

    if(rhs.kind == IR_VAL_ADDR)
    {
      LoadDerefs(lang_stat, &state->cur_block->irs, &rhs);
      EnsureValue(lang_stat, state, &ir.bin.lhs);
    }
    
    char reg=-1;
    u32 tp_sz = GetTypeSize(ast->index.lhs_type.tp);
    if(rhs.type == IR_TYPE_INT)
    {
      rhs.i *= tp_sz;
    }
    else
    {
      ir.type = IR_BIN;
      ir.bin.op = T_MUL;
      ir.bin.lhs = rhs;
      ir.bin.rhs.type = IR_TYPE_INT;
      ir.bin.rhs.i = tp_sz;

      InsertIr(state, ir);
    }

    //BREAK(ast->line_number == 1098)
    lhs = GetIRFromAst2(lang_stat, ast->index.lhs, state, true);

    bool can_do_plus = true;
    lhs.kind = IR_VAL_ADDR;
    if(lhs.type == IR_TYPE_DECL)
    {
      if(lhs.ptr > 0)
      {
        LoadDerefs(lang_stat, &state->cur_block->irs, &lhs);
        EnsureValue(lang_stat, state, &ir.bin.lhs);

      }
      else
      {
        ir.type = IR_ADDRESS_OF;
        ir.bin.lhs.type = IR_TYPE_REG;
        ir.bin.lhs.reg = GetAvailableReg(lang_stat);
        ir.bin.lhs.reg_sz = 8;
        ir.bin.rhs = lhs;

        if(rhs.type == IR_TYPE_INT)
        {
          ir.bin.rhs.voffset = rhs.i;
          can_do_plus = false;
        }

        InsertIr(state, ir);

        bool was_float = lhs.is_float;
        bool was_packed = lhs.is_packed_float;
        char reg_sz = lhs.reg_sz;

        lhs = ir.bin.lhs;
        lhs.is_float = was_float;
        lhs.is_packed_float = was_packed;
        lhs.reg_sz = reg_sz;
        lhs.ptr = 0;
        lhs.deref = 0;
        //lhs.kind = IR_VAL_VALUE;
        lhs.type = IR_TYPE_REG;
      }
    }

    if(can_do_plus)
    {
      ir.type = IR_BIN;
      ir.bin.op = T_PLUS;
      ir.bin.lhs = lhs;
      ir.bin.lhs.reg_sz = 8;
      ir.bin.lhs.is_float = false;
      ir.bin.lhs.is_packed_float = false;
      ir.bin.rhs = rhs;

      InsertIr(state, ir);
    }

    ret = lhs;
    //ret.type = IR_TYPE_REG_MEM;
    if(!is_lhs)
    {
      ret.deref = 1;
      char prev_reg = ret.reg;
      LoadDerefs(lang_stat, &state->cur_block->irs, &ret);
      //EnsureValue(lang_stat, state, &ret);
      //
      if(ret.is_float) FreeSpecificReg(lang_stat, prev_reg);

      ret.deref = 0;
      ret.kind = IR_VAL_VALUE;
      ret.type = IR_TYPE_REG;
    }
    else
    {
      if(ret.type == IR_TYPE_REG)
      {
        ret.type = IR_TYPE_REG_MEM;
      }
    }

  }break;
  case AST_WHILE: 
  {
    ret.reg_sz = 1;
    bool prev_is_in_stmnt = lang_stat->ir_in_stmnt;
    lang_stat->ir_in_stmnt = false;
    int stmnt_idx = IRCreateBeginBlock(lang_stat, &state->cur_block->irs, IR_BEGIN_STMNT,
                                       (void *)(long long)ast->line_number);

    // int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);
    block2 *cond_true = CreateBlock(lang_stat, state);
    block2 *cond_start = CreateBlock(lang_stat, state);
    block2 *merge = CreateBlock(lang_stat, state);

    EmitBlockMakeCurrent(lang_stat, state, cond_start);
    GetIRCond2(lang_stat, ast->loop.cond, state, cond_true, merge);

    IRCreateEndBlock(lang_stat, stmnt_idx, &state->cur_block->irs, IR_END_STMNT);

    EmitBlockMakeCurrent(lang_stat, state, cond_true);

    push_tracker_stack(&state->continue_start_block, cond_true);
    push_tracker_stack(&state->break_end_block, merge);

    if (ast->loop.scope) {
      GetIRFromAst2(lang_stat, ast->loop.scope, state, false);
    }


    EmitJmp(lang_stat, state, cond_start);


    EmitBlockMakeCurrent(lang_stat, state, merge);

    pop_tracker_stack(&state->continue_start_block);
    pop_tracker_stack(&state->break_end_block);

    lang_stat->ir_in_stmnt = prev_is_in_stmnt;
    return ret;

  } break;
  case AST_FOR: 
  {
    ret.reg_sz = 1;
    bool prev_is_in_stmnt = lang_stat->ir_in_stmnt;
    lang_stat->ir_in_stmnt = false;
    int stmnt_idx = IRCreateBeginBlock(lang_stat, &state->cur_block->irs, IR_BEGIN_STMNT,
                                       (void *)(long long)ast->line_number);

    GetIRFromAst2(lang_stat, ast->for_info.start_stat, state, false);
    FreeRegs2(lang_stat);

    // int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);
    block2 *cond_true = CreateBlock(lang_stat, state);
    block2 *cond_start = CreateBlock(lang_stat, state);
    block2 *for_end = CreateBlock(lang_stat, state);
    block2 *merge = CreateBlock(lang_stat, state);

    EmitBlockMakeCurrent(lang_stat, state, cond_start);
    GetIRCond2(lang_stat, ast->for_info.cond_stat, state, cond_true, merge);

    FreeRegs2(lang_stat);

    IRCreateEndBlock(lang_stat, stmnt_idx, &state->cur_block->irs, IR_END_STMNT);

    EmitBlockMakeCurrent(lang_stat, state, cond_true);

    push_tracker_stack(&state->continue_start_block, for_end);
    push_tracker_stack(&state->break_end_block, merge);

    if (ast->for_info.scope) {
      GetIRFromAst2(lang_stat, ast->for_info.scope, state, false);
    }

    EmitBlockMakeCurrent(lang_stat, state, for_end);

    FreeRegs2(lang_stat);
    GetIRFromAst2(lang_stat, ast->for_info.at_loop_end_stat, state, false);

    EmitJmp(lang_stat, state, cond_start);


    EmitBlockMakeCurrent(lang_stat, state, merge);

    pop_tracker_stack(&state->continue_start_block);
    pop_tracker_stack(&state->break_end_block);

    lang_stat->ir_in_stmnt = prev_is_in_stmnt;

    return ret;
  } break;
  case AST_RET: 
  {
    if(ast->ret.ast){
      bool is_lhs = false;
      //BREAK(ast->line_number == 2045)

      if(lang_stat->cur_func->ret_type.type == TYPE_STRUCT && lang_stat->cur_func->ret_type.ptr == 0)
      {
        is_lhs = true;
      }

      ret = GetIRFromAst2(lang_stat, ast->ret.ast, state, is_lhs);


      if(is_lhs)
      {
        ir.type = IR_ADDRESS_OF;
        ir.bin.lhs.reg_sz = 8;
      }
      else
      {
        if(ret.ptr > 0 && !ret.is_packed_float)
        {
          ret.is_float = false;
        }
        EnsureValue(lang_stat, state, &ret);
        ir.type = IR_BIN;
        ir.bin.op = T_EQUAL;
        ir.bin.lhs.reg_sz = ret.reg_sz;
      }
      ir.bin.lhs.type = IR_TYPE_REG;
      ir.bin.lhs.reg = (char)regs_enum::RAX;
      ir.bin.lhs.is_unsigned = ret.is_unsigned;
      ir.bin.lhs.is_float = ret.is_float;
      ir.bin.lhs.is_packed_float = ret.is_packed_float;
      ir.bin.rhs = ret;

      InsertIr(state, ir);
    }
    ir.type = IR_RET;
    ir.bin.lhs.reg_sz = 8;
    InsertIr(state, ir);


  }break;
  case AST_CALL: 
  {
    ret = GetIRCall(lang_stat, ast, state, false);
  }break;
  case AST_ON: {
    int prev_sz = lang_stat->cur_ast;

    ast_rep *if_ast = NewAstLinear(lang_stat);
    ast_rep *cur_if = if_ast;
    cur_if->type = AST_IF;
    cur_if->cond.from_on_ast = true;

    ast_rep *main_ast = ast->on.main;

    ast_rep *cond_eq = NewAstLinear(lang_stat);
    cond_eq->type = AST_BINOP;
    cond_eq->e_holder.expr.emplace_back(main_ast);
    cond_eq->op = T_COND_EQ;

    FOR_VEC(cur_cond, ast->on.exprs) {
      cur_if->cond.scope = cur_cond->scp;

      if (cur_if != if_ast) {
        if_ast->cond.elses.emplace_back(cur_if);
        cur_if->type = AST_ELSE_IF;
      }

      if (cur_cond->cond != nullptr) {
        if (cur_cond->cond->type == AST_BINOP &&
            cur_cond->cond->op == T_COMMA) {
          ast_rep *cond_or = NewAstLinear(lang_stat);
          cond_or->type = AST_BINOP;
          cond_or->op = T_COND_OR;
          FOR_VEC(comma, cur_cond->cond->e_holder.expr) {

            ast_rep *c = *comma;
            cond_eq->e_holder.expr.emplace_back(c);
            cond_or->e_holder.expr.emplace_back(cond_eq);

            cond_eq = NewAstLinear(lang_stat);
            cond_eq->type = AST_BINOP;
            cond_eq->e_holder.expr.emplace_back(main_ast);
            cond_eq->op = T_COND_EQ;
          }
          cur_if->cond.cond = cond_or;
        } else {
          cond_eq->e_holder.expr.emplace_back(cur_cond->cond);
          cur_if->cond.cond = cond_eq;
        }
      }

      cond_eq = NewAstLinear(lang_stat);
      cond_eq->type = AST_BINOP;
      cond_eq->e_holder.expr.emplace_back(main_ast);
      cond_eq->op = T_COND_EQ;

      cur_if = NewAstLinear(lang_stat);
      cur_if->type = AST_ELSE;
    }
    if (ast->on.def) {
      ast_rep *def_ast = NewAstLinear(lang_stat);
      def_ast->type = AST_ELSE;
      def_ast->cond.scope = ast->on.def;
      if_ast->cond.elses.emplace_back(def_ast);
    }

    auto prev_bool = lang_stat->ir_in_stmnt;
    auto prev_bool_conds = lang_stat->no_stmnt_of_conds;

    lang_stat->ir_in_stmnt = false;
    lang_stat->no_stmnt_of_conds = true;

    GetIRFromAst2(lang_stat, if_ast, state, true);

    lang_stat->cur_ast = prev_sz;
    lang_stat->ir_in_stmnt = prev_bool;
    lang_stat->no_stmnt_of_conds = prev_bool_conds;
  } break;
  case AST_IF: 
  {
    FreeRegs2(lang_stat);

    bool is_if_expr = false;
    bool is_if_expr_float = false;
    bool is_if_expr_packed_float = false;
    bool is_if_expr_unsigned = false;
    char is_if_expr_reg_sz = 0;

    if(ast->cond.expr_type.type != TYPE_AUTO)
    {
      is_if_expr = ast->cond.expr_type.type != TYPE_AUTO;
      is_if_expr_float = ast->cond.expr_type.IsFloat();
      is_if_expr_packed_float = ast->cond.expr_type.type == TYPE_VECTOR;
      is_if_expr_unsigned = IsUnsigned(ast->cond.expr_type.type);
      is_if_expr_reg_sz = GetTypeSize(&ast->cond.expr_type);
      is_if_expr = true;

    }

    bool is_stmnt_without_semicolon =
        ast->cond.scope->stats.back()->stmnt_without_semicolon;
    bool was_in_stmnt = lang_stat->ir_in_stmnt;
    if (is_stmnt_without_semicolon)
      lang_stat->ir_in_stmnt = false;
    /*

    int stmnt_without_semicolon_idx = 0;

    if(is_stmnt_without_semicolon)
            stmnt_without_semicolon_idx = IRCreateBeginBlock(lang_stat, out,
    IR_BEGIN_IF_EXPR_BLOCK, (void *)(long long)ast->line_number);
    */
    block2 *cond_true = CreateBlock(lang_stat, state);
    block2 *cond_false = CreateBlock(lang_stat, state);
    block2 *merge = CreateBlock(lang_stat, state);

    int stmnt_idx = 0;
    if (!lang_stat->ir_in_stmnt && !lang_stat->no_stmnt_of_conds)
      stmnt_idx = IRCreateBeginBlock(lang_stat, &state->cur_block->irs, IR_BEGIN_STMNT,
                                     (void *)(long long)ast->line_number);
    int on_idx = 0;
    //int if_idx = IRCreateBeginBlock(lang_stat, &state->cur_block->irs, IR_BEGIN_IF_BLOCK);

    bool has_elses = ast->cond.elses.size();

    int sub_if_idx = 0;

    
    if(ast->cond.from_on_ast)
    {
      push_tracker_stack(&state->break_end_block, merge);
    }
    GetIRCond2(lang_stat, ast->cond.cond, state, cond_true, cond_false);

    if (!lang_stat->ir_in_stmnt && !lang_stat->no_stmnt_of_conds)
      IRCreateEndBlock(lang_stat, stmnt_idx, &state->cur_block->irs, IR_END_STMNT);
    // GetIRFromAst(lang_stat, ast->cond.cond, out);

    EmitBlockMakeCurrent(lang_stat, state, cond_true);

    if (ast->cond.scope) {
      auto prev = lang_stat->no_stmnt_of_conds;
      lang_stat->no_stmnt_of_conds = false;
      lhs = GetIRFromAst2(lang_stat, ast->cond.scope, state, false);
      if(is_if_expr)
      {
        //HERE()
        ir.type = IR_BIN;
        ir.bin.op = T_EQUAL;
        ir.bin.lhs.type = IR_TYPE_REG;
        ir.bin.lhs.reg = 0;
        ir.bin.lhs.is_float = is_if_expr_float;
        ir.bin.lhs.is_packed_float = is_if_expr_packed_float;
        ir.bin.lhs.is_unsigned = is_if_expr_unsigned;
        ir.bin.lhs.reg_sz = is_if_expr_reg_sz;
        ir.bin.rhs = lhs;
        InsertIr(state, ir);

        ret = ir.bin.lhs;
      }
      //if (is_stmnt_without_semicolon)
        //GenIfExpr(lang_stat, ast->cond.scope->stats.back(), out, top);
      if(ast->cond.elses.size() > 0)
        EmitJmp(lang_stat, state, merge);
      lang_stat->no_stmnt_of_conds = prev;
    }

    EmitBlockMakeCurrent(lang_stat, state, cond_false);

    if(ast->cond.elses.size() > 0)
    {
      cond_true = CreateBlock(lang_stat, state);
      cond_false = CreateBlock(lang_stat, state);
    }

    int i = 0;

    FOR_VEC(el, ast->cond.elses) {
      ast_rep *e = *el;

      bool is_last = (i + 1) >= ast->cond.elses.size();

      if (!is_last)
      {
        //sub_if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_SUB_IF_BLOCK);
      }

      if (e->type == AST_ELSE_IF) {
        if (!lang_stat->no_stmnt_of_conds)
          stmnt_idx = IRCreateBeginBlock(lang_stat, &state->cur_block->irs, IR_BEGIN_STMNT,
                                         (void *)(long long)e->line_number);

        GetIRCond2(lang_stat, e->cond.cond, state, cond_true, cond_false);

        if (!lang_stat->no_stmnt_of_conds)
          IRCreateEndBlock(lang_stat, stmnt_idx, &state->cur_block->irs, IR_END_STMNT);

        EmitBlockMakeCurrent(lang_stat, state, cond_true);
      }

      lhs = GetIRFromAst2(lang_stat, e->cond.scope, state, false);
      if(is_if_expr)
      {
        ir.type = IR_BIN;
        ir.bin.op = T_EQUAL;
        ir.bin.lhs.type = IR_TYPE_REG;
        ir.bin.lhs.reg = 0;
        ir.bin.lhs.is_float = is_if_expr_float;
        ir.bin.lhs.is_packed_float = is_if_expr_packed_float;
        ir.bin.lhs.is_unsigned = is_if_expr_unsigned;
        ir.bin.lhs.reg_sz = is_if_expr_reg_sz;
        ir.bin.rhs = lhs;

        ret = ir.bin.lhs;
        InsertIr(state, ir);

      }


      //if (is_stmnt_without_semicolon)
        //GenIfExpr(lang_stat, e->cond.scope->stats.back(), out);

      auto prev_cond_false = cond_false;

      if (!is_last) {
        EmitJmp(lang_stat, state, merge);

        cond_true = CreateBlock(lang_stat, state);
        cond_false = CreateBlock(lang_stat, state);
      }
      EmitBlockMakeCurrent(lang_stat, state, prev_cond_false);


      i++;
    }
    if(ast->cond.from_on_ast)
    {
      pop_tracker_stack(&state->break_end_block);
    }

    if (is_stmnt_without_semicolon)
      lang_stat->ir_in_stmnt = was_in_stmnt;

    EmitBlockMakeCurrent(lang_stat, state, merge);
    ret.kind = IR_VAL_VALUE;
    return ret;
  } break;
  case AST_FUNC:
  {
    func_decl *last_func = lang_stat->cur_func;
    lang_stat->cur_func = ast->func.fdecl;

    ir.type = IR_STACK_BEGIN;
    ir.fdecl = ast->func.fdecl;
    // ir.num  = ast->func.fdecl->stack_size;
    ir.fdecl->biggest_call_args = 0;

    block2 *startb = CreateBlock(lang_stat, state);
    EmitBlock(lang_stat, state, startb);
    state->cur_block = startb;

    InsertIr(state, ir);

    FOR_VEC(arg, ast->func.fdecl->vars) {
      decl2 *a = *arg;
      if (a->type.type == TYPE_TEMPLATE)
        continue;
      ir.fdecl = ast->func.fdecl;
      if (IS_FLAG_OFF(a->flags, DECL_IS_ARG)) {
        ir.type = IR_DECLARE_LOCAL;
        ir.decl = a;
        InsertIr(state, ir);
      }
    }
    ir.type = IR_PROLOGUE_END;
    ir.fdecl = ast->func.fdecl;
    // ir.num  = ast->func.fdecl->stack_size;
    ir.fdecl->biggest_call_args = 0;
    InsertIr(state, ir);

    FOR_VEC(arg, ast->func.fdecl->vars) {
      decl2 *a = *arg;
      if (a->type.type == TYPE_TEMPLATE)
        continue;
      ir.fdecl = ast->func.fdecl;
      if (IS_FLAG_ON(a->flags, DECL_IS_ARG)) {
        ir.type = IR_DECLARE_ARG;

        ir.decl = a;
        InsertIr(state, ir);
      }
    }

    GetIRFromAst2(lang_stat, ast->func.stats, state, false);

    ir.type = IR_STACK_END;
    // ir.num  = ast->func.fdecl->stack_size;
    InsertIr(state, ir);
    lang_stat->cur_func = last_func;

    ast->func.fdecl->strct_constrct_size_per_statement = state->strct_ret_size_per_statement_max_gotten;
    ast->func.fdecl->total_of_var_args = state->var_args_max_gotten;
    return ret;
  }break;
  case AST_STR_LIT: {
    // InsertIntoDataSect(lang_stat, (void *))
    //
    int data_offset = lang_stat->data_sect.size();
    InsertIntoDataSect(lang_stat, ast->str.c_str(), ast->str.size() + 1);
    ir.type = IR_GET_STR_LIT;

    ir.bin.lhs.type = IR_TYPE_REG;
    ir.bin.lhs.reg_sz = 8;
    ir.bin.lhs.reg = GetAvailableReg(lang_stat);
    ir.bin.rhs.on_data_sect_offset = data_offset;
    InsertIr(state, ir);

    ret = ir.bin.lhs;
    ret.kind = IR_VAL_VALUE;
  } break;
  case AST_F64:
  {
    ir.type = IR_GET_FLOAT;
    ir.bin.lhs.type = IR_TYPE_REG;
    ir.bin.lhs.is_float = true;
    ir.bin.lhs.reg = AllocFloatReg(lang_stat);
    ir.bin.lhs.reg_sz = 8 ;
    ir.bin.rhs.f64 = ast->f64;
    InsertIr(state, ir);
    ret = ir.bin.lhs;
    ret.kind = IR_VAL_VALUE;
  }break;
  case AST_FLOAT:
  {
    ir.type = IR_GET_FLOAT;
    ir.bin.lhs.type = IR_TYPE_REG;
    ir.bin.lhs.is_float = true;
    ir.bin.lhs.kind = IR_VAL_VALUE;
    ir.bin.lhs.reg = AllocFloatReg(lang_stat);
    ir.bin.lhs.reg_sz = 4;
    ir.bin.rhs.f32 = ast->f32;

    InsertIr(state, ir);


    ret.kind = IR_VAL_VALUE;
    ret = ir.bin.lhs;
  }break;
  case AST_INT64:
  case AST_INT:
  case AST_CHAR:
  case AST_IDENT:
  case AST_REG:
  {
    GetIRVal(lang_stat, ast, &ret);
  }break;
  case AST_DEREF:
  {
    ast_rep *df = ast->deref.exp;

    rhs = GetIRFromAst2(lang_stat, df, state, is_lhs);

    rhs.deref += ast->deref.times;
    //if(rhs.kind == IR_VAL_ADDR)
      //rhs.deref += 1;
    if(!is_lhs)
    {
      if(rhs.type == IR_TYPE_REG)
        rhs.type = IR_TYPE_REG_MEM;
      EnsureValue(lang_stat, state, &rhs);
      LoadDerefs(lang_stat, &state->cur_block->irs, &rhs);
    }
    else
    {
      //rhs.kind = IR_VAL_ADDR;
      if(rhs.type == IR_TYPE_REG)
      {
        rhs.type = IR_TYPE_REG_MEM;
      }
    }
    rhs.ptr--;
    ret = rhs;
  }break;
  case AST_ADDRESS_OF:
  {
    ast_rep *addr = ast->ast;
    ret = GetIRFromAst2(lang_stat, addr, state, true);
    if(ret.kind == IR_VAL_ADDR)
    {
      ir.type = IR_ADDRESS_OF;
      if(ret.type == IR_TYPE_DECL && IS_FLAG_ON(ret.decl->flags, DECL_IS_GLOBAL))
      {
        ir.type = IR_GET_GLOBAL;
        ir.bin.lhs.ptr = 1;
      }
      ir.bin.lhs.type = IR_TYPE_REG;
      if(ret.type == IR_TYPE_REG || ret.type == IR_TYPE_REG_MEM )
      {
        ir.bin.lhs.reg = ret.reg;
      }
      else
        ir.bin.lhs.reg = GetAvailableReg(lang_stat);
      ir.bin.lhs.reg_sz = 8;
      ir.bin.rhs = ret;
      InsertIr(state, ir);

      ret = ir.bin.lhs;
    }
    if (ast->goes_onto_stack && ret.is_packed_float) 
    {
      ir.type = IR_BIN;
      ir.bin.op = T_EQUAL;
      ir.bin.lhs.type = IR_TYPE_ON_STACK;
      ir.bin.lhs.stack.on_stack_type = ON_STACK_STRUCT_CONSTR;
      ir.bin.lhs.stack.i = ast->at_stack_offset;
      ir.bin.lhs.reg_sz = 8;
      ir.bin.rhs = ret;

      ret = ir.bin.lhs;

      InsertIr(state, ir);
    }

    ret.kind = IR_VAL_VALUE;
    ret.reg_sz = 8;
    ret.ptr++;

  }break;
  case AST_CAST:
  {
    ast_rep *casted_ast = ast->cast.casted;
    ret = GetIRFromAst2(lang_stat, casted_ast, state, true);

    char prev = ast->cast.type.ptr;
    int cast_sz = GetTypeSize(&ast->cast.type, 0);
    if(ast->cast.type.type == TYPE_VOID && ast->cast.type.ptr > 0)
      cast_sz = 8;
    if(ast->cast.type.type == TYPE_VECTOR)
      cast_sz = ast->cast.type.vec_type;


    //ret.reg_sz = cast_sz;

    //BREAK(ast->line_number == 43)
    if(ret.type == IR_TYPE_DECL && ret.decl->type.type == TYPE_STATIC_ARRAY)
    {
      ir.type = IR_ADDRESS_OF;
      ir.bin.lhs.type = IR_TYPE_REG;
      ir.bin.lhs.reg_sz = 8;
      ir.bin.lhs.reg = GetAvailableReg(lang_stat);
      ir.bin.rhs = ret;
      InsertIr(state, ir);
      ret = ir.bin.lhs;
      ret.kind = IR_VAL_VALUE;
    }
    else if(ast->cast.type.ptr > 0)
    {
      //ret.kind = IR_VAL_ADDR;
      if(cast_sz > 8)
        cast_sz = 8;
    }
    else
    {
      char prev_sz = ret.reg_sz;

      bool from_float = ret.is_float;
      bool to_float = ast->cast.type.IsFloat();

      //BREAK(ast->line_number == 425)
      bool casted = false;
      if (ret.ptr > 0 && ast->cast.type.ptr == 0) {
        // Need the pointed value
        if (ret.kind == IR_VAL_ADDR)
          EnsureValue(lang_stat, state, &ret); // load pointer variable

        LoadDerefs(lang_stat, &state->cur_block->irs, &ret);
      }
      if (ast->cast.type.ptr == 0) {
        if(cast_sz == 4 && ret.reg_sz == 8)
        {
          casted = true;
          ret.reg_sz = 4;
        }
        EnsureValue(lang_stat, state, &ret);
      }
      
      //ret.kind = IR_VAL_VALUE;
      //if(ret)


      if(ret.ptr == 0 && !casted)
      {
        if(from_float && to_float && cast_sz != ret.reg_sz)
        {
          ir.type = IR_CAST_FLOAT_TO_FLOAT;
          ir.bin.lhs = ret;
          ir.bin.rhs = ret;
          ir.bin.rhs.reg_sz = ret.reg_sz;
          ir.bin.lhs.reg_sz = cast_sz;

          InsertIr(state, ir);

          ret.kind = IR_VAL_VALUE;
        }
        else if(!from_float && to_float)
        {
          ir.type = IR_CAST_INT_TO_FLOAT;
          ir.bin.lhs.type = IR_TYPE_REG;
          ir.bin.lhs.reg = AllocFloatReg(lang_stat);
          ir.bin.lhs.reg_sz = cast_sz;
          ir.bin.rhs = ret;
          InsertIr(state, ir);

          ret = ir.bin.lhs;
          ret.kind = IR_VAL_VALUE;
        }
        else if(!from_float && !to_float && !(ret.reg_sz == 8 && cast_sz == 8))
        {
          ir.type = IR_CAST_INT_TO_INT;
          ir.bin.lhs.type = IR_TYPE_REG;
          if(ret.type == IR_TYPE_REG)
            ir.bin.lhs.reg = ret.reg;
          else
            ir.bin.lhs.reg = GetAvailableReg(lang_stat);
          ir.bin.lhs.reg_sz = cast_sz;
          ir.bin.rhs = ret;
          InsertIr(state, ir);

          ret = ir.bin.lhs;
          ret.reg = ir.bin.lhs.reg;
          ret.kind = IR_VAL_VALUE;
        }
        else if(from_float && !to_float)
        {
          ir.type = IR_CAST_FLOAT_TO_INT;
          ir.bin.lhs.type = IR_TYPE_REG;
          ir.bin.lhs.reg = GetAvailableReg(lang_stat);
          ir.bin.lhs.reg_sz = cast_sz;
          ir.bin.rhs = ret;
          InsertIr(state, ir);

          ret = ir.bin.lhs;
          ret.kind = IR_VAL_VALUE;
        }
      }
    }

    ir.bin.lhs.is_unsigned = IsUnsigned(ast->cast.type.type);
    ir.bin.lhs.is_float = ast->cast.type.IsFloat();
    ir.bin.lhs.is_packed_float = ast->cast.type.type == TYPE_VECTOR;

    bool was_float = ret.is_float;

    ret.is_float = ir.bin.lhs.is_float;
    ret.is_packed_float = ir.bin.lhs.is_packed_float;
    ret.is_unsigned = ir.bin.lhs.is_unsigned;
    ret.reg_sz = cast_sz;
    ret.ptr = ast->cast.type.ptr;

  }break;
  case AST_STATS:
  {
    FOR_VEC(st, ast->stats) {
      FreeRegs2(lang_stat);
      ast_rep *s = *st;
      clear_strct_ret(state);
      if (s->type == AST_EMPTY)
        continue;

      if (s->type == AST_WHILE || s->type == AST_IF) {
        GetIRFromAst2(lang_stat, s, state, false);
      } else {
        ASSERT(!lang_stat->ir_in_stmnt)
        lang_stat->ir_in_stmnt = true;

        bool can_emplace_stmnt = s->type != AST_IDENT && s->type != AST_FOR &&
                                 !s->dont_make_dbg_stmnt;
        int stmnt_idx = 0;
        if (can_emplace_stmnt)
          stmnt_idx = IRCreateBeginBlock(lang_stat, &state->cur_block->irs, IR_BEGIN_STMNT,
                                         (void *)(long long)s->line_number);

        ret = GetIRFromAst2(lang_stat, s, state, false);

        if (can_emplace_stmnt)
          IRCreateEndBlock(lang_stat, stmnt_idx, &state->cur_block->irs, IR_END_STMNT);

        lang_stat->ir_in_stmnt = false;
      }

      int cur_sz = lang_stat->cur_func->strct_ret_size_per_statement;
      lang_stat->cur_func->strct_ret_size_per_statement =
          max(cur_sz, lang_stat->cur_strct_ret_size_per_statement);
      lang_stat->cur_strct_ret_size_per_statement = 0;

      int i = 0;
    }

    //ret.reg_sz = 1;
    return ret;
  }break;
  case AST_NEGATIVE:
  {
    lhs = GetIRFromAst2(lang_stat, ast->ast, state, false);
    if(lhs.kind != IR_VAL_VALUE)
    {
      EnsureValue(lang_stat, state, &lhs);
      LoadDerefs(lang_stat, &state->cur_block->irs, &lhs);
    }

    if(lhs.is_float)
    {
      //HERE()
      if(lhs.reg_sz == 4)
      {
        rhs = GetFloatIr(lang_stat, state, -1.0);
      }
      else
      {
        rhs = GetDoubleIr(lang_stat, state, -1.0);
      }
      char rhs_reg = rhs.reg;

      if(lhs.is_packed_float)
      {
        ir.type = IR_FILL;
        ir.bin.lhs.type = IR_TYPE_REG;
        ir.bin.lhs.reg = rhs_reg;
        ir.bin.lhs.is_float = true;
        ir.bin.lhs.is_packed_float = true;
        ir.bin.lhs.reg_sz = lhs.reg_sz;
        ir.bin.rhs = rhs;


        InsertIr(state, ir);
        rhs = ir.bin.lhs;

        ir.type = IR_BIN;
        ir.bin.op = T_MUL;
        ir.bin.lhs = lhs;
        ir.bin.rhs = rhs;
        InsertIr(state, ir);

        //lhs = rhs;
        //FreeSpecificFloatReg(lang_stat, reg);
      }
      else
      {
        ir.type = IR_BIN;
        ir.bin.op = T_MUL;
        ir.bin.lhs = lhs;
        ir.bin.rhs = rhs;
        InsertIr(state, ir);
      }
      FreeSpecificFloatReg(lang_stat, rhs_reg);
    }
    else
    {
      ir.type = IR_BIN;
      ir.bin.op = T_MUL;
      ir.bin.lhs = lhs;
      ir.bin.rhs.type = IR_TYPE_INT;
      ir.bin.rhs.i = -1;
      InsertIr(state, ir);
    }
    ret = lhs;
    
  }break;
  case AST_MINUS_MINUS:
  case AST_PLUS_PLUS:
  {
    lhs = GetIRFromAst2(lang_stat, ast->unop_assign.ast, state, true);

    ir.type = IR_BIN;
    tkn_type2 tp;
    switch(ast->type)
    {
    case AST_PLUS_PLUS: tp = T_PLUS; break;
    case AST_MINUS_MINUS: tp = T_MINUS; break;
    default:ASSERT(false)
    }
    ir.bin.op = tp;
    ir.bin.lhs = lhs;
    ir.bin.rhs.type = IR_TYPE_INT;
    ir.bin.rhs.i = 1;

    InsertIr(state, ir);
    ret = lhs;
    
  }break;
  case AST_DBG_BREAK: {
    if (lang_stat->release)
      ir.type = IR_NOP;
    else {
      // int stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT,
      // (void *)(long long)ast->line_number);
      ir.type = IR_DBG_BREAK;
      // IRCreateEndBlock(lang_stat, stmnt_idx, out, IR_END_STMNT);
    }
    InsertIr(state, ir);
    return ret;
  } break;
  case AST_OPPOSITE:
  {
    GetIRCondValue(lang_stat, ast->ast, state, 0, 1);
  }break;
  case AST_ARRAY_COSTRUCTION:
  {
    int offset = ast->ar_constr.at_offset;
    int tp_sz = GetTypeSize(&ast->ar_constr.type);
    // top->i = offset;

    // ir.assign.to_assign.i = offset;
    int len = ast->ar_constr.commas.size();

    for (u32 i = 0; i < len; i++)
    {

      lhs = GetIRFromAst2(lang_stat, ast->ar_constr.commas[i], state, false);
      ir.type = IR_BIN;
      ir.bin.op = T_EQUAL;
      ir.bin.lhs.type = IR_TYPE_ON_STACK;
      ir.bin.lhs.is_unsigned = lhs.is_unsigned;
      ir.bin.lhs.stack.on_stack_type = ON_STACK_STRUCT_CONSTR;
      ir.bin.lhs.stack.i = offset + tp_sz * i;
      ir.bin.lhs.reg_sz = tp_sz;
      ir.bin.lhs.is_float = lhs.is_float;
      ir.bin.rhs = lhs;

      InsertIr(state, ir);

      if(lhs.type == IR_TYPE_REG || lhs.type == IR_TYPE_REG_MEM)
      {
        FreeReg(lang_stat, lhs.reg, lhs.is_float);
      }
    }
    ret.type = IR_TYPE_ON_STACK;
    ret.is_float = ast->ar_constr.type.IsFloat();
    ret.stack.on_stack_type = ON_STACK_STRUCT_CONSTR;
    ret.stack.i = offset;
    ret.reg_sz = ir.bin.lhs.reg_sz;
    ret.voffset = 0;
    ret.is_packed_float = ast->strct_constr.is_vector;
    ret.kind = IR_VAL_ADDR;
    if(ret.is_packed_float)
    {
      ir.type = IR_BIN;
      ir.bin.op = T_EQUAL;
      ir.bin.lhs.type = IR_TYPE_REG;
      ir.bin.lhs.reg = AllocFloatReg(lang_stat);
      ir.bin.lhs.reg_sz = ast->strct_constr.vec_type;
      ir.bin.rhs = ret;

      InsertIr(state, ir);
      ret = ir.bin.lhs;

      ret.kind = IR_VAL_VALUE;

    }

  }break;
  case AST_STRUCT_COSTRUCTION:
  {
    int offset = ast->strct_constr.at_offset;
    // top->i = offset;

    int cur_offset = offset;
    // ir.assign.to_assign.i = offset;
      // for (int i = 0; i < e->strct_constr.commas.size(); i++)
    for (int i = ast->strct_constr.commas.size() - 1; i >= 0; i--) 
    {
      ast_struct_construct_info *cinfo = &ast->strct_constr.commas[i];
      //HERE()
      lhs = GetIRFromAst2(lang_stat, cinfo->exp, state, false);
      ir.type = IR_BIN;
      ir.bin.op = T_EQUAL;
      ir.bin.lhs.type = IR_TYPE_ON_STACK;
      ir.bin.lhs.is_unsigned = lhs.is_unsigned;
      ir.bin.lhs.stack.on_stack_type = ON_STACK_STRUCT_CONSTR;
      ir.bin.lhs.stack.i = cur_offset + cinfo->var->offset;
      ir.bin.lhs.reg_sz = GetTypeSize(&cinfo->var->type);
      ir.bin.lhs.is_float = lhs.is_float;
      ir.bin.rhs = lhs;

      InsertIr(state, ir);

      if(lhs.type == IR_TYPE_REG || lhs.type == IR_TYPE_REG_MEM)
      {
        FreeReg(lang_stat, lhs.reg, lhs.is_float);
      }
    }
    BREAK(ast->line_number == 768)
    ret.type = IR_TYPE_ON_STACK;
    ret.is_float = ast->strct_constr.is_vector;
    ret.stack.on_stack_type = ON_STACK_STRUCT_CONSTR;
    ret.stack.i = offset;
    ret.reg_sz = ir.bin.lhs.reg_sz;
    ret.voffset = 0;
    ret.is_packed_float = ast->strct_constr.is_vector;
    if(ret.is_packed_float)
    {
      ret.reg_sz = ast->strct_constr.vec_type;
      ret.ptr = 0;

    }
    ret.kind = IR_VAL_ADDR;
  }break;
  case AST_BINOP:
  {
    switch(ast->op)
    {
    case T_COND_EQ:
    case T_COND_NE:
    case T_LESSER_THAN:
    case T_LESSER_EQ:
    case T_GREATER_THAN:
    case T_GREATER_EQ:
    case T_COND_AND:
    case T_COND_OR:
    {
      GetIRCondValue(lang_stat, ast, state, 1, 0);

      ret.type = IR_TYPE_REG;
      ret.reg = 0;
      ret.ptr = 0;
      ret.deref = 0;
      ret.reg_sz = 8;
      ret.voffset = 0;
      ret.kind = IR_VAL_VALUE;

    }break;
    case T_POINT:
    {
      lhs = GetIRFromAst2(lang_stat, ast->points[0].exp, state, true);

      /*
      if(lhs.ptr == 0 && lhs.type == IR_TYPE_DECL && IS_FLAG_ON(lhs.decl->flags, DECL_IS_GLOBAL))
      {
        ir.type = IR_GET_GLOBAL;
        ir.bin.lhs.type = IR_TYPE_REG;
        ir.bin.lhs.reg = GetAvailableReg(lang_stat);
        ir.bin.lhs.reg_sz = 8;
        ir.bin.lhs.ptr = 1;
        ir.bin.rhs = lhs;
        InsertIr(state, ir);
        lhs = ir.bin.lhs;
        lhs.type = IR_TYPE_REG_MEM;
        lhs.ptr = 0;
        lhs.deref = 0;

      }
      */
      int offset = 0;
      bool is_static_array;
      for(int i =1; i < ast->points.size();i++)
      {
        is_static_array = false;
        decl2 *d = ast->points[i].decl_strct;

        if(lhs.ptr > 0)
        {
          bool prev_float = lhs.is_float;
          bool prev_is_packed = lhs.is_packed_float;
          lhs.is_float = false;
          lhs.is_packed_float = false;

          EnsureValue(lang_stat, state, &lhs);

          lhs.is_float = prev_float;
          lhs.is_packed_float = prev_is_packed;

          lhs.deref = lhs.ptr - 1;
          LoadDerefs(lang_stat, &state->cur_block->irs, &lhs);


          offset = 0;
          lhs.ptr = d->type.ptr;
        }
        offset += d->offset;

        type2 *tp = &d->type;
        if(d->type.type == TYPE_STATIC_ARRAY)
        {
          is_static_array = true;
          lhs.reg_sz = GetTypeSize(d->type.tp);
          tp = d->type.tp;
        }
        else
          lhs.reg_sz = GetTypeSize(&d->type);
        
        if(d->type.type == TYPE_VECTOR)
          lhs.reg_sz = d->type.vec_type;

        lhs.voffset = offset;
        lhs.is_float = tp->IsFloat();
        lhs.is_packed_float = tp->type == TYPE_VECTOR;
        lhs.deref = 0;
      }
      if(is_static_array)
      {
        char reg_sz = lhs.reg_sz;
        bool was_float = lhs.is_float;
        bool was_packed = lhs.is_packed_float;

        ir.type = IR_ADDRESS_OF;
        ir.bin.lhs.type = IR_TYPE_REG;
        ir.bin.lhs.reg_sz = 8;
        ir.bin.lhs.reg = GetAvailableReg(lang_stat);
        ir.bin.rhs = lhs;

        InsertIr(state, ir);

        lhs = ir.bin.lhs;

        lhs.kind = IR_VAL_VALUE;
        lhs.deref = 0;
        lhs.type = IR_TYPE_REG;
        lhs.is_float = was_float;
        lhs.is_packed_float = was_packed;
        lhs.reg_sz = reg_sz;
        return lhs;
      }
      else
        lhs.kind = IR_VAL_ADDR;
      if(!is_lhs)
      {
        char prev_reg = lhs.reg;
        LoadDerefs(lang_stat, &state->cur_block->irs, &lhs);
        /*
        if(lhs.is_float)
        {
          ir.type = IR_BIN;
          ir.bin.op = T_EQUAL;
          ir.bin.lhs.type = IR_TYPE_REG;
          ir.bin.lhs.reg = AllocFloatReg(lang_stat);
          ir.bin.lhs.is_float = true;
          ir.bin.lhs.reg_sz = lhs.reg_sz;
          ir.bin.lhs.is_packed_float = lhs.is_packed_float;
          ir.bin.rhs = lhs;
          lhs = ir.bin.lhs;
          InsertIr(state, ir);
        }
        else
        */
          EnsureValue(lang_stat, state, &lhs);
        if(lhs.is_float)
        {
          FreeReg(lang_stat, prev_reg, false);
        }
        lhs.voffset = 0;
      }
      else
      {
        if(lhs.type == IR_TYPE_REG)
          lhs.type = IR_TYPE_REG_MEM;
      }
      //BREAK(ast->line_number == 519)
      ret = lhs;
    }break;
    case T_MUL:
    case T_PIPE:
    case T_DIV:
    case T_PERCENT:
    case T_AMPERSAND:
    case T_MINUS:
    case T_SHIFT_LEFT:
    case T_HAT:
    case T_SHIFT_RIGHT:
    case T_PLUS:
    {
      //BREAK(ast->line_number == 612)
      u32 spill_base = state->spilled_regs.cur;

      if(ast->op == T_DIV)
      {
        CheckRegInUseMaybeSpill(lang_stat, state, (char)regs_enum::RAX);
        CheckRegInUseMaybeSpill(lang_stat, state, (char)regs_enum::RDX);
      }

      lhs = GetIRFromAst2(lang_stat, ast->e_holder.expr[0], state, false);

      for(int i= 1; i < ast->e_holder.expr.size(); i++)
      {
        rhs = GetIRFromAst2(lang_stat, ast->e_holder.expr[i], state, false);

        char min_rhs_deref = rhs.deref - 1;
        if(IsIrValLiteral(lhs.type)) min_rhs_deref++;

        if(lhs.kind != IR_VAL_VALUE)
        {
          char prev_reg = LoadDerefs(lang_stat, &state->cur_block->irs, &lhs);
          EnsureValue(lang_stat, state, &lhs);
          if(lhs.is_float && prev_reg != -1)
          {
            FreeReg(lang_stat, prev_reg, false);
          }
        }

        if((rhs.type == IR_TYPE_REG_MEM || rhs.type == IR_TYPE_REG) && (lhs.type != IR_TYPE_INT))
        {
          FreeReg(lang_stat, rhs.reg, rhs.is_float);
        }

        if(IsIrValLiteral(lhs.type) && !IsIrValLiteral(rhs.type))
        {
          // swap rhs and lhs, rhs should be a value, so this should be safe, we wouldnt be modifying any memory
          if(ast->op == T_PLUS || ast->op == T_MUL || ast->op == T_MINUS)
          {
            EnsureValue(lang_stat, state, &rhs);
            ASSERT(rhs.kind == IR_VAL_VALUE)
            auto aux = lhs;
            lhs = rhs;
            rhs = aux;
          }
          else
          {
            ASSERT(false)
          }
        }
        else if(IsIrValLiteral(lhs.type) && IsIrValLiteral(rhs.type))
        {
          ir.type = IR_BIN;
          ir.bin.op = T_EQUAL;
          ir.bin.lhs.type = IR_TYPE_REG;
          ir.bin.lhs.reg = GetAvailableReg(lang_stat);
          ir.bin.lhs.reg_sz = 4;
          ir.bin.rhs = lhs;

          lhs = ir.bin.lhs;

          InsertIr(state, ir);
        }
        if(lhs.is_packed_float && rhs.is_float && !rhs.is_packed_float)
        {
          EnsureValue(lang_stat, state, &rhs);
          ir.type = IR_FILL;
          ir.bin.lhs.type = IR_TYPE_REG;
          ir.bin.lhs.reg = AllocFloatReg(lang_stat);
          ir.bin.lhs.is_float = true;
          ir.bin.lhs.is_packed_float = true;
          ir.bin.lhs.reg_sz = lhs.reg_sz;
          ir.bin.rhs = rhs;

          InsertIr(state, ir);

          rhs = ir.bin.lhs;
        }

        ret = lhs;

        ir.type = IR_BIN;
        ir.bin.op = ast->op;
        ir.bin.lhs = lhs;
        ir.bin.rhs = rhs;

        InsertIr(state, ir);
      }
      ret.kind = IR_VAL_VALUE;

      while(state->spilled_regs.cur > spill_base)
      {
        ir_val unspill = *pop_tracker_stack(&state->spilled_regs);
        UnspillReg(state, &unspill);
      }
    }break;
    case T_PLUS_EQUAL:
    case T_EQUAL:
    {
      rhs = GetIRFromAst2(lang_stat, ast->e_holder.expr[1], state, false);

      lhs = GetIRFromAst2(lang_stat, ast->e_holder.expr[0], state, true);
      bool rhs_wal_value = rhs.kind == IR_VAL_VALUE;

      if(rhs.ptr > 0)
      {
        lhs.is_float = false;
        lhs.is_packed_float = false;
      }
      if(lhs.deref > 0)
      {
        char prev_sz = lhs.reg_sz;
        lhs.reg_sz = 8;
        if(lhs.kind == IR_VAL_ADDR)
          EnsureValue(lang_stat, state, &lhs);
        lhs.reg_sz = prev_sz;

        lhs.deref--;
        LoadDerefs(lang_stat, &state->cur_block->irs, &lhs);

        lhs.type = IR_TYPE_REG_MEM;
      }
      if(lhs.ptr > 0)
      {
        lhs.reg_sz = 8;
        rhs.reg_sz = 8;
        lhs.is_float = false;
        lhs.is_unsigned = true;
        rhs.is_unsigned = true;
      }
      if(rhs.kind != IR_VAL_VALUE)
      {
        char prev_sz = rhs.reg_sz;
        rhs.reg_sz = 8;
        if(rhs.kind == IR_VAL_ADDR)
          EnsureValue(lang_stat, state, &rhs);
        rhs.reg_sz = prev_sz;
        LoadDerefs(lang_stat, &state->cur_block->irs, &rhs);

      }

      if(lhs.is_packed_float && rhs.type == IR_TYPE_INT)
      {
        if(rhs.i == 0)
        {
          ir.type = IR_CLEAR_SSE;
          ir.i = AllocFloatReg(lang_stat);
          ir.i |= lhs.reg_sz << 8;
          rhs.type = IR_TYPE_REG;
          rhs.reg = ir.i;
          rhs.reg_sz = lhs.reg_sz;
          rhs.is_float = true;
          rhs.is_packed_float = true;

          InsertIr(state, ir);
        }
        else
          ASSERT(false)
      }

      if(rhs.type == IR_TYPE_REG_MEM)
      {
        rhs.type = IR_TYPE_REG;
      }

      if(lhs.type == IR_TYPE_DECL && IS_FLAG_ON(lhs.decl->flags, DECL_IS_GLOBAL))
      {
        ir.type = IR_GET_GLOBAL;
        ir.bin.lhs.type = IR_TYPE_REG;
        ir.bin.lhs.reg = GetAvailableReg(lang_stat);
        ir.bin.lhs.reg_sz = 8;
        ir.bin.lhs.ptr = 1;
        ir.bin.rhs = lhs;
        InsertIr(state, ir);

        char prev_sz = lhs.reg_sz;
        int prev_offset = lhs.voffset;

        lhs = ir.bin.lhs;

        lhs.type = IR_TYPE_REG_MEM;
        lhs.reg_sz = prev_sz;
        lhs.voffset = prev_offset;

      }
      switch(ast->op)
      {
      case T_EQUAL:
      {
        MakeIrStore(lang_stat, &lhs, &rhs, &ir);

        InsertIr(state, ir);

      }break;
      case T_PLUS_EQUAL:
      {
        if(lhs.is_float)
        {
          ir_val aux = lhs;
          EnsureValue(lang_stat, state, &aux);
          LoadDerefs(lang_stat, &state->cur_block->irs, &aux);
          ir.type = IR_BIN;
          ir.bin.op = T_PLUS;
          ir.bin.lhs = aux;
          ir.bin.rhs = rhs;

          InsertIr(state, ir);

          MakeIrStore(lang_stat, &lhs, &aux, &ir);

          InsertIr(state, ir);
        }
        else
        {
          ir.type = IR_BIN;
          ir.bin.op = T_PLUS;
          ir.bin.lhs = lhs;
          ir.bin.rhs = rhs;

          InsertIr(state, ir);
        }
      }break;
      }
      ret = lhs;
    }break;
    default: ASSERT(false)
    }
  }break;
  case AST_BREAK: 
  {
    block2 *top = *top_tracker_stack(&state->break_end_block);
    ASSERT(top)
    EmitJmp(lang_stat, state, top);
    return ret;
  }break;
  case AST_CONTINUE: 
  {
    block2 *top = *top_tracker_stack(&state->continue_start_block);
    ASSERT(top)

    EmitJmp(lang_stat, state, top);
    return ret;
  } break;
  default: ASSERT(false)
  }
  ASSERT(ret.reg_sz != 0)
  ASSERT(ret.type != IR_TYPE_NONE)
  return ret;

}
void SetIrStart(ir_rep *ir, int new_start)
{
  ir->start = new_start;
}
void SetIrEnd(ir_rep *ir, int new_end)
{
  ir->end = new_end;
}

