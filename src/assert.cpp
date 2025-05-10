#pragma once

#ifdef LINUX
#define ASSERT(COND) if(!(COND)) {raise(SIGTRAP);}
#else
#define ASSERT(COND) if(!(COND)) {int *a = nullptr; *a = 0;}
#endif
