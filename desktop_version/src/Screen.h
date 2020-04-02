#ifndef SCREEN_H
#define SCREEN_H

#include <SDL.h>

class Screen
{
public:
	Screen();

	void ResizeScreen(int x, int y);
	void GetWindowSize(int* x, int* y);

	void UpdateScreen(SDL_Surface* buffer, SDL_Rect* rect);
	void FlipScreen();

	const SDL_PixelFormat* GetFormat();

	void toggleFullScreen();
	void toggleStretchMode();
	void toggleLinearFilter();

	bool isWindowed;
	bool isFiltered;
	bool badSignalEffect;
	int stretchMode;

	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	SDL_Texture *m_screenTexture;
	SDL_Surface* m_screen;

	SDL_Rect filterSubrect;
};



#endif /* SCREEN_H */
