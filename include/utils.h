#pragma once

#include <stdint.h>

inline int16_t inline_abs(int16_t a)
{
    return a < 0 ? -a : a;
}

inline void inline_swap(int16_t& a, int16_t& b)
{
    int16_t temp = a;
    b = a;
    a = temp;
}

#if defined(PLATFORM_PC)
inline constexpr uint16_t Color565From888(uint16_t r, uint16_t g, uint16_t b)
{
    return ((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3));
}
#else

inline constexpr uint16_t Color565From888(uint16_t r, uint16_t g, uint16_t b)
{
    uint16_t col = ((b >> 3) << 11) | ((g >> 2) << 5) | ((r >> 3));
    return (col << 8) | (col >> 8);
}
#endif
