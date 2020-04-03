#include "Ent.h"

entclass::entclass()
{
	clear();
}

void entclass::clear()
{
	// Set all values to a default, required for creating a new entity
	active = false;
	invis = false;
	type = 0;
	size = 0;
	tile = 0;
	rule = 0;
	state = 0;
	statedelay = 0;
	life = 0;
	colour = 0;
	para = 0;
	behave = 0;
	animate = 0;

	xp = 0;
	yp = 0;
	ax = 0;
	ay = 0;
	vx = 0;
	vy = 0;
	w = 16;
	h = 16;
	cx = 0;
	cy = 0;
	newxp = 0;
	newyp = 0;

	x1 = 0;
	y1 = 0;
	x2 = 320;
	y2 = 240;

	jumping = false;
	gravity = false;
	onground = 0;
	onroof = 0;
	jumpframe = 0;

	onentity = 0;
	harmful = false;
	onwall = 0;
	onxwall = 0;
	onywall = 0;
	isplatform = false;

	framedelay = 0;
	drawframe = 0;
	walkingframe = 0;
	dir = 0;
	actionframe = 0;
}

bool entclass::outside()
{
	// Returns true if any point of the entity is outside the map.
	// Adjusts velocity for a clean collision.
	if (xp < x1)
	{
		xp = x1;
		return true;
	}
	if (yp < y1)
	{
		yp = y1;
		return true;
	}
	if (xp + w > x2)
	{
		xp = x2 - w;
		return true;
	}
	if (yp + h > y2)
	{
		yp = y2 - h;
		return true;
	}
	return false;
}

void entclass::setenemy( int t )
{
	switch(t)
	{
	case 0:
		//lies emitter
		if( (para)==0)
		{
			tile = 60;
			animate = 2;
			colour = 6;
			behave = 10;
			w = 32;
			h = 32;
			x1 = -200;
		}
		else if ( (para) == 1)
		{
			yp += 10;
			tile = 63;
			animate = 100; //LIES
			colour = 6;
			behave = 11;
			para = 9; //destroyed when outside
			x1 = -200;
			x2 = 400;
			w = 26;
			h = 10;
			cx = 1;
			cy = 1;
		}
		else if ( (para) == 2)
		{
			tile = 62;
			animate = 100;
			colour = 6;
			behave = -1;
			w = 32;
			h = 32;
		}
		break;
	case 1:
		//FACTORY emitter
		if( (para)==0)
		{
			tile = 72;
			animate = 3;
			size = 9;
			colour = 6;
			behave = 12;
			w = 64;
			h = 40;
			cx = 0;
			cy = 24;
		}
		else if ( (para) == 1)
		{
			xp += 4;
			yp -= 4;
			tile = 76;
			animate = 100; // Clouds
			colour = 6;
			behave = 13;
			para = -6; //destroyed when outside
			x2 = 400;
			w = 32;
			h = 12;
			cx = 0;
			cy = 6;
		}
		else if ( (para) == 2)
		{
			tile = 77;
			animate = 100;
			colour = 6;
			behave = -1;
			w = 32;
			h = 16;
		}
		break;
	default:
		break;
	}
}
