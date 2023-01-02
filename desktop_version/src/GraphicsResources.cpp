#include "GraphicsResources.h"

#include "Alloc.h"
#include "FileSystemUtils.h"
#include "GraphicsUtil.h"
#include "Vlogging.h"
#include "Screen.h"

// Used to load PNG data
extern "C"
{
    extern unsigned lodepng_decode32(
        unsigned char** out,
        unsigned* w,
        unsigned* h,
        const unsigned char* in,
        size_t insize
    );
    extern const char* lodepng_error_text(unsigned code);
}

static SDL_Surface* LoadImageRaw(const char* filename, unsigned char** data)
{
    //Temporary storage for the image that's loaded
    SDL_Surface* loadedImage = NULL;

    unsigned int width, height;
    unsigned int error;

    unsigned char* fileIn;
    size_t length;
    FILESYSTEM_loadAssetToMemory(filename, &fileIn, &length, false);
    if (fileIn == NULL)
    {
        SDL_assert(0 && "Image file missing!");
        return NULL;
    }
    error = lodepng_decode32(data, &width, &height, fileIn, length);
    VVV_free(fileIn);

    if (error != 0)
    {
        vlog_error("Could not load %s: %s", filename, lodepng_error_text(error));
        return NULL;
    }

    loadedImage = SDL_CreateRGBSurfaceWithFormatFrom(
        *data,
        width,
        height,
        32,
        width * 4,
        SDL_PIXELFORMAT_ABGR8888
    );

    return loadedImage;
}

static SDL_Surface* LoadSurfaceFromRaw(SDL_Surface* loadedImage)
{
    SDL_Surface* optimizedImage = SDL_ConvertSurfaceFormat(
        loadedImage,
        SDL_PIXELFORMAT_ARGB8888,
        0
    );
    SDL_SetSurfaceBlendMode(optimizedImage, SDL_BLENDMODE_BLEND);
    return optimizedImage;
}

/* Can't be static, used in Screen.h */
SDL_Surface* LoadImageSurface(const char* filename)
{
    unsigned char* data;

    SDL_Surface* loadedImage = LoadImageRaw(filename, &data);

    SDL_Surface* optimizedImage = LoadSurfaceFromRaw(loadedImage);
    if (loadedImage != NULL)
    {
        VVV_freefunc(SDL_FreeSurface, loadedImage);
    }

    VVV_free(data);

    if (optimizedImage == NULL)
    {
        VVV_free(data);
        vlog_error("Image not found: %s", filename);
        SDL_assert(0 && "Image not found! See stderr.");
    }

    return optimizedImage;
}

static SDL_Texture* LoadTextureFromRaw(const char* filename, SDL_Surface* loadedImage, const TextureLoadType loadtype)
{
    if (loadedImage == NULL)
    {
        return NULL;
    }

    // Modify the surface with the load type.
    // This could be done in LoadImageRaw, however currently, surfaces are only used for
    // pixel perfect collision (which will be changed later) and the window icon.

    switch (loadtype)
    {
    case TEX_WHITE:
        SDL_LockSurface(loadedImage);
        for (int y = 0; y < loadedImage->h; y++)
        {
            for (int x = 0; x < loadedImage->w; x++)
            {
                SDL_Color color = ReadPixel(loadedImage, x, y);
                color.r = 255;
                color.g = 255;
                color.b = 255;
                DrawPixel(loadedImage, x, y, color);
            }
        }
        SDL_UnlockSurface(loadedImage);
        break;
    case TEX_GRAYSCALE:
        SDL_LockSurface(loadedImage);
        for (int y = 0; y < loadedImage->h; y++)
        {
            for (int x = 0; x < loadedImage->w; x++)
            {
                SDL_Color color = ReadPixel(loadedImage, x, y);

                // Magic numbers used for grayscaling (eyes perceive certain colors brighter than others)
                Uint8 r = color.r * 0.299;
                Uint8 g = color.g * 0.587;
                Uint8 b = color.b * 0.114;

                const double gray = SDL_floor(r + g + b + 0.5);

                color.r = gray;
                color.g = gray;
                color.b = gray;

                DrawPixel(loadedImage, x, y, color);
            }
        }
        SDL_UnlockSurface(loadedImage);
        break;
    default:
        break;
    }

    //Create texture from surface pixels
    SDL_Texture* texture = SDL_CreateTextureFromSurface(gameScreen.m_renderer, loadedImage);
    if (texture == NULL)
    {
        vlog_error("Failed creating texture: %s. SDL error: %s\n", filename, SDL_GetError());
    }

    return texture;
}

SDL_Texture* LoadImage(const char *filename, const TextureLoadType loadtype)
{
    unsigned char* data;

    SDL_Surface* loadedImage = LoadImageRaw(filename, &data);

    SDL_Texture* texture = LoadTextureFromRaw(filename, loadedImage, loadtype);

    if (loadedImage != NULL)
    {
        VVV_freefunc(SDL_FreeSurface, loadedImage);
    }

    VVV_free(data);

    if (texture == NULL)
    {
        vlog_error("Image not found: %s", filename);
        SDL_assert(0 && "Image not found! See stderr.");
    }

    return texture;
}

static SDL_Texture* LoadImage(const char* filename)
{
    return LoadImage(filename, TEX_COLOR);
}

/* Any unneeded variants can be NULL */
static void LoadVariants(const char* filename, SDL_Texture** colored, SDL_Texture** white, SDL_Texture** grayscale)
{
    unsigned char* data;
    SDL_Surface* loadedImage = LoadImageRaw(filename, &data);

    if (colored != NULL)
    {
        *colored = LoadTextureFromRaw(filename, loadedImage, TEX_COLOR);
        if (*colored == NULL)
        {
            vlog_error("Image not found: %s", filename);
            SDL_assert(0 && "Image not found! See stderr.");
        }
    }

    if (grayscale != NULL)
    {
        *grayscale = LoadTextureFromRaw(filename, loadedImage, TEX_GRAYSCALE);
        if (*grayscale == NULL)
        {
            vlog_error("Image not found: %s", filename);
            SDL_assert(0 && "Image not found! See stderr.");
        }
    }

    if (white != NULL)
    {
        *white = LoadTextureFromRaw(filename, loadedImage, TEX_WHITE);
        if (*white == NULL)
        {
            vlog_error("Image not found: %s", filename);
            SDL_assert(0 && "Image not found! See stderr.");
        }
    }

    if (loadedImage != NULL)
    {
        VVV_freefunc(SDL_FreeSurface, loadedImage);
    }

    VVV_free(data);
}

/* The pointers `texture` and `surface` cannot be NULL */
static void LoadSprites(const char* filename, SDL_Texture** texture, SDL_Surface** surface)
{
    unsigned char* data;
    SDL_Surface* loadedImage = LoadImageRaw(filename, &data);

    *texture = LoadTextureFromRaw(filename, loadedImage, TEX_WHITE);
    if (*texture == NULL)
    {
        vlog_error("Image not found: %s", filename);
        SDL_assert(0 && "Image not found! See stderr.");
    }

    *surface = LoadSurfaceFromRaw(loadedImage);
    if (*surface == NULL)
    {
        vlog_error("Image not found: %s", filename);
        SDL_assert(0 && "Image not found! See stderr.");
    }

    if (loadedImage != NULL)
    {
        VVV_freefunc(SDL_FreeSurface, loadedImage);
    }

    VVV_free(data);
}

void GraphicsResources::init(void)
{
    LoadVariants("graphics/tiles.png", &im_tiles, &im_tiles_white, &im_tiles_tint);
    LoadVariants("graphics/tiles2.png", &im_tiles2, NULL, &im_tiles2_tint);
    LoadVariants("graphics/entcolours.png", &im_entcolours, NULL, &im_entcolours_tint);

    LoadSprites("graphics/sprites.png", &im_sprites, &im_sprites_surf);
    LoadSprites("graphics/flipsprites.png", &im_flipsprites, &im_flipsprites_surf);

    im_tiles3 = LoadImage("graphics/tiles3.png");
    im_teleporter = LoadImage("graphics/teleporter.png", TEX_WHITE);

    im_image0 = LoadImage("graphics/levelcomplete.png");
    im_image1 = LoadImage("graphics/minimap.png");
    im_image2 = LoadImage("graphics/covered.png");
    im_image3 = LoadImage("graphics/elephant.png", TEX_WHITE);
    im_image4 = LoadImage("graphics/gamecomplete.png");
    im_image5 = LoadImage("graphics/fliplevelcomplete.png");
    im_image6 = LoadImage("graphics/flipgamecomplete.png");
    im_image7 = LoadImage("graphics/site.png", TEX_WHITE);
    im_image8 = LoadImage("graphics/site2.png", TEX_WHITE);
    im_image9 = LoadImage("graphics/site3.png", TEX_WHITE);
    im_image10 = LoadImage("graphics/ending.png");
    im_image11 = LoadImage("graphics/site4.png", TEX_WHITE);

    im_image12 = SDL_CreateTexture(gameScreen.m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 240, 180);

    if (im_image12 == NULL)
    {
        vlog_error("Failed to create minimap texture: %s", SDL_GetError());
        SDL_assert(0 && "Failed to create minimap texture! See stderr.");
        return;
    }
}


void GraphicsResources::destroy(void)
{
#define CLEAR(img) VVV_freefunc(SDL_DestroyTexture, img)
    CLEAR(im_tiles);
    CLEAR(im_tiles_white);
    CLEAR(im_tiles_tint);
    CLEAR(im_tiles2);
    CLEAR(im_tiles2_tint);
    CLEAR(im_tiles3);
    CLEAR(im_entcolours);
    CLEAR(im_entcolours_tint);
    CLEAR(im_sprites);
    CLEAR(im_flipsprites);
    CLEAR(im_teleporter);

    CLEAR(im_image0);
    CLEAR(im_image1);
    CLEAR(im_image2);
    CLEAR(im_image3);
    CLEAR(im_image4);
    CLEAR(im_image5);
    CLEAR(im_image6);
    CLEAR(im_image7);
    CLEAR(im_image8);
    CLEAR(im_image9);
    CLEAR(im_image10);
    CLEAR(im_image11);
    CLEAR(im_image12);
#undef CLEAR

    VVV_freefunc(SDL_FreeSurface, im_sprites_surf);
    VVV_freefunc(SDL_FreeSurface, im_flipsprites_surf);
}
