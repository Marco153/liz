#pragma once
struct timer
{
	u64 begin_timer;
	u64 end_timer;

	u64 begin_cycles;
	u64 end_cycles;

	u64 frequency;
};

void InitTimer(timer *t)
{
	
}
void StartTimer(timer *t)
{

}
void EndTimer(timer *t)
{
	
}
float GetTimerMSFloat(timer *t)
{

	return 0.0;
}
u32 GetTimerMS(timer *t)
{
	u32 result = (t->end_timer - t->begin_timer);

	return 1;
}
float GetTimerF(timer *t)
{
	u64 result = (t->end_timer - t->begin_timer);

	return 1.0;
}
u64 GetCyclesElapsed(timer *t)
{
	return 1;
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
