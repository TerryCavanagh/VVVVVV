#include "Graphics.h"
#include "Maths.h"
#include "Entity.h"
#include "Map.h"
#include "Screen.h"

Graphics::Graphics()
{
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
    //updatebackground = true;


    //ct = new ColorTransform(0, 0, 0, 1, 255, 255, 255, 1); //Set to white

	linestate = 0;


    trinketcolset = false;

    showcutscenebars = false;
    cutscenebarspos = 0;

    flipmode = false;
    setflipmode = false;
    //flipmatrix.scale(1, -1);
    //flipmatrix.translate(0, 240);
    //flipfontmatrix.scale(1, -1);	flipfontmatrix.translate(0, 8);
    //flipfontmatrix2.scale(1, -1);	flipfontmatrix2.translate(0, 9);

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
    //backboxrect = new Rectangle();
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

    //Textboxes!
    textbox.resize(30);
    ntextbox = 0;

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
}

Graphics::~Graphics()
{

}

void Graphics::drawspritesetcol(int x, int y, int t, int c, UtilityClass& help)
{
    SDL_Rect rect;
    setRect(rect,x,y,sprites_rect.w,sprites_rect.h);
    setcol(c, help);

    BlitSurfaceColoured(sprites[t],NULL,backBuffer, &rect, ct);
    //.copyPixels(sprites[t], sprites_rect, backbuffer, tpoint);
}

void Graphics::Makebfont()
{
    for (int j =  0; j < 16; j++)
    {
        for (int i = 0; i < 16; i++)
        {

            SDL_Surface* temp = GetSubSurface(grphx.im_bfont,i*8,j*8,8,8);
            bfont.push_back(temp);

            temp = GetSubSurface(grphx.im_bfont,i*8,j*8,8,8);
			SDL_Surface* TempFlipped = FlipSurfaceVerticle(temp);

            flipbfont.push_back(TempFlipped);
			SDL_FreeSurface(temp);

        }
    }

    //Ok, now we work out the lengths (this data string cortesy of a program I wrote!)
    for (int i = 0; i < 256; i++)
    {
        bfontlen.push_back(6);
    }


    for(int k = 0; k < 96; k++)
    {
        bfontlen[k + 32] = 8;// int(maprow[k]);
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


void Graphics::Print( int _x, int _y, std::string _s, int r, int g, int b, bool cen /*= false*/ )
{
    r = clamp(r,0,255);
    g = clamp(g,0,255);
    b = clamp(b,0,255);

    ct.colour = getRGB(r, g, b);

    if (cen)
        _x = ((160 ) - ((len(_s)) / 2));
    int bfontpos = 0;
    int curr;
    for (unsigned int i = 0; i < _s.length(); i++)
    {
        curr = (_s.c_str())[i];
        if (curr > 255 || curr < 0)
        {
            curr = '?';
        }
        point tpoint;
        tpoint.x = _x + bfontpos;
        tpoint.y = _y;

        SDL_Rect fontRect = bfont_rect;
        fontRect.x = tpoint.x ;
        fontRect.y = tpoint.y ;

        if (flipmode)
        {
            BlitSurfaceColoured( flipbfont[curr], NULL, backBuffer, &fontRect , ct);
        }
        else
        {
            BlitSurfaceColoured( bfont[curr], NULL, backBuffer, &fontRect , ct);
        }
        bfontpos+=bfontlen[curr] ;
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
    for (unsigned int i = 0; i < _s.length(); i++)
    {
        curr = (_s.c_str())[i];
        if (curr > 255 || curr < 0)
        {
            curr = '?';
        }

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
			SDL_Surface* tempPrint = ScaleSurfaceSlow(flipbfont[curr], bfont[curr]->w *sc,bfont[curr]->h *sc);
			SDL_Rect printrect = { Sint16((_x) + bfontpos), Sint16(_y) , Sint16(bfont_rect.w*sc), Sint16(bfont_rect.h * sc)};
			BlitSurfaceColoured(tempPrint, NULL, backBuffer, &printrect, ct);
			SDL_FreeSurface(tempPrint);
        }
        else
        {
			SDL_Surface* tempPrint = ScaleSurfaceSlow(bfont[curr], bfont[curr]->w *sc,bfont[curr]->h *sc);
			SDL_Rect printrect = { static_cast<Sint16>((_x) + bfontpos), static_cast<Sint16>(_y) , static_cast<Sint16>((bfont_rect.w*sc)+1), static_cast<Sint16>((bfont_rect.h * sc)+1)};
			BlitSurfaceColoured(tempPrint, NULL, backBuffer, &printrect, ct);
			SDL_FreeSurface(tempPrint);
        }
        bfontpos+=bfontlen[curr] *sc;
    }
}

int Graphics::len(std::string t)
{
    int bfontpos = 0;
    for (unsigned int i = 0; i < t.length(); i++)
    {
        int cur = (t.c_str())[i];
        bfontpos+= bfontlen[cur] ;
    }
    return bfontpos;
}

void Graphics::PrintOff( int _x, int _y, std::string _s, int r, int g, int b, bool cen /*= false*/ )
{
    r = clamp(r,0,255);
    g = clamp(g,0,255);
    b = clamp(b,0,255);

    ct.colour = getRGB(r, g, b);

    if (cen)
        _x = ((160) - (len(_s) / 2))+_x;
    int bfontpos = 0;
    int curr;
    for (unsigned int i = 0; i < _s.length(); i++)
    {
        curr = (_s.c_str())[i];
        if (curr > 255 || curr < 0)
        {
            curr = '?';
        }
        point tpoint;
        tpoint.x = _x + bfontpos;
        tpoint.y = _y;

        SDL_Rect fontRect = bfont_rect;
        fontRect.x = tpoint.x ;
        fontRect.y = tpoint.y ;

        if (flipmode)
        {
            //flipbfont[cur].colorTransform(bfont_rect, ct);
            BlitSurfaceColoured( bfont[curr], NULL, backBuffer, &fontRect , ct);
        }
        else
        {
            //bfont[cur].colorTransform(bfont_rect, ct);
            //backBuffer.copyPixels(bfont[cur], bfont_rect, tpoint);
            BlitSurfaceColoured( bfont[curr], NULL, backBuffer, &fontRect , ct);
        }
        bfontpos+=bfontlen[curr] ;
    }
}

void Graphics::bprint( int x, int y, std::string t, int r, int g, int b, bool cen /*= false*/ )
{

    //printmask(x, y, t, cen);
    //Print(x, y - 1, t, 0, 0, 0, cen);
    //if (cen)
    //{
    //	//TODO find different
    //	PrintOff(-1, y, t, 0, 0, 0, cen);
    //	PrintOff(1, y, t, 0, 0, 0, cen);
    //}
    //else
    //{
    //	Print(x  -1, y, t, 0, 0, 0, cen);
    //	Print(x , y, t, 0, 0, 0, cen);
    //}
    //Print(x, y+1, t, 0, 0, 0, cen);

    Print(x, y, t, r, g, b, cen);
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
    for (unsigned int i = 0; i < _s.length(); i++)
    {
        curr = (_s.c_str())[i];
        if (curr > 255 || curr < 0)
        {
            curr = '?';
        }
        point tpoint;
        tpoint.x = _x + bfontpos;
        tpoint.y = _y;

        SDL_Rect fontRect = bfont_rect;
        fontRect.x = tpoint.x ;
        fontRect.y = tpoint.y ;

        if (flipmode)
        {
            //flipbfont[cur].colorTransform(bfont_rect, ct);
            BlitSurfaceColoured( flipbfont[curr], NULL, backBuffer, &fontRect , ct);
        }
        else
        {
            //bfont[cur].colorTransform(bfont_rect, ct);
            //backBuffer.copyPixels(bfont[cur], bfont_rect, tpoint);
            BlitSurfaceColoured( bfont[curr], NULL, backBuffer, &fontRect , ct);
        }
        bfontpos+=bfontlen[curr] ;
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
    //sprites[t].colorTransform(sprites_rect, ct);
    BlitSurfaceColoured(sprites[t], NULL, backBuffer, &rect, ct);
}

void Graphics::drawtile( int x, int y, int t, int r, int g,  int b )
{
    SDL_Rect rect = { Sint16(x), Sint16(y), tiles_rect.w, tiles_rect.h };
    BlitSurfaceStandard(tiles[t], NULL, backBuffer, &rect);
}


void Graphics::drawtile2( int x, int y, int t, int r, int g,  int b )
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

void Graphics::drawgui( UtilityClass& help )
{
    textboxcleanup();
    //Draw all the textboxes to the screen
    for (int i = 0; i<ntextbox; i++)
    {
        //This routine also updates the textboxs
        textbox[i].update();
        if (textbox[i].active)
        {
            if (textbox[i].r == 0 && textbox[i].g == 0 && textbox[i].b == 0)
            {
                if(flipmode)
                {
                    for (j = 0; j < textbox[i].numlines; j++)
                    {
                        Print(textbox[i].xp + 8, textbox[i].yp + (textbox[i].numlines*8) - (j * 8), textbox[i].line[j], 196, 196, 255 - help.glow);
                    }
                }
                else
                {
                    for (j = 0; j < textbox[i].numlines; j++)
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
                for (int k = 0; k < textbox[i].numlines; k++)
                {
                    drawcoloredtile(textbox[i].xp, textbox[i].yp + 8 + (k * 8), 43, textbox[i].r, textbox[i].g, textbox[i].b);
                    drawcoloredtile(textbox[i].xp + textbox[i].w-8, textbox[i].yp + 8 + (k * 8), 44, textbox[i].r, textbox[i].g, textbox[i].b);
                }

                if(flipmode)
                {
                    for (j = 0; j < textbox[i].numlines; j++)
                    {
                        Print(textbox[i].xp + 8, textbox[i].yp  + (textbox[i].numlines*8) - (j * 8), textbox[i].line[j], textbox[i].r, textbox[i].g, textbox[i].b);
                    }
                }
                else
                {
                    for (j = 0; j < textbox[i].numlines; j++)
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
        //backbuffer.copyPixels(images[t], trect, tpoint);
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
        //backbuffer.fillRect(new Rectangle(0, 0, cutscenebarspos, 16), 0x000000);
        FillRect(backBuffer, 360-cutscenebarspos, 224, cutscenebarspos, 16, 0x000000);
        //backbuffer.fillRect(new Rectangle(360-cutscenebarspos, 224, cutscenebarspos, 16), 0x000000);
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
            //backbuffer.fillRect(new Rectangle(0, 0, cutscenebarspos, 16), 0x000000);
            FillRect(backBuffer, 360-cutscenebarspos, 224, cutscenebarspos, 16, 0x000000);
            //backbuffer.fillRect(new Rectangle(360-cutscenebarspos, 224, cutscenebarspos, 16), 0x000000);
        }
    }
}

void Graphics::drawcrewman( int x, int y, int t, bool act, UtilityClass& help, bool noshift /*=false*/ )
{
    if (!act)
    {
        if (noshift)
        {
            if (flipmode)
            {
                drawspritesetcol(x, y, 14, 19, help);
            }
            else
            {
                drawspritesetcol(x, y, 12, 19, help);
            }
        }
        else
        {
            if (flipmode)
            {
                drawspritesetcol(x - 8, y, 14, 19, help);
            }
            else
            {
                drawspritesetcol(x - 8, y, 12, 19, help);
            }
        }
    }
    else
    {
        if (flipmode) crewframe += 6;

        switch(t)
        {
        case 0:
            drawspritesetcol(x, y, crewframe, 0 , help);
            break;
        case 1:
            drawspritesetcol(x, y, crewframe, 20, help);
            break;
        case 2:
            drawspritesetcol(x, y, crewframe, 14, help);
            break;
        case 3:
            drawspritesetcol(x, y, crewframe, 15, help);
            break;
        case 4:
            drawspritesetcol(x, y, crewframe, 13, help);
            break;
        case 5:
            drawspritesetcol(x, y, crewframe, 16, help);
            break;
        }

        if (flipmode) crewframe -= 6;
    }
}

void Graphics::drawpixeltextbox( int x, int y, int w, int h, int w2, int h2, int r, int g, int b, int xo, int yo )
{
    //given these parameters, draw a textbox with a pixel width

    //madrect.x = x; madrect.y = y; madrect.w = w; madrect.h = h;
    //backbuffer.fillRect(madrect, RGB(r / 6, g / 6, b / 6));
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

    //madrect.x = x; madrect.y = y; madrect.w = w; madrect.h = h;
    //backbuffer.fillRect(madrect, RGB(r / 6, g / 6, b / 6));
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
    //madrect.x = x; madrect.y = y; madrect.w = w*8; madrect.h = h*8;
    //backbuffer.fillRect(madrect, RGB(r / 6, g / 6, b / 6));
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
    for (int i = 0; i < ntextbox; i++)
    {
        if (m != i) textbox[i].remove();
    }
}

void Graphics::textboxremovefast()
{
    //Remove all textboxes
    for (int i = 0; i < ntextbox; i++)
    {
        textbox[i].removefast();
    }
}

void Graphics::textboxremove()
{
    //Remove all textboxes
    for (int i = 0; i < ntextbox; i++)
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

    if(ntextbox == 0)
    {
        //If there are no active textboxes, Z=0;
        m = 0;
        ntextbox++;
    }
    else
    {
        /*i = 0; m = -1;
        while (i < ntextbox) {
        if (!textbox[i].active) {	m = i; i = ntextbox;}
        i++;
        }
        if (m == -1) {m = ntextbox; ntextbox++;}
        */
        m = ntextbox;
        ntextbox++;
    }

    if(m<20)
    {
        textbox[m].clear();
        textbox[m].line[0] = t;
        textbox[m].xp = xp;
        if (xp == -1) textbox[m].xp = 160 - (((t.length() / 2) + 1) * 8);
        textbox[m].yp = yp;
        textbox[m].initcol(r, g, b);
        textbox[m].resize();
    }
}

void Graphics::drawfade()
{
    if ((fademode == 1)||(fademode == 4))
    {
        FillRect(backBuffer, 0, 0, backBuffer->w, backBuffer->h, 0x000000);
        //backbuffer.fillRect(backbuffer.rect, 0x000000);
    }
    else if(fademode==3)
    {
        for (int i = 0; i < 15; i++)
        {
            FillRect(backBuffer, fadebars[i], i * 16, fadeamount, 16, 0x000000 );
            //backbuffer.fillRect(new Rectangle(, , , 16), 0x000000);
        }
    }
    else if(fademode==5 )
    {
        for (int i = 0; i < 15; i++)
        {
            FillRect(backBuffer, fadebars[i]-fadeamount, i * 16, 500, 16, 0x000000 );
            //backbuffer.fillRect(new Rectangle(fadebars[i]-fadeamount, i * 16, 500, 16), 0x000000);
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

void Graphics::drawmenu( Game& game, int cr, int cg, int cb, int division /*= 30*/ )
{
    for (int i = 0; i < game.nummenuoptions; i++)
    {
        if (i == game.currentmenuoption)
        {
            //Draw it highlighted
            if (game.menuoptionsactive[i])
            {
                tempstring = game.menuoptions[i];
                std::transform(tempstring.begin(), tempstring.end(),tempstring.begin(), ::toupper);
                tempstring = std::string("[ ") + tempstring + std::string(" ]");
                Print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12) +game.menuyoff, tempstring, cr, cg, cb);
            }
            else
            {
                tempstring = game.menuoptions[i];
                tempstring = "[ " + tempstring + " ]";
                //Draw it in gray
                Print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12)+game.menuyoff, tempstring, 128, 128, 128);
            }
        }
        else
        {
            //Draw it normally
            if (game.menuoptionsactive[i])
            {
                Print(110 + (i * division) +game.menuxoff, 140 + (i * 12)+game.menuyoff, game.menuoptions[i], cr, cg, cb);
            }
            else
            {
                //Draw it in gray
                Print(110 + (i * division) +game.menuxoff, 140 + (i * 12)+game.menuyoff, game.menuoptions[i], 128, 128, 128);
            }
        }
    }
}

void Graphics::drawlevelmenu( Game& game, int cr, int cg, int cb, int division /*= 30*/ )
{
    for (int i = 0; i < game.nummenuoptions; i++)
    {
        if (i == game.currentmenuoption)
        {
          if(game.nummenuoptions-i<=2){
            //Draw it highlighted
            if (game.menuoptionsactive[i])
            {
                tempstring = game.menuoptions[i];
                std::transform(tempstring.begin(), tempstring.end(),tempstring.begin(), ::toupper);
                tempstring = std::string("[ ") + tempstring + std::string(" ]");
                Print(110 + (i * division) - 16 +game.menuxoff, 140+8 + (i * 12) +game.menuyoff, tempstring, cr, cg, cb);
            }
            else
            {
                tempstring = game.menuoptions[i];
                tempstring = "[ " + tempstring + " ]";
                //Draw it in gray
                Print(110 + (i * division) - 16 +game.menuxoff, 140+8 + (i * 12)+game.menuyoff, tempstring, 128, 128, 128);
            }
          }else{
            //Draw it highlighted
            if (game.menuoptionsactive[i])
            {
                tempstring = game.menuoptions[i];
                std::transform(tempstring.begin(), tempstring.end(),tempstring.begin(), ::toupper);
                tempstring = std::string("[ ") + tempstring + std::string(" ]");
                Print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12) +game.menuyoff, tempstring, cr, cg, cb);
            }
            else
            {
                tempstring = game.menuoptions[i];
                tempstring = "[ " + tempstring + " ]";
                //Draw it in gray
                Print(110 + (i * division) - 16 +game.menuxoff, 140 + (i * 12)+game.menuyoff, tempstring, 128, 128, 128);
            }
          }
        }
        else
        {
          if(game.nummenuoptions-i<=2){
            //Draw it normally
            if (game.menuoptionsactive[i])
            {
                Print(110 + (i * division) +game.menuxoff, 140+8 + (i * 12)+game.menuyoff, game.menuoptions[i], cr, cg, cb);
            }
            else
            {
                //Draw it in gray
                Print(110 + (i * division) +game.menuxoff, 140+8 + (i * 12)+game.menuyoff, game.menuoptions[i], 128, 128, 128);
            }
          }else{
            //Draw it normally
            if (game.menuoptionsactive[i])
            {
                Print(110 + (i * division) +game.menuxoff, 140 + (i * 12)+game.menuyoff, game.menuoptions[i], cr, cg, cb);
            }
            else
            {
                //Draw it in gray
                Print(110 + (i * division) +game.menuxoff, 140 + (i * 12)+game.menuyoff, game.menuoptions[i], 128, 128, 128);
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


bool Graphics::Hitest(SDL_Surface* surface1, point p1, int col, SDL_Surface* surface2, point p2, int col2)
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

void Graphics::drawgravityline( int t, entityclass& obj )
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

void Graphics::drawtrophytext( entityclass& obj, UtilityClass& help )
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

void Graphics::drawentities( mapclass& map, entityclass& obj, UtilityClass& help )
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

	trinketcolset = false;

    for (int i = obj.nentity - 1; i >= 0; i--)
    {
        if (!obj.entities[i].invis && obj.entities[i].active)
        {
            if (obj.entities[i].size == 0)
            {
                // Sprites
                if (flipmode)
                {
                    tpoint.x = obj.entities[i].xp;
                    tpoint.y = obj.entities[i].yp;
                    //
                    setcol(obj.entities[i].colour, help);
                    //flipsprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(flipsprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);
                    //backbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, tpoint);
                    if (map.warpx)
                    {
                        //screenwrapping!
                        if (tpoint.x < 0)
                        {
                            tpoint.x += 320;
                            drawRect = sprites_rect;
                            drawRect.x += tpoint.x;
                            drawRect.y += tpoint.y;
                            BlitSurfaceStandard(flipsprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                        }
                        if (tpoint.x > 300)
                        {
                            tpoint.x -= 320;
                            drawRect = sprites_rect;
                            drawRect.x += tpoint.x;
                            drawRect.y += tpoint.y;
                            BlitSurfaceStandard(flipsprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
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
                            BlitSurfaceStandard(flipsprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                        }
                        if (tpoint.y > 210)
                        {
                            tpoint.y -= 230;
                            drawRect = sprites_rect;
                            drawRect.x += tpoint.x;
                            drawRect.y += tpoint.y;
                            BlitSurfaceStandard(flipsprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                        }
                    }
                }
                else
                {
                    tpoint.x = obj.entities[i].xp;
                    tpoint.y = obj.entities[i].yp;
                    //
                    setcol(obj.entities[i].colour, help);
                    //sprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);

                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(sprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);
                    if (map.warpx)
                    {
                        //screenwrapping!
                        if (tpoint.x < 0)
                        {
                            tpoint.x += 320;
                            drawRect = sprites_rect;
                            drawRect.x += tpoint.x;
                            drawRect.y += tpoint.y;
                            BlitSurfaceColoured(sprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);
                        }
                        if (tpoint.x > 300)
                        {
                            tpoint.x -= 320;
                            drawRect = sprites_rect;
                            drawRect.x += tpoint.x;
                            drawRect.y += tpoint.y;
                            BlitSurfaceColoured(sprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);
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
                            BlitSurfaceColoured(sprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);
                        }
                        if (tpoint.y > 210)
                        {
                            tpoint.y -= 230;
                            drawRect = sprites_rect;
                            drawRect.x += tpoint.x;
                            drawRect.y += tpoint.y;
                            BlitSurfaceColoured(sprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);
                        }
                    }
                }
            }
            else if (obj.entities[i].size == 1)
            {
                // Tiles
                tpoint.x = obj.entities[i].xp;
                tpoint.y = obj.entities[i].yp;
                drawRect = tiles_rect;
                drawRect.x += tpoint.x;
                drawRect.y += tpoint.y;
                BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
            }
            else if (obj.entities[i].size == 2)
            {
                // Special: Moving platform, 4 tiles
                tpoint.x = obj.entities[i].xp;
                tpoint.y = obj.entities[i].yp;
                drawRect = tiles_rect;
                drawRect.x += tpoint.x;
                drawRect.y += tpoint.y;
                if(map.custommode){
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                }else{
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                }
            }
            else if (obj.entities[i].size == 3)    // Big chunky pixels!
            {
                prect.x = obj.entities[i].xp;
                prect.y = obj.entities[i].yp;
                //A seperate index of colours, for simplicity
                if(obj.entities[i].colour==1)
                {
                    //backbuffer.fillRect(prect, RGB(196 - (fRandom() * 64), 10, 10));
                    FillRect(backBuffer, prect, (fRandom() * 64), 10, 10);
                }
                else if (obj.entities[i].colour == 2)
                {
                    //backbuffer.fillRect(prect, RGB(160- help.glow/2 - (fRandom()*20), 200- help.glow/2, 220 - help.glow));
                    FillRect(backBuffer,prect, int(160- help.glow/2 - (fRandom()*20)),  200- help.glow/2, 220 - help.glow);
                }
            }
            else if (obj.entities[i].size == 4)    // Small pickups
            {
                drawhuetile(obj.entities[i].xp, obj.entities[i].yp, obj.entities[i].tile, obj.entities[i].colour);
            }
            else if (obj.entities[i].size == 5)    //Horizontal Line
            {
                line_rect.x = obj.entities[i].xp;
                line_rect.y = obj.entities[i].yp;
                line_rect.w = obj.entities[i].w;
                line_rect.h = 1;
                drawgravityline(i, obj);
            }
            else if (obj.entities[i].size == 6)    //Vertical Line
            {
                line_rect.x = obj.entities[i].xp;
                line_rect.y = obj.entities[i].yp;
                line_rect.w = 1;
                line_rect.h = obj.entities[i].h;
                drawgravityline(i, obj);
            }
            else if (obj.entities[i].size == 7)    //Teleporter
            {
                drawtele(obj.entities[i].xp, obj.entities[i].yp, obj.entities[i].drawframe, obj.entities[i].colour, help);
            }
            else if (obj.entities[i].size == 8)    // Special: Moving platform, 8 tiles
            {
				//TODO check this is correct game breaking moving paltform
                tpoint.x = obj.entities[i].xp;
                tpoint.y = obj.entities[i].yp;
                drawRect = sprites_rect;
                drawRect.x += tpoint.x;
                drawRect.y += tpoint.y;

                 if(map.custommode){
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(entcolours[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                }else{
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                  drawRect.x += 8;
                  BlitSurfaceStandard(tiles[obj.entities[i].drawframe],NULL, backBuffer, &drawRect);
                }
            }
            else if (obj.entities[i].size == 9)         // Really Big Sprite! (2x2)
            {
                if (flipmode)
                {
                    setcol(obj.entities[i].colour, help);

                    tpoint.x = obj.entities[i].xp;
                    tpoint.y = obj.entities[i].yp;
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(flipsprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);

                    tpoint.x = obj.entities[i].xp+32;
                    tpoint.y = obj.entities[i].yp;
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(flipsprites[obj.entities[i].drawframe +1],NULL, backBuffer, &drawRect, ct);

                    tpoint.x = obj.entities[i].xp;
                    tpoint.y = obj.entities[i].yp+32;
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(flipsprites[obj.entities[i].drawframe+ 12],NULL, backBuffer, &drawRect, ct);

                    tpoint.x = obj.entities[i].xp+32;
                    tpoint.y = obj.entities[i].yp+32;
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(flipsprites[obj.entities[i].drawframe+ 13],NULL, backBuffer, &drawRect, ct);
                }
                else
                {
                    setcol(obj.entities[i].colour, help);

                    tpoint.x = obj.entities[i].xp;
                    tpoint.y = obj.entities[i].yp;

                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(sprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);

                    tpoint.x = obj.entities[i].xp+32;
                    tpoint.y = obj.entities[i].yp;
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(sprites[obj.entities[i].drawframe+1],NULL, backBuffer, &drawRect, ct);

                    tpoint.x = obj.entities[i].xp;
                    tpoint.y = obj.entities[i].yp+32;
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(sprites[obj.entities[i].drawframe+12],NULL, backBuffer, &drawRect, ct);

                    tpoint.x = obj.entities[i].xp+32;
                    tpoint.y = obj.entities[i].yp+32;
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(sprites[obj.entities[i].drawframe + 13],NULL, backBuffer, &drawRect, ct);
                }
            }
            else if (obj.entities[i].size == 10)         // 2x1 Sprite
            {
                if (flipmode)
                {
                    setcol(obj.entities[i].colour, help);

                    tpoint.x = obj.entities[i].xp;
                    tpoint.y = obj.entities[i].yp;
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(flipsprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);

                    tpoint.x = obj.entities[i].xp+32;
                    tpoint.y = obj.entities[i].yp;
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(flipsprites[obj.entities[i].drawframe+1],NULL, backBuffer, &drawRect, ct);
                }
                else
                {
                    setcol(obj.entities[i].colour, help);

                    tpoint.x = obj.entities[i].xp;
                    tpoint.y = obj.entities[i].yp;
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(sprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);

                    tpoint.x = obj.entities[i].xp+32;
                    tpoint.y = obj.entities[i].yp;
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(sprites[obj.entities[i].drawframe+1],NULL, backBuffer, &drawRect, ct);
                }
            }
            else if (obj.entities[i].size == 11)    //The fucking elephant
            {
				//TODO elephant bug
                setcol(obj.entities[i].colour, help);
                drawimagecol(3, obj.entities[i].xp, obj.entities[i].yp);
            }
            else if (obj.entities[i].size == 12)         // Regular sprites that don't wrap
            {
                if (flipmode)
                {
                    //forget this for a minute;
                    tpoint.x = obj.entities[i].xp;
                    tpoint.y = obj.entities[i].yp;
                    setcol(obj.entities[i].colour, help);
                    //

                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(flipsprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);

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
                        setcol(23, help);

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
                        setcol(23, help);
                        //

                        drawRect = tiles_rect;
                        drawRect.x += tpoint.x;
                        drawRect.y += tpoint.y;
                        BlitSurfaceColoured(tiles[1166],NULL, backBuffer, &drawRect, ct);
                    }
                }
                else
                {
                    tpoint.x = obj.entities[i].xp;
                    tpoint.y = obj.entities[i].yp;
                    setcol(obj.entities[i].colour, help);
                    //
                    drawRect = sprites_rect;
                    drawRect.x += tpoint.x;
                    drawRect.y += tpoint.y;
                    BlitSurfaceColoured(sprites[obj.entities[i].drawframe],NULL, backBuffer, &drawRect, ct);


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
                        setcol(23, help);


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
                        setcol(23, help);
                        //

                        drawRect = tiles_rect;
                        drawRect.x += tpoint.x;
                        drawRect.y += tpoint.y;
                        BlitSurfaceColoured(tiles[1166],NULL, backBuffer, &drawRect, ct);
                    }
                }
            }
            else if (obj.entities[i].size == 13)
            {
                 //Special for epilogue: huge hero!

                	if (flipmode) {



                		//scaleMatrix.scale(6, 6);
                		//bigbuffer.fillRect(bigbuffer.rect, 0x000000);
						FillRect(tempBuffer, 0x000000);

                		tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
                		setcol(obj.entities[i].colour, help);
                		//flipsprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
                		//bigbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, new Point(0, 0));
						SDL_Rect drawRect = {Sint16(obj.entities[i].xp ), Sint16(obj.entities[i].yp), sprites_rect.x, sprites_rect.y   };
						SDL_Surface* TempSurface = ScaleSurface( flipsprites[obj.entities[i].drawframe], 6* sprites_rect.w,6* sprites_rect.w );
						BlitSurfaceColoured(TempSurface, NULL , backBuffer,  &drawRect, ct );
						SDL_FreeSurface(TempSurface);
                		//scaleMatrix.translate(-obj.entities[i].xp, -obj.entities[i].yp);
                	}
					else
					{
						//TODO checkthis
						tpoint.x = obj.entities[i].xp; tpoint.y = obj.entities[i].yp;
						setcol(obj.entities[i].colour, help);
						//flipsprites[obj.entities[i].drawframe].colorTransform(sprites_rect, ct);
						//bigbuffer.copyPixels(flipsprites[obj.entities[i].drawframe], sprites_rect, new Point(0, 0));
						SDL_Rect drawRect = {Sint16(obj.entities[i].xp ), Sint16(obj.entities[i].yp), Sint16(sprites_rect.x * 6), Sint16(sprites_rect.y * 6 ) };
						SDL_Surface* TempSurface = ScaleSurface( flipsprites[obj.entities[i].drawframe], 6 * sprites_rect.w,6* sprites_rect.h );
						BlitSurfaceColoured(TempSurface, NULL , backBuffer,  &drawRect, ct );
						SDL_FreeSurface(TempSurface);
                	}



            }
        }
    }
}

void Graphics::drawbackground( int t, mapclass& map )
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
                //backbuffer.fillRect(stars[i], 0x222222);
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
            //TODO Scroll?
            //towerbuffer.scroll( -3, 0);
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
                    drawtowertile((i * 16) - backoffset, (j * 16), temp+40);
                    drawtowertile((i * 16) - backoffset + 8, (j * 16), temp + 41);
                    drawtowertile((i * 16) - backoffset, (j * 16) + 8, temp + 80);
                    drawtowertile((i * 16) - backoffset + 8, (j * 16) + 8, temp + 81);
                }
            }
            backgrounddrawn = true;
        }
        //TODO this is why map breaks

        //backbuffer.copyPixels(towerbuffer, towerbuffer.rect, tl);
        BlitSurfaceStandard(towerbuffer, NULL, backBuffer, NULL);
        break;
    case 4: //Warp zone (vertical)
        backoffset+=3;
        if (backoffset >= 16) backoffset -= 16;

        if (backgrounddrawn)
        {
            //TODO scroll?!
            //towerbuffer.scroll(0, -3);
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
                    drawtowertile((i * 16), (j * 16)- backoffset, temp+40);
                    drawtowertile((i * 16)+ 8, (j * 16)- backoffset, temp + 41);
                    drawtowertile((i * 16), (j * 16)- backoffset + 8, temp + 80);
                    drawtowertile((i * 16)+ 8, (j * 16)- backoffset + 8, temp + 81);
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
        //TODO
        //backbuffer.copyPixels(backgrounds[t], bg_rect, tl);
        BlitSurfaceStandard(backgrounds[t], NULL, backBuffer, &bg_rect);

        break;
    }
}

void Graphics::drawmap( mapclass& map )
{
    ///TODO forground once;
    if (!foregrounddrawn)
    {
        FillRect(foregroundBuffer, 0xDEADBEEF);
        if(map.tileset==0)
        {
            for (j = 0; j < 29+map.extrarow; j++)
            {
                for (int i = 0; i < 40; i++)
                {
                    if(map.contents[i + map.vmult[j]]>0) drawforetile(i * 8, j * 8, map.contents[i + map.vmult[j]]);
                }
            }
        }
        else if (map.tileset == 1)
        {
            for (int jt = 0; jt < 29+map.extrarow; jt++)
            {
                for (int it = 0; it < 40; it++)
                {
                    if(map.contents[it + map.vmult[jt]]>0) drawforetile2(it * 8, jt * 8, map.contents[it + map.vmult[jt]]);
                }
            }
        }
        else if (map.tileset == 2)
        {
            for (j = 0; j < 29+map.extrarow; j++)
            {
                for (int i = 0; i < 40; i++)
                {
                    if(map.contents[i + map.vmult[j]]>0) drawforetile3(i * 8, j * 8, map.contents[i + map.vmult[j]],map.rcol);
                }
            }
        }
        foregrounddrawn = true;
    }
    OverlaySurfaceKeyed(foregroundBuffer, backBuffer, 0xDEADBEEF);
    //SDL_BlitSurface(foregroundBuffer, NULL, backBuffer, NULL);

}

void Graphics::drawfinalmap(mapclass & map)
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
		FillRect(foregroundBuffer, 0xDEADBEEF);
		if(map.tileset==0){
			for (int j = 0; j < 29+map.extrarow; j++) {
				for (int i = 0; i < 40; i++) {
					if((map.contents[i + map.vmult[j]])>0)
						drawforetile(i * 8, j * 8, map.finalat(i,j));
				}
			}
		}else if (map.tileset == 1) {
			for (int j = 0; j < 29+map.extrarow; j++) {
				for (int i = 0; i < 40; i++) {
					if((map.contents[i + map.vmult[j]])>0)
						drawforetile2(i * 8, j * 8, map.finalat(i,j));
				}
			}
		}
		foregrounddrawn=true;
	}

	OverlaySurfaceKeyed(foregroundBuffer, backBuffer, 0xDEADBEEF);
}

void Graphics::drawtowermap( mapclass& map )
{
    int temp;
    for (int j = 0; j < 30; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            temp = map.tower.at(i, j, map.ypos);
            if (temp > 0) drawtile3(i * 8, (j * 8) - ((int)map.ypos % 8), temp, map.colstate);
        }
    }
}

void Graphics::drawtowermap_nobackground( mapclass& map )
{
    int temp;
    for (j = 0; j < 30; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            temp = map.tower.at(i, j, map.ypos);
            if (temp > 0 && temp<28) drawtile3(i * 8, (j * 8) - ((int)map.ypos % 8), temp, map.colstate);
        }
    }
}

void Graphics::drawtowerentities( mapclass& map, entityclass& obj, UtilityClass& help )
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
    SDL_Rect trect;

    for (int i = 0; i < obj.nentity; i++)
    {
        if (!obj.entities[i].invis && obj.entities[i].active)
        {
            if (obj.entities[i].size == 0)        // Sprites
            {
				trinketcolset = false;
                tpoint.x = obj.entities[i].xp;
                tpoint.y = obj.entities[i].yp-map.ypos;
                setcol(obj.entities[i].colour, help);
                setRect(trect, tpoint.x, tpoint.y, sprites_rect.w, sprites_rect.h);
                BlitSurfaceColoured(sprites[obj.entities[i].drawframe], NULL, backBuffer, &trect, ct);
                //screenwrapping!
                if (!map.minitowermode)
                {
                    if ( map.ypos >= 500 && map.ypos <= 5000)   //The "wrapping" area of the tower
                    {
                        if (tpoint.x < 0)
                        {
                            tpoint.x += 320;
                            setRect(trect, tpoint.x, tpoint.y, sprites_rect.w, sprites_rect.h);
                            BlitSurfaceColoured(sprites[obj.entities[i].drawframe], NULL, backBuffer, &trect, ct);
                        }
                        if (tpoint.x > 300)
                        {
                            tpoint.x -= 320;
                            setRect(trect,  tpoint.x, tpoint.y, sprites_rect.w, sprites_rect.h);
                            BlitSurfaceColoured(sprites[obj.entities[i].drawframe], NULL, backBuffer, &trect, ct);
                        }
                    }
                }
            }
            else if (obj.entities[i].size == 1)
            {
                // Tiles
                tpoint.x = obj.entities[i].xp;
                tpoint.y = obj.entities[i].yp-map.ypos;
                setRect(trect,tiles_rect.w, tiles_rect.h, tpoint.x, tpoint.y);
                BlitSurfaceColoured(tiles[obj.entities[i].drawframe], NULL, backBuffer, &trect, ct);
            }
            else if (obj.entities[i].size == 2)
            {
                // Special: Moving platform, 4 tiles
                tpoint.x = obj.entities[i].xp;
                tpoint.y = obj.entities[i].yp-map.ypos;
                setRect(trect,tiles_rect.w, tiles_rect.h, tpoint.x, tpoint.y);
                BlitSurfaceColoured(tiles[obj.entities[i].drawframe], NULL, backBuffer, &trect, ct);
                tpoint.x += 8;
                setRect(trect,sprites_rect.w, sprites_rect.h, tpoint.x, tpoint.y);
                BlitSurfaceColoured(tiles[obj.entities[i].drawframe], NULL, backBuffer, &trect, ct);
                tpoint.x += 8;
                setRect(trect,sprites_rect.w, sprites_rect.h, tpoint.x, tpoint.y);
                BlitSurfaceColoured(tiles[obj.entities[i].drawframe], NULL, backBuffer, &trect, ct);
                tpoint.x += 8;
                setRect(trect,sprites_rect.w, sprites_rect.h, tpoint.x, tpoint.y);
                BlitSurfaceColoured(tiles[obj.entities[i].drawframe], NULL, backBuffer, &trect, ct);

            }
            else if (obj.entities[i].size == 3)    // Big chunky pixels!
            {
                prect.x = obj.entities[i].xp;
                prect.y = obj.entities[i].yp-map.ypos;
                //A seperate index of colours, for simplicity
                if(obj.entities[i].colour==1)
                {
                    FillRect(backBuffer, prect, getRGB(196 - (fRandom() * 64), 10, 10));
                }
                else if (obj.entities[i].colour == 2)
                {
                    FillRect(backBuffer, prect, getRGB(160- help.glow/2 - (fRandom()*20), 200- help.glow/2, 220 - help.glow));
                }
            }
            else if (obj.entities[i].size == 4)    // Small pickups
            {
                drawhuetile(obj.entities[i].xp, obj.entities[i].yp-map.ypos, obj.entities[i].tile, obj.entities[i].colour);
            }
            else if (obj.entities[i].size == 5)    //Horizontal Line
            {
                line_rect.x = obj.entities[i].xp;
                line_rect.y = obj.entities[i].yp-map.ypos;
                line_rect.w = obj.entities[i].w;
                line_rect.h = 1;
                drawgravityline(i, obj);
            }
            else if (obj.entities[i].size == 6)    //Vertical Line
            {
                line_rect.x = obj.entities[i].xp;
                line_rect.y = obj.entities[i].yp-map.ypos;
                line_rect.w = 1;
                line_rect.h = obj.entities[i].h;
                drawgravityline(i, obj);
            }
        }
    }
}

void Graphics::drawtowerspikes( mapclass& map )
{
    for (int i = 0; i < 40; i++)
    {
        drawtile3(i * 8, -8+map.spikeleveltop, 9, map.colstate);
        drawtile3(i * 8, 230-map.spikelevelbottom, 8, map.colstate);
    }
}

void Graphics::drawtowerbackgroundsolo( mapclass& map )
{
    if (map.bypos < 0)
    {
        map.bypos += 120 * 8;
    }

    int temp = 0;

    if (map.tdrawback)
    {
        //Draw the whole thing; needed for every colour cycle!
        for (int j = 0; j < 31; j++)
        {
            for (int  i = 0; i < 40; i++)
            {
                temp = map.tower.backat(i, j, map.bypos);
                drawtowertile3(i * 8, (j * 8) - (map.bypos % 8), temp, map.colstate);
            }
        }
        SDL_BlitSurface(towerbuffer,NULL, backBuffer,NULL);
        map.tdrawback = false;
    }
    else
    {
        //just update the bottom
        ScrollSurface(towerbuffer,0, -map.bscroll);
        for (int i = 0; i < 40; i++)
        {
            temp = map.tower.backat(i, 0, map.bypos);
            drawtowertile3(i * 8, -(map.bypos % 8), temp, map.colstate);
        }

        SDL_BlitSurface(towerbuffer, NULL, backBuffer,NULL);
    }
}

void Graphics::drawtowerbackground( mapclass& map )
{
    //TODO
    int temp;

    if (map.bypos < 0) map.bypos += 120 * 8;

    if (map.scrolldir == 1) map.tdrawback = true;

    if (map.tdrawback)
    {
        //Draw the whole thing; needed for every colour cycle!
        for (j = 0; j < 30; j++)
        {
            for (int i = 0; i < 40; i++)
            {
                temp = map.tower.backat(i, j, map.bypos);
                drawtowertile3(i * 8, (j * 8) - (map.bypos % 8), temp, map.colstate);
            }
        }

        //backbuffer.copyPixels(towerbuffer, towerbuffer.rect, tl, null, null, false);
        SDL_BlitSurface(towerbuffer,NULL, backBuffer,NULL);

        map.tdrawback = false;
    }
    else
    {
        //just update the bottom
        //TODO SCOLL
        //towerbuffer.scroll(0, -map.bscroll);
        ScrollSurface(towerbuffer, 0, -map.bscroll);
        for (int i = 0; i < 40; i++)
        {
            temp = map.tower.backat(i, 0, map.bypos);
            drawtowertile3(i * 8, -(map.bypos % 8), temp, map.colstate);
        }

        //backbuffer.copyPixels(towerbuffer, towerbuffer.rect, tl, null, null, false);
        SDL_BlitSurface(towerbuffer,NULL, backBuffer,NULL);
    }
}

void Graphics::setcol( int t, UtilityClass& help )
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
	//ct.color = endian_swap(ct.color);
}

void Graphics::menuoffrender()
{
	//TODO
	//screenbuffer.lock();
	//screenbuffer.copyPixels(menubuffer, menubuffer.rect, tl, null, null, false);
	//screenbuffer->UpdateScreen(menubuffer,NULL);
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

		//screenbuffer.copyPixels(backbuffer, backbuffer.rect, new Point(0, menuoffset), null, null, false);
		SDL_Rect offsetRect;
		setRect (offsetRect, 0, menuoffset, backBuffer->w ,backBuffer->h);
		BlitSurfaceStandard(menubuffer,NULL,backBuffer,&offsetRect);
	}

	//screenbuffer.unlock();
	SDL_Rect rect;
	setRect(rect, 0, 0, backBuffer->w, backBuffer->h);
	screenbuffer->UpdateScreen(backBuffer,&rect);
	//backbuffer.lock();
	//backbuffer.fillRect(backbuffer.rect, 0x000000);
	FillRect(backBuffer, 0x000000);
	//backbuffer.unlock();
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
		int i = ntextbox - 1;
		while (i >= 0 && !textbox[i].active)
		{
			ntextbox--;
			i--;
		}
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
	//screenbuffer.lock();
	if(flipmode)
	{
		//	tpoint.x = int((Math.random() * 7) - 4); tpoint.y = int((Math.random() * 7) - 4);
		//	flipmatrix.translate(tpoint.x, tpoint.y);
		//	screenbuffer.draw(backbuffer, flipmatrix);
		//	flipmatrix.translate(-tpoint.x, -tpoint.y);

		screenbuffer->ClearScreen(0x000);
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
		screenbuffer->ClearScreen(0x000);
		tpoint.x =  static_cast<Sint32>((fRandom() * 7) - 4);
		tpoint.y =  static_cast<Sint32>((fRandom() * 7) - 4);
		SDL_Rect shakeRect;
		setRect(shakeRect,tpoint.x, tpoint.y, backBuffer->w, backBuffer->h);
		screenbuffer->UpdateScreen( backBuffer, &shakeRect);
		// screenbuffer.copyPixels(backbuffer, backbuffer.rect, tpoint, null, null, false);
	}
	//screenbuffer.unlock();

	//backbuffer.lock();
	FillRect(backBuffer, 0x000000 );
	//backbuffer.fillRect(backbuffer.rect, 0x000000);
	//backbuffer.unlock();
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
	for (size_t i = 0; i < t.length(); i++)
	{
		cur = (t.c_str())[i];
		if (cur > 255 || cur < 0)
		{
			cur = '?';
		}
		if (flipmode)
		{
			SDL_Surface* tempPrint = ScaleSurfaceSlow(flipbfont[cur], bfont[cur]->w *sc,bfont[cur]->h *sc);
			SDL_Rect printrect = { Sint16(x + bfontpos), Sint16(y) , Sint16(bfont_rect.w*sc), Sint16(bfont_rect.h * sc)};
			BlitSurfaceColoured(tempPrint, NULL, backBuffer, &printrect ,ct);
			SDL_FreeSurface(tempPrint);
		}
		else
		{
			SDL_Surface* tempPrint = ScaleSurfaceSlow(bfont[cur], bfont[cur]->w *sc,bfont[cur]->h *sc);
			SDL_Rect printrect = { Sint16((x) + bfontpos), Sint16(y) , Sint16(bfont_rect.w*sc), Sint16(bfont_rect.h * sc)};
			BlitSurfaceColoured(tempPrint, NULL, backBuffer, &printrect, ct);
			SDL_FreeSurface(tempPrint);
		}
		bfontpos+=bfontlen[cur]* sc;
	}
}

void Graphics::drawtele(int x, int y, int t, int c, UtilityClass& help)
{
	setcolreal(getRGB(16,16,16));

	SDL_Rect telerect;
	setRect(telerect, x , y, tele_rect.w, tele_rect.h );
	BlitSurfaceColoured(tele[0], NULL, backBuffer, &telerect, ct);

	setcol(c, help);
	if (t > 9) t = 8;
	if (t < 0) t = 0;

	BlitSurfaceColoured(tele[t], NULL, backBuffer, &telerect, ct);
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

void Graphics::drawtile(int x, int y, int t)
{
	SDL_Rect drawRect;
	setRect(drawRect,x,y, tiles_rect.w, tiles_rect.h  );
	BlitSurfaceStandard(tiles[t] ,NULL, backBuffer,&drawRect);
}

void Graphics::drawforetile(int x, int y, int t)
{
	//frontbuffer.copyPixels(tiles[t], tiles_rect, tpoint);
	SDL_Rect rect;
	setRect(rect, x,y,tiles_rect.w, tiles_rect.h);
	BlitSurfaceStandard(tiles[t],NULL, foregroundBuffer, &rect  );
}

void Graphics::drawforetile2(int x, int y, int t)
{
	//frontbuffer.copyPixels(tiles2[t], tiles_rect, tpoint);
	SDL_Rect rect;
	setRect(rect, x,y,tiles_rect.w, tiles_rect.h);
	BlitSurfaceStandard(tiles2[t],NULL, foregroundBuffer, &rect  );
}

void Graphics::drawforetile3(int x, int y, int t, int off)
{
	SDL_Rect rect;
	setRect(rect, x,y,tiles_rect.w, tiles_rect.h);
	BlitSurfaceStandard(tiles3[t+(off*30)],NULL, foregroundBuffer, &rect  );
	//frontbuffer.copyPixels(tiles3[t+(off*30)], tiles_rect, tpoint);
}

void Graphics::drawrect(int x, int y, int w, int h, int r, int g, int b)
{
	SDL_Rect madrect;
	//Draw the retangle indicated by that object
	madrect.x = x;
	madrect.y = y;
	madrect.w = w;
	madrect.h = 1;
	//backbuffer.fillRect(madrect, RGB(r,g,b));
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
