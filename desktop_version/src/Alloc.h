#ifndef ALLOCGAME_H
#define ALLOCGAME_H

/* TODO: VVV_malloc, VVV_realloc, etc. */

#define VVV_freefunc(func, obj) \
    do \
    { \
        if (obj != NULL) \
        { \
            func(obj); \
            obj = NULL; \
        } \
    } \
    while (0)

#define VVV_free(obj) VVV_freefunc(SDL_free, obj)

#endif /* ALLOCGAME_H */
