#include "GraphicsResources.h"

#include "Alloc.h"
#include "FileSystemUtils.h"
#include "Vlogging.h"

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

/* Don't declare `static`, this is used elsewhere */
SDL_Surface* LoadImage(const char *filename)
{
    //Temporary storage for the image that's loaded
    SDL_Surface* loadedImage = NULL;
    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

    unsigned char *data;
    unsigned int width, height;
    unsigned int error;

    unsigned char *fileIn;
    size_t length;
    FILESYSTEM_loadAssetToMemory(filename, &fileIn, &length, false);
    if (fileIn == NULL)
    {
        SDL_assert(0 && "Image file missing!");
        return NULL;
    }
    error = lodepng_decode32(&data, &width, &height, fileIn, length);
    VVV_free(fileIn);

    if (error != 0)
    {
        vlog_error("Could not load %s: %s", filename, lodepng_error_text(error));
        return NULL;
    }

    loadedImage = SDL_CreateRGBSurfaceWithFormatFrom(
        data,
        width,
        height,
        32,
        width * 4,
        SDL_PIXELFORMAT_ABGR8888
    );

    if (loadedImage != NULL)
    {
        optimizedImage = SDL_ConvertSurfaceFormat(
            loadedImage,
            SDL_PIXELFORMAT_ARGB8888,
            0
        );
        VVV_freefunc(SDL_FreeSurface, loadedImage);
        VVV_free(data);
        SDL_SetSurfaceBlendMode(optimizedImage, SDL_BLENDMODE_BLEND);
        return optimizedImage;
    }
    else
    {
        VVV_free(data);
        vlog_error("Image not found: %s", filename);
        SDL_assert(0 && "Image not found! See stderr.");
        return NULL;
    }
}

void GraphicsResources::init(void)
{
    im_tiles =        LoadImage("graphics/tiles.png");
    im_tiles2 =        LoadImage("graphics/tiles2.png");
    im_tiles3 =        LoadImage("graphics/tiles3.png");
    im_entcolours =        LoadImage("graphics/entcolours.png");
    im_sprites =        LoadImage("graphics/sprites.png");
    im_flipsprites =    LoadImage("graphics/flipsprites.png");
    im_bfont =        LoadImage("graphics/font.png");
    im_teleporter =        LoadImage("graphics/teleporter.png");

    im_image0 =        LoadImage("graphics/levelcomplete.png");
    im_image1 =        LoadImage("graphics/minimap.png");
    im_image2 =        LoadImage("graphics/covered.png");
    im_image3 =        LoadImage("graphics/elephant.png");
    im_image4 =        LoadImage("graphics/gamecomplete.png");
    im_image5 =        LoadImage("graphics/fliplevelcomplete.png");
    im_image6 =        LoadImage("graphics/flipgamecomplete.png");
    im_image7 =        LoadImage("graphics/site.png");
    im_image8 =        LoadImage("graphics/site2.png");
    im_image9 =        LoadImage("graphics/site3.png");
    im_image10 =        LoadImage("graphics/ending.png");
    im_image11 =        LoadImage("graphics/site4.png");
    im_image12 =        LoadImage("graphics/minimap.png");
}


void GraphicsResources::destroy(void)
{
#define CLEAR(img) VVV_freefunc(SDL_FreeSurface, img)
    CLEAR(im_tiles);
    CLEAR(im_tiles2);
    CLEAR(im_tiles3);
    CLEAR(im_entcolours);
    CLEAR(im_sprites);
    CLEAR(im_flipsprites);
    CLEAR(im_bfont);
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
}
