#pragma once

#include <SDL.h>

class GraphicsResources
{
public:
    void init(void);
    void destroy(void);

    SDL_Surface* im_tiles;
    SDL_Surface* im_tiles2;
    SDL_Surface* im_tiles3;
    SDL_Surface* im_entcolours;
    SDL_Surface* im_sprites;
    SDL_Surface* im_flipsprites;
    SDL_Surface* im_bfont;
    SDL_Surface* im_teleporter;
    SDL_Surface* im_image0;
    SDL_Surface* im_image1;
    SDL_Surface* im_image2;
    SDL_Surface* im_image3;
    SDL_Surface* im_image4;
    SDL_Surface* im_image5;
    SDL_Surface* im_image6;
    SDL_Surface* im_image7;
    SDL_Surface* im_image8;
    SDL_Surface* im_image9;
    SDL_Surface* im_image10;
    SDL_Surface* im_image11;
    SDL_Surface* im_image12;
};
