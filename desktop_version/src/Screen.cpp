#include "Screen.h"

#include <stdlib.h>

#include "FileSystemUtils.h"
#include "GraphicsUtil.h"

// Used to create the window icon
extern "C"
{
	extern unsigned lodepng_decode24(
		unsigned char** out,
		unsigned* w,
		unsigned* h,
		const unsigned char* in,
		size_t insize
	);
}

void Screen::init(
	int windowWidth,
	int windowHeight,
	bool fullscreen,
	bool useVsync,
	int stretch,
	bool linearFilter,
	bool badSignal
) {
	m_window = NULL;
	m_renderer = NULL;
	m_screenTexture = NULL;
	m_screen = NULL;
	isWindowed = !fullscreen;
	stretchMode = stretch;
	isFiltered = linearFilter;
	vsync = useVsync;
	filterSubrect.x = 1;
	filterSubrect.y = 1;
	filterSubrect.w = 318;
	filterSubrect.h = 238;

	SDL_SetHintWithPriority(
		SDL_HINT_RENDER_SCALE_QUALITY,
		isFiltered ? "linear" : "nearest",
		SDL_HINT_OVERRIDE
	);
	SDL_SetHintWithPriority(
		SDL_HINT_RENDER_VSYNC,
		vsync ? "1" : "0",
		SDL_HINT_OVERRIDE
	);

	// Uncomment this next line when you need to debug -flibit
	// SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "software", SDL_HINT_OVERRIDE);
	// FIXME: m_renderer is also created in Graphics::processVsync()!
	SDL_CreateWindowAndRenderer(
		640,
		480,
		SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE,
		&m_window,
		&m_renderer
	);
	SDL_SetWindowTitle(m_window, "VVVVVV");

	LoadIcon();

	// FIXME: This surface should be the actual backbuffer! -flibit
	m_screen = SDL_CreateRGBSurface(
		0,
		320,
		240,
		32,
		0x00FF0000,
		0x0000FF00,
		0x000000FF,
		0xFF000000
	);
	// ALSO FIXME: This SDL_CreateTexture() is duplicated in Graphics::processVsync()!
	m_screenTexture = SDL_CreateTexture(
		m_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		320,
		240
	);

	badSignalEffect = badSignal;

	ResizeScreen(windowWidth, windowHeight);
}

void Screen::LoadIcon()
{
	unsigned char *fileIn = NULL;
	size_t length = 0;
	unsigned char *data;
	unsigned int width, height;
	FILESYSTEM_loadFileToMemory("VVVVVV.png", &fileIn, &length);
	lodepng_decode24(&data, &width, &height, fileIn, length);
	FILESYSTEM_freeMemory(&fileIn);
	SDL_Surface *icon = SDL_CreateRGBSurfaceFrom(
		data,
		width,
		height,
		24,
		width * 3,
		0x000000FF,
		0x0000FF00,
		0x00FF0000,
		0x00000000
	);
	SDL_SetWindowIcon(m_window, icon);
	SDL_FreeSurface(icon);
	free(data);
}

void Screen::ResizeScreen(int x, int y)
{
	static int resX = 320;
	static int resY = 240;
	if (x != -1 && y != -1)
	{
		// This is a user resize!
		resX = x;
		resY = y;
	}

	if(!isWindowed)
	{
		int result = SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		if (result != 0)
		{
			printf("Error: could not set the game to fullscreen mode: %s\n", SDL_GetError());
			return;
		}
	}
	else
	{
		int result = SDL_SetWindowFullscreen(m_window, 0);
		if (result != 0)
		{
			printf("Error: could not set the game to windowed mode: %s\n", SDL_GetError());
			return;
		}
		if (x != -1 && y != -1)
		{
			SDL_SetWindowSize(m_window, resX, resY);
			SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}
	}
	if (stretchMode == 1)
	{
		int winX, winY;
		SDL_GetWindowSize(m_window, &winX, &winY);
		int result = SDL_RenderSetLogicalSize(m_renderer, winX, winY);
		if (result != 0)
		{
			printf("Error: could not set logical size: %s\n", SDL_GetError());
			return;
		}
		result = SDL_RenderSetIntegerScale(m_renderer, SDL_FALSE);
		if (result != 0)
		{
			printf("Error: could not set scale: %s\n", SDL_GetError());
			return;
		}
	}
	else
	{
		SDL_RenderSetLogicalSize(m_renderer, 320, 240);
		int result = SDL_RenderSetIntegerScale(m_renderer, (SDL_bool) (stretchMode == 2));
		if (result != 0)
		{
			printf("Error: could not set scale: %s\n", SDL_GetError());
			return;
		}
	}
	SDL_ShowWindow(m_window);
}

void Screen::ResizeToNearestMultiple()
{
	int w, h;
	GetWindowSize(&w, &h);

	// Check aspect ratio first
	bool using_width;
	int usethisdimension, usethisratio;

	if ((float) w / (float) h > 4.0 / 3.0)
	{
		// Width is bigger, so it's limited by height
		usethisdimension = h;
		usethisratio = 240;
		using_width = false;
	}
	else
	{
		// Height is bigger, so it's limited by width. Or we're exactly 4:3 already
		usethisdimension = w;
		usethisratio = 320;
		using_width = true;
	}

	int floor = (usethisdimension / usethisratio) * usethisratio;
	int ceiling = floor + usethisratio;

	int final_dimension;

	if (usethisdimension - floor < ceiling - usethisdimension)
	{
		// Floor is nearest
		final_dimension = floor;
	}
	else
	{
		// Ceiling is nearest. Or we're exactly on a multiple already
		final_dimension = ceiling;
	}

	if (final_dimension == 0)
	{
		// We're way too small!
		ResizeScreen(320, 240);
		return;
	}

	if (using_width)
	{
		ResizeScreen(final_dimension, final_dimension / 4 * 3);
	}
	else
	{
		ResizeScreen(final_dimension * 4 / 3, final_dimension);
	}
}

void Screen::GetWindowSize(int* x, int* y)
{
	SDL_GetWindowSize(m_window, x, y);
}

void Screen::UpdateScreen(SDL_Surface* buffer, SDL_Rect* rect )
{
	if((buffer == NULL) && (m_screen == NULL) )
	{
		return;
	}

	if(badSignalEffect)
	{
		buffer = ApplyFilter(buffer);
	}


	FillRect(m_screen, 0x000);
	BlitSurfaceStandard(buffer,NULL,m_screen,rect);

	if(badSignalEffect)
	{
		SDL_FreeSurface(buffer);
	}

}

const SDL_PixelFormat* Screen::GetFormat()
{
	return m_screen->format;
}

void Screen::FlipScreen()
{
	SDL_UpdateTexture(
		m_screenTexture,
		NULL,
		m_screen->pixels,
		m_screen->pitch
	);
	SDL_RenderCopy(
		m_renderer,
		m_screenTexture,
		isFiltered ? &filterSubrect : NULL,
		NULL
	);
	SDL_RenderPresent(m_renderer);
	SDL_RenderClear(m_renderer);
	SDL_FillRect(m_screen, NULL, 0x00000000);
}

void Screen::toggleFullScreen()
{
	isWindowed = !isWindowed;
	ResizeScreen(-1, -1);
}

void Screen::toggleStretchMode()
{
	stretchMode = (stretchMode + 1) % 3;
	ResizeScreen(-1, -1);
}

void Screen::toggleLinearFilter()
{
	isFiltered = !isFiltered;
	SDL_SetHintWithPriority(
		SDL_HINT_RENDER_SCALE_QUALITY,
		isFiltered ? "linear" : "nearest",
		SDL_HINT_OVERRIDE
	);
	SDL_DestroyTexture(m_screenTexture);
	m_screenTexture = SDL_CreateTexture(
		m_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		320,
		240
	);
}

void Screen::resetRendererWorkaround()
{
	SDL_SetHintWithPriority(
		SDL_HINT_RENDER_VSYNC,
		vsync ? "1" : "0",
		SDL_HINT_OVERRIDE
	);

	/* FIXME: This exists because SDL_HINT_RENDER_VSYNC is not dynamic!
	 * As a result, our only workaround is to tear down the renderer
	 * and recreate everything so that it can process the variable.
	 * -flibit
	 */

	if (m_renderer == NULL)
	{
		/* We haven't made it to init yet, don't worry about it */
		return;
	}

	SDL_RendererInfo renderInfo;
	SDL_GetRendererInfo(m_renderer, &renderInfo);
	bool curVsync = (renderInfo.flags & SDL_RENDERER_PRESENTVSYNC) != 0;
	if (vsync == curVsync)
	{
		return;
	}

	SDL_DestroyTexture(m_screenTexture);
	SDL_DestroyRenderer(m_renderer);

	m_renderer = SDL_CreateRenderer(m_window, -1, 0);
	m_screenTexture = SDL_CreateTexture(
		m_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		320,
		240
	);

	/* Ugh, have to make sure to re-apply graphics options after doing the
	 * above, otherwise letterbox/integer won't be applied...
	 */
	ResizeScreen(-1, -1);
}
