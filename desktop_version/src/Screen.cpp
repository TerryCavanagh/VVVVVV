#include "Screen.h"

#include "FileSystemUtils.h"
#include "GraphicsUtil.h"

#include <stdlib.h>

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

Screen::Screen()
{
    m_window = NULL;
    m_renderer = NULL;
    m_screenTexture = NULL;
    m_screen = NULL;
    isWindowed = true;
    stretchMode = 0;
    isFiltered = false;
    filterSubrect.x = 1;
    filterSubrect.y = 1;
    filterSubrect.w = 318;
    filterSubrect.h = 238;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	// Uncomment this next line when you need to debug -flibit
	// SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "software", SDL_HINT_OVERRIDE);
	SDL_CreateWindowAndRenderer(
		640,
		480,
		SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE,
		&m_window,
		&m_renderer
	);
	SDL_SetWindowTitle(m_window, "VVVVVV");

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
	m_screenTexture = SDL_CreateTexture(
		m_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		320,
		240
	);

    badSignalEffect = false;

    glScreen = true;
}

int Screen::ResizeScreen(int x, int y)
{
	int result = 0; // 0 is success, nonzero is failure

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
		result = SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else
	{
		result = SDL_SetWindowFullscreen(m_window, 0);
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
		SDL_RenderSetLogicalSize(m_renderer, winX, winY);
		result = SDL_RenderSetIntegerScale(m_renderer, SDL_FALSE);
	}
	else
	{
		SDL_RenderSetLogicalSize(m_renderer, 320, 240);
		result = SDL_RenderSetIntegerScale(m_renderer, (SDL_bool) (stretchMode == 2));
	}
	SDL_ShowWindow(m_window);

	return result;
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

int Screen::toggleFullScreen()
{
	isWindowed = !isWindowed;
	int result = ResizeScreen(-1, -1);
	return result;
}

void Screen::toggleStretchMode()
{
	stretchMode = (stretchMode + 1) % 3;
	ResizeScreen(-1, -1);
}

void Screen::toggleLinearFilter()
{
	isFiltered = !isFiltered;
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, isFiltered ? "linear" : "nearest");
	SDL_DestroyTexture(m_screenTexture);
	m_screenTexture = SDL_CreateTexture(
		m_renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		320,
		240
	);
}

void Screen::ClearScreen( int colour )
{
    //FillRect(m_screen, colour) ;
}
