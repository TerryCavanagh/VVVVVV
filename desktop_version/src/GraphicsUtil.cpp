#include <SDL.h>
#include <stddef.h>
#include <stdlib.h>

#include "Alloc.h"
#include "Constants.h"
#include "Graphics.h"
#include "Maths.h"
#include "Screen.h"
#include "UtilityClass.h"
#include "Vlogging.h"




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

void DrawPixel(SDL_Surface* surface, const int x, const int y, const SDL_Color color)
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

void ApplyFilter(SDL_Surface** src, SDL_Surface** dest)
{
    if (src == NULL || dest == NULL)
    {
        SDL_assert(0 && "NULL src or dest!");
        return;
    }

    if (*src == NULL)
    {
        *src = SDL_CreateRGBSurface(0, SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS, 32, 0, 0, 0, 0);
    }
    if (*dest == NULL)
    {
        *dest = SDL_CreateRGBSurface(0, SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS, 32, 0, 0, 0, 0);
    }
    if (*src == NULL || *dest == NULL)
    {
        WHINE_ONCE_ARGS(("Could not create temporary surfaces: %s", SDL_GetError()));
        return;
    }

    const int result = SDL_RenderReadPixels(gameScreen.m_renderer, NULL, 0, (*src)->pixels, (*src)->pitch);
    if (result != 0)
    {
        SDL_FreeSurface(*src);
        WHINE_ONCE_ARGS(("Could not read pixels from renderer: %s", SDL_GetError()));
        return;
    }

    const int red_offset = rand() % 4;

    for (int x = 0; x < (*src)->w; x++)
    {
        for (int y = 0; y < (*src)->h; y++)
        {
            const int sampley = (y + (int) graphics.lerp(oldscrollamount, scrollamount)) % 240;

            const SDL_Color pixel = ReadPixel(*src, x, sampley);

            Uint8 green = pixel.g;
            Uint8 blue = pixel.b;

            const SDL_Color pixel_offset = ReadPixel(*src, SDL_min(x + red_offset, 319), sampley);
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
            DrawPixel(*dest, x, y, color);
        }
    }

    SDL_UpdateTexture(graphics.gameTexture, NULL, (*dest)->pixels, (*dest)->pitch);
}
