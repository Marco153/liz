#pragma once
//#include "../serializable_pound_defines.h"
enum class lsp_msg_enum
{
	ADD_FOLDER,
	INTELLISENSE,
	LSP_TASK_DONE,

	LSP_DECL_DEF_LINE,
	LSP_DECL_DEF_LINE_RES,

	LSP_GOTO_DEF,
	LSP_GOTO_DEF_RES,
	LSP_ERROR,
	LSP_INTELLISENSE_RES,

	LSP_SYNTAX,
	LSP_SYNTAX_RES,
};
enum class lsp_intention_enum
{
	PAUSED,
	WAITING_FOLDER_TO_COMPILE,
	INTELLISENSE,
	SYNTAX,
	DECL_DEF_LINE,
	GOTO_DEF,
};
struct LspHeader
{
	char magic;
	lsp_msg_enum msg_type;
	u32 msg_len;

};

struct LspPos
{
	int line;
	int column;
};

struct ToLspGoto
{
	int line_str_offset;
	int path_file_name_offset;
};
struct ToLspLineStr
{
	LspHeader hdr;
	LspPos pos;
	int line_str_len;
};
struct GotoDef
{
	LspPos line;
	int file_name_len;
};

enum class lsp_syntax_hightlight_enum
{
	NONE,
	WORD,
	SEOF,
};
struct lsp_syntax_hightlight_hdr
{
	lsp_syntax_hightlight_enum type;
};
struct lsp_syntax_hightlight_word
{
	lsp_syntax_hightlight_hdr hdr;
	int line;
	int column_start;
	int column_end;
	unsigned int color;
};
