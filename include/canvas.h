#pragma once

#include "platform.h"
#include "gfxfont.h"
#include "commandlist.h"

#if defined(PLATFORM_PC)
constexpr uint16_t Color565From888(uint16_t r, uint16_t g, uint16_t b)
{
    return ((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3));
}
#else

constexpr uint16_t Color565From888(uint16_t r, uint16_t g, uint16_t b)
{
    uint16_t col = ((b >> 3) << 11) | ((g >> 2) << 5) | ((r >> 3));
    return (col << 8) | (col >> 8);
}
#endif

class Canvas
{
public:
    Canvas() = default;
    Canvas(int w, int h);

    void setFont(const GFXfont* font) { font_ = font; }

    void executeCommandList(const CommandList& cmdList);

    void drawHLine(int16_t x, int16_t y, int16_t w, uint16_t c);
    void drawVLine(int16_t x, int16_t y, int16_t h, uint16_t c);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c);
    void drawBox(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
    void drawFilledBox(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
    void drawText(int16_t x, int16_t y, const char* text, uint16_t fg, uint16_t bg);
    void drawPixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* pixelData);

    virtual void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c ) = 0;
    virtual void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data) = 0;

    int w() const { return w_; }
    int h() const { return h_; }

private:
    void drawGlyph(int16_t x, int16_t y, const GFXglyph* glyph, uint16_t fg, uint16_t bg);

    const GFXfont* font_;

    static const int16_t PIXEL_BATCH_SIZE = 8 * 8;
    uint16_t pixelBatch_[PIXEL_BATCH_SIZE];

    int w_, h_;


};
