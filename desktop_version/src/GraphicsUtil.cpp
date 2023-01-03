#include <SDL.h>
#include <stddef.h>
#include <stdlib.h>

#include "Alloc.h"
#include "Graphics.h"
#include "Maths.h"




void setRect( SDL_Rect& _r, int x, int y, int w, int h )
{
    _r.x = x;
    _r.y = y;
    _r.w = w;
    _r.h = h;
}

static SDL_Surface* RecreateSurfaceWithDimensions(
    SDL_Surface* surface,
    const int width,
    const int height
) {
    SDL_Surface* retval;
    SDL_BlendMode blend_mode;

    if (surface == NULL)
    {
        return NULL;
    }

    retval = SDL_CreateRGBSurface(
        surface->flags,
        width,
        height,
        surface->format->BitsPerPixel,
        surface->format->Rmask,
        surface->format->Gmask,
        surface->format->Bmask,
        surface->format->Amask
    );

    if (retval == NULL)
    {
        return NULL;
    }

    SDL_GetSurfaceBlendMode(surface, &blend_mode);
    SDL_SetSurfaceBlendMode(retval, blend_mode);

    return retval;
}

static SDL_Surface* RecreateSurface(SDL_Surface* surface)
{
    if (surface == NULL)
    {
        return NULL;
    }

    return RecreateSurfaceWithDimensions(
        surface,
        surface->w,
        surface->h
    );
}

SDL_Surface* GetSubSurface( SDL_Surface* metaSurface, int x, int y, int width, int height )
{
    // Create an SDL_Rect with the area of the _surface
    SDL_Rect area;
    area.x = x;
    area.y = y;
    area.w = width;
    area.h = height;

    //Convert to the correct display format after nabbing the new _surface or we will slow things down.
    SDL_Surface* preSurface = RecreateSurfaceWithDimensions(
        metaSurface,
        width,
        height
    );

    // Lastly, apply the area from the meta _surface onto the whole of the sub _surface.
    SDL_BlitSurface(metaSurface, &area, preSurface, 0);

    // Return the new Bitmap _surface
    return preSurface;
}

static void DrawPixel(SDL_Surface* surface, const int x, const int y, const SDL_Color color)
{
    const SDL_PixelFormat* fmt = surface->format;
    const int bpp = fmt->BytesPerPixel;
    Uint32* pixel = (Uint32*) ((Uint8*) surface->pixels + y * surface->pitch + x * bpp);

    switch (bpp)
    {
    case 1:
    case 2:
    case 3:
        SDL_assert(0 && "Non-32-bit colors not supported!");
        return;

    case 4:
        *pixel = SDL_MapRGBA(fmt, color.r, color.g, color.b, color.a);
    }
}

SDL_Color ReadPixel(const SDL_Surface* surface, const int x, const int y)
{
    const SDL_PixelFormat* fmt = surface->format;
    const int bpp = surface->format->BytesPerPixel;
    const Uint32* pixel = (Uint32*) ((Uint8*) surface->pixels + y * surface->pitch + x * bpp);
    SDL_Color color = {0, 0, 0, 0};

    switch (bpp)
    {
    case 1:
    case 2:
    case 3:
        SDL_assert(0 && "Non-32-bit colors not supported!");
        break;

    case 4:
        SDL_GetRGBA(*pixel, fmt, &color.r, &color.g, &color.b, &color.a);
    }

    return color;
}

SDL_Surface * ScaleSurface( SDL_Surface *_surface, int Width, int Height, SDL_Surface * Dest )
{
    if(!_surface || !Width || !Height)
        return 0;

    SDL_Surface *_ret;
    if(Dest == NULL)
    {
        _ret = RecreateSurfaceWithDimensions(_surface, Width, Height);
        if(_ret == NULL)
        {
            return NULL;
        }

    }
    else
    {
        _ret = Dest;
    }

    SDL_BlitScaled(_surface, NULL, _ret, NULL);

    return _ret;
}

SDL_Surface *  FlipSurfaceVerticle(SDL_Surface* _src)
{
    SDL_Surface * ret = RecreateSurface(_src);
    if(ret == NULL)
    {
        return NULL;
    }

    for(Sint32 y = 0; y < _src->h; y++)
    {
        for(Sint32 x = 0; x < _src->w; x++)
        {
            DrawPixel(ret, x ,(_src->h-1) - y ,ReadPixel(_src, x, y));
        }


    }

    return ret;
}

void BlitSurfaceStandard( SDL_Surface* _src, SDL_Rect* _srcRect, SDL_Surface* _dest, SDL_Rect* _destRect )
{
    SDL_BlitSurface( _src, _srcRect, _dest, _destRect );
}

static void BlitSurfaceTransform(
    SDL_Surface* src,
    const SDL_Rect* src_rect,
    SDL_Surface* dest,
    SDL_Rect* dest_rect,
    SDL_Color (*transform)(SDL_Color pixel, SDL_Color color),
    const SDL_Color color
) {
    if (src == NULL || dest == NULL || transform == NULL)
    {
        return;
    }
    if (color.a == 0)
    {
        return;
    }

    SDL_Rect orig_rect;
    if (src_rect == NULL)
    {
        setRect(orig_rect, 0, 0, src->w, src->h);
    }
    else
    {
        orig_rect = *src_rect;
    }
    int blit_x;
    int blit_y;
    if (dest_rect == NULL)
    {
        blit_x = 0;
        blit_y = 0;
    }
    else
    {
        blit_x = dest_rect->x;
        blit_y = dest_rect->y;
    }

    for (int x = 0; x < orig_rect.w; x++)
    {
        for (int y = 0; y < orig_rect.h; y++)
        {
            if (blit_x + x < 0 || blit_y + y < 0 ||
            blit_x + x >= dest->w || blit_y + y >= dest->h)
            {
                continue;
            }

            const SDL_Color pixel = ReadPixel(src, orig_rect.x + x, orig_rect.y + y);
            if (pixel.a == 0)
            {
                continue;
            }

            const SDL_Color result = transform(pixel, color);
            DrawPixel(dest, blit_x + x, blit_y + y, result);
        }
    }
}

static SDL_Color transform_color(const SDL_Color pixel, const SDL_Color color)
{
    const float div1 = pixel.a / 255.0f;
    const float div2 = color.a / 255.0f;
    const Uint8 alpha = (div1 * div2) * 255.0f;
    const SDL_Color result = {color.r, color.g, color.b, alpha};
    return result;
}

void BlitSurfaceColoured(
    SDL_Surface* src,
    const SDL_Rect* src_rect,
    SDL_Surface* dest,
    SDL_Rect* dest_rect,
    const SDL_Color color
) {
    return BlitSurfaceTransform(
        src, src_rect, dest, dest_rect, transform_color, color
    );
}

static SDL_Color transform_tint(const SDL_Color pixel, const SDL_Color color)
{
    double red = pixel.r * 0.299;
    double green = pixel.g * 0.587;
    double blue = pixel.b * 0.114;

    const double gray = SDL_floor(red + green + blue + 0.5);

    red = gray * color.r / 255.0;
    green = gray * color.g / 255.0;
    blue = gray * color.b / 255.0;

    red = SDL_clamp(red, 0, 255);
    green = SDL_clamp(green, 0, 255);
    blue = SDL_clamp(blue, 0, 255);

    const float div1 = pixel.a / 255.0f;
    const float div2 = color.a / 255.0f;
    const Uint8 alpha = (div1 * div2) * 255.0f;

    const SDL_Color result = {(Uint8) red, (Uint8) green, (Uint8) blue, alpha};
    return result;
}

void BlitSurfaceTinted(
    SDL_Surface* src,
    const SDL_Rect* src_rect,
    SDL_Surface* dest,
    SDL_Rect* dest_rect,
    const SDL_Color color
) {
    return BlitSurfaceTransform(
        src, src_rect, dest, dest_rect, transform_tint, color
    );
}


static int oldscrollamount = 0;
static int scrollamount = 0;
static bool isscrolling = 0;

void UpdateFilter(void)
{
    if (rand() % 4000 < 8)
    {
        isscrolling = true;
    }

    oldscrollamount = scrollamount;
    if(isscrolling == true)
    {
        scrollamount += 20;
        if(scrollamount > 240)
        {
            scrollamount = 0;
            oldscrollamount = 0;
            isscrolling = false;
        }
    }
}

SDL_Surface* ApplyFilter(SDL_Surface* src)
{
    SDL_Surface* ret = RecreateSurface(src);

    const int red_offset = rand() % 4;

    for (int x = 0; x < ret->w; x++)
    {
        for (int y = 0; y < ret->h; y++)
        {
            const int sampley = (y + (int) graphics.lerp(oldscrollamount, scrollamount)) % 240;

            const SDL_Color pixel = ReadPixel(src, x, sampley);

            Uint8 green = pixel.g;
            Uint8 blue = pixel.b;

            const SDL_Color pixel_offset = ReadPixel(src, SDL_min(x + red_offset, 319), sampley);
            Uint8 red = pixel_offset.r;

            double mult;
            int tmp; /* needed to avoid char overflow */
            if (isscrolling && sampley > 220 && ((rand() % 10) < 4))
            {
                mult = 0.6;
            }
            else
            {
                mult = 0.2;
            }

            tmp = red + fRandom() * mult * 254;
            red = SDL_min(tmp, 255);
            tmp = green + fRandom() * mult * 254;
            green = SDL_min(tmp, 255);
            tmp = blue + fRandom() * mult * 254;
            blue = SDL_min(tmp, 255);

            if (y % 2 == 0)
            {
                red = (Uint8) (red / 1.2f);
                green = (Uint8) (green / 1.2f);
                blue = (Uint8) (blue / 1.2f);
            }

            int distX = (int) ((SDL_abs(160.0f - x) / 160.0f) * 16);
            int distY = (int) ((SDL_abs(120.0f - y) / 120.0f) * 32);

            red = SDL_max(red - (distX + distY), 0);
            green = SDL_max(green - (distX + distY), 0);
            blue = SDL_max(blue - (distX + distY), 0);

            const SDL_Color color = {red, green, blue, pixel.a};
            DrawPixel(ret, x, y, color);
        }
    }

    return ret;
}

void FillRect( SDL_Surface* _surface, const int _x, const int _y, const int _w, const int _h, const int r, int g, int b )
{
    SDL_Rect rect = {_x, _y, _w, _h};
    Uint32 color = SDL_MapRGB(_surface->format, r, g, b);
    SDL_FillRect(_surface, &rect, color);
}

void FillRect( SDL_Surface* _surface, const int r, int g, int b )
{
    Uint32 color = SDL_MapRGB(_surface->format, r, g, b);
    SDL_FillRect(_surface, NULL, color);
}

void FillRect( SDL_Surface* _surface, SDL_Rect& _rect, const int r, int g, int b )
{
    Uint32 color = SDL_MapRGB(_surface->format, r, g, b);
    SDL_FillRect(_surface, &_rect, color);
}

void FillRect(SDL_Surface* surface, const SDL_Rect rect, const SDL_Color color)
{
    const Uint32 mapped = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
    SDL_FillRect(surface, &rect, mapped);
}

void FillRect(SDL_Surface* surface, const SDL_Color color)
{
    const Uint32 mapped = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
    SDL_FillRect(surface, NULL, mapped);
}

void FillRect(SDL_Surface* surface, const int x, const int y, const int w, const int h, const SDL_Color color)
{
    const SDL_Rect rect = {x, y, w, h};
    const Uint32 mapped = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
    SDL_FillRect(surface, &rect, mapped);
}

void FillRect(SDL_Surface* surface, const int r, const int g, const int b, const int a)
{
    const Uint32 mapped = SDL_MapRGBA(surface->format, r, g, b, a);
    SDL_FillRect(surface, NULL, mapped);
}

void ClearSurface(SDL_Surface* surface)
{
    SDL_FillRect(surface, NULL, 0x00000000);
}

void ScrollSurface( SDL_Surface* _src, int _pX, int _pY )
{
    SDL_Surface* part1 = NULL;

    SDL_Rect rect1;
    SDL_Rect rect2;
    //scrolling up;
    if(_pY < 0)
    {
        setRect(rect2, 0, 0, _src->w,  _src->h - _pY);

        part1 = GetSubSurface(_src, rect2.x, rect2.y, rect2.w, rect2.h);

        SDL_Rect destrect1;

        SDL_SetSurfaceBlendMode(part1, SDL_BLENDMODE_NONE);

        setRect(destrect1, 0,  _pY, _pX, _src->h);

        SDL_BlitSurface (part1, NULL, _src, &destrect1);
    }

    else if(_pY > 0)
    {

        setRect(rect1, 0, 0, _src->w, _src->h - _pY);

        part1 = GetSubSurface(_src, rect1.x, rect1.y, rect1.w, rect1.h);

        SDL_Rect destrect1;

        SDL_SetSurfaceBlendMode(part1, SDL_BLENDMODE_NONE);

        setRect(destrect1, _pX, _pY, _src->w, _src->h - _pY);

        SDL_BlitSurface (part1, NULL, _src, &destrect1);

    }

    //Right
    else if(_pX <= 0)
    {
        setRect(rect2, 0, 0, _src->w - _pX,  _src->h );

        part1 = GetSubSurface(_src, rect2.x, rect2.y, rect2.w, rect2.h);

        SDL_Rect destrect1;

        SDL_SetSurfaceBlendMode(part1, SDL_BLENDMODE_NONE);

        setRect(destrect1, _pX,  0, _src->w - _pX, _src->h);

        SDL_BlitSurface (part1, NULL, _src, &destrect1);
    }

    else if(_pX > 0)
    {

        setRect(rect1, _pX, 0, _src->w - _pX, _src->h );

        part1 = GetSubSurface(_src, rect1.x, rect1.y, rect1.w, rect1.h);

        SDL_Rect destrect1;

        SDL_SetSurfaceBlendMode(part1, SDL_BLENDMODE_NONE);

        setRect(destrect1, 0, 0, _src->w - _pX, _src->h);

        SDL_BlitSurface (part1, NULL, _src, &destrect1);

    }
    //Cleanup temp surface
    if (part1)
    {
        VVV_freefunc(SDL_FreeSurface, part1);
    }

}
