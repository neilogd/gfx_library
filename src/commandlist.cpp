#include "commandlist.h"

#include <memory.h>

CommandList::CommandList(uint8_t* buffer, uint32_t bufferSize)
    : bufferBegin_(buffer)
    , bufferEnd_(buffer + bufferSize)
    , bufferCurr_(buffer)
{

}

void CommandList::drawHLine(int16_t x, int16_t y, int16_t w, uint16_t c)
{
    auto* cmd = alloc<CommandDrawHLine>();
    cmd->x = x;
    cmd->y = y;
    cmd->w = w;
    cmd->color = c;
}

void CommandList::drawVLine(int16_t x, int16_t y, int16_t h, uint16_t c)
{
    auto* cmd = alloc<CommandDrawVLine>();
    cmd->x = x;
    cmd->y = y;
    cmd->h = h;
    cmd->color = c;
}

void CommandList::drawBox(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c)
{
    auto* cmd = alloc<CommandDrawBox>();
    cmd->x = x;
    cmd->y = y;
    cmd->w = w;
    cmd->h = h;
    cmd->color = c;
}

void CommandList::drawFilledBox(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c)
{
    auto* cmd = alloc<CommandDrawFilledBox>();
    cmd->x = x;
    cmd->y = y;
    cmd->w = w;
    cmd->h = h;
    cmd->color = c;
}

void CommandList::drawText(int16_t x, int16_t y, const char* text, const GFXfont* font, uint16_t fg, uint16_t bg)
{
    auto* cmd = alloc<CommandDrawText>();
    cmd->x = x;
    cmd->y = y;
    cmd->text = text;
    cmd->font = font;
    cmd->fg = fg;
    cmd->bg = bg;
}

void CommandList::drawPixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* pixelData)
{
    auto* cmd = alloc<CommandDrawPixels>();
    cmd->x = x;
    cmd->y = y;
    cmd->w = w;
    cmd->h = h;
    auto* data = allocData<uint16_t>(w * h);
    memcpy(data, pixelData, w * h * 2);
}
