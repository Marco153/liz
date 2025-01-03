#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <string>

#define LANG_NO_ENGINE
#include "../compile.cpp"


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
void Log(std::string message)
{
	HANDLE hFile;
	char DataBuffer[] = "message from lsp";
	DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
	DWORD dwBytesWritten = 0;
	BOOL bErrorFlag = FALSE;

	// Create a new file or open an existing file for writing
	hFile = CreateFile("example.txt",                // name of the file
		    FILE_APPEND_DATA,                // open for writing
		    0,                            // do not share
		    NULL,                         // default security
		    OPEN_EXISTING,                // create new file or overwrite existing
		    FILE_ATTRIBUTE_NORMAL,        // normal file
		    NULL);                        // no attribute template

	if (hFile == INVALID_HANDLE_VALUE) {
		//DisplayError(TEXT("CreateFile"));
		//printf("Terminal failure: Unable to open file for write.\n");
		return;
	}
	Write(hFile, (char *)message.c_str(), message.size());
	CloseHandle(hFile);
}

void InsertHightlightWord(token2 *t, own_std::vector<char> *out_buffer, u32 color)
{
	lsp_syntax_hightlight_word w;
	w.hdr.type = lsp_syntax_hightlight_enum::WORD;
	w.line = t->line;
	w.column_start = t->line_offset;
	w.column_end = t->line_offset + t->str.size();
	w.color = color;
	out_buffer->insert(out_buffer->end(), (char*)&w, (char*)(&w + 1));
}

void CreateSyntaxHighlightingNode(lang_state *lang_stat, node *n, scope *scp, own_std::vector<char> *out_buffer)
{
	if (!n || n->modified || n->type== N_EMPTY)
		return;
	type2 dummy_tp;
	int func_names = 0xcccc44ff;
	int struct_color = 0xff00ff00;
	switch(n->type)
	{
	case N_CALL:
	{
		if (n->l->type == N_IDENTIFIER)
		{
			InsertHightlightWord(n->l->t, out_buffer, func_names);
		}
		CreateSyntaxHighlightingNode(lang_stat, n->r, scp, out_buffer);
	}break;
	case N_IDENTIFIER:
	{
		decl2 *d = FindIdentifier(n->t->str, scp, &dummy_tp);
		if (!d)
			return;
		if(d->type.type == TYPE_STRUCT_TYPE)
		{
			InsertHightlightWord(n->t, out_buffer, struct_color);
		}
		else
			InsertHightlightWord(n->t, out_buffer, 0xffccccff);

	}break;
	case N_SCOPE:
	{
		scope *new_scp = n->scp;
		CreateSyntaxHighlightingNode(lang_stat, n->r, new_scp, out_buffer);
	}break;
	case N_UNOP:
	{
		CreateSyntaxHighlightingNode(lang_stat, n->r, scp, out_buffer);
	}break;
	case N_BINOP:
	{
		switch(n->t->type)
		{
		case T_COLON:
		{
			if (!n->r)
				return;

			switch (n->r->type)
			{
			case N_STRUCT_DECL:
			case N_FUNC_DECL:
			case N_BINOP:
			{
				CreateSyntaxHighlightingNode(lang_stat, n->r, scp, out_buffer);
				if (n->r->r->type == N_FUNC_DECL)
				{
					InsertHightlightWord(n->l->t, out_buffer, func_names);
				}
			}break;
			case N_IDENTIFIER:
			{
				decl2* d = FindIdentifier(n->r->t->str, scp, &dummy_tp);

				if (d && d->type.type == TYPE_STRUCT_TYPE && !n->r->modified)
				{
					InsertHightlightWord(n->r->t, out_buffer, struct_color);
				}

			}break;
			case N_UNOP:
			{
				CreateSyntaxHighlightingNode(lang_stat, n->r, scp, out_buffer);
			}break;
			}
		}break;
		default:
		{
			CreateSyntaxHighlightingNode(lang_stat, n->l, scp, out_buffer);
			CreateSyntaxHighlightingNode(lang_stat, n->r, scp, out_buffer);
		}break;
		}
	}break;
	case N_STRUCT_DECL:
		CreateSyntaxHighlightingNode(lang_stat, n->r, scp, out_buffer);
		break;
	case N_FUNC_DECL:
	{
		CreateSyntaxHighlightingNode(lang_stat, n->r, scp, out_buffer);
		CreateSyntaxHighlightingNode(lang_stat, n->l->l->r, scp, out_buffer);
	}break;
	case N_STMNT:
	{
		own_std::vector<node*> node_stack;
		node* cur_node = n;
		while (cur_node->l->type == node_type::N_STMNT)
		{
			node_stack.emplace_back(cur_node);
			cur_node = cur_node->l;
		}

		int size = node_stack.size();
		if (cur_node->l != nullptr && IS_FLAG_OFF(cur_node->l->flags, NODE_FLAGS_IS_PROCESSED))
		{
			CreateSyntaxHighlightingNode(lang_stat, cur_node->l, scp, out_buffer);
		}
		while (true)
		{
			if (cur_node->r != nullptr && IS_FLAG_OFF(cur_node->r->flags, NODE_FLAGS_IS_PROCESSED))
			{
				CreateSyntaxHighlightingNode(lang_stat, cur_node->r, scp, out_buffer);
			}

			size--;

			if (size < 0)
				break;
			cur_node = *(node_stack.end() - 1);
			node_stack.pop_back();

		}
	}break;
	case N_FOR:
	{
		CreateSyntaxHighlightingNode(lang_stat, n->r, scp, out_buffer);
	}break;
	case N_ELSE_IF:
	case N_IF:
	{
		CreateSyntaxHighlightingNode(lang_stat, n->l->l, scp, out_buffer);
		CreateSyntaxHighlightingNode(lang_stat, n->l->r, scp, out_buffer);
		CreateSyntaxHighlightingNode(lang_stat, n->r, scp, out_buffer);
	}break;
	default:

		CreateSyntaxHighlightingNode(lang_stat, n->l, scp, out_buffer);
		CreateSyntaxHighlightingNode(lang_stat, n->r, scp, out_buffer);
	}
}
void CreateSyntaxHighlightingWholeFile(lang_state *lang_stat, unit_file *f, own_std::vector<char> *out_buffer)
{
	CreateSyntaxHighlightingNode(lang_stat, f->s, f->global, out_buffer);
	lsp_syntax_hightlight_hdr eof;
	eof.type = lsp_syntax_hightlight_enum::SEOF;
	out_buffer->insert(out_buffer->end(), (char*)&eof, (char*)(&eof + 1));

	lsp_header hdr;
	hdr.magic = 0x77;
	hdr.msg_len = sizeof(hdr) + out_buffer->size();
	hdr.msg_type = lsp_msg_enum::LSP_SYNTAX_RES;
	out_buffer->insert(out_buffer->begin(), (char*)&hdr, (char*)(&hdr + 1));
}

node *GetPointNodeUpToChar(lang_state *lang_stat, lsp_pos *intl, char *str_line)
{
	own_std::vector<token2> tkns;
	Tokenize2(str_line, intl->column, &tkns);
	if (tkns.size() <= 1)
		return nullptr;

	int val = setjmp(lang_stat->jump_buffer);
	if (val == 0)
	{
		lang_stat->flags |= PSR_FLAGS_ON_JMP_WHEN_ERROR;
		node_iter niter(&tkns, lang_stat);
		token2 eof;
		eof.type = T_EOF;
		tkns.insert(0, eof);
		niter.cur_idx = niter.tkns->size() - 2;
		niter.rev = true;
		// getting point expression backwards
		node* n = niter.parse_(PREC_POINT, parser_cond::NEQUAL);
		n->FreeTree();

		niter.cur_idx++;
		niter.rev = false;
		// we are now at the first point lhs
		n = niter.parse_(PREC_POINT, parser_cond::NEQUAL);
		return n;
	}
	return nullptr;

}

void SendScopeVars(scope *scp, HANDLE hStdout, bool recursive)
{
	own_std::vector<char> buffer;
	own_std::vector<char> str_tbl;
	int total_decls = 0;
	scope* cur_scp = scp;
	do
	{
		FOR_VEC(d_ptr, cur_scp->vars)
		{
			decl2* d = *d_ptr;
			rel_type2 rel_tp;
			rel_tp.type = d->type.type;
			rel_tp.name = str_tbl.size();
			rel_tp.name_len = d->name.size();
			buffer.insert(buffer.end(), (char*)&rel_tp, (char*)(&rel_tp + 1));
			InsertIntoCharVector(&str_tbl, (char*)d->name.c_str(), d->name.size() + 1);
			//rel_tp.name = d->type.type;

		}
		total_decls += cur_scp->vars.size();
		cur_scp = cur_scp->parent;
	} while (recursive && cur_scp);
	buffer.insert(buffer.begin(), (char*)&total_decls, (char*)(&total_decls + 1));
	buffer.insert(buffer.end(), str_tbl.begin(), str_tbl.end());

	lsp_header hdr;
	hdr.magic = 0x77;
	hdr.msg_type = lsp_msg_enum::LSP_INTELLISENSE_RES;
	hdr.msg_len = sizeof(lsp_header) + buffer.size();

	buffer.insert(buffer.begin(), (char*)&hdr, (char*)(&hdr + 1));
	
	Write(hStdout, (char*)buffer.data(), buffer.size());

}

scope *GetScopeWithLspLineStr(lang_state *lang_stat, to_lsp_linestr *line_info)
{
	char* str_tbl = (char*)(line_info + 1);

	char* file_path = str_tbl + line_info->line_str_len;
	std::string file = file_path;
	unit_file *ufile = ThereIsFile(lang_stat, file);
	if (!ufile)
		return nullptr;

	int line = line_info->pos.line;
	func_decl* found = GetFuncWithLine2(lang_stat, line, ufile);
	if (!found)
		return nullptr;

	return FindScpWithLine(found, line);

}
int main()
{

	struct defer_n
	{
		lang_state* l;
		u64 cur_node_before;
		defer_n(lang_state *lang)
		{
			l = lang;
			cur_node_before = l->cur_nd;
		}
		~defer_n()
		{
			l->cur_nd = cur_node_before;
		}
	};
	HANDLE hFile;
	char DataBuffer[] = "message from lsp";
	DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
	DWORD dwBytesWritten = 0;
	BOOL bErrorFlag = FALSE;



	bool first = true;
	printf("Writing %d bytes to example.txt.\n", dwBytesToWrite);
	 // Obtain the standard input handle with overlapped flag
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening standard input with overlapped flag." << std::endl;
        return 1;
    }
	Write(hStdin, "aoeoae", 5);
	// Set up the input buffer
	const DWORD bufferSize = 4024;
	DWORD bytesRead = 1;
	char* str_cont = "Content-Length: ";
	int len = strlen(str_cont);

	std::string inputBuffer;
	mem_alloc alloc;
	lang_state lang_stat;
	alloc.main_buffer = nullptr;

	InitMemAlloc(&alloc);
	InitLang(&lang_stat, (AllocTypeFunc)heap_alloc, (FreeTypeFunc)heap_free, &alloc);
	while (true) {
		std::string line;
		// Obtain the standard input handle



		if (first)
		{
			//Sleep(5000);
			first = false;
		}
		char tempBuffer[1024];
		DWORD bytesRead = 0;
		DWORD availableBytes = 0;

		if (PeekNamedPipe(hStdin, NULL, 0, NULL, &availableBytes, NULL) && availableBytes > 0) {
			DWORD bytesRead = 0;
			BOOL readResult = ReadFile(hStdin, tempBuffer, sizeof(tempBuffer) - 1, &bytesRead, NULL);
			auto hdr = (lsp_header*)tempBuffer;
			if(hdr->magic == 0x77)
			{
				char* cur_ptr = (char*)(hdr + 1);
				// compile folder
				if(hdr->msg_type == lsp_msg_enum::ADD_FOLDER)
				{
					InitMemAlloc(&alloc);
					InitLang(&lang_stat, (AllocTypeFunc)heap_alloc, (FreeTypeFunc)heap_free, &alloc);
					int str_sz = *(int*)(cur_ptr);
					cur_ptr += 4;
					std::string folder(cur_ptr, str_sz);
					cur_ptr += str_sz;

					str_sz = *(int*)(cur_ptr);
					cur_ptr += 4;
					std::string exe_dir(cur_ptr, str_sz);
					lang_stat.exe_dir = exe_dir;
					cur_ptr += str_sz;
					lang_stat.lsp_stage = LSP_STAGE_NAME_FINDING;
					lang_stat.is_lsp = true;
					LspAddFolder(&lang_stat, folder);
					//LspCompile(&lang_stat, folder, 0, 0, nullptr);
					continue;
				}
				else if(hdr->msg_type == lsp_msg_enum::LSP_DECL_DEF_LINE)
				{
					auto line_info = (to_lsp_linestr*)hdr;
					scope* scp = GetScopeWithLspLineStr(&lang_stat, line_info);
					if (!scp)
						continue;

					type2 dummy;
					auto str = (char*)(line_info + 1);
					decl2 *d = FindIdentifier(str, scp, &dummy);
					if(d)
					{
						char* line = d->from_file->lines[d->decl_nd->t->line - 1];
						int line_ln = strlen(line) + 1;

						own_std::vector<char> buffer;

						lsp_header hdr;
						hdr.magic = 0x77;
						hdr.msg_type = lsp_msg_enum::LSP_DECL_DEF_LINE_RES;
						hdr.msg_len = sizeof(lsp_header) + line_ln;

						buffer.insert(buffer.end(), (char*)&hdr, (char*)(&hdr + 1));
						InsertIntoCharVector(&buffer, (char*)line, line_ln);
						Write(hStdout, (char*)buffer.data(), buffer.size());
					}

				}
				else if(hdr->msg_type == lsp_msg_enum::LSP_GOTO_FUNC_DEF)
				{
					auto pos = (lsp_pos*)(hdr + 1);
					auto dir = (char *)(pos + 1);
					auto file_name = (char *)(dir + 1);
					unit_file *fl = ThereIsFile(&lang_stat, std::string(file_name));
					if (!fl)
						continue;

					u32 func_idx = 0;

					if (fl->funcs_scp->vars.size() == 0)
						continue;

					FOR_VEC(f_ptr, fl->funcs_scp->vars)
					{
						if ((*f_ptr)->type.type != TYPE_FUNC)
						{
							func_idx++;
							continue;
						}
						

						if (pos->line <= (*f_ptr)->type.fdecl->scp->line_start)
							break;
						func_idx++;
					}
					do
					{
						func_idx = (func_idx + *dir) % fl->funcs_scp->vars.size();
					} while (fl->funcs_scp->vars[func_idx]->type.type != TYPE_FUNC);
					lsp_header hdr;
					hdr.magic = 0x77;
					hdr.msg_type = lsp_msg_enum::LSP_GOTO_FUNC_RES;
					hdr.msg_len = sizeof(hdr) + 4;

					decl2* d = fl->funcs_scp->vars[func_idx];

					int line_start = d->type.fdecl->scp->line_start;
					own_std::vector<char> buffer;
					buffer.insert(buffer.end(), (char*)(&hdr), (char*)(&hdr + 1));
					buffer.insert(buffer.end(), (char*)(&line_start), (char*)(&line_start + 1));
					Write(hStdout, (char*)buffer.data(), buffer.size());


				}
				else if(hdr->msg_type == lsp_msg_enum::LSP_GOTO_DEF)
				{
					auto line_info = (to_lsp_linestr*)hdr;

					char* str_tbl = (char*)(line_info + 1);
					node *n = GetPointNodeUpToChar(&lang_stat, &line_info->pos, str_tbl);
					if (!n)
						continue;
					defer_n defered(&lang_stat);

					scope* scp = GetScopeWithLspLineStr(&lang_stat, line_info);
					if (!scp)
						continue;

					decl2* d = nullptr;
					type2 tp;

					if (n->type == N_IDENTIFIER)
					{
						d = FindIdentifier(n->t->str, scp, &tp);
					}
					else if (n->type == N_BINOP)
					{
						d = PointLogic(&lang_stat, n, scp, &tp);
					}
					else
						continue;
					if (!d)
						continue;

					std::string file_name = d->from_file->path + d->from_file->name;

					lsp_header hdr;
					hdr.magic = 0x77;
					hdr.msg_type = lsp_msg_enum::LSP_GOTO_DEF_RES;
					hdr.msg_len = sizeof(lsp_header) + sizeof(lsp_pos);
					goto_def intl_val;
					intl_val.line.line = d->decl_nd->t->line;
					intl_val.line.column = d->decl_nd->t->line_offset;

					if(d->type.type == TYPE_FUNC || d->type.type == TYPE_STRUCT_TYPE)
					{
						
						intl_val.line.column = d->decl_nd->l->t->line_offset;
					}

					intl_val.file_name_len = file_name.size();


					own_std::vector<char> buffer;
					buffer.insert(buffer.end(), (char*)&hdr, (char*)(&hdr + 1));
					buffer.insert(buffer.end(), (char*)&intl_val, (char*)(&intl_val + 1));
					InsertIntoCharVector(&buffer, (char *)file_name.c_str(), file_name.size() + 1);
					Write(hStdout, (char*)buffer.data(), buffer.size());

				}
				if (hdr->msg_type == lsp_msg_enum::LSP_DECL_DEF_LINE)
				{
					auto pos = (lsp_pos*)(hdr + 1);
					auto file_name = (char*)(pos + 1);

				}
				if (hdr->msg_type == lsp_msg_enum::LSP_SYNTAX)
				{
					std::string file_name = (char*)(hdr + 1);
					own_std::vector<char> final_buffer;
					int last_bar = file_name.find_last_of("\\/");
					std::string name = file_name.substr(last_bar + 1);
					std::string path = file_name.substr(0, last_bar+1);
					unit_file *file = AddNewFile(&lang_stat, name, path);
					CreateSyntaxHighlightingWholeFile(&lang_stat, file, &final_buffer);
					Write(hStdout, final_buffer.data(), final_buffer.size());
				}
				if(hdr->msg_type == lsp_msg_enum::INTELLISENSE)
				{

					auto line_info = (to_lsp_linestr*)hdr;
					scope* scp = GetScopeWithLspLineStr(&lang_stat, line_info);
					if (!scp)
						continue;

					auto str_line = (char*)(line_info + 1);

					char ch = str_line[line_info->pos.column];
					if(ch == '.')
					{
						auto pos = &line_info->pos;
						node *n = GetPointNodeUpToChar(&lang_stat, pos, str_line);
						if (!n)
							continue;

						defer_n defered(&lang_stat);

						type2 tp;
						decl2* last_decl = nullptr;

						if (n->type == N_BINOP && n->t->type == T_POINT)
						{
							last_decl = PointLogic(&lang_stat, n, scp, &tp);
						}
						else if(n->type == N_IDENTIFIER)
						{
							last_decl = FindIdentifier(n->t->str, scp, &tp);
						}
						else
						{
							continue;
						}
						tp = last_decl->type;

						//std::vector<char> type_tbl;
						if (tp.type == TYPE_STRUCT)
						{
							SendScopeVars(tp.strct->scp, hStdout, false);
						}
						if(tp.type == TYPE_ENUM_TYPE)
						{
							SendScopeVars(tp.scp, hStdout, false);
						}
					}
					else
					{
						SendScopeVars(scp, hStdout, true);
					}


				}
				else
				{
					lsp_header hdr;
					hdr.magic = 0x77;
					hdr.msg_type = lsp_msg_enum::LSP_ERROR;
					
					Write(hStdout, (char*)&hdr, sizeof(lsp_header));
				}

			}

			inputBuffer.clear();

		}
		if(lang_stat.lsp_stage == LSP_STAGE_PAUSE || lang_stat.lsp_stage == LSP_STAGE_DONE)
			Sleep(100);
		LspCompile(&lang_stat, "", 0, 0, nullptr);

		if (lang_stat.lsp_stage == LSP_STAGE_DONE)
		{
			lang_stat.lsp_stage = LSP_STAGE_PAUSE;
			lsp_header hdr;
			hdr.magic = 0x77;
			hdr.msg_type = lsp_msg_enum::LSP_TASK_DONE;
			
			Write(hStdout, (char*)&hdr, sizeof(lsp_header));
		}

    }

	return 0;
	printf("hello from lsp");
}
