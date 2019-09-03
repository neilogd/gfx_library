#pragma once

#include <stdint.h>

#if defined(PLATFORM_STM32)
#include "cmsis_gcc.h"
#endif

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

inline uint8_t reversebit8(uint8_t v)
{
#if 0 && defined(PLATFORM_STM32)
    v = __RBIT(v) >> 24;  
#else
    v = ((v & 0b11110000) >> 4) | ((v & 0b00001111) << 4);
    v = ((v & 0b11001100) >> 2) | ((v & 0b00110011) << 2);
    v = ((v & 0b10101010) >> 1) | ((v & 0b01010101) << 1);
#endif
    return v;
}


inline constexpr uint16_t Color565From888(uint16_t r, uint16_t g, uint16_t b)
{
#if defined(PLATFORM_PC)
    return ((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3));
#else
    uint16_t col = ((b >> 3) << 11) | ((g >> 2) << 5) | ((r >> 3));
    return (col << 8) | (col >> 8);
#endif
}


static constexpr uint16_t COLOR_BLACK   = Color565From888(0, 0, 0);
static constexpr uint16_t COLOR_WHITE   = Color565From888(255, 255, 255);
static constexpr uint16_t COLOR_RED     = Color565From888(255, 0, 0);
static constexpr uint16_t COLOR_GREEN   = Color565From888(0, 255, 0);
static constexpr uint16_t COLOR_BLUE    = Color565From888(0, 0, 255);
