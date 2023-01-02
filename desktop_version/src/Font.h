#ifndef FONT_H
#define FONT_H

#include <SDL.h>
#include <stdint.h>

#include "GraphicsUtil.h"

namespace font
{

#define GLYPH_EXISTS 0x1
#define GLYPH_COLOR 0x2

struct GlyphInfo
{
    uint16_t image_idx;
    uint8_t advance;
    uint8_t flags;
};

/* Codepoints go up to U+10FFFF, so we have 0x110 (272) pages
 * of 0x1000 (4096) glyphs, allocated as needed */
#define FONT_N_PAGES 0x110
#define FONT_PAGE_SIZE 0x1000

struct Font
{
    uint8_t glyph_w;
    uint8_t glyph_h;

    SDL_Texture* image;

    GlyphInfo* glyph_page[FONT_N_PAGES];
};

extern Font temp_bfont;

void load_main(void);
void load_custom(void);
void unload_custom(void);
void destroy(void);

int get_advance(const Font* f, uint32_t codepoint); // TODO de-api
int print_char(const Font* f, uint32_t codepoint, int x, int y, int scale, uint8_t r, uint8_t g, uint8_t b, uint8_t a); // TODO de-api

} // namespace font


#endif // FONT_H
