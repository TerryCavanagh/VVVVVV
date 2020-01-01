#include "BlockV.h"

blockclass::blockclass()
{
	clear();
}

void blockclass::clear()
{
	active = false;
	type = 0;
	trigger = 0;

	xp = 0;
	yp = 0;
	wp = 0;
	hp = 0;
	rect.x = xp;
	rect.y = yp;
	rect.w = wp;
	rect.h = hp;

	prompt = "";
	script = "";
	r = 0;
	g = 0;
	b = 0;
}

void blockclass::rectset(const int xi, const int yi, const int wi, const int hi)
{
	rect.x = xi;
	rect.y = yi;
	rect.w = wi;
	rect.h = hi;
}
