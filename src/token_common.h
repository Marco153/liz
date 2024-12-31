#pragma once

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

	std::string ToString();
	
	token2 *NewTkn(lang_state *lang_stat)
	{
		auto ret = (token2 *)AllocMiscData(lang_stat, sizeof(token2));
		memset(ret, 0, sizeof(token2));
		
		if (type == T_WORD)
		{
			ret->line = line;
			ret->line_str = line_str;
			ret->line_offset = line_offset;
			ret->str = str.substr();
		}
		else
		{
			memcpy(ret, this, sizeof(token2));
		}
		
		return ret;
	}
};
