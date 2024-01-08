#ifndef CWRAPPERS_H
#define CWRAPPERS_H

#include <SDL_surface.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char* HELP_number_words(int _t, const char* number_class);
uint32_t LOC_toupper_ch(uint32_t ch);
SDL_Surface* GRAPHICS_tempScreenshot(void);
SDL_Surface* GRAPHICS_tempScreenshot2x(void);
uint8_t UTIL_TakeScreenshot(SDL_Surface** surface);
uint8_t UTIL_UpscaleScreenshot2x(SDL_Surface* src, SDL_Surface** dest);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CWRAPPERS_H */
