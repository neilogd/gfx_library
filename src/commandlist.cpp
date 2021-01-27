#include "commandlist.h"

#include <memory.h>

CommandList::CommandList(uint8_t* buffer, uint32_t bufferSize)
    : bufferBegin_(buffer)
    , bufferEnd_(buffer + bufferSize)
    , bufferCurr_(buffer)
    , cursorX_(0)
    , cursorY_(0)
    , fg_(0)
    , bg_(0)
    , font_(nullptr)
{

}

void CommandList::drawHLine(int16_t x, int16_t y, int16_t w)
{
    auto* cmd = alloc<CommandDrawHLine>();
    cmd->x = x + cursorX_;
    cmd->y = y + cursorY_;
    cmd->w = w;
}

void CommandList::drawVLine(int16_t x, int16_t y, int16_t h)
{
    auto* cmd = alloc<CommandDrawVLine>();
    cmd->x = x + cursorX_;
    cmd->y = y + cursorY_;
    cmd->h = h;
}

void CommandList::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    auto* cmd = alloc<CommandDrawLine>();
    cmd->x0 = x0 + cursorX_;
    cmd->y0 = y0 + cursorY_;
    cmd->x1 = x1 + cursorX_;
    cmd->y1 = y1 + cursorY_;
}

void CommandList::drawBox(int16_t x, int16_t y, int16_t w, int16_t h)
{
    auto* cmd = alloc<CommandDrawBox>();
    cmd->x = x + cursorX_;
    cmd->y = y + cursorY_;
    cmd->w = w;
    cmd->h = h;
}

void CommandList::drawFilledBox(int16_t x, int16_t y, int16_t w, int16_t h)
{
    auto* cmd = alloc<CommandDrawFilledBox>();
    cmd->x = x + cursorX_;
    cmd->y = y + cursorY_;
    cmd->w = w;
    cmd->h = h;
}

void CommandList::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* bitmapData)
{
    auto* cmd = alloc<CommandDrawBitmap>();
    cmd->x = x + cursorX_;
    cmd->y = y + cursorY_;
    cmd->w = w;
    cmd->h = h;
    cmd->data = bitmapData;
}

void CommandList::drawPixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* pixelData)
{
    auto* cmd = alloc<CommandDrawPixels>();
    cmd->x = x + cursorX_;
    cmd->y = y + cursorY_;
    cmd->w = w;
    cmd->h = h;
    cmd->data = pixelData;
}

void CommandList::drawText(int16_t x, int16_t y, const char* text)
{
    auto* cmd = alloc<CommandDrawText>();
    cmd->x = x + cursorX_;
    cmd->y = y + cursorY_;
    cmd->length = strlen(text) + 1;
    char* cmdListText = allocData<char>(cmd->length);
    strcpy(cmdListText, text);
}

void CommandList::setColors(uint16_t fg, uint16_t bg)
{
    if(fg_ != fg || bg_ != bg || bufferCurr_ == bufferBegin_)
    {
        fg_ = fg;
        bg_ = bg;

        auto* cmd = alloc<CommandSetColors>();
        cmd->fg = fg;
        cmd->bg = bg;
    }
}

void CommandList::setFont(const GFXfont* font)
{
    if(font_ != font || bufferCurr_ == bufferBegin_)
    {
        font_ = font;
        
        auto* cmd = alloc<CommandSetFont>();
        cmd->font = font;
    }
}

void CommandList::setCursor(int16_t x, int16_t y)
{
    cursorX_ = x;
    cursorY_ = y;
}
