/*
    SDL_uikit_main.c, placed in the public domain by Sam Lantinga  3/18/2019
*/

/* Include the SDL main definition header */
#include <SDL3/SDL_main.h>

#if defined(SDL_PLATFORM_IOS) || defined(SDL_PLATFORM_TVOS)

#ifndef SDL_MAIN_HANDLED
#ifdef main
#undef main
#endif

int main(int argc, char *argv[])
{
    return SDL_RunApp(argc, argv, SDL_main, NULL);
}
#endif /* !SDL_MAIN_HANDLED */

#endif /* SDL_PLATFORM_IOS || SDL_PLATFORM_TVOS */

/* vi: set ts=4 sw=4 expandtab: */
