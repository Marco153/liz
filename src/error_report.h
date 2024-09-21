#pragma once

struct unit_file;
struct lang_state;
enum type_error
{
	ERR_RETURN,
	ERR_MISMATCHED_TYPES,
	ERR_MSG,
};


#define VAR_ARGS(...)__VA_ARGS__
#define REPORT_ERROR(ln, ln_offset, args)\
					int written = GetCurFileNameAndLine(lang_stat, msg_hdr, 256, ln);\
					snprintf(&msg_hdr[written], 256, args);\
					ReportError(lang_stat, ln, ln_offset, msg_hdr, 0);

char *GetFileLn(lang_state*, int line, unit_file * = nullptr);
int GetCurFileNameAndLine(lang_state *, char *buffer, int sz, int ln);
void ReportError(lang_state *, int, int, char *, int);
