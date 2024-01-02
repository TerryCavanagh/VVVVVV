#ifndef FONTBIDI_H
#define FONTBIDI_H

namespace font
{

void bidi_init(void);
void bidi_destroy(void);
bool bidi_should_transform(const char* text);
const char* bidi_transform(const char* text);

} // namespace font

#endif // FONTBIDI_H
