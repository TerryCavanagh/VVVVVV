#ifndef GRAPHICSUTIL_H
#define GRAPHICSUTIL_H

#include "SDL.h"

struct colourTransform
{
    Uint32 colour;
};


void setRect(SDL_Rect& _r, int x, int y, int w, int h);

unsigned int endian_swap(unsigned int x);

SDL_Surface* GetSubSurface( SDL_Surface* metaSurface, int x, int y, int width, int height );

void DrawPixel( SDL_Surface *surface, int x, int y, Uint32 pixel );

Uint32 ReadPixel( SDL_Surface *surface, int x, int y );

SDL_Surface * ScaleSurface( SDL_Surface *Surface, int Width, int Height, SDL_Surface * Dest = NULL );

void BlitSurfaceStandard( SDL_Surface* _src, SDL_Rect* _srcRect, SDL_Surface* _dest, SDL_Rect* _destRect );

void BlitSurfaceColoured( SDL_Surface* _src, SDL_Rect* _srcRect, SDL_Surface* _dest, SDL_Rect* _destRect, colourTransform& ct );

void FillRect( SDL_Surface* surface, const int x, const int y, const int w, const int h, const int r, int g, int b );

void FillRect( SDL_Surface* surface, const int r, int g, int b );

void FillRect( SDL_Surface* surface, const int color );

void FillRect( SDL_Surface* surface, const int x, const int y, const int w, const int h, int rgba );

void FillRect( SDL_Surface* surface, SDL_Rect& rect, const int r, int g, int b );

void FillRect( SDL_Surface* surface, SDL_Rect rect, int rgba );

bool intersectRect(float left1, float right1, float bottom1, float top1, float left2, float right2, float bottom2, float top2);

void OverlaySurfaceKeyed(SDL_Surface* _src, SDL_Surface* _dest, Uint32 _key);

void ScrollSurface(SDL_Surface* _src, int pX, int py);

SDL_Surface * FlipSurfaceHorizontal(SDL_Surface* _src);
SDL_Surface * FlipSurfaceVerticle(SDL_Surface* _src);
SDL_Surface * ScaleSurfaceSlow( SDL_Surface *_surface, int Width, int Height );
void UpdateFilter();
SDL_Surface* ApplyFilter( SDL_Surface* _src );

#endif /* GRAPHICSUTIL_H */
