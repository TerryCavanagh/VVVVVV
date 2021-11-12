#ifndef TEMPSDLFALLTHROUGH_H
#define TEMPSDLFALLTHROUGH_H

/* FIXME: Delete this when SDL 2.0.18 releases! */

#include <SDL_stdinc.h>

#ifndef SDL_FALLTHROUGH
#   ifndef __has_attribute
#      define __has_attribute(x) 0
#   endif

#   if __has_attribute(__fallthrough__)
#      define VVV_fallthrough __attribute__((__fallthrough__))
#   else
#      define VVV_fallthrough do { } while (false) /* fallthrough */
#   endif

#   define SDL_FALLTHROUGH VVV_fallthrough
#endif

#endif /* TEMPSDLFALLTHROUGH_H */
