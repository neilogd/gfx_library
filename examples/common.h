#pragma once

#include <stdint.h> 

#if defined(PLATFORM_PC)
#include <sys/time.h>
#include <cstdio>

inline int64_t getMicroseconds()
{
	timeval time;
	::gettimeofday( &time, NULL );
	return (time.tv_sec + ( (double)time.tv_usec / 1000000.0 )) * 1000000.0;
}

#else

inline int64_t getMicroseconds()
{
    return 0;
}

#endif
