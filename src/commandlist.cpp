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

void CommandList::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c)
{
    auto* cmd = alloc<CommandDrawLine>();
    cmd->x0 = x0;
    cmd->y0 = y0;
    cmd->x1 = x1;
    cmd->y1 = y1;
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

void CommandList::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t fg, uint16_t bg, const uint8_t* bitmapData)
{
    auto* cmd = alloc<CommandDrawBitmap>();
    cmd->x = x;
    cmd->y = y;
    cmd->w = w;
    cmd->h = h;
    cmd->fg = fg;
    cmd->bg = bg;
    cmd->data = bitmapData;
}

void CommandList::drawPixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* pixelData)
{
    auto* cmd = alloc<CommandDrawPixels>();
    cmd->x = x;
    cmd->y = y;
    cmd->w = w;
    cmd->h = h;
    cmd->data = pixelData;
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

