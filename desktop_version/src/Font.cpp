#include "Font.h"

#include <tinyxml2.h>
#include <utf8/unchecked.h>

#include "Alloc.h"
#include "Constants.h"
#include "FileSystemUtils.h"
#include "Graphics.h"
#include "Localization.h"
#include "UtilityClass.h"
#include "XMLUtils.h"

// Sigh... This is the second forward-declaration, we need to put this in a header file
SDL_Texture* LoadImage(const char *filename, const TextureLoadType loadtype);

namespace font
{

Font temp_bfont; // replace with like, a vector of all loaded fonts

static void codepoint_split(
    const uint32_t codepoint,
    short* page,
    short* glyph
)
{
    // Splits a code point (0x10FFFF) into page (0x10F) and glyph (0xFFF)
    if (codepoint > 0x10FFFF)
    {
        codepoint_split(0xFFFD, page, glyph);
        return;
    }
    *page = codepoint >> 12;
    *glyph = codepoint % FONT_PAGE_SIZE;
}

static GlyphInfo* get_glyphinfo(
    const Font* f,
    const uint32_t codepoint
)
{
    short page, glyph;
    codepoint_split(codepoint, &page, &glyph);

    if (f->glyph_page[page] == NULL)
    {
        return NULL;
    }

    return &f->glyph_page[page][glyph];
}

static void add_glyphinfo(
    Font* f,
    const uint32_t codepoint,
    const int image_idx
)
{
    if (image_idx < 0 || image_idx > 65535)
    {
        return;
    }

    short page, glyph;
    codepoint_split(codepoint, &page, &glyph);

    if (f->glyph_page[page] == NULL)
    {
        f->glyph_page[page] = (GlyphInfo*) SDL_calloc(FONT_PAGE_SIZE, sizeof(GlyphInfo));
        if (f->glyph_page[page] == NULL)
        {
            return;
        }
    }

    f->glyph_page[page][glyph].image_idx = image_idx;
    f->glyph_page[page][glyph].advance = f->glyph_w;
    f->glyph_page[page][glyph].flags = GLYPH_EXISTS;
}

static bool glyph_is_valid(const GlyphInfo* glyph)
{
    return glyph->flags & GLYPH_EXISTS;
}

static GlyphInfo* find_glyphinfo(const Font* f, const uint32_t codepoint)
{
    /* Get the GlyphInfo for a specific codepoint, or <?> or ? if it doesn't exist.
     * As a last resort, may return NULL. */
    GlyphInfo* glyph = get_glyphinfo(f, codepoint);
    if (glyph != NULL && glyph_is_valid(glyph))
    {
        return glyph;
    }
    glyph = get_glyphinfo(f, 0xFFFD);
    if (glyph != NULL && glyph_is_valid(glyph))
    {
        return glyph;
    }
    glyph = get_glyphinfo(f, '?');
    if (glyph != NULL && glyph_is_valid(glyph))
    {
        return glyph;
    }

    return NULL;
}

int get_advance(const Font* f, const uint32_t codepoint)
{
    // Get the width of a single character in a font
    GlyphInfo* glyph = find_glyphinfo(f, codepoint);
    if (glyph == NULL)
    {
        return f->glyph_w;
    }

    return glyph->advance;
}

static bool decode_xml_range(tinyxml2::XMLElement* elem, unsigned* start, unsigned* end)
{
    // We do support hexadecimal start/end like "0x10FFFF"
    if (elem->QueryUnsignedAttribute("start", start) != tinyxml2::XML_SUCCESS
        || elem->QueryUnsignedAttribute("end", end) != tinyxml2::XML_SUCCESS
        || *end < *start || *start > 0x10FFFF
    )
    {
        return false;
    }

    *end = SDL_min(*end, 0x10FFFF);
    return true;
}

static void load_font(Font* f, const char* name)
{
    char name_png[256];
    char name_txt[256];
    char name_xml[256];
    SDL_snprintf(name_png, sizeof(name_png), "graphics/%s.png", name);
    SDL_snprintf(name_txt, sizeof(name_txt), "graphics/%s.txt", name);
    SDL_snprintf(name_xml, sizeof(name_xml), "graphics/%s.fontmeta", name);

    f->glyph_w = 8;
    f->glyph_h = 8;

    bool white_teeth = false;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLHandle hDoc(&doc);
    tinyxml2::XMLElement* pElem;
    bool xml_loaded = false;

    if (FILESYSTEM_areAssetsInSameRealDir(name_png, name_xml)
        && FILESYSTEM_loadAssetTiXml2Document(name_xml, doc)
    )
    {
        xml_loaded = true;
        hDoc = hDoc.FirstChildElement("font_metadata");

        if ((pElem = hDoc.FirstChildElement("width").ToElement()) != NULL)
        {
            f->glyph_w = help.Int(pElem->GetText());
        }
        if ((pElem = hDoc.FirstChildElement("height").ToElement()) != NULL)
        {
            f->glyph_h = help.Int(pElem->GetText());
        }
        if ((pElem = hDoc.FirstChildElement("white_teeth").ToElement()) != NULL)
        {
            // If 1, we don't need to whiten the entire font (like in old versions)
            white_teeth = help.Int(pElem->GetText());
        }
    }

    f->image = LoadImage(name_png, white_teeth ? TEX_COLOR : TEX_WHITE);
    SDL_zeroa(f->glyph_page);

    if (f->image == NULL)
    {
        return;
    }

    /* We may have a 2.3-style font.txt with all the characters.
     * font.txt takes priority over <chars> in the XML.
     * If neither exist, it's just ASCII. */
    bool charset_loaded = false;
    bool special_loaded = false;
    unsigned char* charmap = NULL;
    size_t length;
    if (FILESYSTEM_areAssetsInSameRealDir(name_png, name_txt))
    {
        FILESYSTEM_loadAssetToMemory(name_txt, &charmap, &length, false);
    }
    if (charmap != NULL)
    {
        charset_loaded = true;
        unsigned char* current = charmap;
        unsigned char* end = charmap + length;
        int pos = 0;
        while (current != end)
        {
            uint32_t codepoint = utf8::unchecked::next(current);
            add_glyphinfo(f, codepoint, pos);
            ++pos;
        }

        VVV_free(charmap);
    }

    if (xml_loaded)
    {
        if (!charset_loaded && (pElem = hDoc.FirstChildElement("chars").ToElement()) != NULL)
        {
            // <chars> in the XML is only looked at if we haven't already seen font.txt.
            int pos = 0;
            tinyxml2::XMLElement* subElem;
            FOR_EACH_XML_SUB_ELEMENT(pElem, subElem)
            {
                EXPECT_ELEM(subElem, "range");

                unsigned start, end;
                if (!decode_xml_range(subElem, &start, &end))
                {
                    continue;
                }

                for (uint32_t codepoint = start; codepoint <= end; codepoint++)
                {
                    add_glyphinfo(f, codepoint, pos);
                    ++pos;
                }
            }
            charset_loaded = true;
        }

        if ((pElem = hDoc.FirstChildElement("special").ToElement()) != NULL)
        {
            tinyxml2::XMLElement* subElem;
            FOR_EACH_XML_SUB_ELEMENT(pElem, subElem)
            {
                EXPECT_ELEM(subElem, "range");

                unsigned start, end;
                if (!decode_xml_range(subElem, &start, &end))
                {
                    continue;
                }

                int advance = subElem->IntAttribute("advance", -1);
                int color = subElem->IntAttribute("color", -1);

                for (uint32_t codepoint = start; codepoint <= end; codepoint++)
                {
                    GlyphInfo* glyph = get_glyphinfo(f, codepoint);
                    if (glyph == NULL)
                    {
                        continue;
                    }
                    if (advance >= 0 && advance < 256)
                    {
                        glyph->advance = advance;
                    }
                    if (color == 0)
                    {
                        glyph->flags &= ~GLYPH_COLOR;
                    }
                    else if (color == 1)
                    {
                        glyph->flags |= GLYPH_COLOR;
                    }
                }
            }
            special_loaded = true;
        }
    }

    if (!charset_loaded)
    {
        /* If we don't have font.txt and no <chars> tag either,
         * this font is 2.2-and-below-style plain ASCII. */
        for (uint32_t codepoint = 0x00; codepoint < 0x80; codepoint++)
        {
            add_glyphinfo(f, codepoint, codepoint);
        }
    }

    if (!special_loaded && f->glyph_w == 8 && f->glyph_h == 8)
    {
        /* If we don't have <special>, and the font is 8x8,
         * 0x00-0x1F will be less wide because that's how it has always been. */
        for (uint32_t codepoint = 0x00; codepoint < 0x20; codepoint++)
        {
            GlyphInfo* glyph = get_glyphinfo(f, codepoint);
            if (glyph != NULL)
            {
                glyph->advance = 6;
            }
        }
    }
}

void load_main(void)
{
    // TODO PHYSFS_enumerateFiles, load everything that matches *.fontmeta or font.png (but not font.fontmeta)
    load_font(&temp_bfont, "font");
}

void load_custom(void)
{
    // Custom (level-specific assets) fonts NYI
}

void unload_custom(void)
{
    /* Unload all custom fonts */

}

void destroy(void)
{
    /* Unload all fonts (main and custom) for exiting */
    Font* f = &temp_bfont;
    VVV_freefunc(SDL_DestroyTexture, f->image);

    for (int i = 0; i < FONT_N_PAGES; i++)
    {
        VVV_free(f->glyph_page[i]);
    }
}

static bool next_wrap(
    size_t* start,
    size_t* len,
    const char* str,
    const int maxwidth
) {
    /* This function is UTF-8 aware. But start/len still are bytes. */
    size_t idx = 0;
    size_t lenfromlastspace = 0;
    size_t lastspace = 0;
    int linewidth = 0;
    *len = 0;

    if (str[idx] == '\0')
    {
        return false;
    }

    while (true)
    {
        /* FIXME: This only checks one byte, not multiple! */
        if ((str[idx] & 0xC0) == 0x80)
        {
            /* Skip continuation byte. */
            goto next;
        }

        linewidth += get_advance(&font::temp_bfont, str[idx]);

        switch (str[idx])
        {
        case ' ':
            if (loc::get_langmeta()->autowordwrap)
            {
                lenfromlastspace = idx;
                lastspace = *start;
            }
            break;
        case '\n':
        case '|':
            *start += 1;
            SDL_FALLTHROUGH;
        case '\0':
            return true;
        }

        if (linewidth > maxwidth)
        {
            if (lenfromlastspace != 0)
            {
                *len = lenfromlastspace;
                *start = lastspace + 1;
            }
            return true;
        }

next:
        idx += 1;
        *start += 1;
        *len += 1;
    }
}

static bool next_wrap_s(
    char buffer[],
    const size_t buffer_size,
    size_t* start,
    const char* str,
    const int maxwidth
) {
    size_t len = 0;
    const size_t prev_start = *start;

    const bool retval = next_wrap(start, &len, &str[*start], maxwidth);

    if (retval)
    {
        /* Like next_split_s(), don't use SDL_strlcpy() here. */
        const size_t length = SDL_min(buffer_size - 1, len);
        SDL_memcpy(buffer, &str[prev_start], length);
        buffer[length] = '\0';
    }

    return retval;
}

std::string string_wordwrap(const std::string& s, int maxwidth, short *lines /*= NULL*/)
{
    /* Return a string wordwrapped to a maximum limit by adding newlines.
     * CJK will need to have autowordwrap disabled and have manually inserted newlines. */

    if (lines != NULL)
    {
        *lines = 1;
    }

    const char* orig = s.c_str();

    std::string result;
    size_t start = 0;
    bool first = true;

    while (true)
    {
        size_t len = 0;
        const char* part = &orig[start];

        const bool retval = next_wrap(&start, &len, part, maxwidth);

        if (!retval)
        {
            return result;
        }

        if (first)
        {
            first = false;
        }
        else
        {
            result.push_back('\n');

            if (lines != NULL)
            {
                (*lines)++;
            }
        }
        result.append(part, len);
    }
}

std::string string_wordwrap_balanced(const std::string& s, int maxwidth)
{
    /* Return a string wordwrapped to a limit of maxwidth by adding newlines.
     * Try to fill the lines as far as possible, and return result where lines are most filled.
     * Goal is to have all lines in textboxes be about as long and to avoid wrapping just one word to a new line.
     * CJK will need to have autowordwrap disabled and have manually inserted newlines. */

    if (!loc::get_langmeta()->autowordwrap)
    {
        return s;
    }

    short lines;
    string_wordwrap(s, maxwidth, &lines);

    int bestwidth = maxwidth;
    if (lines > 1)
    {
        for (int curlimit = maxwidth; curlimit > 1; curlimit -= 8)
        {
            short try_lines;
            string_wordwrap(s, curlimit, &try_lines);

            if (try_lines > lines)
            {
                bestwidth = curlimit + 8;
                break;
            }
        }
    }

    return string_wordwrap(s, bestwidth);
}

std::string string_unwordwrap(const std::string& s)
{
    /* Takes a string wordwrapped by newlines, and turns it into a single line, undoing the wrapping.
     * Also trims any leading/trailing whitespace and collapses multiple spaces into one (to undo manual centering)
     * Only applied to English, so langmeta.autowordwrap isn't used here (it'd break looking up strings) */

    std::string result;
    std::back_insert_iterator<std::string> inserter = std::back_inserter(result);
    std::string::const_iterator iter = s.begin();
    bool latest_was_space = true; // last character was a space (or the beginning, don't want leading whitespace)
    int consecutive_newlines = 0; // number of newlines currently encountered in a row (multiple newlines should stay!)
    while (iter != s.end())
    {
        uint32_t ch = utf8::unchecked::next(iter);

        if (ch == '\n')
        {
            if (consecutive_newlines == 0)
            {
                ch = ' ';
            }
            else if (consecutive_newlines == 1)
            {
                // The last character was already a newline, so change it back from the space we thought it should have become.
                result[result.size()-1] = '\n';
            }
            consecutive_newlines++;
        }
        else
        {
            consecutive_newlines = 0;
        }

        if (ch != ' ' || !latest_was_space)
        {
            utf8::unchecked::append(ch, inserter);
        }

        latest_was_space = (ch == ' ' || ch == '\n');
    }

    // We could have one trailing space
    if (!result.empty() && result[result.size()-1] == ' ')
    {
        result.erase(result.end()-1);
    }

    return result;
}

static int print_char(
    const Font* f,
    const uint32_t codepoint,
    const int x,
    const int y,
    const int scale,
    uint8_t r,
    uint8_t g,
    uint8_t b,
    const uint8_t a,
    const uint8_t colorglyph_bri
)
{
    /* Draws the glyph for a codepoint at x,y.
     * Returns the amount of pixels to advance the cursor. */
    GlyphInfo* glyph = find_glyphinfo(f, codepoint);
    if (glyph == NULL)
    {
        return f->glyph_w * scale;
    }

    if (glyph->flags & GLYPH_COLOR && (r | g | b) != 0)
    {
        r = g = b = colorglyph_bri;
    }

    graphics.draw_grid_tile(f->image, glyph->image_idx, x, y, f->glyph_w, f->glyph_h, r, g, b, a, scale, scale * (graphics.flipmode ? -1 : 1));

    return glyph->advance * scale;
}

#define FLAG_PART(start, count) ((flags >> start) % (1 << count))
static PrintFlags decode_print_flags(uint32_t flags)
{
    PrintFlags pf;
    pf.scale = FLAG_PART(0, 3) + 1;
    pf.font_sel = FLAG_PART(3, 5);

    if (flags & PR_AB_IS_BRI)
    {
        pf.alpha = 255;
        pf.colorglyph_bri = ~FLAG_PART(8, 8) & 0xff;
    }
    else
    {
        pf.alpha = ~FLAG_PART(8, 8) & 0xff;
        pf.colorglyph_bri = 255;
    }

    pf.border = flags & PR_BOR;
    pf.align_cen = flags & PR_CEN;
    pf.align_right = flags & PR_RIGHT;
    pf.cjk_low = flags & PR_CJK_LOW;
    pf.cjk_high = flags & PR_CJK_HIGH;

    return pf;
}
#undef FLAG_PART

int len(const uint32_t flags, const std::string& t)
{
    PrintFlags pf = decode_print_flags(flags);
    // TODO flags!

    int text_len = 0;
    std::string::const_iterator iter = t.begin();
    while (iter != t.end())
    {
        int cur = utf8::unchecked::next(iter);
        text_len += get_advance(&font::temp_bfont, cur);
    }
    return text_len;
}

void print(
    const uint32_t flags,
    int x,
    int y,
    const std::string& text,
    const uint8_t r,
    const uint8_t g,
    const uint8_t b
)
{
    PrintFlags pf = decode_print_flags(flags);

    // TODO pf.font_sel

    if (pf.align_cen || pf.align_right)
    {
        const int textlen = graphics.len(text) * pf.scale;

        if (pf.align_cen)
        {
            if (x == -1)
            {
                x = SCREEN_WIDTH_PIXELS / 2;
            }
            x = SDL_max(x - textlen/2, 0);
        }
        else
        {
            x -= textlen;
        }
    }
    // TODO cjk_low/cjk_high

    if (pf.border && !graphics.notextoutline)
    {
        static const int offsets[4][2] = {{0,-1}, {-1,0}, {1,0}, {0,1}};

        for (int offset = 0; offset < 4; offset++)
        {
            print(
                flags & ~PR_BOR & ~PR_CEN & ~PR_RIGHT,
                x + offsets[offset][0]*pf.scale,
                y + offsets[offset][1]*pf.scale,
                text,
                0, 0, 0
            );
        }
    }

    int position = 0;
    std::string::const_iterator iter = text.begin();
    while (iter != text.end())
    {
        const uint32_t character = utf8::unchecked::next(iter);
        position += font::print_char(
            &font::temp_bfont,
            character,
            x + position,
            y,
            pf.scale,
            r,
            g,
            b,
            pf.alpha,
            pf.colorglyph_bri
        );
    }
}

int print_wrap(
    const uint32_t flags,
    const int x,
    int y,
    const std::string& text,
    const uint8_t r,
    const uint8_t g,
    const uint8_t b,
    int linespacing /*= -1*/,
    int maxwidth /*= -1*/
)
{
    if (linespacing == -1)
    {
        linespacing = 10;
    }
    linespacing = SDL_max(linespacing, loc::get_langmeta()->font_h);

    if (maxwidth == -1)
    {
        maxwidth = 304;
    }

    // TODO look through all the flags

    const char* str = text.c_str();
    // This could fit 64 non-BMP characters onscreen, should be plenty
    char buffer[256];
    size_t start = 0;

    if (graphics.flipmode)
    {
        // Correct for the height of the resulting print.
        size_t len = 0;
        while (next_wrap(&start, &len, &str[start], maxwidth))
        {
            y += linespacing;
        }
        y -= linespacing;
        start = 0;
    }

    while (next_wrap_s(buffer, sizeof(buffer), &start, str, maxwidth))
    {
        print(flags, x, y, buffer, r, g, b);

        if (graphics.flipmode)
        {
            y -= linespacing;
        }
        else
        {
            y += linespacing;
        }
    }

    return y + linespacing;
}

} // namespace font
