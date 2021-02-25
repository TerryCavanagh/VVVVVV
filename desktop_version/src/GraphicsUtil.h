#ifndef GRAPHICSUTIL_H
#define GRAPHICSUTIL_H

#include <SDL.h>

struct colourTransform
{
    Uint32 colour;
};


void setRect(SDL_Rect& _r, int x, int y, int w, int h);

SDL_Surface* GetSubSurface( SDL_Surface* metaSurface, int x, int y, int width, int height );

Uint32 ReadPixel( SDL_Surface *surface, int x, int y );

SDL_Surface * ScaleSurface( SDL_Surface *Surface, int Width, int Height, SDL_Surface * Dest = NULL );

void BlitSurfaceStandard( SDL_Surface* _src, SDL_Rect* _srcRect, SDL_Surface* _dest, SDL_Rect* _destRect );

void BlitSurfaceColoured( SDL_Surface* _src, SDL_Rect* _srcRect, SDL_Surface* _dest, SDL_Rect* _destRect, colourTransform& ct );

void BlitSurfaceTinted( SDL_Surface* _src, SDL_Rect* _srcRect, SDL_Surface* _dest, SDL_Rect* _destRect, colourTransform& ct );

void FillRect( SDL_Surface* surface, const int x, const int y, const int w, const int h, const int r, int g, int b );

void FillRect( SDL_Surface* surface, const int r, int g, int b );

void FillRect( SDL_Surface* surface, const int color );

void FillRect( SDL_Surface* surface, const int x, const int y, const int w, const int h, int rgba );

void FillRect( SDL_Surface* surface, SDL_Rect& rect, const int r, int g, int b );

void FillRect( SDL_Surface* surface, SDL_Rect rect, int rgba );

void ClearSurface(SDL_Surface* surface);

void ScrollSurface(SDL_Surface* _src, int pX, int py);

SDL_Surface * FlipSurfaceVerticle(SDL_Surface* _src);
void UpdateFilter(void);
SDL_Surface* ApplyFilter( SDL_Surface* _src );

#endif /* GRAPHICSUTIL_H */
