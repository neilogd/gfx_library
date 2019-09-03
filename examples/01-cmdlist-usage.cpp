#include "canvas.h"
#include "commandlist.h"
#include "common.h"

// Declare some memory for the command list to use.
static uint8_t cmdListBuffer[1024];
// Declare command list.
static CommandList cmdList(cmdListBuffer, sizeof(cmdListBuffer));


const char* ExampleName = "01-cmdlist-usage";

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

    // Get canvas to execute command list.
    canvas.executeCommandList(cmdList);
}
