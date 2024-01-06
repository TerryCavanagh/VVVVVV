#define HELP_DEFINITION
#include "UtilityClass.h"

#include <SDL.h>
#include <sstream>

#include "Constants.h"
#include "Localization.h"
#include "Maths.h"
#include "VFormat.h"

int ss_toi(const std::string& str)
{
    int retval = 0;
    bool negative = false;
    static const int radix = 10;

    for (size_t i = 0; i < str.size(); ++i)
    {
        const char chr = str[i];

        if (i == 0 && chr == '-')
        {
            negative = true;
            continue;
        }

        if (SDL_isdigit(chr))
        {
            retval *= radix;
            retval += chr - '0';
        }
        else
        {
            break;
        }
    }

    if (negative)
    {
        return -retval;
    }

    return retval;
}

bool next_split(
    size_t* start,
    size_t* len,
    const char* str,
    const char delim
) {
    size_t idx = 0;
    *len = 0;

    if (str[idx] == '\0')
    {
        return false;
    }

    while (true)
    {
        if (str[idx] == delim)
        {
            *start += 1;
            return true;
        }
        else if (str[idx] == '\0')
        {
            return true;
        }

        idx += 1;
        *start += 1;
        *len += 1;
    }
}

bool next_split_s(
    char buffer[],
    const size_t buffer_size,
    size_t* start,
    const char* str,
    const char delim
) {
    size_t len = 0;
    const size_t prev_start = *start;

    const bool retval = next_split(start, &len, &str[*start], delim);

    if (retval)
    {
        /* Using SDL_strlcpy() here results in calling SDL_strlen() */
        /* on the whole string, which results in a visible freeze */
        /* if it's a very large string */
        const size_t length = SDL_min(buffer_size - 1, len);
        SDL_memcpy(buffer, &str[prev_start], length);
        buffer[length] = '\0';
    }

    return retval;
}

UtilityClass::UtilityClass(void) :
glow(0),
    glowdir(0)
{
    slowsine = 0;
}

std::string UtilityClass::String( int _v )
{
    std::ostringstream os;
    os << _v;
    return(os.str());
}

int UtilityClass::Int(const char* str, int fallback /*= 0*/)
{
    if (!is_number(str))
    {
        return fallback;
    }

    return (int) SDL_strtol(str, NULL, 0);
}

int UtilityClass::hms_to_seconds(int h, int m, int s)
{
    return h*3600 + m*60 + s;
}

void UtilityClass::format_time(char* buffer, const size_t buffer_size, int seconds, int frames, bool always_minutes)
{
    int s = seconds % 60;
    int m = (seconds / 60) % 60;
    int h = seconds / 3600;

    if (h > 0)
    {
        /* H:MM:SS / H:MM:SS.CC */
        vformat_buf(buffer, buffer_size,
            loc::gettext(frames == -1 ? "{hrs}:{min|digits=2}:{sec|digits=2}" : "{hrs}:{min|digits=2}:{sec|digits=2}.{cen|digits=2}"),
            "hrs:int, min:int, sec:int, cen:int",
            h, m, s, frames * 100 / 30
        );
    }
    else if (m > 0 || always_minutes || frames == -1)
    {
        /* M:SS / M:SS.CC */
        vformat_buf(buffer, buffer_size,
            loc::gettext(frames == -1 ? "{min}:{sec|digits=2}" : "{min}:{sec|digits=2}.{cen|digits=2}"),
            "min:int, sec:int, cen:int",
            m, s, frames * 100 / 30
        );
    }
    else
    {
        /* S.CC */
        vformat_buf(buffer, buffer_size,
            loc::gettext("{sec}.{cen|digits=2}"),
            "sec:int, cen:int",
            s, frames * 100 / 30
        );
    }
}

std::string UtilityClass::timestring( int t )
{
    //given a time t in frames, return a time in seconds
    char output[SCREEN_WIDTH_CHARS + 1];
    format_time(output, sizeof(output), t / 30, t % 30, false);
    return output;
}

std::string UtilityClass::number_words(int _t, const char* number_class)
{
    if (loc::lang != "en")
    {
        return loc::getnumber(_t, number_class);
    }

    static const std::string ones_place[] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"};
    static const std::string tens_place[] = {"Ten", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};
    static const std::string teens[] = {"Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};

    if (_t < 0 || _t > 100)
    {
        return String(_t);
    }
    else if (_t == 0)
    {
        return "Zero";
    }
    else if (_t == 100)
    {
        return "One Hundred";
    }
    else if (_t >= 1 && _t <= 9)
    {
        return ones_place[_t-1];
    }
    else if (_t >= 11 && _t <= 19)
    {
        return teens[_t-11];
    }
    else if (_t % 10 == 0)
    {
        return tens_place[(_t/10)-1];
    }
    else
    {
        return tens_place[(_t/10)-1] + " " + ones_place[(_t%10)-1];
    }
}

bool UtilityClass::intersects( SDL_Rect A, SDL_Rect B )
{
    return (SDL_HasIntersection(&A, &B) == SDL_TRUE);
}

void UtilityClass::updateglow(void)
{
    slowsine++;
    if (slowsine >= 64) slowsine = 0;

    if (glowdir == 0) {
        glow+=2;
        if (glow >= 62) glowdir = 1;
    }else {
        glow-=2;
        if (glow < 2) glowdir = 0;
    }
}

bool is_number(const char* str)
{
    if (!SDL_isdigit(str[0]) && str[0] != '-')
    {
        return false;
    }

    if (str[0] == '-' && str[1] == '\0')
    {
        return false;
    }

    for (size_t i = 1; str[i] != '\0'; ++i)
    {
        if (!SDL_isdigit(str[i]))
        {
            return false;
        }
    }

    return true;
}

bool is_positive_num(const char* str, const bool hex)
{
    if (str[0] == '\0')
    {
        return false;
    }

    for (size_t i = 0; str[i] != '\0'; ++i)
    {
        if (hex)
        {
            if (!SDL_isxdigit(str[i]))
            {
                return false;
            }
        }
        else
        {
            if (!SDL_isdigit(str[i]))
            {
                return false;
            }
        }
    }

    return true;
}

bool endsWith(const char* str, const char* suffix)
{
    const size_t str_size = SDL_strlen(str);
    const size_t suffix_size = SDL_strlen(suffix);

    if (str_size < suffix_size)
    {
        return false;
    }

    return SDL_strcmp(&str[str_size - suffix_size], suffix) == 0;
}

void VVV_fillstring(
    char* buffer,
    const size_t buffer_size,
    const char fillchar
) {
    SDL_memset(buffer, fillchar, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
}
void _VVV_between(
    const char* original,
    const size_t left_length,
    char* middle,
    const size_t right_length,
    const size_t middle_size
) {
    size_t middle_length = SDL_strlen(original);
    middle_length -= left_length + right_length;
    SDL_strlcpy(
        middle,
        &original[left_length],
        SDL_min(middle_length + 1, middle_size)
    );
}
