#pragma once

template <typename T, typename U>
struct rel_ptr
{
	T offset;
	
	void operator=(std::string &ptr)
	{
	}
	void operator=(char *ptr)
	{
		offset = ptr ? (int)(ptr - (char *)&offset):0;
	}
	U *Ptr()
	{
		return offset ? (T *)((char *)&offset + offset) : nullptr;
	}
	U *operator->()
	{
		return offset ? (T *)((char *)&offset + offset) : nullptr;
	}
};

struct rel_str
{
	rel_ptr<int, char *> data;
	int len;
};

template <typename T>
struct rel_array
{
	rel_ptr<int, char> data;
	int len;
	
	T *operator[](int idx)
	{
		ASSERT(idx < len)
		return (T *)((char *)(this + 1) + idx * sizeof (T));
	}
};

struct rel_type2
{
	int name;
	int name_len;
	int type;
};

void rel_ptr<char, char *>::operator=(std::string &s)
{
	*this = (char *)s.data();
}
