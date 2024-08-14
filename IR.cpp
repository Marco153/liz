#include "IR.h"

bool IsAstSimple(lang_state *lang_stat, ast_rep *ast);
decl2* PointLogic(lang_state* lang_stat, node* n, scope* scp, type2* ret_tp);
bool NameFindingGetType(lang_state* lang_stat, node* n, scope* scp, type2& ret_type);
void GenStackThenIR(lang_state* lang_stat, ast_rep* ast, own_std::vector<ir_rep>* out, ir_val* dst_val);


#define STACK_PTR_REG 8
#define BASE_STACK_PTR_REG 9
#define RET_1_REG 10
#define RET_2_REG 11

#define MAKE_DST_IR_VAL(ir_tp, ptr) (((short)ir_tp) | (((int)ptr)<<16))
#define MAKE_DST_IR_VAL(ir_tp, ptr) (((short)ir_tp) | (((int)ptr)<<16))

ast_rep *NewAst()
{
	auto ret = (ast_rep*)__lang_globals.alloc(__lang_globals.data, sizeof(ast_rep));
    memset(ret, 0, sizeof(ast_rep));
    return ret;

}


ast_rep *AstFromNode(lang_state *lang_stat, node *n, scope *scp)
{
    ast_rep *ret = NewAst();
	ret->line_number = n->t->line;
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
		case  T_AMPERSAND:
		{
			ret->type = AST_ADDRESS_OF;
			ret->ast = AstFromNode(lang_stat, n->r, scp);
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
		if (cur)
		{
			while (cur->type == N_ELSE_IF || cur->type == N_ELSE)
			{
				ret->cond.elses.emplace_back(AstFromNode(lang_stat, cur, scp));
				cur = cur->r;
			}
		}
    }break;
	case node_type::N_BINOP:
    {
		//ret->t = n->l->t;
        if(n->t->type == T_COLON)
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

		ast_rep *lhs = AstFromNode(lang_stat, cur->l, scp);
		ast_rep *rhs = AstFromNode(lang_stat, cur->r, scp);

		ret->expr.emplace_back(lhs);
		ret->expr.emplace_back(rhs);

        for(int i = node_stack.size() - 2; i >= 0; i--)
        {
            cur = *(node_stack.begin() + i);
            rhs = AstFromNode(lang_stat, cur->r, scp);
            ret->expr.emplace_back(rhs);
        }


        switch(n->t->type)
        {
		case T_POINT:
		{
			auto new_ar = (own_std::vector<ast_point> *) AllocMiscData(lang_stat, sizeof(own_std::vector<ast_point>));
			memset(new_ar, 0, sizeof(*new_ar));
			ast_point aux;
			node *first_node = *(node_stack.begin() + last_idx);

			dummy_type = DescendNode(lang_stat, first_node->l, scp);
			aux.decl_strct = dummy_type.strct->this_decl;
			aux.exp = ret->expr[0];
			new_ar->emplace_back(aux);

			decl2* strct = aux.decl_strct;
			for(int i = 1; i < ret->expr.size(); i++)
			{
				rhs = *(ret->expr.begin() + i);
				aux.decl_strct = rhs->decl;
				aux.exp = rhs;
				decl2* is_struct = strct->type.strct->FindDecl(rhs->str);
				if (is_struct)
				{
					rhs->decl = is_struct;
					aux.decl_strct = is_struct;

					strct = aux.decl_strct;
				}

				new_ar->emplace_back(aux);
			}
			ret->points = *new_ar;

		}break;
		case T_OR:
		case T_AND:
		case T_COMMA:
		case T_MUL:
		case T_COND_NE:
		case T_PLUS_EQUAL:
		case T_COND_EQ:
		case T_LESSER_THAN:
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
	case node_type::N_IDENTIFIER:
    {
        ret->type = AST_IDENT;
        type2 ret_type;
		ret->decl = FindIdentifier(n->t->str, scp, &ret_type);
		if (!ret->decl)
		{
			ret->str = n->t->str.substr();
		}
    }break;
	case node_type::N_KEYWORD:
    {
        switch(n->kw)
        {
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
			if(n->r)
				ret->ast = AstFromNode(lang_stat, n->r, scp);
        }break;
		default:
			ASSERT(0);
        }
    }break;
	case node_type::N_SCOPE:
    {
		if (n->r && n->r->type != N_STMNT)
		{
			ret->type = AST_STATS;
			ret->line_number = n->r->t->line;
			ret->stats.emplace_back(AstFromNode(lang_stat, n->r, n->scp));
		}
		else
			ret = AstFromNode(lang_stat, n->r, n->scp);
    }break;
	case node_type::N_STRUCT_CONSTRUCTION:
	{
		auto last = lang_stat->cur_func;
        ret->type = AST_STRUCT_COSTRUCTION;
		ret->strct_constr.strct = FindIdentifier(n->l->t->str, scp, &dummy_type)->type.strct;

		int tp_size = ret->strct_constr.strct->size;
		lang_stat->cur_strct_constrct_size_per_statement += tp_size;

		ret->strct_constr.at_offset = lang_stat->cur_strct_constrct_size_per_statement;


		int cur_sz = lang_stat->cur_func->strct_constrct_size_per_statement;
		lang_stat->cur_func->strct_constrct_size_per_statement = max(cur_sz, lang_stat->cur_strct_constrct_size_per_statement);

		type_struct2* strct =  ret->strct_constr.strct;
		FOR_VEC(c, *n->exprs)
		{
			ast_struct_construct_info info;
			info.var = strct->FindDecl(c->n->l->t->str);
			info.exp = AstFromNode(lang_stat, c->n->r, scp);
			ret->strct_constr.commas.emplace_back(info);
		}
		lang_stat->cur_strct_constrct_size_per_statement -= tp_size;
	}break;
	case node_type::N_FUNC_DECL:
	{
        ret->type = AST_FUNC;
		ASSERT(n->fdecl);
        ret->func.fdecl = n->fdecl;

		auto last = lang_stat->cur_func;
		lang_stat->cur_func = n->fdecl;
        
        ret->func.fdecl = n->fdecl;
        ret->func.stats = AstFromNode(lang_stat, n->r, n->fdecl->scp);

		lang_stat->cur_func = last;

	}break;
	case node_type::N_CALL:
	{
		ret->type = AST_CALL;
		decl2* decl = FindIdentifier(n->l->t->str, scp, &dummy_type);
		ret->call.fdecl = decl->type.fdecl;
		ret->call.in_func = scp->fdecl;

		ret->call.indirect = false;
		if (decl->type.type == TYPE_FUNC_PTR)
			ret->call.indirect = true;

		func_decl* f = ret->call.fdecl;

		if (IS_FLAG_ON(f->flags, FUNC_DECL_INTERNAL))
		{
			if (f->name == "sizeof")
			{
				dummy_type = DescendNode(lang_stat, n->r, scp);
				ret->type = AST_INT;
				ret->num = GetTypeSize(&dummy_type);
				
			}
			else
				ASSERT(0);
		}
		else if(n->r)
		{

			if (n->r->type == N_BINOP && n->r->t->type == T_COMMA)
			{
				ast_rep* args = AstFromNode(lang_stat, n->r, scp);
				ret->call.args = args->expr;
			}
			else
			{
				ret->call.args.emplace_back(AstFromNode(lang_stat, n->r, scp));
			}
		}
	}break;
	case node_type::N_FLOAT:
	{
		ret->type = AST_FLOAT;
		ret->f32 = n->t->f;

	}break;
	case node_type::N_STMNT:
	{
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
		ret->stats.emplace_back(lhs);

        for(int i = node_stack.size() - 1; i >= 0; i--)
        {
			node* s = *(node_stack.begin() + i);
			ast_rep* rhs = AstFromNode(lang_stat, s->r, scp);

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
    case AST_IDENT:
    {
        val->type = IR_TYPE_DECL;
        val->decl = ast->decl;
		val->ptr = ast->decl->type.ptr;
		val->reg_sz = GetTypeSize(&ast->decl->type);
		val->is_unsigned = false;
		if(ast->decl->type.type != TYPE_STRUCT && ast->decl->type.ptr == 0)
			val->is_unsigned = IsUnsigned(ast->decl->type.type);
		if (ast->decl->type.ptr > 0)
			val->is_unsigned = true;
		val->is_float = ast->decl->type.type == TYPE_F32;
    }break;
    case AST_FLOAT:
    {
        val->type = IR_TYPE_F32;
        val->f32 = ast->f32;
		val->reg_sz = 4;
		val->is_unsigned = false;
		val->is_float = true;
    }break;
    case AST_INT:
    {
        val->type = IR_TYPE_INT;
        val->i = ast->num;
		val->is_unsigned = ast->num < 0 ? false : true;
		val->reg_sz = 4;
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

    ir_rep *begin = &(*out)[begin_sub_if_idx];

    out->emplace_back(ir);


	begin->block.other_idx = out->size();
	switch (type)
	{
	case IR_END_STMNT:
	{
		ASSERT(begin->type == IR_BEGIN_STMNT);
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
		out->back().block.stmnt.line = (int)(long long)data;
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
	case T_LESSER_THAN:
		return IR_CMP_LT;
	case T_COND_NE:
		return IR_CMP_NE;
	default:
		ASSERT(0)
	}
}
#define REG_FREE_FLAG 0x100
#define REG_SPILLED 0x200
void FreeReg(lang_state* lang_stat, char reg_idx)
{
	lang_stat->regs[reg_idx] &= ~REG_FREE_FLAG;
}
void FreeAllRegs(lang_state* lang_stat)
{
	memset(lang_stat->regs, 0, sizeof(lang_stat->regs));
	//lang_stat->regs[reg_idx] &= ~REG_FREE_FLAG;
}
void AllocSpecificReg(lang_state* lang_stat, char idx)
{
	ASSERT(IS_FLAG_OFF(lang_stat->regs[idx], REG_FREE_FLAG));
	lang_stat->regs[idx] |= REG_FREE_FLAG;
}
char AllocReg(lang_state* lang_stat)
{
	for (int i = 0; i < 7; i++)
	{
		if (IS_FLAG_OFF(lang_stat->regs[i], REG_FREE_FLAG))
		{
			lang_stat->regs[i] |= REG_FREE_FLAG;
			return i;
		}
	}
	ASSERT(0);
}
void GetIRBin(lang_state *lang_stat, ast_rep *ast_bin, own_std::vector<ir_rep> *out, ir_type type, void *data = nullptr)
{
	ir_rep ir;
	memset(&ir, 0, sizeof(ir_rep));
	ir.assign.to_assign.type = IR_TYPE_REG;
	ir.assign.to_assign.reg = AllocReg(lang_stat);


	ir.bin.lhs.type = IR_TYPE_REG;
	ir.bin.lhs.reg_sz = 4;
	ir.bin.lhs.reg = AllocReg(lang_stat);

	ir.bin.rhs.type = IR_TYPE_REG;
	ir.bin.rhs.reg_sz = 4;
	ir.bin.rhs.reg = AllocReg(lang_stat);
	GenStackThenIR(lang_stat, ast_bin->expr[0], out, &ir.bin.lhs);
	GenStackThenIR(lang_stat, ast_bin->expr[0], out, &ir.bin.rhs);

	ir.type = type;
	ir.bin.op = ast_bin->op;
	switch (type)
	{
	case IR_CMP_NE:
	case IR_CMP_EQ:
	case IR_CMP_GE:
	{
		ir.bin.it_is_jmp_if_true = (bool)data;
	}break;
	default:
		ASSERT(0);
	}
	out->emplace_back(ir);



		//ir.type = IR_CMP;


}

void GetIRFromAst(lang_state* lang_stat, ast_rep* ast, own_std::vector<ir_rep>* out);
void GetIRCond(lang_state* lang_stat, ast_rep* ast, own_std::vector<ir_rep>* out)
{
	if (ast->type == AST_BINOP && (ast->op != T_OR && ast->op != T_AND))
	{
		tkn_type2 opposite = OppositeCondCmp(ast->op);
		ast->op = opposite;
		GetIRBin(lang_stat, ast, out, FromTokenOpToIRType(opposite), (void*)false);
	}
	else
        GetIRFromAst(lang_stat, ast, out);
}

void GenIRSubBin(lang_state* lang_stat, ast_rep* ast, own_std::vector<ir_rep>* out, ir_val_type to, void* data)
{
	ast_rep* lhs_exp = ast->expr[0];
	ast_rep* rhs_exp = ast->expr[1];

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
void PushAstsInOrder(lang_state* lang_stat, ast_rep* ast, own_std::vector<ast_rep*>* out)
{
	switch (ast->type)
	{
	case AST_CALL:
	{

		FOR_VEC(arg, ast->call.args)
		{
			PushAstsInOrder(lang_stat, (*arg), out);
		}
		out->emplace_back(ast);
	}break;
	case AST_INT:
	case AST_FLOAT:
	case AST_IDENT:
	{
		out->emplace_back(ast);
	}break;
	case AST_CAST:
	{
		PushAstsInOrder(lang_stat, ast->cast.casted, out);
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
				out->emplace_back(ast);
				//lhs /
			}
		}
		else
		{
			PushArrayOfAsts(lang_stat, &ast->expr, ast, out);
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
bool IsIrValFloat(ir_val* val)
{
	return val->type == IR_TYPE_DECL && val->decl->type.type == TYPE_F32 || val->is_float || val->type == IR_TYPE_F32;
}

void GinIRFromStack(lang_state* lang_stat, own_std::vector<ast_rep *> &exps, own_std::vector<ir_rep> *out)
{
	own_std::vector<ir_val> stack;
	stack.reserve(4);
	ir_rep ir;
	for(int j = 0; j < exps.size(); j++)
	{
		ast_rep* e = exps[j];
		
		ir_val val;
		memset(&val, 0, sizeof(ir_val));
		memset(&ir, 0, sizeof(ir_rep));
		ir.type = IR_NONE;

		switch (e->type)
		{
		case AST_CALL:
		{

			int cur_biggest = e->call.in_func->biggest_call_args;
			e->call.in_func->biggest_call_args = max(cur_biggest, e->call.fdecl->args.size());


			if (e->call.args.size() > 0)
			{
				for (int i = e->call.args.size() - 1; i >= 0; i--)
				{
					ir_val* top = &stack[stack.size() - 1];
					stack.pop_back();
					ir = {};
					ir.type = IR_ASSIGNMENT;
					ir.assign.to_assign.type = IR_TYPE_ARG_REG;
					ir.assign.to_assign.reg = i;
					ir.assign.to_assign.reg_sz = 8;
					ir.assign.only_lhs = true;

					ir.assign.lhs = *top;
					out->emplace_back(ir);
				}
			}
			
			ir.type = IR_CALL;
			if(e->call.indirect)
				ir.type = IR_INDIRECT_CALL;

			ir.call.is_outsider = false;
			if (IS_FLAG_ON(e->call.fdecl->flags, FUNC_DECL_IS_OUTSIDER))
				ir.call.is_outsider = true;

			ir.call.fdecl = e->call.fdecl;
			out->emplace_back(ir);

			// if is not last, we will move to some reg
			if ((j + 1) < exps.size())
			{
				ir = {};
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg = AllocReg(lang_stat);
				ir.assign.to_assign.reg_sz = 8;
				ir.assign.only_lhs = true;

				ir.assign.lhs.type = IR_TYPE_RET_REG;
				ir.assign.lhs.reg = 0;
				ir.assign.lhs.reg_sz = 4;
				out->emplace_back(ir);
				val = ir.assign.to_assign;
			}
			else
			{
				val.type = IR_TYPE_RET_REG;
				val.reg_sz = 0;
				val.reg = 0;
			}

			stack.emplace_back(val);;
		}break;
		case AST_ADDRESS_OF:
		{
			ir_val* top = &stack[stack.size() - 1];
			//ASSERT(top->ptr > 0);
			//if (e->deref.times == 0) break;
			top->ptr = -1;

			ir.type = IR_ASSIGNMENT;

			ir.assign.to_assign.type = IR_TYPE_REG;
			ir.assign.to_assign.reg_sz = 4;
			ir.assign.to_assign.reg = AllocReg(lang_stat);
			ir.assign.only_lhs = true;
			ir.assign.lhs = *top;
			ir.assign.lhs.ptr = -1;
			//ir.assign.lhs.ptr = e->deref.times;
			out->emplace_back(ir);

		}break;
		case AST_STRUCT_COSTRUCTION:
		{
			ir_val* top = &stack.back();
			int offset = e->strct_constr.at_offset;
			//top->i = offset;

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
				ir.assign.to_assign.i = offset - cinfo->var->offset;
				ir.assign.to_assign.reg_sz = GetTypeSize(&cinfo->var->type);
				ir.assign.lhs = *top;
				out->emplace_back(ir);
			}
			val.type = IR_TYPE_ON_STACK;
			val.i = offset;
			stack.emplace_back(val);
		}break;
		case AST_DEREF:
		{
			ir_val* top = &stack[stack.size() - 1];
			//ASSERT(top->ptr > 0);
			if (e->deref.times == 0) break;

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
			ir.assign.lhs.ptr = e->deref.times;
			out->emplace_back(ir);

		}break;
		case AST_CAST:
		{
			ir_val* top = &stack[stack.size() - 1];
			top->ptr = top->ptr;
			top->reg_sz = GetTypeSize(&e->cast.type);
			//top->reg_sz = e->cast.type;
		}break;
		case AST_INT:
		case AST_FLOAT:
		case AST_IDENT:
		{
			GetIRVal(lang_stat, e, &val);
			val.ptr = 0;
			stack.emplace_back(val);

		}break;
		case AST_BINOP:
		{
			ASSERT(stack.size() >= 2);
			ir_val* top = &stack[stack.size() - 1];
			ir_val* one_minus_top = &stack[stack.size() - 2];
			//ir.assign.to_assign.reg = AllocReg(lang_stat);
			ir.type = IR_ASSIGNMENT;
			ir.assign.to_assign.type = IR_TYPE_REG;
			ir.assign.to_assign.reg_sz = 4;
			ir.assign.op = e->op;
			ir.assign.only_lhs = false;
			ir.assign.lhs = stack[stack.size() - 2];
			ir.assign.rhs = stack[stack.size() - 1];

			if (IsIrValFloat(top))
				ir.assign.to_assign.is_float = true;
			
			if (top->type == IR_TYPE_REG)
				FreeReg(lang_stat, top->reg);

			// reusing the reg
			if (one_minus_top->type == IR_TYPE_REG)
			{
				ir.assign.to_assign.type = IR_TYPE_REG;
				ir.assign.to_assign.reg = one_minus_top->reg;
			}
			else
				ir.assign.to_assign.reg = AllocReg(lang_stat);


			stack.pop_back();

			top = &stack[stack.size() - 1];
			top->type = IR_TYPE_REG;
			top->reg = ir.assign.to_assign.reg;

			
			out->emplace_back(ir);

		}break;
		default:
			ASSERT(0);
		}
	}
}

void GenStackThenIR(lang_state *lang_stat, ast_rep *ast, own_std::vector<ir_rep> *out, ir_val *dst_val)
{
	own_std::vector<ast_rep*> exps;

	switch (ast->type)
	{
	case AST_CALL:
	{
		FOR_VEC(arg, ast->call.args)
		{
			ast_rep* a = *arg;
			PushAstsInOrder(lang_stat, a, &exps);
		}
		exps.emplace_back(ast);
	}break;
	case AST_CAST:
	case AST_ADDRESS_OF:
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
	case AST_IDENT:
	{
		GetIRVal(lang_stat, ast, dst_val);
		return;
	}break;
	default:
		ASSERT(0);
	}

	ir_rep ir;
	int begin_complex_idx = out->size();
	GinIRFromStack(lang_stat, exps, out);

	// get last assignment and moving it to dst complex
	for (int i = out->size() - 1; i >= begin_complex_idx; i--)
	{
		ir_rep* cur = &(*out)[i];
		if (cur->type == IR_ASSIGNMENT)
		{
			if (cur->assign.to_assign.reg != dst_val->reg)
			{
				ir.type = IR_ASSIGNMENT;
				ir.assign.to_assign = *dst_val;
				ir.assign.to_assign.is_float = cur->assign.lhs.is_float;
				ir.assign.to_assign.reg = dst_val->reg;
				ir.assign.only_lhs = true;
				ir.assign.lhs = cur->assign.to_assign;
				dst_val->is_float = ir.assign.to_assign.is_float;
				out->emplace_back(ir);
			}
			break;
		}
	}
	//ir.type = IR_END_COMPLEX;
	//out->emplace_back(ir);

	/*
	if (ast->expr.size() == 2)
	{
		

	}
	else
	{

	}
	ir_rep ir;

	ASSERT(ast->type == AST_BINOP);
	ast_rep* lhs_exp = ast->expr[0];
	ast_rep* rhs_exp = ast->expr[1];

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

	for(int i = 2; i < ast->expr.size(); i++)
	{
		ast_rep* cur = ast->expr[i];

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
	ir.assign.to_assign.reg_sz = 4;
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
	case AST_CALL:
	{
		return GetTypeSize(&ast->call.fdecl->ret_type);
	}break;
	case AST_BINOP:
	{
		return GetAstTypeSize(lang_stat, ast->expr[0]);
	}break;
	case AST_CAST:
	{
		int prev = ast->cast.type.ptr;

		//ast->cast.type.ptr = max(0, prev - 1);

		int sz = GetTypeSize(&ast->cast.type);
		ast->cast.type.ptr = prev;
		return  sz;
	}break;
	case AST_DEREF:
	{
		return GetAstTypeSize(lang_stat, ast->deref.exp);
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

void FreeArgsRegs(lang_state* lang_stat, func_decl* func, own_std::vector<ir_rep>* out)
{
	for (int i = 0; i < func->args.size(); i++)
	{
		if (IS_FLAG_ON(lang_stat->arg_regs[i], REG_SPILLED))
		{
			lang_stat->arg_regs[i] |= REG_FREE_FLAG;
			ir_rep ir;
			ir.type = IR_UNSPILL_REG;
			ir.spill.dst.type = IR_TYPE_ARG_REG;
			ir.spill.dst.reg = i;
			ir.spill.offset = lang_stat->cur_spilled_offset - i * 8;
			out->emplace_back(ir);
		}
	}

}
int AllocArgsRegsReturnSpilled(lang_state* lang_stat, func_decl* func, own_std::vector<ir_rep>* out)
{
	ASSERT(func->args.size() < 32);
	int spilled = 0;
	int final_offset = lang_stat->cur_spilled_offset;
	for (int i = 0; i < func->args.size(); i++)
	{
		if (IS_FLAG_ON(lang_stat->arg_regs[i], REG_FREE_FLAG))
		{
			final_offset += 8;
			lang_stat->arg_regs[i] |= REG_FREE_FLAG;
		}
	}
	for (int i = 0; i < func->args.size(); i++)
	{
		if (IS_FLAG_OFF(lang_stat->arg_regs[i], REG_FREE_FLAG))
		{
			lang_stat->arg_regs[i] |= REG_FREE_FLAG;
		}
		else
		{
			ir_rep ir;
			ir.type = IR_SPILL_REG;
			ir.spill.dst.type = IR_TYPE_ARG_REG;
			ir.spill.dst.reg = i;
			ir.spill.offset = final_offset - i * 8;
			out->emplace_back(ir);

			lang_stat->arg_regs[i] |= REG_SPILLED;

			spilled++;
		}
	}
	return spilled;
}
void GetIRFromAst(lang_state *lang_stat, ast_rep *ast, own_std::vector<ir_rep> *out)
{
	ir_rep ir = {};
    switch(ast->type)
    {
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
            ir.fdecl = ast->func.fdecl;
			if(IS_FLAG_ON(a->flags, DECL_IS_ARG))
				ir.type = IR_DECLARE_ARG;
			else 
				ir.type = IR_DECLARE_LOCAL;
			ir.decl = a;
			out->emplace_back(ir);
		}
        ir.type = IR_STACK_BEGIN;
		ir.fdecl = ast->func.fdecl;
        //ir.num  = ast->func.fdecl->stack_size;
        out->emplace_back(ir);

		GetIRFromAst(lang_stat, ast->func.stats, out);

        ir.type = IR_STACK_END;
        //ir.num  = ast->func.fdecl->stack_size;
        out->emplace_back(ir);
		lang_stat->cur_func = last_func;
	}break;
    case AST_CALL:
	{
		GenStackThenIR(lang_stat, ast, out, &ir.assign.lhs);
		return;
		short arg_regs[MAX_ARGS];
		memcpy(arg_regs, lang_stat->arg_regs, sizeof(lang_stat->arg_regs));

		int spilled = AllocArgsRegsReturnSpilled(lang_stat, ast->call.fdecl, out) * 8;
		int cur_max_spilled = lang_stat->cur_func->max_spilled_bytes;

		lang_stat->cur_spilled_offset += spilled;

		lang_stat->cur_func->max_spilled_bytes = max(cur_max_spilled, cur_max_spilled + spilled);
		lang_stat->cur_func->spilled_offset = lang_stat->cur_spilled_offset;



		ir.type = IR_BEGIN_CALL;
		ir.call.fdecl = ast->call.fdecl;
		out->emplace_back(ir);
		int cur_biggest = ast->call.in_func->biggest_call_args;
		ast->call.in_func->biggest_call_args = max(cur_biggest, ast->call.fdecl->args.size());

		int cur_arg = 0;
		FOR_VEC(arg, ast->call.args)
		{
			ast_rep* a = *arg;
			ir = {};
			ir.type = IR_ASSIGNMENT;
			ir.assign.to_assign.type = IR_TYPE_ARG_REG;
			ir.assign.to_assign.reg = cur_arg;
			ir.assign.to_assign.reg_sz = 8;
			ir.assign.only_lhs = true;

			if (a->type == AST_CAST)
				a = a->cast.casted;
			ir.assign.lhs.type = IR_TYPE_REG;
			ir.assign.lhs.reg = 0;
			ir.assign.lhs.reg_sz = 4;

			GenStackThenIR(lang_stat, a, out, &ir.assign.lhs);
			/*
			if (IsAstSimple(lang_stat, a))
			{
				GetIRVal(lang_stat, a, &ir.assign.lhs);
			}
			else if (a->type == AST_ADDRESS_OF)
			{
				ir.assign.op = T_AMPERSAND;
				if (IsAstSimple(lang_stat, a->ast))
				{
					GetIRVal(lang_stat, a->ast, &ir.assign.lhs);
				}
				else if (a->ast->type == AST_STRUCT_COSTRUCTION)
				{
					DoStructConstruction(lang_stat, a->ast, IR_TYPE_ARG_REG, (void *)(long long)cur_arg, out);
				}
				else
					ASSERT(0);
			}
			else
				ASSERT(0);
				*/
			out->emplace_back(ir);
			cur_arg++;
		}
		ir.type = IR_END_CALL;
		ir.call.fdecl = ast->call.fdecl;
		out->emplace_back(ir);

		FreeArgsRegs(lang_stat, ast->call.fdecl, out);

		lang_stat->cur_spilled_offset -= spilled;

		memcpy(lang_stat->arg_regs, arg_regs, sizeof(lang_stat->arg_regs));
	}break;
    case AST_RET:
	{
		ir.type = IR_ASSIGNMENT;
		ast_rep *rhs_ast = ast->ast;
		if (!rhs_ast)
			return;
		ir.assign.to_assign.type = IR_TYPE_RET_REG;
		ir.assign.to_assign.reg = RET_1_REG;
		ir.assign.to_assign.reg_sz = 4;

		switch (rhs_ast->type)
		{
		case AST_CALL:
		{
			GenStackThenIR(lang_stat, rhs_ast, out, &ir.assign.to_assign);
		}break;
		case AST_BINOP:
		{
			ir.assign.only_lhs = true;
			ir.assign.lhs.type = IR_TYPE_REG;
			ir.assign.lhs.reg = AllocReg(lang_stat);
			ir.assign.lhs.reg_sz = 4;

			GenStackThenIR(lang_stat, rhs_ast, out, &ir.assign.lhs);
			out->emplace_back(ir);
		}break;
		case AST_IDENT:
		case AST_INT:
		{
			ir.assign.only_lhs = true;
			GetIRVal(lang_stat, rhs_ast, &ir.assign.lhs);
			out->emplace_back(ir);
		}break;
		{
			ir.assign.only_lhs = true;
			GetIRVal(lang_stat, rhs_ast, &ir.assign.lhs);
			out->emplace_back(ir);
		}break;
		default:
			ASSERT(0)
		}
	}break;
    case AST_STATS:
	{
		FOR_VEC(st, ast->stats)
		{
			FreeAllRegs(lang_stat);
			ast_rep* s = *st;
			if (s->type == AST_EMPTY)
				continue;

			ASSERT(!lang_stat->ir_in_stmnt)


			if (s->type == AST_WHILE || s->type == AST_IF)
			{
				GetIRFromAst(lang_stat, s, out);
			}
			else
			{
				lang_stat->ir_in_stmnt = true;

				bool can_emplace_stmnt = s->type != AST_IDENT;
				int stmnt_idx = 0;
				if(can_emplace_stmnt)
					stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT, (void *)(long long)s->line_number);

				GetIRFromAst(lang_stat, s, out);

				if(can_emplace_stmnt)
					IRCreateEndBlock(lang_stat, stmnt_idx, out, IR_END_STMNT);

				lang_stat->ir_in_stmnt = false;
			}



			int i = 0;
		}
	}break;
    case AST_BINOP:
    {
        switch(ast->op)
        {
        case T_PLUS_EQUAL:
        case T_EQUAL:
        {
			FreeAllRegs(lang_stat);
            ir_val lhs;
            ir_val rhs;

            ast_rep *rhs_ast = ast->expr[1];
            ast_rep *lhs_ast = ast->expr[0];
            
			ir.type = IR_ASSIGNMENT;

			ir.assign.to_assign.reg = 0;
			ir.assign.to_assign.ptr = 1;
			ir.assign.to_assign.type = IR_TYPE_REG;
			ir.assign.to_assign.reg_sz = GetAstTypeSize(lang_stat, ast->expr[0]);
			if (lhs_ast->type == AST_DEREF)
				lhs_ast->deref.times--;
				//ir.assign.to_assign.ptr = lhs_ast->deref.times - 1;


			GenStackThenIR(lang_stat, ast->expr[0], out, &ir.assign.to_assign);
			//GetIRVal(lang_stat, ast->expr[0], &ir.assign.to_assign);

			switch (rhs_ast->type)
			{
			case AST_CALL:
			case AST_BINOP:
			case AST_CAST:
			case AST_DEREF: 
			{
				ir.assign.lhs.type = IR_TYPE_REG;
				ir.assign.lhs.reg = AllocReg(lang_stat);
				ir.assign.lhs.reg_sz = GetAstTypeSize(lang_stat, rhs_ast);
				ir.assign.only_lhs = true;
				GenStackThenIR(lang_stat, rhs_ast, out, &ir.assign.lhs);
			}break;
			case AST_INT: 
			case AST_FLOAT: 
			case AST_IDENT:
			{
				ir.assign.only_lhs = true;
				GetIRVal(lang_stat, rhs_ast, &ir.assign.lhs);
			}break;
			default:
				ASSERT(0)
			}
			if (ast->op == T_PLUS_EQUAL)
			{
				ir.assign.only_lhs = false;
				ir.assign.op = T_PLUS;
				ir.assign.rhs = ir.assign.to_assign;
				ir.assign.rhs.is_unsigned = ir.assign.lhs.is_unsigned;

				if (IsIrValFloat(&ir.assign.lhs))
					ir.assign.to_assign.is_float = true;
				//ir.assign.rhs.ptr++////;
			}
			ASSERT(ir.assign.lhs.reg_sz <= 8);
			ASSERT(ir.assign.rhs.reg_sz <= 8);
			ASSERT(ir.assign.to_assign.reg_sz <= 8);
			out->emplace_back(ir);
			FreeAllRegs(lang_stat);
        }break;
		case T_AND:
		{
            FOR_VEC(expr, ast->expr)
            {
                ast_rep *e = *expr;
                if(e->type == AST_BINOP && e->op == T_OR)
                {
                    int block_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_AND_BLOCK);

                    GetIRFromAst(lang_stat, e, out);

                    IRCreateBeginBlock(lang_stat, out, IR_END_AND_BLOCK);

                }
				else if (e->type == AST_BINOP)
				{
					ASSERT(e->op == T_COND_NE || e->op == T_COND_EQ);
					
					
					tkn_type2 opposite = OppositeCondCmp(e->op);
					e->op = opposite;
					// false is for the it_is_jmp_if_true var in ir_rep
					GetIRBin(lang_stat, e, out, FromTokenOpToIRType(opposite), (void *)false);

				}
                else
                {
                    GetIRFromAst(lang_stat, e, out);
                }
            }
		}break;
        case T_OR:
        {
			int idx = 0;
            FOR_VEC(expr, ast->expr)
            {
                ast_rep *e = *expr;
                if(e->type == AST_BINOP && e->op == T_AND)
                {
                    int block_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_OR_BLOCK);

                    GetIRFromAst(lang_stat, e, out);

                    IRCreateBeginBlock(lang_stat, out, IR_END_OR_BLOCK);

                }
				else if (e->type == AST_BINOP)
				{
					ASSERT(e->op == T_COND_NE || e->op == T_COND_EQ);
					
					
					GetIRBin(lang_stat, e, out, FromTokenOpToIRType(e->op), (void *)true);

				}
                else
                {
                    GetIRFromAst(lang_stat, e, out);
                }
				idx++;
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
		int stmnt_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_STMNT, (void *)(long long)ast->line_number);
		int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);
		int loop_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_LOOP_BLOCK);
		GetIRCond(lang_stat, ast->loop.cond, out);
		IRCreateEndBlock(lang_stat, stmnt_idx, out, IR_END_STMNT);

		if (ast->loop.scope)
		{
			GetIRFromAst(lang_stat, ast->loop.scope, out);
		}

		IRCreateEndBlock(lang_stat, loop_idx, out, IR_END_LOOP_BLOCK);
		IRCreateEndBlock(lang_stat, cond_idx, out, IR_END_IF_BLOCK);
    }break;
	case AST_BREAK:
	{
		ir.type = IR_BREAK;
		out->emplace_back(ir);
	}break;
	case AST_IF:
    {
        int if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_IF_BLOCK);

        bool has_elses = ast->cond.elses.size();

        int sub_if_idx = 0;
        if(has_elses)
            sub_if_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_SUB_IF_BLOCK);

		int cond_idx = IRCreateBeginBlock(lang_stat, out, IR_BEGIN_COND_BLOCK);
		GetIRCond(lang_stat, ast->cond.cond, out);
        //GetIRFromAst(lang_stat, ast->cond.cond, out);
		IRCreateEndBlock(lang_stat,cond_idx, out, IR_END_COND_BLOCK);

        if(ast->cond.scope)
        {
            GetIRFromAst(lang_stat, ast->cond.scope, out);
            if(has_elses)
            {
                ir.type = IR_BREAK_IF;
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
				GetIRCond(lang_stat, e->cond.cond, out);

            GetIRFromAst(lang_stat, e->cond.scope, out);

            if(!is_last)
            {
                ir.type = IR_BREAK_IF;
                out->emplace_back(ir);
                IRCreateEndBlock(lang_stat, sub_if_idx, out, IR_END_SUB_IF_BLOCK);
            }
        }

        IRCreateEndBlock(lang_stat, if_idx, out, IR_END_IF_BLOCK);

    }break;
    default:
        ASSERT(0)
    }
}

