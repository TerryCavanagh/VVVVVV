#define GRAPHICS_DEFINITION
#include "Graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <utf8/unchecked.h>

#include "editor.h"
#include "Entity.h"
#include "FileSystemUtils.h"
#include "Map.h"
#include "Music.h"
#include "Screen.h"
#include "UtilityClass.h"

void Graphics::init()
{
    grphx.init();

    flipmode = false;
    setRect(tiles_rect, 0,0,8,8);
    setRect(sprites_rect, 0,0,32,32);
    setRect(bfont_rect, 0,0,8,8);
    setRect(bg_rect, 0,0,320,240);
    setRect(footerrect, 0, 230, 320, 10);
    setRect(prect, 0, 0, 4, 4);
    setRect(line_rect, 0,0,0,0);
    setRect(tele_rect,0,0,96,96);
    setRect(towerbuffer_rect, 8, 8, 320, 240);


    //We initialise a few things

    linestate = 0;


    trinketcolset = false;

    showcutscenebars = false;
    cutscenebarspos = 0;
    oldcutscenebarspos = 0;
    notextoutline = false;

    flipmode = false;
    setflipmode = false;

    //Background inits
    for (int i = 0; i < numstars; i++)
    {
        SDL_Rect s = {Sint16(fRandom() * 320), Sint16(fRandom() * 240), 2, 2};
        int s2 = 4+(fRandom()*4);
        stars[i] = s;
        starsspeed[i] = s2;
    }

    for (int i = 0; i < numbackboxes; i++)
    {
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
        backboxes[i] = bb;
        backboxvx[i] = bvx;
        backboxvy[i] = bvy;
        backboxint[i] = bint;
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
    oldmenuoffset = 0;
    resumegamemode = false;

    //Fading stuff
    SDL_memset(fadebars, 0, sizeof(fadebars));

    fadeamount = 0;
    oldfadeamount = 0;
    fademode = 0;

    // initialize everything else to zero
    backBuffer = NULL;
    ct = colourTransform();
    foot_rect = SDL_Rect();
    foregrounddrawn = false;
    foregroundBuffer = NULL;
    backgrounddrawn = false;
    images_rect = SDL_Rect();
    m = 0;
    linedelay = 0;
    menubuffer = NULL;
    screenbuffer = NULL;
    tempBuffer = NULL;
    warpbuffer = NULL;
    warpbuffer_lerp = NULL;
    footerbuffer = NULL;
    ghostbuffer = NULL;
    towerbg = TowerBG();
    titlebg = TowerBG();
    trinketr = 0;
    trinketg = 0;
    trinketb = 0;
    warprect = SDL_Rect();

    translucentroomname = false;
    showmousecursor = true;

    alpha = 1.0f;

    screenshake_x = 0;
    screenshake_y = 0;

    col_crewred = 0x00000000;
    col_crewyellow = 0x00000000;
    col_crewgreen = 0x00000000;
    col_crewcyan = 0x00000000;
    col_crewblue = 0x00000000;
    col_crewpurple = 0x00000000;
    col_crewinactive = 0x00000000;
    col_clock = 0x00000000;
    col_trinket = 0x00000000;
    col_tr = 0;
    col_tg = 0;
    col_tb = 0;

    kludgeswnlinewidth = false;
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

void Graphics::drawspritesetcol(int x, int y, int t, int c)
{
    if (!INBOUNDS_VEC(t, sprites))
    {
        return;
    }
    SDL_Rect rect;
    setRect(rect,x,y,sprites_rect.w,sprites_rect.h);
    setcol(c);

    BlitSurfaceColoured(sprites[t],NULL,backBuffer, &rect, ct);
}

void Graphics::updatetitlecolours()
{
    setcol(15);
    col_crewred = ct.colour;
    setcol(14);
    col_crewyellow = ct.colour;
    setcol(13);
    col_crewgreen = ct.colour;
    setcol(0);
    col_crewcyan = ct.colour;
    setcol(16);
    col_crewblue = ct.colour;
    setcol(20);
    col_crewpurple = ct.colour;
    setcol(19);
    col_crewinactive = ct.colour;

    setcol(18);
    col_clock = ct.colour;
    setcol(18);
    col_trinket = ct.colour;
}

#define PROCESS_TILESHEET_CHECK_ERROR(tilesheet, tile_square) \
    if (grphx.im_##tilesheet->w % tile_square != 0 \
    || grphx.im_##tilesheet->h % tile_square != 0) \
    { \
        const char* error = "Error: %s.png dimensions not exact multiples of %i!"; \
        char message[128]; \
        SDL_snprintf(message, sizeof(message), error, #tilesheet, tile_square); \
        \
        const char* error_title = "Error with %s.png"; \
        char message_title[128]; \
        SDL_snprintf(message_title, sizeof(message_title), error_title, #tilesheet); \
        \
        puts(message); \
        \
        SDL_ShowSimpleMessageBox( \
            SDL_MESSAGEBOX_ERROR, \
            message_title, \
            message, \
            NULL \
        ); \
        \
        exit(1); \
    }

#define PROCESS_TILESHEET_RENAME(tilesheet, vector, tile_square, extra_code) \
    PROCESS_TILESHEET_CHECK_ERROR(tilesheet, tile_square) \
    \
    for (int j = 0; j < grphx.im_##tilesheet->h / tile_square; j++) \
    { \
        for (int i = 0; i < grphx.im_##tilesheet->w / tile_square; i++) \
        { \
            SDL_Surface* temp = GetSubSurface( \
                grphx.im_##tilesheet, \
                i * tile_square, j * tile_square, \
                tile_square, tile_square \
            ); \
            vector.push_back(temp); \
            \
            extra_code \
        } \
    }

#define PROCESS_TILESHEET(tilesheet, tile_square, extra_code) \
    PROCESS_TILESHEET_RENAME(tilesheet, tilesheet, tile_square, extra_code)

void Graphics::Makebfont()
{
    PROCESS_TILESHEET(bfont, 8,
    {
        SDL_Surface* TempFlipped = FlipSurfaceVerticle(temp);
        flipbfont.push_back(TempFlipped);
    })

    unsigned char* charmap = NULL;
    size_t length;
    FILESYSTEM_loadFileToMemory("graphics/font.txt", &charmap, &length);
    if (charmap != NULL)
    {
        unsigned char* current = charmap;
        unsigned char* end = charmap + length;
        int pos = 0;
        while (current != end)
        {
            int codepoint = utf8::unchecked::next(current);
            font_positions[codepoint] = pos;
            ++pos;
        }
        FILESYSTEM_freeMemory(&charmap);
    }
    else
    {
        font_positions.clear();
    }
}

int Graphics::bfontlen(uint32_t ch)
{
    if (ch < 32)
    {
        return 6;
    }
    else
    {
        return 8;
    }
}

void Graphics::MakeTileArray()
{
    PROCESS_TILESHEET(tiles, 8, )
    PROCESS_TILESHEET(tiles2, 8, )
    PROCESS_TILESHEET(tiles3, 8, )
    PROCESS_TILESHEET(entcolours, 8, )
}

void Graphics::maketelearray()
{
    PROCESS_TILESHEET_RENAME(teleporter, tele, 96, )
}

void Graphics::MakeSpriteArray()
{
    PROCESS_TILESHEET(sprites, 32, )
    PROCESS_TILESHEET(flipsprites, 32, )
}

#undef PROCESS_TILESHEET
#undef PROCESS_TILESHEET_RENAME
#undef PROCESS_TILESHEET_CHECK_ERROR


void Graphics::map_tab(int opt, const std::string& text, bool selected /*= false*/)
{
    int x = opt*80 + 40 - len(text)/2;
    if (selected)
    {
        Print(x-8, 220, "[" + text + "]", 196, 196, 255 - help.glow);
    }
    else
    {
        Print(x, 220, text, 64, 64, 64);
    }
}

void Graphics::map_option(int opt, int num_opts, const std::string& text, bool selected /*= false*/)
{
    int x = 80 + opt*32;
    int y = 136; // start from middle of menu

    int yoff = -(num_opts * 12) / 2; // could be simplified to -num_opts * 6, this conveys my intent better though
    yoff += opt * 12;

    if (flipmode)
    {
        y -= yoff; // going down, which in Flip Mode means going up
        y -= 40;
    }
    else
    {
        y += yoff; // going up
    }

    if (selected)
    {
        std::string text_upper(text);
        for (size_t i = 0; i < text_upper.length(); i++)
        {
            text_upper[i] = SDL_toupper(text_upper[i]);
        }
        Print(x - 16, y, "[ " + text_upper + " ]", 196, 196, 255 - help.glow);
    }
    else
    {
        Print(x, y, text, 96, 96, 96);
    }
}

void Graphics::Print( int _x, int _y, std::string _s, int r, int g, int b, bool cen /*= false*/ ) {
    return PrintAlpha(_x,_y,_s,r,g,b,255,cen);
}

void Graphics::PrintAlpha( int _x, int _y, std::string _s, int r, int g, int b, int a, bool cen /*= false*/ )
{
    std::vector<SDL_Surface*>& font = flipmode ? flipbfont : bfont;

    r = clamp(r,0,255);
    g = clamp(g,0,255);
    b = clamp(b,0,255);
    a = clamp(a,0,255);

    ct.colour = getRGBA(r, g, b, a);

    if (cen)
        _x = ((160 ) - ((len(_s)) / 2));
    int bfontpos = 0;
    int curr;
    int idx;
    std::string::iterator iter = _s.begin();
    while (iter != _s.end()) {
        curr = utf8::unchecked::next(iter);
        point tpoint;
        tpoint.x = _x + bfontpos;
        tpoint.y = _y;

        SDL_Rect fontRect = bfont_rect;
        fontRect.x = tpoint.x ;
        fontRect.y = tpoint.y ;

        idx = font_idx(curr);
        if (INBOUNDS_VEC(idx, font))
        {
            BlitSurfaceColoured( font[idx], NULL, backBuffer, &fontRect , ct);
        }
        bfontpos+=bfontlen(curr) ;
    }
}


void Graphics::bigprint(  int _x, int _y, std::string _s, int r, int g, int b, bool cen, int sc )
{
    std::vector<SDL_Surface*>& font = flipmode ? flipbfont : bfont;

    r = clamp(r,0,255);
    g = clamp(g,0,255);
    b = clamp(b,0,255);

    ct.colour = getRGB(r, g, b);

    if (cen)
    {
        _x = VVV_max(160 - (int((len(_s)/ 2.0)*sc)), 0 );
    }

    int bfontpos = 0;
    int curr;
    int idx;
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

        idx = font_idx(curr);
        if (INBOUNDS_VEC(idx, font))
        {
            SDL_Surface* tempPrint = ScaleSurface(font[idx], font[idx]->w *sc,font[idx]->h *sc);
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

void Graphics::PrintOffAlpha( int _x, int _y, std::string _s, int r, int g, int b, int a, bool cen /*= false*/ )
{
    std::vector<SDL_Surface*>& font = flipmode ? flipbfont : bfont;

    r = clamp(r,0,255);
    g = clamp(g,0,255);
    b = clamp(b,0,255);
    a = clamp(a,0,255);

    ct.colour = getRGB(r, g, b);

    if (cen)
        _x = ((160) - (len(_s) / 2))+_x;
    int bfontpos = 0;
    int idx;
    std::string::iterator iter = _s.begin();
    while (iter != _s.end()) {
        int curr = utf8::unchecked::next(iter);
        point tpoint;
        tpoint.x = _x + bfontpos;
        tpoint.y = _y;

        SDL_Rect fontRect = bfont_rect;
        fontRect.x = tpoint.x ;
        fontRect.y = tpoint.y ;

        idx = font_idx(curr);
        if (INBOUNDS_VEC(idx, font))
        {
            BlitSurfaceColoured( font[idx], NULL, backBuffer, &fontRect , ct);
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
    if (!INBOUNDS_VEC(t, sprites))
    {
        WHINE_ONCE("drawsprite() out-of-bounds!");
    }

    SDL_Rect rect = { Sint16(x), Sint16(y), sprites_rect.w, sprites_rect.h };
    setcolreal(getRGB(r,g,b));
    BlitSurfaceColoured(sprites[t], NULL, backBuffer, &rect, ct);
}

void Graphics::drawsprite(int x, int y, int t, Uint32 c)
{
    if (!INBOUNDS_VEC(t, sprites))
    {
        WHINE_ONCE("drawsprite() out-of-bounds!");
    }

    SDL_Rect rect = { Sint16(x), Sint16(y), sprites_rect.w, sprites_rect.h };
    setcolreal(c);
    BlitSurfaceColoured(sprites[t], NULL, backBuffer, &rect, ct);
}

void Graphics::drawtile( int x, int y, int t )
{
    if (!INBOUNDS_VEC(t, tiles))
    {
        WHINE_ONCE("drawtile() out-of-bounds!")
        return;
    }

    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };

#if !defined(NO_CUSTOM_LEVELS)
    if (t >= 14 && t <= 17 && (!FILESYSTEM_assetsmounted || ed.onewaycol_override))
    {
        colourTransform thect = {ed.getonewaycol()};
        BlitSurfaceTinted(tiles[t], NULL, backBuffer, &rect, thect);
    }
    else
#endif
    {
        BlitSurfaceStandard(tiles[t], NULL, backBuffer, &rect);
    }
}


void Graphics::drawtile2( int x, int y, int t )
{
    if (!INBOUNDS_VEC(t, tiles2))
    {
        WHINE_ONCE("drawtile2() out-of-bounds!")
        return;
    }

    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };

#if !defined(NO_CUSTOM_LEVELS)
    if (t >= 14 && t <= 17 && (!FILESYSTEM_assetsmounted || ed.onewaycol_override))
    {
        colourTransform thect = {ed.getonewaycol()};
        BlitSurfaceTinted(tiles2[t], NULL, backBuffer, &rect, thect);
    }
    else
#endif
    {
        BlitSurfaceStandard(tiles2[t], NULL, backBuffer, &rect);
    }
}



void Graphics::drawtile3( int x, int y, int t, int off, int height_subtract /*= 0*/ )
{
    t += off * 30;
    if (!INBOUNDS_VEC(t, tiles3))
    {
        WHINE_ONCE("drawtile3() out-of-bounds!")
        return;
    }
    SDL_Rect src_rect = { 0, 0, tiles_rect.w, tiles_rect.h - height_subtract };
    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };
    BlitSurfaceStandard(tiles3[t], &src_rect, backBuffer, &rect);
}

void Graphics::drawtowertile( int x, int y, int t )
{
    if (!INBOUNDS_VEC(t, tiles2))
    {
        WHINE_ONCE("drawtowertile() out-of-bounds!")
        return;
    }
    x += 8;
    y += 8;
    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };
    BlitSurfaceStandard(tiles2[t], NULL, warpbuffer, &rect);
}


void Graphics::drawtowertile3( int x, int y, int t, TowerBG& bg_obj )
{
    t += bg_obj.colstate*30;
    if (!INBOUNDS_VEC(t, tiles3))
    {
        WHINE_ONCE("drawtowertile3() out-of-bounds!")
        return;
    }
    x += 8;
    y += 8;
    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };
    BlitSurfaceStandard(tiles3[t], NULL, bg_obj.buffer, &rect);
}

void Graphics::drawgui()
{
    //Draw all the textboxes to the screen
    for (size_t i = 0; i<textbox.size(); i++)
    {
        //This routine also updates textbox colors
        float tl_lerp = lerp(textbox[i].prev_tl, textbox[i].tl);
        textbox[i].setcol(textbox[i].tr * tl_lerp, textbox[i].tg * tl_lerp, textbox[i].tb * tl_lerp);

        if (textbox[i].r == 0 && textbox[i].g == 0 && textbox[i].b == 0)
        {
            if(flipmode)
            {
                for (size_t j = 0; j < textbox[i].line.size(); j++)
                {
                    Print(textbox[i].xp + 8, textbox[i].yp + (textbox[i].line.size()*8) - (j * 8), textbox[i].line[j], 196, 196, 255 - help.glow);
                }
            }
            else
            {
                for (size_t j = 0; j < textbox[i].line.size(); j++)
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
                for (size_t j = 0; j < textbox[i].line.size(); j++)
                {
                    Print(textbox[i].xp + 8, textbox[i].yp  + (textbox[i].line.size()*8) - (j * 8), textbox[i].line[j], textbox[i].r, textbox[i].g, textbox[i].b);
                }
            }
            else
            {
                for (size_t j = 0; j < textbox[i].line.size(); j++)
                {
                    Print(textbox[i].xp + 8, textbox[i].yp +8 + (j * 8), textbox[i].line[j], textbox[i].r, textbox[i].g, textbox[i].b);
                }
            }
        }

        // Only draw special images when fully opaque
        // This prevents flashes of special images during delta frames
        bool drawspecial = textbox[i].tl >= 1.0;

        if ((textbox[i].yp == 12 || textbox[i].yp == 180) && textbox[i].r == 165 && drawspecial)
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
        else if ((textbox[i].yp == 12 || textbox[i].yp == 180) && textbox[i].g == 165 && drawspecial)
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
            if (textbox[i].r == 175 && textbox[i].g == 175 && drawspecial)
            {
                //purple guy
                drawsprite(80 - 6, 64 + 48 + 4, 6, 220- help.glow/4 - int(fRandom()*20), 120- help.glow/4, 210 - help.glow/4);
            }
            else if (textbox[i].r == 175 && textbox[i].b == 175 && drawspecial)
            {
                //red guy
                drawsprite(80 - 6, 64 + 48+ 4, 6, 255 - help.glow/8, 70 - help.glow/4, 70 - help.glow / 4);
            }
            else if (textbox[i].r == 175 && drawspecial)
            {
                //green guy
                drawsprite(80 - 6, 64 + 48 + 4, 6, 120 - help.glow / 4 - int(fRandom() * 20), 220 - help.glow / 4, 120 - help.glow / 4);
            }
            else if (textbox[i].g == 175 && drawspecial)
            {
                //yellow guy
                drawsprite(80 - 6, 64 + 48+ 4, 6, 220- help.glow/4 - int(fRandom()*20), 210 - help.glow/4, 120- help.glow/4);
            }
            else if (textbox[i].b == 175 && drawspecial)
            {
                //blue guy
                drawsprite(80 - 6, 64 + 48+ 4, 6, 75, 75, 255- help.glow/4 - int(fRandom()*20));
            }
        }
        else
        {
            if (textbox[i].r == 175 && textbox[i].g == 175 && drawspecial)
            {
                //purple guy
                drawsprite(80 - 6, 64 + 32 + 4, 0, 220- help.glow/4 - int(fRandom()*20), 120- help.glow/4, 210 - help.glow/4);
            }
            else if (textbox[i].r == 175 && textbox[i].b == 175 && drawspecial)
            {
                //red guy
                drawsprite(80 - 6, 64 + 32 + 4, 0, 255 - help.glow/8, 70 - help.glow/4, 70 - help.glow / 4);
            }
            else if (textbox[i].r == 175 && drawspecial)
            {
                //green guy
                drawsprite(80 - 6, 64 + 32 + 4, 0, 120 - help.glow / 4 - int(fRandom() * 20), 220 - help.glow / 4, 120 - help.glow / 4);
            }
            else if (textbox[i].g == 175 && drawspecial)
            {
                //yellow guy
                drawsprite(80 - 6, 64 + 32 + 4, 0, 220- help.glow/4 - int(fRandom()*20), 210 - help.glow/4, 120- help.glow/4);
            }
            else if (textbox[i].b == 175 && drawspecial)
            {
                //blue guy
                drawsprite(80 - 6, 64 + 32 + 4, 0, 75, 75, 255- help.glow/4 - int(fRandom()*20));
            }
        }
    }
}

void Graphics::updatetextboxes()
{
    for (size_t i = 0; i < textbox.size(); i++)
    {
        textbox[i].update();

        if (textbox[i].tm == 2 && textbox[i].tl <= 0.5)
        {
            textbox.erase(textbox.begin() + i);
            i--;
            continue;
        }
    }
}

void Graphics::drawimagecol( int t, int xp, int yp, int r = 0, int g = 0, int b = 0, bool cent/*= false*/ )
{
    if (!INBOUNDS_VEC(t, images))
    {
        return;
    }
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
    if (!INBOUNDS_VEC(t, images))
    {
        return;
    }

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
  if (!INBOUNDS_VEC(t, images))
  {
    return;
  }

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
    int usethispos = lerp(oldcutscenebarspos, cutscenebarspos);
    if (showcutscenebars)
    {
        FillRect(backBuffer, 0, 0, usethispos, 16, 0x000000);
        FillRect(backBuffer, 360-usethispos, 224, usethispos, 16, 0x000000);
    }
    else if (cutscenebarspos > 0) //disappearing
    {
        //draw
        FillRect(backBuffer, 0, 0, usethispos, 16, 0x000000);
        FillRect(backBuffer, 360-usethispos, 224, usethispos, 16, 0x000000);
    }
}

void Graphics::cutscenebarstimer()
{
    oldcutscenebarspos = cutscenebarspos;
    if (showcutscenebars)
    {
        cutscenebarspos += 25;
        cutscenebarspos = VVV_min(cutscenebarspos, 361);
    }
    else if (cutscenebarspos > 0)
    {
        //disappearing
        cutscenebarspos -= 25;
        cutscenebarspos = VVV_max(cutscenebarspos, 0);
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
                drawsprite(x, y, 14, col_crewinactive);
            }
            else
            {
                drawsprite(x, y, 12, col_crewinactive);
            }
        }
        else
        {
            if (flipmode)
            {
                drawsprite(x - 8, y, 14, col_crewinactive);
            }
            else
            {
                drawsprite(x - 8, y, 12, col_crewinactive);
            }
        }
    }
    else
    {
        if (flipmode) crewframe += 6;

        switch(t)
        {
        case 0:
            drawsprite(x, y, crewframe, col_crewcyan);
            break;
        case 1:
            drawsprite(x, y, crewframe, col_crewpurple);
            break;
        case 2:
            drawsprite(x, y, crewframe, col_crewyellow);
            break;
        case 3:
            drawsprite(x, y, crewframe, col_crewred);
            break;
        case 4:
            drawsprite(x, y, crewframe, col_crewgreen);
            break;
        case 5:
            drawsprite(x, y, crewframe, col_crewblue);
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

    for (int k = 0; k < w2-2; k++)
    {
        drawcoloredtile(x + 8-xo + (k * 8), y, 41, r, g, b);
        drawcoloredtile(x + 8-xo + (k * 8), y + (h) - 8, 46, r, g, b);
    }

    for (int k = 0; k < h2-2; k++)
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

    for (int k = 0; k < w2-2; k++)
    {
        drawcoloredtile(x + 8-xo + (k * 8), y, 41, r, g, b);
        drawcoloredtile(x + 8-xo + (k * 8), y + (h) - 8, 46, r, g, b);
    }


    drawcoloredtile(x+ (w) - 16, y, 41, r, g, b);
    drawcoloredtile(x+ (w) - 16, y + (h) - 8, 46, r, g, b);
    drawcoloredtile(x+ (w) - 24, y, 41, r, g, b);
    drawcoloredtile(x+ (w) - 24, y + (h) - 8, 46, r, g, b);

    for (int k = 0; k < h2-2; k++)
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
    if (!INBOUNDS_VEC(m, textbox))
    {
        puts("textboxtimer() out-of-bounds!");
        return;
    }

    textbox[m].timer=t;
}

void Graphics::addline( std::string t )
{
    if (!INBOUNDS_VEC(m, textbox))
    {
        puts("addline() out-of-bounds!");
        return;
    }

    textbox[m].addline(t);
}

void Graphics::textboxadjust()
{
    if (!INBOUNDS_VEC(m, textbox))
    {
        puts("textboxadjust() out-of-bounds!");
        return;
    }

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
    int usethisamount = lerp(oldfadeamount, fadeamount);
    if ((fademode == 1)||(fademode == 4))
    {
        FillRect(backBuffer, 0, 0, backBuffer->w, backBuffer->h, 0x000000);
    }
    else if(fademode==3)
    {
        for (size_t i = 0; i < SDL_arraysize(fadebars); i++)
        {
            FillRect(backBuffer, fadebars[i], i * 16, usethisamount, 16, 0x000000 );
        }
    }
    else if(fademode==5 )
    {
        for (size_t i = 0; i < SDL_arraysize(fadebars); i++)
        {
            FillRect(backBuffer, fadebars[i]-usethisamount, i * 16, 500, 16, 0x000000 );
        }
    }

}

void Graphics::processfade()
{
    oldfadeamount = fadeamount;
    if (fademode > 1)
    {
        if (fademode == 2)
        {
            //prepare fade out
            for (size_t i = 0; i < SDL_arraysize(fadebars); i++)
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
            for (size_t i = 0; i < SDL_arraysize(fadebars); i++)
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

void Graphics::drawmenu( int cr, int cg, int cb, bool levelmenu /*= false*/ )
{
    for (size_t i = 0; i < game.menuoptions.size(); i++)
    {
        MenuOption& opt = game.menuoptions[i];

        int fr, fg, fb;
        if (opt.active)
        {
            // Color it normally
            fr = cr;
            fg = cg;
            fb = cb;
        }
        else
        {
            // Color it gray
            fr = 128;
            fg = 128;
            fb = 128;
        }

        int x = i*game.menuspacing + game.menuxoff;
        int y = 140 + i*12 + game.menuyoff;

        if (levelmenu)
        {
            if (game.menuoptions.size() - i <= 3)
            {
                // We're on "next page", "previous page", or "return to menu". Draw them separated by a bit
                y += 8;
            }
            else
            {
                // Get out of the way of the level descriptions
                y += 4;
            }
        }

        char tempstring[Game::menutextbytes];
        SDL_strlcpy(tempstring, opt.text, sizeof(tempstring));

        char buffer[Game::menutextbytes];
        if ((int) i == game.currentmenuoption)
        {
            if (opt.active)
            {
                // Uppercase the text
                // FIXME: This isn't UTF-8 aware!
                size_t templen = SDL_strlen(tempstring);
                for (size_t ii = 0; ii < templen; ii++)
                {
                    tempstring[ii] = SDL_toupper(tempstring[ii]);
                }
            }

            // Add brackets
            SDL_snprintf(buffer, sizeof(buffer), "[ %s ]", tempstring);
            // Account for brackets
            x -= 16;
        }
        else
        {
            SDL_strlcpy(buffer, tempstring, sizeof(buffer));
        }

        Print(x, y, buffer, fr, fg, fb);
    }
}


void Graphics::drawcoloredtile( int x, int y, int t, int r, int g, int b )
{
    if (!INBOUNDS_VEC(t, tiles))
    {
        return;
    }
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

    SDL_Rect rect1 = {p1.x, p1.y, surface1->w, surface1->h};
    SDL_Rect rect2 = {p2.x, p2.y, surface2->w, surface2->h};
    bool intersection = help.intersects(rect1, rect2);

    if(intersection)
    {
        int r3_left = VVV_max(r1_left, r2_left);
        int r3_top = VVV_min(r1_top, r2_top);
        int r3_right = VVV_min(r1_right, r2_right);
        int r3_bottom= VVV_max(r1_bottom, r2_bottom);

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
    }
    return false;

}

void Graphics::drawgravityline( int t )
{
    if (!INBOUNDS_VEC(t, obj.entities))
    {
        WHINE_ONCE("drawgravityline() out-of-bounds!");
        return;
    }

    if (obj.entities[t].life == 0 || obj.entities[t].onentity == 1) // FIXME: Remove 'onentity == 1' when game loop order is fixed!
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
        int usethismult = lerp(obj.oldtrophytext, obj.trophytext);
        temp = (196 * usethismult) / 15;
        temp2 = (196 * usethismult) / 15;
        temp3 = ((255 - help.glow) * usethismult) / 15;
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
    const int yoff = map.towermode ? lerp(map.oldypos, map.ypos) : 0;

    if (!map.custommode)
    {
        for (int i = obj.entities.size() - 1; i >= 0; i--)
        {
            if (!obj.entities[i].ishumanoid())
            {
                drawentity(i, yoff);
            }
        }

        for (int i = obj.entities.size() - 1; i >= 0; i--)
        {
            if (obj.entities[i].ishumanoid())
            {
                drawentity(i, yoff);
            }
        }
    }
    else
    {
        for (int i = obj.entities.size() - 1; i >= 0; i--)
        {
            drawentity(i, yoff);
        }
    }
}

void Graphics::drawentity(const int i, const int yoff)
{
    if (!INBOUNDS_VEC(i, obj.entities))
    {
        WHINE_ONCE("drawentity() out-of-bounds!");
        return;
    }

    if (obj.entities[i].invis)
    {
        return;
    }

    point tpoint;

    SDL_Rect drawRect;

#if !defined(NO_CUSTOM_LEVELS)
    // Special case for gray Warp Zone tileset!
    const int room = game.roomx-100 + (game.roomy-100) * ed.maxwidth;
    const bool custom_gray = INBOUNDS_ARR(room, ed.level)
    && ed.level[room].tileset == 3 && ed.level[room].tilecol == 6;
#else
    const bool custom_gray = false;
#endif

    std::vector<SDL_Surface*>& tilesvec = (map.custommode && !map.finalmode) ? entcolours : tiles;

    std::vector<SDL_Surface*>& spritesvec = flipmode ? flipsprites : sprites;

    const int xp = lerp(obj.entities[i].lerpoldxp, obj.entities[i].xp);
    const int yp = lerp(obj.entities[i].lerpoldyp, obj.entities[i].yp);

    switch (obj.entities[i].size)
    {
    case 0:
    {
        // Sprites
        if (!INBOUNDS_VEC(obj.entities[i].drawframe, spritesvec))
        {
            return;
        }
        tpoint.x = xp;
        tpoint.y = yp - yoff;
        setcolreal(obj.entities[i].realcol);

        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;
        BlitSurfaceColoured(spritesvec[obj.entities[i].drawframe], NULL, backBuffer, &drawRect, ct);

        //screenwrapping!
        point wrappedPoint;
        bool wrapX = false;
        bool wrapY = false;

        wrappedPoint.x = tpoint.x;
        if (tpoint.x < 0)
        {
            wrapX = true;
            wrappedPoint.x += 320;
        }
        else if (tpoint.x > 300)
        {
            wrapX = true;
            wrappedPoint.x -= 320;
        }

        wrappedPoint.y = tpoint.y;
        if (tpoint.y < 0)
        {
            wrapY = true;
            wrappedPoint.y += 230;
        }
        else if (tpoint.y > 210)
        {
            wrapY = true;
            wrappedPoint.y -= 230;
        }

        const bool isInWrappingAreaOfTower = map.towermode && !map.minitowermode && map.ypos >= 500 && map.ypos <= 5000;
        if (wrapX && (map.warpx || isInWrappingAreaOfTower))
        {
            drawRect = sprites_rect;
            drawRect.x += wrappedPoint.x;
            drawRect.y += tpoint.y;
            BlitSurfaceColoured(spritesvec[obj.entities[i].drawframe], NULL, backBuffer, &drawRect, ct);
        }
        if (wrapY && map.warpy)
        {
            drawRect = sprites_rect;
            drawRect.x += tpoint.x;
            drawRect.y += wrappedPoint.y;
            BlitSurfaceColoured(spritesvec[obj.entities[i].drawframe], NULL, backBuffer, &drawRect, ct);
        }
        if (wrapX && wrapY && map.warpx && map.warpy)
        {
            drawRect = sprites_rect;
            drawRect.x += wrappedPoint.x;
            drawRect.y += wrappedPoint.y;
            BlitSurfaceColoured(spritesvec[obj.entities[i].drawframe], NULL, backBuffer, &drawRect, ct);
        }
        break;
    }
    case 1:
        // Tiles
        if (!INBOUNDS_VEC(obj.entities[i].drawframe, tiles))
        {
            return;
        }
        tpoint.x = xp;
        tpoint.y = yp - yoff;
        drawRect = tiles_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;
        BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
        break;
    case 2:
    case 8:
    {
        // Special: Moving platform, 4 tiles or 8 tiles
        if (!INBOUNDS_VEC(obj.entities[i].drawframe, tilesvec))
        {
            return;
        }
        tpoint.x = xp;
        tpoint.y = yp - yoff;
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
            if (custom_gray)
            {
                colourTransform temp_ct;
                temp_ct.colour = 0xFFFFFFFF;
                BlitSurfaceTinted(tilesvec[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, temp_ct);
            }
            else
            {
                BlitSurfaceStandard(tilesvec[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
            }
        }
        break;
    }
    case 3:    // Big chunky pixels!
        prect.x = xp;
        prect.y = yp - yoff;
        FillRect(backBuffer, prect, obj.entities[i].realcol);
        break;
    case 4:    // Small pickups
        setcolreal(obj.entities[i].realcol);
        drawhuetile(xp, yp - yoff, obj.entities[i].tile);
        break;
    case 5:    //Horizontal Line
    {
        int oldw = obj.entities[i].w;
        if ((game.swngame == 3 || kludgeswnlinewidth) && obj.getlineat(84 - 32) == i)
        {
            oldw -= 24;
        }
        line_rect.x = xp;
        line_rect.y = yp - yoff;
        line_rect.w = lerp(oldw, obj.entities[i].w);
        line_rect.h = 1;
        drawgravityline(i);
        break;
    }
    case 6:    //Vertical Line
        line_rect.x = xp;
        line_rect.y = yp - yoff;
        line_rect.w = 1;
        line_rect.h = obj.entities[i].h;
        drawgravityline(i);
        break;
    case 7:    //Teleporter
        drawtele(xp, yp - yoff, obj.entities[i].drawframe, obj.entities[i].realcol);
        break;
    //case 8:    // Special: Moving platform, 8 tiles
        // Note: This code is in the 4-tile code
        break;
    case 9:         // Really Big Sprite! (2x2)
        if (!INBOUNDS_VEC(obj.entities[i].drawframe, spritesvec))
        {
            return;
        }
        setcolreal(obj.entities[i].realcol);

        tpoint.x = xp;
        tpoint.y = yp - yoff;

        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;
        BlitSurfaceColoured(spritesvec[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);

        tpoint.x = xp+32;
        tpoint.y = yp - yoff;
        //
        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;
        BlitSurfaceColoured(spritesvec[obj.entities[i].drawframe+1],NULL, backBuffer, &drawRect, ct);

        tpoint.x = xp;
        tpoint.y = yp+32 - yoff;
        //
        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;
        BlitSurfaceColoured(spritesvec[obj.entities[i].drawframe+12],NULL, backBuffer, &drawRect, ct);

        tpoint.x = xp+32;
        tpoint.y = yp+32 - yoff;
        //
        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;
        BlitSurfaceColoured(spritesvec[obj.entities[i].drawframe + 13],NULL, backBuffer, &drawRect, ct);
        break;
    case 10:         // 2x1 Sprite
        if (!INBOUNDS_VEC(obj.entities[i].drawframe, spritesvec))
        {
            return;
        }
        setcolreal(obj.entities[i].realcol);

        tpoint.x = xp;
        tpoint.y = yp - yoff;
        //
        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;
        BlitSurfaceColoured(spritesvec[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);

        tpoint.x = xp+32;
        tpoint.y = yp - yoff;
        //
        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;
        BlitSurfaceColoured(spritesvec[obj.entities[i].drawframe+1],NULL, backBuffer, &drawRect, ct);
        break;
    case 11:    //The fucking elephant
        setcolreal(obj.entities[i].realcol);
        drawimagecol(3, xp, yp - yoff);
        break;
    case 12:         // Regular sprites that don't wrap
        if (!INBOUNDS_VEC(obj.entities[i].drawframe, spritesvec))
        {
            return;
        }
        tpoint.x = xp;
        tpoint.y = yp - yoff;
        setcolreal(obj.entities[i].realcol);
        //
        drawRect = sprites_rect;
        drawRect.x += tpoint.x;
        drawRect.y += tpoint.y;
        BlitSurfaceColoured(spritesvec[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);


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
        if (!INBOUNDS_VEC(obj.entities[i].drawframe, spritesvec))
        {
            return;
        }

        tpoint.x = xp; tpoint.y = yp - yoff;
        setcolreal(obj.entities[i].realcol);
        setRect(drawRect, Sint16(obj.entities[i].xp ), Sint16(obj.entities[i].yp - yoff), Sint16(sprites_rect.x * 6), Sint16(sprites_rect.y * 6 ) );
        SDL_Surface* TempSurface = ScaleSurface( spritesvec[obj.entities[i].drawframe], 6 * sprites_rect.w,6* sprites_rect.h );
        BlitSurfaceColoured(TempSurface, NULL , backBuffer,  &drawRect, ct );
        SDL_FreeSurface(TempSurface);



        break;
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
        for (int i = 0; i < numstars; i++)
        {
            stars[i].w = 2;
            stars[i].h = 2;
            SDL_Rect star_rect = stars[i];
            star_rect.x = lerp(star_rect.x + starsspeed[i], star_rect.x);
            if (starsspeed[i] <= 6)
            {
                FillRect(backBuffer,star_rect, getRGB(0x22,0x22,0x22));
            }
            else
            {
                FillRect(backBuffer,star_rect, getRGB(0x55,0x55,0x55));
            }
        }
        break;
    case 2:
    {
        int bcol, bcol2;

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

        for (int i = 0; i < numbackboxes; i++)
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

            SDL_Rect backboxrect = backboxes[i];
            backboxrect.x = lerp(backboxes[i].x - backboxvx[i], backboxes[i].x);
            backboxrect.y = lerp(backboxes[i].y - backboxvy[i], backboxes[i].y);

            FillRect(backBuffer, backboxrect, bcol);
            backboxrect.x += 1;
            backboxrect.y += 1;
            backboxrect.w -= 2;
            backboxrect.h -= 2;
            FillRect(backBuffer,backboxrect, bcol2);
        }
        break;
    }
    case 3: //Warp zone (horizontal)
        FillRect(backBuffer, 0x000000);
        BlitSurfaceStandard(warpbuffer, NULL, warpbuffer_lerp, NULL);
        ScrollSurface(warpbuffer_lerp, lerp(0, -3), 0);
        BlitSurfaceStandard(warpbuffer_lerp, &towerbuffer_rect, backBuffer, NULL);
        break;
    case 4: //Warp zone (vertical)
        FillRect(backBuffer, 0x000000);
        SDL_BlitSurface(warpbuffer, NULL, warpbuffer_lerp, NULL);
        ScrollSurface(warpbuffer_lerp, 0, lerp(0, -3));
        SDL_BlitSurface(warpbuffer_lerp, &towerbuffer_rect, backBuffer, NULL);
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

        for (int i = 10 ; i >= 0; i--)
        {
            temp = (i << 4) + lerp(backoffset - 1, backoffset);
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
        for (int i = 0; i < numstars; i++)
        {
            stars[i].w = 2;
            stars[i].h = 2;
            SDL_Rect star_rect = stars[i];
            star_rect.y = lerp(star_rect.y + starsspeed[i], star_rect.y);
            if (starsspeed[i] <= 8)
            {
                FillRect(backBuffer, star_rect, getRGB(0x22, 0x22, 0x22));
            }
            else
            {
                FillRect(backBuffer, star_rect, getRGB(0x55, 0x55, 0x55));
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

        break;
    }
}

void Graphics::updatebackground(int t)
{
    switch (t)
    {
    case 1:
        //Starfield
        for (int i = 0; i < numstars; i++)
        {
            stars[i].w = 2;
            stars[i].h = 2;
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
        if (rcol == 6)
        {
            //crazy caze
            spcoldel--;
            if (spcoldel <= 0)
            {
                spcoldel = 15;
                spcol++;
                if (spcol >= 12) spcol = 0;
            }
        }
        for (int i = 0; i < numbackboxes; i++)
        {
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
    {
        int temp = 680 + (rcol * 3);
        backoffset+=3;
        if (backoffset >= 16) backoffset -= 16;

        if (backgrounddrawn)
        {
            ScrollSurface(warpbuffer, -3, 0 );
            for (int j = 0; j < 15; j++)
            {
                for (int i = 0; i < 2; i++)
                {
                    drawtowertile(317 - backoffset + (i * 16), (j * 16), temp+40);  //20*16 = 320
                    drawtowertile(317 - backoffset + (i * 16) + 8, (j * 16), temp + 41);
                    drawtowertile(317 - backoffset + (i * 16), (j * 16) + 8, temp + 80);
                    drawtowertile(317 - backoffset + (i * 16) + 8, (j * 16) + 8, temp + 81);
                }
            }
        }
        else
        {
            //draw the whole thing for the first time!
            backoffset = 0;
            FillRect(warpbuffer, 0x000000);
            for (int j = 0; j < 15; j++)
            {
                for (int i = 0; i < 21; i++)
                {
                    drawtowertile((i * 16) - backoffset - 3, (j * 16), temp+40);
                    drawtowertile((i * 16) - backoffset + 8 - 3, (j * 16), temp + 41);
                    drawtowertile((i * 16) - backoffset - 3, (j * 16) + 8, temp + 80);
                    drawtowertile((i * 16) - backoffset + 8 - 3, (j * 16) + 8, temp + 81);
                }
            }
            backgrounddrawn = true;
        }
        break;
    }
    case 4: //Warp zone (vertical)
    {
        int temp = 760 + (rcol * 3);
        backoffset+=3;
        if (backoffset >= 16) backoffset -= 16;

        if (backgrounddrawn)
        {
            ScrollSurface(warpbuffer,0,-3);
            for (int j = 0; j < 2; j++)
            {
                for (int i = 0; i < 21; i++)
                {
                    drawtowertile((i * 16), 237 - backoffset + (j * 16), temp + 40); //14*17=240 - 3
                    drawtowertile((i * 16) + 8, 237 - backoffset + (j * 16), temp + 41);
                    drawtowertile((i * 16), 237 - backoffset + (j * 16) + 8, temp + 80);
                    drawtowertile((i * 16) + 8, 237 - backoffset + (j * 16) + 8, temp + 81);
                }
            }
        }
        else
        {
            //draw the whole thing for the first time!
            backoffset = 0;
            FillRect(warpbuffer,0x000000 );
            for (int j = 0; j < 16; j++)
            {
                for (int i = 0; i < 21; i++)
                {
                    drawtowertile((i * 16), (j * 16)- backoffset - 3, temp+40);
                    drawtowertile((i * 16)+ 8, (j * 16)- backoffset - 3, temp + 41);
                    drawtowertile((i * 16), (j * 16)- backoffset + 8 - 3, temp + 80);
                    drawtowertile((i * 16)+ 8, (j * 16)- backoffset + 8 - 3, temp + 81);
                }
            }
            backgrounddrawn = true;
        }
        break;
    }
    case 5:
        //Warp zone, central

        backoffset += 1;
        if (backoffset >= 16)
        {
            backoffset -= 16;
            warpskip = (warpskip + 1) % 2;
        }
        break;
    case 6:
        //Final Starfield
        for (int i = 0; i < numstars; i++)
        {
            stars[i].w = 2;
            stars[i].h = 2;
            stars[i].y -= starsspeed[i];
            if (stars[i].y < -10)
            {
                stars[i].y += 260;
                stars[i].x = fRandom() * 320;
                starsspeed[i] = 5+(fRandom()*5);
            }
        }
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
            for (int j = 0; j < 30; j++)
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
            for (int j = 0; j < 30; j++)
            {
                for (int i = 0; i < 40; i++)
                {
                    if(map.contents[i + map.vmult[j]]>0) drawforetile3(i * 8, j * 8, map.contents[i + map.vmult[j]],map.rcol);
                }
            }
        }
        foregrounddrawn = true;
    }
    SDL_BlitSurface(foregroundBuffer, NULL, backBuffer, NULL);

}

void Graphics::drawfinalmap()
{
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

    SDL_BlitSurface(foregroundBuffer, NULL, backBuffer, NULL);
}

void Graphics::drawtowermap()
{
    int temp;
    int yoff = lerp(map.oldypos, map.ypos);
    for (int j = 0; j < 31; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            temp = map.tower.at(i, j, yoff);
            if (temp > 0) drawtile3(i * 8, (j * 8) - (yoff % 8), temp, towerbg.colstate);
        }
    }
}

void Graphics::drawtowerspikes()
{
    int spikeleveltop = lerp(map.oldspikeleveltop, map.spikeleveltop);
    int spikelevelbottom = lerp(map.oldspikelevelbottom, map.spikelevelbottom);
    for (int i = 0; i < 40; i++)
    {
        drawtile3(i * 8, -8+spikeleveltop, 9, towerbg.colstate);
        drawtile3(i * 8, 230-spikelevelbottom, 8, towerbg.colstate, 8 - spikelevelbottom);
    }
}

void Graphics::drawtowerbackground(const TowerBG& bg_obj)
{
    FillRect(backBuffer, 0x000000);
    SDL_BlitSurface(bg_obj.buffer, NULL, bg_obj.buffer_lerp, NULL);
    ScrollSurface(bg_obj.buffer_lerp, 0, lerp(0, -bg_obj.bscroll));
    SDL_BlitSurface(bg_obj.buffer_lerp, &towerbuffer_rect, backBuffer, NULL);
}

void Graphics::updatetowerbackground(TowerBG& bg_obj)
{
    int temp;

    if (bg_obj.bypos < 0) bg_obj.bypos += 120 * 8;

    if (bg_obj.tdrawback)
    {
        int off = bg_obj.scrolldir == 0 ? 0 : bg_obj.bscroll;
        //Draw the whole thing; needed for every colour cycle!
        for (int j = -1; j < 32; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                temp = map.tower.backat(i, j, bg_obj.bypos);
                drawtowertile3(i * 8, (j * 8) - (bg_obj.bypos % 8) - off, temp, bg_obj);
            }
        }

        bg_obj.tdrawback = false;
    }
    else
    {
        //just update the bottom
        ScrollSurface(bg_obj.buffer, 0, -bg_obj.bscroll);
        if (bg_obj.scrolldir == 0)
        {
            for (int i = 0; i < 40; i++)
            {
                temp = map.tower.backat(i, -1, bg_obj.bypos);
                drawtowertile3(i * 8, -1*8 - (bg_obj.bypos % 8), temp, bg_obj);
                temp = map.tower.backat(i, 0, bg_obj.bypos);
                drawtowertile3(i * 8, -(bg_obj.bypos % 8), temp, bg_obj);
            }
        }
        else
        {
            for (int i = 0; i < 40; i++)
            {
                temp = map.tower.backat(i, 29, bg_obj.bypos);
                drawtowertile3(i * 8, 29*8 - (bg_obj.bypos % 8) - bg_obj.bscroll, temp, bg_obj);
                temp = map.tower.backat(i, 30, bg_obj.bypos);
                drawtowertile3(i * 8, 30*8 - (bg_obj.bypos % 8) - bg_obj.bscroll, temp, bg_obj);
                temp = map.tower.backat(i, 31, bg_obj.bypos);
                drawtowertile3(i * 8, 31*8 - (bg_obj.bypos % 8) - bg_obj.bscroll, temp, bg_obj);
                temp = map.tower.backat(i, 32, bg_obj.bypos);
                drawtowertile3(i * 8, 32*8 - (bg_obj.bypos % 8) - bg_obj.bscroll, temp, bg_obj);
            }
        }
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
		//Yellow
	case 30:
		ct.colour = RGBf(160, 200, 220);
		break;
		//Purple
	case 31:
		ct.colour = RGBf(220, 120, 210);
		break;
		//cyan
	case 32:
		ct.colour = RGBf(220, 210, 120);
		break;
		//Blue
	case 33:
		ct.colour = RGBf(255, 70, 70);
		break;
		//green
	case 34:
		ct.colour = RGBf(120, 220, 120);
		break;
		//red
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
		ct.colour = getRGB(255, 255, 255);
		break;
	}
}

void Graphics::menuoffrender()
{
	SDL_Rect offsetRect1;
	setRect (offsetRect1, 0, 0, backBuffer->w ,backBuffer->h);

	//put the back buffer in the menubuffer
	BlitSurfaceStandard(backBuffer, NULL, menubuffer, NULL);



	int usethisoffset = lerp(oldmenuoffset, menuoffset);
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
		setRect (offsetRect, 0, usethisoffset, backBuffer->w ,backBuffer->h);
		SDL_Surface* temp = FlipSurfaceVerticle(menubuffer);
		BlitSurfaceStandard(temp,NULL,backBuffer,&offsetRect);
		SDL_FreeSurface(temp);
	}
	else
	{
		//put the stored backbuffer in the backbuffer.
		BlitSurfaceStandard(tempBuffer, NULL, backBuffer, NULL);

		SDL_Rect offsetRect;
		setRect (offsetRect, 0, usethisoffset, backBuffer->w ,backBuffer->h);
		BlitSurfaceStandard(menubuffer,NULL,backBuffer,&offsetRect);
	}

	SDL_Rect rect;
	setRect(rect, 0, 0, backBuffer->w, backBuffer->h);
	screenbuffer->UpdateScreen(backBuffer,&rect);
	FillRect(backBuffer, 0x000000);
}

void Graphics::drawhuetile( int x, int y, int t )
{
	if (!INBOUNDS_VEC(t, tiles))
	{
		return;
	}
	point tpoint;
	tpoint.x = x;
	tpoint.y = y;


	SDL_Rect rect;
	setRect(rect,tpoint.x,tpoint.y,tiles_rect.w, tiles_rect.h);
	BlitSurfaceColoured(tiles[t],NULL,backBuffer, &rect, ct);
}

void Graphics::huetilesetcol(int t)
{
	switch (t)
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
}

Uint32 Graphics::bigchunkygetcol(int t)
{
	//A seperate index of colours, for simplicity
	switch (t)
	{
	case 1:
		return getRGB((fRandom() * 64), 10, 10);
	case 2:
		return getRGB(int(160- help.glow/2 - (fRandom()*20)),  200- help.glow/2, 220 - help.glow);
	}
	return 0x00000000;
}

void Graphics::setwarprect( int a, int b, int c, int d )
{
	warprect.x = a;
	warprect.y = b;
	warprect.w = c;
	warprect.h = d;
}

void Graphics::textboxcenterx()
{
	if (!INBOUNDS_VEC(m, textbox))
	{
		puts("textboxcenterx() out-of-bounds!");
		return;
	}

	textbox[m].centerx();
}

int Graphics::textboxwidth()
{
	if (!INBOUNDS_VEC(m, textbox))
	{
		puts("textboxwidth() out-of-bounds!");
		return 0;
	}

	return textbox[m].w;
}

void Graphics::textboxmoveto(int xo)
{
	if (!INBOUNDS_VEC(m, textbox))
	{
		puts("textboxmoveto() out-of-bounds!");
		return;
	}

	textbox[m].xp = xo;
}

void Graphics::textboxcentery()
{
	if (!INBOUNDS_VEC(m, textbox))
	{
		puts("textboxcentery() out-of-bounds!");
		return;
	}

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
	if(flipmode)
	{
		//	tpoint.x = int((Math.random() * 7) - 4); tpoint.y = int((Math.random() * 7) - 4);
		//	flipmatrix.translate(tpoint.x, tpoint.y);
		//	screenbuffer.draw(backbuffer, flipmatrix);
		//	flipmatrix.translate(-tpoint.x, -tpoint.y);

		SDL_Rect shakeRect;
		setRect(shakeRect,screenshake_x, screenshake_y, backBuffer->w, backBuffer->h);
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
		SDL_Rect shakeRect;
		setRect(shakeRect,screenshake_x, screenshake_y, backBuffer->w, backBuffer->h);
		screenbuffer->UpdateScreen( backBuffer, &shakeRect);
	}

	FillRect(backBuffer, 0x000000 );
}

void Graphics::updatescreenshake()
{
	screenshake_x =  static_cast<Sint32>((fRandom() * 7) - 4);
	screenshake_y =  static_cast<Sint32>((fRandom() * 7) - 4);
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
		flashlight();
	}

	if (game.screenshake > 0 && !game.noflashingmode)
	{
		screenshake();
	}
	else
	{
		render();
	}
}

void Graphics::bigrprint(int x, int y, std::string& t, int r, int g, int b, bool cen, float sc)
{
	std::vector<SDL_Surface*>& font = flipmode ? flipbfont : bfont;

	r = clamp(r, 0, 255);
	g = clamp(g, 0, 255);
	b = clamp(b, 0, 255);
	ct.colour = getRGB(r, g, b);

	x = x /  (sc);

	x -= (len(t));

	if (cen)
	{
		x = VVV_max(160 - (int((len(t)/ 2.0)*sc)), 0 );
	}
	else
	{
		x *=  (sc);
	}

	int bfontpos = 0;
	int cur;
	int idx;
	std::string::iterator iter = t.begin();
	while (iter != t.end()) {
		cur = utf8::unchecked::next(iter);
		idx = font_idx(cur);
		if (INBOUNDS_VEC(idx, font))
		{
			SDL_Surface* tempPrint = ScaleSurface(font[idx], font[idx]->w *sc,font[idx]->h *sc);
			SDL_Rect printrect = { Sint16((x) + bfontpos), Sint16(y) , Sint16(bfont_rect.w*sc), Sint16(bfont_rect.h * sc)};
			BlitSurfaceColoured(tempPrint, NULL, backBuffer, &printrect, ct);
			SDL_FreeSurface(tempPrint);
		}
		bfontpos+=bfontlen(cur)* sc;
	}
}

void Graphics::drawtele(int x, int y, int t, Uint32 c)
{
	setcolreal(getRGB(16,16,16));

	SDL_Rect telerect;
	setRect(telerect, x , y, tele_rect.w, tele_rect.h );
	if (INBOUNDS_VEC(0, tele))
	{
		BlitSurfaceColoured(tele[0], NULL, backBuffer, &telerect, ct);
	}

	setcolreal(c);
	if (t > 9) t = 8;
	if (t < 1) t = 1;

	setRect(telerect, x , y, tele_rect.w, tele_rect.h );
	if (INBOUNDS_VEC(t, tele))
	{
		BlitSurfaceColoured(tele[t], NULL, backBuffer, &telerect, ct);
	}
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
	if (!INBOUNDS_VEC(t, tiles))
	{
		WHINE_ONCE("drawforetile() out-of-bounds!")
		return;
	}

	SDL_Rect rect;
	setRect(rect, x,y,tiles_rect.w, tiles_rect.h);

#if !defined(NO_CUSTOM_LEVELS)
	if (t >= 14 && t <= 17 && (!FILESYSTEM_assetsmounted || ed.onewaycol_override))
	{
		colourTransform thect = {ed.getonewaycol()};
		BlitSurfaceTinted(tiles[t], NULL, foregroundBuffer, &rect, thect);
	}
	else
#endif
	{
		BlitSurfaceStandard(tiles[t],NULL, foregroundBuffer, &rect  );
	}
}

void Graphics::drawforetile2(int x, int y, int t)
{
	if (!INBOUNDS_VEC(t, tiles2))
	{
		WHINE_ONCE("drawforetile2() out-of-bounds!")
		return;
	}

	SDL_Rect rect;
	setRect(rect, x,y,tiles_rect.w, tiles_rect.h);

#if !defined(NO_CUSTOM_LEVELS)
	if (t >= 14 && t <= 17 && (!FILESYSTEM_assetsmounted || ed.onewaycol_override))
	{
		colourTransform thect = {ed.getonewaycol()};
		BlitSurfaceTinted(tiles2[t], NULL, foregroundBuffer, &rect, thect);
	}
	else
#endif
	{
		BlitSurfaceStandard(tiles2[t],NULL, foregroundBuffer, &rect  );
	}
}

void Graphics::drawforetile3(int x, int y, int t, int off)
{
	t += off * 30;
	if (!INBOUNDS_VEC(t, tiles3))
	{
		WHINE_ONCE("drawforetile3() out-of-bounds!")
		return;
	}
	SDL_Rect rect;
	setRect(rect, x,y,tiles_rect.w, tiles_rect.h);
	BlitSurfaceStandard(tiles3[t],NULL, foregroundBuffer, &rect  );
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

	if (screenbuffer != NULL)
	{
		screenbuffer->LoadIcon();
	}

	music.init();
}

Uint32 Graphics::crewcolourreal(int t)
{
	switch (t)
	{
	case 0:
		return col_crewcyan;
	case 1:
		return col_crewpurple;
	case 2:
		return col_crewyellow;
	case 3:
		return col_crewred;
	case 4:
		return col_crewgreen;
	case 5:
		return col_crewblue;
	}
	return col_crewcyan;
}
