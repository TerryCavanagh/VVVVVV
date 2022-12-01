#include <SDL.h>

#include "UtilityClass.h"

extern "C" char* HELP_number_words(int _t)
{
    /* C wrapper for UtilityClass::number_words.
     * Caller must VVV_free. */

    std::string str = help.number_words(_t);

    char* buffer = (char*) SDL_malloc(str.size() + 1);
    str.copy(buffer, str.size());
    buffer[str.size()] = '\0';

    return buffer;
}
