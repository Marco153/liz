#pragma once
#include "Array.h"
template <typename T>
struct RatedStuff
{
	T type;
	float val;
	RatedStuff()
	{
	}
	RatedStuff(T t, float val)
	{
		type = t;
		this->val = val;
	}
};
template <typename T>
void SortRatedStuff(own_std::vector<RatedStuff<T>>* ar);
template <typename T>
void SortRatedStuffDescending(own_std::vector<RatedStuff<T>>* ar);
