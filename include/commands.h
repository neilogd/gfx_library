#pragma once

#include "gfxfont.h"

#include <cstdint>
#include <cassert>

enum class CommandType : uint8_t
{
    NONE,
    DRAW_H_LINE,
    DRAW_V_LINE,
    DRAW_BOX,
    DRAW_FILLED_BOX,
    DRAW_TEXT,
};

struct BaseCommand
{
    BaseCommand(CommandType inType)
        : type(inType)
    {}

    CommandType type;

    template<typename COMMAND_T>
    const COMMAND_T& as() const
    {
        assert(COMMAND_T::TYPE == type && "Invalid conversion.");
        return *static_cast<const COMMAND_T*>(this);
    }
};

static_assert(sizeof(BaseCommand) == 1, "Base command must be exactly ONE byte!");

template<typename COMMAND_T>
struct Command : BaseCommand
{
    Command()
        : BaseCommand(COMMAND_T::TYPE)
    {}
};

struct CommandDrawHLine : Command<CommandDrawHLine>
{
    static const CommandType TYPE = CommandType::DRAW_H_LINE;

    int16_t x;
    int16_t y;
    int16_t w;
    uint16_t color;
};

struct CommandDrawVLine : Command<CommandDrawVLine>
{
    static const CommandType TYPE = CommandType::DRAW_H_LINE;

    int16_t x;
    int16_t y;
    int16_t h;
    uint16_t color;
};

struct CommandDrawBox : Command<CommandDrawBox>
{
    static const CommandType TYPE = CommandType::DRAW_BOX;

    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    uint16_t color;
};

struct CommandDrawFilledBox : Command<CommandDrawFilledBox>
{
    static const CommandType TYPE = CommandType::DRAW_FILLED_BOX;

    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    uint16_t color;
};

struct CommandDrawText : Command<CommandDrawText>
{
    static const CommandType TYPE = CommandType::DRAW_TEXT;

    const GFXfont* font;
    const char* text;
    int16_t x;
    int16_t y;
    uint16_t fg;
    uint16_t bg;
};
