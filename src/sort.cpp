#include "sort.h"
template <typename T>
void SortRatedStuffDescending(own_std::vector<RatedStuff<T>>* v)
{
    auto ar = &v->ar;
	int n =  ar->count;
	for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            // Find the index of the minimum element in the remaining unsorted part
            if ((*ar)[j]->val > (*ar)[minIndex]->val){
                minIndex = j;
            }
        }
        
        // Swap the minimum element with the first element in the unsorted part
        RatedStuff<T> temp;
		memcpy(&temp, (*ar)[i], sizeof(RatedStuff<T>));

		memcpy((*ar)[i], (*ar)[minIndex], sizeof(RatedStuff<T>));
		memcpy((*ar)[minIndex], &temp, sizeof(RatedStuff<T>));
    }

}
template <typename T>

void SortRatedStuff(own_std::vector<RatedStuff<T>> *v)
{
    auto ar = &v->ar;
	int n =  ar->count;
	for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            // Find the index of the minimum element in the remaining unsorted part
            if ((*ar)[j]->val < (*ar)[minIndex]->val){
                minIndex = j;
            }
        }
        
        // Swap the minimum element with the first element in the unsorted part
        RatedStuff<T> temp;
		memcpy(&temp, (*ar)[i], sizeof(RatedStuff<T>));

		memcpy((*ar)[i], (*ar)[minIndex], sizeof(RatedStuff<T>));
		memcpy((*ar)[minIndex], &temp, sizeof(RatedStuff<T>));
    }
}