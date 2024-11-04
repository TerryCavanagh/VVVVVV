#include "Textbox.h"

#include <SDL.h>

#include "Font.h"
#include "Localization.h"
#include "UTF8.h"
#include "Vlogging.h"

textboxclass::textboxclass(int gap)
{
    w = 0;
    h = 0;
    tl = 0;
    prev_tl = 0;
    tm = 0;
    timer = 0;

    xp = 0;
    yp = 0;
    r = 0;
    g = 0;
    b = 0;
    linegap = gap;

    flipme = false;

    rand = 0;

    large = false;

    position_absolute = false;

    should_centerx = false;
    should_centery = false;

    print_flags = PR_FONT_LEVEL;
    translate = TEXTTRANSLATE_NONE;
    function = NULL;
    fill_buttons = false;

    sprites.clear();

    image = TEXTIMAGE_NONE;

    force_outline = false;
    outline = false;

    crewmate_position = TextboxCrewmatePosition();
    original = TextboxOriginalContext();
    original.text_case = 1;
    spacing = TextboxSpacing();

    other_textbox_index = -1;
}

void textboxclass::addsprite(int x, int y, int tile, int col)
{
    TextboxSprite sprite;
    sprite.x = x;
    sprite.y = y;
    sprite.tile = tile;
    sprite.col = col;
    sprites.push_back(sprite);
}

void textboxclass::setimage(TextboxImage new_image)
{
    image = new_image;
}

void textboxclass::centerx(void)
{
    resize();
    xp = 160 - (w / 2);
}
void textboxclass::centery(void)
{
    resize();
    yp = 120 - (h / 2);
}

void textboxclass::applyposition(void)
{
    resize();
    if (!position_absolute)
    {
        reposition();
        if (should_centerx)
        {
            centerx();
        }
        if (should_centery)
        {
            centery();
        }
        if (translate == TEXTTRANSLATE_CUTSCENE)
        {
            adjust();
        }
    }
}

void textboxclass::adjust(void)
{
    resize();
    if (xp < 10) xp = 10;
    if (yp < 10) yp = 10;
    if (xp + w > 310) xp = 310 - w;
    if (yp + h > 230) yp = 230 - h;
}

void textboxclass::initcol(int rr, int gg, int bb)
{
    r = rr;
    g = gg;
    b = bb;
    tl = 0.5;
}

void textboxclass::update(void)
{
    prev_tl = tl;
    if (tm == 0)
    {
        tl += .1f;
        if (tl >= 1)
        {
            tl = 1;
            tm = 1;
        }
    }
    else if (tm == 2)
    {
        tl -= .1f;
        if (tl <= 0.5)
        {
            tl = 0.5;
            //this textbox will be removed by updatetextboxes() later
        }
    }
    if (timer > 0)
    {
        timer--;
        if (timer == 0) tm = 2;
    }
}

void textboxclass::remove(void)
{
    tm = 2;
    tl = 1.0f; //Remove mode
}

void textboxclass::removefast(void)
{
    tm = 2;
    tl = 0.4f; //Remove mode
}

void textboxclass::resize(void)
{
    //Set the width and height to the correct sizes
    int max = 0;
    for (size_t iter = 0; iter < lines.size(); iter++)
    {
        int len = font::len(print_flags, lines[iter].c_str());
        if (len > max) max = len;
    }

    // 16 for the borders
    w = max + 16;
    h = lines.size()*(font::height(print_flags) + linegap) + 16 - linegap;
}

void textboxclass::reposition(void)
{
    // Function-based translation overrides position.
    if (translate == TEXTTRANSLATE_FUNCTION)
    {
        return;
    }

    const int font_height = font::height(print_flags);

    // Reposition based off crewmate position, if applicable
    // Otherwise use original position, if applicable
    if (crewmate_position.override_x)
    {
        if (crewmate_position.dir == 1) // left
        {
            xp = crewmate_position.x - w + 16;
        }
        else if (crewmate_position.dir == 0) // right
        {
            xp = crewmate_position.x - 16;
        }
    }
    else
    {
        xp = original.x;
    }

    if (crewmate_position.override_y)
    {
        if (crewmate_position.text_above)
        {
            if (crewmate_position.dir == 1) // left
            {
                yp = crewmate_position.y - 16 - (lines.size() * (font_height + linegap) - linegap);
            }
            else if (crewmate_position.dir == 0) // right
            {
                yp = crewmate_position.y - 18 - (lines.size() * (font_height + linegap) - linegap);
            }
        }
        else
        {
            yp = crewmate_position.y + 26;
        }
    }
    else
    {
        yp = original.y;
    }
}

void textboxclass::addline(const std::string& t)
{
    lines.push_back(t);
    resize();
    if ((int)lines.size() > (large ? 26 : 11))
    {
        lines.clear();
    }
}

void textboxclass::pad(size_t left_pad, size_t right_pad)
{
    // Pad the current text with a certain number of spaces on the left and right
    if (font::is_rtl(print_flags))
    {
        // Swap left and right, because left will end up on the right and vice versa...
        size_t old_left_pad = left_pad;
        left_pad = right_pad;
        right_pad = old_left_pad;
    }

    for (size_t iter = 0; iter < lines.size(); iter++)
    {
        lines[iter] = std::string(left_pad, ' ') + lines[iter] + std::string(right_pad, ' ');
    }
    resize();
}

void textboxclass::padtowidth(size_t new_w)
{
    /* Pad the current text so that each line is new_w pixels wide.
     * Each existing line is centered in that width. */
    resize();
    uint8_t glyph_w = 8;
    font::glyph_dimensions(print_flags, &glyph_w, NULL);
    size_t chars_w = SDL_max(w-16, new_w) / glyph_w;
    for (size_t iter = 0; iter < lines.size(); iter++)
    {
        size_t n_glyphs = font::len(print_flags, lines[iter].c_str()) / glyph_w;
        signed int padding_needed = chars_w - n_glyphs;
        if (padding_needed < 0)
        {
            continue;
        }
        size_t left_pad = padding_needed / 2;
        size_t right_pad = padding_needed - left_pad;

        lines[iter] = std::string(left_pad, ' ') + lines[iter] + std::string(right_pad, ' ');
    }
    resize();
}

void textboxclass::centertext(void)
{
    padtowidth(w-16);
}

void textboxclass::wrap(int pad)
{
    /* This function just takes a single-line textbox and wraps it...
     * pad = the total number of characters we are going to pad this textbox.
     * (or how many characters we should stay clear of 288 pixels width in general)
     * Only to be used after a manual graphics.createtextbox[flipme] call,
     * or the retranslation of a text box created with said call.
     * Returns the new, total height of the textbox. */
    if (lines.empty())
    {
        vlog_error("textboxclass::wrap() has no first line!");
        return;
    }

    std::string wrapped = font::string_wordwrap_balanced(
        print_flags,
        lines[0],
        36 * 8 - pad * 8
    );
    lines.clear();

    size_t startline = 0;
    size_t newline;
    do {
        size_t pos_n = wrapped.find('\n', startline);
        size_t pos_p = wrapped.find('|', startline);
        newline = SDL_min(pos_n, pos_p);
        addline(wrapped.substr(startline, newline-startline));
        startline = newline + 1;
    } while (newline != std::string::npos);
}

void textboxclass::copyoriginaltext(void)
{
    // Copy the original back, but keep the limit of lines in mind
    lines.clear();
    for (size_t i = 0; i < original.lines.size(); i++)
    {
        addline(original.lines[i]);
    }
}

void textboxclass::applyoriginalspacing(void)
{
    if (spacing.centertext)
    {
        centertext();
    }
    if (spacing.pad_left > 0 || spacing.pad_right > 0)
    {
        pad(spacing.pad_left, spacing.pad_right);
    }
    if (spacing.padtowidth > 0)
    {
        padtowidth(spacing.padtowidth);
    }
}

void textboxclass::updatetext(void)
{
    switch (translate)
    {
    case TEXTTRANSLATE_NONE:
        copyoriginaltext();
        applyoriginalspacing();
        break;
    case TEXTTRANSLATE_CUTSCENE:
        translatecutscene();
        break;
    case TEXTTRANSLATE_FUNCTION:
        if (function == NULL)
        {
            SDL_assert(0 && "function is NULL!");
            break;
        }
        function(this);
    }
}

void textboxclass::translatecutscene(void)
{
    if (!loc::is_cutscene_translated(original.script_name))
    {
        copyoriginaltext();
        applyoriginalspacing();
        return;
    }

    // English text needs to be un-wordwrapped, translated, and re-wordwrapped
    std::string eng;
    for (size_t i = 0; i < original.lines.size(); i++)
    {
        if (i != 0)
        {
            eng.append("\n");
        }
        eng.append(original.lines[i]);
    }

    eng = font::string_unwordwrap(eng);
    const loc::TextboxFormat* format = loc::gettext_cutscene(original.script_name, eng, original.text_case);
    if (format == NULL || format->text == NULL || format->text[0] == '\0')
    {
        copyoriginaltext();
        applyoriginalspacing();
        return;
    }

    std::string tra;
    if (format->tt)
    {
        tra = std::string(format->text);
        size_t pipe;
        while (true)
        {
            pipe = tra.find('|', 0);
            if (pipe == std::string::npos)
            {
                break;
            }
            tra.replace(pipe, 1, "\n");
        }
    }
    else
    {
        tra = font::string_wordwrap_balanced(PR_FONT_LEVEL, format->text, format->wraplimit);
    }

    lines.clear();
    size_t startline = 0;
    size_t newline;
    do
    {
        newline = tra.find('\n', startline);
        lines.push_back(tra.substr(startline, newline - startline));
        startline = newline + 1;
    }
    while (newline != std::string::npos);

    resize();

    if (format->centertext)
    {
        centertext();
    }
    if (format->pad_left > 0 || format->pad_right > 0)
    {
        pad(format->pad_left, format->pad_right);
    }
    if (format->padtowidth > 0)
    {
        padtowidth(format->padtowidth);
    }
}
