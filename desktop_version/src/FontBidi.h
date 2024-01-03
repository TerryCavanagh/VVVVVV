#ifndef FONTBIDI_H
#define FONTBIDI_H

#include <stdint.h>

namespace font
{

void bidi_init(void);
void bidi_destroy(void);
bool is_directional_character(uint32_t codepoint);
bool is_joiner(uint32_t codepoint);
bool bidi_should_transform(bool rtl, const char* text);
const char* bidi_transform(bool rtl, const char* text);

} // namespace font

#endif // FONTBIDI_H
