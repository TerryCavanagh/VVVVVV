#include "Graphics.h"
#include "Maths.h"
#include "Entity.h"
#include "Map.h"
#include "Screen.h"
#include "FileSystemUtils.h"
#include <utf8/unchecked.h>
#include <stdio.h>
#include <stdlib.h>

void Graphics::init()
{
    grphx.init();

    flipmode = false;
    setRect(tiles_rect, 0,0,8,8);
    setRect(sprites_rect, 0,0,32,32);
    setRect(bfont_rect, 0,0,8,8);
    setRect(bfontmask_rect, 0,0,8,8);
    setRect(bg_rect, 0,0,320,240);
    setRect(footerrect, 0, 230, 320, 10);
    setRect(prect, 0, 0, 4, 4);
    setRect(line_rect, 0,0,0,0);
    setRect(tele_rect,0,0,96,96);


    //We initialise a few things

    linestate = 0;


    trinketcolset = false;

    showcutscenebars = false;
    cutscenebarspos = 0;
    notextoutline = false;

    flipmode = false;
    setflipmode = false;

    //Background inits
    for (int i = 0; i < 50; i++)
    {
        SDL_Rect s = {Sint16(fRandom() * 320), Sint16(fRandom() * 240), 2, 2};
        int s2 = 4+(fRandom()*4);
        stars.push_back(s);
        starsspeed.push_back(s2);

        SDL_Rect bb;
        int bvx = 0;
        int bvy = 0;
        if(fRandom()*100 > 50)
        {
            bvx = 9 - (fRandom() * 19);
            if (bvx > -6 && bvx < 6) bvx = 6;
            bvx = bvx * 1.5;
            setRect(bb, fRandom() * 320, fRandom() * 240, 32, 12);
        }
        else
        {
            bvy = 9 - (fRandom() * 19);
            if (bvy > -6 && bvy < 6) bvy = 6;
            bvy = bvy * 1.5;
            setRect(bb, fRandom() * 320, fRandom() * 240, 12, 32) ;
        }
        float bint = 0.5 + ((fRandom() * 100) / 200);
        backboxes.push_back(bb);
        backboxvx.push_back(bvx);
        backboxvy.push_back(bvy);
        backboxint.push_back(bint);
    }
    backoffset = 0;
    backgrounddrawn = false;

    warpskip = 0;
    warpfcol = 0x000000;
    warpbcol = 0x000000;

    spcol = 0;
    spcoldel = 0;
    rcol = 0;

    crewframe = 0;
    crewframedelay = 4;
    menuoffset = 0;
    resumegamemode = false;

    //Fading stuff
    fadebars.resize(15);

    fadeamount = 0;
    fademode = 0;

    // initialize everything else to zero
    backBuffer = NULL;
    backboxrect = SDL_Rect();
    bcol = 0;
    bcol2 = 0;
    ct = colourTransform();
    foot_rect = SDL_Rect();
    foregrounddrawn = false;
    foregroundBuffer = NULL;
    backgrounddrawn = false;
    images_rect = SDL_Rect();
    j = 0;
    k = 0;
    m = 0;
    linedelay = 0;
    menubuffer = NULL;
    screenbuffer = NULL;
    tempBuffer = NULL;
    tl = point();
    towerbuffer = NULL;
    trinketr = 0;
    trinketg = 0;
    trinketb = 0;
    warprect = SDL_Rect();

    translucentroomname = false;
    showmousecursor = true;
}

int Graphics::font_idx(uint32_t ch) {
    if (font_positions.size() > 0) {
        std::map<int, int>::iterator iter = font_positions.find(ch);
        if (iter == font_positions.end()) {
            iter = font_positions.find('?');
            if (iter == font_positions.end()) {
                puts("font.txt missing fallback character!");
                exit(1);
            }
        }
        return iter->second;
    } else {
        return ch;
    }
}

Graphics::~Graphics()
{

}

void Graphics::drawspritesetcol(int x, int y, int t, int c)
{
    SDL_Rect rect;
    setRect(rect,x,y,sprites_rect.w,sprites_rect.h);
    setcol(c);

    BlitSurfaceColoured(sprites[t],NULL,backBuffer, &rect, ct);
}

void Graphics::Makebfont()
{
    for (int j =  0; j < (grphx.im_bfont->h / 8); j++)
    {
        for (int i = 0; i < 16; i++)
        {

            SDL_Surface* temp = GetSubSurface(grphx.im_bfont,i*8,j*8,8,8);
            bfont.push_back(temp);

            SDL_Surface* TempFlipped = FlipSurfaceVerticle(temp);
            flipbfont.push_back(TempFlipped);
        }
    }

    unsigned char* charmap = NULL;
    size_t length;
    FILESYSTEM_loadFileToMemory("graphics/font.txt", &charmap, &length);
    if (charmap != NULL) {
        unsigned char* current = charmap;
        unsigned char* end = charmap + length;
        int pos = 0;
        while (current != end) {
            int codepoint = utf8::unchecked::next(current);
            font_positions[codepoint] = pos;
            ++pos;
        }
        FILESYSTEM_freeMemory(&charmap);
    }
}

int Graphics::bfontlen(uint32_t ch) {
    if (ch < 32) {
        return 6;
    } else {
        return 8;
    }
}

void Graphics::MakeTileArray()
{
    for(int j = 0; j <30; j++)
    {
        for(int i = 0; i <40; i++)
        {
            SDL_Surface* temp = GetSubSurface(grphx.im_tiles,i*8,j*8,8,8);
            tiles.push_back(temp);
        }
    }
    for(int j = 0; j <30; j++)
    {
        for(int i = 0; i <40; i++)
        {
            SDL_Surface* temp = GetSubSurface(grphx.im_tiles2,i*8,j*8,8,8);
            tiles2.push_back(temp);
        }
    }

    for(int j = 0; j <30; j++)
    {
        for(int i = 0; i <30; i++)
        {
            SDL_Surface* temp = GetSubSurface(grphx.im_tiles3,i*8,j*8,8,8);
            tiles3.push_back(temp);
        }
    }

    for(int j = 0; j <60; j++)
    {
        for(int i = 0; i <12; i++)
        {
            SDL_Surface* temp = GetSubSurface(grphx.im_entcolours,i*8,j*8,8,8);
            entcolours.push_back(temp);
        }
    }
}

void Graphics::maketelearray()
{
    for (int i = 0; i < 10; i++)
    {
        SDL_Surface* temp = GetSubSurface(grphx.im_teleporter,i*96,0,96,96);
        tele.push_back(temp);
    }
}

void Graphics::MakeSpriteArray()
{
    for(int j = 0; j <16; j++)
    {
        for(int i = 0; i <12; i++)
        {
            SDL_Surface* temp = GetSubSurface(grphx.im_sprites,i*32,j*32,32,32);
            sprites.push_back(temp);
            temp = GetSubSurface(grphx.im_flipsprites,i*32,j*32,32,32);
            flipsprites.push_back(temp);
        }
    }
}


void Graphics::Print( int _x, int _y, std::string _s, int r, int g, int b, bool cen /*= false*/ ) {
    return PrintAlpha(_x,_y,_s,r,g,b,255,cen);
}

void Graphics::PrintAlpha( int _x, int _y, std::string _s, int r, int g, int b, int a, bool cen /*= false*/ )
{
    r = clamp(r,0,255);
    g = clamp(g,0,255);
    b = clamp(b,0,255);
    a = clamp(a,0,255);

    ct.colour = getRGBA(r, g, b, a);

    if (cen)
        _x = ((160 ) - ((len(_s)) / 2));
    int bfontpos = 0;
    int curr;
    std::string::iterator iter = _s.begin();
    while (iter != _s.end()) {
        curr = utf8::unchecked::next(iter);
        point tpoint;
        tpoint.x = _x + bfontpos;
        tpoint.y = _y;

        SDL_Rect fontRect = bfont_rect;
        fontRect.x = tpoint.x ;
        fontRect.y = tpoint.y ;

        if (flipmode)
        {
            BlitSurfaceColoured( flipbfont[font_idx(curr)], NULL, backBuffer, &fontRect , ct);
        }
        else
        {
            BlitSurfaceColoured( bfont[font_idx(curr)], NULL, backBuffer, &fontRect , ct);
        }
        bfontpos+=bfontlen(curr) ;
    }
}


void Graphics::bigprint(  int _x, int _y, std::string _s, int r, int g, int b, bool cen, int sc )
{
    r = clamp(r,0,255);
    g = clamp(g,0,255);
    b = clamp(b,0,255);

    ct.colour = getRGB(r, g, b);

    if (cen)
    {
        _x = std::max(160 - (int((len(_s)/ 2.0)*sc)), 0 );
    }

    int bfontpos = 0;
    int curr;
    std::string::iterator iter = _s.begin();
    while (iter != _s.end()) {
        curr = utf8::unchecked::next(iter);

        /*
        point tpoint;
        tpoint.x = _x + bfontpos;
        tpoint.y = _y;

        SDL_Rect fontRect = bfont_rect;
        fontRect.x = tpoint.x ;
        fontRect.y = tpoint.y ;
        */

        if (flipmode)
        {
            SDL_Surface* tempPrint = ScaleSurfaceSlow(flipbfont[font_idx(curr)], bfont[font_idx(curr)]->w *sc,bfont[font_idx(curr)]->h *sc);
            SDL_Rect printrect = { Sint16((_x) + bfontpos), Sint16(_y) , Sint16(bfont_rect.w*sc), Sint16(bfont_rect.h * sc)};
            BlitSurfaceColoured(tempPrint, NULL, backBuffer, &printrect, ct);
            SDL_FreeSurface(tempPrint);
        }
        else
        {
            SDL_Surface* tempPrint = ScaleSurfaceSlow(bfont[font_idx(curr)], bfont[font_idx(curr)]->w *sc,bfont[font_idx(curr)]->h *sc);
            SDL_Rect printrect = { static_cast<Sint16>((_x) + bfontpos), static_cast<Sint16>(_y) , static_cast<Sint16>((bfont_rect.w*sc)+1), static_cast<Sint16>((bfont_rect.h * sc)+1)};
            BlitSurfaceColoured(tempPrint, NULL, backBuffer, &printrect, ct);
            SDL_FreeSurface(tempPrint);
        }
        bfontpos+=bfontlen(curr) *sc;
    }
}

int Graphics::len(std::string t)
{
    int bfontpos = 0;
    std::string::iterator iter = t.begin();
    while (iter != t.end()) {
        int cur = utf8::unchecked::next(iter);
        bfontpos += bfontlen(cur);
    }
    return bfontpos;
}

void Graphics::PrintOff( int _x, int _y, std::string _s, int r, int g, int b, bool cen /*= false*/ ) {
    PrintOffAlpha(_x,_y,_s,r,g,b,255,cen);
}

void Graphics::PrintOffAlpha( int _x, int _y, std::string _s, int r, int g, int b, int a, bool cen /*= false*/ )
{
    r = clamp(r,0,255);
    g = clamp(g,0,255);
    b = clamp(b,0,255);
    a = clamp(a,0,255);

    ct.colour = getRGB(r, g, b);

    if (cen)
        _x = ((160) - (len(_s) / 2))+_x;
    int bfontpos = 0;
    std::string::iterator iter = _s.begin();
    while (iter != _s.end()) {
        int curr = utf8::unchecked::next(iter);
        point tpoint;
        tpoint.x = _x + bfontpos;
        tpoint.y = _y;

        SDL_Rect fontRect = bfont_rect;
        fontRect.x = tpoint.x ;
        fontRect.y = tpoint.y ;

        if (flipmode)
        {
            BlitSurfaceColoured( flipbfont[font_idx(curr)], NULL, backBuffer, &fontRect , ct);
        }
        else
        {
            BlitSurfaceColoured( bfont[font_idx(curr)], NULL, backBuffer, &fontRect , ct);
        }
        bfontpos+=bfontlen(curr) ;
    }
}

void Graphics::bprint( int x, int y, std::string t, int r, int g, int b, bool cen /*= false*/ ) {
    bprintalpha(x,y,t,r,g,b,255,cen);
}

void Graphics::bprintalpha( int x, int y, std::string t, int r, int g, int b, int a, bool cen /*= false*/ )
{
    if (!notextoutline)
    {
        PrintAlpha(x, y - 1, t, 0, 0, 0, a, cen);
        if (cen)
        {
            PrintOffAlpha(-1, y, t, 0, 0, 0, a, cen);
            PrintOffAlpha(1, y, t, 0, 0, 0, a, cen);
        }
        else
        {
            PrintAlpha(x  -1, y, t, 0, 0, 0, a, cen);
            PrintAlpha(x  +1, y, t, 0, 0, 0, a, cen);
        }
        PrintAlpha(x, y+1, t, 0, 0, 0, a, cen);
    }

    PrintAlpha(x, y, t, r, g, b, a, cen);
}

void Graphics::RPrint( int _x, int _y, std::string _s, int r, int g, int b, bool cen /*= false*/ )
{
    r = clamp(r,0,255);
    g = clamp(g,0,255);
    b = clamp(b,0,255);
    ct.colour = getRGB(r, g, b);

    if (cen)
        _x = ((308) - (_s.length() / 2));
    int bfontpos = 0;
    int curr;
    std::string::iterator iter = _s.begin();
    while (iter != _s.end()) {
        curr = utf8::unchecked::next(iter);
        point tpoint;
        tpoint.x = _x + bfontpos;
        tpoint.y = _y;

        SDL_Rect fontRect = bfont_rect;
        fontRect.x = tpoint.x ;
        fontRect.y = tpoint.y ;

        if (flipmode)
        {
            BlitSurfaceColoured( flipbfont[font_idx(curr)], NULL, backBuffer, &fontRect , ct);
        }
        else
        {
            BlitSurfaceColoured( bfont[font_idx(curr)], NULL, backBuffer, &fontRect , ct);
        }
        bfontpos+=bfontlen(curr) ;
    }
}

void Graphics::printcrewname( int x, int y, int t )
{
    //Print the name of crew member t in the right colour
    switch(t)
    {
    case 0:
        Print(x, y, "Viridian", 16, 240, 240,false );
        break;
    case 1:
        Print(x, y, "Violet", 240, 16, 240,false);
        break;
    case 2:
        Print(x, y, "Vitellary", 240, 240, 16,false);
        break;
    case 3:
        Print(x, y, "Vermilion", 240, 16, 16,false);
        break;
    case 4:
        Print(x, y, "Verdigris", 16, 240, 16,false);
        break;
    case 5:
        Print(x, y, "Victoria", 16, 16, 240,false);
        break;
    }
}

void Graphics::printcrewnamedark( int x, int y, int t )
{
    //Print the name of crew member t as above, but in black and white
    switch(t)
    {
    case 0:
        Print(x, y, "Viridian", 128,128,128,false);
        break;
    case 1:
        Print(x, y, "Violet", 128,128,128,false);
        break;
    case 2:
        Print(x, y, "Vitellary", 128,128,128,false);
        break;
    case 3:
        Print(x, y, "Vermilion", 128,128,128,false);
        break;
    case 4:
        Print(x, y, "Verdigris", 128,128,128,false);
        break;
    case 5:
        Print(x, y, "Victoria", 128,128,128,false);
        break;
    }
}

void Graphics::printcrewnamestatus( int x, int y, int t )
{
    //Print the status of crew member t in the right colour
    switch(t)
    {
    case 0:
        Print(x, y, "(that's you!)", 12, 140, 140,false);
        break;
    case 1:
        Print(x, y, "Rescued!", 140, 12, 140,false);
        break;
    case 2:
        Print(x, y, "Rescued!", 140, 140, 12,false);
        break;
    case 3:
        Print(x, y, "Rescued!", 140, 12, 12,false);
        break;
    case 4:
        Print(x, y, "Rescued!", 12, 140, 12,false);
        break;
    case 5:
        Print(x, y, "Rescued!", 12, 12, 140,false);
        break;
    }
}

void Graphics::drawsprite( int x, int y, int t, int r, int g,  int b )
{
    SDL_Rect rect = { Sint16(x), Sint16(y), sprites_rect.w, sprites_rect.h };
    setcolreal(getRGB(r,g,b));
    BlitSurfaceColoured(sprites[t], NULL, backBuffer, &rect, ct);
}

void Graphics::drawtile( int x, int y, int t )
{
    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };
    BlitSurfaceStandard(tiles[t], NULL, backBuffer, &rect);
}


void Graphics::drawtile2( int x, int y, int t )
{
    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };
    BlitSurfaceStandard(tiles2[t], NULL, backBuffer, &rect);
}



void Graphics::drawtile3( int x, int y, int t, int off )
{
    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };
    BlitSurfaceStandard(tiles3[t+(off*30)], NULL, backBuffer, &rect);
}

void Graphics::drawentcolours( int x, int y, int t)
{
    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };
    BlitSurfaceStandard(entcolours[t], NULL, backBuffer, &rect);
}

void Graphics::drawtowertile( int x, int y, int t )
{
    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };
    BlitSurfaceStandard(tiles2[t], NULL, towerbuffer, &rect);
}


void Graphics::drawtowertile3( int x, int y, int t, int off )
{
    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };
    BlitSurfaceStandard(tiles3[t+(off*30)], NULL, towerbuffer, &rect);
}

void Graphics::drawgui()
{
    textboxcleanup();
    //Draw all the textboxes to the screen
    for (size_t i = 0; i<textbox.size(); i++)
    {
        //This routine also updates the textboxs
        textbox[i].update();
        if (textbox[i].tm == 2 && textbox[i].tl <= 0.5)
        {
            textbox.erase(textbox.begin() + i);
            i--;
            continue;
        }

        if (textbox[i].r == 0 && textbox[i].g == 0 && textbox[i].b == 0)
        {
            if(flipmode)
            {
                for (j = 0; j < (int) textbox[i].line.size(); j++)
                {
                    Print(textbox[i].xp + 8, textbox[i].yp + (textbox[i].line.size()*8) - (j * 8), textbox[i].line[j], 196, 196, 255 - help.glow);
                }
            }
            else
            {
                for (j = 0; j < (int) textbox[i].line.size(); j++)
                {
                    Print(textbox[i].xp + 8, textbox[i].yp + 8 + (j * 8), textbox[i].line[j], 196, 196, 255 - help.glow);
                }
            }
        }
        else
        {

            FillRect(backBuffer,textbox[i].textrect, textbox[i].r/6, textbox[i].g/6, textbox[i].b / 6 );

            drawcoloredtile(textbox[i].xp, textbox[i].yp, 40, textbox[i].r, textbox[i].g, textbox[i].b);
            drawcoloredtile(textbox[i].xp+textbox[i].w-8, textbox[i].yp, 42, textbox[i].r, textbox[i].g, textbox[i].b);
            drawcoloredtile(textbox[i].xp, textbox[i].yp+textbox[i].h-8, 45, textbox[i].r, textbox[i].g, textbox[i].b);
            drawcoloredtile(textbox[i].xp+textbox[i].w-8, textbox[i].yp+textbox[i].h-8, 47, textbox[i].r, textbox[i].g, textbox[i].b);
            for (int k = 0; k < textbox[i].lw; k++)
            {
                drawcoloredtile(textbox[i].xp + 8 + (k * 8), textbox[i].yp, 41, textbox[i].r, textbox[i].g, textbox[i].b);
                drawcoloredtile(textbox[i].xp + 8 + (k * 8), textbox[i].yp+textbox[i].h-8, 46, textbox[i].r, textbox[i].g, textbox[i].b);
            }
            for (size_t k = 0; k < textbox[i].line.size(); k++)
            {
                drawcoloredtile(textbox[i].xp, textbox[i].yp + 8 + (k * 8), 43, textbox[i].r, textbox[i].g, textbox[i].b);
                drawcoloredtile(textbox[i].xp + textbox[i].w-8, textbox[i].yp + 8 + (k * 8), 44, textbox[i].r, textbox[i].g, textbox[i].b);
            }

            if(flipmode)
            {
                for (j = 0; j < (int) textbox[i].line.size(); j++)
                {
                    Print(textbox[i].xp + 8, textbox[i].yp  + (textbox[i].line.size()*8) - (j * 8), textbox[i].line[j], textbox[i].r, textbox[i].g, textbox[i].b);
                }
            }
            else
            {
                for (j = 0; j < (int) textbox[i].line.size(); j++)
                {
                    Print(textbox[i].xp + 8, textbox[i].yp +8 + (j * 8), textbox[i].line[j], textbox[i].r, textbox[i].g, textbox[i].b);
                }
            }
        }

        if ((textbox[i].yp == 12 || textbox[i].yp == 180) && textbox[i].r == 165)
        {
            if (flipmode)
            {
                drawimage(5, 0, 180, true);
            }
            else
            {
                drawimage(0, 0, 12, true);
            }
        }
        else if ((textbox[i].yp == 12 || textbox[i].yp == 180) && textbox[i].g == 165)
        {
            if (flipmode)
            {
                drawimage(6, 0, 180, true);
            }
            else
            {
                drawimage(4, 0, 12, true);
            }
        }
        if (flipmode)
        {
            if (textbox[i].r == 175 && textbox[i].g == 175)
            {
                //purple guy
                drawsprite(80 - 6, 64 + 48 + 4, 6, 220- help.glow/4 - int(fRandom()*20), 120- help.glow/4, 210 - help.glow/4);
            }
            else if (textbox[i].r == 175 && textbox[i].b == 175)
            {
                //red guy
                drawsprite(80 - 6, 64 + 48+ 4, 6, 255 - help.glow/8, 70 - help.glow/4, 70 - help.glow / 4);
            }
            else if (textbox[i].r == 175)
            {
                //green guy
                drawsprite(80 - 6, 64 + 48 + 4, 6, 120 - help.glow / 4 - int(fRandom() * 20), 220 - help.glow / 4, 120 - help.glow / 4);
            }
            else if (textbox[i].g == 175)
            {
                //yellow guy
                drawsprite(80 - 6, 64 + 48+ 4, 6, 220- help.glow/4 - int(fRandom()*20), 210 - help.glow/4, 120- help.glow/4);
            }
            else if (textbox[i].b == 175)
            {
                //blue guy
                drawsprite(80 - 6, 64 + 48+ 4, 6, 75, 75, 255- help.glow/4 - int(fRandom()*20));
            }
        }
        else
        {
            if (textbox[i].r == 175 && textbox[i].g == 175)
            {
                //purple guy
                drawsprite(80 - 6, 64 + 32 + 4, 0, 220- help.glow/4 - int(fRandom()*20), 120- help.glow/4, 210 - help.glow/4);
            }
            else if (textbox[i].r == 175 && textbox[i].b == 175)
            {
                //red guy
                drawsprite(80 - 6, 64 + 32 + 4, 0, 255 - help.glow/8, 70 - help.glow/4, 70 - help.glow / 4);
            }
            else if (textbox[i].r == 175)
            {
                //green guy
                drawsprite(80 - 6, 64 + 32 + 4, 0, 120 - help.glow / 4 - int(fRandom() * 20), 220 - help.glow / 4, 120 - help.glow / 4);
            }
            else if (textbox[i].g == 175)
            {
                //yellow guy
                drawsprite(80 - 6, 64 + 32 + 4, 0, 220- help.glow/4 - int(fRandom()*20), 210 - help.glow/4, 120- help.glow/4);
            }
            else if (textbox[i].b == 175)
            {
                //blue guy
                drawsprite(80 - 6, 64 + 32 + 4, 0, 75, 75, 255- help.glow/4 - int(fRandom()*20));
            }
        }
    }
}

void Graphics::drawimagecol( int t, int xp, int yp, int r = 0, int g = 0, int b = 0, bool cent/*= false*/ )
{
    SDL_Rect trect;
    if(r+g+b != 0)
    {
        RGBf(r,g,b);
    }

    point tpoint;
    if (cent)
    {
        tpoint.x = 160 - int(images[t]->w / 2);
        tpoint.y = yp;
        trect.x = tpoint.x ;
        trect.y = tpoint.y;
        trect.w = images[t]->w;
        trect.h= images[t]->h;
        BlitSurfaceColoured(images[t], NULL, backBuffer, &trect, ct);

    }
    else
    {
        trect.x = xp;
        trect.y = yp;
        trect.w = images[t]->w;
        trect.h = images[t]->h;
        BlitSurfaceColoured(images[t], NULL, backBuffer, &trect, ct);

    }
}

void Graphics::drawimage( int t, int xp, int yp, bool cent/*=false*/ )
{

    SDL_Rect trect;
    if (cent)
    {
        trect.x = 160 - int(images[t]->w / 2);
        trect.y = yp;
        trect.w = images[t]->w;
        trect.h = images[t]->h;
        BlitSurfaceStandard(images[t], NULL, backBuffer, &trect);
    }
    else
    {

        trect.x = xp;
        trect.y = yp;
        trect.w = images[t]->w;
        trect.h= images[t]->h;

        BlitSurfaceStandard(images[t], NULL, backBuffer, &trect);
    }
}

void Graphics::drawpartimage( int t, int xp, int yp, int wp, int hp)
{
  SDL_Rect trect;

  trect.x = xp;
  trect.y = yp;
  trect.w = wp;
  trect.h= hp;

  SDL_Rect trect2;

  trect2.x = 0;
  trect2.y = 0;
  trect2.w = wp;
  trect2.h= hp;

  BlitSurfaceStandard(images[t], &trect2, backBuffer, &trect);
}

void Graphics::cutscenebars()
{
    if (showcutscenebars)
    {
        cutscenebarspos += 25;
        cutscenebarspos = std::min(cutscenebarspos, 361);
        FillRect(backBuffer, 0, 0, cutscenebarspos, 16, 0x000000);
        FillRect(backBuffer, 360-cutscenebarspos, 224, cutscenebarspos, 16, 0x000000);
    }
    else
    {
        //disappearing
        if (cutscenebarspos > 0)
        {
            cutscenebarspos -= 25;
            cutscenebarspos = std::max(cutscenebarspos, 0);
            //draw
            FillRect(backBuffer, 0, 0, cutscenebarspos, 16, 0x000000);
            FillRect(backBuffer, 360-cutscenebarspos, 224, cutscenebarspos, 16, 0x000000);
        }
    }
}

void Graphics::drawcrewman( int x, int y, int t, bool act, bool noshift /*=false*/ )
{
    if (!act)
    {
        if (noshift)
        {
            if (flipmode)
            {
                drawspritesetcol(x, y, 14, 19);
            }
            else
            {
                drawspritesetcol(x, y, 12, 19);
            }
        }
        else
        {
            if (flipmode)
            {
                drawspritesetcol(x - 8, y, 14, 19);
            }
            else
            {
                drawspritesetcol(x - 8, y, 12, 19);
            }
        }
    }
    else
    {
        if (flipmode) crewframe += 6;

        switch(t)
        {
        case 0:
            drawspritesetcol(x, y, crewframe, 0);
            break;
        case 1:
            drawspritesetcol(x, y, crewframe, 20);
            break;
        case 2:
            drawspritesetcol(x, y, crewframe, 14);
            break;
        case 3:
            drawspritesetcol(x, y, crewframe, 15);
            break;
        case 4:
            drawspritesetcol(x, y, crewframe, 13);
            break;
        case 5:
            drawspritesetcol(x, y, crewframe, 16);
            break;
        }

        if (flipmode) crewframe -= 6;
    }
}

void Graphics::drawpixeltextbox( int x, int y, int w, int h, int w2, int h2, int r, int g, int b, int xo, int yo )
{
    //given these parameters, draw a textbox with a pixel width

    //madrect.x = x; madrect.y = y; madrect.w = w; madrect.h = h;
    FillRect(backBuffer,x,y,w,h, r/6, g/6, b/6 );

    for (k = 0; k < w2-2; k++)
    {
        drawcoloredtile(x + 8-xo + (k * 8), y, 41, r, g, b);
        drawcoloredtile(x + 8-xo + (k * 8), y + (h) - 8, 46, r, g, b);
    }

    for (k = 0; k < h2-2; k++)
    {
        drawcoloredtile(x, y + 8-yo + (k * 8), 43, r, g, b);
        drawcoloredtile(x + (w) - 8, y + 8-yo + (k * 8), 44, r, g, b);
    }

    drawcoloredtile(x, y, 40, r, g, b);
    drawcoloredtile(x + (w) - 8, y, 42, r, g, b);
    drawcoloredtile(x, y + (h) - 8, 45, r, g, b);
    drawcoloredtile(x + (w) - 8, y + (h) - 8, 47, r, g, b);
}

void Graphics::drawcustompixeltextbox( int x, int y, int w, int h, int w2, int h2, int r, int g, int b, int xo, int yo )
{
    //given these parameters, draw a textbox with a pixel width

    FillRect(backBuffer,x,y,w,h, r/6, g/6, b/6 );

    for (k = 0; k < w2-2; k++)
    {
        drawcoloredtile(x + 8-xo + (k * 8), y, 41, r, g, b);
        drawcoloredtile(x + 8-xo + (k * 8), y + (h) - 8, 46, r, g, b);
    }


    drawcoloredtile(x+ (w) - 16, y, 41, r, g, b);
    drawcoloredtile(x+ (w) - 16, y + (h) - 8, 46, r, g, b);
    drawcoloredtile(x+ (w) - 24, y, 41, r, g, b);
    drawcoloredtile(x+ (w) - 24, y + (h) - 8, 46, r, g, b);

    for (k = 0; k < h2-2; k++)
    {
        drawcoloredtile(x, y + 8-yo + (k * 8), 43, r, g, b);
        drawcoloredtile(x + (w) - 8, y + 8-yo + (k * 8), 44, r, g, b);
    }

    drawcoloredtile(x, y + (h) - 16, 43, r, g, b);
    drawcoloredtile(x + (w) - 8, y + (h) - 16, 44, r, g, b);
    drawcoloredtile(x, y + (h) - 24, 43, r, g, b);
    drawcoloredtile(x + (w) - 8, y + (h) - 24, 44, r, g, b);

    drawcoloredtile(x, y, 40, r, g, b);
    drawcoloredtile(x + (w) - 8, y, 42, r, g, b);
    drawcoloredtile(x, y + (h) - 8, 45, r, g, b);
    drawcoloredtile(x + (w) - 8, y + (h) - 8, 47, r, g, b);
}

void Graphics::drawtextbox( int x, int y, int w, int h, int r, int g, int b )
{
    //given these parameters, draw a textbox
    FillRect(backBuffer,x,y,w*8,h*8, r/6, g/6, b/6 );

    drawcoloredtile(x, y, 40, r, g, b);
    drawcoloredtile(x + (w*8) - 8, y, 42, r, g, b);
    drawcoloredtile(x, y + (h*8) - 8, 45, r, g, b);
    drawcoloredtile(x + (w*8) - 8, y + (h*8) - 8, 47, r, g, b);

    for (int k = 0; k < w-2; k++)
    {
        drawcoloredtile(x + 8 + (k * 8), y, 41, r, g, b);
        drawcoloredtile(x + 8 + (k * 8), y + (h * 8) - 8, 46, r, g, b);
    }

    for (int k = 0; k < h-2; k++)
    {
        drawcoloredtile(x, y + 8 + (k * 8), 43, r, g, b);
        drawcoloredtile(x + (w * 8) - 8, y + 8 + (k * 8), 44, r, g, b);
    }
}

void Graphics::textboxactive()
{
    //Remove all but the most recent textbox
    for (int i = 0; i < (int) textbox.size(); i++)
    {
        if (m != i) textbox[i].remove();
    }
}

void Graphics::textboxremovefast()
{
    //Remove all textboxes
    for (size_t i = 0; i < textbox.size(); i++)
    {
        textbox[i].removefast();
    }
}

void Graphics::textboxremove()
{
    //Remove all textboxes
    for (size_t i = 0; i < textbox.size(); i++)
    {
        textbox[i].remove();
    }
}

void Graphics::textboxtimer( int t )
{
    textbox[m].timer=t;
}

void Graphics::addline( std::string t )
{
    textbox[m].addline(t);
}

void Graphics::textboxadjust()
{
    textbox[m].adjust();
}


void Graphics::createtextbox( std::string t, int xp, int yp, int r/*= 255*/, int g/*= 255*/, int b /*= 255*/ )
{
    m = textbox.size();

    if(m<20)
    {
        textboxclass text;
        text.line.push_back(t);
        text.xp = xp;
        int length = utf8::unchecked::distance(t.begin(), t.end());
        if (xp == -1) text.xp = 160 - (((length / 2) + 1) * 8);
        text.yp = yp;
        text.initcol(r, g, b);
        text.resize();
        textbox.push_back(text);
    }
}

void Graphics::drawfade()
{
    if ((fademode == 1)||(fademode == 4))
    {
        FillRect(backBuffer, 0, 0, backBuffer->w, backBuffer->h, 0x000000);
    }
    else if(fademode==3)
    {
        for (int i = 0; i < 15; i++)
        {
            FillRect(backBuffer, fadebars[i], i * 16, fadeamount, 16, 0x000000 );
        }
    }
    else if(fademode==5 )
    {
        for (int i = 0; i < 15; i++)
        {
            FillRect(backBuffer, fadebars[i]-fadeamount, i * 16, 500, 16, 0x000000 );
        }
    }

}

void Graphics::processfade()
{
    if (fademode > 1)
    {
        if (fademode == 2)
        {
            //prepare fade out
            for (int i = 0; i < 15; i++)
            {
                fadebars[i] = -int(fRandom() * 12) * 8;
            }
            fadeamount = 0;
            fademode = 3;
        }
        else if (fademode == 3)
        {
            fadeamount += 24;
            if (fadeamount > 416)
            {
                fademode = 1; //faded
            }
        }
        else if (fademode == 4)
        {
            //prepare fade in
            for (int i = 0; i < 15; i++)
            {
                fadebars[i] = 320 + int(fRandom() * 12) * 8;
            }
            fadeamount = 416;
            fademode = 5;
        }
        else if (fademode == 5)
        {
            fadeamount -= 24;
            if (fadeamount <= 0)
            {
                fademode = 0; //normal
            }
        }
    }
}

void Graphics::drawmenu( int cr, int cg, int cb, int division /*= 30*/ )
{
    for (size_t i = 0; i < game.menuoptions.size(); i++)
    {
        if ((int) i == game.currentmenuoption)
        {
            //Draw it highlighted
            if (game.menuoptions[i].active)
            {
                std::string tempstring = game.menuoptions[i].text;
                std::transform(tempstring.begin(), tempstring.end(),tempstring.begin(), ::toupper);
                tempstring = std::string("[ ") + tempstring + std::string(" ]");
                Print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12) +game.menuyoff, tempstring, cr, cg, cb);
            }
            else
            {
                std::string tempstring = game.menuoptions[i].text;
                tempstring = "[ " + tempstring + " ]";
                //Draw it in gray
                Print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12)+game.menuyoff, tempstring, 128, 128, 128);
            }
        }
        else
        {
            //Draw it normally
            if (game.menuoptions[i].active)
            {
                Print(110 + (i * division) +game.menuxoff, 140 + (i * 12)+game.menuyoff, game.menuoptions[i].text, cr, cg, cb);
            }
            else
            {
                //Draw it in gray
                Print(110 + (i * division) +game.menuxoff, 140 + (i * 12)+game.menuyoff, game.menuoptions[i].text, 128, 128, 128);
            }
        }
    }
}

void Graphics::drawlevelmenu( int cr, int cg, int cb, int division /*= 30*/ )
{
    for (size_t i = 0; i < game.menuoptions.size(); i++)
    {
        if ((int) i == game.currentmenuoption)
        {
          if(game.menuoptions.size()-i<=3){
            //Draw it highlighted
            if (game.menuoptions[i].active)
            {
                std::string tempstring = game.menuoptions[i].text;
                std::transform(tempstring.begin(), tempstring.end(),tempstring.begin(), ::toupper);
                tempstring = std::string("[ ") + tempstring + std::string(" ]");
                Print(110 + (i * division) - 16 +game.menuxoff, 140+8 + (i * 12) +game.menuyoff, tempstring, cr, cg, cb);
            }
            else
            {
                std::string tempstring = game.menuoptions[i].text;
                tempstring = "[ " + tempstring + " ]";
                //Draw it in gray
                Print(110 + (i * division) - 16 +game.menuxoff, 140+8 + (i * 12)+game.menuyoff, tempstring, 128, 128, 128);
            }
          }else{
            //Draw it highlighted
            if (game.menuoptions[i].active)
            {
                std::string tempstring = game.menuoptions[i].text;
                std::transform(tempstring.begin(), tempstring.end(),tempstring.begin(), ::toupper);
                tempstring = std::string("[ ") + tempstring + std::string(" ]");
                Print(110 + (i * division) - 16 +game.menuxoff, 144 + (i * 12) +game.menuyoff, tempstring, cr, cg, cb);
            }
            else
            {
                std::string tempstring = game.menuoptions[i].text;
                tempstring = "[ " + tempstring + " ]";
                //Draw it in gray
                Print(110 + (i * division) - 16 +game.menuxoff, 144 + (i * 12)+game.menuyoff, tempstring, 128, 128, 128);
            }
          }
        }
        else
        {
          if(game.menuoptions.size()-i<=3){
            //Draw it normally
            if (game.menuoptions[i].active)
            {
                Print(110 + (i * division) +game.menuxoff, 140+8 + (i * 12)+game.menuyoff, game.menuoptions[i].text, cr, cg, cb);
            }
            else
            {
                //Draw it in gray
                Print(110 + (i * division) +game.menuxoff, 140+8 + (i * 12)+game.menuyoff, game.menuoptions[i].text, 128, 128, 128);
            }
          }else{
            //Draw it normally
            if (game.menuoptions[i].active)
            {
                Print(110 + (i * division) +game.menuxoff, 144 + (i * 12)+game.menuyoff, game.menuoptions[i].text, cr, cg, cb);
            }
            else
            {
                //Draw it in gray
                Print(110 + (i * division) +game.menuxoff, 144 + (i * 12)+game.menuyoff, game.menuoptions[i].text, 128, 128, 128);
            }
          }
        }
    }
}


void Graphics::drawcoloredtile( int x, int y, int t, int r, int g, int b )
{
    setcolreal(getRGB(r,g,b));

    SDL_Rect rect;
    setRect(rect,x,y,tiles_rect.w,tiles_rect.h);
    BlitSurfaceColoured(tiles[t],NULL, backBuffer, &rect, ct );
}


bool Graphics::Hitest(SDL_Surface* surface1, point p1, SDL_Surface* surface2, point p2)
{

    //find rectangle where they intersect:

    int r1_left = p1.x;
    int r1_right = r1_left + surface1->w;
    int r2_left = p2.x;
    int r2_right = r2_left + surface2->w;

    int r1_bottom = p1.y;
    int r1_top = p1.y + surface1->h;
    int r2_bottom  = p2.y;
    int r2_top = p2.y + surface2->h;

    bool intersection = intersectRect(r1_left, r1_right, r1_bottom, r1_top, r2_left, r2_right, r2_bottom, r2_top);

    if(intersection)
    {
        int r3_left = std::max(r1_left, r2_left);
        int r3_top = std::min(r1_top, r2_top);
        int r3_right = std::min(r1_right, r2_right);
        int r3_bottom= std::max(r1_bottom, r2_bottom);

        //for every pixel inside rectangle
        for(int x = r3_left; x < r3_right; x++)
        {
            for(int y = r3_bottom; y < r3_top; y++)
            {
                Uint32 pixel1 = ReadPixel(surface1 , x - p1.x, y - p1.y);
                Uint32 pixel2 = ReadPixel(surface2 , x - p2.x, y - p2.y);
                if ((pixel1 & 0x000000FF) && (pixel2 & 0x000000FF))
                {
                    return true;
                }
            }
        }
        return false;
    }
    else
    {
        return false;
    }

}

void Graphics::drawgravityline( int t )
{
    if (obj.entities[t].life == 0)
    {
        switch(linestate)
        {
        case 0:
            FillRect(backBuffer,line_rect, getRGB(200-20, 200-20, 200-20));
            break;
        case 1:
            FillRect(backBuffer,line_rect, getRGB(225-30, 245-30, 245-30));
            break;
        case 2:
            FillRect(backBuffer,line_rect, getRGB(245-30, 245-30, 225-30));
            break;
        case 3:
            FillRect(backBuffer,line_rect, getRGB(164-10, 200-20, 200-20));
            break;
        case 4:
            FillRect(backBuffer,line_rect, getRGB(224-20, 255-30, 196-20));
            break;
        case 5:
            FillRect(backBuffer,line_rect, getRGB(205-20, 235-30, 196-20));
            break;
        case 6:
            FillRect(backBuffer,line_rect, getRGB(164-10, 164-10, 164-10));
            break;
        case 7:
            FillRect(backBuffer,line_rect, getRGB(225-30, 245-30, 205-20));
            break;
        case 8:
            FillRect(backBuffer,line_rect, getRGB(205-20, 255-30, 225-30));
            break;
        case 9:
            FillRect(backBuffer,line_rect, getRGB(245-30, 245-30, 245-30));
            break;
        }
    }
    else
    {
        FillRect(backBuffer,line_rect, getRGB(96, 96, 96));
    }
}

void Graphics::drawtrophytext()
{
    int temp, temp2, temp3;

    if (obj.trophytext < 15)
    {
        temp = (196 * obj.trophytext) / 15;
        temp2 = (196 * obj.trophytext) / 15;
        temp3 = ((255 - help.glow) * obj.trophytext) / 15;
    }
    else
    {
        temp = 196;
        temp2 = 196;
        temp3 = 255 - help.glow;
    }
    switch(obj.trophytype)
    {
    case 1:
        Print( -1, 6, "SPACE STATION 1 MASTERED", temp, temp2, temp3, true);
        Print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
        break;
    case 2:
        Print( -1, 6, "LABORATORY MASTERED", temp, temp2, temp3, true);
        Print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
        break;
    case 3:
        Print( -1, 6, "THE TOWER MASTERED", temp, temp2, temp3, true);
        Print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
        break;
    case 4:
        Print( -1, 6, "SPACE STATION 2 MASTERED", temp, temp2, temp3, true);
        Print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
        break;
    case 5:
        Print( -1, 6, "WARP ZONE MASTERED", temp, temp2, temp3, true);
        Print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
        break;
    case 6:
        Print( -1, 6, "FINAL LEVEL MASTERED", temp, temp2, temp3, true);
        Print( -1, 16, "Obtain a V Rank in this Time Trial", temp, temp2, temp3, true);
        break;
    case 7:
        Print( -1, 6, "GAME COMPLETE", temp, temp2, temp3, true);
        Print( -1, 16, "Complete the game", temp, temp2, temp3, true);
        break;
    case 8:
        Print( -1, 6, "FLIP MODE COMPLETE", temp, temp2, temp3, true);
        Print( -1, 16, "Complete the game in flip mode", temp, temp2, temp3, true);
        break;
    case 9:
        Print( -1, 11, "Win with less than 50 deaths", temp, temp2, temp3, true);
        break;
    case 10:
        Print( -1, 11, "Win with less than 100 deaths", temp, temp2, temp3, true);
        break;
    case 11:
        Print( -1, 11, "Win with less than 250 deaths", temp, temp2, temp3, true);
        break;
    case 12:
        Print( -1, 11, "Win with less than 500 deaths", temp, temp2, temp3, true);
        break;
    case 13:
        Print( -1, 11, "Last 5 seconds on the Super Gravitron", temp, temp2, temp3, true);
        break;
    case 14:
        Print( -1, 11, "Last 10 seconds on the Super Gravitron", temp, temp2, temp3, true);
        break;
    case 15:
        Print( -1, 11, "Last 15 seconds on the Super Gravitron", temp, temp2, temp3, true);
        break;
    case 16:
        Print( -1, 11, "Last 20 seconds on the Super Gravitron", temp, temp2, temp3, true);
        break;
    case 17:
        Print( -1, 11, "Last 30 seconds on the Super Gravitron", temp, temp2, temp3, true);
        break;
    case 18:
        Print( -1, 11, "Last 1 minute on the Super Gravitron", temp, temp2, temp3, true);
        break;
    case 20:
        Print( -1, 6, "MASTER OF THE UNIVERSE", temp, temp2, temp3, true);
        Print( -1, 16, "Complete the game in no death mode", temp, temp2, temp3, true);
        break;
    }
}

void Graphics::drawentities()
{
    //Update line colours!
    if (linedelay <= 0)
    {
        linestate++;
        if (linestate >= 10) linestate = 0;
        linedelay = 2;
    }
    else
    {
        linedelay--;
    }

    point tpoint;

    SDL_Rect drawRect;

    std::vector<SDL_Surface*> *tilesvec;
    if (map.custommode)
    {
        tilesvec = &entcolours;
    }
    else
    {
        tilesvec = &tiles;
    }

    std::vector<SDL_Surface*> *spritesvec;
    if (flipmode)
    {
        spritesvec = &flipsprites;
    }
    else
    {
        spritesvec = &sprites;
    }

    int yoff;
    if (map.towermode)
    {
        yoff = map.ypos;
    }
    else
    {
        yoff = 0;
    }

    trinketcolset = false;

    for (int i = obj.entities.size() - 1; i >= 0; i--)
    {
        if (obj.entities[i].invis)
        {
            continue;
        }

        switch (obj.entities[i].size)
        {
        case 0:
            // Sprites
            tpoint.x = obj.entities[i].xp;
            tpoint.y = obj.entities[i].yp - yoff;
            setcol(obj.entities[i].colour);

            drawRect = sprites_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;
            BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);
            //screenwrapping!
            if (map.warpx ||
            (map.towermode && !map.minitowermode
            && map.ypos >= 500 && map.ypos <= 5000))   //The "wrapping" area of the tower
            {
                if (tpoint.x < 0)
                {
                    tpoint.x += 320;
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);
                }
                else if (tpoint.x > 300)
                {
                    tpoint.x -= 320;
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);
                }
            }
            else if (map.warpy)
            {
                if (tpoint.y < 0)
                {
                    tpoint.y += 230;
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);
                }
                else if (tpoint.y > 210)
                {
                    tpoint.y -= 230;
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);
                }
            }
            break;
        case 1:
            // Tiles
            tpoint.x = obj.entities[i].xp;
            tpoint.y = obj.entities[i].yp - yoff;
            drawRect = tiles_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;
            BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
            break;
        case 2:
        case 8:
        {
            // Special: Moving platform, 4 tiles or 8 tiles
            tpoint.x = obj.entities[i].xp;
            tpoint.y = obj.entities[i].yp - yoff;
            int thiswidth = 4;
            if (obj.entities[i].size == 8)
            {
                thiswidth = 8;
            }
            for (int ii = 0; ii < thiswidth; ii++)
            {
                drawRect = tiles_rect;
                drawRect.x += tpoint.x;
                drawRect.y += tpoint.y;
                drawRect.x += 8 * ii;
                BlitSurfaceStandard((*tilesvec)[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
            }
            break;
        }
        case 3:    // Big chunky pixels!
            prect.x = obj.entities[i].xp;
            prect.y = obj.entities[i].yp - yoff;
            //A seperate index of colours, for simplicity
            if(obj.entities[i].colour==1)
            {
                FillRect(backBuffer, prect, (fRandom() * 64), 10, 10);
            }
            else if (obj.entities[i].colour == 2)
            {
                FillRect(backBuffer,prect, int(160- help.glow/2 - (fRandom()*20)),  200- help.glow/2, 220 - help.glow);
            }
            break;
        case 4:    // Small pickups
            drawhuetile(obj.entities[i].xp, obj.entities[i].yp - yoff, obj.entities[i].tile, obj.entities[i].colour);
            break;
        case 5:    //Horizontal Line
            line_rect.x = obj.entities[i].xp;
            line_rect.y = obj.entities[i].yp - yoff;
            line_rect.w = obj.entities[i].w;
            line_rect.h = 1;
            drawgravityline(i);
            break;
        case 6:    //Vertical Line
            line_rect.x = obj.entities[i].xp;
            line_rect.y = obj.entities[i].yp - yoff;
            line_rect.w = 1;
            line_rect.h = obj.entities[i].h;
            drawgravityline(i);
            break;
        case 7:    //Teleporter
            drawtele(obj.entities[i].xp, obj.entities[i].yp - yoff, obj.entities[i].drawframe, obj.entities[i].colour);
            break;
        //case 8:    // Special: Moving platform, 8 tiles
            // Note: This code is in the 4-tile code
            break;
        case 9:         // Really Big Sprite! (2x2)
            setcol(obj.entities[i].colour);

            tpoint.x = obj.entities[i].xp;
            tpoint.y = obj.entities[i].yp - yoff;

            drawRect = sprites_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;
            BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);

            tpoint.x = obj.entities[i].xp+32;
            tpoint.y = obj.entities[i].yp - yoff;
            //
            drawRect = sprites_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;
            BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe+1],NULL, backBuffer, &drawRect, ct);

            tpoint.x = obj.entities[i].xp;
            tpoint.y = obj.entities[i].yp+32 - yoff;
            //
            drawRect = sprites_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;
            BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe+12],NULL, backBuffer, &drawRect, ct);

            tpoint.x = obj.entities[i].xp+32;
            tpoint.y = obj.entities[i].yp+32 - yoff;
            //
            drawRect = sprites_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;
            BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe + 13],NULL, backBuffer, &drawRect, ct);
            break;
        case 10:         // 2x1 Sprite
            setcol(obj.entities[i].colour);

            tpoint.x = obj.entities[i].xp;
            tpoint.y = obj.entities[i].yp - yoff;
            //
            drawRect = sprites_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;
            BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);

            tpoint.x = obj.entities[i].xp+32;
            tpoint.y = obj.entities[i].yp - yoff;
            //
            drawRect = sprites_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;
            BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe+1],NULL, backBuffer, &drawRect, ct);
            break;
        case 11:    //The fucking elephant
            setcol(obj.entities[i].colour);
            drawimagecol(3, obj.entities[i].xp, obj.entities[i].yp - yoff);
            break;
        case 12:         // Regular sprites that don't wrap
            tpoint.x = obj.entities[i].xp;
            tpoint.y = obj.entities[i].yp - yoff;
            setcol(obj.entities[i].colour);
            //
            drawRect = sprites_rect;
            drawRect.x += tpoint.x;
            drawRect.y += tpoint.y;
            BlitSurfaceColoured((*spritesvec)[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);


            //if we're outside the screen, we need to draw indicators

            if (obj.entities[i].xp < -20 && obj.entities[i].vx > 0)
            {
                if (obj.entities[i].xp < -100)
                {
                    tpoint.x = -5 + (int(( -obj.entities[i].xp) / 10));
                }
                else
                {
                    tpoint.x = 5;
                }

                tpoint.y = tpoint.y+4;
                setcol(23);


                drawRect = tiles_rect;
                drawRect.x += tpoint.x;
                drawRect.y += tpoint.y;
                BlitSurfaceColoured(tiles[1167],NULL, backBuffer, &drawRect, ct);

            }
            else if (obj.entities[i].xp > 340 && obj.entities[i].vx < 0)
            {
                if (obj.entities[i].xp > 420)
                {
                    tpoint.x = 320 - (int(( obj.entities[i].xp-320) / 10));
                }
                else
                {
                    tpoint.x = 310;
                }

                tpoint.y = tpoint.y+4;
                setcol(23);
                //

                drawRect = tiles_rect;
                drawRect.x += tpoint.x;
                drawRect.y += tpoint.y;
                BlitSurfaceColoured(tiles[1166],NULL, backBuffer, &drawRect, ct);
            }
            break;
        case 13:
        {
            //Special for epilogue: huge hero!

            tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp - yoff;
            setcol(obj.entities[i].colour);
            SDL_Rect drawRect = {Sint16(obj.entities[i].xp ), Sint16(obj.entities[i].yp - yoff), Sint16(sprites_rect.x * 6), Sint16(sprites_rect.y * 6 ) };
            SDL_Surface* TempSurface = ScaleSurface( (*spritesvec)[obj.entities[i].drawframe], 6 * sprites_rect.w,6* sprites_rect.h );
            BlitSurfaceColoured(TempSurface, NULL , backBuffer,  &drawRect, ct );
            SDL_FreeSurface(TempSurface);



            break;
        }
        }
    }
}

void Graphics::drawbackground( int t )
{
    int temp = 0;

    switch(t)
    {
    case 1:
        //Starfield
        FillRect(backBuffer,0x00000);
        for (int i = 0; i < 50; i++)
        {
            stars[i].w = 2;
            stars[i].h = 2;
            if (starsspeed[i] <= 6)
            {
                FillRect(backBuffer,stars[i], getRGB(0x22,0x22,0x22));
            }
            else
            {
                FillRect(backBuffer,stars[i], getRGB(0x55,0x55,0x55));
            }
            stars[i].x -= Sint16(starsspeed[i]);
            if (stars[i].x < -10)
            {
                stars[i].x += 340;
                stars[i].y = int(fRandom() * 240);
                stars[i].w = 2;
                starsspeed[i] = 4+int(fRandom()*4);
            }
        }
        break;
    case 2:
            //Lab
            switch(rcol)
            {
                //Akward ordering to match tileset
            case 0:
                bcol2 = RGBflip(0, 16*backboxint[0], 16*backboxint[0]);
                break; //Cyan
            case 1:
                bcol2 = RGBflip(16*backboxint[0], 0, 0);
                break;  //Red
            case 2:
                bcol2 = RGBflip(16*backboxint[0], 0, 16*backboxint[0]);
                break; //Purple
            case 3:
                bcol2 = RGBflip(0, 0, 16*backboxint[0]);
                break;  //Blue
            case 4:
                bcol2 = RGBflip(16*backboxint[0], 16*backboxint[0], 0);
                break; //Yellow
            case 5:
                bcol2 = RGBflip(0, 16 * backboxint[0], 0);
                break;  //Green
            case 6:
                //crazy case
                spcoldel--;
                if (spcoldel <= 0)
                {
                    spcoldel = 15;
                    spcol++;
                    if (spcol >= 12) spcol = 0;
                }
                switch(spcol)
                {
                case 0:
                    bcol2 = RGBflip(0, 16*backboxint[0], 16*backboxint[0]);
                    break; //Cyan
                case 1:
                    bcol2 = RGBflip(0, (spcoldel+1)*backboxint[0], 16*backboxint[0]);
                    break; //Cyan
                case 2:
                    bcol2 = RGBflip(0, 0, 16*backboxint[0]);
                    break;  //Blue
                case 3:
                    bcol2 = RGBflip((16-spcoldel)*backboxint[0], 0, 16*backboxint[0]);
                    break;  //Blue
                case 4:
                    bcol2 = RGBflip(16*backboxint[0], 0, 16*backboxint[0]);
                    break; //Purple
                case 5:
                    bcol2 = RGBflip(16*backboxint[0], 0, (spcoldel+1)*backboxint[0]);
                    break; //Purple
                case 6:
                    bcol2 = RGBflip(16*backboxint[0], 0, 0);
                    break;  //Red
                case 7:
                    bcol2 = RGBflip(16*backboxint[0], (16-spcoldel)*backboxint[0], 0);
                    break;  //Red
                case 8:
                    bcol2 = RGBflip(16*backboxint[0], 16*backboxint[0], 0);
                    break; //Yellow
                case 9:
                    bcol2 = RGBflip((spcoldel+1)*backboxint[0], 16*backboxint[0], 0);
                    break; //Yellow
                case 10:
                    bcol2 = RGBflip(0, 16 * backboxint[0], 0);
                    break;  //Green
                case 11:
                    bcol2 = RGBflip(0, 16 * backboxint[0], (16-spcoldel)*backboxint[0]);
                    break;  //Green
                }
            break;
        }
        FillRect(backBuffer,bcol2);

        for (int i = 0; i < 18; i++)
        {
            switch(rcol)
            {
                //Akward ordering to match tileset
            case 0:
                bcol = RGBflip(16, 128*backboxint[0], 128*backboxint[0]);
                break; //Cyan
            case 1:
                bcol = RGBflip(128*backboxint[0], 16, 16);
                break;  //Red
            case 2:
                bcol = RGBflip(128*backboxint[0], 16, 128*backboxint[0]);
                break; //Purple
            case 3:
                bcol = RGBflip(16, 16, 128*backboxint[0]);
                break;  //Blue
            case 4:
                bcol = RGBflip(128*backboxint[0], 128*backboxint[0], 16);
                break; //Yellow
            case 5:
                bcol = RGBflip(16, 128 * backboxint[0], 16);
                break;  //Green
            case 6:
                //crazy case
                switch(spcol)
                {
                case 0:
                    bcol = RGBflip(16, 128*backboxint[0], 128*backboxint[0]);
                    break; //Cyan
                case 1:
                    bcol = RGBflip(16, ((spcoldel+1)*8)*backboxint[0], 128*backboxint[0]);
                    break; //Cyan
                case 2:
                    bcol = RGBflip(16, 16, 128*backboxint[0]);
                    break;  //Blue
                case 3:
                    bcol = RGBflip((128-(spcoldel*8))*backboxint[0], 16, 128*backboxint[0]);
                    break;  //Blue
                case 4:
                    bcol = RGBflip(128*backboxint[0], 16, 128*backboxint[0]);
                    break; //Purple
                case 5:
                    bcol = RGBflip(128*backboxint[0], 16, ((spcoldel+1)*8)*backboxint[0]);
                    break; //Purple
                case 6:
                    bcol = RGBflip(128*backboxint[0], 16, 16);
                    break;  //Red
                case 7:
                    bcol = RGBflip(128*backboxint[0], (128-(spcoldel*8))*backboxint[0], 16);
                    break;  //Red
                case 8:
                    bcol = RGBflip(128*backboxint[0], 128*backboxint[0], 16);
                    break; //Yellow
                case 9:
                    bcol = RGBflip(((spcoldel+1)*8)*backboxint[0], 128*backboxint[0], 16);
                    break; //Yellow
                case 10:
                    bcol = RGBflip(16, 128 * backboxint[0], 16);
                    break;  //Green
                case 11:
                    bcol = RGBflip(16, 128 * backboxint[0], (128-(spcoldel*8))*backboxint[0]);
                    break;  //Green
                }
                break;
            }
            FillRect(backBuffer, backboxes[i], bcol);
            backboxrect.x = backboxes[i].x + 1;
            backboxrect.y = backboxes[i].y + 1;
            backboxrect.w = backboxes[i].w - 2;
            backboxrect.h = backboxes[i].h - 2;
            FillRect(backBuffer,backboxrect, bcol2);

            backboxes[i].x += backboxvx[i];
            backboxes[i].y += backboxvy[i];
            if (backboxes[i].x < -40)
            {
                backboxes[i].x = 320;
                backboxes[i].y = fRandom() * 240;
            }
            if (backboxes[i].x > 320)
            {
                backboxes[i].x = -32;
                backboxes[i].y = fRandom() * 240;
            }
            if (backboxes[i].y < -40)
            {
                backboxes[i].y = 240;
                backboxes[i].x = fRandom() * 320;
            }
            if (backboxes[i].y > 260)
            {
                backboxes[i].y = -32;
                backboxes[i].x = fRandom() * 320;
            }
        }
        break;
    case 3: //Warp zone (horizontal)
        backoffset+=3;
        if (backoffset >= 16) backoffset -= 16;

        if (backgrounddrawn)
        {
            ScrollSurface(towerbuffer, -3, 0 );
            for (int j = 0; j < 15; j++)
            {
                temp = 680 + (rcol * 3);
                drawtowertile(317 - backoffset, (j * 16), temp+40);  //20*16 = 320
                drawtowertile(317 - backoffset + 8, (j * 16), temp + 41);
                drawtowertile(317 - backoffset, (j * 16) + 8, temp + 80);
                drawtowertile(317 - backoffset + 8, (j * 16) + 8, temp + 81);
            }
        }
        else
        {
            //draw the whole thing for the first time!
            backoffset = 0;
            FillRect(towerbuffer, 0x000000);
            for (int j = 0; j < 15; j++)
            {
                for (int i = 0; i < 21; i++)
                {
                    temp = 680 + (rcol * 3);
                    drawtowertile((i * 16) - backoffset - 3, (j * 16), temp+40);
                    drawtowertile((i * 16) - backoffset + 8 - 3, (j * 16), temp + 41);
                    drawtowertile((i * 16) - backoffset - 3, (j * 16) + 8, temp + 80);
                    drawtowertile((i * 16) - backoffset + 8 - 3, (j * 16) + 8, temp + 81);
                }
            }
            backgrounddrawn = true;
        }
        BlitSurfaceStandard(towerbuffer, NULL, backBuffer, NULL);
        break;
    case 4: //Warp zone (vertical)
        backoffset+=3;
        if (backoffset >= 16) backoffset -= 16;

        if (backgrounddrawn)
        {
            ScrollSurface(towerbuffer,0,-3);
            for (int i = 0; i < 21; i++)
            {
                temp = 760 + (rcol * 3);
                drawtowertile((i * 16), 237 - backoffset, temp + 40); //14*17=240 - 3
                drawtowertile((i * 16) + 8, 237 - backoffset, temp + 41);
                drawtowertile((i * 16), 237 - backoffset + 8, temp + 80);
                drawtowertile((i * 16) + 8, 237 - backoffset + 8, temp + 81);
            }
        }
        else
        {
            //draw the whole thing for the first time!
            backoffset = 0;
            FillRect(towerbuffer,0x000000 );
            for (j = 0; j < 15; j++)
            {
                for (int i = 0; i < 21; i++)
                {
                    temp = 760 + (rcol * 3);
                    drawtowertile((i * 16), (j * 16)- backoffset - 3, temp+40);
                    drawtowertile((i * 16)+ 8, (j * 16)- backoffset - 3, temp + 41);
                    drawtowertile((i * 16), (j * 16)- backoffset + 8 - 3, temp + 80);
                    drawtowertile((i * 16)+ 8, (j * 16)- backoffset + 8 - 3, temp + 81);
                }
            }
            backgrounddrawn = true;
        }

        SDL_BlitSurface(towerbuffer,NULL, backBuffer,NULL);
        break;
    case 5:
        //Warp zone, central
        switch(rcol)
        {
            //Akward ordering to match tileset
        case 0:
            warpbcol = RGBflip(0x0A, 0x10, 0x0E);
            warpfcol = RGBflip(0x10, 0x22, 0x21);
            break; //Cyan
        case 1:
            warpbcol = RGBflip(0x11, 0x09, 0x0B);
            warpfcol = RGBflip(0x22, 0x10, 0x11);
            break; //Red
        case 2:
            warpbcol = RGBflip(0x0F, 0x0A, 0x10);
            warpfcol = RGBflip(0x22,0x10,0x22);
            break; //Purple
        case 3:
            warpbcol = RGBflip(0x0A, 0x0B, 0x10);
            warpfcol = RGBflip(0x10, 0x10, 0x22);
            break; //Blue
        case 4:
            warpbcol = RGBflip(0x10, 0x0D, 0x0A);
            warpfcol = RGBflip(0x22, 0x1E, 0x10);
            break; //Yellow
        case 5:
            warpbcol = RGBflip(0x0D, 0x10, 0x0A);
            warpfcol = RGBflip(0x14, 0x22, 0x10);
            break; //Green
        case 6:
            warpbcol = RGBflip(0x0A, 0x0A, 0x0A);
            warpfcol = RGBflip(0x12, 0x12, 0x12);
            break; //Gray
        default:
            warpbcol = RGBflip(0xFF, 0xFF, 0xFF);
            warpfcol = RGBflip(0xFF, 0xFF, 0xFF);
        }

        backoffset += 1;
        if (backoffset >= 16)
        {
            backoffset -= 16;
            warpskip = (warpskip + 1) % 2;
        }

        for (int i = 10 ; i >= 0; i--)
        {
            temp = (i << 4) + backoffset;
            setwarprect(160 - temp, 120 - temp, temp * 2, temp * 2);
            if (i % 2 == warpskip)
            {
                FillRect(backBuffer, warprect, warpbcol);
            }
            else
            {
                FillRect(backBuffer,warprect, warpfcol);
            }
        }
        break;
    case 6:
        //Final Starfield
        FillRect(backBuffer,0x000000);
        for (int i = 0; i < 50; i++)
        {
            if (starsspeed[i] <= 8)
            {
                FillRect(backBuffer, stars[i], getRGB(0x22, 0x22, 0x22));
            }
            else
            {
                FillRect(backBuffer, stars[i], getRGB(0x55, 0x55, 0x55));
            }
            stars[i].y -= starsspeed[i];
            if (stars[i].y < -10)
            {
                stars[i].y += 260;
                stars[i].x = fRandom() * 320;
                starsspeed[i] = 5+(fRandom()*5);
            }
        }
        break;
    case 7:
        //Static, unscrolling section of the tower
        for (int j = 0; j < 30; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                drawtile3(i * 8, j * 8, map.tower.backat(i, j, 200), 15);
            }
        }
        break;
    case 8:
        //Static, unscrolling section of the tower
        for (int j = 0; j < 30; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                drawtile3(i * 8, j * 8, map.tower.backat(i, j, 200), 10);
            }
        }
        break;
    case 9:
        //Static, unscrolling section of the tower
        for (int j = 0; j < 30; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                drawtile3(i * 8, j * 8, map.tower.backat(i, j, 600), 0);
            }
        }
        break;
    default:
        FillRect(backBuffer, 0x000000 );
        BlitSurfaceStandard(backgrounds[t], NULL, backBuffer, &bg_rect);

        break;
    }
}

void Graphics::drawmap()
{
    if (!foregrounddrawn)
    {
        FillRect(foregroundBuffer, 0x00000000);
        if(map.tileset==0)
        {
            for (j = 0; j < 30; j++)
            {
                for (int i = 0; i < 40; i++)
                {
                    if(map.contents[i + map.vmult[j]]>0) drawforetile(i * 8, j * 8, map.contents[i + map.vmult[j]]);
                }
            }
        }
        else if (map.tileset == 1)
        {
            for (int jt = 0; jt < 30; jt++)
            {
                for (int it = 0; it < 40; it++)
                {
                    if(map.contents[it + map.vmult[jt]]>0) drawforetile2(it * 8, jt * 8, map.contents[it + map.vmult[jt]]);
                }
            }
        }
        else if (map.tileset == 2)
        {
            for (j = 0; j < 30; j++)
            {
                for (int i = 0; i < 40; i++)
                {
                    if(map.contents[i + map.vmult[j]]>0) drawforetile3(i * 8, j * 8, map.contents[i + map.vmult[j]],map.rcol);
                }
            }
        }
        foregrounddrawn = true;
    }
    OverlaySurfaceKeyed(foregroundBuffer, backBuffer, 0x00000000);
    //SDL_BlitSurface(foregroundBuffer, NULL, backBuffer, NULL);

}

void Graphics::drawfinalmap()
{
    //Update colour cycling for final level
    if (map.final_colormode) {
        map.final_aniframedelay--;
        if(map.final_aniframedelay==0)
        {
            foregrounddrawn=false;
        }
        if (map.final_aniframedelay <= 0) {
            map.final_aniframedelay = 2;
            map.final_aniframe++;
            if (map.final_aniframe >= 4)
                map.final_aniframe = 0;
        }
    }

    if (!foregrounddrawn) {
        FillRect(foregroundBuffer, 0x00000000);
        if(map.tileset==0){
            for (int j = 0; j < 30; j++) {
                for (int i = 0; i < 40; i++) {
                    if((map.contents[i + map.vmult[j]])>0)
                        drawforetile(i * 8, j * 8, map.finalat(i,j));
                }
            }
        }else if (map.tileset == 1) {
            for (int j = 0; j < 30; j++) {
                for (int i = 0; i < 40; i++) {
                    if((map.contents[i + map.vmult[j]])>0)
                        drawforetile2(i * 8, j * 8, map.finalat(i,j));
                }
            }
        }
        foregrounddrawn=true;
    }

    OverlaySurfaceKeyed(foregroundBuffer, backBuffer, 0x00000000);
}

void Graphics::drawtowermap()
{
    int temp;
    for (int j = 0; j < 31; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            temp = map.tower.at(i, j, map.ypos);
            if (temp > 0) drawtile3(i * 8, (j * 8) - ((int)map.ypos % 8), temp, map.colstate);
        }
    }
}

void Graphics::drawtowermap_nobackground()
{
    int temp;
    for (j = 0; j < 31; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            temp = map.tower.at(i, j, map.ypos);
            if (temp > 0 && temp<28) drawtile3(i * 8, (j * 8) - ((int)map.ypos % 8), temp, map.colstate);
        }
    }
}

void Graphics::drawtowerspikes()
{
    for (int i = 0; i < 40; i++)
    {
        drawtile3(i * 8, -8+map.spikeleveltop, 9, map.colstate);
        drawtile3(i * 8, 230-map.spikelevelbottom, 8, map.colstate);
    }
}

void Graphics::drawtowerbackground()
{
    int temp;

    if (map.bypos < 0) map.bypos += 120 * 8;

    if (map.tdrawback)
    {
        int off = map.scrolldir == 0 ? 0 : map.bscroll;
        //Draw the whole thing; needed for every colour cycle!
        for (int j = 0; j < 31; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                temp = map.tower.backat(i, j, map.bypos);
                drawtowertile3(i * 8, (j * 8) - (map.bypos % 8) - off, temp, map.colstate);
            }
        }

        SDL_BlitSurface(towerbuffer,NULL, backBuffer,NULL);

        map.tdrawback = false;
    }
    else
    {
        //just update the bottom
        ScrollSurface(towerbuffer, 0, -map.bscroll);
        if (map.scrolldir == 0)
        {
            for (int i = 0; i < 40; i++)
            {
                temp = map.tower.backat(i, 0, map.bypos);
                drawtowertile3(i * 8, -(map.bypos % 8), temp, map.colstate);
            }
        }
        else
        {
            for (int i = 0; i < 40; i++)
            {
                temp = map.tower.backat(i, 29, map.bypos);
                drawtowertile3(i * 8, 29*8 - (map.bypos % 8) - map.bscroll, temp, map.colstate);
                temp = map.tower.backat(i, 30, map.bypos);
                drawtowertile3(i * 8, 30*8 - (map.bypos % 8) - map.bscroll, temp, map.colstate);
                temp = map.tower.backat(i, 31, map.bypos);
                drawtowertile3(i * 8, 31*8 - (map.bypos % 8) - map.bscroll, temp, map.colstate);
            }
        }

        SDL_BlitSurface(towerbuffer,NULL, backBuffer,NULL);
    }
}

void Graphics::setcol( int t )
{
	int temp;

	//Setup predefinied colours as per our zany palette
	switch(t)
	{
		//Player Normal
	case 0:
		ct.colour = getRGB(160- help.glow/2 - (fRandom()*20), 200- help.glow/2, 220 - help.glow);
		break;
		//Player Hurt
	case 1:
		ct.colour = getRGB(196 - (fRandom() * 64), 10, 10);
		break;
		//Enemies and stuff
	case 2:
		ct.colour = getRGB(225-(help.glow/2), 75, 30);
		break;
	case 3: //Trinket
		if (!trinketcolset)
		{
			trinketr = 200 - (fRandom() * 64);
			trinketg = 200 - (fRandom() * 128);
			trinketb = 164 + (fRandom() * 60);
			trinketcolset = true;
		}
		ct.colour = getRGB(trinketr, trinketg, trinketb);
		break;
	case 4: //Inactive savepoint
		temp = (help.glow/2) + (fRandom() * 8);
		ct.colour = getRGB(80 + temp, 80 + temp, 80 + temp);
		break;
	case 5: //Active savepoint
		ct.colour = getRGB(164+(fRandom()*64),164+(fRandom()*64), 255-(fRandom()*64));
		break;
	case 6: //Enemy : Red
		ct.colour = getRGB(250 - help.glow/2, 60- help.glow/2, 60 - help.glow/2);
		break;
	case 7: //Enemy : Green
		ct.colour = getRGB(100 - help.glow/2 - (fRandom()*30), 250 - help.glow/2, 100 - help.glow/2 - (fRandom()*30));
		break;
	case 8: //Enemy : Purple
		ct.colour = getRGB(250 - help.glow/2, 20, 128 - help.glow/2 + (fRandom()*30));
		break;
	case 9: //Enemy : Yellow
		ct.colour = getRGB(250 - help.glow/2, 250 - help.glow/2, 20);
		break;
	case 10: //Warp point (white)
		ct.colour = getRGB(255 - (fRandom() * 64), 255 - (fRandom() * 64), 255 - (fRandom() * 64));
		break;
	case 11: //Enemy : Cyan
		ct.colour = getRGB(20, 250 - help.glow/2, 250 - help.glow/2);
		break;
	case 12: //Enemy : Blue
		ct.colour = getRGB(90- help.glow/2, 90 - help.glow/2, 250 - help.glow/2);
		break;
		//Crew Members
		//green
	case 13:
		ct.colour = getRGB(120- help.glow/4 - (fRandom()*20), 220 - help.glow/4, 120- help.glow/4);
		break;
		//Yellow
	case 14:
		ct.colour = getRGB(220- help.glow/4 - (fRandom()*20), 210 - help.glow/4, 120- help.glow/4);
		break;
		//pink
	case 15:
		ct.colour = getRGB(255 - help.glow/8, 70 - help.glow/4, 70 - help.glow / 4);
		break;
		//Blue
	case 16:
		ct.colour = getRGB(75, 75, 255- help.glow/4 - (fRandom()*20));
		break;


	case 17: //Enemy : Orange
		ct.colour = getRGB(250 - help.glow/2, 130 - help.glow/2, 20);
		break;
	case 18: //Enemy : Gray
		ct.colour = getRGB(130- help.glow/2, 130 - help.glow/2, 130 - help.glow/2);
		break;
	case 19: //Enemy : Dark gray
		ct.colour = getRGB(60- help.glow/8, 60 - help.glow/8, 60 - help.glow/8);
		break;
		//Purple
	case 20:
		ct.colour = getRGB(220 - help.glow / 4 - (fRandom() * 20), 120 - help.glow / 4, 210 - help.glow / 4);
		break;

	case 21: //Enemy : Light Gray
		ct.colour = getRGB(180- help.glow/2, 180 - help.glow/2, 180 - help.glow/2);
		break;
	case 22: //Enemy : Indicator Gray
		ct.colour = getRGB(230- help.glow/2, 230- help.glow/2, 230- help.glow/2);
		break;
	case 23: //Enemy : Indicator Gray
		ct.colour = getRGB(255- help.glow/2 - (fRandom() * 40) , 255- help.glow/2 - (fRandom() * 40), 255- help.glow/2 - (fRandom() * 40));
		break;

		//Trophies
		//cyan
	case 30:
		ct.colour = RGBf(160, 200, 220);
		break;
		//Purple
	case 31:
		ct.colour = RGBf(220, 120, 210);
		break;
		//Yellow
	case 32:
		ct.colour = RGBf(220, 210, 120);
		break;
		//red
	case 33:
		ct.colour = RGBf(255, 70, 70);
		break;
		//green
	case 34:
		ct.colour = RGBf(120, 220, 120);
		break;
		//Blue
	case 35:
		ct.colour = RGBf(75, 75, 255);
		break;
		//Gold
	case 36:
		ct.colour = getRGB(180, 120, 20);
		break;
	case 37: //Trinket
		if (!trinketcolset)
		{
			trinketr = 200 - (fRandom() * 64);
			trinketg = 200 - (fRandom() * 128);
			trinketb = 164 + (fRandom() * 60);
			trinketcolset = true;
		}
		ct.colour = RGBf(trinketr, trinketg, trinketb);
		break;
		//Silver
	case 38:
		ct.colour = RGBf(196, 196, 196);
		break;
		//Bronze
	case 39:
		ct.colour = RGBf(128, 64, 10);
		break;
		//Awesome
	case 40: //Teleporter in action!
		temp = fRandom() * 150;
		if(temp<33)
		{
			ct.colour = RGBf(255 - (fRandom() * 64), 64 + (fRandom() * 64), 64 + (fRandom() * 64));
		}
		else if (temp < 66)
		{
			ct.colour = RGBf(64 + (fRandom() * 64), 255 - (fRandom() * 64), 64 + (fRandom() * 64));
		}
		else if (temp < 100)
		{
			ct.colour = RGBf(64 + (fRandom() * 64), 64 + (fRandom() * 64), 255 - (fRandom() * 64));
		}
		else
		{
			ct.colour = RGBf(164+(fRandom()*64),164+(fRandom()*64), 255-(fRandom()*64));
		}
		break;

	case 100: //Inactive Teleporter
		temp = (help.glow/2) + (fRandom() * 8);
		ct.colour = getRGB(42 + temp, 42 + temp, 42 + temp);
		break;
	case 101: //Active Teleporter
		ct.colour = getRGB(164+(fRandom()*64),164+(fRandom()*64), 255-(fRandom()*64));
		break;
	case 102: //Teleporter in action!
		temp = fRandom() * 150;
		if(temp<33)
		{
			ct.colour = getRGB(255 - (fRandom() * 64), 64 + (fRandom() * 64), 64 + (fRandom() * 64));
		}
		else if (temp < 66)
		{
			ct.colour = getRGB(64 + (fRandom() * 64), 255 - (fRandom() * 64), 64 + (fRandom() * 64));
		}
		else if (temp < 100)
		{
			ct.colour = getRGB(64 + (fRandom() * 64), 64 + (fRandom() * 64), 255 - (fRandom() * 64));
		}
		else
		{
			ct.colour = getRGB(164+(fRandom()*64),164+(fRandom()*64), 255-(fRandom()*64));
		}
		break;

	default:
		ct.colour = 0xFFFFFF;
		break;
	}
}

void Graphics::menuoffrender()
{
	SDL_Rect offsetRect1;
	setRect (offsetRect1, 0, 0, backBuffer->w ,backBuffer->h);

	//put the back buffer in the menubuffer
	BlitSurfaceStandard(backBuffer, NULL, menubuffer, NULL);



	if(flipmode)
	{
		//	flipmatrix.translate(0, menuoffset);
		//	screenbuffer.draw(backbuffer, flipmatrix);
		//	flipmatrix.translate(0, -menuoffset);
		SDL_Surface* tempbufferFlipped = FlipSurfaceVerticle(tempBuffer);
		//put the stored backbuffer in the backbuffer.
		SDL_FillRect(backBuffer, NULL, 0x00000000);
		BlitSurfaceStandard(tempbufferFlipped, NULL, backBuffer, NULL);
		SDL_FreeSurface(tempbufferFlipped);
		SDL_Rect offsetRect;
		setRect (offsetRect, 0, menuoffset, backBuffer->w ,backBuffer->h);
		SDL_Surface* temp = FlipSurfaceVerticle(menubuffer);
		BlitSurfaceStandard(temp,NULL,backBuffer,&offsetRect);
		SDL_FreeSurface(temp);
	}
	else
	{
		//put the stored backbuffer in the backbuffer.
		BlitSurfaceStandard(tempBuffer, NULL, backBuffer, NULL);

		SDL_Rect offsetRect;
		setRect (offsetRect, 0, menuoffset, backBuffer->w ,backBuffer->h);
		BlitSurfaceStandard(menubuffer,NULL,backBuffer,&offsetRect);
	}

	SDL_Rect rect;
	setRect(rect, 0, 0, backBuffer->w, backBuffer->h);
	screenbuffer->UpdateScreen(backBuffer,&rect);
	FillRect(backBuffer, 0x000000);
}

void Graphics::drawhuetile( int x, int y, int t, int c )
{
	point tpoint;
	tpoint.x = x;
	tpoint.y = y;
	switch(c)
	{
	case 0:
		setcolreal(getRGB(250-int(fRandom()*32), 250-int(fRandom()*32), 10));
		break;
	case 1:
		setcolreal(getRGB(250-int(fRandom()*32), 250-int(fRandom()*32), 10));
		break;
	default:
		setcolreal(getRGB(250-int(fRandom()*32), 250-int(fRandom()*32),  10));
		break;
	}


	SDL_Rect rect;
	setRect(rect,tpoint.x,tpoint.y,tiles_rect.w, tiles_rect.h);
	BlitSurfaceColoured(tiles[t],NULL,backBuffer, &rect, ct);
}

void Graphics::setwarprect( int a, int b, int c, int d )
{
	warprect.x = a;
	warprect.y = b;
	warprect.w = c;
	warprect.h = d;
}

	void Graphics::textboxcleanup()
	{
	}

void Graphics::textboxcenter()
{
	textbox[m].centerx();
	textbox[m].centery();
}

void Graphics::textboxcenterx()
{
	textbox[m].centerx();
}

int Graphics::textboxwidth()
{
	return textbox[m].w;
}

void Graphics::textboxmove(int xo, int yo)
{
	textbox[m].xp += xo;
	textbox[m].yp += yo;
}

void Graphics::textboxmoveto(int xo)
{
	textbox[m].xp = xo;
}

void Graphics::textboxcentery()
{
	textbox[m].centery();
}

int Graphics::crewcolour(const int t)
{
	//given crewmate t, return colour in setcol
	if (t == 0) return 0;
	if (t == 1) return 20;
	if (t == 2) return 14;
	if (t == 3) return 15;
	if (t == 4) return 13;
	if (t == 5) return 16;
	return 0;
}

void Graphics::flashlight()
{
	FillRect(backBuffer, 0xBBBBBBBB);
}

void Graphics::screenshake()
{
	point tpoint;
	if(flipmode)
	{
		//	tpoint.x = int((Math.random() * 7) - 4); tpoint.y = int((Math.random() * 7) - 4);
		//	flipmatrix.translate(tpoint.x, tpoint.y);
		//	screenbuffer.draw(backbuffer, flipmatrix);
		//	flipmatrix.translate(-tpoint.x, -tpoint.y);

		tpoint.x =  (fRandom() * 7) - 4;
		tpoint.y =  (fRandom() * 7) - 4;
		SDL_Rect shakeRect;
		setRect(shakeRect,tpoint.x, tpoint.y, backBuffer->w, backBuffer->h);
		SDL_Surface* flipBackBuffer = FlipSurfaceVerticle(backBuffer);
		screenbuffer->UpdateScreen( flipBackBuffer, &shakeRect);
		SDL_FreeSurface(flipBackBuffer);
	}
	else
	{
		//FillRect(screenbuffer, 0x000);
		//SDL_Rect rect;
		//setRect(rect, blackBars/2, 0, screenbuffer->w, screenbuffer->h);
		//SDL_BlitSurface(backBuffer, NULL, screenbuffer, &rect);
		tpoint.x =  static_cast<Sint32>((fRandom() * 7) - 4);
		tpoint.y =  static_cast<Sint32>((fRandom() * 7) - 4);
		SDL_Rect shakeRect;
		setRect(shakeRect,tpoint.x, tpoint.y, backBuffer->w, backBuffer->h);
		screenbuffer->UpdateScreen( backBuffer, &shakeRect);
	}

	FillRect(backBuffer, 0x000000 );
}

void Graphics::render()
{
	if(screenbuffer == NULL)
	{
		return;
	}
	if(flipmode)
	{
		SDL_Rect rect;
		setRect(rect, 0, 0, backBuffer->w, backBuffer->h);
		//setRect(rect, 0, 0, backBuffer->w, backBuffer->h);
		//SDL_BlitSurface(backBuffer, NULL, screenbuffer, &rect);
		SDL_Surface* tempsurface = FlipSurfaceVerticle(backBuffer);
		if(tempsurface != NULL)
		{
			screenbuffer->UpdateScreen( tempsurface, &rect);
			SDL_FreeSurface(tempsurface);
		}
	}
	else
	{
		SDL_Rect rect;
		setRect(rect, 0, 0, backBuffer->w, backBuffer->h);
		//setRect(rect, 0, 0, backBuffer->w, backBuffer->h);
		//SDL_BlitSurface(backBuffer, NULL, screenbuffer, &rect);
		screenbuffer->UpdateScreen( backBuffer, &rect);
	}
}

void Graphics::renderwithscreeneffects()
{
	if (game.flashlight > 0 && !game.noflashingmode)
	{
		graphics.flashlight();
	}

	if (game.screenshake > 0 && !game.noflashingmode)
	{
		graphics.screenshake();
	}
	else
	{
		graphics.render();
	}
}

void Graphics::bigrprint(int x, int y, std::string& t, int r, int g, int b, bool cen, float sc)
{
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	ct.colour = getRGB(r, g, b);

	x = x /  (sc);

	x -= (len(t));

	if (r < -1) r = -1;
	if (g < 0) g = 0;
	if (b < 0) b = 0;
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	ct.colour = getRGB(r, g, b);

	if (cen)
	{
		x = std::max(160 - (int((len(t)/ 2.0)*sc)), 0 );
	}
	else
	{
		x *=  (sc);
	}

	int bfontpos = 0;
	int cur;
	std::string::iterator iter = t.begin();
	while (iter != t.end()) {
		cur = utf8::unchecked::next(iter);
		if (flipmode)
		{
			SDL_Surface* tempPrint = ScaleSurfaceSlow(flipbfont[font_idx(cur)], bfont[font_idx(cur)]->w *sc,bfont[font_idx(cur)]->h *sc);
			SDL_Rect printrect = { Sint16(x + bfontpos), Sint16(y) , Sint16(bfont_rect.w*sc), Sint16(bfont_rect.h * sc)};
			BlitSurfaceColoured(tempPrint, NULL, backBuffer, &printrect ,ct);
			SDL_FreeSurface(tempPrint);
		}
		else
		{
			SDL_Surface* tempPrint = ScaleSurfaceSlow(bfont[font_idx(cur)], bfont[font_idx(cur)]->w *sc,bfont[font_idx(cur)]->h *sc);
			SDL_Rect printrect = { Sint16((x) + bfontpos), Sint16(y) , Sint16(bfont_rect.w*sc), Sint16(bfont_rect.h * sc)};
			BlitSurfaceColoured(tempPrint, NULL, backBuffer, &printrect, ct);
			SDL_FreeSurface(tempPrint);
		}
		bfontpos+=bfontlen(cur)* sc;
	}
}

void Graphics::drawtele(int x, int y, int t, int c)
{
	setcolreal(getRGB(16,16,16));

	SDL_Rect telerect;
	setRect(telerect, x , y, tele_rect.w, tele_rect.h );
	BlitSurfaceColoured(tele[0], NULL, backBuffer, &telerect, ct);

	setcol(c);
	if (t > 9) t = 8;
	if (t < 0) t = 0;

	setRect(telerect, x , y, tele_rect.w, tele_rect.h );
	BlitSurfaceColoured(tele[t], NULL, backBuffer, &telerect, ct);
}

Uint32 Graphics::getRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	return SDL_MapRGBA(backBuffer->format, b, g, r, a);
}

Uint32 Graphics::getRGB(Uint8 r, Uint8 g, Uint8 b)
{
	return SDL_MapRGB(backBuffer->format, b, g, r);
}

Uint32 Graphics::getBGR(Uint8 r, Uint8 g, Uint8 b)
{
	return SDL_MapRGB(backBuffer->format, r, g, b);
}

Uint32 Graphics::getRGB(Uint32 _col)
{
	return ( _col);
}

Uint32 Graphics::RGBflip(Uint8  r, Uint8  g, Uint8  b)
{
	return SDL_MapRGB(backBuffer->format, r, g, b);
}

Uint32 Graphics::RGBf(int r, int g, int b)
{
	r = (r+128) / 3;
	g = (g+128) / 3;
	b = (b+128) / 3;
	return SDL_MapRGB(backBuffer->format, r, g, b);
}

void Graphics::setcolreal(Uint32 t)
{
	ct.colour = t;
}

void Graphics::drawforetile(int x, int y, int t)
{
	SDL_Rect rect;
	setRect(rect, x,y,tiles_rect.w, tiles_rect.h);
	BlitSurfaceStandard(tiles[t],NULL, foregroundBuffer, &rect  );
}

void Graphics::drawforetile2(int x, int y, int t)
{
	SDL_Rect rect;
	setRect(rect, x,y,tiles_rect.w, tiles_rect.h);
	BlitSurfaceStandard(tiles2[t],NULL, foregroundBuffer, &rect  );
}

void Graphics::drawforetile3(int x, int y, int t, int off)
{
	SDL_Rect rect;
	setRect(rect, x,y,tiles_rect.w, tiles_rect.h);
	BlitSurfaceStandard(tiles3[t+(off*30)],NULL, foregroundBuffer, &rect  );
}

void Graphics::drawrect(int x, int y, int w, int h, int r, int g, int b)
{
	SDL_Rect madrect;
	//Draw the retangle indicated by that object
	madrect.x = x;
	madrect.y = y;
	madrect.w = w;
	madrect.h = 1;
	FillRect(backBuffer, madrect, getRGB(b,g,r));

	madrect.w = 1;
	madrect.h = h;
	FillRect(backBuffer, madrect, getRGB(b,g,r));

	madrect.x = x + w - 1;
	madrect.w = 1;
	madrect.h = h;
	FillRect(backBuffer, madrect, getRGB(b,g,r));
	madrect.x = x;
	madrect.y = y + h - 1;
	madrect.w = w;
	madrect.h = 1;
	FillRect(backBuffer, madrect, getRGB(b,g,r));
}

bool Graphics::onscreen(int t)
{
	return (t >= -40 && t <= 280);
}

void Graphics::reloadresources() {
	grphx.destroy();
	grphx = GraphicsResources();
	grphx.init();

	#define CLEAR_ARRAY(name) \
		for (size_t i = 0; i < name.size(); i += 1) \
		{ \
			SDL_FreeSurface(name[i]); \
		} \
		name.clear();

	CLEAR_ARRAY(tiles)
	CLEAR_ARRAY(tiles2)
	CLEAR_ARRAY(tiles3)
	CLEAR_ARRAY(entcolours)
	CLEAR_ARRAY(sprites)
	CLEAR_ARRAY(flipsprites)
	CLEAR_ARRAY(tele)
	CLEAR_ARRAY(bfont)
	CLEAR_ARRAY(flipbfont)

	#undef CLEAR_ARRAY

	MakeTileArray();
	MakeSpriteArray();
	maketelearray();
	Makebfont();

	images.clear();

	images.push_back(grphx.im_image0);
	images.push_back(grphx.im_image1);
	images.push_back(grphx.im_image2);
	images.push_back(grphx.im_image3);
	images.push_back(grphx.im_image4);
	images.push_back(grphx.im_image5);
	images.push_back(grphx.im_image6);

	images.push_back(grphx.im_image7);
	images.push_back(grphx.im_image8);
	images.push_back(grphx.im_image9);
	images.push_back(grphx.im_image10);
	images.push_back(grphx.im_image11);
	images.push_back(grphx.im_image12);

	music.init();
}
