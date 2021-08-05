#ifndef GLITCHRUNNERMODE_H
#define GLITCHRUNNERMODE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Have fun speedrunners! <3 Misa */

/* When a version is added, update the lookup table in GlitchrunnerMode.c */

enum GlitchrunnerMode
{
    GlitchrunnerNone,
    Glitchrunner2_0,
    Glitchrunner2_2, /* 2.1 is same as 2.2 */
    GlitchrunnerNumVersions
};

const char* GlitchrunnerMode_enum_to_string(enum GlitchrunnerMode mode);

enum GlitchrunnerMode GlitchrunnerMode_string_to_enum(const char* string);

void GlitchrunnerMode_set(enum GlitchrunnerMode mode);

enum GlitchrunnerMode GlitchrunnerMode_get(void);

int GlitchrunnerMode_less_than_or_equal(enum GlitchrunnerMode mode);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GLITCHRUNNERMODE_H */
