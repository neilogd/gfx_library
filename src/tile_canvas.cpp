#include "tile_canvas.h"
#include "profiling.h"

#include <algorithm>

#pragma GCC optimize ("O3")

BaseTileCanvas::BaseTileCanvas(int16_t tileW, int16_t tileH, int16_t tilesX, int16_t tilesY, uint16_t* pixBuffer, Tile* tileBuffer)
    : Canvas(tileW * tilesX, tileH * tilesY)
    , tileW_(tileW)
    , tileH_(tileH)
    , tilesX_(tilesX)
    , tilesY_(tilesY)
    , pixBuffer_(pixBuffer)
    , tileBuffer_(tileBuffer)
    , currTile_(nullptr)
{
    Tile* tile = tileBuffer_;
    for(int y = 0; y < tilesY_; ++y)
    {
        for(int x = 0; x < tilesX_; ++x)
        {
            tile->x_ = x * tileW_;
            tile->y_ = y * tileH_;
            tile->hash_ = 0;
            ++tile;
        }
    }
}

BaseTileCanvas::~BaseTileCanvas()
{
}

void BaseTileCanvas::init()
{
#if defined(PLATFORM_STM32)
    __HAL_RCC_CRC_CLK_ENABLE();

    hcrc_.Instance = CRC;
    HAL_CRC_Init(&hcrc_);
#endif
}

void BaseTileCanvas::flush()
{
    if(hashTiles_)
    {
        uint32_t hash = calcHash(0, pixBuffer_, tileW_ * tileH_ * 2);
        if(hash != currTile_->hash_)
        {
            currTile_->hash_ = hash;
            canvas_->writePixels(currTile_->x_, currTile_->y_, tileW_, tileH_, pixBuffer_);
        }
    }
    else
    {
        canvas_->writePixels(currTile_->x_, currTile_->y_, tileW_, tileH_, pixBuffer_);
    }
}

void BaseTileCanvas::writePixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* data)
{
    assert(currTile_ != nullptr);
    
    const int16_t wminx = x;
    const int16_t wminy = y;
    const int16_t wmaxx = wminx + w;
    const int16_t wmaxy = wminy + h;
    const int16_t tminx = currTile_->x_;
    const int16_t tminy = currTile_->y_;
    const int16_t tmaxx = tminx + tileW_;
    const int16_t tmaxy = tminy + tileH_;
    if(wminx > tmaxx || wminy > tmaxy ||
       wmaxx < tminx || wmaxy < tminy)
    {
        return;
    }
    {
        x -= tminx;
        y -= tminy;

        for(int j = 0; j < h; ++j)
        {
            int16_t ny = y + j;
            int16_t nx = x;
            for(int i = 0; i < w; ++i)
            {
                if(nx >= 0 && nx < tileW_ && ny >= 0 && ny < tileH_)
                    pixBuffer_[nx + ny * tileW_] = *data;
                ++nx;
                ++data;
            }
        }
    }
}

void BaseTileCanvas::writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c)
{
    assert(currTile_ != nullptr);

    const int16_t wminx = x;
    const int16_t wminy = y;
    const int16_t wmaxx = wminx + w;
    const int16_t wmaxy = wminy + h;
    const int16_t tminx = currTile_->x_;
    const int16_t tminy = currTile_->y_;
    const int16_t tmaxx = tminx + tileW_;
    const int16_t tmaxy = tminy + tileH_;
    if(wminx > tmaxx || wminy > tmaxy ||
       wmaxx < tminx || wmaxy < tminy)
    {
        return;
    }
    {
        const int16_t minx = std::max(wminx - tminx, 0);
        const int16_t miny = std::max(wminy - tminy, 0);
        const int16_t maxx = std::min(wmaxx, tmaxx) - tminx;
        const int16_t maxy = std::min(wmaxy, tmaxy) - tminy;
        const int16_t pixels = maxx - minx;

        uint16_t* pixBuffer = pixBuffer_ + (miny * tileW_);
        for(int ny = miny; ny < maxy; ++ny)
        {
            for(int nx = minx; nx < maxx; ++nx)
            {
                pixBuffer[nx] = c;
            }
            pixBuffer += tileW_;
        }
    }
}

uint32_t BaseTileCanvas::calcHash(uint32_t input, const void* inData, size_t size)
{
    ProfilingTimestamp("BaseTileCanvas::calcHash BEGIN");

#if defined(PLATFORM_STM32) && defined(USE_CRC_PERIPHERAL)
    uint32_t hash = HAL_CRC_Calculate(&hcrc_, (uint32_t*)inData, size >> 2);
#else
    const uint32_t* data = reinterpret_cast<const uint32_t*>(inData);
    uint32_t hash = input;
    size >>= 2;
    while(size--)
        hash = *data++ + (hash << 6) + (hash << 16) - hash;
#endif

    ProfilingTimestamp("BaseTileCanvas::calcHash END");
    return hash;
}
