#include "tile_canvas.h"

TileCanvas::TileCanvas(uint16_t* buffer, int16_t x, int16_t y, int16_t w, int16_t h)
    : Canvas(w, h)
    , buffer_(buffer)
    , x_(x)
    , y_(y)
    , w_(w)
    , h_(h)
{
}

TileCanvas::~TileCanvas()
{
}

void TileCanvas::writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t* data)
{
    x -= x_;
    y -= y_;

    for(int j = 0; j < h; ++j)
    {
        int16_t ny = y + j;
        for(int i = 0; i < w; ++i)
        {
            int16_t nx = x + i;
            if(nx >= 0 && nx < w_ && ny >= 0 && ny < h_)
            {
                buffer_[nx + ny * w_] = *data;
            }
            ++data;
        }
    }
}

void TileCanvas::writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c)
{
    x -= x_;
    y -= y_;

    for(int j = 0; j < h; ++j)
    {
        int16_t ny = y + j;
        for(int i = 0; i < w; ++i)
        {
            int16_t nx = x + i;
            if(nx >= 0 && nx < w_ && ny >= 0 && ny < h_)
            {
                buffer_[nx + ny * w_] = c;
            }
        }
    }
}

uint32_t TileCanvas::calcHash(uint32_t input, const void* inData, size_t size)
{
    const uint8_t* data = reinterpret_cast<const uint8_t*>(inData);
    uint32_t hash = input;
    while(size--)
        hash = (uint32_t)*data++ + (hash << 6) + (hash << 16) - hash;
    return hash;
}

bool TileCanvas::present(Canvas& canvas, bool hashTile)
{
    if(hashTile)
    {
        uint32_t hash = calcHash(0, buffer_, w_ * h_ * 2);
        if(hash != hash_)
        {
            hash_ = hash;
            canvas.writePixels(x_, y_, w_, h_, buffer_);
            return true;
        }
        return false;
    }
    else
    {
        canvas.writePixels(x_, y_, w_, h_, buffer_);
        return true;
    }
}
