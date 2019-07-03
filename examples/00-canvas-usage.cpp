#include "canvas.h"
#include "common.h"

#define PROGMEM
#include "fonts/Picopixel.h"
#include "fonts/Org_01.h"

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
    canvas.drawFilledBox(0, 0, 128, 128, Color565From888(0, 0, 0));
    canvas.drawFilledBox(0, 0, 64, 64, Color565From888(255, 0, 0));
    canvas.drawFilledBox(64, 0, 64, 64, Color565From888(0, 255, 0));
    canvas.drawFilledBox(0, 64, 64, 64, Color565From888(255, 0, 255));
}
