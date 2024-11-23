#ifndef GRAPHICSRESOURCES_H
#define GRAPHICSRESOURCES_H

#include <SDL.h>

enum TextureLoadType
{
    TEX_COLOR,
    TEX_WHITE,
    TEX_GRAYSCALE
};

class GraphicsResources
{
public:
    void init(void);
    void destroy(void);

    void init_translations(void);

    SDL_Surface* im_sprites_surf;
    SDL_Surface* im_flipsprites_surf;

    SDL_Texture* im_tiles;
    SDL_Texture* im_tiles_white;
    SDL_Texture* im_tiles_tint;
    SDL_Texture* im_tiles2;
    SDL_Texture* im_tiles2_tint;
    SDL_Texture* im_tiles3;
    SDL_Texture* im_entcolours;
    SDL_Texture* im_entcolours_tint;
    SDL_Texture* im_sprites;
    SDL_Texture* im_flipsprites;
    SDL_Texture* im_teleporter;
    SDL_Texture* im_image0;
    SDL_Texture* im_image1;
    SDL_Texture* im_image2;
    SDL_Texture* im_image3;
    SDL_Texture* im_image4;
    SDL_Texture* im_image5;
    SDL_Texture* im_image6;
    SDL_Texture* im_image7;
    SDL_Texture* im_image8;
    SDL_Texture* im_image9;
    SDL_Texture* im_image10;
    SDL_Texture* im_image11;
    SDL_Texture* im_image12;

    SDL_Texture* im_sprites_translated;
    SDL_Texture* im_flipsprites_translated;

    /* Touch */
    SDL_Texture* im_button_left;
    SDL_Texture* im_button_right;
    SDL_Texture* im_button_map;
};

SDL_Surface* LoadImageSurface(const char* filename);
SDL_Texture* LoadImage(const char *filename, TextureLoadType loadtype);

bool SaveImage(const SDL_Surface* surface, const char* filename);
bool SaveScreenshot(void);

#endif /* GRAPHICSRESOURCES_H */
