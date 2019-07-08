#include "tile_canvas.h"

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

void BaseTileCanvas::writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t* data)
{
    assert(currTile_ != nullptr);
    x -= currTile_->x_;
    y -= currTile_->y_;

    for(int j = 0; j < h; ++j)
    {
        int16_t ny = y + j;
        for(int i = 0; i < w; ++i)
        {
            int16_t nx = x + i;
            if(nx >= 0 && nx < tileW_ && ny >= 0 && ny < tileH_)
            {
                pixBuffer_[nx + ny * tileW_] = *data;
            }
            ++data;
        }
    }
}

void BaseTileCanvas::writePixels(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c)
{
    assert(currTile_ != nullptr);
    x -= currTile_->x_;
    y -= currTile_->y_;

    for(int j = 0; j < h; ++j)
    {
        int16_t ny = y + j;
        for(int i = 0; i < w; ++i)
        {
            int16_t nx = x + i;
            if(nx >= 0 && nx < tileW_ && ny >= 0 && ny < tileH_)
            {
                pixBuffer_[nx + ny * tileW_] = c;
            }
        }
    }
}

uint32_t BaseTileCanvas::calcHash(uint32_t input, const void* inData, size_t size)
{
    const uint8_t* data = reinterpret_cast<const uint8_t*>(inData);
    uint32_t hash = input;
    while(size--)
        hash = (uint32_t)*data++ + (hash << 6) + (hash << 16) - hash;
    return hash;
}
