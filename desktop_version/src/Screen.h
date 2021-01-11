#pragma once

#include <SDL.h>

#include "ScreenSettings.h"

class Screen
{
public:
	void init(const ScreenSettings& settings);

	void GetSettings(ScreenSettings* settings);

	void LoadIcon();

	void ResizeScreen(int x, int y);
	void ResizeToNearestMultiple();
	void GetWindowSize(int* x, int* y);

	void UpdateScreen(SDL_Surface* buffer, SDL_Rect* rect);
	void FlipScreen();

	const SDL_PixelFormat* GetFormat();

	void toggleFullScreen();
	void toggleStretchMode();
	void toggleLinearFilter();
	void resetRendererWorkaround();

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
