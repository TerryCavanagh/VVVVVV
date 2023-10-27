#ifndef GRAPHICSUTIL_H
#define GRAPHICSUTIL_H

#include <SDL.h>

void setRect(SDL_Rect& _r, int x, int y, int w, int h);

SDL_Surface* GetSubSurface( SDL_Surface* metaSurface, int x, int y, int width, int height );

void DrawPixel(SDL_Surface* surface, int x, int y, SDL_Color color);

SDL_Color ReadPixel(const SDL_Surface* surface, int x, int y);

void UpdateFilter(void);
void ApplyFilter(SDL_Surface** src, SDL_Surface** dest);

#endif /* GRAPHICSUTIL_H */
