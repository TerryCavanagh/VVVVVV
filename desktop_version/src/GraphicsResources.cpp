#include "GraphicsResources.h"
#include "FileSystemUtils.h"
#include <stdio.h>
#include <stdlib.h>

// Used to load PNG data
extern "C"
{
	extern unsigned lodepng_decode24(
		unsigned char** out,
		unsigned* w,
		unsigned* h,
		const unsigned char* in,
		size_t insize
	);
	extern unsigned lodepng_decode32(
		unsigned char** out,
		unsigned* w,
		unsigned* h,
		const unsigned char* in,
		size_t insize
	);
}

SDL_Surface* LoadImage(const char *filename, bool noBlend = true, bool noAlpha = false)
{
	//Temporary storage for the image that's loaded
	SDL_Surface* loadedImage = NULL;
	//The optimized image that will be used
	SDL_Surface* optimizedImage = NULL;

	unsigned char *data;
	unsigned int width, height;

	unsigned char *fileIn = NULL;
	size_t length = 0;
	FILESYSTEM_loadFileToMemory(filename, &fileIn, &length);
	if (noAlpha)
	{
		lodepng_decode24(&data, &width, &height, fileIn, length);
	}
	else
	{
		lodepng_decode32(&data, &width, &height, fileIn, length);
	}
	FILESYSTEM_freeMemory(&fileIn);

	loadedImage = SDL_CreateRGBSurfaceFrom(
		data,
		width,
		height,
		noAlpha ? 24 : 32,
		width * (noAlpha ? 3 : 4),
		0x000000FF,
		0x0000FF00,
		0x00FF0000,
		noAlpha ? 0x00000000 : 0xFF000000
	);

	if (loadedImage != NULL)
	{
		optimizedImage = SDL_ConvertSurfaceFormat(
			loadedImage,
			SDL_PIXELFORMAT_ABGR8888, // FIXME: Format? -flibit
			0
		);
		SDL_FreeSurface( loadedImage );
		free(data);
		if (noBlend)
		{
			SDL_SetSurfaceBlendMode(optimizedImage, SDL_BLENDMODE_BLEND);
		}
		return optimizedImage;
	}
	else
	{
		fprintf(stderr,"Image not found: %s\n", filename);
		SDL_assert(0 && "Image not found! See stderr.");
		return NULL;
	}
}

void GraphicsResources::init(void)
{
	im_tiles =		LoadImage("graphics/tiles.png");
	im_tiles2 =		LoadImage("graphics/tiles2.png");
	im_tiles3 =		LoadImage("graphics/tiles3.png");
	im_entcolours =		LoadImage("graphics/entcolours.png");
	im_sprites =		LoadImage("graphics/sprites.png");
	im_flipsprites =	LoadImage("graphics/flipsprites.png");
	im_bfont =		LoadImage("graphics/font.png");
	im_bfontmask =		LoadImage("graphics/fontmask.png");
	im_teleporter =		LoadImage("graphics/teleporter.png");

	im_image0 =		LoadImage("graphics/levelcomplete.png", false);
	im_image1 =		LoadImage("graphics/minimap.png", true, true);
	im_image2 =		LoadImage("graphics/covered.png", true, true);
	im_image3 =		LoadImage("graphics/elephant.png");
	im_image4 =		LoadImage("graphics/gamecomplete.png", false);
	im_image5 =		LoadImage("graphics/fliplevelcomplete.png", false);
	im_image6 =		LoadImage("graphics/flipgamecomplete.png", false);
	im_image7 =		LoadImage("graphics/site.png", false);
	im_image8 =		LoadImage("graphics/site2.png");
	im_image9 =		LoadImage("graphics/site3.png");
	im_image10 =		LoadImage("graphics/ending.png");
	im_image11 =		LoadImage("graphics/site4.png");
	im_image12 =		LoadImage("graphics/minimap.png");
}


void GraphicsResources::destroy(void)
{
	SDL_FreeSurface(im_tiles);
	SDL_FreeSurface(im_tiles2);
	SDL_FreeSurface(im_tiles3);
	SDL_FreeSurface(im_entcolours);
	SDL_FreeSurface(im_sprites);
	SDL_FreeSurface(im_flipsprites);
	SDL_FreeSurface(im_bfont);
	SDL_FreeSurface(im_bfontmask);
	SDL_FreeSurface(im_teleporter);

	SDL_FreeSurface(im_image0);
	SDL_FreeSurface(im_image1);
	SDL_FreeSurface(im_image2);
	SDL_FreeSurface(im_image3);
	SDL_FreeSurface(im_image4);
	SDL_FreeSurface(im_image5);
	SDL_FreeSurface(im_image6);
	SDL_FreeSurface(im_image7);
	SDL_FreeSurface(im_image8);
	SDL_FreeSurface(im_image9);
	SDL_FreeSurface(im_image10);
	SDL_FreeSurface(im_image11);
	SDL_FreeSurface(im_image12);
}
