#pragma once
#include "Array.h"

struct timer_struct;

#define TIMER_ACTION_FUNC_TYPE_UPDATE 0 
#define TIMER_ACTION_FUNC_TYPE_TIMES_UP 1
#define TIMER_ACTION_FUNC_ARGS timer_struct *t, int type, float dt
typedef void (*timer_func)(TIMER_ACTION_FUNC_ARGS);
#define TIMER_FLAGS_USED  1
#define TIMER_FLAGS_DIRTY 2



#define SET_TIMER_ARGS timer_func func, void * user_data, float time, int idx
struct timer_struct
{
    int flags;

    float timer;
    float time;

    void *user_data;
    
    int idx;

    timer_func func;
};



