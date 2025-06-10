#include "FontBidi.h"

#include <SDL.h>
#include <SheenBidi/SheenBidi.h>

#include "Alloc.h"
#include "UTF8.h"

extern "C"
{
#include <c-hashmap/map.h>
}

namespace font
{

struct ArabicLetter
{
    uint32_t letter;

    uint32_t isolated;
    uint32_t initial;
    uint32_t medial;
    uint32_t final;
};

// Arabic reshaping lookup table from https://github.com/TerryCavanagh/hx_arabic_shaper
static ArabicLetter arabic_letters[] = {
    // ARABIC LETTER HAMZA
    {0x0621, 0xFE80, 0, 0, 0},
    // ARABIC LETTER ALEF WITH MADDA ABOVE
    {0x0622, 0xFE81, 0, 0, 0xFE82},
    // ARABIC LETTER ALEF WITH HAMZA ABOVE
    {0x0623, 0xFE83, 0, 0, 0xFE84},
    // ARABIC LETTER WAW WITH HAMZA ABOVE
    {0x0624, 0xFE85, 0, 0, 0xFE86},
    // ARABIC LETTER ALEF WITH HAMZA BELOW
    {0x0625, 0xFE87, 0, 0, 0xFE88},
    // ARABIC LETTER YEH WITH HAMZA ABOVE
    {0x0626, 0xFE89, 0xFE8B, 0xFE8C, 0xFE8A},
    // ARABIC LETTER ALEF
    {0x0627, 0xFE8D, 0, 0, 0xFE8E},
    // ARABIC LETTER BEH
    {0x0628, 0xFE8F, 0xFE91, 0xFE92, 0xFE90},
    // ARABIC LETTER TEH MARBUTA
    {0x0629, 0xFE93, 0, 0, 0xFE94},
    // ARABIC LETTER TEH
    {0x062A, 0xFE95, 0xFE97, 0xFE98, 0xFE96},
    // ARABIC LETTER THEH
    {0x062B, 0xFE99, 0xFE9B, 0xFE9C, 0xFE9A},
    // ARABIC LETTER JEEM
    {0x062C, 0xFE9D, 0xFE9F, 0xFEA0, 0xFE9E},
    // ARABIC LETTER HAH
    {0x062D, 0xFEA1, 0xFEA3, 0xFEA4, 0xFEA2},
    // ARABIC LETTER KHAH
    {0x062E, 0xFEA5, 0xFEA7, 0xFEA8, 0xFEA6},
    // ARABIC LETTER DAL
    {0x062F, 0xFEA9, 0, 0, 0xFEAA},
    // ARABIC LETTER THAL
    {0x0630, 0xFEAB, 0, 0, 0xFEAC},
    // ARABIC LETTER REH
    {0x0631, 0xFEAD, 0, 0, 0xFEAE},
    // ARABIC LETTER ZAIN
    {0x0632, 0xFEAF, 0, 0, 0xFEB0},
    // ARABIC LETTER SEEN
    {0x0633, 0xFEB1, 0xFEB3, 0xFEB4, 0xFEB2},
    // ARABIC LETTER SHEEN
    {0x0634, 0xFEB5, 0xFEB7, 0xFEB8, 0xFEB6},
    // ARABIC LETTER SAD
    {0x0635, 0xFEB9, 0xFEBB, 0xFEBC, 0xFEBA},
    // ARABIC LETTER DAD
    {0x0636, 0xFEBD, 0xFEBF, 0xFEC0, 0xFEBE},
    // ARABIC LETTER TAH
    {0x0637, 0xFEC1, 0xFEC3, 0xFEC4, 0xFEC2},
    // ARABIC LETTER ZAH
    {0x0638, 0xFEC5, 0xFEC7, 0xFEC8, 0xFEC6},
    // ARABIC LETTER AIN
    {0x0639, 0xFEC9, 0xFECB, 0xFECC, 0xFECA},
    // ARABIC LETTER GHAIN
    {0x063A, 0xFECD, 0xFECF, 0xFED0, 0xFECE},
    // ARABIC TATWEEL
    {0x0640, 0x0640, 0x0640, 0x0640, 0x0640},
    // ARABIC LETTER FEH
    {0x0641, 0xFED1, 0xFED3, 0xFED4, 0xFED2},
    // ARABIC LETTER QAF
    {0x0642, 0xFED5, 0xFED7, 0xFED8, 0xFED6},
    // ARABIC LETTER KAF
    {0x0643, 0xFED9, 0xFEDB, 0xFEDC, 0xFEDA},
    // ARABIC LETTER LAM
    {0x0644, 0xFEDD, 0xFEDF, 0xFEE0, 0xFEDE},
    // ARABIC LETTER MEEM
    {0x0645, 0xFEE1, 0xFEE3, 0xFEE4, 0xFEE2},
    // ARABIC LETTER NOON
    {0x0646, 0xFEE5, 0xFEE7, 0xFEE8, 0xFEE6},
    // ARABIC LETTER HEH
    {0x0647, 0xFEE9, 0xFEEB, 0xFEEC, 0xFEEA},
    // ARABIC LETTER WAW
    {0x0648, 0xFEED, 0, 0, 0xFEEE},
    // ARABIC LETTER [UIGHUR KAZAKH KIRGHIZ]? ALEF MAKSURA
    {0x0649, 0xFEEF, 0xFBE8, 0xFBE9, 0xFEF0},
    // ARABIC LETTER YEH
    {0x064A, 0xFEF1, 0xFEF3, 0xFEF4, 0xFEF2},
    // ARABIC LETTER ALEF WASLA
    {0x0671, 0xFB50, 0, 0, 0xFB51},
    // ARABIC LETTER U WITH HAMZA ABOVE
    {0x0677, 0xFBDD, 0, 0, 0},
    // ARABIC LETTER TTEH
    {0x0679, 0xFB66, 0xFB68, 0xFB69, 0xFB67},
    // ARABIC LETTER TTEHEH
    {0x067A, 0xFB5E, 0xFB60, 0xFB61, 0xFB5F},
    // ARABIC LETTER BEEH
    {0x067B, 0xFB52, 0xFB54, 0xFB55, 0xFB53},
    // ARABIC LETTER PEH
    {0x067E, 0xFB56, 0xFB58, 0xFB59, 0xFB57},
    // ARABIC LETTER TEHEH
    {0x067F, 0xFB62, 0xFB64, 0xFB65, 0xFB63},
    // ARABIC LETTER BEHEH
    {0x0680, 0xFB5A, 0xFB5C, 0xFB5D, 0xFB5B},
    // ARABIC LETTER NYEH
    {0x0683, 0xFB76, 0xFB78, 0xFB79, 0xFB77},
    // ARABIC LETTER DYEH
    {0x0684, 0xFB72, 0xFB74, 0xFB75, 0xFB73},
    // ARABIC LETTER TCHEH
    {0x0686, 0xFB7A, 0xFB7C, 0xFB7D, 0xFB7B},
    // ARABIC LETTER TCHEHEH
    {0x0687, 0xFB7E, 0xFB80, 0xFB81, 0xFB7F},
    // ARABIC LETTER DDAL
    {0x0688, 0xFB88, 0, 0, 0xFB89},
    // ARABIC LETTER DAHAL
    {0x068C, 0xFB84, 0, 0, 0xFB85},
    // ARABIC LETTER DDAHAL
    {0x068D, 0xFB82, 0, 0, 0xFB83},
    // ARABIC LETTER DUL
    {0x068E, 0xFB86, 0, 0, 0xFB87},
    // ARABIC LETTER RREH
    {0x0691, 0xFB8C, 0, 0, 0xFB8D},
    // ARABIC LETTER JEH
    {0x0698, 0xFB8A, 0, 0, 0xFB8B},
    // ARABIC LETTER VEH
    {0x06A4, 0xFB6A, 0xFB6C, 0xFB6D, 0xFB6B},
    // ARABIC LETTER PEHEH
    {0x06A6, 0xFB6E, 0xFB70, 0xFB71, 0xFB6F},
    // ARABIC LETTER KEHEH
    {0x06A9, 0xFB8E, 0xFB90, 0xFB91, 0xFB8F},
    // ARABIC LETTER NG
    {0x06AD, 0xFBD3, 0xFBD5, 0xFBD6, 0xFBD4},
    // ARABIC LETTER GAF
    {0x06AF, 0xFB92, 0xFB94, 0xFB95, 0xFB93},
    // ARABIC LETTER NGOEH
    {0x06B1, 0xFB9A, 0xFB9C, 0xFB9D, 0xFB9B},
    // ARABIC LETTER GUEH
    {0x06B3, 0xFB96, 0xFB98, 0xFB99, 0xFB97},
    // ARABIC LETTER NOON GHUNNA
    {0x06BA, 0xFB9E, 0, 0, 0xFB9F},
    // ARABIC LETTER RNOON
    {0x06BB, 0xFBA0, 0xFBA2, 0xFBA3, 0xFBA1},
    // ARABIC LETTER HEH DOACHASHMEE
    {0x06BE, 0xFBAA, 0xFBAC, 0xFBAD, 0xFBAB},
    // ARABIC LETTER HEH WITH YEH ABOVE
    {0x06C0, 0xFBA4, 0, 0, 0xFBA5},
    // ARABIC LETTER HEH GOAL
    {0x06C1, 0xFBA6, 0xFBA8, 0xFBA9, 0xFBA7},
    // ARABIC LETTER KIRGHIZ OE
    {0x06C5, 0xFBE0, 0, 0, 0xFBE1},
    // ARABIC LETTER OE
    {0x06C6, 0xFBD9, 0, 0, 0xFBDA},
    // ARABIC LETTER U
    {0x06C7, 0xFBD7, 0, 0, 0xFBD8},
    // ARABIC LETTER YU
    {0x06C8, 0xFBDB, 0, 0, 0xFBDC},
    // ARABIC LETTER KIRGHIZ YU
    {0x06C9, 0xFBE2, 0, 0, 0xFBE3},
    // ARABIC LETTER VE
    {0x06CB, 0xFBDE, 0, 0, 0xFBDF},
    // ARABIC LETTER FARSI YEH
    {0x06CC, 0xFBFC, 0xFBFE, 0xFBFF, 0xFBFD},
    // ARABIC LETTER E
    {0x06D0, 0xFBE4, 0xFBE6, 0xFBE7, 0xFBE5},
    // ARABIC LETTER YEH BARREE
    {0x06D2, 0xFBAE, 0, 0, 0xFBAF},
    // ARABIC LETTER YEH BARREE WITH HAMZA ABOVE
    {0x06D3, 0xFBB0, 0, 0, 0xFBB1},

    // ZWJ
    {0x200D, 0x200D, 0x200D, 0x200D, 0x200D},
};

// Our ligatures are all simple A+B -> C conversions
struct ArabicLigature
{
    uint32_t source[2];
    uint32_t target;
    bool mandatory;
};

static ArabicLigature arabic_ligatures[] = {
    {{0xFEDF, 0xFE8E}, 0xFEFB, true},
    {{0xFEDF, 0xFE82}, 0xFEF5, true},
    {{0xFEDF, 0xFE84}, 0xFEF7, true},
    {{0xFEDF, 0xFE88}, 0xFEF9, true},
    {{0xFEE0, 0xFE8E}, 0xFEFC, true},
    {{0xFEE0, 0xFE82}, 0xFEF6, true},
    {{0xFEE0, 0xFE84}, 0xFEF8, true},
    {{0xFEE0, 0xFE88}, 0xFEFA, true},
    {{0xFE8D, 0xFEDF}, 0xFBF0, false},
    {{0xFE8E, 0xFEDF}, 0xFBF0, false},
    {{0xFEDF, 0xFEDF}, 0xFBF1, false},
    {{0xFEE0, 0xFEDF}, 0xFBF1, false},
    {{0xFE8B, 0xFE8E}, 0xFBF2, false},
    {{0xFE8C, 0xFE8E}, 0xFBF2, false},
    {{0xFEE7, 0xFE8E}, 0xFBF3, false},
    {{0xFEE8, 0xFE8E}, 0xFBF3, false},
    {{0xFE91, 0xFE8E}, 0xFBF4, false},
    {{0xFE92, 0xFE8E}, 0xFBF4, false},
    {{0xFEF3, 0xFE8E}, 0xFBF5, false},
    {{0xFEF4, 0xFE8E}, 0xFBF5, false},
    {{0xFEAD, 0xFE8D}, 0xFBF6, false},
    {{0xFEAE, 0xFE8D}, 0xFBF6, false},
    {{0xFEAF, 0xFE8D}, 0xFBF7, false},
    {{0xFEB0, 0xFE8D}, 0xFBF7, false},
    {{0xFEAD, 0xFEDF}, 0xFBF8, false},
    {{0xFEAE, 0xFEDF}, 0xFBF8, false},
    {{0xFEAF, 0xFEDF}, 0xFBF9, false},
    {{0xFEB0, 0xFEDF}, 0xFBF9, false},
    {{0xFEDF, 0xFEE0}, 0xFBF1, false},
    {{0xFEE0, 0xFEE0}, 0xFBF1, false},
};

static hashmap* arabic_letters_map;
static hashmap* arabic_ligatures_map;

void bidi_init(void)
{
    arabic_letters_map = hashmap_create();

    for (size_t i = 0; i < sizeof(arabic_letters)/sizeof(ArabicLetter); i++)
    {
        hashmap_set(
            arabic_letters_map,
            &arabic_letters[i].letter,
            sizeof(uint32_t),
            (uintptr_t) &arabic_letters[i]
        );
    }

    arabic_ligatures_map = hashmap_create();

    for (size_t i = 0; i < sizeof(arabic_ligatures)/sizeof(ArabicLigature); i++)
    {
        hashmap_set(
            arabic_ligatures_map,
            &arabic_ligatures[i].source,
            sizeof(uint32_t) * 2,
            (uintptr_t) &arabic_ligatures[i]
        );
    }
}

void bidi_destroy(void)
{
    VVV_freefunc(hashmap_free, arabic_ligatures_map);
    VVV_freefunc(hashmap_free, arabic_letters_map);
}


bool is_directional_character(const uint32_t codepoint)
{
    // LEFT-TO-RIGHT MARK and RIGHT-TO-LEFT MARK
    if (codepoint == 0x200E || codepoint == 0x200F) return true;

    // Some other directional formatting: LRE, RLE, PDF, RLO, LRO
    if (codepoint >= 0x202A && codepoint <= 0x202E) return true;

    // The more recent isolates: LRI, RLI, FSI, PDI
    if (codepoint >= 0x2066 && codepoint <= 0x2069) return true;

    return false;
}

bool is_joiner(const uint32_t codepoint)
{
    return codepoint == 0x200C || codepoint == 0x200D;
}

bool bidi_should_transform(const bool rtl, const char* text)
{
    /* Just as an optimization, only run the whole bidi machinery if the
     * language is actually an RTL one, _or_ if an RTL character is found. */

    if (rtl)
    {
        return true;
    }

    const char* text_ptr = text;
    uint32_t ch;
    while ((ch = UTF8_next(&text_ptr)))
    {
        // The standard Hebrew and Arabic blocks
        if (ch >= 0x590 && ch <= 0x77F) return true;

        // Extended Arabic B and A
        if (ch >= 0x870 && ch <= 0x8FF) return true;

        // Any directional control character
        if (is_directional_character(ch)) return true;

        // Hebrew presentation forms
        if (ch >= 0xFB1D && ch <= 0xFB4F) return true;

        // Arabic presentation forms A
        if (ch >= 0xFB50 && ch <= 0xFDFF) return true;

        // Arabic presentation forms B
        if (ch >= 0xFE70 && ch <= 0xFEFE) return true;
    }

    return false;
}

const char* bidi_transform(const bool rtl, const char* text)
{
    uint32_t utf32_in[1024];
    int n_codepoints = 0;

    const char* text_ptr = text;
    uint32_t codepoint;
    while ((codepoint = UTF8_next(&text_ptr)))
    {
        if (codepoint == '\r' || codepoint == '\n')
        {
            // Don't treat newlines in font::print differently in bidi
            codepoint = ' ';
        }

        utf32_in[n_codepoints++] = codepoint;

        if (n_codepoints >= 1023)
        {
            break;
        }
    }
    utf32_in[n_codepoints] = 0;

    if (n_codepoints == 0)
    {
        return text;
    }

    static char utf8_out[1024];
    size_t utf8_out_cur = 0;

    SBCodepointSequence codepoint_sequence = {SBStringEncodingUTF32, (void*) utf32_in, (SBUInteger) n_codepoints};

    SBAlgorithmRef algorithm = SBAlgorithmCreate(&codepoint_sequence);
    if (algorithm == NULL)
    {
        return text;
    }
    SBParagraphRef paragraph = SBAlgorithmCreateParagraph(
        algorithm,
        0,
        INT32_MAX,
        rtl ? SBLevelDefaultRTL : SBLevelDefaultLTR
    );
    SDL_assert(paragraph != NULL);
    SBUInteger paragraph_len = SBParagraphGetLength(paragraph);
    SBLineRef paragraph_line = SBParagraphCreateLine(paragraph, 0, paragraph_len);
    SDL_assert(paragraph_line != NULL);

    // Make sure )brackets( are mirrored correctly...
    SBMirrorLocatorRef mirror_locator = SBMirrorLocatorCreate();
    if (mirror_locator != NULL)
    {
        SBMirrorLocatorLoadLine(mirror_locator, paragraph_line, (void*) utf32_in);
        const SBMirrorAgent *mirror_agent = SBMirrorLocatorGetAgent(mirror_locator);
        while (SBMirrorLocatorMoveNext(mirror_locator))
        {
            utf32_in[mirror_agent->index] = mirror_agent->mirror;
        }
        VVV_freefunc(SBMirrorLocatorRelease, mirror_locator);
    }

    SBUInteger n_runs = SBLineGetRunCount(paragraph_line);
    const SBRun *runs = SBLineGetRunsPtr(paragraph_line);

    for (SBUInteger i = 0; i < n_runs; i++)
    {
        bool is_ltr = runs[i].level % 2 == 0;
        if (!is_ltr)
        {
            // Time for reshaping!
            enum arabic_form { NONE, ISOLATED, INITIAL, MEDIAL, FINAL };
            arabic_form forms[1024];
            uint32_t replacements[1024];

            const ArabicLetter* letter;
            const ArabicLetter* previous_letter = NULL;
            arabic_form previous_form = NONE;
            for (size_t c = 0; c < runs[i].length; c++)
            {
                uintptr_t letter_ptr;
                bool found = hashmap_get(arabic_letters_map, &utf32_in[runs[i].offset + c], sizeof(uint32_t), &letter_ptr);
                if (!found)
                {
                    forms[c] = NONE;
                    replacements[c] = 0;
                    previous_form = NONE;
                    previous_letter = NULL;
                    continue;
                }
                letter = (const ArabicLetter*) letter_ptr;

                if (previous_form == NONE)
                {
                    // Maybe the first letter, or the one after an unknown one
                    forms[c] = ISOLATED;
                    replacements[c] = letter->isolated;
                }
                else if (letter->final == 0 && letter->medial == 0)
                {
                    // letter doesn't connect with the one before
                    forms[c] = ISOLATED;
                    replacements[c] = letter->isolated;
                }
                else if (previous_letter->initial == 0 && previous_letter->medial == 0)
                {
                    // previous_letter doesn't connect with the one after
                    forms[c] = ISOLATED;
                    replacements[c] = letter->isolated;
                }
                else if (previous_form == FINAL && previous_letter->medial == 0)
                {
                    // previous_letter doesn't connect with the ones before and after
                    forms[c] = ISOLATED;
                    replacements[c] = letter->isolated;
                }
                else if (previous_form == ISOLATED)
                {
                    forms[c-1] = INITIAL;
                    forms[c] = FINAL;
                    replacements[c-1] = previous_letter->initial;
                    replacements[c] = letter->final;
                }
                else
                {
                    /* Otherwise, we will change the previous letter
                     * to connect to the current letter */
                    forms[c-1] = MEDIAL;
                    forms[c] = FINAL;
                    replacements[c-1] = previous_letter->medial;
                    replacements[c] = letter->final;
                }

                previous_form = forms[c];
                previous_letter = (const ArabicLetter*) letter;
            }

            // Now that we have all the forms, time to change the codepoints!
            for (size_t c = 0; c < runs[i].length; c++)
            {
                if (replacements[c] != 0)
                {
                    utf32_in[runs[i].offset + c] = replacements[c];
                }
            }

            /* Ligature time! We have to do these after the reshaping process, that is, now!
             * Again, all our ligatures are just A+B -> C, so we can just do a single pass,
             * up until the second-to-last character (because the last character can't form
             * a ligature with the character after).
             * Actually, did I say single pass... The mandatory ligatures must be prioritized
             * over the optional ones... */
            for (char pass = 0; pass < 2; pass++)
            {
                for (size_t c = 0; c < runs[i].length - 1; c++)
                {
                    if (pass == 1 && utf32_in[runs[i].offset + c + 1] == 0xFFFFFFFF)
                    {
                        c++;
                        continue;
                    }

                    uintptr_t ligature_ptr;
                    bool found = hashmap_get(arabic_ligatures_map, &utf32_in[runs[i].offset + c], sizeof(uint32_t)*2, &ligature_ptr);
                    if (!found)
                    {
                        continue;
                    }

                    const ArabicLigature* ligature = (const ArabicLigature*) ligature_ptr;
                    if (pass == 0 && !ligature->mandatory)
                    {
                        continue;
                    }

                    /* We have a match, that means [c]+[c+1] needs to be replaced!
                     * We'll use 0xFFFFFFFF as a special tombstone character,
                     * otherwise we'd have to keep shifting the array contents... */
                    utf32_in[runs[i].offset + c] = ligature->target;
                    utf32_in[runs[i].offset + c + 1] = 0xFFFFFFFF;

                    // Don't bother comparing the tombstone with the next letter
                    c++;
                }
            }
        }
        for (size_t c = 0; c < runs[i].length; c++)
        {
            size_t ix;
            if (is_ltr)
            {
                ix = runs[i].offset + c;
            }
            else
            {
                ix = runs[i].offset + runs[i].length - 1 - c;
            }

            if (utf32_in[ix] == 0xFFFFFFFF)
            {
                continue;
            }

            int out_room_left = sizeof(utf8_out) - 1 - utf8_out_cur;
            if (out_room_left <= 0)
            {
                goto no_more_runs;
            }

            UTF8_encoding enc = UTF8_encode(utf32_in[ix]);
            size_t n_copy = SDL_min(enc.nbytes, (size_t) out_room_left);
            SDL_memcpy(
                &utf8_out[utf8_out_cur],
                enc.bytes,
                n_copy
            );

            utf8_out_cur += n_copy;
        }
    }
    no_more_runs:
    utf8_out[utf8_out_cur] = '\0';

    VVV_freefunc(SBLineRelease, paragraph_line);
    VVV_freefunc(SBParagraphRelease, paragraph);
    VVV_freefunc(SBAlgorithmRelease, algorithm);

    return utf8_out;
}

} // namespace font
