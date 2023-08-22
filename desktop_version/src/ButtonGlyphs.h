#ifndef BUTTONGLYPHS_H
#define BUTTONGLYPHS_H

#include <SDL.h>
#include <stdbool.h>

#include "ActionSets.h"

#ifdef __cplusplus
extern "C"
{
#endif

void BUTTONGLYPHS_init(void);

bool BUTTONGLYPHS_keyboard_is_available(void);
bool BUTTONGLYPHS_keyboard_is_active(void);
void BUTTONGLYPHS_keyboard_set_active(bool active);

void BUTTONGLYPHS_update_layout(Uint16 vendor, Uint16 product);

const char* BUTTONGLYPHS_get_wasd_text(void);
const char* BUTTONGLYPHS_get_button(ActionSet actionset, Action action, int binding);

char* BUTTONGLYPHS_get_all_gamepad_buttons(
    char* buffer,
    size_t buffer_len,
    ActionSet actionset,
    int action
);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // BUTTONGLYPHS_H
