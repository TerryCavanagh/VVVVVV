#include "Font.h"

#include <tinyxml2.h>

#include "Alloc.h"
#include "Constants.h"
#include "CustomLevels.h"
#include "FileSystemUtils.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "Localization.h"
#include "UTF8.h"
#include "UtilityClass.h"
#include "Vlogging.h"
#include "XMLUtils.h"

extern "C"
{
#include <c-hashmap/map.h>
}

// Sigh... This is the second forward-declaration, we need to put this in a header file
SDL_Texture* LoadImage(const char *filename, const TextureLoadType loadtype);

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

enum FontType
{
    FontType_FONT,
    FontType_BUTTONS
};

struct Font
{
    char name[64];
    char display_name[SCREEN_WIDTH_CHARS + 1];

    FontType type;

    uint8_t glyph_w;
    uint8_t glyph_h;

    SDL_Texture* image;

    GlyphInfo* glyph_page[FONT_N_PAGES];

    char fallback_key[64];
    uint8_t fallback_idx;
    bool fallback_idx_valid;
};

struct FontContainer
{
    uint8_t count;
    Font* fonts;
    hashmap* map_name_idx;
};

struct PrintFlags
{
    uint8_t scale;
    Font* font_sel;
    uint8_t brightness;
    bool border;
    bool align_cen;
    bool align_right;
    bool cjk_low;
    bool cjk_high;
};

static FontContainer fonts_main = {};
static FontContainer fonts_custom = {};

static uint8_t font_idx_8x8 = 0;

uint8_t font_idx_options_n = 0;
uint8_t font_idx_options[20];

static bool font_level_is_interface = false;
static bool font_idx_level_is_custom = false;
static uint8_t font_idx_level = 0;

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

static Font* fallback_for(const Font* f)
{
    if (!f->fallback_idx_valid)
    {
        return NULL;
    }
    return &fonts_main.fonts[f->fallback_idx];
}

static GlyphInfo* find_glyphinfo(const Font* f, const uint32_t codepoint, const Font** f_glyph)
{
    /* Get the GlyphInfo for a specific codepoint, or <?> or ? if it doesn't exist.
     * f_glyph may be either set to f (the main specified font) or its fallback font, if it exists.
     * As a last resort, may return NULL. */
    *f_glyph = f;

    GlyphInfo* glyph = get_glyphinfo(f, codepoint);
    if (glyph != NULL && glyph_is_valid(glyph))
    {
        return glyph;
    }

    Font* f_fallback = fallback_for(f);
    if (f_fallback != NULL && (glyph = get_glyphinfo(f_fallback, codepoint)) != NULL && glyph_is_valid(glyph))
    {
        *f_glyph = f_fallback;
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

static int get_advance_ff(const Font* f, const Font* f_glyph, const GlyphInfo* glyph)
{
    /* Internal function - get the correct advance after we have
     * determined whether the glyph is from the fallback font or not. */

    if (glyph == NULL)
    {
        return f->glyph_w;
    }

    /* If the glyph is a fallback glyph, center it relative to the main font
     * instead of trusting the fallback's width */
    if (f_glyph != f)
    {
        return f->glyph_w;
    }

    return glyph->advance;
}

int get_advance(const Font* f, const uint32_t codepoint)
{
    // Get the width of a single character in a font
    if (f == NULL)
    {
        return 8;
    }

    const Font* f_glyph;
    GlyphInfo* glyph = find_glyphinfo(f, codepoint, &f_glyph);
    return get_advance_ff(f, f_glyph, glyph);
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

static uint8_t load_font(FontContainer* container, const char* name)
{
    if (container->count >= 254)
    {
        return 0;
    }
    Font* new_fonts = (Font*) SDL_realloc(container->fonts, sizeof(Font)*(container->count+1));
    if (new_fonts == NULL)
    {
        return 0;
    }
    container->fonts = new_fonts;
    uint8_t f_idx = container->count++;
    Font* f = &container->fonts[f_idx];

    vlog_info("Loading font \"%s\"...", name);

    char name_png[256];
    char name_txt[256];
    char name_xml[256];
    SDL_snprintf(name_png, sizeof(name_png), "graphics/%s.png", name);
    SDL_snprintf(name_txt, sizeof(name_txt), "graphics/%s.txt", name);
    SDL_snprintf(name_xml, sizeof(name_xml), "graphics/%s.fontmeta", name);
    SDL_strlcpy(f->name, name, sizeof(f->name));
    SDL_strlcpy(f->display_name, name, sizeof(f->display_name));

    f->type = FontType_FONT;

    f->glyph_w = 8;
    f->glyph_h = 8;

    f->fallback_key[0] = '\0';
    f->fallback_idx_valid = false;

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

        if ((pElem = hDoc.FirstChildElement("display_name").ToElement()) != NULL)
        {
            SDL_strlcpy(f->display_name, pElem->GetText(), sizeof(f->display_name));
        }
        if ((pElem = hDoc.FirstChildElement("type").ToElement()) != NULL)
        {
            if (SDL_strcmp(pElem->GetText(), "buttons") == 0)
            {
                f->type = FontType_BUTTONS;
            }
        }
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
        if ((pElem = hDoc.FirstChildElement("fallback").ToElement()) != NULL)
        {
            SDL_strlcpy(f->fallback_key, pElem->GetText(), sizeof(f->fallback_key));
        }
    }

    f->image = LoadImage(name_png, white_teeth ? TEX_COLOR : TEX_WHITE);
    SDL_zeroa(f->glyph_page);

    if (f->image == NULL)
    {
        return f_idx;
    }

    /* We may have a 2.3-style font.txt with all the characters.
     * font.txt takes priority over <chars> in the XML.
     * If neither exist, it's just ASCII. */
    bool charset_loaded = false;
    bool special_loaded = false;
    unsigned char* charmap = NULL;
    if (FILESYSTEM_areAssetsInSameRealDir(name_png, name_txt))
    {
        FILESYSTEM_loadAssetToMemory(name_txt, &charmap, NULL, false);
    }
    if (charmap != NULL)
    {
        // We have a .txt! It's an obsolete system, but it takes priority if the file exists.
        const char* current = (char*) charmap;
        int pos = 0;
        uint32_t codepoint;
        while ((codepoint = UTF8_next(&current)))
        {
            add_glyphinfo(f, codepoint, pos);
            ++pos;
        }

        VVV_free(charmap);
        charset_loaded = true;
    }

    if (xml_loaded && !charset_loaded && (pElem = hDoc.FirstChildElement("chars").ToElement()) != NULL)
    {
        // <chars> in the XML is the preferred system.
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

    if (!charset_loaded)
    {
        /* If we don't have font.txt and no <chars> tag either,
         * this font is 2.2-and-below-style plain ASCII. */
        for (uint32_t codepoint = 0x00; codepoint < 0x80; codepoint++)
        {
            add_glyphinfo(f, codepoint, codepoint);
        }
    }

    if (xml_loaded && (pElem = hDoc.FirstChildElement("special").ToElement()) != NULL)
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

    return f_idx;
}

static bool find_font_by_name(FontContainer* container, const char* name, uint8_t* idx)
{
    // Returns true if font found (and idx is set), false if not found
    if (container->map_name_idx == NULL)
    {
        // No fonts yet...
        return false;
    }

    uintptr_t i;
    if (hashmap_get(container->map_name_idx, (char*) name, SDL_strlen(name), &i))
    {
        *idx = i;
        return true;
    }
    return false;
}

bool find_main_font_by_name(const char* name, uint8_t* idx)
{
    return find_font_by_name(&fonts_main, name, idx);
}

uint8_t get_font_idx_8x8(void)
{
    return font_idx_8x8;
}

bool level_font_is_main_idx(const uint8_t idx)
{
    return !font_idx_level_is_custom && font_idx_level == idx;
}

void set_level_font(const char* name)
{
    /* Apply the choice for a certain level-specific font.
     * This function is for custom levels. */
    font_level_is_interface = false;

    if (find_font_by_name(&fonts_custom, name, &font_idx_level))
    {
        font_idx_level_is_custom = true;
    }
    else
    {
        font_idx_level_is_custom = false;
        if (!find_font_by_name(&fonts_main, name, &font_idx_level))
        {
            if (SDL_strcmp(name, "font") != 0)
            {
                set_level_font("font");
            }
            else
            {
                font_idx_level = font_idx_8x8;
            }
        }
    }
}

void set_level_font_interface(void)
{
    /* Set the level font equal to the interface font.
     * This function is for the main game. */
    font_level_is_interface = true;
}

void set_level_font_new(void)
{
    /* Set the level font to the default font for new levels.
     * This function is for starting the editor. */
    font_level_is_interface = false;
    font_idx_level_is_custom = false;
    if (loc::new_level_font == "")
    {
        /* Just take the language's font
         * (Japanese VVVVVV can make Japanese levels by default, etc) */
        font_idx_level = loc::get_langmeta()->font_idx;
    }
    else
    {
        /* If the user has changed the font (wants to make levels
         * for a different userbase) then remember that choice. */
        if (!find_main_font_by_name(loc::new_level_font.c_str(), &font_idx_level))
        {
            font_idx_level = font_idx_8x8;
        }
    }

#ifndef NO_CUSTOM_LEVELS
    cl.level_font_name = get_main_font_name(font_idx_level);
#endif
}

static void fill_map_name_idx(FontContainer* container)
{
    /* Initialize the name->idx hashmap for the fonts in this container.
     * This should only be done once, after all the fonts are added. */
    container->map_name_idx = hashmap_create();

    for (uint8_t i = 0; i < container->count; i++)
    {
        Font* f = &container->fonts[i];
        hashmap_set(container->map_name_idx, f->name, SDL_strlen(f->name), i);
    }
}

static void set_fallbacks(FontContainer* container)
{
    /* Initialize the value of fallback_idx for all fonts in this container.
     * Only main fonts can be fallback fonts. */
    for (uint8_t i = 0; i < container->count; i++)
    {
        Font* f = &container->fonts[i];
        if (find_main_font_by_name(f->fallback_key, &f->fallback_idx))
        {
            f->fallback_idx_valid = true;
        }
    }
}

static void load_font_filename(bool is_custom, const char* filename)
{
    // Load font.png, and everything that matches *.fontmeta (but not font.fontmeta)
    size_t expected_ext_start;
    bool is_fontpng = SDL_strcmp(filename, "font.png") == 0;
    if (is_fontpng)
    {
        expected_ext_start = SDL_strlen(filename)-4;
    }
    else
    {
        expected_ext_start = SDL_strlen(filename)-9;
    }
    if (is_fontpng || (endsWith(filename, ".fontmeta") && SDL_strcmp(filename, "font.fontmeta") != 0))
    {
        char font_name[64];
        SDL_strlcpy(font_name, filename, sizeof(font_name));
        font_name[SDL_min(63, expected_ext_start)] = '\0';

        uint8_t f_idx = load_font(is_custom ? &fonts_custom : &fonts_main, font_name);

        if (is_fontpng && !is_custom)
        {
            font_idx_8x8 = f_idx;
        }
    }
}

void load_main(void)
{
    // Load all global fonts
    EnumHandle handle = {};
    const char* item;
    while ((item = FILESYSTEM_enumerate("graphics", &handle)) != NULL)
    {
        load_font_filename(false, item);
    }
    FILESYSTEM_freeEnumerate(&handle);
    font_idx_level = font_idx_8x8;

    fill_map_name_idx(&fonts_main);
    set_fallbacks(&fonts_main);

    // Initialize the font menu options, 8x8 font first
    font_idx_options[0] = font_idx_8x8;
    font_idx_options_n = 1;

    for (uint8_t i = 0; i < fonts_main.count; i++)
    {
        if (i == font_idx_8x8)
        {
            continue;
        }
        if (fonts_main.fonts[i].type != FontType_FONT)
        {
            continue;
        }
        font_idx_options[font_idx_options_n++] = i;
        if (font_idx_options_n >= sizeof(font_idx_options))
        {
            break;
        }
    }
}

void load_custom(const char* name)
{
    // Load all custom (level-specific assets) fonts
    unload_custom();
    EnumHandle handle = {};
    const char* item;
    while ((item = FILESYSTEM_enumerateAssets("graphics", &handle)) != NULL)
    {
        load_font_filename(true, item);
    }
    FILESYSTEM_freeEnumerate(&handle);

    fill_map_name_idx(&fonts_custom);
    set_fallbacks(&fonts_custom);

    set_level_font(name);
}

void unload_font(Font* f)
{
    VVV_freefunc(SDL_DestroyTexture, f->image);

    for (int i = 0; i < FONT_N_PAGES; i++)
    {
        VVV_free(f->glyph_page[i]);
    }
}

void unload_font_container(FontContainer* container)
{
    VVV_freefunc(hashmap_free, container->map_name_idx);

    for (uint8_t i = 0; i < container->count; i++)
    {
        unload_font(&container->fonts[i]);
    }
    VVV_free(container->fonts);
    container->fonts = NULL;
    container->count = 0;
}

void unload_custom(void)
{
    // Unload all custom fonts
    unload_font_container(&fonts_custom);
}

void destroy(void)
{
    // Unload all fonts (main and custom) for exiting
    unload_custom();
    unload_font_container(&fonts_main);
}

static Font* container_get(FontContainer* container, uint8_t idx)
{
    /* Get a certain font from the given container (with bounds checking).
     * Does its best to return at least something,
     * but if there are no fonts whatsoever, can return NULL. */

    if (idx < container->count)
    {
        return &container->fonts[idx];
    }
    if (font_idx_8x8 < fonts_main.count)
    {
        return &fonts_main.fonts[font_idx_8x8];
    }
    if (fonts_main.count > 0)
    {
        return &fonts_main.fonts[0];
    }
    if (fonts_custom.count > 0)
    {
        return &fonts_custom.fonts[0];
    }
    return NULL;
}

static Font* fontsel_to_font(int sel)
{
    /* Take font selection integer (0-31) and turn it into the correct Font
     * 0: PR_FONT_INTERFACE - use interface font
     * 1: PR_FONT_LEVEL     - use level font
     * 2: PR_FONT_8X8       - use 8x8 font no matter what
     * 3-31:                - use (main) font index 0-28 */

    if (sel < 0)
    {
        // Shouldn't happen but better safe than sorry...
        return NULL;
    }

    switch (sel)
    {
    case 1:
        if (!font_level_is_interface)
        {
            if (font_idx_level_is_custom)
            {
                return container_get(&fonts_custom, font_idx_level);
            }
            else
            {
                return container_get(&fonts_main, font_idx_level);
            }
        }
        SDL_FALLTHROUGH;
    case 0:
        return container_get(&fonts_main, loc::get_langmeta()->font_idx);
    case 2:
        return container_get(&fonts_main, font_idx_8x8);
    }

    return container_get(&fonts_main, sel-3);
}

#define FLAG_PART(start, count) ((flags >> start) % (1 << count))
static PrintFlags decode_print_flags(uint32_t flags)
{
    PrintFlags pf;
    pf.scale = FLAG_PART(0, 3) + 1;
    pf.font_sel = fontsel_to_font(FLAG_PART(3, 5));
    pf.brightness = ~FLAG_PART(8, 8) & 0xff;
    pf.border = flags & PR_BOR;
    pf.align_cen = flags & PR_CEN;
    pf.align_right = flags & PR_RIGHT;
    pf.cjk_low = flags & PR_CJK_LOW;
    pf.cjk_high = flags & PR_CJK_HIGH;

    return pf;
}
#undef FLAG_PART

static bool next_wrap(
    Font* f,
    size_t* start,
    size_t* len,
    const char* str,
    const int maxwidth
) {
    /* Get information about the current line in wordwrapped text,
     * given this line starts at str[*start].
     * *start is updated to the start of the next line. */
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
        uint8_t codepoint_nbytes;
        uint32_t codepoint = UTF8_peek_next(&str[idx], &codepoint_nbytes);

        switch (codepoint)
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

        linewidth += get_advance(f, codepoint);

        if (linewidth > maxwidth)
        {
            if (lenfromlastspace != 0)
            {
                *len = lenfromlastspace;
                *start = lastspace + 1;
            }
            if (idx == 0)
            {
                // Oops, we're stuck at a single character
                *len = 1;
                *start += 1;
            }
            return true;
        }

        idx += codepoint_nbytes;
        *start += codepoint_nbytes;
        *len += codepoint_nbytes;
    }
}

static bool next_wrap_buf(
    Font* f,
    char buffer[],
    const size_t buffer_size,
    size_t* start,
    const char* str,
    const int maxwidth
) {
    /* Get each line of wordwrapped text, writing one line at a time to a buffer.
     * Call as follows:
     *
     * char buf[256];
     * size_t start = 0;
     * while (next_wrap_buf(font, buf, sizeof(buf), &start, "String to wordwrap", 320))
     * {
     *     // buf contains a line of text
     * }
     */
    size_t len = 0;
    const size_t prev_start = *start;

    const bool retval = next_wrap(f, start, &len, &str[*start], maxwidth);

    if (retval)
    {
        /* Like next_split_s(), don't use SDL_strlcpy() here. */
        const size_t length = SDL_min(buffer_size - 1, len);
        SDL_memcpy(buffer, &str[prev_start], length);
        buffer[length] = '\0';
    }

    return retval;
}

std::string string_wordwrap(const uint32_t flags, const std::string& s, int maxwidth, short *lines /*= NULL*/)
{
    /* Return a string wordwrapped to a maximum limit by adding newlines.
     * CJK will need to have autowordwrap disabled and have manually inserted newlines. */

    if (lines != NULL)
    {
        *lines = 1;
    }

    PrintFlags pf = decode_print_flags(flags);
    if (pf.font_sel == NULL)
    {
        return s;
    }

    const char* orig = s.c_str();

    std::string result;
    size_t start = 0;
    bool first = true;

    while (true)
    {
        size_t len = 0;
        const char* part = &orig[start];

        const bool retval = next_wrap(pf.font_sel, &start, &len, part, maxwidth);

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

std::string string_wordwrap_balanced(const uint32_t flags, const std::string& s, int maxwidth)
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
    string_wordwrap(flags, s, maxwidth, &lines);

    int bestwidth = maxwidth;
    if (lines > 1)
    {
        for (int curlimit = maxwidth; curlimit > 1; curlimit -= 8)
        {
            short try_lines;
            string_wordwrap(flags, s, curlimit, &try_lines);

            if (try_lines > lines)
            {
                bestwidth = curlimit + 8;
                break;
            }
        }
    }

    return string_wordwrap(flags, s, bestwidth);
}

std::string string_unwordwrap(const std::string& s)
{
    /* Takes a string wordwrapped by newlines, and turns it into a single line, undoing the wrapping.
     * Also trims any leading/trailing whitespace and collapses multiple spaces into one (to undo manual centering)
     * Only applied to English, so langmeta.autowordwrap isn't used here (it'd break looking up strings) */

    std::string result;
    result.reserve(s.length());
    bool latest_was_space = true; // last character was a space (or the beginning, don't want leading whitespace)
    int consecutive_newlines = 0; // number of newlines currently encountered in a row (multiple newlines should stay!)

    const char* str = s.c_str();
    uint32_t ch;
    while ((ch = UTF8_next(&str)))
    {
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
            result.append(UTF8_encode(ch).bytes);
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
    int x,
    int y,
    const int scale,
    uint8_t r,
    uint8_t g,
    uint8_t b,
    const uint8_t brightness
)
{
    /* Draws the glyph for a codepoint at x,y.
     * Returns the amount of pixels to advance the cursor. */
    const Font* f_glyph;
    GlyphInfo* glyph = find_glyphinfo(f, codepoint, &f_glyph);
    if (glyph == NULL)
    {
        return f->glyph_w * scale;
    }

    if (glyph->flags & GLYPH_COLOR && (r | g | b) != 0)
    {
        r = g = b = brightness;
    }
    else if (brightness < 255)
    {
        float bri_factor = brightness / (float) 255;
        r *= bri_factor;
        g *= bri_factor;
        b *= bri_factor;
    }

    // If the glyph is a fallback glyph, center it
    if (f_glyph != f)
    {
        x += (f->glyph_w - f_glyph->glyph_w) / 2;
        y += (f->glyph_h - f_glyph->glyph_h) / 2;
    }

    graphics.draw_grid_tile(
        f_glyph->image,
        glyph->image_idx,
        x,
        y,
        f_glyph->glyph_w,
        f_glyph->glyph_h,
        r, g, b,
        scale,
        scale * (graphics.flipmode ? -1 : 1)
    );

    return get_advance_ff(f, f_glyph, glyph) * scale;
}

const char* get_main_font_name(uint8_t idx)
{
    Font* f = container_get(&fonts_main, idx);
    if (f == NULL)
    {
        return "";
    }
    return f->name;
}

const char* get_main_font_display_name(uint8_t idx)
{
    Font* f = container_get(&fonts_main, idx);
    if (f == NULL)
    {
        return "";
    }

    if (idx == font_idx_8x8)
    {
        // Deciding the name for the 8x8 font was harder than I'd like to admit.
        if (loc::lang == "en" || loc::get_langmeta()->font_idx != font_idx_8x8)
        {
            // If you use English, or a CJK language: "english/..."
            SDL_strlcpy(
                f->display_name,
                "english/…",
                sizeof(f->display_name)
            );
        }
        else
        {
            // If you use another, e.g. German: "english/deutsch/..."
            SDL_snprintf(
                f->display_name, sizeof(f->display_name),
                "english/%s/…",
                loc::get_langmeta()->nativename.c_str()
            );
        }
    }

    return f->display_name;
}

const char* get_level_font_display_name(void)
{
    if (font_idx_level_is_custom)
    {
        Font* f = container_get(&fonts_custom, font_idx_level);
        if (f == NULL)
        {
            return "";
        }
        return f->display_name;
    }

    return get_main_font_display_name(font_idx_level);
}

bool glyph_dimensions(uint32_t flags, uint8_t* glyph_w, uint8_t* glyph_h)
{
    /* Gets the dimensions (glyph_w and glyph_h) of a certain font.
     * Returns true if the font is valid (glyph_w and/or glyph_h were written to if not NULL), false if not. */

    PrintFlags pf = decode_print_flags(flags);

    if (pf.font_sel == NULL)
    {
        return false;
    }
    if (glyph_w != NULL)
    {
        *glyph_w = pf.font_sel->glyph_w;
    }
    if (glyph_h != NULL)
    {
        *glyph_h = pf.font_sel->glyph_h;
    }
    return true;
}

int len(const uint32_t flags, const std::string& t)
{
    PrintFlags pf = decode_print_flags(flags);

    int text_len = 0;
    uint32_t codepoint;
    const char* text = t.c_str(); // TODO no std::string
    while ((codepoint = UTF8_next(&text)))
    {
        text_len += get_advance(pf.font_sel, codepoint);
    }
    return text_len * pf.scale;
}

int height(const uint32_t flags)
{
    PrintFlags pf = decode_print_flags(flags);
    if (pf.font_sel == NULL)
    {
        return 8;
    }

    return pf.font_sel->glyph_h * pf.scale;
}

void print(
    const uint32_t flags,
    int x,
    int y,
    const char* text,
    const uint8_t r,
    const uint8_t g,
    const uint8_t b
)
{
    PrintFlags pf = decode_print_flags(flags);
    if (pf.font_sel == NULL)
    {
        return;
    }

    if (pf.align_cen || pf.align_right)
    {
        const int textlen = len(flags, text);

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

    int h_diff_8 = (pf.font_sel->glyph_h-8)*pf.scale;
    if (h_diff_8 < 0)
    {
        /* If the font is less high than 8,
         * just center it (lower on screen) */
        y -= h_diff_8/2;
    }
    else if (pf.cjk_high)
    {
        y -= h_diff_8;
    }
    else if (!pf.cjk_low)
    {
        y -= h_diff_8/2;
    }

    int position = 0;
    uint32_t codepoint;
    while ((codepoint = UTF8_next(&text)))
    {
        position += font::print_char(
            pf.font_sel,
            codepoint,
            x + position,
            y,
            pf.scale,
            r,
            g,
            b,
            pf.brightness
        );
    }
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
    // Just a std::string overload for now because it's more .c_str() to add than I'm comfortable with...
    print(flags, x, y, text.c_str(), r, g, b);
}

int print_wrap(
    uint32_t flags,
    const int x,
    int y,
    const char* text,
    const uint8_t r,
    const uint8_t g,
    const uint8_t b,
    int linespacing /*= -1*/,
    int maxwidth /*= -1*/
)
{
    PrintFlags pf = decode_print_flags(flags);
    if (pf.font_sel == NULL)
    {
        return y;
    }

    if (linespacing == -1)
    {
        linespacing = 10;
    }
    linespacing = SDL_max(linespacing, pf.font_sel->glyph_h * pf.scale);

    if (maxwidth == -1)
    {
        maxwidth = 304;
    }

    if (pf.border && !graphics.notextoutline && (r|g|b) != 0)
    {
        print_wrap(flags, x, y, text, 0, 0, 0, linespacing, maxwidth);
        flags &= ~PR_BOR;
    }

    // This could fit 64 non-BMP characters onscreen, should be plenty
    char buffer[256];
    size_t start = 0;

    if (graphics.flipmode)
    {
        // Correct for the height of the resulting print.
        size_t len = 0;
        while (next_wrap(pf.font_sel, &start, &len, &text[start], maxwidth))
        {
            y += linespacing;
        }
        y -= linespacing;
        start = 0;
    }

    while (next_wrap_buf(pf.font_sel, buffer, sizeof(buffer), &start, text, maxwidth))
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
