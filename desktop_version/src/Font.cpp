#include "Font.h"

#include <tinyxml2.h>
#include <utf8/unchecked.h>

#include "Alloc.h"
#include "FileSystemUtils.h"
#include "Graphics.h"
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
        }
    }

    if (!charset_loaded)
    {
        /* If we don't have font.txt and no <chars> tag either,
         * this font is 2.2-and-below-style plain ASCII. */
        for (uint8_t codepoint = 0x00; codepoint < 0x80; codepoint++)
        {
            add_glyphinfo(f, codepoint, codepoint);
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

int get_advance(const Font* f, const uint32_t codepoint)
{
    /* Get the width of a single character in a font */
    GlyphInfo* glyph = find_glyphinfo(f, codepoint);
    if (glyph == NULL)
    {
        return f->glyph_w;
    }

    return glyph->advance;
}

int print_char(
    const Font* f,
    const uint32_t codepoint,
    const int x,
    const int y,
    const int scale,
    const uint8_t r,
    const uint8_t g,
    const uint8_t b,
    const uint8_t a
)
{
    /* Draws the glyph for a codepoint at x,y.
     * Returns the amount of pixels to advance the cursor. */
    GlyphInfo* glyph = find_glyphinfo(f, codepoint);;
    if (glyph == NULL)
    {
        return f->glyph_w * scale;
    }

    if (glyph->flags & GLYPH_COLOR)
    {
        graphics.draw_grid_tile(f->image, glyph->image_idx, x, y, f->glyph_w, f->glyph_h, 255, 255, 255, 255, scale, scale * (graphics.flipmode ? -1 : 1));
    }
    else
    {
        graphics.draw_grid_tile(f->image, glyph->image_idx, x, y, f->glyph_w, f->glyph_h, r, g, b, a, scale, scale * (graphics.flipmode ? -1 : 1));
    }

    return glyph->advance * scale;
}

} // namespace font
