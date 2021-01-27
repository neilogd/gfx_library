#pragma once

#include <stdint.h>

struct ProfilingTimestampData
{
    const char* name;
    int8_t id;
    uint32_t time;
};

constexpr int32_t MAX_TIMESTAMPS = 64;

extern int8_t ProfilingTimestampCount;
extern ProfilingTimestampData ProfilingTimestamps[MAX_TIMESTAMPS];

#if defined(PLATFORM_PC)
inline void ProfilingTimestamp(const char* name, int8_t id = 0)
{
    ProfilingTimestampData& timestamp = ProfilingTimestamps[ProfilingTimestampCount++ & (MAX_TIMESTAMPS-1)];
    timestamp.name = name;
    timestamp.id = id;
    timestamp.time = 0;
}

#elif defined(PLATFORM_STM32)
#include "stm32f1xx_hal.h"

inline void ProfilingTimestamp(const char* name, int8_t id = 0)
{
    ProfilingTimestampData& timestamp = ProfilingTimestamps[ProfilingTimestampCount++ & (MAX_TIMESTAMPS-1)];
    timestamp.name = name;
    timestamp.id = id;
    timestamp.time = DWT->CYCCNT;
}

#else
inline void ProfilingTimestamp(const char* name, int8_t id = 0)
{
    ProfilingTimestampData& timestamp = ProfilingTimestamps[ProfilingTimestampCount++ & (MAX_TIMESTAMPS-1)];
    timestamp.name = name;
    timestamp.id = id;
    timestamp.time = 0;
}
#endif
