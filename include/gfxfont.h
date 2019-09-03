// Font structures for newer Adafruit_GFX (1.1 and later).
// Example fonts are included in 'Fonts' directory.
// To use a font in your Arduino sketch, #include the corresponding .h
// file and pass address of GFXfont struct to setFont().  Pass NULL to
// revert to 'classic' fixed-space bitmap font.

#ifndef _GFXFONT_H_
#define _GFXFONT_H_

#include <stdint.h>

/// Font data stored PER GLYPH
struct GFXglyph
{
	uint16_t bitmapOffset;     ///< Pointer into GFXfont->bitmap
	uint8_t  width;            ///< Bitmap dimensions in pixels
	uint8_t  height;           ///< Bitmap dimensions in pixels
	uint8_t  xAdvance;         ///< Distance to advance cursor (x axis)
	int8_t   xOffset;          ///< X dist from cursor pos to UL corner
	int8_t   yOffset;          ///< Y dist from cursor pos to UL corner
};

/// Data stored for FONT AS A WHOLE
struct GFXfont
{ 
	uint8_t  *bitmap;      ///< Glyph bitmaps, concatenated
	GFXglyph *glyph;       ///< Glyph array
	uint8_t   first;       ///< ASCII extents (first char)
	uint8_t   last;        ///< ASCII extents (last char)
	uint8_t   yAdvance;    ///< Newline distance (y axis)

	const GFXglyph* getGlyph(uint8_t c) const
	{
		if(c >= first && c <= last)
			return &glyph[c - first];
		else
			return nullptr;
	}

	int16_t measureTextWidth(const char* text) const
	{
		uint8_t c = 0;
		int16_t x = 0;
		int16_t w = 0;
		while((c = *text++))
		{
			if(c == '\n')
				x = 0;
			else if(const GFXglyph* glyph = getGlyph(c))
				x += glyph->xAdvance;
			if(x > w) w = x;
		}
		return w;
	}
};


#endif // _GFXFONT_H_
