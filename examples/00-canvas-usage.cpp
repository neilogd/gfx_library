#include "canvas.h"
#include "common.h"

const char* ExampleName = "00-canvas-usage";

void ExampleInit()
{
}

void ExampleInput(uint16_t id, uint16_t val)
{
}

void ExampleTick(Canvas& canvas)
{
    // Draw some filled boxes.
    canvas.setColors(Color565From888(0, 0, 0), Color565From888(0, 0, 0));
    canvas.drawFilledBox(0, 0, 128, 128);

    canvas.setColors(Color565From888(255, 0, 0), Color565From888(0, 0, 0));
    canvas.drawFilledBox(0, 0, 64, 64);

    canvas.setColors(Color565From888(0, 255, 0), Color565From888(0, 0, 0));
    canvas.drawFilledBox(64, 0, 64, 64);

    canvas.setColors(Color565From888(255, 0, 255), Color565From888(0, 0, 0));
    canvas.drawFilledBox(0, 64, 64, 64);
}
