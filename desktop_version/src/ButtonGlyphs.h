#ifndef BUTTONGLYPHS_H
#define BUTTONGLYPHS_H

#include <stdbool.h>

#include "ActionSets.h"

#ifdef __cplusplus
extern "C"
{
#endif

void BUTTONGLYPHS_init(void);

bool BUTTONGLYPHS_keyboard_is_available(void);
bool BUTTONGLYPHS_keyboard_is_active(void);
const char* BUTTONGLYPHS_get_wasd_text(void);
const char* BUTTONGLYPHS_get_button(ActionSet actionset, Action action);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // BUTTONGLYPHS_H
