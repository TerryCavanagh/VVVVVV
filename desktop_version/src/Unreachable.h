#ifndef UNREACHABLE_H
#define UNREACHABLE_H

#include <SDL_stdinc.h>

SDL_NORETURN SDL_INLINE void VVV_unreachable(void)
{
    /* __builtin_unreachable() and __assume(0) execute undefined behavior.
     * Otherwise, a noreturn function returning is also undefined behavior. */
#if _SDL_HAS_BUILTIN(__builtin_unreachable)
    __builtin_unreachable();
#elif defined(_MSC_VER)
    __assume(0);
#endif
}

#endif /* UNREACHABLE_H */
