#pragma once
#include<string>

struct lang_state;
enum tkn_type2
{
	T_WORD,
	T_PLUS,
	T_PIPE,
	T_EQUAL,
	T_COND_AND,
	T_COND_EQ,
	T_COND_NE,
	T_PLUS_EQUAL,
	T_MINUS_EQUAL,
	T_COND_OR,
	T_EXCLAMATION,
	T_MINUS,
	T_DIV,
	T_AT,
	T_PLUS_PLUS,
	T_MINUS_MINUS,
	T_OPEN_PARENTHESES,
	T_CLOSE_PARENTHESES,
	T_NEW_LINE,
	T_SEMI_COLON,
	T_OPEN_BRACKETS,
	T_CLOSE_BRACKETS,
	T_NUM,
	T_INT,
	T_HASHTAG,
	T_FLOAT,
	T_DOLLAR,
	T_OPEN_CURLY,
	T_COLON,
	T_CLOSE_CURLY,
	T_TILDE,
	T_COMMA,
	T_POINT,
	T_GREATER_THAN,
	T_GREATER_EQ,
	T_LESSER_THAN,
	T_LESSER_EQ,
	T_MUL,
	T_AMPERSAND,
	T_PERCENT,

	T_STR_LIT,
	T_APOSTROPHE,

	T_QUESTION_MARK,

	T_TWO_POINTS,
	T_IN,

	T_THREE_POINTS,

	T_EOF,
};
struct token2
{
	tkn_type2 type;
	int line;
	int line_offset;
	int line_offset_end;
	char *line_str;
	union
	{
		std::string str;
		union
		{
			int i;
			long long i64;
			unsigned long long u64;
		};
		float f;
	};
	int flags;

	token2(){}
	token2 operator=(const token2& other) 
	{
		token2 t;
		memcpy(this, &other, sizeof(other));

		return t;
	}
	token2(const token2& other) 
	{
		memcpy(this, &other, sizeof(*this));
	}
	~token2(){}

	token2* NewTkn(lang_state* lang_stat);
	std::string ToString();
	
};
//void Tokenize2(char *input, unsigned int input_sz, own_std::vector<token2> *tkns, own_std::vector<char *> *lines_out = nullptr)
