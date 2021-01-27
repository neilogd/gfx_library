#pragma once

#include "gfxfont.h"
#include "debug.h"

#include <stdint.h>

#define PACKED __attribute__((packed))

// Max addressable width & height.
constexpr const int MAX_W_BITS = 10;
constexpr const int MAX_H_BITS = 10;

// X & Y require a signed bit.
constexpr const int MAX_X_BITS = MAX_W_BITS + 1;
constexpr const int MAX_Y_BITS = MAX_H_BITS + 1;

enum class CommandType : uint8_t
{
    NONE,

    DRAW_H_LINE,
    DRAW_V_LINE,
    DRAW_LINE,
    DRAW_BOX,
    DRAW_FILLED_BOX,
    DRAW_BITMAP,
    DRAW_PIXELS,
    DRAW_TEXT,

    SET_COLORS,
    SET_FONT,

    MAX,
};

static_assert((int)CommandType::MAX < 16, "Exceeded maximum number of commands supported.");

struct PACKED BaseCommand
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
struct PACKED Command : BaseCommand
{
    Command()
        : BaseCommand(COMMAND_T::TYPE)
    {}

    int16_t size() const
    {
        return sizeof(COMMAND_T);
    }
};

struct PACKED CommandDrawHLine : Command<CommandDrawHLine>
{
    static const CommandType TYPE = CommandType::DRAW_H_LINE;

    int16_t x : MAX_X_BITS;
    int16_t y : MAX_Y_BITS;
    uint16_t w : MAX_W_BITS;
};

struct PACKED CommandDrawVLine : Command<CommandDrawVLine>
{
    static const CommandType TYPE = CommandType::DRAW_V_LINE;

    int16_t x : MAX_X_BITS;
    int16_t y : MAX_Y_BITS;
    uint16_t h : MAX_H_BITS;
};

struct PACKED CommandDrawLine : Command<CommandDrawLine>
{
    static const CommandType TYPE = CommandType::DRAW_LINE;

    int16_t x0 : MAX_X_BITS;
    int16_t y0 : MAX_Y_BITS;
    int16_t x1 : MAX_X_BITS;
    int16_t y1 : MAX_Y_BITS;
};

struct PACKED CommandDrawBox : Command<CommandDrawBox>
{
    static const CommandType TYPE = CommandType::DRAW_BOX;

    int16_t x : MAX_X_BITS;
    int16_t y : MAX_Y_BITS;
    uint16_t w : MAX_W_BITS;
    uint16_t h : MAX_H_BITS;
};

struct PACKED CommandDrawFilledBox : Command<CommandDrawFilledBox>
{
    static const CommandType TYPE = CommandType::DRAW_FILLED_BOX;

    int16_t x : MAX_X_BITS;
    int16_t y : MAX_Y_BITS;
    uint16_t w : MAX_W_BITS;
    uint16_t h : MAX_H_BITS;
};

struct PACKED CommandDrawBitmap : Command<CommandDrawBitmap>
{
    static const CommandType TYPE = CommandType::DRAW_BITMAP;

    int16_t x : MAX_X_BITS;
    int16_t y : MAX_Y_BITS;
    uint16_t w : MAX_W_BITS;
    uint16_t h : MAX_H_BITS;
    const uint8_t* data;

    int16_t size() const
    {
        return sizeof(CommandDrawBitmap);
    }
};

struct PACKED CommandDrawPixels : Command<CommandDrawPixels>
{
    static const CommandType TYPE = CommandType::DRAW_PIXELS;

    int16_t x : MAX_X_BITS;
    int16_t y : MAX_Y_BITS;
    uint16_t w : MAX_W_BITS;
    uint16_t h : MAX_H_BITS;
    const uint16_t* data;

    int16_t size() const
    {
        return sizeof(CommandDrawPixels);
    }
};

struct PACKED CommandDrawText : Command<CommandDrawText>
{
    static const CommandType TYPE = CommandType::DRAW_TEXT;

    int16_t x : MAX_X_BITS;
    int16_t y : MAX_Y_BITS;
    uint16_t length : 10;

    int16_t size() const
    {
        return sizeof(CommandDrawText) + length;
    }
};

struct PACKED CommandSetColors : Command<CommandSetColors>
{
    static const CommandType TYPE = CommandType::SET_COLORS;

    uint16_t fg;
    uint16_t bg;
};

struct PACKED CommandSetFont : Command<CommandSetFont>
{
    static const CommandType TYPE = CommandType::SET_FONT;

    const GFXfont* font;
};
