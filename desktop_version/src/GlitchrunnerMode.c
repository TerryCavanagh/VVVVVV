#include "GlitchrunnerMode.h"

#include <SDL_assert.h>
#include <SDL_stdinc.h>

#define LOOKUP_TABLE \
    FOREACH_ENUM(GlitchrunnerNone, "") \
    FOREACH_ENUM(Glitchrunner2_0, "2.0") \
    FOREACH_ENUM(Glitchrunner2_2, "2.2") \

const char* GlitchrunnerMode_enum_to_string(const enum GlitchrunnerMode mode)
{
    switch (mode)
    {
#define FOREACH_ENUM(MODE, STRING) \
    case MODE: \
        return STRING;

    LOOKUP_TABLE

#undef FOREACH_ENUM

    /* Compiler raises warning about this enum not being handled. */
    case GlitchrunnerNumVersions:
        break;
    }

    SDL_assert(0 && "Passed non-existent GlitchrunnerMode!");
    return GlitchrunnerMode_enum_to_string(GlitchrunnerNone);
}

enum GlitchrunnerMode GlitchrunnerMode_string_to_enum(const char* string)
{
#define FOREACH_ENUM(MODE, STRING) \
    if (SDL_strcmp(STRING, string) == 0) \
    { \
        return MODE; \
    }

    LOOKUP_TABLE

#undef FOREACH_ENUM

    return GlitchrunnerNone;
}

#undef LOOKUP_TABLE

static enum GlitchrunnerMode current_mode = GlitchrunnerNone;

void GlitchrunnerMode_set(const enum GlitchrunnerMode mode)
{
    current_mode = mode;
}

enum GlitchrunnerMode GlitchrunnerMode_get(void)
{
    return current_mode;
}

int GlitchrunnerMode_less_than_or_equal(const enum GlitchrunnerMode mode)
{
    if (current_mode == GlitchrunnerNone)
    {
        return current_mode == mode;
    }

    return current_mode <= mode;
}
