#include "canvas.h"
#include "tile_canvas.h"
#include "common.h"

#define PROGMEM
#include "fonts/Picopixel.h"
#include "fonts/Org_01.h"

static uint8_t cmdListBuffer[1024];
static CommandList cmdList(cmdListBuffer, sizeof(cmdListBuffer));

constexpr int16_t TILE_W = 16;
constexpr int16_t TILE_H = 16;
constexpr int16_t CANVAS_W = 128;
constexpr int16_t CANVAS_H = 128;

static TileCanvas<TILE_W, TILE_H, CANVAS_W, CANVAS_H> tileCanvas;

static bool debugShowTiles = false;
static bool debugHashTiles = false;

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

    cmdList.drawFilledBox(0, 0, 128, 128, Color565From888(0, 0, 64));

    const char* testString = "ABCDEFGHIJK\nLMNOPQRSTUV\nWXYZ\n1234567890\n!@#$%^&*()\n[]<>;:'-+=\n\n";
    int16_t texty = 0;
    cmdList.drawText(8, texty, testString, &Org_01, 0xffff, 0x0000);
    texty += 48;

    cmdList.drawFilledBox(0, texty, 64, texty + 16, Color565From888(255, 0, 255));
    cmdList.drawBox(0, texty, 64, texty + 16, Color565From888(255, 255, 255));
    cmdList.drawText(8, texty, testString, &Picopixel, 0xffff, 0xffff);
    texty += 48;

    static int16_t bx = 92;
    static int16_t by = 98;
    static int16_t dx = 2;
    static int16_t dy = 1;

    bx += dx;
    by += dy;
    if(bx >= 128 || bx <= 0) dx = -dx;
    if(by >= 128 || by <= 0) dy = -dy;
    cmdList.drawBox(bx-1, by-1, 3, 3, Color565From888(255, 0, 0));

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
    canvas.executeCommandList(cmdList);
#endif
}
