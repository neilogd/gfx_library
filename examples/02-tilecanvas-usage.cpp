#include "canvas.h"
#include "commandlist.h"
#include "tile_canvas.h"
#include "common.h"

// Declare some memory for the command list to use.
static uint8_t cmdListBuffer[1024];
// Declare command list.
static CommandList cmdList(cmdListBuffer, sizeof(cmdListBuffer));

// Setup a canvas with tile size of 32x32
constexpr int16_t TILE_W = 32;
constexpr int16_t TILE_H = 32;
constexpr int16_t CANVAS_W = 128;
constexpr int16_t CANVAS_H = 128;

static TileCanvas<TILE_W, TILE_H, CANVAS_W, CANVAS_H> tileCanvas;

const char* ExampleName = "02-tilecanvas-usage";

void ExampleInit()
{
}

void ExampleInput(uint16_t id, uint16_t val)
{
}

void ExampleTick(Canvas& canvas)
{
    // Clear command list prior to drawing.
    cmdList.clear();

    // Draw some filled boxes.
    cmdList.setColors(Color565From888(0, 0, 0), Color565From888(0, 0, 0));
    cmdList.drawFilledBox(0, 0, 128, 128);

    cmdList.setColors(Color565From888(255, 0, 0), Color565From888(0, 0, 0));
    cmdList.drawFilledBox(0, 0, 64, 64);

    cmdList.setColors(Color565From888(0, 255, 0), Color565From888(0, 0, 0));
    cmdList.drawFilledBox(64, 0, 64, 64);

    cmdList.setColors(Color565From888(255, 0, 255), Color565From888(0, 0, 0));
    cmdList.drawFilledBox(0, 64, 64, 64);

    // Use tile canvas to execute command list.
    // Lambda is called for each tile invocation.
    tileCanvas.draw(canvas, true,
        [&]()
        {
            // Get canvas to execute command list.
            tileCanvas.executeCommandList(cmdList);
        });
}
