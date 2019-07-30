#pragma once

#include "platform.h"
#include "gfxfont.h"
#include "commandlist.h"
#include "utils.h"

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
    void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t fg, uint16_t bg, const uint8_t* bitmapData);
    void drawPixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* pixelData);
    void drawText(int16_t x, int16_t y, const char* text, uint16_t fg, uint16_t bg);

    virtual void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c ) = 0;
    virtual void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data) = 0;

    int w() const { return w_; }
    int h() const { return h_; }

private:
    const GFXfont* font_;

    static const int16_t PIXEL_BATCH_SIZE = 8 * 8;
    uint16_t pixelBatch_[PIXEL_BATCH_SIZE];

    int w_, h_;
};
