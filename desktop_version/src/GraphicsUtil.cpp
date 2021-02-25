#include "Graphics.h"




void setRect( SDL_Rect& _r, int x, int y, int w, int h )
{
    _r.x = x;
    _r.y = y;
    _r.w = w;
    _r.h = h;
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
    SDL_Surface* preSurface = SDL_CreateRGBSurface(
        SDL_SWSURFACE,
        width,
        height,
        metaSurface->format->BitsPerPixel,
        metaSurface->format->Rmask,
        metaSurface->format->Gmask,
        metaSurface->format->Bmask,
        metaSurface->format->Amask
    );

    // Lastly, apply the area from the meta _surface onto the whole of the sub _surface.
    SDL_BlitSurface(metaSurface, &area, preSurface, 0);

    // Return the new Bitmap _surface
    return preSurface;
}

static void DrawPixel( SDL_Surface *_surface, int x, int y, Uint32 pixel )
{
    int bpp = _surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)_surface->pixels + y * _surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

Uint32 ReadPixel( SDL_Surface *_surface, int x, int y )
{
    int bpp = _surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)_surface->pixels + y * _surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

SDL_Surface * ScaleSurface( SDL_Surface *_surface, int Width, int Height, SDL_Surface * Dest )
{
    if(!_surface || !Width || !Height)
        return 0;

    SDL_Surface *_ret;
    if(Dest == NULL)
    {
        _ret = SDL_CreateRGBSurface(_surface->flags, Width, Height, _surface->format->BitsPerPixel,
                                    _surface->format->Rmask, _surface->format->Gmask, _surface->format->Bmask, _surface->format->Amask);
        if(_ret == NULL)
        {
            return NULL;
        }

    }
    else
    {
        _ret = Dest;
    }

    float  _stretch_factor_x = (static_cast<double>(Width)  / static_cast<double>(_surface->w)), _stretch_factor_y = (static_cast<double>(Height) / static_cast<double>(_surface->h));


    SDL_Rect gigantoPixel;
    for(Sint32 y = 0; y < _surface->h; y++)
        for(Sint32 x = 0; x < _surface->w; x++)
        {
            setRect(gigantoPixel, static_cast<Sint32>(float(x)*_stretch_factor_x), static_cast<Sint32>(float(y) *_stretch_factor_y), static_cast<Sint32>(_stretch_factor_x),static_cast<Sint32>( _stretch_factor_y)) ;
            SDL_FillRect(_ret, &gigantoPixel, ReadPixel(_surface, x, y));
        }

    return _ret;
}

SDL_Surface *  FlipSurfaceVerticle(SDL_Surface* _src)
{
    SDL_Surface * ret = SDL_CreateRGBSurface(_src->flags, _src->w, _src->h, _src->format->BitsPerPixel,
        _src->format->Rmask, _src->format->Gmask, _src->format->Bmask, _src->format->Amask);
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

void BlitSurfaceColoured(
    SDL_Surface* _src,
    SDL_Rect* _srcRect,
    SDL_Surface* _dest,
    SDL_Rect* _destRect,
    colourTransform& ct
) {
    SDL_Rect *tempRect = _destRect;

    const SDL_PixelFormat& fmt = *(_src->format);

    SDL_Surface* tempsurface =  SDL_CreateRGBSurface(
        SDL_SWSURFACE,
        _src->w,
        _src->h,
        fmt.BitsPerPixel,
        fmt.Rmask,
        fmt.Gmask,
        fmt.Bmask,
        fmt.Amask
    );

    for(int x = 0; x < tempsurface->w; x++)
    {
        for(int y = 0; y < tempsurface->h; y++)
        {
            Uint32 pixel = ReadPixel(_src, x, y);
            Uint32 Alpha = pixel & fmt.Amask;
            Uint32 result = ct.colour & 0x00FFFFFF;
            Uint32 CTAlpha = ct.colour & fmt.Amask;
            float div1 = ((Alpha >> 24) / 255.0f);
            float div2 = ((CTAlpha >> 24) / 255.0f);
            Uint32 UseAlpha = (div1 * div2) * 255.0f;
            DrawPixel(tempsurface, x, y, result | (UseAlpha << 24));
        }
    }

    SDL_BlitSurface(tempsurface, _srcRect, _dest, tempRect);
    SDL_FreeSurface(tempsurface);
}

void BlitSurfaceTinted(
    SDL_Surface* _src,
    SDL_Rect* _srcRect,
    SDL_Surface* _dest,
    SDL_Rect* _destRect,
    colourTransform& ct
) {
    SDL_Rect *tempRect = _destRect;

    const SDL_PixelFormat& fmt = *(_src->format);

    SDL_Surface* tempsurface =  SDL_CreateRGBSurface(
        SDL_SWSURFACE,
        _src->w,
        _src->h,
        fmt.BitsPerPixel,
        fmt.Rmask,
        fmt.Gmask,
        fmt.Bmask,
        fmt.Amask
    );

    for (int x = 0; x < tempsurface->w; x++) {
        for (int y = 0; y < tempsurface->h; y++) {
            Uint32 pixel = ReadPixel(_src, x, y);

            Uint8 pixred = (pixel & _src->format->Rmask) >> 16;
            Uint8 pixgreen = (pixel & _src->format->Gmask) >> 8;
            Uint8 pixblue = (pixel & _src->format->Bmask) >> 0;

            double temp_pixred = pixred * 0.299;
            double temp_pixgreen = pixgreen * 0.587;
            double temp_pixblue = pixblue * 0.114;

            double gray = floor((temp_pixred + temp_pixgreen + temp_pixblue + 0.5));

            Uint8 ctred = (ct.colour & graphics.backBuffer->format->Rmask) >> 16;
            Uint8 ctgreen = (ct.colour & graphics.backBuffer->format->Gmask) >> 8;
            Uint8 ctblue = (ct.colour & graphics.backBuffer->format->Bmask) >> 0;

            temp_pixred = gray * ctred / 255.0;
            temp_pixgreen = gray * ctgreen / 255.0;
            temp_pixblue = gray * ctblue / 255.0;

            if (temp_pixred > 255)
                temp_pixred = 255;
            if (temp_pixgreen > 255)
                temp_pixgreen = 255;
            if (temp_pixblue > 255)
                temp_pixblue = 255;

            pixred = temp_pixred;
            pixgreen = temp_pixgreen;
            pixblue = temp_pixblue;

            Uint32 Alpha = pixel & fmt.Amask;
            Uint32 result = (pixred << 16) + (pixgreen << 8) + (pixblue << 0);
            Uint32 CTAlpha = ct.colour & fmt.Amask;
            float div1 = ((Alpha >> 24) / 255.0f);
            float div2 = ((CTAlpha >> 24) / 255.0f);
            Uint32 UseAlpha = (div1 * div2) * 255.0f;

            DrawPixel(tempsurface, x, y, result | (UseAlpha << 24));
        }
    }

    SDL_BlitSurface(tempsurface, _srcRect, _dest, tempRect);
    SDL_FreeSurface(tempsurface);
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

SDL_Surface* ApplyFilter( SDL_Surface* _src )
{
    SDL_Surface* _ret = SDL_CreateRGBSurface(_src->flags, _src->w, _src->h, _src->format->BitsPerPixel,
        _src->format->Rmask, _src->format->Gmask, _src->format->Bmask, _src->format->Amask);

    int redOffset = rand() % 4;

    for(int x = 0; x < _src->w; x++)
    {
        for(int y = 0; y < _src->h; y++)
        {
            int sampley = (y + (int) graphics.lerp(oldscrollamount, scrollamount) )% 240;

            Uint32 pixel = ReadPixel(_src, x,sampley);

            Uint8 green = (pixel & _src->format->Gmask) >> 8;
            Uint8 blue = (pixel & _src->format->Bmask) >> 0;

            Uint32 pixelOffset = ReadPixel(_src, VVV_min(x+redOffset, 319), sampley) ;
            Uint8 red = (pixelOffset & _src->format->Rmask) >> 16 ;

            if(isscrolling && sampley > 220 && ((rand() %10) < 4))
            {
                red = VVV_min(int(red+(fRandom() * 0.6)  * 254) , 255);
                green = VVV_min(int(green+(fRandom() * 0.6)  * 254) , 255);
                blue = VVV_min(int(blue+(fRandom() * 0.6)  * 254) , 255);
            }
            else
            {
                red = VVV_min(int(red+(fRandom() * 0.2)  * 254) , 255);
                green = VVV_min(int(green+(fRandom() * 0.2)  * 254) , 255);
                blue = VVV_min(int(blue+(fRandom() * 0.2)  * 254) , 255);
            }


            if(y % 2 == 0)
            {
                red = static_cast<Uint8>(red / 1.2f);
                green = static_cast<Uint8>(green / 1.2f);
                blue =  static_cast<Uint8>(blue / 1.2f);
            }

            int distX =  static_cast<int>((SDL_abs (160.0f -x ) / 160.0f) *16);
            int distY =  static_cast<int>((SDL_abs (120.0f -y ) / 120.0f)*32);

            red = VVV_max(red - ( distX +distY), 0);
            green = VVV_max(green - ( distX +distY), 0);
            blue = VVV_max(blue - ( distX +distY), 0);

            Uint32 finalPixel = ((red<<16) + (green<<8) + (blue<<0)) | (pixel &_src->format->Amask);
            DrawPixel(_ret,x,y,  finalPixel);

        }
    }
    return _ret;
}

void FillRect( SDL_Surface* _surface, const int _x, const int _y, const int _w, const int _h, const int r, int g, int b )
{
    SDL_Rect rect = {Sint16(_x),Sint16(_y),Sint16(_w),Sint16(_h)};
    Uint32 color;
    color = SDL_MapRGB(_surface->format, r, g, b);
    SDL_FillRect(_surface, &rect, color);
}

void FillRect( SDL_Surface* _surface, const int r, int g, int b )
{
    Uint32 color = SDL_MapRGB(_surface->format, r, g, b);
    SDL_FillRect(_surface, NULL, color);
}

void FillRect( SDL_Surface* _surface, const int color )
{
    SDL_FillRect(_surface, NULL, color);
}

void FillRect( SDL_Surface* _surface, const int x, const int y, const int w, const int h, int rgba )
{
    SDL_Rect rect = {Sint16(x)  ,Sint16(y) ,Sint16(w) ,Sint16(h) };
    SDL_FillRect(_surface, &rect, rgba);
}

void FillRect( SDL_Surface* _surface, SDL_Rect& _rect, const int r, int g, int b )
{
    Uint32 color;
    color = SDL_MapRGB(_surface->format, r, g, b);
    SDL_FillRect(_surface, &_rect, color);
}

void FillRect( SDL_Surface* _surface, SDL_Rect rect, int rgba )
{
    SDL_FillRect(_surface, &rect, rgba);
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
        SDL_FreeSurface(part1);
    }

}
