#pragma once
struct timer
{
	__int64 begin_timer;
	__int64 end_timer;

	__int64 begin_cycles;
	__int64 end_cycles;

	__int64 frequency;
};

void InitTimer(timer *t)
{
	LARGE_INTEGER I;
	QueryPerformanceFrequency(&I);
	t->frequency = I.QuadPart;
}
void StartTimer(timer *t)
{
	LARGE_INTEGER I;
	QueryPerformanceCounter(&I);
	t->begin_timer  = I.QuadPart;
	t->begin_cycles = __rdtsc();
}
void EndTimer(timer *t)
{
	LARGE_INTEGER I;
	QueryPerformanceCounter(&I);
	t->end_timer  = I.QuadPart;
	t->end_cycles = __rdtsc();
}
u32 GetTimerMS(timer *t)
{
	u32 result = (t->end_timer - t->begin_timer);

	return (u32) (((float)result / (float)t->frequency) * 1000);
}
float GetTimerF(timer *t)
{
	__int64 result = (t->end_timer - t->begin_timer);

	return ((float)result / (float)t->frequency);
}
__int64 GetCyclesElapsed(timer *t)
{
	return t->end_cycles - t->begin_cycles;
}

/*
void timer_man::Update(float dt)
{
    timer_struct *t = nullptr;
    FOR(tmrs)
    {
        if((it->flags & TIMER_FLAGS_USED) != 0)
        {
            it->timer += dt;
            if(it->timer += it->time)
            {
                it->func(it, TIMER_ACTION_FUNC_TYPE_TIMES_UP, dt);
                if((it->flags & TIMER_FLAGS_DIRTY) != 0)
                {
                    memset(it, 0, sizeof(timer_struct));
                }
                it->flags &= ~TIMER_FLAGS_DIRTY;
            }
        }
    }
}
void timer_man::SetTimer(SET_TIMER_ARGS)
{
    timer_struct *t = nullptr;
    FOR(tmrs)
    {
        if((it->flags & TIMER_FLAGS_USED) == 0)
        {
            t = it;
        }
    }

    ASSERT(t);
    t->func = func;
    t->user_data = user_data;

}
*/
