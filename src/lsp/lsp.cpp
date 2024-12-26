#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <string>
#include "json.hpp"

#define LANG_NO_ENGINE
#include "../compile.cpp"


std::string parseMessage(std::string &buffer) {
    size_t header_end = buffer.find("\r\n\r\n");
    if (header_end == std::string::npos) return "";  // Header not complete

    size_t content_length_pos = buffer.find("Content-Length: ");
    if (content_length_pos == std::string::npos) return "";

    size_t start = content_length_pos + 16;  // Skip "Content-Length: "
    size_t end = buffer.find("\r\n", start);
    int content_length = std::stoi(buffer.substr(start, end - start));

    size_t total_length = header_end + 4 + content_length;
    if (buffer.size() < total_length) return "";  // Message not complete

    std::string message = buffer.substr(header_end + 4, content_length);
    //buffer = buffer.substr(total_length);  // Remove processed message
    return message;
}
void sendResponse( nlohmann::json &response, std::string *out) {

    std::string body = response.dump();
    std::string header = "Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	*out = header + body;
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	Write(hout, (char *)out->c_str(), out->size());
	CloseHandle(hout);
	//W
    //std::cout << header << body;
	//std::cout << std::flush;
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
		InsertHightlightWord(n->t, out_buffer, 0xffccccff);

	}break;
	case N_SCOPE:
	{
		scope *new_scp = n->scp;
		CreateSyntaxHighlightingNode(lang_stat, n->r, new_scp, out_buffer);
	}break;
	case N_BINOP:
	{
		switch(n->t->type)
		{
		case T_COLON:
		{
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
					InsertHightlightWord(n->r->t, out_buffer, 0xff00ff00);
				}

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
	case N_FUNC_DECL:
	{
		CreateSyntaxHighlightingNode(lang_stat, n->r, scp, out_buffer);
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
		while (true)
		{
			if (cur_node->l != nullptr && IS_FLAG_OFF(cur_node->l->flags, NODE_FLAGS_IS_PROCESSED))
			{
				CreateSyntaxHighlightingNode(lang_stat, cur_node->l, scp, out_buffer);
			}
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

	LspHeader hdr;
	hdr.magic = 0x77;
	hdr.msg_len = sizeof(hdr) + out_buffer->size();
	hdr.msg_type = lsp_msg_enum::LSP_SYNTAX_RES;
	out_buffer->insert(out_buffer->begin(), (char*)&hdr, (char*)(&hdr + 1));
}

node *GetPointNodeUpToChar(lang_state *lang_stat, char *cur_ptr)
{

	auto intl = (LspPos* )cur_ptr;
	char* str_line = (char *)(intl + 1);
	own_std::vector<token2> tkns;
	Tokenize2(str_line, intl->column, &tkns);
	if (tkns.size() <= 1)
		return nullptr;

	int val = setjmp(*lang_stat->jump_buffer);
	if (val == 0)
	{
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

int main()
{
	using json = nlohmann::json;

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
			auto hdr = (LspHeader*)tempBuffer;
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
				else if(hdr->msg_type == lsp_msg_enum::LSP_GOTO_DEF)
				{
					auto intl = (LspPos*)(hdr + 1);
					node *n = GetPointNodeUpToChar(&lang_stat, cur_ptr);
					if (!n)
						continue;


					func_decl* found = GetFuncWithLine(&lang_stat, intl->line);
					if (!found)
						continue;

					scope *scp = FindScpWithLine(found, intl->line);

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

					std::string file_name = d->from_file->path + '/' + d->from_file->name;

					LspHeader hdr;
					hdr.magic = 0x77;
					hdr.msg_type = lsp_msg_enum::LSP_GOTO_DEF_RES;
					hdr.msg_len = sizeof(LspHeader) + sizeof(LspPos);
					GotoDef intl_val;
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

					auto intl = (LspPos*)(hdr + 1);
					node *n = GetPointNodeUpToChar(&lang_stat, cur_ptr);
					if (!n)
						continue;

					func_decl* found = GetFuncWithLine(&lang_stat, intl->line);
					if (!found)
						continue;

					scope *scp = FindScpWithLine(found, intl->line);

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

					own_std::vector<char> buffer;
					own_std::vector<char> str_tbl;
					//std::vector<char> type_tbl;
					if (tp.type == TYPE_STRUCT)
					{
						type_struct2 *st = tp.strct;
						FOR_VEC(d_ptr, st->scp->vars)
						{
							decl2* d = *d_ptr;
							rel_type2 rel_tp;
							rel_tp.type = d->type.type;
							rel_tp.name = str_tbl.size();
							rel_tp.name_len = d->name.size();
							buffer.insert(buffer.end(), (char*)&rel_tp, (char*)(&rel_tp + 1));
							InsertIntoCharVector(&str_tbl, (char *)d->name.c_str(), d->name.size() + 1);
							//rel_tp.name = d->type.type;

						}
						int total_decls = st->scp->vars.size();
						buffer.insert(buffer.begin(), (char*)&total_decls, (char*)(&total_decls + 1));
						buffer.insert(buffer.end(), str_tbl.begin(), str_tbl.end());

						LspHeader hdr;
						hdr.magic = 0x77;
						hdr.msg_type = lsp_msg_enum::LSP_INTELLISENSE_RES;
						hdr.msg_len = sizeof(LspHeader) + buffer.size();

						buffer.insert(buffer.begin(), (char*)&hdr, (char*)(&hdr + 1));
						
						Write(hStdout, (char*)buffer.data(), buffer.size());
					}
				}
				else
				{
					LspHeader hdr;
					hdr.magic = 0x77;
					hdr.msg_type = lsp_msg_enum::LSP_ERROR;
					
					Write(hStdout, (char*)&hdr, sizeof(LspHeader));
				}

			}

			tempBuffer[bytesRead] = '\0';
			inputBuffer += tempBuffer;
			inputBuffer += " yes i gotchu";
			Write(hStdout, (char *)inputBuffer.data(), inputBuffer.size());
			inputBuffer.clear();

		}
		if(lang_stat.lsp_stage == LSP_STAGE_PAUSE || lang_stat.lsp_stage == LSP_STAGE_DONE)
			Sleep(100);
		LspCompile(&lang_stat, "", 0, 0, nullptr);

		if (lang_stat.lsp_stage == LSP_STAGE_DONE)
		{
			lang_stat.lsp_stage = LSP_STAGE_PAUSE;
			LspHeader hdr;
			hdr.magic = 0x77;
			hdr.msg_type = lsp_msg_enum::LSP_TASK_DONE;
			
			Write(hStdout, (char*)&hdr, sizeof(LspHeader));
		}

    }

	return 0;
	printf("hello from lsp");
}
