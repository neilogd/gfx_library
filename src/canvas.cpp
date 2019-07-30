#include "canvas.h"
#include "profiling.h"

Canvas::Canvas(int w, int h)
    : w_(w)
    , h_(h)
{

}

void Canvas::executeCommandList(const CommandList& cmdList)
{
    ProfilingTimestamp("executeCommandList BEGIN");

    const BaseCommand* it = cmdList.begin();
    const BaseCommand* end = cmdList.end();

    while(it != end)
    {
        switch(it->type)
        {
        case CommandType::NONE:
            {
                it += sizeof(BaseCommand);                
            }
            break;
        case CommandType::DRAW_H_LINE:
            {
                const auto& cmd = it->as<CommandDrawHLine>();
                drawHLine(cmd.x, cmd.y, cmd.w, cmd.color);
                it += cmd.size();
            }
            break;
        case CommandType::DRAW_V_LINE:
            {
                const auto& cmd = it->as<CommandDrawVLine>();
                drawVLine(cmd.x, cmd.y, cmd.h, cmd.color);
                it += cmd.size();
            }
            break;
        case CommandType::DRAW_LINE:
            {
                const auto& cmd = it->as<CommandDrawLine>();
                drawLine(cmd.x0, cmd.y0, cmd.x1, cmd.y1, cmd.color);
                it += cmd.size();
            }
            break;
        case CommandType::DRAW_BOX:
            {
                const auto& cmd = it->as<CommandDrawBox>();
                drawBox(cmd.x, cmd.y, cmd.w, cmd.h, cmd.color);
                it += cmd.size();
            }
            break;
        case CommandType::DRAW_FILLED_BOX:
            {
                const auto& cmd = it->as<CommandDrawFilledBox>();
                drawFilledBox(cmd.x, cmd.y, cmd.w, cmd.h, cmd.color);
                it += cmd.size();
            }
            break;
        case CommandType::DRAW_BITMAP:
            {
                const auto& cmd = it->as<CommandDrawBitmap>();
                drawBitmap(cmd.x, cmd.y, cmd.w, cmd.h, cmd.fg, cmd.bg, cmd.data);
                it += cmd.size();
            }
            break;
        case CommandType::DRAW_PIXELS:
            {
                const auto& cmd = it->as<CommandDrawPixels>();
                drawPixels(cmd.x, cmd.y, cmd.w, cmd.h, cmd.data);
                it += cmd.size();
            }
            break;
        case CommandType::DRAW_TEXT:
            {
                const auto& cmd = it->as<CommandDrawText>();
                setFont(cmd.font);
                drawText(cmd.x, cmd.y, cmd.text, cmd.fg, cmd.bg);
                it += cmd.size();
            }
            break;
        }
    }

    ProfilingTimestamp("executeCommandList END");
}

void Canvas::drawHLine(int16_t x, int16_t y, int16_t w, uint16_t c)
{
    writePixels(x, y, w, 1, c);
}

void Canvas::drawVLine(int16_t x, int16_t y, int16_t h, uint16_t c)
{
    writePixels(x, y, 1, h, c);
}

void Canvas::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c)
{
    auto drawLineX = [this](int16_t x0, int16_t x1, int16_t y0, int16_t y1, uint16_t c)
    {
        int16_t dx = x1 - x0;
        int16_t dy = y1 - y0;
        int16_t yi = dy < 0 ? -1 : 1;
        int16_t xi = 1;
        if(dy < 0)
        {
            yi = -1;
            dy = -dy;
        }
        int16_t d = 2 * dy - dx;
        int16_t y = y0;
        for(int16_t x = x0; x < x1; ++x)
        {
            writePixels(x, y, 1, 1, c);
            if(d > 0)
            {
                y += yi;
                d -= 2 * dx;
            }
            d += 2 * dy;
        }
    };

    auto drawLineY = [this](int16_t x0, int16_t x1, int16_t y0, int16_t y1, uint16_t c)
    {
        int16_t dx = x1 - x0;
        int16_t dy = y1 - y0;
        int16_t xi = 1;
        if(dx < 0)
        {
            xi = -1;
            dx = -dx;
        }
        int16_t d = 2 * dx - dy;
        int16_t x = x0;
        for(int16_t y = y0; y < y1; ++y)
        {
            writePixels(x, y, 1, 1, c);
            if(d > 0)
            {
                x += xi;
                d -= 2 * dy;
            }
            d += 2 * dx;
        }
    };

    const int16_t dx = x1 - x0;
    const int16_t dy = y1 - y0;

    if(inline_abs(dy) < inline_abs(dx))
    {
        if(x0 > x1)
            drawLineX(x1, x0, y1, y0, c);
        else
            drawLineX(x0, x1, y0, y1, c);
        
    }
    else
    {
        if(y0 > y1)
            drawLineY(x1, x0, y1, y0, c);
        else
            drawLineY(x0, x1, y0, y1, c);
    }
}

void Canvas::drawBox(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c)
{
    drawHLine(x, y, w, c);
    drawHLine(x, y + h - 1, w, c);
    drawVLine(x, y, h, c);
    drawVLine(x + w - 1, y, h, c);
}

void Canvas::drawFilledBox(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c)
{
    writePixels(x, y, w, h, c);
}

void Canvas::drawText(int16_t x, int16_t y, const char* text, uint16_t fg, uint16_t bg)
{
    uint8_t c;
    int16_t ox = x;
    while((c = *text++))
    {
        if(c == '\n')
        {
            x = ox;
            y += font_->yAdvance;
        }
        else if(c >= font_->first && c <= font_->last)
        {
            const GFXglyph* glyph = &font_->glyph[c - font_->first];

            drawBitmap(x + glyph->xOffset, 
                y + glyph->yOffset + font_->yAdvance, 
                glyph->width, 
                glyph->height, 
                fg, bg, 
                &font_->bitmap[glyph->bitmapOffset]);

            x += glyph->xAdvance;
        }
    }
}

void Canvas::drawPixels(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t* pixelData)
{
    writePixels(x, y, w, h, pixelData);
}

void Canvas::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t fg, uint16_t bg, const uint8_t* bitmapData)
{
    const uint16_t numPixels = w * h;
    const int16_t maxx = x + w;
    const int16_t maxy = y + h;

    uint8_t mask = 0;
    uint8_t byte = 0;

    // Can we use the fast path? 
    if(fg != bg && numPixels <= PIXEL_BATCH_SIZE)
    {
        uint16_t pixelIndex = 0;
        for(int16_t j = y; j < maxy; j++)
        {
            for(int16_t i = x; i < maxx; i++)
            {
                if((mask>>=1) == 0x0)
                {
                    mask = 0b10000000;
                    byte = *bitmapData++;
                }

                pixelBatch_[pixelIndex++] = ((byte & mask) != 0) ? fg : bg;
            }
        }

        writePixels(x, y, w, h, pixelBatch_);
    }
    else
    {
        for(int16_t j = y; j < maxy; j++)
        {
            for(int16_t i = x; i < maxx; i++)
            {
                if((mask>>=1) == 0x0)
                {
                    mask = 0b10000000;
                    byte = *bitmapData++;
                }

                if((byte & mask) != 0)
                {
                    writePixels(i, j, 1, 1, &fg);
                }
                else if(fg != bg)
                {
                    writePixels(i, j, 1, 1, &bg);
                }                
            }
        }
    }
}
