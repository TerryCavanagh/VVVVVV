#include "FontBidi.h"

#include <SDL.h>

#include "Alloc.h"
#include "UTF8.h"

extern "C"
{
#include <SheenBidi.h>
}

namespace font
{

bool bidi_should_transform(const char* text)
{
    // TODO
    return true;
}

const char* bidi_transform(const char* text)
{
    uint32_t utf32_in[1024];
    int n_codepoints = 0;

    const char* text_ptr = text;
    while ((utf32_in[n_codepoints] = UTF8_next(&text_ptr)))
    {
        n_codepoints++;

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
    SBParagraphRef paragraph = SBAlgorithmCreateParagraph(algorithm, 0, INT32_MAX, SBLevelDefaultRTL);
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

            int out_room_left = sizeof(utf8_out) - 1 - utf8_out_cur;
            if (out_room_left <= 0)
            {
                goto no_more_runs;
            }

            // TODO prolly do something with reshaping here

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
