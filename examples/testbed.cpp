#include "canvas.h"
#include "tile_canvas.h"
#include "common.h"

#include <cmath>

#define PROGMEM
#include "fonts/Picopixel.h"
#include "fonts/Org_01.h"
#include "icons.h"

static uint8_t cmdListBuffer[1024];
static CommandList cmdList(cmdListBuffer, sizeof(cmdListBuffer));

constexpr int16_t TILE_W = 64;
constexpr int16_t TILE_H = 32;
constexpr int16_t CANVAS_W = 128;
constexpr int16_t CANVAS_H = 128;

static TileCanvas<TILE_W, TILE_H, CANVAS_W, CANVAS_H> tileCanvas;

static bool debugShowTiles = false;
static bool debugHashTiles = false;

struct GFXbitmap
{
    int16_t w;
    int16_t h;
    const uint8_t* data;
};

const char* ExampleName = "testbed";

void ExampleInit()
{

}

void ExampleInput(uint16_t id, uint16_t val)
{
    if(val > 0)
    {
        if(!debugShowTiles && id == 't')
            debugShowTiles = true;
        if(!debugHashTiles && id == 'h')
            debugHashTiles = true;
    }
    else
    {
        if(debugShowTiles && id == 't')
            debugShowTiles = false;
        if(debugHashTiles && id == 'h')
            debugHashTiles = false;
    }
}

void ExampleTick(Canvas& canvas)
{
    cmdList.clear();

    cmdList.setColors(Color565From888(0, 0, 64), 0x0000);
    cmdList.drawFilledBox(0, 0, 128, 128);

    const char* testString = "ABCDEFGHIJK\nLMNOPQRSTUV\nWXYZ\n1234567890\n!@#$%^&*()\n[]<>;:'-+=\n\n";
    int16_t texty = 0;
    cmdList.setColors(0xffff, 0x0000);
    cmdList.setFont(&Org_01);
    cmdList.drawText(8, texty, testString);
    texty += 48;

    cmdList.setColors(Color565From888(255, 0, 255), 0);
    cmdList.drawFilledBox(0, texty, 64, texty + 16);

    cmdList.setColors(Color565From888(255, 255, 255), 0);
    cmdList.drawBox(0, texty, 64, texty + 16);

    cmdList.setColors(0xffff, 0xffff);
    cmdList.setFont(&Picopixel);
    cmdList.drawText(8, texty, testString);
    texty += 48;

    static int16_t bx = 92;
    static int16_t by = 98;
    static int16_t dx = 2;
    static int16_t dy = 1;

    bx += dx;
    by += dy;
    if(bx >= 128 || bx <= 0) dx = -dx;
    if(by >= 128 || by <= 0) dy = -dy;

    static uint32_t spriteIdx = 0; 
    const uint8_t* bmpData = &icons_data[0] + ((spriteIdx >> 3) % 16) * (16*2);
    cmdList.setColors(Color565From888(255, 255, 255), Color565From888(0, 0, 0));
    cmdList.drawBitmap(bx-8, by-8, 16, 16, bmpData);
    spriteIdx++;

    cmdList.drawLine(64, 64, 128, 64);
    cmdList.drawLine(64, 64, 64, 128);
    cmdList.drawLine(64, 64, 128, 96);
    cmdList.drawLine(64, 64, 96, 128);
    cmdList.drawLine(128, 128, 64, 64);

#if 0
    static float ticker = 0.0f;
    ticker += 0.1f;
    cmdList.drawLine(64, 64, 64 + std::cos(ticker) * 32, 64 + std::sin(ticker) * 32, Color565From888(255, 255, 255));
#endif

#if defined(PLATFORM_PC)
    static int64_t debugTileExecuteTime = 0;
    static int64_t debugTilePresentTime = 0;
    static int64_t debugNumTileSamples = 0;
    static int64_t debugFrameTime = 0;
    static int64_t debugNumFrameSamples = 0;

#if 0 
    debugFrameTime -= getMicroseconds();
#endif

    tileCanvas.draw(canvas, debugHashTiles,
        []()
        {
            debugTileExecuteTime -= getMicroseconds();
            tileCanvas.executeCommandList(cmdList);
            debugTileExecuteTime += getMicroseconds();
            debugNumTileSamples += 1.0;
        });

#if 0
    debugFrameTime += getMicroseconds();
    debugNumFrameSamples += 1.0;

    static int logCount = 0;
    if(logCount++ > 30)
    {
        const int32_t cmdBufferSize = cmdList.end() - cmdList.begin(); 
        printf("Stats:\n");
        printf("- CommandList size: %d bytes\n", cmdBufferSize);
        printf("- Tiles: %d\n", (int)TILES_NUM);
        printf("- Avg. execute per tile: %f us\n", (double)debugTileExecuteTime / (double)debugNumTileSamples);
        printf("- Avg. present per tile: %f us\n", (double)debugTilePresentTime / (double)debugNumTileSamples);
        printf("- Avg. frame time: %f us\n", (double)debugFrameTime / (double)debugNumFrameSamples);
        debugTileExecuteTime = 0.0;
        debugTilePresentTime = 0.0;
        debugNumTileSamples = 0.0;
        debugFrameTime = 0.0;
        debugNumFrameSamples = 0.0;
        logCount = 0;
    }
#endif

#else
    tileCanvas.draw(canvas, debugHashTiles,
        []()
        {
            tileCanvas.executeCommandList(cmdList);
        });
#endif
}
