#ifndef LEVELDEBUGGER_H
#define LEVELDEBUGGER_H

#include <SDL.h>

namespace level_debugger
{
    bool is_pausing(void);
    bool is_active(void);
    void input(void);
    void render(void);
}

#endif /* LEVELDEBUGGER_H */
