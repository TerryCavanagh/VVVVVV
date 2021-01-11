#pragma once

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
