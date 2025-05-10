#pragma once
#include "Array.h"
template <typename T, typename ValT = float>
struct RatedStuff
{
	T type;
	ValT val;
	RatedStuff()
	{
	}
	RatedStuff(T t, float val)
	{
		type = t;
		this->val = val;
	}
};
template <typename T, typename ValT>
void SortRatedStuff(own_std::vector<RatedStuff<T, ValT>>* ar);
template <typename T, typename ValT>
void SortRatedStuffDescending(own_std::vector<RatedStuff<T, ValT>>* ar);
