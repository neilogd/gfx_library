#pragma once

#include "canvas.h"

class TileCanvas : public Canvas
{
public:
    TileCanvas(uint16_t* buffer, int16_t x, int16_t y, int16_t w, int16_t h);
    ~TileCanvas();

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t* data) override;
    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) override;

	uint32_t calcHash(uint32_t input, const void* inData, size_t size);

    /**
     * Present tile to a canvas.
     */
    bool present(Canvas& canvas, bool hashTile);

    uint16_t* buffer_;
    int16_t x_;
    int16_t y_;
    int16_t w_;
    int16_t h_;
    uint32_t hash_;
};
