#include "canvas.h"

Canvas::Canvas(int w, int h)
    : w_(w)
    , h_(h)
{

}

void Canvas::executeCommandList(const CommandList& cmdList)
{
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
                it += sizeof(CommandDrawHLine);
            }
            break;
        case CommandType::DRAW_V_LINE:
            {
                const auto& cmd = it->as<CommandDrawVLine>();
                drawVLine(cmd.x, cmd.y, cmd.h, cmd.color);
                it += sizeof(CommandDrawVLine);
            }
            break;
        case CommandType::DRAW_BOX:
            {
                const auto& cmd = it->as<CommandDrawBox>();
                drawBox(cmd.x, cmd.y, cmd.w, cmd.h, cmd.color);
                it += sizeof(CommandDrawBox);
            }
            break;
        case CommandType::DRAW_FILLED_BOX:
            {
                const auto& cmd = it->as<CommandDrawFilledBox>();
                drawFilledBox(cmd.x, cmd.y, cmd.w, cmd.h, cmd.color);
                it += sizeof(CommandDrawFilledBox);
            }
            break;
        case CommandType::DRAW_TEXT:
            {
                const auto& cmd = it->as<CommandDrawText>();
                setFont(cmd.font);
                drawText(cmd.x, cmd.y, cmd.text, cmd.fg, cmd.bg);
                it += sizeof(CommandDrawText);
            }
            break;
        }
    }
}

void Canvas::drawHLine(int16_t x, int16_t y, int16_t w, uint16_t c)
{
    writePixels(x, y, w, 1, c);
}

void Canvas::drawVLine(int16_t x, int16_t y, int16_t h, uint16_t c)
{
    writePixels(x, y, 1, h, c);
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

void Canvas::drawGlyph(int16_t x, int16_t y, const GFXglyph* glyph, uint16_t fg, uint16_t bg)
{
    const uint16_t glyphPixels = glyph->width * glyph->height;

    x += glyph->xOffset;
    y += glyph->yOffset + font_->yAdvance;
    const int8_t maxx = x + glyph->width;
    const int8_t maxy = y + glyph->height;
    const uint8_t* bitmap = &font_->bitmap[glyph->bitmapOffset];
    uint8_t mask = 0;
    uint8_t byte = 0;

    // Can we use the fast path? 
    if(fg != bg && glyphPixels <= PIXEL_BATCH_SIZE)
    {
        uint16_t pixelIndex = 0;
        for(int8_t j = y; j < maxy; j++)
        {
            for(int8_t i = x; i < maxx; i++)
            {
                if((mask>>=1) == 0x0)
                {
                    mask = 0b10000000;
                    byte = *bitmap++;
                }

                pixelBatch_[pixelIndex++] = ((byte & mask) != 0) ? fg : bg;
            }
        }

        writePixels(x, y, glyph->width, glyph->height, pixelBatch_);
    }
    else
    {
        for(int8_t j = y; j < maxy; j++)
        {
            for(int8_t i = x; i < maxx; i++)
            {
                if((mask>>=1) == 0x0)
                {
                    mask = 0b10000000;
                    byte = *bitmap++;
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
            drawGlyph(x, y, glyph, fg, bg);
            x += glyph->xAdvance;
        }
    }
}
