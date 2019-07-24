#pragma once

#include "canvas.h"

struct Tile
{
    int16_t x_;
    int16_t y_;
    uint32_t hash_;
};

class BaseTileCanvas : public Canvas
{
public:
    BaseTileCanvas() = default;
    BaseTileCanvas(int16_t tileW, int16_t tileH, int16_t tilesX, int16_t tilesY, uint16_t* pixBuffer, Tile* tileBuffer);
    ~BaseTileCanvas();

    void init();

    int16_t begin(Canvas& canvas, bool hashTiles);

    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data) override;
    void writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) override;

	uint32_t calcHash(uint32_t input, const void* inData, size_t size);

    /**
     * Present tiles to a canvas.
     */
    bool present(Canvas& canvas, bool hashTiles);

protected:
    void flush();

    uint16_t* pixBuffer_;
    Tile* tileBuffer_;
    int16_t tileW_;
    int16_t tileH_;
    int16_t tilesX_;
    int16_t tilesY_;

    // Runtime data.
    Tile* currTile_;
    Canvas* canvas_;
    bool hashTiles_;

#if defined(PLATFORM_STM32)
    CRC_HandleTypeDef hcrc_;
#endif
};

template<int16_t TILE_W, int16_t TILE_H, int16_t CANVAS_W, int16_t CANVAS_H>
class TileCanvas : public BaseTileCanvas
{
public:
    constexpr static int32_t TILE_PIXELS = TILE_W * TILE_H;
    constexpr static int16_t TILES_X = CANVAS_W / TILE_W;
    constexpr static int16_t TILES_Y = CANVAS_H / TILE_H;
    constexpr static int16_t TILES_NUM = TILES_X * TILES_Y;

    TileCanvas() :
        BaseTileCanvas(TILE_W, TILE_H, TILES_X, TILES_Y, pixBuffer_, tileBuffer_)
    {
    }

    template<typename CALLABLE>
    void draw(Canvas& canvas, bool hashTiles, CALLABLE&& drawFn)
    {
        canvas_ = &canvas;
        hashTiles_ = hashTiles;

        for(int i = 0; i < TILES_NUM; ++i)
        {
            currTile_ = &tileBuffer_[i];
            drawFn();
            flush();
        }

        currTile_ = nullptr;
        canvas_ = nullptr;
    }

private:
    uint16_t pixBuffer_[TILE_PIXELS];
    Tile tileBuffer_[TILES_NUM];
};
