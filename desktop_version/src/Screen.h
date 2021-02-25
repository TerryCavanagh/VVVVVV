#ifndef SCREEN_H
#define SCREEN_H

#include <SDL.h>

#include "ScreenSettings.h"

class Screen
{
public:
	void init(const ScreenSettings& settings);
	void destroy(void);

	void GetSettings(ScreenSettings* settings);

	void LoadIcon(void);

	void ResizeScreen(int x, int y);
	void ResizeToNearestMultiple(void);
	void GetWindowSize(int* x, int* y);

	void UpdateScreen(SDL_Surface* buffer, SDL_Rect* rect);
	void FlipScreen(void);

	const SDL_PixelFormat* GetFormat(void);

	void toggleFullScreen(void);
	void toggleStretchMode(void);
	void toggleLinearFilter(void);
	void resetRendererWorkaround(void);

	bool isWindowed;
	bool isFiltered;
	bool badSignalEffect;
	int stretchMode;
	bool vsync;

	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	SDL_Texture *m_screenTexture;
	SDL_Surface* m_screen;

	SDL_Rect filterSubrect;
};

#endif /* SCREEN_H */
