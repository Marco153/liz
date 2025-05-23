#include "IR.h"

bool IsAstSimple(lang_state *lang_stat, ast_rep *ast);
decl2* PointLogic(lang_state* lang_stat, node* n, scope* scp, type2* ret_tp);
bool NameFindingGetType(lang_state* lang_stat, node* n, scope* scp, type2& ret_type, int);
void GenStackThenIR(lang_state* lang_stat, ast_rep* ast, own_std::vector<ir_rep>* out, ir_val* dst_val, ir_val *i=nullptr);
void CreateOppositeRegAssigmentAfterCondChecking(lang_state* lang_stat, own_std::vector<ir_rep>* out, int sub_if_idx, int if_idx, int reg);
bool IsNodeOperator(node* nd, tkn_type2 tkn);


int clamp(int, int, int);

#define MAKE_DST_IR_VAL(ir_tp, ptr) (((short)ir_tp) | (((int)ptr)<<16))
#define MAKE_DST_IR_VAL(ir_tp, ptr) (((short)ir_tp) | (((int)ptr)<<16))

#define IR_VAL_FROM_POINT 0x100
#define IR_VAL_ALREADY_ON_PARAM_REG 0x200
#define IR_VAL_FROM_DEREF 0x400
#define IR_VAL_FROM_POINT_BUT_IS_STATIC_ARRAY 0x800


#define IR_BEGIN_ARG 25
#define IR_END_ARG 26
#define IR_BEGIN_AND 20
#define IR_END_AND 21
#define IR_BEGIN_OR 22
#define IR_END_OR 23
#define IR_BEGIN_INSIDE_IF 24
#define IR_END_INSIDE_IF 25

bool CheckIrValIsPointIncDeref(ir_val *val)
{
	if (IS_FLAG_ON(val->reg_ex, IR_VAL_FROM_POINT) && val->type != IR_TYPE_DECL && val->deref < 0)
	{
		val->deref++;
	}
	return false;
}
ast_rep *NewAst()
{
	auto ret = (ast_rep*)__lang_globals.alloc(__lang_globals.data, sizeof(ast_rep));
    memset(ret, 0, sizeof(ast_rep));
    return ret;

}


bool CheckStmntWithoutSemicolon(lang_state* lang_stat, own_std::vector<ast_rep *>* ar)
{
	FOR_VEC(stmnt, (*ar))
	{
		ast_rep* st = *stmnt;
		bool is_stmnt_without_semicolon = st->stmnt_without_semicolon;
		if (is_stmnt_without_semicolon)
		{
			// stmnt without semicolon should be at the end of the scope, because they are like a return
			ASSERT((stmnt + 1) == ar->end());
			return true;
		}

	}
	return false;
}

ast_rep* CreateAstBin(lang_state* lang_stat, tkn_type2 op, ast_rep *lhs, ast_rep *rhs, node *lhs_n, scope *scp)
{
	ast_rep* ret = NewAst();
	ret->type = AST_BINOP;
	ret->op = op;
	ret->lhs_tp = DescendNode(lang_stat, lhs_n, scp);
	ret->e_holder.expr.emplace_back(lhs);
	ret->e_holder.expr.emplace_back(rhs);

	return ret;
}
void InsertDeferd(ast_rep **out, scope *scp, bool recursive)
{
	auto start_func = scp->fdecl;
	while(scp && scp->fdecl == start_func)
	{
		if (scp->defered.size() > 0)
		{
			if ((*out)->type != AST_STATS)
			{
				ast_rep* new_ret = NewAst();
				new_ret->type = AST_STATS;
				INSERT_VEC(new_ret->stats, scp->defered);
				new_ret->stats.emplace_back(*out);
				*out = new_ret;
			}
			else
			{
				INSERT_VEC((*out)->stats, scp->defered);
			}
		}
		if (!recursive)
			return;
		scp = scp->parent;
	}
}
ast_rep *AstFromNode(lang_state *lang_stat, node *n, scope *scp)
{
    ast_rep *ret = NewAst();
	ret->line_number = n->t->line;
	ret->line_offset_start = n->t->line_offset;
	type2 dummy_type;
    switch(n->type)
    {
	case N_TYPE:
	{
		ret->type = AST_TYPE;
		ret->tp = n->decl_type;
	}break;
	case node_type::N_UNOP:
	{
		switch (n->t->type)
		{
		case T_MINUS_MINUS:
		{
			ret->type = AST_MINUS_MINUS;
			ret->unop_assign.tp = DescendNode(lang_stat, n->r, scp);
			ret->unop_assign.ast = AstFromNode(lang_stat, n->r, scp);
		}break;
		case T_PLUS_PLUS:
		{
			ret->type = AST_PLUS_PLUS;
			ret->unop_assign.tp = DescendNode(lang_stat, n->r, scp);
			ret->unop_assign.ast = AstFromNode(lang_stat, n->r, scp);
		}break;
		case T_EXCLAMATION:
		{
			ret->type = AST_OPPOSITE;
			ret->ast = AstFromNode(lang_stat, n->r, scp);
		}break;
		case  T_TILDE:
		{
			ret->type = AST_NEGATE;
			ret->ast = AstFromNode(lang_stat, n->r, scp);
		}break;
		case  T_MINUS:
		{
			ret->type = AST_NEGATIVE;
			ret->ast = AstFromNode(lang_stat, n->r, scp);
		}break;
		case  T_AMPERSAND:
		{
			ret->type = AST_ADDRESS_OF;
			ret->ast = AstFromNode(lang_stat, n->r, scp);
			type2* tp = &ret->ast->lhs_tp;
			if(tp->type == TYPE_VECTOR && !CMP_NTYPE_BIN(n->r, T_POINT))
			{
				ret->goes_onto_stack = true;
				ret->at_stack_offset = lang_stat->cur_strct_constrct_size_per_statement;

				lang_stat->cur_strct_constrct_size_per_statement += 16;

				int cur_sz = lang_stat->cur_func->strct_constrct_size_per_statement;
				lang_stat->cur_func->strct_constrct_size_per_statement = max(cur_sz, lang_stat->cur_strct_constrct_size_per_statement);
			}
		}break;
		case T_MUL:
		{
			ret->type = AST_DEREF;
			node* cur = n;
			while (cur->type == N_UNOP && n->t->type == T_MUL)
			{
				ret->deref.times++;
				cur = cur->r;
			}
			ret->deref.exp = AstFromNode(lang_stat, cur, scp);
			ret->deref.type = DescendNode(lang_stat, n, scp);
		}break;
		case T_DOLLAR:
		{
			ret->type = AST_LABEL;
			ret->decl = FindIdentifier(n->r->t->str, scp, &dummy_type);
		}break;
		default:
			ASSERT(0)
		}
	}break;
	case node_type::N_CAST:
    {
        ret->type = AST_CAST;
		NameFindingGetType(lang_stat, n->l, scp, ret->cast.type);
		ret->cast.type.type = FromTypeToVarType(ret->cast.type.type);
        ret->cast.casted = AstFromNode(lang_stat, n->r, scp);
        //ret->cast.type = AstFromNode(lang_stat, n->l, scp);
    }break;
	case node_type::N_INDEX:
    {
        ret->type = AST_INDEX;
		ret->index.type = AST_INDEX_TP_NORMAL;
		ret->index.lhs_type = DescendNode(lang_stat, n->l, scp);
		ret->index.lhs = AstFromNode(lang_stat, n->l, scp);
		ret->index.rhs = AstFromNode(lang_stat, n->r, scp);
    }break;
	case node_type::N_ELSE:
    {
        ret->type = AST_ELSE;
        ret->cond.scope = AstFromNode(lang_stat, n->r, scp);
    }break;
	case node_type::N_ELSE_IF:
    {
        ret->type = AST_ELSE_IF;
        ret->cond.cond = AstFromNode(lang_stat, n->l->l, scp);
        ret->cond.scope = AstFromNode(lang_stat, n->l->r, scp);
    }break;
	case node_type::N_WHILE:
    {
        ret->type = AST_WHILE;
        ret->loop.cond = AstFromNode(lang_stat, n->l, scp);
        ret->loop.scope = AstFromNode(lang_stat, n->r, scp);
    }break;
	case node_type::N_IF:
    {
        ret->type = AST_IF;

        ret->cond.cond = AstFromNode(lang_stat, n->l->l, scp);

        ret->cond.scope = AstFromNode(lang_stat, n->l->r, scp);
        node *cur = n->r;
		while (cur && (cur->type == N_ELSE_IF || cur->type == N_ELSE))
		{
			ret->cond.elses.emplace_back(AstFromNode(lang_stat, cur, scp));
			cur = cur->r;
		}


		if (CheckStmntWithoutSemicolon(lang_stat, &ret->cond.scope->stats))
			GetLastStmntType(lang_stat, n->l->r->r, scp, ret->cond.expr_type);

		FOR_VEC(cond, ret->cond.elses)
		{
			ast_rep* c = *cond;
			CheckStmntWithoutSemicolon(lang_stat, &c->cond.scope->stats);
		}


    }break;
	case node_type::N_BINOP:
    {
		if (n->r->type == N_QUESTION_MARK)
			return nullptr;
		//ret->t = n->l->t;
		if(n->r->type == N_SCOPE)
            return AstFromNode(lang_stat, n->r, scp);
        if(n->t->type == T_COLON || n->r->type == N_SCOPE)
            return AstFromNode(lang_stat, n->l, scp);

        ret->type = AST_BINOP;
        ret->op = n->t->type;

		own_std::vector<node*> node_stack;
		node* cur_node = n;
        node_stack.emplace_back(n);

        // putting all equal operators in a single array
		while (cur_node->l && cur_node->l->type == node_type::N_BINOP && cur_node->l->t->type == n->t->type) 
		{
			node_stack.emplace_back(cur_node->l);
			cur_node = cur_node->l;
		}
		int last_idx = node_stack.size() - 1;
		node *cur = *(node_stack.begin() + last_idx);

		ret->lhs_tp = DescendNode(lang_stat, cur->l, scp);

		ast_rep *lhs = AstFromNode(lang_stat, cur->l, scp);
		lhs->lhs_tp = ret->lhs_tp;
		ast_rep *rhs = AstFromNode(lang_stat, cur->r, scp);
		if(!CMP_NTYPE_BIN(cur, T_POINT))
			rhs->lhs_tp = DescendNode(lang_stat, cur->r, scp);

		ret->e_holder.expr.emplace_back(lhs);
		ret->e_holder.expr.emplace_back(rhs);

        for(int i = node_stack.size() - 2; i >= 0; i--)
        {
            cur = *(node_stack.begin() + i);
            rhs = AstFromNode(lang_stat, cur->r, scp);
			if(!CMP_NTYPE_BIN(cur, T_POINT))
				rhs->lhs_tp = DescendNode(lang_stat, cur->r, scp);
            ret->e_holder.expr.emplace_back(rhs);
        }


        switch(n->t->type)
        {
		case T_POINT:
		{
			ret->point_get_last_val = true;

			node *first_node = *(node_stack.begin() + last_idx);

			dummy_type = DescendNode(lang_stat, first_node->l, scp);
			
			if (dummy_type.type == TYPE_ENUM_TYPE)
			{
				decl2 *d = dummy_type.e_decl->type.GetEnumDecl(n->r->t->str);
				ret->type = AST_INT;
				ret->num = d->type.e_idx;;
			}
			else if (dummy_type.type == TYPE_STRUCT_TYPE && IS_FLAG_ON(dummy_type.strct->flags, TP_STRCT_ETRUCT))
			{
				decl2 *d = dummy_type.strct->FindDecl(n->r->t->str);
				ret->type = AST_INT;
				ret->num = d->type.e_idx;;
			}
			else
			{

				auto new_ar = (own_std::vector<ast_point> *) AllocMiscData(lang_stat, sizeof(own_std::vector<ast_point>));
				memset(new_ar, 0, sizeof(*new_ar));

				ast_point aux;
				aux.decl_strct = dummy_type.strct->this_decl;
				aux.exp = ret->e_holder.expr[0];
				new_ar->emplace_back(aux);

				decl2* strct = aux.decl_strct;
				for (int i = node_stack.size() - 1; i >= 0; i--)
				{
					first_node = *(node_stack.begin() + i);

					decl2* is_struct = nullptr;
					bool is_tuple = IS_FLAG_ON(strct->type.strct->flags, TP_STRCT_TUPLE);
					if (is_tuple)
					{
						is_struct = strct->type.strct->scp->vars[first_node->t->i];
					}
					else
						is_struct = FindIdentifier(first_node->r->t->str, strct->type.strct->scp, &dummy_type);
					ASSERT(is_struct);
					if (is_struct)
					{
						aux.decl_strct = is_struct;
						aux.exp = AstFromNode(lang_stat, first_node->r, strct->type.strct->scp);
						//aux.
						if (is_tuple)
						{
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
			//ret->points = *new_ar;

		}break;
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
		case T_LESSER_THAN:
		case T_GREATER_EQ:
		case T_GREATER_THAN:
		case T_MINUS:
		case T_SHIFT_LEFT:
		case T_SHIFT_RIGHT:
		case T_DIV:
		case T_PERCENT:
		{
		}break;
        case T_EQUAL:
        {
            
        }break;
        case T_PLUS:
        {
            
        }break;
        default:
            ASSERT(0);
        }
    }break;
	case node_type::N_INT:
    {
        ret->type = AST_INT;
        ret->num = n->t->i;
    }break;
	case N_CONST_DECL:
    {
        ret->type = AST_IDENT;
        type2 ret_type;
		ret->decl = FindIdentifier(n->l->t->str, scp, &ret_type);
		if (!ret->decl)
		{
			ret->str = n->t->str;
		}
    }break;
	case node_type::N_IDENTIFIER:
    {
        ret->type = AST_IDENT;
        type2 ret_type;
		if (n->decl)
		{
			ret->decl = n->decl;
			memcpy(&ret_type, &ret->decl->type, sizeof(type2));
			//ret_type = ret->decl->type;
		}
		else
			ret->decl = FindIdentifier(n->t->str, scp, &ret_type);
		if (!ret->decl)
		{
			ret->str = n->t->str;
		}
    }break;
	case node_type::N_KEYWORD:
    {
        switch(n->kw)
        {
        case KW_CONTINUE:
        {
            ret->type = AST_CONTINUE;
        }break;
        case KW_DBG_BREAK:
        {
            ret->type = AST_DBG_BREAK;
        }break;
        case KW_BREAK:
        {
            ret->type = AST_BREAK;
        }break;
        case KW_USING:
        {
            ret->type = AST_EMPTY;
        }break;
        case KW_TRUE:
        {
            ret->type = AST_INT;
			ret->num = 1;
        }break;
        case KW_NIL:
        case KW_FALSE:
        {
            ret->type = AST_INT;
			ret->num = 0;
        }break;
        case KW_RETURN:
        {
            ret->type = AST_RET;
			if(IS_FLAG_ON(scp->fdecl->flags, FUNC_DECL_COROUTINE))
			{
				decl2* d = FindIdentifier(own_std::string("CoroutineEnding"), scp, &dummy_type);

				decl2* first_arg = d->type.fdecl->args[0];

				node* label_name = new_node(lang_stat, n->t);
				label_name->type = N_IDENTIFIER;
				label_name->t->str = own_std::string("_ret_label") + own_std::to_string(scp->fdecl->total_hidden_ret_labels);

				scp->fdecl->total_hidden_ret_labels++;

				first_arg->type.nd = label_name;

				node* new_tree = d->type.fdecl->func_node->r->r->NewTree(lang_stat);
				BuildMacroTree(lang_stat, d->type.fdecl->scp, new_tree, n->t->line);
				LookingForLabels(lang_stat, new_tree, scp);

				decl2* res = DescendNameFinding(lang_stat, new_tree, scp);
				ASSERT(res);

				DescendNode(lang_stat, new_tree, scp);

				// disabling the flag, because it will cause this trigger this return again on the
				// other return that we have inside the macro
				scp->fdecl->flags &= ~FUNC_DECL_COROUTINE;
				ret = AstFromNode(lang_stat, new_tree, scp);
				scp->fdecl->flags |= FUNC_DECL_COROUTINE;
				ASSERT(ret->type == AST_STATS);
				FOR_VEC(cur_st_ptr, ret->stats)
				{
					ast_rep* cur_st = *cur_st_ptr;
					cur_st->dont_make_dbg_stmnt = true;
				}
				ret->stats[ret->stats.size() - 2]->dont_make_dbg_stmnt = false;
			}
			if (n->r)
			{
				ret->ret.ast = AstFromNode(lang_stat, n->r, scp);
				ret->ret.tp = DescendNode(lang_stat, n->r, scp);

			}
			InsertDeferd(&ret, scp, true);
        }break;
		default:
			ASSERT(0);
        }
    }break;
	case node_type::N_STR_LIT:
	{
		ret->type = AST_STR_LIT;
		ret->str = own_std::string(n->t->str);
	}break;
	case node_type::N_SCOPE:
    {
		ret->type = AST_STATS;
		ret->line_number = n->r->t->line;
		ast_rep* rhs = AstFromNode(lang_stat, n->r, n->scp);
		if (rhs->type == AST_STATS)
			INSERT_VEC(ret->stats, rhs->stats);
		else
			ret->stats.emplace_back(rhs);
		InsertDeferd(&ret, n->scp, false);
		/*
		if (n->r && n->r->type != N_STMNT && n->r->type != N_IF)
		{
		}
		else
			ret = AstFromNode(lang_stat, n->r, n->scp);
			*/
    }break;
	case node_type::N_ARRAY_CONSTRUCTION:
	{
		auto last = lang_stat->cur_func;
        ret->type = AST_ARRAY_COSTRUCTION;
		type2 tp_ar;
		NameFindingGetType(lang_stat, n->l, scp, tp_ar);

		ret->ar_constr.type = *tp_ar.tp;

		int tp_size = GetTypeSize(&tp_ar);
		ret->ar_constr.at_offset = lang_stat->cur_strct_constrct_size_per_statement;
		lang_stat->cur_strct_constrct_size_per_statement += tp_size;



		int cur_sz = lang_stat->cur_func->strct_constrct_size_per_statement;
		lang_stat->cur_func->strct_constrct_size_per_statement = max(cur_sz, lang_stat->cur_strct_constrct_size_per_statement);

		//type_struct2* strct =  ret->strct_constr.strct;
		FOR_VEC(c, *n->exprs)
		{
			//ast_struct_construct_info info;
			//info.var = strct->FindDecl(c->n->l->t->str);
			ast_rep *a = AstFromNode(lang_stat, c->n, scp);
			ret->ar_constr.commas.emplace_back(a);
		}
		lang_stat->cur_strct_constrct_size_per_statement -= tp_size;
	}break;
	case node_type::N_STRUCT_CONSTRUCTION:
	{
		auto last = lang_stat->cur_func;
        ret->type = AST_STRUCT_COSTRUCTION;
		FindIdentifier(n->l->t->str, scp, &dummy_type);
		int tp_size = 0;
		if (dummy_type.type == TYPE_STRUCT_TYPE)
		{
			ret->strct_constr.strct = dummy_type.strct;
		}
		else
		{
			tp_size = 16;
			ret->strct_constr.is_vector = true;
			//ASSERT(0)
		}
		//tp_size = ret->strct_constr.strct->size;


		ret->strct_constr.at_offset = lang_stat->cur_strct_constrct_size_per_statement;

		lang_stat->cur_strct_constrct_size_per_statement += tp_size;

		int cur_sz = lang_stat->cur_func->strct_constrct_size_per_statement;
		lang_stat->cur_func->strct_constrct_size_per_statement = max(cur_sz, lang_stat->cur_strct_constrct_size_per_statement);

		type_struct2* strct =  ret->strct_constr.strct;
		int i = 0;
		FOR_VEC(c, *n->exprs)
		{
			ast_struct_construct_info info;
			if (dummy_type.type == TYPE_VECTOR_TYPE)
			{
				info.var = lang_stat->f32_decl;
				info.exp = AstFromNode(lang_stat, c->n, scp);
			}
			else if (IS_FLAG_ON(strct->flags, TP_STRCT_TUPLE))
			{
				info.var = strct->scp->vars[i];
				info.exp = AstFromNode(lang_stat, c->n, scp);
			}
			else
			{
				info.var = strct->FindDecl(c->n->l->t->str);
				info.exp = AstFromNode(lang_stat, c->n->r, scp);
			}
			ret->strct_constr.commas.emplace_back(info);
			i++;
		}
		//lang_stat->cur_strct_constrct_size_per_statement -= tp_size;
	}break;
	case node_type::N_OP_OVERLOAD:
	case node_type::N_FUNC_DECL:
	{
        ret->type = AST_FUNC;
		ASSERT(n->fdecl);
        ret->func.fdecl = n->fdecl;

		auto last = lang_stat->cur_func;
		lang_stat->cur_func = n->fdecl;
        
        ret->func.fdecl = n->fdecl;

		ast_rep* coroutine_prolegue = nullptr;
		if (IS_FLAG_ON(n->fdecl->flags, FUNC_DECL_COROUTINE))
		{
			decl2* d = FindIdentifier(own_std::string("CoroutinePrologue"), scp, &dummy_type);
			node* new_tree = n->fdecl->coroutine_prologue_tree;
			//BuildMacroTree(lang_stat, n->fdecl->scp, new_tree, n->t->line);

			lang_stat->flags |= PSR_FLAGS_REPORT_UNDECLARED_IDENTS;
			unit_file* prev_file = lang_stat->cur_file;
			lang_stat->cur_file = n->fdecl->from_file;
			lang_stat->flags |= PSR_FLAGS_DONT_DECLARE_VARIABLES;
			decl2* res = DescendNameFinding(lang_stat, new_tree, scp);
			lang_stat->flags &= ~PSR_FLAGS_DONT_DECLARE_VARIABLES;
			lang_stat->cur_file = prev_file;
			lang_stat->flags &= ~PSR_FLAGS_REPORT_UNDECLARED_IDENTS;

			ASSERT(res);
			DescendNode(lang_stat, new_tree, scp);
			coroutine_prolegue = AstFromNode(lang_stat, new_tree, scp);
			if (coroutine_prolegue->type == AST_STATS)
			{
				FOR_VEC(cur_st_ptr, coroutine_prolegue->stats)
				{
					ast_rep* cur_st = *cur_st_ptr;
					cur_st->dont_make_dbg_stmnt = true;
				}
			}
			//ret->func.stats->stats.insert(ret->func.stats->stats.begin(), prologue_ast->stats.begin(), prologue_ast->stats.end());

		}

        ret->func.stats = AstFromNode(lang_stat, n->r->r, n->fdecl->scp);
		if (ret->func.stats->type != AST_STATS)
		{
			ast_rep* new_ast = NewAst();
			new_ast->type = AST_STATS;
			new_ast->stats.emplace_back(ret->func.stats);
			ret->func.stats = new_ast;
		}
		if (IS_FLAG_ON(n->fdecl->flags, FUNC_DECL_COROUTINE))
		{
			ASSERT(coroutine_prolegue);
			ret->func.stats->stats.insert(ret->func.stats->stats.begin(), coroutine_prolegue->stats.begin(), coroutine_prolegue->stats.end());
		}


		lang_stat->cur_func = last;

	}break;
	case node_type::N_DEFER:
	{
		ret = AstFromNode(lang_stat, n->r, scp);
		if (ret->type == AST_STATS)
		{
			INSERT_VEC(scp->defered, ret->stats);
		}
		else
		{
			scp->defered.emplace_back(ret);
		}
	}break;
	case node_type::N_CALL:
	{
		ret->type = AST_CALL;

		ret->call.in_func = scp->fdecl;
		
		if (n->l->type != N_IDENTIFIER)
		{
			ret->call.lhs = AstFromNode(lang_stat, n->l, scp);
			ret->call.indirect = true;
			ret->call.fdecl = DescendNode(lang_stat, n->l, scp).fdecl;
		}
		else
		{
			decl2* decl = FindIdentifier(n->l->t->str, scp, &dummy_type);
			ret->call.fdecl = decl->type.fdecl;

			ret->call.fdecl->references++;

			ret->call.indirect = false;
			if (decl->type.type == TYPE_FUNC_PTR)
			{
				ret->call.indirect = true;
				ret->call.func_ptr_var = decl;
			}
		}

		func_decl* f = ret->call.fdecl;

		if (IS_FLAG_ON(f->flags, FUNC_DECL_INTERNAL))
		{
			if (f->name == "sizeof")
			{
				dummy_type = DescendNode(lang_stat, n->r, scp);
				ret->type = AST_INT;
				ret->num = GetTypeSize(&dummy_type);
				
			}
			else if (f->name == "GetTypeData")
			{
				dummy_type = DescendNode(lang_stat, n->r, scp);
				ret->type = AST_INT;
				if (dummy_type.type == TYPE_STRUCT_TYPE || dummy_type.type == TYPE_STRUCT)
				{
					ret->num = dummy_type.strct->type_sect_offset + MEM_PTR_START_ADDR;
				}
			}
			else if (f->name == "get_func_bc")
			{
				dummy_type = DescendNode(lang_stat, n->r, scp);
				ret->type = AST_GET_FUNC_BC;
				ret->call.fdecl = dummy_type.fdecl;
			}
			else if (f->name == "__is_struct")
			{
				dummy_type = DescendNode(lang_stat, n->r, scp);
				ret->type = AST_INT;
				ret->num = dummy_type.type == TYPE_STRUCT;
			}
			else
				ASSERT(0);
		}
		else if(n->r)
		{

			if (n->r->type == N_BINOP && n->r->t->type == T_COMMA)
			{
				ast_rep* args = AstFromNode(lang_stat, n->r, scp);
				ret->call.args = args->e_holder.expr;
			}
			else
			{
				ret->call.args.emplace_back(AstFromNode(lang_stat, n->r, scp));
			}
		}
		if (IS_FLAG_ON(f->flags, FUNC_DECL_VAR_ARGS))
		{
			// we will create an array of the fixed args, and after the type info
			// and the value of the var args
			// inserting fixed args 
			int var_arg_start_idx = f->args.size() - 1;
			own_std::vector<ast_rep*> var_arg_info;
			var_arg_info.insert(var_arg_info.begin(), ret->call.args.begin(), ret->call.args.begin() + var_arg_start_idx);

			// rel_array
			ast_rep* offset_to_var_args = NewAst();
			offset_to_var_args->type = AST_INT;
			// 16 is the size of rel_array struct
			offset_to_var_args->num = 16;
			var_arg_info.emplace_back(offset_to_var_args);

			int total_var_args = ret->call.args.size() - var_arg_start_idx;
			ast_rep* total_var_args_ast = NewAst();
			total_var_args_ast->type = AST_INT;
			total_var_args_ast->num = total_var_args;
			var_arg_info.emplace_back(total_var_args_ast);

			

			ast_rep* zero = NewAst();
			zero->type = AST_INT;
			zero->num = 0;


			for(int i = var_arg_start_idx; i < ret->call.args.size();i++)
			{
				ast_rep* cur_arg = ret->call.args[i];

				ast_rep* type_int = NewAst();
				type_int->type = AST_INT;
				type_int->num = cur_arg->lhs_tp.type;
				var_arg_info.emplace_back(type_int);

				if(cur_arg->lhs_tp.type == TYPE_STRUCT)
				{
					type_int = NewAst();
					type_int->type = AST_INT;
					type_int->num = MEM_PTR_START_ADDR + cur_arg->lhs_tp.strct->type_sect_offset;
					var_arg_info.emplace_back(type_int);
				}
				else
					var_arg_info.emplace_back(zero);
				
				ast_rep* ptr = NewAst();
				ptr->type = AST_INT;
				ptr->num = cur_arg->lhs_tp.ptr;
				var_arg_info.emplace_back(ptr);


				// casting to the highest type so that we zero the
				// rest of the register when generating the code

				ast_rep* arg = cur_arg;
				if(cur_arg->lhs_tp.type == TYPE_STRUCT)
				{
					ast_rep* ref = NewAst();
					ref->type = AST_ADDRESS_OF;
					ref->ast = cur_arg;
					arg = ref;
				}

				ast_rep* cast_to_s64 = NewAst();
				cast_to_s64->type = AST_CAST;
				cast_to_s64->cast.casted = arg;
				cast_to_s64->cast.type.type = TYPE_S64;
				var_arg_info.emplace_back(cast_to_s64);
				auto a = 0;
			}
			ret->call.args = var_arg_info;
			//ret->call.args.erase()
		}
	}break;
	case node_type::N_FLOAT:
	{
		ret->type = AST_FLOAT;
		ret->f32 = n->t->f;

	}break;
	case node_type::N_STMNT:
	{
		lang_stat->cur_strct_constrct_size_per_statement = 0;
		ret->type = AST_STATS;
		ret->line_number = n->t->line;
		own_std::vector<node*> node_stack;
		node* cur_node = n;
		node_stack.emplace_back(cur_node);
		while (cur_node->l->type == node_type::N_STMNT)
		{
			node_stack.emplace_back(cur_node->l);
			cur_node = cur_node->l;
		}
		int size = node_stack.size();

		ast_rep* lhs = AstFromNode(lang_stat, cur_node->l, scp);
		if (IS_FLAG_ON(cur_node->l->flags, NODE_FLAGS_STMNT_WITHOUT_SEMICOLON))
			lhs->stmnt_without_semicolon = true;

		if (lhs->type == AST_STATS)
			INSERT_VEC(ret->stats, lhs->stats);
		else
			ret->stats.emplace_back(lhs);

        for(int i = node_stack.size() - 1; i >= 0; i--)
        {
			node* s = *(node_stack.begin() + i);
			if (!s->r)
				continue;
			ast_rep* rhs = AstFromNode(lang_stat, s->r, scp);
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
	}break;
	case N_FOR:
	{
		ret->type = AST_FOR;
		node* for_nd = n;
		scp = n->r->scp;
		bool is_rev = false;
		if (n->l->type == N_KEYWORD && n->l->kw == KW_REV)
		{
			is_rev = true;
			n = n->l->r;
		}
		else
			n = n->l;
		if (IsNodeOperator(n, T_IN))
		{
			node* var_name_nd = n->l;
			ast_rep* var_name = AstFromNode(lang_stat, var_name_nd, scp);
			ast_rep* var_name_og = var_name;
			if (IsNodeOperator(n->r, T_TWO_POINTS))
			{
				type2 var_type = DescendNode(lang_stat, var_name_nd, scp);

				node* start_val_nd = n->r->l;
				node* end_val_nd = n->r->r;
				ast_rep* start_val = AstFromNode(lang_stat, start_val_nd, scp);
				ast_rep* end_val = AstFromNode(lang_stat, end_val_nd, scp);

				tkn_type2 cmp_op;

				if (var_type.ptr > 0)
				{
					ast_rep* address = NewAst();
					address->type = AST_ADDRESS_OF;
					address->ast = var_name;
					address->line_number = n->t->line;

					ast_rep* cast_to_ptr_u64 = NewAst();
					cast_to_ptr_u64->type = AST_CAST;
					cast_to_ptr_u64->cast.casted = address;
					cast_to_ptr_u64->cast.type.type = TYPE_U64;
					cast_to_ptr_u64->cast.type.ptr = 1;
					cast_to_ptr_u64->line_number = n->t->line;
					var_name = cast_to_ptr_u64;

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

				}
				else
				{
					ast_rep* inc_val = NewAst();

					if (is_rev)
					{
						inc_val->type = AST_MINUS_MINUS;
					}
					else
					{
						inc_val->type = AST_PLUS_PLUS;
					}
					inc_val->unop_assign.ast = var_name;
					inc_val->unop_assign.tp = var_type;
					ret->for_info.at_loop_end_stat = inc_val;
				}
				
				if (is_rev)
				{
					cmp_op = T_GREATER_THAN;
				}
				else
				{
					cmp_op = T_LESSER_THAN;
				}
				ret->for_info.start_stat = CreateAstBin(lang_stat, T_EQUAL, var_name, start_val, start_val_nd, scp);

				ret->for_info.cond_stat = CreateAstBin(lang_stat, cmp_op, var_name_og, end_val, end_val_nd, scp);

			}
			else
			{
				ASSERT(0)
			}

		}
		else
		{
			ASSERT(0)
		}
		ret->for_info.scope = AstFromNode(lang_stat, for_nd->r, scp);
	}break;
	case N_WHEN_USED:
	case N_HASHTAG:
	{

	}break;
	case N_APOSTROPHE:
	{
		ret->type = AST_CHAR;
		ret->num = n->t->i;
	}break;
	case N_EMPTY:
		break;
    default:
        ASSERT(0);
    }
    return ret;
}

void GetIRVal(lang_state *lang_stat, ast_rep *ast, ir_val *val)
{
    switch(ast->type)
    {
    case AST_STRUCT_COSTRUCTION:
    {
        val->type = IR_TYPE_ON_STACK;
        val->i = ast->strct_constr.at_offset;
		//val->is_unsigned = IsUnsigned(ast->decl->type.type);
    }break;
    case AST_STR_LIT:
	{
		//InsertIntoDataSect(lang_stat, (void *))
		val->type = IR_TYPE_STR_LIT;
		val->str = std_str_to_heap(lang_stat, &ast->str);
	}break;
    case AST_IDENT:
    {
        val->type = IR_TYPE_DECL;
        val->decl = ast->decl;
		val->ptr = ast->decl->type.ptr;
		val->reg_sz = GetTypeSize(&ast->decl->type);
		if (val->reg_sz < 0)
			val->reg_sz = 8;
		val->reg_sz = min(val->reg_sz, 8);
		val->is_unsigned = false;
		if (ast->decl->type.type == TYPE_STATIC_ARRAY)
		{
			//val->is_unsigned = IsUnsigned(ast->decl->type.tp->type);
			val->is_unsigned = true;
			val->reg_sz = 8;
		}
		else if(ast->decl->type.type == TYPE_STRUCT_TYPE 
			&& IS_FLAG_ON(ast->decl->type.strct->flags, TP_STRCT_ETRUCT))
		{
			val->is_unsigned = true;
		}
		else if(ast->decl->type.type != TYPE_STRUCT)
			val->is_unsigned = IsUnsigned(ast->decl->type.type);
		else if(ast->decl->type.type == TYPE_STRUCT
			&& IS_FLAG_ON(ast->decl->type.strct->flags, TP_STRCT_ETRUCT))
		{
			val->reg_sz = GetTypeSize(&ast->decl->type.strct->vars[0]->type);
		}
		//if (ast->decl->type.ptr > 0)
			//val->is_unsigned = true;
		val->is_float = ast->decl->type.type == TYPE_F32;
		val->is_packed_float = ast->decl->type.type == TYPE_VECTOR;
		val->is_float = val->is_float || val->is_packed_float;
		val->ptr = ast->decl->type.ptr;
		val->deref = 0;
    }break;
    case AST_FLOAT:
    {
        val->type = IR_TYPE_F32;
        val->f32 = ast->f32;
		val->reg_sz = 8;
		val->is_unsigned = false;
		val->is_float = true;
    }break;
    case AST_CHAR:
    case AST_INT:
    {
        val->type = IR_TYPE_INT;
        val->i = ast->num;
		val->is_unsigned = ast->num < 0 ? false : true;
		val->reg_sz = 8;
    }break;
    default:
        ASSERT(0)
    }

}

void GenIRBinOp(lang_state *lang_stat, ast_rep *ast, ir_val *lhs, ir_val *rhs, own_std::vector<ir_rep> *out)
{
    switch(ast->op)
    {
    case T_PLUS:
    {
        
    }break;
    default:
        ASSERT(0)

    }
}
bool IsAstSimple(lang_state *lang_stat, ast_rep *ast)
{
    if(ast->type == AST_INT || ast->type == AST_IDENT)
        return true;
    return false;
}

void IRCreateEndBlock(lang_state *lang_stat, int begin_sub_if_idx, own_std::vector<ir_rep> *out, ir_type type)
{
    int end_idx = out->size();

    ir_rep ir;
    ir.type = type;
    ir.block.other_idx = begin_sub_if_idx;


    out->emplace_back(ir);
    ir_rep *begin = &(*out)[begin_sub_if_idx];


	begin->block.other_idx = out->size();
	switch (type)
	{
	case IR_BEGIN_STMNT:
	{
		ASSERT(!lang_stat->in_ir_stmnt);
		lang_stat->in_ir_stmnt = true;
	}break;
	case IR_END_STMNT:
	{
		ASSERT(begin->type == IR_BEGIN_STMNT);
		lang_stat->in_ir_stmnt = false;
	}break;
	}
}
int IRCreateBeginBlock(lang_state *lang_stat, own_std::vector<ir_rep> *out, ir_type type, void *data = nullptr)
{
    ir_rep ir;
    ir.type = type;
    int ret = out->size();

    out->emplace_back(ir);
	if (type == IR_BEGIN_STMNT)
	{
		auto l = (int)(long long)data;
		out->back().block.stmnt.line = l;
		//ir.type = IR_NOP;
		//out->emplace_back(ir);
	}

    return ret;
}

ir_type FromTokenOpToIRType(tkn_type2 op)
{
	switch (op)
	{
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
void FreeReg(lang_state* lang_stat, char reg_idx)
{
	lang_stat->regs[reg_idx] &= ~REG_USED_FLAG;
}


void FreeAllFloatRegs(lang_state* lang_stat)
{
	memset(lang_stat->float_regs, 0, sizeof(lang_stat->float_regs));
	//lang_stat->float_regs[reg_idx] &= ~REG_FREE_FLAG;
}
void FreeSpecificFloatReg(lang_state* lang_stat, char idx)
{
	//ASSERT(IS_FLAG_ON(lang_stat->float_regs[idx], REG_FREE_FLAG));
	lang_stat->float_regs[idx] &= ~REG_USED_FLAG;
}
void AllocSpecificFloatReg(lang_state* lang_stat, char idx)
{
	//ASSERT(IS_FLAG_OFF(lang_stat->float_regs[idx], REG_FREE_FLAG));
	lang_stat->float_regs[idx] |= REG_USED_FLAG;
}
char AllocFloatReg(lang_state* lang_stat)
{
	for (int i = 0; i < 9; i++)
	{
		if (IS_FLAG_OFF(lang_stat->float_regs[i], REG_USED_FLAG))
		{
			lang_stat->float_regs[i] |= REG_USED_FLAG;
			return i;
		}
	}
	ASSERT(0);
	return -1;

}


void FreeAllRegs(lang_state* lang_stat)
{
	memset(lang_stat->regs, 0, sizeof(lang_stat->regs));
	//lang_stat->regs[reg_idx] &= ~REG_FREE_FLAG;
}
void FreeSpecificReg(lang_state* lang_stat, char idx)
{
	//ASSERT(IS_FLAG_ON(lang_stat->regs[idx], REG_FREE_FLAG));
	lang_stat->regs[idx] &= ~REG_USED_FLAG;
}
void AllocSpecificReg(lang_state* lang_stat, char idx)
{
	//ASSERT(IS_FLAG_OFF(lang_stat->regs[idx], REG_FREE_FLAG));
	lang_stat->regs[idx] |= REG_USED_FLAG;
	if(lang_stat->track_alloc_regs)
	{
		lang_stat->tracked_regs.emplace_back(idx);
	}
}
char AllocReg(lang_state* lang_stat, int start = 0)
{
	for (int i = start; i < 9; i++)
	{
		if (i == PRE_X64_RSP_REG || i == AUX_DECL_REG)
			continue;
		if (IS_FLAG_OFF(lang_stat->regs[i], REG_USED_FLAG))
		{
			//lang_stat->regs[i] |= REG_FREE_FLAG;
			AllocSpecificReg(lang_stat, i);
			return i;
		}
	}
	ASSERT(0);
}
bool HasCall(lang_state *lang_stat, ast_rep *ast)
{
	switch (ast->type)
	{
	case AST_INT:
	{
		return false;
	}break;
	case AST_EMPTY:
	{
		return false;
	}break;
	case AST_CALL:
	{
		return true;
	}break;
	case AST_IDENT:
		return false;
	case AST_INDEX:
	{
		bool is_static_ar = ast->index.lhs->type == AST_IDENT && ast->index.lhs->decl->type.type == TYPE_STATIC_ARRAY;
		if(ast->index.lhs_type.type != TYPE_STATIC_ARRAY)
		{
			return true;
		}
		return false;
	}break;
	case AST_FLOAT:
	{
		return false;
	}break;
	case AST_DEREF:
	{
		return HasCall(lang_stat, ast->deref.exp);
	}break;
	case AST_ADDRESS_OF:
	{
		return HasCall(lang_stat, ast->ast);
	}break;
	case AST_CAST:
	{
		return HasCall(lang_stat, ast->cast.casted);
	}break;
	case AST_BINOP:
	{
		if(ast->op == T_POINT)
		{
			if (HasCall(lang_stat, ast->e_holder.expr[0]))
			{
				return true;
			}
		}
		else
		{
			FOR_VEC(a, ast->e_holder.expr)
			{
				if (HasCall(lang_stat, *a))
				{
					return true;
				}
			}
		}
		return false;
	}break;
	default:
		ASSERT(0);
	}
}

void MaybeUnspillRegisters(lang_state* lang_stat, int* had_spilled_reg, own_std::vector<ir_rep> *out)
{
	ir_rep ir;
	if(*had_spilled_reg != -1)
	{
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
		ir.assign.lhs.on_stack_type = ON_STACK_SPILL;
		out->emplace_back(ir);
	}
}
int SpillRegisters(lang_state* lang_stat, int spilled_reg, own_std::vector<ir_rep> *out)
{
	ir_rep ir;
	int spill_offset = 0;
	ir = {};
	ir.type = IR_ASSIGNMENT;
	ir.assign.to_assign.type = IR_TYPE_ON_STACK;
	ir.assign.to_assign.reg_sz = 8;
	ir.assign.to_assign.deref = -1;
	ir.assign.to_assign.on_stack_type = ON_STACK_SPILL;
	ir.assign.to_assign.i = spill_offset;
	ir.assign.only_lhs = true;
	ir.assign.lhs.type = IR_TYPE_REG;
	ir.assign.lhs.reg_sz = 8;
	ir.assign.lhs.reg = spilled_reg;
	ir.assign.lhs.deref = -1;
	out->emplace_back(ir);
	return spill_offset;
	// we should only have one reg used
}
void MaybeSpillRegisters(lang_state* lang_stat, ast_rep *ast, int* had_spilled_reg, own_std::vector<ir_rep> *out)
{
	ir_rep ir;
	if (HasCall(lang_stat, ast))
	{
		for (int i = 0; i < 9; i++)
		{
			if (i == PRE_X64_RSP_REG || i == AUX_DECL_REG)
				continue;
			if (IS_FLAG_ON(lang_stat->regs[i], REG_USED_FLAG))
			{
				SpillRegisters(lang_stat, i, out);
				*had_spilled_reg = i;
			}
		}
	}
}
void GetIRFromAst(lang_state* lang_stat, ast_rep* ast, own_std::vector<ir_rep>* out, ir_val *top = nullptr);
void GetIRBin(lang_state *lang_stat, ast_rep *ast_bin, own_std::vector<ir_rep> *out, ir_type type, void *data = nullptr)
{
	ir_rep ir;
	memset(&ir, 0, sizeof(ir_rep));
	//ir.assign.to_assign.type = IR_TYPE_REG;
	//ir.assign.to_assign.reg = AllocReg(lang_stat);


	if(ast_bin->e_holder.expr[0]->type == AST_BINOP && (ast_bin->e_holder.expr[0]->op == T_COND_AND || ast_bin->e_holder.expr[0]->op == T_COND_OR))
	{
		own_std::vector<ir_rep> irs;

		int if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);
		int sub_if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_SUB_IF_BLOCK);
		int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_COND_BLOCK);

		GetIRFromAst(lang_stat, ast_bin->e_holder.expr[0], out);

		IRCreateEndBlock(lang_stat,cond_idx, out, IR_END_COND_BLOCK);

		char reg = AllocReg(lang_stat);

		GenStackThenIR(lang_stat, ast_bin->e_holder.expr[1], out, &ir.bin.rhs, &ir.bin.rhs);

		CreateOppositeRegAssigmentAfterCondChecking(lang_stat, out, sub_if_idx, if_idx, reg);

		ir.type = IR_CMP_EQ;
		ir.bin.op = T_COND_EQ;
		ir.bin.lhs.type = IR_TYPE_REG;
		ir.bin.lhs.reg = reg;
		ir.bin.lhs.reg_sz = 1;
		ir.bin.lhs.deref = -1;
		out->emplace_back(ir);


	}
	else
	{


		ir.bin.rhs.type = IR_TYPE_REG;
		ir.bin.rhs.reg_sz = 8;
		ir.bin.rhs.reg = AllocReg(lang_stat);
		GenStackThenIR(lang_stat, ast_bin->e_holder.expr[1], out, &ir.bin.rhs, &ir.bin.rhs);

		if (ir.bin.rhs.type == IR_TYPE_RET_REG)
		{
			//at the moment not allowing fonction calls at conds
			//ASSERT(false);
			ir_val saved = ir.bin.rhs;
			ir.type = IR_ASSIGNMENT;
			ir.assign.only_lhs = true;
			ir.assign.lhs = ir.bin.rhs;
			ir.assign.to_assign.type = IR_TYPE_REG;
			ir.assign.to_assign.reg_sz = saved.reg_sz;
			ir.assign.to_assign.is_float = saved.is_float;
			ir.assign.to_assign.is_float = saved.is_float;
			if (ir.bin.rhs.is_float)
			{
				ir.assign.to_assign.reg = AllocFloatReg(lang_stat);
			}
			else
				ir.assign.to_assign.reg = AllocReg(lang_stat);

			out->emplace_back(ir);
			ir.bin.rhs = ir.assign.to_assign;
		}
		ir.bin.lhs.type = IR_TYPE_REG;
		ir.bin.lhs.reg_sz = 8;
		ir.bin.lhs.reg = AllocReg(lang_stat);
		int had_spilled_reg = -1;

		if (HasCall(lang_stat, ast_bin->e_holder.expr[0]) && ir.bin.rhs.type == IR_TYPE_REG)
		{
			had_spilled_reg = ir.bin.rhs.reg;
			SpillRegisters(lang_stat, had_spilled_reg, out);
		}
		GenStackThenIR(lang_stat, ast_bin->e_holder.expr[0], out, &ir.bin.lhs, &ir.bin.lhs);
		MaybeUnspillRegisters(lang_stat, &had_spilled_reg, out);

		if (ir.bin.lhs.ptr > 0)
		{
			ir.bin.lhs.is_float = false;
			ir.bin.lhs.is_packed_float = false;
		}

		ir.bin.rhs.is_unsigned = ir.bin.lhs.is_unsigned;

		ir.type = type;
		ir.bin.op = ast_bin->op;
		switch (type)
		{
		case IR_CMP_NE:
		case IR_CMP_LE:
		case IR_CMP_EQ:
		case IR_CMP_LT:
		case IR_CMP_GE:
		case IR_CMP_GT:
		{
			ir.bin.it_is_jmp_if_true = (bool)data;
		}break;
		default:
			ASSERT(0);
		}
		CheckIrValIsPointIncDeref(&ir.bin.lhs);
		CheckIrValIsPointIncDeref(&ir.bin.rhs);

		out->emplace_back(ir);
	}
}

bool IsCondAndOr(tkn_type2 t)
{
	return t == T_COND_AND || t == T_COND_OR;
}
void GetIRCond(lang_state* lang_stat, ast_rep* ast, own_std::vector<ir_rep>* out)
{
	if (ast->type == AST_EMPTY)
		return;
	ir_rep ir = {};
	lang_stat->inside_ifs.clear();
	if (ast->type == AST_BINOP && ast->op != T_POINT)
	{
		if (!IsCondAndOr(ast->op))
		{
			tkn_type2 opposite = OppositeCondCmp(ast->op);
			ast->op = opposite;
			GetIRBin(lang_stat, ast, out, FromTokenOpToIRType(opposite), (void*)false);
		}
		else
		{
			GetIRFromAst(lang_stat, ast, out);
		}
	}
	else if (ast->type == AST_OPPOSITE && ast->opposite.exp->type == AST_BINOP)
	{
		if (IsCondAndOr(ast->opposite.exp->op))
		{
			int if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);
			int sub_if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_SUB_IF_BLOCK);
			int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_COND_BLOCK);

			ir_val top = {};
			GetIRFromAst(lang_stat, ast->opposite.exp, out, &top);
			if (top.reg_sz == 0)
				top.reg_sz = 1;
			ASSERT(top.reg_sz <= 8)
			//ASSERT(top.reg_sz != 0);

			IRCreateEndBlock(lang_stat, cond_idx, out, IR_END_COND_BLOCK);

			char reg = AllocReg(lang_stat);

			CreateOppositeRegAssigmentAfterCondChecking(lang_stat, out, sub_if_idx, if_idx, reg);

			ir.type = IR_CMP_EQ;
			ir.bin.op = T_COND_EQ;
			ir.bin.lhs.type = IR_TYPE_REG;
			ir.bin.lhs.reg = reg;
			ir.bin.lhs.reg_sz = top.reg_sz;
			ir.bin.lhs.deref = -1;
			ir.bin.rhs.type = IR_TYPE_INT;
			ir.bin.rhs.i = 0;
			out->emplace_back(ir);
		}
		else
		{

			GenStackThenIR(lang_stat, ast->opposite.exp, out, &ir.bin.lhs, &ir.bin.lhs);
			ir.type = IR_CMP_NE;
			ir.bin.op = T_COND_NE;
			if (IS_FLAG_ON(ir.bin.lhs.reg_ex, IR_VAL_FROM_POINT) && ir.bin.lhs.type == IR_TYPE_REG)
			{
				ir.bin.lhs.deref = 0;
			}
			ir.bin.rhs.type = IR_TYPE_INT;
			ir.bin.rhs.i = 0;
			out->emplace_back(ir);
		}
	}
	else
	{
		ir.type = IR_CMP_EQ;
		ir.bin.op = T_COND_EQ;
		ir.bin.lhs.type = IR_TYPE_REG;
		ir.bin.lhs.reg  = AllocReg(lang_stat);
		ir.bin.lhs.reg_sz  = 8;
		ir.bin.rhs.type = IR_TYPE_INT;
		ir.bin.rhs.i = 0;
		GenStackThenIR(lang_stat, ast, out, &ir.bin.lhs, &ir.bin.lhs);

		/*
		if (ast->type == AST_IDENT || ast->type == AST_OPPOSITE || ast->type == AST_CALL)
		{
			ir.bin.lhs.deref = 1;
		}
		else if (ast->type == AST_DEREF || ast->type == AST_BINOP)
		{
			ir.bin.lhs.deref += 1;
		}
		else
		{
		}
		*/
		ir.bin.rhs.is_unsigned = ir.bin.lhs.is_unsigned;;
		ir.bin.lhs.ptr = 1;
		out->emplace_back(ir);


	}
}

void GenIRSubBin(lang_state* lang_stat, ast_rep* ast, own_std::vector<ir_rep>* out, ir_val_type to, void* data)
{
	ast_rep* lhs_exp = ast->e_holder.expr[0];
	ast_rep* rhs_exp = ast->e_holder.expr[1];

	bool lhs_complex = IsAstSimple(lang_stat, lhs_exp);
	bool rhs_complex = IsAstSimple(lang_stat, rhs_exp);

	//switch(lhs_complex)

}

void PushAstsInOrder(lang_state* lang_stat, ast_rep* ast, own_std::vector<ast_rep*>* out);
void PushArrayOfAsts(lang_state* lang_stat, own_std::vector<ast_rep *> *ar, ast_rep *op, own_std::vector<ast_rep*>* out)
{
	PushAstsInOrder(lang_stat, *ar->begin(), out);
	for (int i = 1; i < ar->size(); i++)
	{
		ast_rep* cur = (*ar)[i];
		PushAstsInOrder(lang_stat, cur, out);
		out->emplace_back(op);
		//lhs /
	}

}
void PushFuncAsts(lang_state* lang_stat, ast_rep* ast, own_std::vector<ast_rep*>* out)
{
	int arg_idx = 0;
	FOR_VEC(arg, ast->call.args)
	{
		ast_rep* a = *arg;
		PushAstsInOrder(lang_stat, a, out);
		out->emplace_back((ast_rep*)(long long)(IR_END_ARG | (arg_idx << 8) | (ast->line_number << 16) | ((long long)0xbeba << 32)));
		arg_idx++;
	}
	if (ast->call.lhs)
		PushAstsInOrder(lang_stat, ast->call.lhs, out);
	out->emplace_back(ast);
}
void PushAstsInOrder(lang_state* lang_stat, ast_rep* ast, own_std::vector<ast_rep*>* out)
{
	ir_rep ir;
	switch (ast->type)
	{
	case AST_CALL:
	{

		PushFuncAsts(lang_stat, ast, out);
		/*
		FOR_VEC(arg, ast->call.args)
		{
			PushAstsInOrder(lang_stat, (*arg), out);
		}
		if(ast->call.lhs)
			PushAstsInOrder(lang_stat, ast->call.lhs, out);
		out->emplace_back(ast);
		*/
	}break;
	case AST_INT:
	case AST_FLOAT:
	case AST_STR_LIT:
	case AST_CHAR:
	case AST_IDENT:
	{
		out->emplace_back(ast);
	}break;
	case AST_NEGATE:
	case AST_NEGATIVE:
	case AST_OPPOSITE:
	{
		PushAstsInOrder(lang_stat, ast->ast, out);
		out->emplace_back(ast);
	}break;
	case AST_CAST:
	{
		PushAstsInOrder(lang_stat, ast->cast.casted, out);
		out->emplace_back(ast);
	}break;
	case AST_ARRAY_COSTRUCTION:
	{
		PushAstsInOrder(lang_stat, ast->ar_constr.commas[0], out);
		for (int i = 1; i < ast->ar_constr.commas.size(); i++)
		{
			ast_rep* cur = ast->ar_constr.commas[i];
			PushAstsInOrder(lang_stat, cur, out);
		}
		out->emplace_back(ast);
	}break;
	case AST_STRUCT_COSTRUCTION:
	{
		PushAstsInOrder(lang_stat, ast->strct_constr.commas[0].exp, out);
		for (int i = 1; i < ast->strct_constr.commas.size(); i++)
		{
			ast_rep* cur = ast->strct_constr.commas[i].exp;
			PushAstsInOrder(lang_stat, cur, out);
		}
		out->emplace_back(ast);
	}break;
	case AST_ADDRESS_OF:
	{
		PushAstsInOrder(lang_stat, ast->ast, out);
		out->emplace_back(ast);
	}break;
	case AST_INDEX:
	{
		PushAstsInOrder(lang_stat, ast->index.lhs, out);
		PushAstsInOrder(lang_stat, ast->index.rhs, out);
		out->emplace_back(ast);
	}break;
	case AST_DEREF:
	{
		PushAstsInOrder(lang_stat, ast->deref.exp, out);
		out->emplace_back(ast);
	}break;
	case AST_BINOP:
	{
		own_std::vector<ast_rep*>* ar = nullptr;
		if (ast->op == T_POINT)
		{
			PushAstsInOrder(lang_stat, ast->points[0].exp, out);
			for (int i = 1; i < ast->points.size(); i++)
			{
				ast_rep* cur = ast->points[i].exp;

				PushAstsInOrder(lang_stat, cur, out);
				//lhs /
			}
			out->emplace_back(ast);
		}
		else if (ast->op == T_COND_EQ || ast->op == T_COND_NE)
		{
			ASSERT(ast->e_holder.expr.size() == 2);

			//out->emplace_back((ast_rep*)(long long)(IR_BEGIN_OR| (ast->line_number << 16) | ((long long)0xbeba << 32)));

			if(ast->e_holder.expr[0]->type == AST_BINOP && (ast->e_holder.expr[0]->op == T_COND_AND || ast->e_holder.expr[0]->op == T_COND_OR))
			{
				own_std::vector<ir_rep> irs;
				
				lang_stat->inside_ifs.make_count(lang_stat->inside_ifs.size() + 1);

				auto last = (own_std::vector<ir_rep>*)&lang_stat->inside_ifs.back();

				int if_idx = IRCreateBeginBlock(lang_stat, last, IR_BEGIN_IF_BLOCK);
				int sub_if_idx = IRCreateBeginBlock(lang_stat, last, IR_BEGIN_SUB_IF_BLOCK);
				int cond_idx = IRCreateBeginBlock(lang_stat, last, IR_BEGIN_COND_BLOCK);

				GetIRFromAst(lang_stat, ast->e_holder.expr[0], last);

				IRCreateEndBlock(lang_stat,cond_idx, last, IR_END_COND_BLOCK);

				char reg = AllocReg(lang_stat);

				CreateOppositeRegAssigmentAfterCondChecking(lang_stat, last, sub_if_idx, if_idx, reg);

				ir.type = IR_CMP_EQ;
				ir.bin.op = T_COND_EQ;
				ir.bin.lhs.type = IR_TYPE_REG;
				ir.bin.lhs.reg = reg;
				ir.bin.lhs.reg_sz = 1;
				ir.bin.lhs.deref = -1;
				ir.bin.rhs.type = IR_TYPE_INT;
				ir.bin.rhs.i = 0;
				last->emplace_back(ir);


				out->emplace_back((ast_rep*)(long long)(IR_BEGIN_INSIDE_IF| ((lang_stat->inside_ifs.size() - 1) << 16) | ((long long)0xbeba << 32)));
				PushAstsInOrder(lang_stat, ast->e_holder.expr[1], out);
				//PushAstsInOrder(lang_stat, ast->e_holder.expr[0], out);
				//PushAstsInOrder(lang_stat, ast->e_holder.expr[1], out);
				out->emplace_back(ast);

			}
			else
			{
				PushAstsInOrder(lang_stat, ast->e_holder.expr[0], out);
				PushAstsInOrder(lang_stat, ast->e_holder.expr[1], out);
				out->emplace_back(ast);
			}
			//out->emplace_back((ast_rep*)(long long)(IR_END_OR| (ast->line_number << 16) | ((long long)0xbeba << 32)));
		}
		else if (ast->op == T_COND_OR)
		{
			out->emplace_back((ast_rep*)(long long)(IR_BEGIN_OR|(ast->line_number << 16) | ((long long)0xbeba << 32)));
			PushArrayOfAsts(lang_stat, &ast->e_holder.expr, ast, out);
			out->emplace_back((ast_rep*)(long long)(IR_END_OR| (ast->line_number << 16) | ((long long)0xbeba << 32)));
		}
		else if (ast->op == T_COND_AND)
		{
			out->emplace_back((ast_rep*)(long long)(IR_BEGIN_AND| (ast->line_number << 16) | ((long long)0xbeba << 32)));
			PushArrayOfAsts(lang_stat, &ast->e_holder.expr, ast, out);
			out->emplace_back((ast_rep*)(long long)(IR_END_AND| (ast->line_number << 16) | ((long long)0xbeba << 32)));
		}
		else
		{
			PushArrayOfAsts(lang_stat, &ast->e_holder.expr, ast, out);
		}
	}break;
	default:
		ASSERT(0);
	}
}



void GenIRComplex(lang_state* lang_stat, ast_rep* ast, own_std::vector<ir_rep>* out, ir_val *dst_val)
{
	ASSERT(0);
	short reg = dst_val->reg;
	//char ptr = dst_
	ir_rep ir = {};

	int begin_complex_idx = out->size();
	ir.type = IR_BEGIN_COMPLEX;

	ir.complx.dst = *dst_val;
	out->emplace_back(ir);
	
	own_std::vector<ast_rep*> exps;
	PushAstsInOrder(lang_stat, ast, &exps);

	int state = 0;


}
bool IsIrValPackedFloat(ir_val* val)
{
	type2* decl_type = &val->decl->type;

	bool is_decl = val->type == IR_TYPE_DECL;
	bool is_ar_float = false;
	if(is_decl)
		is_ar_float = decl_type->type == TYPE_STATIC_ARRAY && decl_type->tp->type == TYPE_VECTOR;

	return is_decl && (val->decl->type.type == TYPE_VECTOR || is_ar_float) || val->is_packed_float;
}
bool IsIrValFloat(ir_val* val)
{
	type2* decl_type = &val->decl->type;

	bool is_decl = val->type == IR_TYPE_DECL;
	bool is_ar_float = false;
	if(is_decl)
		is_ar_float = decl_type->type == TYPE_STATIC_ARRAY && decl_type->tp->type == TYPE_F32;

	return is_decl && (val->decl->type.type == TYPE_F32  || is_ar_float) || val->is_float || val->type == IR_TYPE_F32;
}

void CreateOppositeRegAssigmentAfterCondChecking(lang_state *lang_stat, own_std::vector<ir_rep> *out, int sub_if_idx, int if_idx, int reg)
{
	ir_rep ir = {};
	ir.type = IR_ASSIGNMENT;
	ir.assign.to_assign.type = IR_TYPE_REG;
	ir.assign.to_assign.reg = reg;
	ir.assign.to_assign.deref = -1;
	ir.assign.to_assign.reg_sz = 8;
	ir.assign.only_lhs = true;

	ir.assign.lhs.type = IR_TYPE_INT;
	ir.assign.lhs.i = 0;
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
	ir.assign.lhs.i = 1;
	out->emplace_back(ir);
	IRCreateEndBlock(lang_stat, sub_if_idx, out, IR_END_SUB_IF_BLOCK);
	IRCreateEndBlock(lang_stat, if_idx, out, IR_END_IF_BLOCK);

}
void GinIRMemCpy(lang_state* lang_stat, own_std::vector<ir_rep>* out)
{
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
	//int stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT, (void *)(long long)ast->line_number);
	//int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);
	//GetIRCond(lang_stat, ast->loop.cond, out);
	//IRCreateEndBlock(lang_stat, stmnt_idx, out, IR_END_STMNT);

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
	//ir.assign.only_lhs = true;
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
	//ir.assign.only_lhs = true;
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
	//ir.assign.only_lhs = true;
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
	//ir.assign.only_lhs = true;
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
	//IRCreateEndBlock(lang_stat, cond_idx, out, IR_END_IF_BLOCK);
	IRCreateEndBlock(lang_stat, loop_idx, out, IR_END_LOOP_BLOCK);
	IRCreateEndBlock(lang_stat, block_idx, out, IR_END_BLOCK);

}
bool IsBeginInsideIf(ast_rep *e)
{
	bool is_end_arg = ((long long)e & 0xff) == IR_BEGIN_INSIDE_IF;
	is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
	return is_end_arg;
}
bool IsEndInsideIf(ast_rep *e)
{
	bool is_end_arg = ((long long)e & 0xff) == IR_END_INSIDE_IF;
	is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
	return is_end_arg;
}
bool IsBeginOr(ast_rep *e)
{
	bool is_end_arg = ((long long)e & 0xff) == IR_BEGIN_OR;
	is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
	return is_end_arg;
}
bool IsEndOr(ast_rep *e)
{
	bool is_end_arg = ((long long)e & 0xff) == IR_END_OR;
	is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
	return is_end_arg;
}
bool IsBeginAnd(ast_rep *e)
{
	bool is_end_arg = ((long long)e & 0xff) == IR_BEGIN_AND;
	is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
	return is_end_arg;
}
bool IsEndAnd(ast_rep *e)
{
	bool is_end_arg = ((long long)e & 0xff) == IR_END_AND;
	is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
	return is_end_arg;
}
bool IsEndArg(ast_rep *e)
{
	bool is_end_arg = ((long long)e & 0xff) == IR_END_ARG;
	is_end_arg = is_end_arg && ((((long long)e) >> 32) & 0xffff) == 0xbeba;
	return is_end_arg;
}
void GenIrToArgReg(lang_state* lang_stat, own_std::vector<ir_val> &stack, int arg_idx, own_std::vector<ast_rep *> &exps, own_std::vector<ir_rep> *out)
{
	ir_val* top = &stack[stack.size() - 1];
	//stack.pop_back();
	ir_rep ir = {};
	ir.type = IR_ASSIGNMENT;
	ir.assign.to_assign.type = IR_TYPE_ARG_REG;
	ir.assign.to_assign.reg = arg_idx;
	ir.assign.to_assign.reg_sz = 8;
	ir.assign.to_assign.deref = -1;
	ir.assign.to_assign.is_float = top->is_float;
	ir.assign.only_lhs = true;


	if(IS_FLAG_ON(top->reg_ex, IR_VAL_FROM_POINT) && IS_FLAG_ON(top->reg_ex, IR_VAL_FROM_DEREF))
	{
		//top->deref--;
	}
	ir.assign.lhs = *top;

	ir.assign.lhs.reg_sz = 8;
	if (top->type == IR_TYPE_DECL && top->decl->type.type == TYPE_STATIC_ARRAY)
	{
		ir.assign.lhs.deref = -1;

	}
	if (top->is_packed_float)
	{
		ir.assign.lhs.is_float = false;
		ir.assign.lhs.is_packed_float = false;
	}

	//ir.assign.lhs.deref--;
	//if (IS_FLAG_ON(top->reg_ex, IR_VAL_FROM_POINT) || top->ptr > 0)
		//ir.assign.lhs.deref++;
	out->emplace_back(ir);

	if (top->type == IR_TYPE_REG)
	{
		FreeReg(lang_stat, top->reg);
	}
	top->reg_ex |= IR_VAL_ALREADY_ON_PARAM_REG;
}
void GinIRFromStack(lang_state* lang_stat, own_std::vector<ast_rep *> &exps, own_std::vector<ir_rep> *out, ir_val *top_info)
{
	own_std::vector<ir_val> stack;
	stack.reserve(4);
	ir_rep ir;
	for(int j = 0; j < exps.size(); j++)
	{
		ast_rep* e = exps[j];
		bool is_end_arg = IsEndArg(e);
		if(is_end_arg)
		{
			int cur_line = (((long long)e) >> 16) & 0xffff;
			bool found_call = false;
			int calls_found = 0;
			
			// searching for more than call
			// if we have more than one call in this statement, we cant be sure the address of the
			// arg params will remain the same, because the next func may have already
			// assigned the arg_param0 for example and we will reassign with our own thing, overriding it
			// so we will not continue moving to the reg param
			for (int h = j + 1; h < exps.size(); h++)
			{
				ast_rep *aux = exps[h];
				if(!IsEndArg(aux) && aux->type == AST_CALL)
				{
					if (calls_found == 1)
					{
						found_call = true;
						break;
					}
					calls_found++;
				}
			}
			if (found_call)
			{
				continue;
			}
			int arg_idx = (((long long)e) >> 8) & 0xff;
			GenIrToArgReg(lang_stat, stack, arg_idx, exps, out);
			continue;
		}
		else if (IsBeginAnd(e))
		{
			ir.type = IR_BEGIN_AND_BLOCK;
			out->emplace_back(ir);
			continue;
		}
		else if (IsEndAnd(e))
		{
			ir.type = IR_END_AND_BLOCK;
			out->emplace_back(ir);
			continue;
		}
		else if (IsBeginOr(e))
		{
			ir.type = IR_BEGIN_OR_BLOCK;
			out->emplace_back(ir);
			continue;
		}
		else if (IsEndOr(e))
		{
			ir.type = IR_END_OR_BLOCK;
			out->emplace_back(ir);
			continue;
		}
		else if (IsBeginInsideIf(e))
		{
			char vec_idx = (((long long)e) >> 16) & 0xff;
			auto last = (own_std::vector<ir_rep>*) &lang_stat->inside_ifs[vec_idx];
			out->insert(out->end(), last->begin(), last->end());

			continue;
		}
		
		ir_val val;
		memset(&val, 0, sizeof(ir_val));
		memset(&ir, 0, sizeof(ir_rep));
		ir.type = IR_NONE;

		switch (e->type)
		{
		case AST_CALL:
		{
			
			ir_val lhs_expr = {};
			if (e->call.lhs)
			{
				lhs_expr = stack.back();
				stack.pop_back();
			}
			int cur_biggest = e->call.in_func->biggest_call_args;
			lang_stat->cur_func->biggest_call_args = max(cur_biggest, e->call.args.size());

			
			int more_stack_vals = stack.size() - e->call.args.size();
			if (more_stack_vals > 0)
			{
				// spilling
				ir_val* to_spill = stack.begin();

				cur_biggest = e->call.in_func->to_spill_size;

				e->call.in_func->to_spill_size = max(cur_biggest, more_stack_vals);

				int cur_spill_offset = 0;

				while (more_stack_vals > 0)
				{
					if (to_spill->type == IR_TYPE_REG)
					{
						ir = {};
						ir.type = IR_ASSIGNMENT;
						ir.assign.to_assign.type = IR_TYPE_ON_STACK;
						ir.assign.to_assign.reg_sz = 8;
						ir.assign.to_assign.deref = -1;
						ir.assign.to_assign.on_stack_type = ON_STACK_SPILL;
						ir.assign.to_assign.i = cur_spill_offset;
						ir.assign.only_lhs = true;
						ir.assign.lhs = *to_spill;
						ir.assign.lhs.deref = -1;
						ir.assign.lhs.reg_sz = 8;
						ir.assign.to_assign.is_float = ir.assign.lhs.is_float;
						//ir.assign.lhs.deref = 1;
						out->emplace_back(ir);


						cur_spill_offset += 8;
					}
					to_spill++;
					more_stack_vals--;
				}
			}

			if (e->call.args.size() > 0)
			{
				for (int i = e->call.args.size() - 1; i >= 0; i--)
				{
					ir_val* top = &stack[stack.size() - 1];
					if(IS_FLAG_OFF(top->reg_ex, IR_VAL_ALREADY_ON_PARAM_REG))
						GenIrToArgReg(lang_stat, stack, i, exps, out);
					if (top->type == IR_TYPE_REG)
						FreeSpecificReg(lang_stat, top->reg);
					stack.pop_back();
				}
			}
			
			ir.type = IR_CALL;

			ir.call.is_outsider = false;
			if (IS_FLAG_ON(e->call.fdecl->flags, FUNC_DECL_IS_OUTSIDER))
				ir.call.is_outsider = true;

			ir.call.fdecl = e->call.fdecl;

			if (e->call.indirect)
			{
				ir.type = IR_INDIRECT_CALL;
				if (e->call.lhs)
				{
					ir.bin.lhs.type = IR_TYPE_REG;
					ir.bin.lhs.deref = -1;
					ir.bin.lhs = lhs_expr;
					ir.bin.lhs.decl = e->call.fdecl->this_decl;
				}
				else
				{
					ir.bin.lhs.type = IR_TYPE_DECL;
					ir.bin.lhs.decl = e->call.func_ptr_var;
					ir.bin.lhs.deref = 0;
				}
				ir.bin.lhs.is_float = false;
			}
			/*
			
			if (e->call.fdecl->name == "memcpy")
			{
				GinIRMemCpy(lang_stat, out);
			}
			else
			*/
				out->emplace_back(ir);



			bool there_is_exps_in_stack = (j + 1) < exps.size();
			// if is not last, we will move to some reg
			if (there_is_exps_in_stack)
			{
				ir = {};
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				if(e->call.fdecl->ret_type.IsFloat())
					ir.assign.to_assign.reg = AllocFloatReg(lang_stat);
				else
					ir.assign.to_assign.reg = AllocReg(lang_stat);
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.is_float = e->call.fdecl->ret_type.IsFloat();
				ir.assign.only_lhs = true;

				ir.assign.lhs.type = IR_TYPE_RET_REG;
				ir.assign.lhs.reg = 0;
				ir.assign.lhs.reg_sz = 8;
				ir.assign.lhs.is_float = e->call.fdecl->ret_type.IsFloat();
				ir.assign.lhs.deref = -1;
				out->emplace_back(ir);
				val = ir.assign.to_assign;
			}
			else
			{
				val.type = IR_TYPE_RET_REG;
				val.reg = 0;
			}
			// copying ret structs to cur_stack
			func_decl* call = e->call.fdecl;
			if (call->ret_type.type == TYPE_STRUCT && call->ret_type.ptr == 0)
			{
				int strct_sz = GetTypeSize(&call->ret_type);
				int cur_offset = lang_stat->cur_strct_ret_size_per_statement;
				lang_stat->cur_strct_ret_size_per_statement += strct_sz;

				// arg_reg0 = &struct_ret
				ir = {};
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg = 32;
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.is_float = false;
				ir.assign.only_lhs = true;

				ir.assign.lhs.type = IR_TYPE_ON_STACK;
				ir.assign.lhs.on_stack_type = ON_STACK_STRUCT_RET;
				ir.assign.lhs.deref = -1;
				ir.assign.lhs.ptr = -1;
				ir.assign.lhs.reg_sz = 8;
				ir.assign.lhs.i = cur_offset;
				out->emplace_back(ir);

				// arg_reg1 = *ret_reg0
				ir = {};
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg = 33;
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.is_float = false;
				ir.assign.only_lhs = true;

				ir.assign.lhs.type = val.type;
				ir.assign.lhs.reg = val.reg;
				ir.assign.lhs.reg_sz = 8;
				ir.assign.lhs.deref = -1;
				out->emplace_back(ir);

				// arg_reg2 = sizeof(struct)
				ir = {};
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg = 3;
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.is_float = false;
				ir.assign.only_lhs = true;

				ir.assign.lhs.type = IR_TYPE_INT;
				ir.assign.lhs.i = strct_sz;
				out->emplace_back(ir);

				/*
				ir.type = IR_CALL;
				type2 dummy;
				ir.call.fdecl = FindIdentifier("memcpy", lang_stat->funcs_scp, &dummy)->type.fdecl;

				out->emplace_back(ir);
				*/

				GinIRMemCpy(lang_stat, out);

				val.type = IR_TYPE_ON_STACK;
				val.on_stack_type = ON_STACK_STRUCT_RET;
				val.reg_sz = 8;
				val.i = cur_offset;

			}
			else
			{
				val.reg_sz = GetTypeSize(&call->ret_type);
				val.is_float = call->ret_type.type == TYPE_F32;
			}
			//unspilling
			if (stack.size() > 0)
			{
				ir_val* begin = stack.begin();

				int cur_spill_offset = 0;
				while (begin < stack.end())
				{
					if (begin->type == IR_TYPE_REG)
					{
						ir = {};
						ir.type = IR_ASSIGNMENT;
						if (there_is_exps_in_stack && begin->reg == 0)
						{
							if (begin->is_float)
							{
								begin->reg = AllocFloatReg(lang_stat);
								begin->deref = -1;
							}
							//else
								//ir.assign.to_assign.reg = AllocReg(lang_stat);
						}
						ir.assign.to_assign = *begin;
						ir.assign.to_assign.deref = -1;
						ir.assign.to_assign.reg_sz = 8;
						ir.assign.only_lhs = true;
						ir.assign.lhs.type = IR_TYPE_ON_STACK;
						ir.assign.lhs.on_stack_type = ON_STACK_SPILL;
						//ir.assign.lhs.deref = 1;
						ir.assign.lhs.i = cur_spill_offset;
						ir.assign.lhs.is_float = begin->is_float;
						ir.assign.lhs.reg_sz = 8;
						out->emplace_back(ir);

						cur_spill_offset += 8;
					}
					begin++;
				}
				
			}
			val.is_packed_float = e->call.fdecl->ret_type.type == TYPE_VECTOR;
			val.is_float = val.is_float || val.is_packed_float;
			val.deref = -1;

			if(call->ret_type.type != TYPE_VOID || call->ret_type.ptr > 0)
			{
				AllocSpecificReg(lang_stat, 0);
				val.ptr = call->ret_type.ptr - 1;
				if(call->ret_type.type == TYPE_VECTOR)
				{
					val.ptr++;
				}

			}

			stack.emplace_back(val);;
		}break;
		case AST_ADDRESS_OF:
		{
			ir_val* top = &stack[stack.size() - 1];


			//ASSERT(top->type == IR_TYPE_DECL);
			//ASSERT(top->ptr > 0);
			//if (e->deref.times == 0) break;
			//top->ptr = -1;

			if (top->type != IR_TYPE_REG)
			{
				ir.type = IR_ASSIGNMENT;

				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg_sz = 8;
				if (top->type == IR_TYPE_ON_STACK)
				{
					ir.assign.to_assign.reg = AllocReg(lang_stat);
				}
				else
				{
					ir.assign.to_assign.reg = AllocReg(lang_stat, 6);
				}
				ir.assign.to_assign.deref = -1;
				ir.assign.only_lhs = true;
				ir.assign.lhs = *top;
				ir.assign.lhs.is_float = false;
				ir.assign.lhs.ptr--;
				ir.assign.lhs.deref--;
					

				*top = ir.assign.to_assign;
				//ir.assign.lhs.ptr = e->deref.times;
				out->emplace_back(ir);
			}
			if (e->goes_onto_stack && top->is_packed_float)
			{
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_ON_STACK;
				ir.assign.to_assign.on_stack_type = ON_STACK_STRUCT_CONSTR;
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.i = e->at_stack_offset;
				ir.assign.only_lhs = true;
				ir.assign.lhs = *top;

				out->emplace_back(ir);

				*top = ir.assign.to_assign;
			}
			top->ptr = -1;
			top->reg_sz = 8;

			if (top->type == IR_TYPE_REG)
			{
				char saved = top->reg;
				top->reg_ex &= ~0x100;
				top->reg = saved;
				top->deref = 0;
			}
			top->is_float = false;
			top->deref = -1;

		}break;
		case AST_ARRAY_COSTRUCTION:
		{
			ir_val* top = &stack.back();
			int offset = e->ar_constr.at_offset;
			int tp_sz = GetTypeSize(&e->ar_constr.type);
			//top->i = offset;

			//ir.assign.to_assign.i = offset;
			int len = e->ar_constr.commas.size();
			for (int i = 0; i < len; i++)
			{
				//ast_struct_construct_info* cinfo = &e->strct_constr.commas[i];
				top = &stack.back();
				stack.pop_back();

				ir.type = IR_ASSIGNMENT;
				/*
				if (top->type == IR_TYPE_REG && top->deref == 0)
					top->deref++;
					*/
				ir.assign.only_lhs = true;
				ir.assign.to_assign.is_unsigned = top->is_unsigned;
				ir.assign.to_assign.type = IR_TYPE_ON_STACK;
				ir.assign.to_assign.on_stack_type = ON_STACK_STRUCT_CONSTR;
				ir.assign.to_assign.i = offset + tp_sz * (len - (i + 1));
				ir.assign.to_assign.reg_sz = tp_sz;
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.is_float = top->is_float;
				ir.assign.lhs = *top;
				out->emplace_back(ir);
				ASSERT(ir.assign.to_assign.reg_sz > 0);
			}
			val.type = IR_TYPE_ON_STACK;
			val.i = offset;//;; -tp_sz * e->ar_constr.commas.size();
			val.reg_sz = ir.assign.to_assign.reg_sz;
			stack.emplace_back(val);
		}break;
		case AST_STRUCT_COSTRUCTION:
		{
			ir_val* top = &stack.back();
			int offset = e->strct_constr.at_offset;
			//top->i = offset;

			int cur_offset = offset;
			//ir.assign.to_assign.i = offset;
			for (int i = e->strct_constr.commas.size() - 1; i >= 0; i--)
			{
				ast_struct_construct_info* cinfo = &e->strct_constr.commas[i];
				top = &stack.back();
				stack.pop_back();

				ir.type = IR_ASSIGNMENT;
				ir.assign.only_lhs = true;
				ir.assign.to_assign.is_unsigned = top->is_unsigned;
				ir.assign.to_assign.type = IR_TYPE_ON_STACK;
				ir.assign.to_assign.on_stack_type = ON_STACK_STRUCT_CONSTR;
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.to_assign.i = cur_offset + cinfo->var->offset;
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.reg_sz = GetTypeSize(&cinfo->var->type);
				ir.assign.lhs = *top;
				out->emplace_back(ir);
				ASSERT(ir.assign.to_assign.reg_sz > 0);
				if (e->strct_constr.is_vector)
					cur_offset += 4;
			}
			val.type = IR_TYPE_ON_STACK;
			val.i = offset;
			val.reg_sz = ir.assign.to_assign.reg_sz;
			val.is_float = e->lhs_tp.type == TYPE_VECTOR_TYPE;
			val.is_packed_float = e->lhs_tp.type == TYPE_VECTOR_TYPE;
			stack.emplace_back(val);
		}break;
		case AST_DEREF:
		{
			ir_val* top = &stack[stack.size() - 1];

			//top->ptr++;
			//break;
			//top->ptr--;
			
			bool was_reg = false;
			if (top->type == IR_TYPE_REG)
			{
				was_reg = true;
				//ASSERT(top->ptr <= 0);
			}


			int val_to_modify = top->type == IR_TYPE_REG ? -1 : 1;
			
			/*
			if (top->is_float && (top->ptr > 0 || top->ptr < 0))
			{
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg = AllocFloatReg(lang_stat);
				ir.assign.to_assign.reg_sz = 4;
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.is_float = true;
				ir.assign.only_lhs = true;
				ir.assign.lhs = *top;
				//ir.assign.lhs.type = IR_TYPE_REG;
				ir.assign.lhs.deref = 0 + top->ptr > 0 ? 1 :0 ;
				out->emplace_back(ir);
				*top = ir.assign.to_assign;
				FreeSpecificReg(lang_stat, top->reg);
			}
			else
			{

			}
			*/
			//if(IS_FLAG_OFF(top->reg_ex, IR_VAL_FROM_POINT))
				top->deref++;
			top->ptr = top->ptr;
			/*
			if(top->ptr == 1)
			{
				if(top->type == IR_TYPE_DECL && top->decl->type.type == TYPE_VECTOR)
				{
					top->is_float = true;
					top->is_packed_float = true;
				}
			}
			else
			{
				//top->is_float = false;
				//top->is_packed_float = false;
			}
			*/
			top->reg_sz = GetTypeSize(&e->deref.type);
			top->reg_sz = min(top->reg_sz, 8);
			top->reg_ex |= IR_VAL_FROM_DEREF;
			/*
			if ((top->ptr + -val_to_modify) != 0)
			{
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				if (top->type == IR_TYPE_REG)
				{
					ir.assign.to_assign = *top;
				}
				else
					ir.assign.to_assign.reg = AllocReg(lang_stat);

				ir.assign.only_lhs = true;
				ir.assign.lhs = *top;
				ir.assign.to_assign.reg_sz = top->reg_sz;
				ir.assign.to_assign.ptr = 0;
				top->ptr = top->ptr + -(e->deref.times * val_to_modify);
				ir.assign.lhs.ptr = top->ptr;
				out->emplace_back(ir);
				//break;
			}
				*/



			//top->type = IR_TYPE_REG;
			//top->reg = ir.assign.to_assign.reg;
			//top->reg_sz = ir.assign.to_assign.reg_sz;

		}break;
		case AST_OPPOSITE:
		{
			ir_val* top = &stack[stack.size() - 1];

            int if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);
            int sub_if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_SUB_IF_BLOCK);
            int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_COND_BLOCK);

			ir = {};
			ir.type = IR_CMP_NE;
			ir.bin.op = T_COND_NE;
			ir.bin.lhs = *top;

			//ir.bin.lhs.deref += 1;
			ir.bin.rhs.type = IR_TYPE_INT;
			ir.bin.rhs.i = 1;
			ir.bin.rhs.is_unsigned = top->is_unsigned;
			out->emplace_back(ir);

            IRCreateEndBlock(lang_stat,cond_idx, out, IR_END_COND_BLOCK);

			int reg = 0;
			if (top->type == IR_TYPE_REG)
				reg = top->reg;
			else
				reg = AllocReg(lang_stat);


			CreateOppositeRegAssigmentAfterCondChecking(lang_stat, out, sub_if_idx, if_idx, reg);
			top->type = IR_TYPE_REG;
			top->reg_sz = 8;
			top->deref = -1;
			top->reg = reg;

		}break;
		case AST_NEGATE:
		{
			ir_val* top = &stack[stack.size() - 1];
			ir.type = IR_ASSIGNMENT;
			ir.assign.op = T_MINUS;

			ir.assign.to_assign.deref = -1;
			if (top->type == IR_TYPE_REG)
			{
				ir.assign.to_assign = *top;
				
			}
			else
			{
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.to_assign.reg = AllocReg(lang_stat);
			}
			ir.assign.rhs = *top;
			ir.assign.lhs.type = IR_TYPE_INT;
			ir.assign.lhs.i = -1;
			ir.assign.lhs.is_unsigned = top->is_unsigned;
			ir.assign.only_lhs = false;
			out->emplace_back(ir);

			*top = ir.assign.to_assign;
		}break;
		case AST_NEGATIVE:
		{
			ir_val* top = &stack[stack.size() - 1];
			ir.type = IR_ASSIGNMENT;
			ir.assign.op = T_MUL;
			ir.assign.to_assign.type = IR_TYPE_REG;
			ir.assign.to_assign.reg = AllocReg(lang_stat);
			ir.assign.to_assign.reg_sz = 8;
			ir.assign.to_assign.deref = -1;
			ir.assign.to_assign.is_float = top->is_float;
			ir.assign.to_assign.is_unsigned = top->is_unsigned;
			//ir.assign.to_assign.deref--;
			ir.assign.lhs = *top;
			//ir.assign.lhs.deref++;
			ir.assign.rhs.type = IR_TYPE_INT;
			ir.assign.rhs.is_unsigned = ir.assign.lhs.is_unsigned;
			if (top->is_float)
			{
				ir.assign.rhs.type = IR_TYPE_F32;
				ir.assign.rhs.f32 = -1.0;
				ir.assign.rhs.is_float = true;
				top->deref = 0;
			}
			else
			{
				ir.assign.rhs.is_float = false;
				ir.assign.rhs.i = -1;
			}
			*top = ir.assign.to_assign;
			out->emplace_back(ir);
			//top->reg_sz = e->cast.type;
		}break;
		case AST_CAST:
		{
			ir_val* top = &stack[stack.size() - 1];

			char last_ptr = top->ptr;
			bool is_int = e->cast.type.type == TYPE_U32 || e->cast.type.type == TYPE_S32 || e->cast.type.type == TYPE_S64 || e->cast.type.type == TYPE_U64;


			if (is_int && top->is_float == true && top->ptr == 0)
			{
				ir = {};
				ir.type = IR_CAST_F32_TO_INT;
				ir.bin.lhs.type = IR_TYPE_REG;
				ir.bin.lhs.deref = -1;
				ir.bin.lhs.reg_sz = 4;
				ir.bin.lhs.reg = AllocReg(lang_stat);
				ir.bin.lhs.is_float = false;
				ir.bin.rhs = *top;

				out->emplace_back(ir);
				*top = ir.bin.lhs;
				//top->deref = -1;

			}
			else if (top->reg_sz < GetTypeSize(&e->cast.type) && !top->is_float && !e->cast.type.IsFloat())
			{
				ir.type = IR_CAST_INT_TO_INT;
				ir.bin.lhs.type = IR_TYPE_REG;
				ir.bin.lhs.deref = -1;
				ir.bin.lhs.reg_sz = GetTypeSize(&e->cast.type);
				ir.bin.lhs.reg_sz = clamp(ir.bin.lhs.reg_sz, 1, 8);
				if (top->type == IR_TYPE_REG)
					ir.bin.lhs.reg = top->reg;
				else
					ir.bin.lhs.reg = AllocReg(lang_stat);

				ir.bin.rhs = *top;

				out->emplace_back(ir);
				*top = ir.bin.lhs;
			}
			else if (e->cast.type.type == TYPE_F32 && e->cast.type.ptr == 0)
			{
				ir = {};
				if(top->is_float)
				{
					ir.type = IR_ASSIGNMENT;
					ir.assign.only_lhs = true;
					ir.assign.to_assign.type = IR_TYPE_REG;
					ir.assign.to_assign.is_float = true;
					ir.assign.to_assign.deref = -1;
					ir.assign.to_assign.reg_sz = 4;
					if (top->type == IR_TYPE_REG)
						ir.assign.to_assign.reg = top->reg;
					else
						ir.assign.to_assign.reg = AllocFloatReg(lang_stat);
					ir.assign.lhs = *top;
					*top = ir.assign.to_assign;
					//top->reg = ir.assign.lhs.reg;
				}
				else
				{
					ir.type = IR_CAST_INT_TO_F32;
					ir.bin.lhs.type = IR_TYPE_REG;
					ir.bin.lhs.deref = -1;
					ir.bin.lhs.reg = AllocFloatReg(lang_stat);
					ir.bin.lhs.is_float = true;
					ir.bin.rhs = *top;
					*top = ir.bin.lhs;
				}
				/*
				if (top->type == IR_TYPE_DECL)
					ir.bin.rhs.deref = 0;
				else
					ir.bin.rhs.deref++;
					*/

				out->emplace_back(ir);

			}
			else if (top->ptr > 0 && e->cast.type.ptr > 0 && top->deref >= 0)
			{
				ir = {};
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg = AllocReg(lang_stat);
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.only_lhs = true;
				ir.assign.lhs = *top;
				ir.assign.lhs.is_float = e->cast.type.IsFloat();
				//ir.assign.lhs.deref = 0;

				out->emplace_back(ir);
				*top = ir.assign.to_assign;
			}
			else if (top->ptr > 0 && e->cast.type.ptr == 0)
			{
				ir = {};
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg = AllocReg(lang_stat);
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.only_lhs = true;
				ir.assign.lhs = *top;
				ir.assign.lhs.is_float = false;
				ir.assign.lhs.is_packed_float = false;
				//ir.assign.lhs.deref = 0;

				out->emplace_back(ir);
				*top = ir.assign.to_assign;
			}
			if (top->type == IR_TYPE_REG && IS_FLAG_ON(top->reg_ex, IR_VAL_FROM_POINT))
			{

				if (top->reg_sz > GetTypeSize(&e->cast.type) && !top->is_float && !e->cast.type.IsFloat())
				{
					ir.type = IR_CAST_INT_TO_INT;
					ir.bin.lhs.type = IR_TYPE_REG;
					ir.bin.lhs.deref = -1;
					//ir.bin.lhs.reg_sz = top->reg_sz;
					ir.bin.lhs.reg_sz = GetTypeSize(&e->cast.type);
					if (top->type == IR_TYPE_REG)
						ir.bin.lhs.reg = top->reg;
					else
						ir.bin.lhs.reg = AllocReg(lang_stat);

					ir.bin.rhs = *top;
					out->emplace_back(ir);
					*top = ir.bin.lhs;
				}
				else if (!top->is_float && !e->cast.type.IsFloat() && IS_FLAG_OFF(top->reg_ex, IR_VAL_FROM_POINT_BUT_IS_STATIC_ARRAY))
				{
					ir.type = IR_ASSIGNMENT;
					ir.assign.to_assign = *top;
					ir.assign.to_assign.deref = -1;
					ir.assign.lhs = *top;
					ir.assign.lhs.deref = 0;
					ir.assign.only_lhs = true;
					out->emplace_back(ir);
					top->deref--;
					//ir.assign = *top;
				}
				top->reg_ex &= ~IR_VAL_FROM_POINT;
				if(top->ptr == 0)
					top->deref = -1;
			}
			//top->deref = -1;
			//if (top->ptr > e->cast.type.ptr && e->cast.type.ptr > 0)
				//top->deref++;
			if (top->ptr < e->cast.type.ptr && top->ptr >= 0)
			{
				char diff = top->ptr - e->cast.type.ptr;
				top->ptr = e->cast.type.ptr - 1;
			}
			else
				top->ptr = e->cast.type.ptr * (last_ptr < 0 ? -1 : 1);
			if (e->cast.type.type != TYPE_VECTOR)
				top->is_float = e->cast.type.IsFloat();
			else
			{
				top->is_float = true;
				top->is_packed_float = true;
				top->ptr = e->cast.type.ptr;
			}
			//top->is_packed_float = 

			if (top->type == IR_TYPE_DECL && top->decl->type.type == TYPE_STATIC_ARRAY && e->cast.type.ptr > 0)
			{
				top->deref = -1;
			}
			//top->reg_ex /&= ~IR_VAL_FROM_POINT;
			/*
			if (e->cast.type.ptr == 0 && top->type != IR_TYPE_INT)
			{
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg = AllocReg(lang_stat);
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.only_lhs = true;
				ir.assign.lhs = *top;
				ir.assign.lhs.deref = 1;
				*top = ir.assign.to_assign;
				out->emplace_back(ir);
			}
			*/
			top->is_unsigned = IsUnsigned(e->cast.type.type);

			top->reg_sz = GetTypeSize(&e->cast.type);
			top->reg_sz = clamp(top->reg_sz, 0, 8);

			//if (e->cast.type.ptr > 0 && top->type != IR_TYPE_REG)// && (top->type != IR_TYPE_REG && top->type != IR_TYPE_RET_REG)) 
				//top->deref++;

		}break;
		case AST_INT:
		case AST_CHAR:
		case AST_STR_LIT:
		case AST_FLOAT:
		case AST_IDENT:
		{
			GetIRVal(lang_stat, e, &val);
			//val.ptr = 0;
			stack.emplace_back(val);
			/*
			if(val.type == IR_TYPE_DECL && val.decl->name == "vertices")
			{
				auto a = 0;
			}
			*/

		}break;
		case AST_INDEX:
		{
			ir_val* top = &stack[stack.size() - 1];
			ir_val* one_minus_top = &stack[stack.size() - 2];

			stack.pop_back();

			char offset_reg = AllocReg(lang_stat);
			ir.type = IR_ASSIGNMENT;
			ir.assign.op = T_MUL;
			ir.assign.to_assign.type = IR_TYPE_REG;
			ir.assign.to_assign.reg = offset_reg;
			ir.assign.to_assign.deref = -1;
			ir.assign.to_assign.ptr = -1;
			ir.assign.to_assign.reg_sz = 8;
			ir.assign.lhs = *top;
			ir.assign.lhs.deref--;
			ir.assign.lhs.is_float = false;
			ir.assign.rhs.type = IR_TYPE_INT;
			ir.assign.rhs.i = GetTypeSize(e->index.lhs_type.tp);
			ir.assign.rhs.is_unsigned = ir.assign.lhs.is_unsigned;


			out->emplace_back(ir);

			ir.type = IR_ASSIGNMENT;
			ir.assign.to_assign.type = IR_TYPE_REG;
			ir.assign.to_assign.reg_sz = 8;
			ir.assign.op = T_PLUS;

			/*
			if (top->type != IR_TYPE_REG)
				ir.assign.to_assign.reg = AllocReg(lang_stat);
			else
				ir.assign.to_assign.reg = top->reg;
				*/
			ir.assign.to_assign.reg = offset_reg;

			ir.assign.rhs.type = IR_TYPE_REG;
			ir.assign.rhs.reg = offset_reg;
			ir.assign.rhs.reg_sz = 8;
			ir.assign.rhs.deref--;
			ir.assign.rhs.is_float = false;
			ir.assign.lhs = *one_minus_top;
			ir.assign.lhs.ptr = -1;
			ir.assign.lhs.deref--;
			ir.assign.lhs.is_unsigned = ir.assign.rhs.is_unsigned;
			ir.assign.lhs.is_float = false;
			if (one_minus_top->ptr < 0)
				ir.assign.lhs.deref++;
			out->emplace_back(ir);

			top = &stack.back();
			//bool was_float = IsIrValFloat(one_minus_top);;
			*top = ir.assign.to_assign;
			top->is_float = e->index.lhs_type.tp->IsFloat();
			top->is_packed_float = e->index.lhs_type.tp->type == TYPE_VECTOR;
			top->is_float = top->is_float || top->is_packed_float;
			top->ptr = -1;
			top->deref = -1;

			/*
			if (one_minus_top->type == IR_TYPE_REG)
			{
				//FreeSpecificReg(lang_stat, one_minus_top->reg);
			}
			*/

		}break;
		case AST_BINOP:
		{
			if (e->op == T_COND_AND || e->op == T_COND_OR)
				continue;
			ASSERT(stack.size() >= 2);
			switch (e->op)
			{
			case T_COND_EQ:
			case T_GREATER_THAN:
			case T_GREATER_EQ:
			case T_LESSER_EQ:
			case T_LESSER_THAN:
			case T_COND_NE:
			{
				ir_val* top = &stack[stack.size() - 1];
				ir_val* one_minus_top = &stack[stack.size() - 2];

				ir.type = IR_CMP_EQ;
				ir.bin.op = e->op;
				ir.bin.lhs = *one_minus_top;
				ir.bin.rhs = *top;
				ir.bin.it_is_jmp_if_true = true;
				out->emplace_back(ir);
				stack.pop_back();
				auto a = 0;
			}break;
			case T_POINT:
			{
				ast_rep* next = exps[min(j + 1, exps.size() - 1)];


				ir_val* first = (stack.end()) - e->points.size();
				/*
				if(first->type == IR_TYPE_REG && first->is_packed_float)
				{
					auto a = 0;
					ir.type == IR_ASSIGNMENT;
					ir.assign.to_assign.type = IR_TYPE_ON_STACK;
					ir.assign.to_assign.i = lang_stat->cur_strct_constrct_size_per_statement;
					ir.assign.


					lang_stat->cur_strct_constrct_size_per_statement += FLOAT_REG_SIZE_BYTES;

					int cur_sz = lang_stat->cur_func->strct_constrct_size_per_statement;
					lang_stat->cur_func->strct_constrct_size_per_statement = max(cur_sz, lang_stat->cur_strct_constrct_size_per_statement);
				}
				*/
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.reg = AllocReg(lang_stat);
				ir.assign.op = e->op;

				ir.assign.only_lhs = false;
				ir.assign.lhs = *first;
				ir.assign.lhs.deref = first->ptr - 2;
				ir.assign.lhs.ptr = -1;

				stack.pop_back();
				ir.assign.rhs.i = 0;

				char prev_ptr = first->ptr;
				ir.assign.rhs.type = IR_TYPE_INT;
				bool added_final_ir = false;
				bool last_was_static_ar = false;
				for (int i = 1; i < e->points.size(); i++)
				{
					last_was_static_ar = false;
					auto cur = first + i;
					auto next = first + i;
					ir.assign.rhs.reg_sz = cur->reg_sz;
					if (prev_ptr == 0)
					{
						ir.assign.rhs.is_float = cur->is_float;
						ir.assign.rhs.is_packed_float = cur->is_packed_float;
						ir.assign.rhs.i += cur->decl->offset;
						added_final_ir = false;
						if ((i + 1) < e->points.size() && next->ptr > 0)
						{
							//ir.assign.lhs.is_unsigned = ir.assign.rhs.is_unsigned;
							out->emplace_back(ir);
							ir.assign.lhs = ir.assign.to_assign;
						}
						if (cur->decl->type.type == TYPE_STATIC_ARRAY)
						{
							last_was_static_ar = true;
						}
					}
					else
					{
						int offset = ir.assign.rhs.i;
						//ir.assign.rhs = *cur;
						ir.assign.rhs.i = cur->decl->offset;
						ir.assign.rhs.is_unsigned = ir.assign.lhs.is_unsigned;
						//ir.assign.rhs.is_float = IsIrValFloat(cur);
						ir.assign.rhs.is_packed_float = IsIrValPackedFloat(cur);
						ir.assign.rhs.is_float = IsIrValFloat(cur) || ir.assign.rhs.is_packed_float;
						ir.assign.lhs.deref = prev_ptr - 1;
						ir.assign.lhs.ptr = prev_ptr;
						if (ir.assign.lhs.type == IR_TYPE_REG && IS_FLAG_ON(ir.assign.lhs.reg_ex, IR_VAL_FROM_POINT))
						{
							ir.assign.lhs.deref--;
							ir.assign.lhs.reg_ex &= ~IR_VAL_FROM_POINT;
						}
						if (ir.assign.lhs.type == IR_TYPE_REG && ir.assign.lhs.is_packed_float)
						{
							ir.assign.lhs.deref--;
						}

						if (cur->decl->type.type == TYPE_STATIC_ARRAY && cur->decl->type.tp->IsFloat())
						{
							ir.assign.rhs.is_float = true;
							last_was_static_ar = true;
						}


						//if(i == (e->points.size() - 1))
						//ir.assign.rhs.ptr = 1;

						out->emplace_back(ir);
						ir.assign.rhs.i = 0;
						added_final_ir = true;
						ir.assign.lhs = ir.assign.to_assign;
					}

					prev_ptr = cur->ptr;
					if (cur->type == IR_TYPE_DECL && cur->decl->type.type == TYPE_STR_LIT)
						prev_ptr = 1;
					stack.pop_back();
				}

				if (!added_final_ir)
				{
					ir.assign.lhs.is_unsigned = ir.assign.rhs.is_unsigned;
					ir.assign.lhs.deref = -1;
					out->emplace_back(ir);
				}

				val = ir.assign.to_assign;
				val.ptr = prev_ptr;
				val.reg_ex |= IR_VAL_FROM_POINT;
				if (last_was_static_ar)
					val.reg_ex |= IR_VAL_FROM_POINT_BUT_IS_STATIC_ARRAY;
				val.deref = 0;
				/*
				if (ir.assign.rhs.type == IR_TYPE_DECL && ir.assign.rhs.decl->type.type == TYPE_STATIC_ARRAY)
				{
					val.ptr *= -1;
				}
				else
					val.deref = 1;
				//val.deref = .ptr + 1;
				*/
				//if(ir)
				val.is_float = ir.assign.rhs.is_float;
				val.is_packed_float = ir.assign.rhs.is_packed_float;
				val.reg_sz = ir.assign.rhs.reg_sz;
				val.is_unsigned = ir.assign.rhs.is_unsigned;
				if (ir.assign.lhs.is_packed_float && ir.assign.lhs.type == IR_TYPE_REG)
				{
					val.deref = -1;
				}
				stack.emplace_back(val);
			}break;
			default:
			{
				ir_val* top = &stack[stack.size() - 1];
				ir_val* one_minus_top = &stack[stack.size() - 2];

				//ir.assign.to_assign.reg = AllocReg(lang_stat);
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.deref = -1;
				ir.assign.to_assign.reg_sz = one_minus_top->reg_sz;
				ir.assign.op = e->op;
				ir.assign.only_lhs = false;
				ir.assign.lhs = stack[stack.size() - 2];
				ir.assign.rhs = stack[stack.size() - 1];
				//ASSERT(ir.assign.lhs.reg_sz == ir.assign.rhs.reg_sz)


				if (ir.assign.lhs.type == IR_TYPE_REG && IS_FLAG_ON(ir.assign.lhs.reg_ex, IR_VAL_FROM_POINT))
				{
					//ir.assign.lhs.deref = 0;
					ir.assign.lhs.reg_ex &= ~IR_VAL_FROM_POINT;
				}

				ir.assign.rhs.is_unsigned = ir.assign.lhs.is_unsigned;
				if (ir.assign.rhs.ptr <= 0 || (ir.assign.rhs.ptr - ir.assign.rhs.deref) == 0 || ir.assign.rhs.deref < 0)
				{
					ir.assign.rhs.is_float = ir.assign.lhs.is_float || ir.assign.lhs.is_packed_float;
					//ir.assign.rhs.is_packed_float = ir.assign.lhs.is_packed_float;
				}
				else
				{
					ir.assign.rhs.is_float = false;
					ir.assign.rhs.is_packed_float = false;
				}


				ir.assign.to_assign.is_float = ir.assign.lhs.is_float;
				ir.assign.to_assign.is_packed_float = ir.assign.lhs.is_packed_float;

				ASSERT(e->op != T_COND_AND && e->op != T_COND_OR);

				if (IsIrValFloat(top))
					ir.assign.to_assign.is_float = true;

				if (top->type == IR_TYPE_REG)
					FreeReg(lang_stat, top->reg);


				/*
				if (ir.assign.lhs.type != IR_TYPE_REG)
					ir.assign.lhs.deref += 1;
				if (ir.assign.rhs.type != IR_TYPE_REG)
					ir.assign.rhs.deref += 1;
			*/

			// reusing the reg
				if (one_minus_top->type == IR_TYPE_REG)
				{
					ir.assign.to_assign.type = IR_TYPE_REG;
					ir.assign.to_assign.reg = one_minus_top->reg;
				}
				else
					ir.assign.to_assign.reg = AllocReg(lang_stat);


				int min_sz = min(top->reg_sz, one_minus_top->reg_sz);
				stack.pop_back();

				//if(one_minus_top->type ==)

				top = &stack[stack.size() - 1];

				top->type = IR_TYPE_REG;
				top->reg_ex = 0;
				top->reg = ir.assign.to_assign.reg;
				if (top->ptr <= -1)
				{
					top->is_float = ir.assign.to_assign.is_float;
					top->is_packed_float = ir.assign.to_assign.is_packed_float;
				}
				top->is_unsigned = ir.assign.lhs.is_unsigned;
				top->reg_sz = min_sz;
				top->ptr = 0;
				top->deref = -1;
				out->emplace_back(ir);
			}break;
			}

			

		}break;
		default:
			ASSERT(0);
		}
		*top_info = stack.back();
	}
}

void GenStackThenIR(lang_state *lang_stat, ast_rep *ast, own_std::vector<ir_rep> *out, ir_val *dst_val, ir_val *top)
{
	own_std::vector<ast_rep*> exps;

	switch (ast->type)
	{
	case AST_CALL:
	{

		PushFuncAsts(lang_stat, ast, &exps);
	}break;
	case AST_CAST:
	case AST_ADDRESS_OF:
	case AST_ARRAY_COSTRUCTION:
	case AST_INDEX:
	case AST_OPPOSITE:
	case AST_NEGATIVE:
	case AST_DEREF:
	{
		PushAstsInOrder(lang_stat, ast, &exps);
	}break;
	case AST_BINOP:
	{
		PushAstsInOrder(lang_stat, ast, &exps);
	}break;
	case AST_FLOAT:
	case AST_INT:
	case AST_CHAR:
	case AST_IDENT:
	{
		GetIRVal(lang_stat, ast, dst_val);
		//dst_val->ptr = 1;
		return;
	}break;
	default:
		ASSERT(0);
	}

	ir_rep ir;
	int begin_complex_idx = out->size();
	ir_val top_info = {};
	GinIRFromStack(lang_stat, exps, out, &top_info);

	if (!dst_val)
		return;;

	bool diff_type = top_info.type != dst_val->type && dst_val->type != IR_NONE && top_info.type != IR_TYPE_INT;
	/*
	if (!diff_type && top_info.reg != dst_val->reg || diff_type)
	{
		ir.type = IR_ASSIGNMENT;
		ir.assign.to_assign = *dst_val;
		ir.assign.to_assign.is_float = false;
		ir.assign.to_assign.reg = dst_val->reg;

		if (top_info.ptr > 0)
			ir.assign.to_assign.reg_sz = 8;

		if(top_info.ptr < 0)
			ir.assign.to_assign.ptr = top_info.ptr;
		else
			ir.assign.to_assign.ptr = 0;
		ir.assign.only_lhs = true;
		ir.assign.lhs = top_info;
		//ir.assign.lhs = top_info;
		ir.assign.lhs.deref += max(dst_val->deref + top_info.ptr - 1, 0);
		
		dst_val->is_float = top_info.is_float;
		dst_val->ptr = top_info.ptr;

		if(dst_val->deref == -1 && top_info.deref > 0 && top_info.type != IR_TYPE_DECL)
			ir.assign.lhs.deref = 0;
		else
			top_info.deref = 0;
		top_info.type = IR_TYPE_REG;
		top_info.reg = dst_val->reg;
		//top_info.reg_sz = dst_val->reg_sz;

		out->emplace_back(ir);
	}
	*/
	if (top)
		*top = top_info;
	//dst_val->reg_ex = top_info.reg_ex;
	//ir.type = IR_END_COMPLEX;
	//out->emplace_back(ir);

	/*
	if (ast->e_holder.expr.size() == 2)
	{
		

	}
	else
	{

	}
	ir_rep ir;

	ASSERT(ast->type == AST_BINOP);
	ast_rep* lhs_exp = ast->e_holder.expr[0];
	ast_rep* rhs_exp = ast->e_holder.expr[1];

	int reg = dst_val->reg;

	ir.type = IR_ASSIGNMENT;
	ir.assign.to_assign = *dst_val;
	ir.assign.op = ast->op;


	ir.assign.only_lhs = false;
	bool lhs_complex = IsAstSimple(lang_stat, lhs_exp);
	bool rhs_complex = IsAstSimple(lang_stat, rhs_exp);

	char type = (char)lhs_complex | (((char)rhs_complex) << 1);
	switch (type)
	{
	// no simple
	case 0:
	{
		GenIRComplex(lang_stat, ast, out, dst_val);
		
	}break;		
	// only lhs simple
	case 1:
	{
		ASSERT(0);
	}break;		
	// only rhs simple
	case 2:
	{
		ASSERT(0);
	}break;		
	// both simple
	case 3:
	{
		GetIRVal(lang_stat, lhs_exp, &ir.assign.lhs);
		GetIRVal(lang_stat, rhs_exp, &ir.assign.rhs);
	}break;		
	}

	out->emplace_back(ir);

	for(int i = 2; i < ast->e_holder.expr.size(); i++)
	{
		ast_rep* cur = ast->e_holder.expr[i];

		if (IsAstSimple(lang_stat, cur))
		{
			ir.assign.lhs.type = IR_TYPE_REG;
			ir.assign.lhs.reg = reg;

			GetIRVal(lang_stat, cur, &ir.assign.rhs);
		}
		else
			ASSERT(0);
		out->emplace_back(ir);
		 
	}
	*/
}
void DoStructConstruction(lang_state* lang_stat, ast_rep* ast, ir_val_type dts_type, void* data, own_std::vector<ir_rep> *out)
{
	ir_rep ir;
	ir.type = IR_ASSIGNMENT;
	ir.assign.to_assign.type = IR_TYPE_ON_STACK;
	ir.assign.to_assign.reg_sz = 8;
	//ir.assign.to_assign.i = ast->strct_constr.at_offset;
	FOR_VEC(info, ast->strct_constr.commas)
	{
		ir.assign.to_assign.i = ast->strct_constr.at_offset + info->var->offset;

		if (IsAstSimple(lang_stat, info->exp))
		{
			ir.assign.only_lhs = true;
			GetIRVal(lang_stat, info->exp, &ir.assign.lhs);
		}
		else
			ASSERT(0);

		out->emplace_back(ir);
	}
}

void GenIRMaybeComplex(lang_state* lang_stat, ast_rep* ast, own_std::vector<ir_rep>* out, ir_val *out_val, ir_val *complex_info, bool lhs_equal = false)
{
	ASSERT(0)
	if (IsAstSimple(lang_stat, ast))
	{
		GetIRVal(lang_stat, ast, out_val);
	}
	else
	{
		// making sure we have some ptr to assign to if we're trying to modify it, instead of just its value
		if (lhs_equal && ast->type == AST_DEREF)
			// decrement and also making sure its not lesser than 0
			ast->deref.times = max(0, ast->deref.times - 1);

		if (complex_info->reg == -1)
			complex_info->reg = AllocReg(lang_stat);
		else
			AllocSpecificReg(lang_stat, complex_info->reg);
		GenIRComplex(lang_stat, ast, out, complex_info);
		*out_val = *complex_info;
	}
}

int GetAstTypeSize(lang_state* lang_stat, ast_rep* ast)
{
	switch (ast->type)
	{
	case AST_ARRAY_COSTRUCTION:
	case AST_ADDRESS_OF:
	{
		return 8;
	}break;
	case AST_FLOAT:
	case AST_INT:
	{
		return 4;
	}break;
	case AST_CALL:
	{
		return GetTypeSize(&ast->call.fdecl->ret_type);
	}break;
	case AST_BINOP:
	{
		if (ast->op == T_POINT)
		{
			return GetAstTypeSize(lang_stat, ast->points.back().exp);
		}
		else
			return GetAstTypeSize(lang_stat, ast->e_holder.expr[0]);
	}break;
	case AST_CAST:
	{
		int prev = ast->cast.type.ptr;

		//ast->cast.type.ptr = max(0, prev - 1);

		int sz = GetTypeSize(&ast->cast.type);
		ast->cast.type.ptr = prev;
		return  sz;
	}break;
	case AST_NEGATIVE:
	{
		return GetAstTypeSize(lang_stat, ast->ast);
	}break;
	case AST_DEREF:
	{
		return GetAstTypeSize(lang_stat, ast->deref.exp);
	}break;
	case AST_INDEX:
	{
		return GetTypeSize(&ast->index.lhs_type);
	}break;
	case AST_IDENT:
	{
		return GetTypeSize(&ast->decl->type);
	}break;
	default:
		ASSERT(0);
	}
	return 0;
}

void GenIfExpr(lang_state* lang_stat, ast_rep* ast, own_std::vector<ir_rep>* out, ir_val *top = nullptr)
{
	ir_rep ir = {};

	ir.type = IR_ASSIGNMENT;
	ir.assign.to_assign.type = IR_TYPE_RET_REG;
	ir.assign.to_assign.reg_sz = 8;
	ir.assign.to_assign.reg = 0;
	ir.assign.to_assign.deref = -1;
	ir.assign.only_lhs = true;

	GenStackThenIR(lang_stat, ast, out, &ir.assign.lhs);
	ir.assign.to_assign.is_float = ir.assign.lhs.is_float;
	if (top)
		*top = ir.assign.to_assign;
	//ir.assign.lhs.deref++;
	out->emplace_back(ir);

}
void GenLhsEqual(lang_state* lang_stat, ast_rep* lhs_ast, type2 *lhs_tp, own_std::vector<ir_rep>* out, ir_val *assign)
{
	ir_rep ir = {};
	ir.type = IR_ASSIGNMENT; 
	/*

	// 
	ir.assign.to_assign.deref = -1;

	ir.assign.to_assign.reg_sz = min(8, GetTypeSize(lhs_tp));
	ASSERT(ir.assign.to_assign.reg_sz != 0);

	*/
	GenStackThenIR(lang_stat, lhs_ast, out, assign, assign);
	assign->deref--;

	//ir.assign.lhs

	/*
	if (ir.assign.to_assign.deref == -1)
		ir.assign.to_assign.deref = 0;


	ir.assign.to_assign.reg_sz = min(8, GetTypeSize(lhs_tp));
	ASSERT(ir.assign.to_assign.reg_sz != 0);

	char final_deref = -1;
	if (ir.assign.to_assign.type == IR_TYPE_DECL)
	{
		final_deref++;
	}
	//ir.assign.to_assign.deref = 0;
	else if (ir.assign.to_assign.type == IR_TYPE_REG)
	{
		final_deref++;

	}
		//ir.assign.to_assign.deref = 1;
	else
		ASSERT(0);


	final_deref += ir.assign.to_assign.deref;

	ir.assign.to_assign.deref = final_deref;

	if (ir.assign.to_assign.deref > 0 && ir.assign.to_assign.type == IR_TYPE_DECL)
	{
		ir.assign.to_assign.reg = AllocReg(lang_stat);
		ir.assign.to_assign.reg_sz = 8;
		ir.assign.to_assign.type = IR_TYPE_REG;
	}

	//ir.assign.to_assign.deref = ir.assign.to_assign.ptr + (ir.assign.to_assign.deref) - 1;
	ASSERT(ir.assign.to_assign.deref >= 0);
	*assign = ir.assign.to_assign;
	*/
}
short MaybeAllocDiffAssignReg(lang_state* lang_stat, ir_val &ir, own_std::vector<ir_rep> *out) 
{
	char final_reg = ir.reg;
	if (ir.type == IR_TYPE_REG || ir.deref >=0)
	{
		final_reg = AllocReg(lang_stat);
		ir_rep new_ir = {};
		new_ir.type = IR_ASSIGNMENT;
		new_ir.assign.to_assign.type = IR_TYPE_REG;
		new_ir.assign.to_assign.reg = final_reg;
		new_ir.assign.to_assign.reg_sz = 8;
		new_ir.assign.to_assign.deref = -1;
		new_ir.assign.only_lhs = true;
		new_ir.assign.lhs = ir;
		new_ir.assign.lhs.deref = ir.ptr - 1;
		ir.type = IR_TYPE_REG;
		out->emplace_back(new_ir);
		
	}
	return final_reg;
}

void GetIRFromAst(lang_state *lang_stat, ast_rep *ast, own_std::vector<ir_rep> *out, ir_val *top)
{
	ir_rep ir = {};
    switch(ast->type)
    {
	case AST_LABEL:
	{
		ir.type = IR_LABEL;
		ir.decl = ast->decl;
		out->emplace_back(ir);
	}break;
	case AST_FLOAT:
	case AST_INT:
	{

	}break;
	case AST_UNOP:
	{
		ASSERT(0);
	}break;
	case AST_FUNC:
	{
		func_decl* last_func = lang_stat->cur_func;
		lang_stat->cur_func = ast->func.fdecl;
		FOR_VEC(arg, ast->func.fdecl->vars)
		{
			decl2* a = *arg;
			if (a->type.type == TYPE_TEMPLATE)
				continue;
            ir.fdecl = ast->func.fdecl;
			if (IS_FLAG_OFF(a->flags, DECL_IS_ARG))
			{
				ir.type = IR_DECLARE_LOCAL;
				ir.decl = a;
				out->emplace_back(ir);
			}
		}
        ir.type = IR_STACK_BEGIN;
		ir.fdecl = ast->func.fdecl;
        //ir.num  = ast->func.fdecl->stack_size;
		ir.fdecl->biggest_call_args = 0;
        out->emplace_back(ir);

		FOR_VEC(arg, ast->func.fdecl->vars)
		{
			decl2* a = *arg;
			if (a->type.type == TYPE_TEMPLATE)
				continue;
            ir.fdecl = ast->func.fdecl;
			if (IS_FLAG_ON(a->flags, DECL_IS_ARG))
			{
				ir.type = IR_DECLARE_ARG;

				ir.decl = a;
				out->emplace_back(ir);
			}
		}

		GetIRFromAst(lang_stat, ast->func.stats, out);

        ir.type = IR_STACK_END;
        //ir.num  = ast->func.fdecl->stack_size;
        out->emplace_back(ir);
		lang_stat->cur_func = last_func;
	}break;
    case AST_CALL:
	{
		GenStackThenIR(lang_stat, ast, out, nullptr);
		return;
	}break;
    case AST_RET:
	{
		ir.type = IR_RET;
		ast_rep *rhs_ast = ast->ret.ast;
		ir.ret.no_ret_val = false;
		if (!rhs_ast)
		{
			ir.ret.no_ret_val = true;
			out->emplace_back(ir);
			return;
		}
		ir.ret.assign.to_assign.type = IR_TYPE_RET_REG;
		ir.ret.assign.to_assign.reg = 0;
		ir.ret.assign.to_assign.deref = -1;
		ir.ret.assign.to_assign.is_float = lang_stat->cur_func->ret_type.IsFloat();
		ir.ret.assign.to_assign.reg_sz = 8;

		switch (rhs_ast->type)
		{
		case AST_CAST:
		case AST_DEREF:
		case AST_CALL:
		{
			GenStackThenIR(lang_stat, rhs_ast, out, &ir.ret.assign.to_assign, &ir.ret.assign.lhs);
			ir.ret.assign.only_lhs = true;
		}break;
		case AST_NEGATIVE:
		case AST_BINOP:
		{
			ir.ret.assign.only_lhs = true;
			ir.ret.assign.lhs.type = IR_TYPE_REG;
			//ir.ret.assign.lhs.reg = AllocReg(lang_stat);
			ir.ret.assign.lhs.reg_sz = 8;

			GenStackThenIR(lang_stat, rhs_ast, out, &ir.ret.assign.lhs, &ir.ret.assign.lhs);
		}break;
		case AST_IDENT:
		case AST_INT:
		case AST_STR_LIT:
		case AST_FLOAT:
		{
			ir.ret.assign.only_lhs = true;
			GetIRVal(lang_stat, rhs_ast, &ir.ret.assign.lhs);
			ir_val* val = &ir.ret.assign.lhs;
			if (val->type == IR_TYPE_DECL && val->decl->type.type == TYPE_STRUCT && val->ptr == 0)
			{
				val->ptr = -1;
				val->reg_sz = 8;
				val->deref = -1;
			}
			
		}break;
		{
			ir.ret.assign.only_lhs = true;
			GetIRVal(lang_stat, rhs_ast, &ir.ret.assign.lhs);
		}break;
		default:
			ASSERT(0)
		}
		/*
		//if()
		if (ast->ret.tp.type == TYPE_STRUCT && ast->ret.tp.ptr == 0)
			ir.ret.assign.lhs.deref = 0;
		else
			ir.ret.assign.lhs.deref += 1;
			*/
		//if(ir.ret.assign.lhs.type == IR_TYPE_REG)
		//ir.ret.assign.lhs.deref--;


		ir.ret.assign.lhs.is_float = ir.ret.assign.to_assign.is_float;
		out->emplace_back(ir);
	}break;
    case AST_STATS:
	{
		FOR_VEC(st, ast->stats)
		{
			FreeAllRegs(lang_stat);
			FreeAllFloatRegs(lang_stat);
			ast_rep* s = *st;
			if (s->type == AST_EMPTY)
				continue;



			if (s->type == AST_WHILE || s->type == AST_IF)
			{
				GetIRFromAst(lang_stat, s, out);
			}
			else
			{
				ASSERT(!lang_stat->ir_in_stmnt)
				lang_stat->ir_in_stmnt = true;

				bool can_emplace_stmnt = s->type != AST_IDENT && s->type != AST_FOR && !s->dont_make_dbg_stmnt;
				int stmnt_idx = 0;
				if(can_emplace_stmnt)
					stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT, (void *)(long long)s->line_number);

				GetIRFromAst(lang_stat, s, out);

				if(can_emplace_stmnt)
					IRCreateEndBlock(lang_stat, stmnt_idx, out, IR_END_STMNT);

				lang_stat->ir_in_stmnt = false;
			}



			int cur_sz = lang_stat->cur_func->strct_ret_size_per_statement;
			lang_stat->cur_func->strct_ret_size_per_statement = max(cur_sz, lang_stat->cur_strct_ret_size_per_statement);
			lang_stat->cur_strct_ret_size_per_statement = 0;

			int i = 0;
		}
	}break;
    case AST_MINUS_MINUS:
    case AST_PLUS_PLUS:
	{
		FreeAllRegs(lang_stat);
		GenLhsEqual(lang_stat, ast->unop_assign.ast, &ast->unop_assign.tp, out, &ir.assign.to_assign);
		//ir.assign.to_assign.deref++;
		
		ir.type = IR_ASSIGNMENT;
		ir.assign.only_lhs = false;
		ir.assign.op = ast->type == AST_PLUS_PLUS ? T_PLUS : T_MINUS;
		ir.assign.lhs = ir.assign.to_assign;


		ir.assign.lhs.deref = ir.assign.lhs.ptr;
		ir.assign.lhs.is_unsigned = ir.assign.rhs.is_unsigned;
		ir.assign.rhs.type = IR_TYPE_INT;
		ir.assign.rhs.i = 1;

		if (ir.assign.to_assign.type == IR_TYPE_REG)
			ir.assign.to_assign.deref++;
		if (IsIrValFloat(&ir.assign.lhs))
			ir.assign.to_assign.is_float = true;

		ir.assign.to_assign.reg = MaybeAllocDiffAssignReg(lang_stat, ir.assign.to_assign, out);

		out->emplace_back(ir);
		//ir.assign.rhs.ptr++////;

	}break;
    case AST_BINOP:
    {
        switch(ast->op)
        {
        case T_PLUS_EQUAL:
        case T_MINUS_EQUAL:
        case T_EQUAL:
        {
			FreeAllRegs(lang_stat);
            ir_val lhs;
            ir_val rhs;

            ast_rep *rhs_ast = ast->e_holder.expr[1];
            ast_rep *lhs_ast = ast->e_holder.expr[0];

			ir.type = IR_ASSIGNMENT;
            

			ir_val rhs_top = {};
			switch (rhs_ast->type)
			{
			case AST_GET_FUNC_BC:
			{
				ir.assign.only_lhs = true;
				ir.assign.lhs.type = IR_TYPE_GET_FUNC_BC;
				ir.assign.lhs.fdecl = rhs_ast->call.fdecl;
			}break;
			case AST_IF:
			{
				GetIRFromAst(lang_stat, rhs_ast, out, &ir.assign.lhs);
				ir.assign.only_lhs = true;
				ir.assign.lhs.type = IR_TYPE_RET_REG;
				ir.assign.lhs.deref = -1;
				ir.assign.lhs.reg = 0;
				//ir.assign.lhs.is_float = ast->cond.expr_type.IsFloat();

			}break;
			case AST_CALL:
			case AST_BINOP:
			case AST_CAST:
			case AST_INDEX:
			case AST_DEREF: 
			case AST_OPPOSITE: 
			case AST_NEGATIVE: 
			case AST_ARRAY_COSTRUCTION:
			case AST_ADDRESS_OF: 
			{
				ir.assign.lhs.type = IR_TYPE_REG;
				//ir.assign.lhs.reg = AllocReg(lang_stat);
				ir.assign.lhs.reg_sz = GetTypeSize(&ast->lhs_tp);

				ir.assign.only_lhs = true;
				ir.assign.lhs.deref = -1;
				GenStackThenIR(lang_stat, rhs_ast, out, &ir.assign.lhs, &rhs_top);
				ir.assign.lhs = rhs_top;
				if (ir.assign.lhs.reg_sz > 8 && ast->lhs_tp.type == TYPE_VECTOR)
					ir.assign.lhs.reg_sz = 8;
				//ir.assign.lhs.deref--;// = rhs_top;
				//ir.assign.lhs.deref = -1;

			}break;
			case AST_IDENT:
			{
				ir.assign.only_lhs = true;
				GetIRVal(lang_stat, rhs_ast, &ir.assign.lhs);
				//ir.assign.lhs.ptr = 1;

			}break;
			case AST_INT: 
			case AST_FLOAT: 
			case AST_STR_LIT:
			case AST_CHAR:
			{
				ir.assign.only_lhs = true;
				GetIRVal(lang_stat, rhs_ast, &ir.assign.lhs);
				ir.assign.lhs.ptr = 1;

			}break;
			default:
				ASSERT(0)
			}


			int had_spilled_reg = -1;
			MaybeSpillRegisters(lang_stat, lhs_ast, &had_spilled_reg, out);

			GenStackThenIR(lang_stat, lhs_ast, out, &ir.assign.to_assign, &ir.assign.to_assign);
			ir.assign.to_assign.deref--;

			MaybeUnspillRegisters(lang_stat, &had_spilled_reg, out);

			//CheckIrValIsPointIncDeref(&ir.assign.to_assign);
			if (IS_FLAG_ON(ir.assign.to_assign.reg_ex, IR_VAL_FROM_POINT) && ir.assign.to_assign.type != IR_TYPE_DECL)
			{
				ir.assign.to_assign.deref++;
			}
			if(ir.assign.to_assign.type == IR_TYPE_REG && ir.assign.to_assign.deref < 0)
				ir.assign.to_assign.deref +=  -ir.assign.to_assign.deref;


			//ir.assign.to_assign.deref += ir.assign.to_assign.ptr;
			//GenLhsEqual(lang_stat, lhs_ast, &ast->lhs_tp, out, &ir.assign.to_assign);

			//GetIRVal(lang_stat, ast->e_holder.expr[0], &ir.assign.to_assign);
			if (ast->op == T_PLUS_EQUAL || ast->op == T_MINUS_EQUAL)
			{
				ir.assign.rhs = ir.assign.lhs;
				if(ir.assign.to_assign.type == IR_TYPE_DECL && ir.assign.to_assign.deref >= 0)
				{
					ir_rep new_ir = {};
					new_ir.type = IR_ASSIGNMENT;
					new_ir.assign.to_assign.type = IR_TYPE_REG;
					new_ir.assign.to_assign.reg = AllocReg(lang_stat);
					new_ir.assign.to_assign.reg_sz = 8;
					new_ir.assign.to_assign.deref = -1;
					new_ir.assign.lhs = ir.assign.to_assign;
					new_ir.assign.lhs.deref++;
					//new_ir.assign.lhs.deref = 0;
					new_ir.assign.only_lhs = true;

					out->emplace_back(new_ir);

					ir.assign.lhs = new_ir.assign.to_assign;

				}
				else
				{
					ir.assign.lhs = ir.assign.to_assign;
					ir.assign.lhs.deref = 0;
				}
				short final_reg = MaybeAllocDiffAssignReg(lang_stat, ir.assign.to_assign, out);
				ir.assign.only_lhs = false;
				ir.assign.op = ast->op == T_PLUS_EQUAL ? T_PLUS : T_MINUS;
				ir.assign.lhs.is_unsigned = ir.assign.rhs.is_unsigned;
				ir.assign.lhs.is_float = ir.assign.rhs.is_float;
				ir.assign.to_assign.reg = final_reg;

				if (IsIrValFloat(&ir.assign.lhs))
					ir.assign.to_assign.is_float = true;
				//ir.assign.rhs.ptr++////;
			}
			if (ir.assign.lhs.ptr == -1)
				ir.assign.to_assign.is_float = false;
			/*
			if (ir.assign.to_assign.type == IR_TYPE_DECL && ir.assign.to_assign.decl->type.type == TYPE_FUNC_PTR)
				ir.assign.lhs.deref = 0;
			if (ir.assign.to_assign.type == IR_TYPE_REG)
				ir.assign.to_assign.deref=1;
				*/
			//if(ir.assign.to_assign.deref >= 0 && IS_FLAG_OFF(ir.assign.to_assign.reg_ex, IR_VAL_FROM_POINT))
				//ir.assign.to_assign.is_float = false;
			ASSERT(ir.assign.to_assign.type != IR_TYPE_NONE);

			if (ir.assign.only_lhs &&( ir.assign.to_assign.ptr >= 1 && ir.assign.lhs.ptr >= 1 || ir.assign.lhs.ptr >= 1))
			{
				ir.assign.to_assign.is_float = false;
				ir.assign.to_assign.is_packed_float = false;
				ir.assign.lhs.is_float = false;
				ir.assign.lhs.is_packed_float = false;
			}
			
			/*
			if (ir.assign.to_assign.type == IR_TYPE_REG && ir.assign.to_assign.is_float && ir.assign.to_assign.deref > 0)
			{
				ir.assign.to_assign.is_float = false;
				ir.assign.to_assign.type = IR_TYPE_REG;
			}
			*/
			ASSERT(ir.assign.lhs.reg_sz <= 8);
			ASSERT(ir.assign.rhs.reg_sz <= 8);
			ASSERT(ir.assign.to_assign.reg_sz <= 8);
			//ASSERT(ir.assign.to_assign.deref >= -1);
			out->emplace_back(ir);
			FreeAllRegs(lang_stat);
        }break;
		case T_COND_AND:
		{
            FOR_VEC(expr, ast->e_holder.expr)
            {
                ast_rep *e = *expr;
                if(e->type == AST_BINOP && e->op == T_COND_OR)
                {
                    int block_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_AND_BLOCK);

                    GetIRFromAst(lang_stat, e, out);

                    IRCreateBeginBlock(lang_stat, out, IR_END_AND_BLOCK);

                }
                else if(e->type == AST_BINOP && e->op == T_COND_AND)
                {
                    int block_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_AND_BLOCK);

                    GetIRFromAst(lang_stat, e, out);

                    IRCreateBeginBlock(lang_stat, out, IR_END_AND_BLOCK);

                }
				else if (e->type == AST_BINOP && e->op != T_POINT)
				{
					tkn_type2 op;
					tkn_type2 opposite = OppositeCondCmp(e->op);
					e->op = opposite;
					// false is for the it_is_jmp_if_true var in ir_rep
					GetIRBin(lang_stat, e, out, FromTokenOpToIRType(opposite), (void *)false);

				}
                else
                {
					ir_rep ir = {};
					ir.bin.lhs.type = IR_TYPE_REG;
					ir.bin.lhs.reg_sz = 8;
					ir.bin.lhs.reg = AllocReg(lang_stat);
					GenStackThenIR(lang_stat, e, out, &ir.bin.lhs, &ir.bin.lhs);
					//ir.bin.lhs.deref++;
					ir.bin.rhs.type = IR_TYPE_INT;
					ir.bin.rhs.i = 1;
					ir.type = IR_CMP_NE;
					ir.bin.it_is_jmp_if_true = false;
					ir.bin.op = T_COND_NE;
					out->emplace_back(ir);

                    //GetIRFromAst(lang_stat, e, out);
                }
				FreeAllRegs(lang_stat);
				FreeAllFloatRegs(lang_stat);
            }
		}break;
        case T_COND_OR:
        {
			int idx = 0;
            FOR_VEC(expr, ast->e_holder.expr)
            {
                ast_rep *e = *expr;
				bool last = idx == (ast->e_holder.expr.size() - 1);
                if(last && e->type == AST_BINOP && e->op == T_COND_AND)
                {
                    GetIRFromAst(lang_stat, e, out);
                }
                else if(e->type == AST_BINOP && e->op == T_COND_OR)
                {
                    int block_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_OR_BLOCK);

                    GetIRFromAst(lang_stat, e, out);

                    IRCreateBeginBlock(lang_stat, out, IR_END_OR_BLOCK);

                }
                else if(e->type == AST_BINOP && e->op == T_COND_AND)
                {
                    int block_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_OR_BLOCK);

                    GetIRFromAst(lang_stat, e, out);

                    IRCreateBeginBlock(lang_stat, out, IR_END_OR_BLOCK);

                }
				else if (e->type == AST_BINOP && e->op != T_POINT)
				{
					tkn_type2 op = e->op;
					if (idx == (ast->e_holder.expr.size() - 1))
					{
						tkn_type2 opposite = OppositeCondCmp(e->op);
						e->op = opposite;
						GetIRBin(lang_stat, e, out, FromTokenOpToIRType(e->op), (void *)false);
					}
					else
						GetIRBin(lang_stat, e, out, FromTokenOpToIRType(e->op), (void *)true);

				}
                else
                {
					ir_rep ir = {};
					ir.bin.lhs.type = IR_TYPE_REG;
					ir.bin.lhs.reg_sz = 8;
					ir.bin.lhs.reg = AllocReg(lang_stat);
					GenStackThenIR(lang_stat, e, out, &ir.bin.lhs, &ir.bin.lhs);
					ir.bin.rhs.type = IR_TYPE_INT;
					ir.bin.it_is_jmp_if_true = true;
					ir.bin.rhs.i = 1;
					ir.type = IR_CMP_EQ;
					ir.bin.op = T_COND_EQ;
					if (last)
					{
						ir.bin.it_is_jmp_if_true = false;
						ir.type = IR_CMP_NE;
						ir.bin.op = T_COND_NE;
					}
					out->emplace_back(ir);
                }
				idx++;
				FreeAllRegs(lang_stat);
				FreeAllFloatRegs(lang_stat);
            }
        }break;
		case T_COND_NE:
        {
			GetIRBin(lang_stat, ast, out, IR_CMP_NE);
        }break;
        case T_COND_EQ:
        {
			GetIRBin(lang_stat, ast, out, IR_CMP_EQ);
        }break;
		default:
			ASSERT(0)

        }
    }break;
	case AST_IDENT: break;
	case AST_WHILE:
    {
		bool prev_is_in_stmnt = lang_stat->ir_in_stmnt;
		lang_stat->ir_in_stmnt = false;
		int block_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_BLOCK);
		int loop_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_LOOP_BLOCK);
		int sub_block_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_BLOCK);
		int stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT, (void *)(long long)ast->line_number);
		//int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);
		GetIRCond(lang_stat, ast->loop.cond, out);
		IRCreateEndBlock(lang_stat, stmnt_idx, out, IR_END_STMNT);

		if (ast->loop.scope)
		{
			GetIRFromAst(lang_stat, ast->loop.scope, out);
		}

		IRCreateEndBlock(lang_stat, sub_block_idx, out, IR_END_BLOCK);
		//IRCreateEndBlock(lang_stat, cond_idx, out, IR_END_IF_BLOCK);
		IRCreateEndBlock(lang_stat, loop_idx, out, IR_END_LOOP_BLOCK);
		IRCreateEndBlock(lang_stat, block_idx, out, IR_END_BLOCK);

		lang_stat->ir_in_stmnt = prev_is_in_stmnt;
    }break;
	case AST_DBG_BREAK:
	{
		if (lang_stat->release)
			ir.type = IR_NOP;
		else
		{
			//int stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT, (void *)(long long)ast->line_number);
			ir.type = IR_DBG_BREAK;
			//IRCreateEndBlock(lang_stat, stmnt_idx, out, IR_END_STMNT);
		}
		out->emplace_back(ir);
	}break;
	case AST_BREAK:
	{
		ir.type = IR_BREAK;
		out->emplace_back(ir);
	}break;
	case AST_FOR:
	{
		bool prev_is_in_stmnt = lang_stat->ir_in_stmnt;
		lang_stat->ir_in_stmnt = false;
		int main_block_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_BLOCK);
		int stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT, (void *)(long long)ast->line_number);
		GetIRFromAst(lang_stat, ast->for_info.start_stat, out);
		int loop_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_LOOP_BLOCK);
		int block_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_BLOCK);
		//int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);
		GetIRCond(lang_stat, ast->for_info.cond_stat, out);
		IRCreateEndBlock(lang_stat, stmnt_idx, out, IR_END_STMNT);

		if (ast->for_info.scope)
		{
			GetIRFromAst(lang_stat, ast->for_info.scope, out);
		}

		ir_rep* for_ir = &(*out)[loop_idx];
		for_ir->block.is_for_loop = true;
		for_ir->block.for_loop_end_stat = out->size();
		IRCreateEndBlock(lang_stat, block_idx, out, IR_END_BLOCK);
		GetIRFromAst(lang_stat, ast->for_info.at_loop_end_stat, out);
		//IRCreateEndBlock(lang_stat, cond_idx, out, IR_END_IF_BLOCK);
		IRCreateEndBlock(lang_stat, loop_idx, out, IR_END_LOOP_BLOCK);

		IRCreateEndBlock(lang_stat, main_block_idx, out, IR_END_BLOCK);

		lang_stat->ir_in_stmnt = prev_is_in_stmnt;

	}break;
	case AST_IF:
    {
		FreeAllRegs(lang_stat);

		bool is_stmnt_without_semicolon = ast->cond.scope->stats.back()->stmnt_without_semicolon;
		bool was_in_stmnt = lang_stat->ir_in_stmnt;
		if (is_stmnt_without_semicolon)
			lang_stat->ir_in_stmnt = false;
		/*

		int stmnt_without_semicolon_idx = 0;

		if(is_stmnt_without_semicolon)
			stmnt_without_semicolon_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_EXPR_BLOCK, (void *)(long long)ast->line_number);
		*/
		int stmnt_idx = 0;
		if(!lang_stat->ir_in_stmnt)
			stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT, (void *)(long long)ast->line_number);
        int if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);

        bool has_elses = ast->cond.elses.size();

        int sub_if_idx = 0;
        if(has_elses)
            sub_if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_SUB_IF_BLOCK);

		int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_COND_BLOCK);
		GetIRCond(lang_stat, ast->cond.cond, out);
		if(!lang_stat->ir_in_stmnt)
			IRCreateEndBlock(lang_stat, stmnt_idx, out, IR_END_STMNT);
        //GetIRFromAst(lang_stat, ast->cond.cond, out);
		IRCreateEndBlock(lang_stat,cond_idx, out, IR_END_COND_BLOCK);

        if(ast->cond.scope)
        {
            GetIRFromAst(lang_stat, ast->cond.scope, out);
			if (is_stmnt_without_semicolon)
				GenIfExpr(lang_stat, ast->cond.scope->stats.back(), out, top);
            if(has_elses)
            {
                ir.type = IR_BREAK_OUT_IF_BLOCK;
                out->emplace_back(ir);
            }

        }

        if(has_elses)
            IRCreateEndBlock(lang_stat, sub_if_idx, out, IR_END_SUB_IF_BLOCK);

        int i = 0;
        FOR_VEC(el, ast->cond.elses)
        {
            ast_rep *e = *el;

            bool is_last = (i + 1) >= ast->cond.elses.size();

            if(!is_last)
                sub_if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_SUB_IF_BLOCK);

			if (e->type == AST_ELSE_IF)
			{
				stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT, (void *)(long long)e->line_number);
				cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_COND_BLOCK);
				GetIRCond(lang_stat, e->cond.cond, out);
				IRCreateEndBlock(lang_stat,cond_idx, out, IR_END_COND_BLOCK);
				IRCreateEndBlock(lang_stat, stmnt_idx, out, IR_END_STMNT);
			}

            GetIRFromAst(lang_stat, e->cond.scope, out);

			if (is_stmnt_without_semicolon)
				GenIfExpr(lang_stat, e->cond.scope->stats.back(), out);

            if(!is_last)
            {
                ir.type = IR_BREAK_OUT_IF_BLOCK;
                out->emplace_back(ir);
                IRCreateEndBlock(lang_stat, sub_if_idx, out, IR_END_SUB_IF_BLOCK);
            }
        }

        IRCreateEndBlock(lang_stat, if_idx, out, IR_END_IF_BLOCK);
		if (is_stmnt_without_semicolon)
			lang_stat->ir_in_stmnt = was_in_stmnt;

    }break;
	case AST_CONTINUE:
	{
		//int stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT, (void *)(long long)ast->line_number);
		ir.type = IR_CONTINUE;
		out->emplace_back(ir);
		//IRCreateEndBlock(lang_stat, stmnt_idx, out, IR_END_STMNT);
	}break;
    default:
        ASSERT(0)
    }
}

void AstCreateCode(lang_state* lang_stat, node* n, scope* scp, own_std::vector<int> *ir)
{
	ast_rep *ast = AstFromNode(lang_stat, n, scp);
	//auto irs = (own_std::vector<ir_rep> *) ir;
	GetIRFromAst(lang_stat, ast, (own_std::vector<ir_rep> *) ir);
	//WasmIrInterp(lang_stat->dstate, ir);
}
void CompileDo(lang_state* lang_stat, node* n, scope* scp)
{
	ast_rep *ast = AstFromNode(lang_stat, n, scp);
	own_std::vector<ir_rep> irs;
	GetIRFromAst(lang_stat, ast, &irs);
	WasmIrInterp(lang_stat->dstate, (own_std::vector<int>  *)&irs);

}

ast_rep *CreateDbgEqualStmnt(lang_state *lang_stat)
{
	ast_rep *bin = NewAst();
	ast_rep *lhs = NewAst();
	ast_rep *rhs = NewAst();

	lhs->type = AST_IDENT;
	lhs->decl = FindIdentifier(own_std::string("__global_dummy"), lang_stat->root, &rhs->lhs_tp);

	bin->type = AST_BINOP;
	bin->op = T_EQUAL;
	bin->e_holder.expr.emplace_back(lhs);
	bin->e_holder.expr.emplace_back(rhs);
	return bin;

}
