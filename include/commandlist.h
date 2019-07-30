#pragma once

#include "commands.h"
#include <new>

class CommandList
{
public:
    CommandList(uint8_t* buffer, uint32_t bufferSize);

    void drawHLine(int16_t x, int16_t y, int16_t w, uint16_t c);
    void drawVLine(int16_t x, int16_t y, int16_t h, uint16_t c);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c);
    void drawBox(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
    void drawFilledBox(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
    void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t fg, uint16_t bg, const uint8_t* bitmapData);
    void drawPixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* pixelData);
    void drawText(int16_t x, int16_t y, const char* text, const GFXfont* font, uint16_t fg, uint16_t bg);
 
    const BaseCommand* begin() const { return reinterpret_cast<BaseCommand*>(bufferBegin_); }
    const BaseCommand* end() const { return reinterpret_cast<BaseCommand*>(bufferCurr_); }
    void clear() { bufferCurr_ = bufferBegin_; }

private:
    template<typename COMMAND_T>
    COMMAND_T* alloc()
    {
        uint8_t* bufferNext = bufferCurr_ + sizeof(COMMAND_T);

#ifdef _DEBUG
        if(bufferNext >= bufferEnd_)
        {
            return nullptr;
        }
#endif
        void* cmd = reinterpret_cast<COMMAND_T*>(bufferCurr_);
        bufferCurr_ = bufferNext;
        return new (cmd) COMMAND_T;
    }

    template<typename DATA_T>
    DATA_T* allocData(uint16_t num)
    {
        uint8_t* bufferNext = bufferCurr_ + sizeof(DATA_T) * num;

#ifdef _DEBUG
        if(bufferNext >= bufferEnd_)
        {
            return nullptr;
        }
#endif
        void* data = reinterpret_cast<DATA_T*>(bufferCurr_);
        bufferCurr_ = bufferNext;
        return reinterpret_cast<DATA_T*>(data);
    }

    uint8_t* bufferBegin_;
    uint8_t* bufferEnd_;
    uint8_t* bufferCurr_;
};
