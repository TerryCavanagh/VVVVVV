#include <SDL_stdinc.h>

// Handle third-party dependencies' needs here

void* lodepng_malloc(size_t size)
{
    return SDL_malloc(size);
}

void* lodepng_realloc(void* ptr, size_t new_size)
{
    return SDL_realloc(ptr, new_size);
}

void lodepng_free(void* ptr)
{
    SDL_free(ptr);
}
