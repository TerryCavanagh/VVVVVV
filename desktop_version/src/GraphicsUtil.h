#ifndef GRAPHICSUTIL_H
#define GRAPHICSUTIL_H

#include "SDL.h"

struct colourTransform
{
    Uint32 colour;
};


void _setRect(SDL_Rect& _r, int x, int y, int w, int h);
template<class A, class B, class C, class D>
void setRect(SDL_Rect& _r, A x, B y, C w, D h)
{
    _setRect(_r, static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h));
}

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

void _FillRect( SDL_Surface* surface, SDL_Rect& rect, const int r, int g, int b );
template<class R, class G, class B>
void FillRect(SDL_Surface* surface, SDL_Rect& rect, const R r, G g, B b)
{
    _FillRect(surface, rect, static_cast<int>(r), static_cast<int>(g), static_cast<int>(b));
}

void FillRect( SDL_Surface* surface, SDL_Rect rect, int rgba );

bool _intersectRect(float left1, float right1, float bottom1, float top1, float left2, float right2, float bottom2, float top2);
template<class A, class B, class C, class D, class E, class F, class G, class H>
bool intersectRect(A left1, B right1, C bottom1, D top1, E left2, F right2, G bottom2, H top2)
{
    return _intersectRect(static_cast<float>(left1), static_cast<float>(right1), static_cast<float>(bottom1), static_cast<float>(top1), static_cast<float>(left2), static_cast<float>(right2), static_cast<float>(bottom2), static_cast<float>(top2));
}

void OverlaySurfaceKeyed(SDL_Surface* _src, SDL_Surface* _dest, Uint32 _key);

void ScrollSurface(SDL_Surface* _src, int pX, int py);

SDL_Surface * FlipSurfaceHorizontal(SDL_Surface* _src);
SDL_Surface * FlipSurfaceVerticle(SDL_Surface* _src);
SDL_Surface * _ScaleSurfaceSlow( SDL_Surface *_surface, int Width, int Height );
template<class W, class H>
SDL_Surface * ScaleSurfaceSlow( SDL_Surface *_surface, W Width, H Height )
{
    return _ScaleSurfaceSlow(_surface, static_cast<int>(Width), static_cast<int>(Height));
}
SDL_Surface* ApplyFilter( SDL_Surface* _src );

#endif /* GRAPHICSUTIL_H */
