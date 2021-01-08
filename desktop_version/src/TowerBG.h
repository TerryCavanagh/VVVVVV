#ifndef TOWERBG_H
#define TOWERBG_H

#include <SDL.h>

struct TowerBG
{
	SDL_Surface* buffer;
	SDL_Surface* buffer_lerp;
	bool tdrawback;
	int bypos;
	int bscroll;
	int colstate;
	int scrolldir;
	int r;
	int g;
	int b;
};

#endif /* TOWERBG_H */
