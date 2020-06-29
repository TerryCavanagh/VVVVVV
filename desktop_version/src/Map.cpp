#include "Map.h"

#include "MakeAndPlay.h"

mapclass::mapclass()
{
	//Start here!
	r = 196;
	g = 196;
	b = 196;
	colstate = 0;
	colstatedelay = 0;
	colsuperstate = 0;
	spikeleveltop = 0;
	spikelevelbottom = 0;
	oldspikeleveltop = 0;
	oldspikelevelbottom = 0;
	warpx = false;
	warpy = false;
	extrarow = 0;

	showteleporters = false;
	showtargets = false;
	showtrinkets = false;

	finalmode = false;
	finalstretch = false;
	finalx = 50;
	finaly = 50;

	cursorstate = 0;
	cursordelay = 0;

	towermode = false;
	cameraseekframe = 0;
	resumedelay = 0;

	final_colormode = false;
	final_colorframe = 0;
	final_colorframedelay = 0;
	final_mapcol = 0;
	final_aniframe = 0;
	final_aniframedelay = 0;

	custommode=false;
	custommodeforreal=false;
	customx=0; customy=0;
	customwidth=20; customheight=20;
	custommmxoff=0; custommmyoff=0; custommmxsize=0; custommmysize=0;
	customzoom=0;
	customshowmm=true;

	rcol = 0;

	//This needs to be in map instead!
	invincibility = false;

	//We init the lookup table:
	for (int i = 0; i < 30; i++)
	{
		vmult.push_back(int(i * 40));
	}
	//We create a blank map
	contents.resize(40 * 30);

	roomdeaths.resize(20 * 20);
	roomdeathsfinal.resize(20 * 20);
	explored.resize(20 * 20);

	tileset = 0;
	initmapdata();

	specialnames.resize(8);
	resetnames();

	//Areamap starts at 100,100 and extends 20x20
	const int tmap[] = {
	1,2,2,2,2,2,2,2,0,3,0,0,0,4,4,4,4,4,4,4,
	1,2,2,2,2,2,2,0,0,3,0,0,0,0,4,4,4,4,4,4,
	0,1,0,0,2,0,0,0,0,3,0,0,0,0,4,4,4,4,4,4,
	0,0,0,0,2,0,0,0,0,3,0,0,5,5,5,5,4,4,4,4,
	0,0,2,2,2,0,0,0,0,3,11,11,5,5,5,5,0,0,0,0,
	0,0,0,0,0,0,0,0,0,3,5,5,5,5,5,5,0,0,0,0,
	0,0,0,0,0,0,0,0,0,3,5,5,5,5,5,5,5,0,0,0,
	0,0,0,0,0,0,0,0,0,3,5,5,5,5,5,5,5,5,5,0,
	0,0,0,0,0,0,0,0,0,3,0,0,0,5,5,5,5,5,5,0,
	0,0,0,0,0,0,0,0,11,3,0,0,0,5,5,5,5,5,5,0,
	0,0,0,0,0,0,0,0,0,3,0,0,0,5,5,5,5,5,5,0,
	0,0,0,0,0,0,0,0,0,3,0,5,5,5,5,5,5,5,5,0,
	0,0,0,0,0,0,0,0,0,3,0,5,5,5,5,5,5,0,5,0,
	0,0,0,0,0,0,0,0,0,3,0,5,5,5,5,5,5,0,5,0,
	0,0,0,0,0,0,0,0,0,3,0,5,5,0,0,0,0,0,5,0,
	0,0,0,0,0,0,0,2,0,3,0,0,0,0,0,0,0,0,0,0,
	0,0,2,2,2,2,2,2,0,3,0,0,0,0,0,0,0,0,0,0,
	0,2,2,2,2,2,2,2,0,3,0,0,0,0,0,0,0,0,0,0,
	2,2,2,2,2,0,0,2,0,3,0,0,0,0,0,0,0,0,0,0,
	2,2,2,2,2,0,0,2,0,3,0,0,0,0,0,0,0,0,0,0,
	};
	areamap.insert(areamap.end(), tmap, tmap+400);

	ypos = 0;
	oldypos = 0;
	bypos = 0;
}

int mapclass::RGB(int red,int green,int blue)
{
	return (blue | (green << 8) | (red << 16));
}

int mapclass::intpol(int a, int b, float c)
{
	return static_cast<int>(a + ((b - a) * c));
}

void mapclass::setteleporter(int x, int y)
{
	point temp;
	temp.x = x;
	temp.y = y;
	teleporters.push_back(temp);
}

void mapclass::settrinket(int x, int y)
{
	point temp;
	temp.x = x;
	temp.y = y;
	shinytrinkets.push_back(temp);
}

void mapclass::resetmap()
{
	//clear the explored area of the map
	explored.clear();
	explored.resize(20 * 20);
}

void mapclass::resetnames()
{
	//Reset all the special names
	specialnames[0] = "Rear Window";
	specialnames[1] = "On the Waterfront";
	specialnames[2] = "The Untouchables";
	specialnames[3] = "Television Newsveel";
	specialnames[4] = "Vwitched";
	specialnames[5] = "Gvnsmoke";
	specialnames[6] = "Please enjoy these repeats";
	specialnames[7] = "Try Jiggling the Antenna";

	glitchmode = 0;
	glitchdelay = 0;
}

void mapclass::transformname(int t)
{
	//transform special names into new ones, one step at a time

	glitchdelay--;
	if(glitchdelay<=0)
	{
		switch(t)
		{
		case 3:
			//Television Newsveel -> The 9 O'Clock News
			if (specialnames[3] == "Television Newsveel")
			{
				specialnames[3] = "Television Newsvel";
			}
			else if (specialnames[3] == "Television Newsvel")
			{
				specialnames[3] = "TelevisvonvNewsvel";
			}
			else if (specialnames[3] == "TelevisvonvNewsvel")
			{
				specialnames[3] = "TvlvvvsvonvNevsvel";
			}
			else if (specialnames[3] == "TvlvvvsvonvNevsvel")
			{
				specialnames[3] = "vvvvvvsvovvNe svel";
			}
			else if (specialnames[3] == "vvvvvvsvovvNe svel")
			{
				specialnames[3] = "vhv vvv'vvovv vevl";
			}
			else if (specialnames[3] == "vhv vvv'vvovv vevl")
			{
				specialnames[3] = "vhv V v'Cvovv vewv";
			}
			else if (specialnames[3] == "vhv V v'Cvovv vewv")
			{
				specialnames[3] = "vhe 9 v'Cvovv vewv";
			}
			else if (specialnames[3] == "vhe 9 v'Cvovv vewv")
			{
				specialnames[3] = "vhe 9 v'Cvovv Newv";
			}
			else if (specialnames[3] == "vhe 9 v'Cvovv Newv")
			{
				specialnames[3] = "The 9 O'Cvovk Newv";
			}
			else if (specialnames[3] == "The 9 O'Cvovk Newv")
			{
				specialnames[3] = "The 9 O'Clock News";
			}
			break;
		case 4:
			//Vwitched -> Dial M for Murder
			if (specialnames[4] == "Vwitched")
			{
				specialnames[4] = "Vwitvhed";
			}
			else if (specialnames[4] == "Vwitvhed")
			{
				specialnames[4] = "vVwivcvedv";
			}
			else if (specialnames[4] == "vVwivcvedv")
			{
				specialnames[4] = "vvvwMvcvMdvv";
			}
			else if (specialnames[4] == "vvvwMvcvMdvv")
			{
				specialnames[4] = "DvvvwMvfvvMdvvv";
			}
			else if (specialnames[4] == "DvvvwMvfvvMdvvv")
			{
				specialnames[4] = "Dvav Mvfvr Mdvvvv";
			}
			else if (specialnames[4] == "Dvav Mvfvr Mdvvvv")
			{
				specialnames[4] = "Diav M for Mdrver";
			}
			else if (specialnames[4] == "Diav M for Mdrver")
			{
				specialnames[4] = "Dial M for Murder";
			}
			break;
		case 5:
			//Gvnsmoke -> Gunsmoke 1966
			if (specialnames[5] == "Gvnsmoke")
			{
				specialnames[5] = "Gvnsmove";
			}
			else if (specialnames[5] == "Gvnsmove")
			{
				specialnames[5] = "Gvnvmovevv";
			}
			else if (specialnames[5] == "Gvnvmovevv")
			{
				specialnames[5] = "Gunvmove1vv6";
			}
			else if (specialnames[5] == "Gunvmove1vv6")
			{
				specialnames[5] = "Vunsmoke 19v6";
			}
			else if (specialnames[5] == "Vunsmoke 19v6")
			{
				specialnames[5] = "Gunsmoke 1966";
			}
			break;
		case 6:
			//Please enjoy these repeats -> In the Margins
			if (specialnames[6] == "Please enjoy these repeats")
			{
				specialnames[6] = "Please envoy theve repeats";
			}
			else if (specialnames[6] == "Please envoy theve repeats")
			{
				specialnames[6] = "Plse envoy tse rvpvas";
			}
			else if (specialnames[6] == "Plase envoy these rvpeas")
			{
				specialnames[6] = "Plse envoy tse rvpvas";
			}
			else if (specialnames[6] == "Plse envoy tse rvpvas")
			{
				specialnames[6] = "Vl envoy te rvevs";
			}
			else if (specialnames[6] == "Vl envoy te rvevs")
			{
				specialnames[6] = "Vv evo tv vevs";
			}
			else if (specialnames[6] == "Vv evo tv vevs")
			{
				specialnames[6] = "Iv vhv Mvrvivs";
			}
			else if (specialnames[6] == "Iv vhv Mvrvivs")
			{
				specialnames[6] = "In the Margins";
			}
			break;
		case 7:
			//Try Jiggling the Antenna -> Heaven's Gate
			if (specialnames[7] == "Try Jiggling the Antenna")
			{
				specialnames[7] = "Try Viggling the Antenna";
			}
			else if (specialnames[7] == "Try Viggling the Antenna")
			{
				specialnames[7] = "TryJivglvng theAvtevna";
			}
			else if (specialnames[7] == "TryJivglvng theAvtevna")
			{
				specialnames[7] = "Tvvivglvng thAvtvvv";
			}
			else if (specialnames[7] == "Tvvivglvng thAvtvvv")
			{
				specialnames[7] = "Vvvgglvnv tvnvva";
			}
			else if (specialnames[7] == "Vvvgglvnv tvnvva")
			{
				specialnames[7] = "Vvavvnvs vvtv";
			}
			else if (specialnames[7] == "Vvavvnvs vvtv")
			{
				specialnames[7] = "Veavvn's Gvte";
			}
			else if (specialnames[7] == "Veavvn's Gvte")
			{
				specialnames[7] = "Heaven's Gate";
			}
			break;

		}
		glitchdelay = 5;
	}
	else
	{
		glitchdelay--;
	}
}

std::string mapclass::getglitchname(int x, int y)
{
	//Returns the name in the final area.
	if (roomname == "glitch")
	{
		//8 Cases!
		//First, the three "glitches"
		glitchdelay--;
		if (glitchdelay <= -5)
		{
			glitchmode = (glitchmode + 1) % 2;
			glitchdelay = 0;
			if (glitchmode == 0) glitchdelay = 20 +int(fRandom() * 10);
		}

		if (x == 42 && y == 51)
		{
			if (glitchmode == 0)
			{
				return specialnames[0];
			}
			else return "Rear Vindow";
		}
		else if (x == 48 && y == 51)
		{
			if (glitchmode == 0)
			{
				return specialnames[1];
			}
			else return "On the Vaterfront";
		}
		else if (x == 49 && y == 51)
		{
			if (glitchmode == 0)
			{
				return specialnames[2];
			}
			else return "The Untouchavles";
		}
	}
	else if (roomname == "change")
	{
		if (finalstretch)
		{
			if (x == 45 && y == 51) transformname(3);
			if (x == 46 && y == 51) transformname(4);
			if (x == 47 && y == 51) transformname(5);
			if (x == 50 && y == 53) transformname(6);
			if (x == 50 && y == 54) transformname(7);
		}

		if (x == 45 && y == 51) return specialnames[3];
		if (x == 46 && y == 51) return specialnames[4];
		if (x == 47 && y == 51) return specialnames[5];
		if (x == 50 && y == 53) return specialnames[6];
		if (x == 50 && y == 54) return specialnames[7];
		return roomname;
	}
	else
	{
		return roomname;
	}
	return roomname;
}

void mapclass::initmapdata()
{
	//Set up static map information like teleporters and shiny trinkets.
	setteleporter(0, 0);
	setteleporter(0, 16);
	setteleporter(2, 4);
	setteleporter(2, 11);
	setteleporter(7, 9);
	setteleporter(7, 15);
	setteleporter(8, 11);
	setteleporter(10, 5);
	setteleporter(11, 4);
	setteleporter(13, 2);
	setteleporter(13, 8);
	setteleporter(14, 19);
	setteleporter(15, 0);
	setteleporter(17, 12);
	setteleporter(17, 17);
	setteleporter(18, 1);
	setteleporter(18, 7);

	settrinket(14, 4);
	settrinket(13, 6);
	settrinket(11, 12);
	settrinket(15, 12);
	settrinket(14, 11);
	settrinket(18, 14);
	settrinket(11, 7);
	settrinket(9, 2);
	settrinket(9, 16);
	settrinket(2, 18);
	settrinket(7, 18);
	settrinket(6, 1);
	settrinket(17, 3);
	settrinket(10, 19);
	settrinket(5, 15);
	settrinket(1, 10);
	settrinket(3, 2);
	settrinket(10, 8);
}

int mapclass::finalat(int x, int y)
{
	//return the tile index of the final stretch tiles offset by the colour difference
	if (contents[x + vmult[y]] == 740)
	{
		//Special case: animated tiles
		if (final_mapcol == 1)
		{
			// Windows hits fRandom() == 1 frequently! For fuck sake! -flibit
			return 737 + (std::min(int(fRandom() * 12), 11) * 40);
		}
		else
		{
			return contents[x + vmult[y]] - (final_mapcol * 3) + (final_aniframe * 40);
		}
	}
	else if (contents[x + vmult[y]] >= 80)
	{
		return contents[x + vmult[y]] - (final_mapcol * 3);
	}
	else
	{
		return contents[x + vmult[y]];
	}
	return 0;
}

int mapclass::maptiletoenemycol(int t)
{
	//returns the colour index for enemies that matches the map colour t
	switch(t)
	{
	case 0:
		return 11;
		break;
	case 1:
		return 6;
		break;
	case 2:
		return 8;
		break;
	case 3:
		return 12;
		break;
	case 4:
		return 9;
		break;
	case 5:
		return 7;
		break;
	case 6:
		return 18;
		break;
	}
	return 11;
}

void mapclass::changefinalcol(int t)
{
	//change the map to colour t - for the game's final stretch.
	//First up, the tiles. This is just a setting:
	final_mapcol = t;
	temp = 6 - t;
	//Next, entities
	for (size_t i = 0; i < obj.entities.size(); i++)
	{
		if (obj.entities[i].type == 1) //something with a movement behavior
		{
			if (obj.entities[i].animate == 10 || obj.entities[i].animate == 11) //treadmill
			{
				if(temp<3)
				{
					obj.entities[i].tile = 907 + (temp * 80);
				}
				else
				{
					obj.entities[i].tile = 911 + ((temp-3) * 80);
				}
				if(obj.entities[i].animate == 10)	obj.entities[i].tile += 40;
			}
			else if (obj.entities[i].isplatform)
			{
				obj.entities[i].tile = 915+(temp*40);
			}
			else	//just an enemy
			{
				obj.entities[i].colour = maptiletoenemycol(temp);
			}
		}
		else if (obj.entities[i].type == 2)	//disappearing platforms
		{
			obj.entities[i].tile = 915+(temp*40);
		}
	}
}

void mapclass::setcol(const int r1, const int g1, const int b1 , const int r2, const int g2, const int b2, const int c)
{
	r = intpol(r1, r2, c / 5);
	g = intpol(g1, g2, c / 5);
	b = intpol(b1, b2, c / 5);
}

void mapclass::updatetowerglow()
{
	if (colstatedelay <= 0 || colsuperstate > 0)
	{
		if (colsuperstate > 0) colstate--;
		colstate++;
		if (colstate >= 30) colstate = 0;
		check = colstate % 5; //current state of phase
		cmode = (colstate - check) / 5; // current colour transition

		switch(cmode)
		{
		case 0:
			setcol(255, 93, 107, 255, 255, 93, check);
			break;
		case 1:
			setcol(255, 255, 93, 159, 255, 93, check);
			break;
		case 2:
			setcol(159, 255, 93, 93, 245, 255, check);
			break;
		case 3:
			setcol(93, 245, 255, 177, 93, 255, check);
			break;
		case 4:
			setcol(177, 93, 255, 255, 93, 255, check);
			break;
		case 5:
			setcol(255, 93, 255, 255, 93, 107, check);
			break;
		}

		if (check == 0)
		{
			colstatedelay = 45;
		}
		else
		{
			colstatedelay = 0;
		}
		if (colsuperstate > 0) colstatedelay = 0;

		tdrawback = true;
		towercol = RGB(r*0.04f, g*0.04f, b*0.04f);
	}
	else
	{
		colstatedelay--;
	}
}

void mapclass::nexttowercolour()
{
	colstate+=5;
	if (colstate >= 30) colstate = 0;
	check = colstate % 5; //current state of phase
	cmode = (colstate - check) / 5; // current colour transition

	switch(cmode)
	{
	case 0:
		setcol(255, 93, 107, 255, 255, 93, check);
		break;
	case 1:
		setcol(255, 255, 93, 159, 255, 93, check);
		break;
	case 2:
		setcol(159, 255, 93, 93, 245, 255, check);
		break;
	case 3:
		setcol(93, 245, 255, 177, 93, 255, check);
		break;
	case 4:
		setcol(177, 93, 255, 255, 93, 255, check);
		break;
	case 5:
		setcol(255, 93, 255, 255, 93, 107, check);
		break;
	}

	tdrawback = true;
	towercol = RGB(r*0.04, g*0.04, b*0.04);
}

void mapclass::settowercolour(int t)
{
	colstate=t*5;
	if (colstate >= 30) colstate = 0;
	check = colstate % 5; //current state of phase
	cmode = (colstate - check) / 5; // current colour transition

	switch(cmode)
	{
	case 0:
		setcol(255, 93, 107, 255, 255, 93, check);
		break;
	case 1:
		setcol(255, 255, 93, 159, 255, 93, check);
		break;
	case 2:
		setcol(159, 255, 93, 93, 245, 255, check);
		break;
	case 3:
		setcol(93, 245, 255, 177, 93, 255, check);
		break;
	case 4:
		setcol(177, 93, 255, 255, 93, 255, check);
		break;
	case 5:
		setcol(255, 93, 255, 255, 93, 107, check);
		break;
	}

	tdrawback = true;
	towercol = RGB(r*0.04, g*0.04, b*0.04);
}

bool mapclass::spikecollide(int x, int y)
{
	if (invincibility) return false;
	if (tower.at(x,y,0)>= 6 && tower.at(x,y,0) <= 11) return true;
	return false;
}

bool mapclass::collide(int x, int y)
{
	if (towermode)
	{
		if (tower.at(x, y, 0) >= 12 && tower.at(x, y, 0) <= 27) return true;
		if (invincibility)
		{
			if (tower.at(x, y, 0) >= 6 && tower.at(x, y, 0) <= 11) return true;
		}
	}
	else if (tileset == 2)
	{
		if (y == -1) return collide(x, y + 1);
		if (y == 29+extrarow) return collide(x, y - 1);
		if (x == -1) return collide(x + 1, y);
		if (x == 40) return collide(x - 1, y);
		if (x < 0 || y < 0 || x >= 40 || y >= 29 + extrarow) return false;
		if (contents[x + vmult[y]] >= 12 && contents[x + vmult[y]] <= 27) return true;
		if (invincibility)
		{
			if (contents[x + vmult[y]] >= 6 && contents[x + vmult[y]] <= 11) return true;
		}
	}
	else
	{
		if (y == -1) return collide(x, y + 1);
		if (y == 29+extrarow) return collide(x, y - 1);
		if (x == -1) return collide(x + 1, y);
		if (x == 40) return collide(x - 1, y);
		if (x < 0 || y < 0 || x >= 40 || y >= 29+extrarow) return false;
		if (contents[x + vmult[y]] == 1) return true;
		if (tileset==0 && contents[x + vmult[y]] == 59) return true;
		if (contents[x + vmult[y]]>= 80 && contents[x + vmult[y]] < 680) return true;
		if (contents[x + vmult[y]] == 740 && tileset==1) return true;
		if (invincibility)
		{
			if (contents[x + vmult[y]]>= 6 && contents[x + vmult[y]] <= 9) return true;
			if (contents[x + vmult[y]]>= 49 && contents[x + vmult[y]] <= 50) return true;
			if (tileset == 1)
			{
				if (contents[x + vmult[y]]>= 49 && contents[x + vmult[y]] < 80) return true;
			}
		}
	}
	return false;
}

void mapclass::settile(int xp, int yp, int t)
{
	if (xp >= 0 && xp < 40 && yp >= 0 && yp < 29+extrarow)
	{
		contents[xp + vmult[yp]] = t;
	}
}


int mapclass::area(int _rx, int _ry)
{
	//THIS IS THE BUG
	if (finalmode)
	{
		return 6;
	}
	else
	{
		int lookup = (_rx - 100) + ((_ry - 100) * 20);
		//lookup = std::max(0,lookup);
		if(_rx-100>=0 && _rx-100<20 && _ry-100>=0 && _ry-100<20){
			return areamap[lookup];
		}
		else
		{
			return 6;
		}
	}
}

void mapclass::exploretower()
{
	for (int i = 0; i < 20; i++)
	{
		explored[9 + (i * 20)] = 1;
	}
}

void mapclass::hideship()
{
	//remove the ship from the explored areas
	explored[2 + (10 * 20)] = 0;
	explored[3 + (10 * 20)] = 0;
	explored[4 + (10 * 20)] = 0;
	explored[2 + (11 * 20)] = 0;
	explored[3 + (11 * 20)] = 0;
	explored[4 + (11 * 20)] = 0;
}

void mapclass::showship()
{
	//remove the ship from the explored areas
	explored[2 + (10 * 20)] = 1;
	explored[3 + (10 * 20)] = 1;
	explored[4 + (10 * 20)] = 1;
	explored[2 + (11 * 20)] = 1;
	explored[3 + (11 * 20)] = 1;
	explored[4 + (11 * 20)] = 1;
}

void mapclass::resetplayer()
{
	bool was_in_tower = towermode;
	if (game.roomx != game.saverx || game.roomy != game.savery)
	{
		gotoroom(game.saverx, game.savery);
	}

	game.deathseq = -1;
	int i = obj.getplayer();
	if(i>-1)
	{
		obj.entities[i].vx = 0;
		obj.entities[i].vy = 0;
		obj.entities[i].ax = 0;
		obj.entities[i].ay = 0;
		obj.entities[i].xp = game.savex;
		obj.entities[i].yp = game.savey;
		obj.entities[i].dir = game.savedir;
		obj.entities[i].colour = 0;
		game.lifeseq = 10;
		obj.entities[i].invis = true;
		if (!game.glitchrunnermode)
		{
			obj.entities[i].size = 0;
			obj.entities[i].cx = 6;
			obj.entities[i].cy = 2;
			obj.entities[i].h = 21;
		}

		// If we entered a tower as part of respawn, reposition camera
		if (!was_in_tower && towermode)
		{
			ypos = obj.entities[i].yp - 120;
			if (ypos < 0)
			{
				ypos = 0;
			}
			oldypos = ypos;
			bypos = ypos / 2;
		}
	}

	game.scmhurt = false; //Just in case the supercrewmate is fucking this up!
	if (game.supercrewmate)
	{
		if (game.roomx == game.scmprogress + 41)
		{
			game.scmprogress = game.roomx - 41;
		}
		else
		{
			game.scmprogress = game.roomx - 40;
		}
		if (game.scmprogress != 0)
		{
			game.scmmoveme = true;
		}
		else
		{
			game.scmmoveme = false;
		}
	}
}

void mapclass::warpto(int rx, int ry , int t, int tx, int ty)
{
	gotoroom(rx, ry);
	game.teleport = false;
	if (t >= 0 && t < (int) obj.entities.size())
	{
		obj.entities[t].xp = tx * 8;
		obj.entities[t].yp = (ty * 8) - obj.entities[t].h;
	}
	game.gravitycontrol = 0;
}

void mapclass::gotoroom(int rx, int ry)
{
	//First, destroy the current room
	obj.removeallblocks();
	game.activetele = false;
	game.readytotele = 0;

	//Ok, let's save the position of all lines on the screen
	obj.linecrosskludge.clear();
	for (size_t i = 0; i < obj.entities.size(); i++)
	{
		if (obj.entities[i].type == 9)
		{
			//It's a horizontal line
			if (obj.entities[i].xp <= 0 || (obj.entities[i].xp + obj.entities[i].w) >= 312)
			{
				//it's on a screen edge
				obj.copylinecross(i);
			}
		}
	}

	for (size_t i = 0; i < obj.entities.size(); i++)
	{
		if (obj.entities[i].rule != 0)
		{
			removeentity_iter(i);
		}
	}

	game.door_up = rx + ((ry - 1) * 100);
	game.door_down = rx + ((ry + 1) * 100);
	game.door_right = rx + 1 + (ry * 100);
	game.door_left = rx -1 + (ry * 100);

	if (rx < game.roomx)
	{
		game.roomchangedir = 0;
	}
	else
	{
		game.roomchangedir = 1;
	}

	if (finalmode)
	{
		//Ok, what way are we moving?
		finalx = rx;
		finaly = ry;
		game.roomx = finalx;
		game.roomy = finaly;
		game.roomchange = true;
		rx = finalx;
		ry = finaly;

		if (game.roomy < 10)
		{
			game.roomy = 11;
			finaly = 11;
		}

		if(game.roomx>=41 && game.roomy>=48 && game.roomx<61 && game.roomy<68 )
		{
			game.currentroomdeaths = roomdeathsfinal[game.roomx - 41 + (20 * (game.roomy - 48))];
		}
		else
		{
			game.currentroomdeaths = 0;
		}

		//Final level for time trial
		if (game.intimetrial)
		{
			if (game.roomx == 46 && game.roomy == 54) music.niceplay(15); //Final level remix
		}
	}
#if !defined(NO_CUSTOM_LEVELS)
	else if (custommode)
	{
		game.roomx = rx;
		game.roomy = ry;
		game.roomchange = true;
		if (game.roomx < 100) game.roomx = 100 + ed.mapwidth-1;
		if (game.roomy < 100) game.roomy = 100 + ed.mapheight-1;
		if (game.roomx > 100 + ed.mapwidth-1) game.roomx = 100;
		if (game.roomy > 100 + ed.mapheight-1) game.roomy = 100;
	}
#endif
	else
	{
		game.roomx = rx;
		game.roomy = ry;
		game.roomchange = true;
		if (game.roomx < 100) game.roomx = 119;
		if (game.roomy < 100) game.roomy = 119;
		if (game.roomx > 119) game.roomx = 100;
		if (game.roomy > 119) game.roomy = 100;

		game.currentroomdeaths = roomdeaths[game.roomx - 100 + (20 * (game.roomy - 100))];

		//Alright, change music depending on where we are:
		//Tower
		if (game.roomx == 107 && game.roomy == 106) music.niceplay(4);
		if (game.roomx == 107 && game.roomy == 107) music.niceplay(4);
		if (game.roomx == 107 && game.roomy == 108) music.niceplay(4);
		if (game.roomx == 107 && game.roomy == 109) music.niceplay(4);
		if (game.roomx == 108 && game.roomy == 109)
		{
			if (graphics.setflipmode)
			{
				music.niceplay(9);
			}
			else
			{
				music.niceplay(2);
			}
		}
		if (game.roomx == 109)
		{
			if (graphics.setflipmode)
			{
				music.niceplay(9);
			}
			else
			{
				music.niceplay(2);
			}
		}
		//Warp Zone
		if (game.roomx == 112 && game.roomy == 101) music.niceplay(4);
		if (game.roomx == 113 && game.roomy == 101) music.niceplay(4);
		if (game.roomx == 113 && game.roomy == 102) music.niceplay(4);
		if (game.roomx == 114 && game.roomy == 101) music.niceplay(12);
		if (game.roomx == 115 && game.roomy == 101) music.niceplay(12);
		if (game.roomx == 115 && game.roomy == 102) music.niceplay(12);
		//Lab
		if (game.roomx == 101 && game.roomy == 115) music.niceplay(4);
		if (game.roomx == 100 && game.roomy == 115) music.niceplay(4);
		if (game.roomx == 101 && game.roomy == 116) music.niceplay(4);
		if (game.roomx == 100 && game.roomy == 116) music.niceplay(4);
		if (game.roomx == 102 && game.roomy == 116) music.niceplay(3);
		if (game.roomx == 102 && game.roomy == 117) music.niceplay(3);
		if (game.roomx == 101 && game.roomy == 117) music.niceplay(3);
		//Space Station
		if (game.intimetrial)
		{
			if (game.roomx == 111 && game.roomy == 112) music.niceplay(1);
			if (game.roomx == 111 && game.roomy == 113) music.niceplay(1);
			if (game.roomx == 112 && game.roomy == 114) music.niceplay(1);
			if (game.roomx == 112 && game.roomy == 115) music.niceplay(1);
		}
		else
		{
			if (game.roomx == 111 && game.roomy == 112) music.niceplay(1);
			if (game.roomx == 111 && game.roomy == 113) music.niceplay(1);
			if (game.roomx == 112 && game.roomy == 114) music.niceplay(4);
			if (game.roomx == 112 && game.roomy == 115) music.niceplay(4);
		}
		//Leaving the Ship
		if (game.roomx == 104 && game.roomy == 112) music.niceplay(4);
	}
	temp = rx + (ry * 100);
	loadlevel(game.roomx, game.roomy);


	//Do we need to reload the background?
	bool redrawbg = game.roomx != game.prevroomx || game.roomy != game.prevroomy;

	if(redrawbg)
	{
		graphics.backgrounddrawn = false; //Used for background caching speedup
	}
	graphics.foregrounddrawn = false; //Used for background caching speedup

	game.prevroomx = game.roomx;
	game.prevroomy = game.roomy;

	//a very special case: if entering the communication room, room 13,4 before tag 5 is set, set the game state to a background
	//textbox thingy. if tag five is not set when changing room, reset the game state. (tag 5 is set when you get back to the ship)
	if(!game.intimetrial && !custommode)
	{
		if (!obj.flags[5] && !finalmode)
		{
			game.state = 0;
			if (game.roomx == 113 && game.roomy == 104)
			{
				game.state = 50;
			}
		}
	}

	//Ok, kludge to fix lines in crossing rooms - if we're intersecting a gravity line right now, let's
	//set it to an inactive state.

	//Alright! So, let's look at our lines from the previous rooms, and determine if any of them are actually
	//continuations!

	temp = obj.getplayer();
	if(temp>-1)
	{
		obj.entities[temp].oldxp = obj.entities[temp].xp - int(obj.entities[temp].vx);
		obj.entities[temp].oldyp = obj.entities[temp].yp - int(obj.entities[temp].vy);
	}

	for (size_t i = 0; i < obj.entities.size(); i++)
	{
		if (obj.entities[i].type == 9)
		{
			//It's a horizontal line
			if (obj.entities[i].xp <= 0 || obj.entities[i].xp + obj.entities[i].w >= 312)
			{
				//it's on a screen edge
				for (size_t j = 0; j < obj.linecrosskludge.size(); j++)
				{
					if (obj.entities[i].yp == obj.linecrosskludge[j].yp)
					{
						//y's match, how about x's?
						//we're moving left:
						if (game.roomchangedir == 0)
						{
							if (obj.entities[i].xp + obj.entities[i].w >= 312 && obj.linecrosskludge[j].xp <= 0)
							{
								obj.revertlinecross(i, j);
							}
						}
						else
						{
							if (obj.entities[i].xp <= 0 && obj.linecrosskludge[j].xp + obj.linecrosskludge[j].w >= 312)
							{
								obj.revertlinecross(i, j);
							}
						}
					}
				}
			}
		}
	}
}

std::string mapclass::currentarea(int t)
{
	switch(t)
	{
	case 0:
		return "Dimension VVVVVV";
		break;
	case 1:
		return "Dimension VVVVVV";
		break;
	case 2:
		return "Laboratory";
		break;
	case 3:
		return "The Tower";
		break;
	case 4:
		return "Warp Zone";
		break;
	case 5:
		return "Space Station";
		break;
	case 6:
		return "Outside Dimension VVVVVV";
		break;
	case 7:
		return "Outside Dimension VVVVVV";
		break;
	case 8:
		return "Outside Dimension VVVVVV";
		break;
	case 9:
		return "Outside Dimension VVVVVV";
		break;
	case 10:
		return "Outside Dimension VVVVVV";
		break;
	case 11:
		return "The Tower";
		break;
	}
	return "???";
}

void mapclass::loadlevel(int rx, int ry)
{
	int t;
	if (!finalmode)
	{
		explored[rx - 100 + ((ry - 100) * 20)] = 1;
		if (rx == 109 && !custommode)
		{
			exploretower();
		}
	}


	roomtexton = false;
	roomtext.clear();

	obj.platformtile = 0;
	obj.customplatformtile=0;
	obj.vertplatforms = false;
	obj.horplatforms = false;
	roomname = "";
	hiddenname = "";
	background = 1;
	warpx = false;
	warpy = false;

	towermode = false;
	ypos = 0;
	oldypos = 0;
	extrarow = 0;
	spikeleveltop = 0;
	spikelevelbottom = 0;
	oldspikeleveltop = 0;
	oldspikelevelbottom = 0;

	//Custom stuff for warplines
	obj.customwarpmode=false;
	obj.customwarpmodevon=false;
	obj.customwarpmodehon=false;

	if (finalmode)
	{
		t = 6;
		//check if we're in the towers
		if (rx == 49 && ry == 52)
		{
			//entered tower 1
			t = 7;
		}
		else if (rx == 49 && ry == 53)
		{
			//re entered tower 1
			t = 8;
		}
		else if (rx == 51 && ry == 54)
		{
			//entered tower 2
			t = 9;
		}
		else if (rx == 51 && ry == 53)
		{
			//re entered tower 2
			t = 10;
		}
	}
	else if (custommode)
	{
		t= 12;
	}
	else
	{
		t = area(rx, ry);

		if (t == 3)
		{
			//correct position for tower
			if (ry == 109)
			{
				//entered from ground floor
				int player = obj.getplayer();
				if (player > -1)
				{
					obj.entities[player].yp += (671 * 8);
				}

				ypos = (700-29) * 8;
				oldypos = ypos;
				bypos = ypos / 2;
				cameramode = 0;
				colstate = 0;
				colsuperstate = 0;
			}
			else if (ry == 104)
			{
				//you've entered from the top floor
				ypos = 0;
				oldypos = ypos;
				bypos = 0;
				cameramode = 0;
				colstate = 0;
				colsuperstate = 0;
			}
		}

		if (t < 2) //on the world map, want to test if we're in the secret lab
		{
			if (rx >= 116)
			{
				if (ry >= 105)
				{
					if (ry <= 107)
					{
						if (rx == 119 && ry == 105)
						{
							//Ah, this is just a normal area
						}
						else
						{
							//in the secret lab! Crazy background!
							background = 2;
							if (rx == 116 && ry == 105) graphics.rcol = 1;
							if (rx == 117 && ry == 105) graphics.rcol = 5;
							if (rx == 118 && ry == 105) graphics.rcol = 4;
							if (rx == 117 && ry == 106) graphics.rcol = 2;
							if (rx == 118 && ry == 106) graphics.rcol = 0;
							if (rx == 119 && ry == 106) graphics.rcol = 3;
							if (rx == 119 && ry == 107) graphics.rcol = 1;
						}
					}
				}
			}
		}
	}

	if (rx == 119 && ry == 108 && !custommode)
	{
		background = 5;
		graphics.rcol = 3;
		warpx = true;
		warpy = true;
	}

	switch(t)
	{
#if !defined(MAKEANDPLAY)
	case 0:
	case 1: //World Map
		tileset = 1;
		extrarow = 1;
		contents = otherlevel.loadlevel(rx, ry);
		roomname = otherlevel.roomname;
		tileset = otherlevel.roomtileset;
		//do the appear/remove roomname here

		if (otherlevel.roomtexton)
		{
			roomtexton = true;
			roomtext = std::vector<Roomtext>(otherlevel.roomtext);
		}

		if (game.roomx >= 102 && game.roomx <= 104 && game.roomy >= 110 && game.roomy <= 111)
		{
			hiddenname = "The Ship";
		}
		else
		{
			hiddenname = "Dimension VVVVVV";
		}
		break;
	case 2: //The Lab
		contents = lablevel.loadlevel(rx, ry);
		roomname = lablevel.roomname;
		tileset = 1;
		background = 2;
		graphics.rcol = lablevel.rcol;
		break;
	case 3: //The Tower
		tdrawback = true;
		minitowermode = false;
		tower.minitowermode = false;
		bscroll = 0;
		scrolldir = 0;

		roomname = "The Tower";
		tileset = 1;
		background = 3;
		towermode = true;
		//bypos = 0; ypos = 0; cameramode = 0;

		//All the entities for here are just loaded here; it's essentially one room after all


		obj.createentity(48, 5456, 10, 1, 505007); // (savepoint)
		obj.createentity(224, 4528, 10, 1, 505017); // (savepoint)
		obj.createentity(232, 4168, 10, 0, 505027); // (savepoint)
		obj.createentity(280, 3816, 10, 1, 505037); // (savepoint)
		obj.createentity(152, 3552, 10, 1, 505047); // (savepoint)
		obj.createentity(216, 3280, 10, 0, 505057); // (savepoint)
		obj.createentity(216, 4808, 10, 1, 505067); // (savepoint)
		obj.createentity(72, 3096, 10, 0, 505077); // (savepoint)
		obj.createentity(176, 2600, 10, 0, 505087); // (savepoint)
		obj.createentity(216, 2392, 10, 0, 505097); // (savepoint)
		obj.createentity(152, 1184, 10, 1, 505107); // (savepoint)
		obj.createentity(152, 912, 10, 1, 505117); // (savepoint)
		obj.createentity(152, 536, 10, 1, 505127); // (savepoint)
		obj.createentity(120, 5136, 10, 0, 505137); // (savepoint)
		obj.createentity(144, 1824, 10, 0, 505147); // (savepoint)
		obj.createentity(72, 2904, 10, 0, 505157); // (savepoint)
		obj.createentity(224, 1648, 10, 1, 505167); // (savepoint)
		obj.createentity(112, 5280, 10, 1, 50517); // (savepoint)

		obj.createentity(24, 4216, 9, 7); // (shiny trinket)
		obj.createentity(280, 3216, 9, 8); // (shiny trinket)
		break;
	case 4: //The Warpzone
		contents = warplevel.loadlevel(rx, ry);
		roomname = warplevel.roomname;
		tileset = 1;
		background = 3;
		graphics.rcol = warplevel.rcol;
		graphics.backgrounddrawn = false;

		warpx = warplevel.warpx;
		warpy = warplevel.warpy;
		background = 5;
		if (warpy) background = 4;
		if (warpx) background = 3;
		if (warpx && warpy) background = 5;
		break;
	case 5: //Space station
		contents = spacestation2.loadlevel(rx, ry);
		roomname = spacestation2.roomname;
		tileset = 0;
		break;
	case 6: //final level
		contents = finallevel.loadlevel(finalx, finaly);
		roomname = finallevel.roomname;
		tileset = 1;
		background = 3;
		graphics.rcol = finallevel.rcol;
		graphics.backgrounddrawn = false;

		if (finalstretch)
		{
			background = 6;
		}
		else
		{
			warpx = finallevel.warpx;
			warpy = finallevel.warpy;
			background = 5;
			if (warpy) background = 4;
			if (warpx) background = 3;
			if (warpx && warpy) background = 5;
		}

		graphics.rcol = 6;
		changefinalcol(final_mapcol);
		for (size_t i = 0; i < obj.entities.size(); i++)
		{
			if (obj.entities[i].type == 1 || obj.entities[i].type == 2)
			{
				//Fix 1-frame glitch
				obj.entities[i].drawframe = obj.entities[i].tile;
			}
		}
		break;
	case 7: //Final Level, Tower 1
		tdrawback = true;
		minitowermode = true;
		tower.minitowermode = true;
		bscroll = 0;
		scrolldir = 1;

		roomname = "Panic Room";
		tileset = 1;
		background = 3;
		towermode = true;

		tower.loadminitower1();

		ypos = 0;
		oldypos = 0;
		bypos = 0;
		cameramode = 0;
		colstate = 0;
		colsuperstate = 0;
		break;
	case 8: //Final Level, Tower 1 (reentered from below)
	{
		tdrawback = true;
		minitowermode = true;
		tower.minitowermode = true;
		bscroll = 0;
		scrolldir = 1;

		roomname = "Panic Room";
		tileset = 1;
		background = 3;
		towermode = true;

		tower.loadminitower1();

		int i = obj.getplayer();
		if (i > -1)
		{
			obj.entities[i].yp += (71 * 8);
		}
		game.roomy--;
		finaly--;

		ypos = (100-29) * 8;
		oldypos = ypos;
		bypos = ypos/2;
		cameramode = 0;
		colstate = 0;
		colsuperstate = 0;}
		break;
	case 9: //Final Level, Tower 2
	{
		tdrawback = true;
		minitowermode = true;
		tower.minitowermode = true;
		bscroll = 0;
		scrolldir = 0;
		final_colorframe = 2;

		roomname = "The Final Challenge";
		tileset = 1;
		background = 3;
		towermode = true;

		tower.loadminitower2();

		obj.createentity(56, 556, 11, 136); // (horizontal gravity line)
		obj.createentity(184, 592, 10, 0, 50500); // (savepoint)
		obj.createentity(184, 644, 11, 88); // (horizontal gravity line)
		obj.createentity(56, 460, 11, 136); // (horizontal gravity line)
		obj.createentity(216, 440, 10, 0, 50501); // (savepoint)
		obj.createentity(104, 508, 11, 168); // (horizontal gravity line)
		obj.createentity(219, 264, 12, 56); // (vertical gravity line)
		obj.createentity(120, 332, 11, 96); // (horizontal gravity line)
		obj.createentity(219, 344, 12, 56); // (vertical gravity line)
		obj.createentity(224, 332, 11, 48); // (horizontal gravity line)
		obj.createentity(56, 212, 11, 144); // (horizontal gravity line)
		obj.createentity(32, 20, 11, 96); // (horizontal gravity line)
		obj.createentity(72, 156, 11, 200); // (horizontal gravity line)

		int i = obj.getplayer();
		if (i > -1)
		{
			obj.entities[i].yp += (71 * 8);
		}
		game.roomy--;
		finaly--;

		ypos = (100-29) * 8;
		oldypos = ypos;
		bypos = ypos/2;
		cameramode = 0;
		colstate = 0;
		colsuperstate = 0;
	}
		break;
	case 10: //Final Level, Tower 2
	{

		tdrawback = true;
		minitowermode = true;
		tower.minitowermode = true;
		bscroll = 0;
		scrolldir = 0;
		final_colorframe = 2;

		roomname = "The Final Challenge";
		tileset = 1;
		background = 3;
		towermode = true;

		tower.loadminitower2();

		obj.createentity(56, 556, 11, 136); // (horizontal gravity line)
		obj.createentity(184, 592, 10, 0, 50500); // (savepoint)
		obj.createentity(184, 644, 11, 88); // (horizontal gravity line)
		obj.createentity(56, 460, 11, 136); // (horizontal gravity line)
		obj.createentity(216, 440, 10, 0, 50501); // (savepoint)
		obj.createentity(104, 508, 11, 168); // (horizontal gravity line)
		obj.createentity(219, 264, 12, 56); // (vertical gravity line)
		obj.createentity(120, 332, 11, 96); // (horizontal gravity line)
		obj.createentity(219, 344, 12, 56); // (vertical gravity line)
		obj.createentity(224, 332, 11, 48); // (horizontal gravity line)
		obj.createentity(56, 212, 11, 144); // (horizontal gravity line)
		obj.createentity(32, 20, 11, 96); // (horizontal gravity line)
		obj.createentity(72, 156, 11, 200); // (horizontal gravity line)

		ypos = 0;
		oldypos = 0;
		bypos = 0;
		cameramode = 0;
		colstate = 0;
		colsuperstate = 0;
	}
		break;
	case 11: //Tower Hallways //Content is held in final level routine
	{
		contents = finallevel.loadlevel(rx, ry);
		roomname = finallevel.roomname;
		tileset = 2;
		if (rx == 108)
		{
			background = 7;
			rcol = 15;
		}
		if (rx == 110)
		{
			background = 8;
			rcol = 10;
		}
		if (rx == 111)
		{
			background = 9;
			rcol = 0;
		}
	}
		break;
#endif
#if !defined(NO_CUSTOM_LEVELS)
	case 12: //Custom level
		int curlevel=(rx-100)+((ry-100)*ed.maxwidth);
		game.customcol=ed.getlevelcol(curlevel)+1;
		obj.customplatformtile=game.customcol*12;
		switch(ed.level[curlevel].tileset){
			case 0: //Space Station
			tileset = 0;
			background = 1;
			break;
			case 1: //Outside
			tileset = 1;
			background = 1;
			break;
			case 2: //Lab
			tileset = 1;
			background = 2;
			graphics.rcol = ed.level[curlevel].tilecol;
			break;
			case 3: //Warp Zone/intermission
			tileset = 1;
			background = 6;
			break;
			case 4://Ship
			tileset = 1;
			background = 1;
			break;
			default:
			tileset = 1;
			background = 1;
			break;
		}

		//If screen warping, then override all that:
		bool redrawbg = game.roomx != game.prevroomx || game.roomy != game.prevroomy;
		if(redrawbg){
			graphics.backgrounddrawn = false;
		}
		if(ed.level[curlevel].warpdir>0){
			if(ed.level[curlevel].warpdir==1){
			warpx=true;
			background=3;
			graphics.rcol = ed.getwarpbackground(rx-100,ry-100);
			}else if(ed.level[curlevel].warpdir==2){
			warpy=true;
			background=4;
			graphics.rcol = ed.getwarpbackground(rx-100,ry-100);
			}else if(ed.level[curlevel].warpdir==3){
			warpx=true; warpy=true;
			background = 5;
			graphics.rcol = ed.getwarpbackground(rx-100,ry-100);
			}
		}

		roomname="";
		if(ed.level[curlevel].roomname!=""){
			roomname=ed.level[curlevel].roomname;
		}
		extrarow = 1;
		contents = ed.loadlevel(rx, ry);


		roomtexton = false;
		roomtext.clear();

		//Entities have to be created HERE, akwardly
		int tempcheckpoints=0;
		int tempscriptbox=0;
		for(size_t edi=0; edi<edentity.size(); edi++){
			//If entity is in this room, create it
			int tsx=(edentity[edi].x-(edentity[edi].x%40))/40;
			int tsy=(edentity[edi].y-(edentity[edi].y%30))/30;
			if(tsx==rx-100 && tsy==ry-100){
			switch(edentity[edi].t){
				case 1: //Enemies
				int bx1, by1, bx2, by2;
				bx1=ed.level[rx-100+((ry-100)*ed.maxwidth)].enemyx1;
				by1=ed.level[rx-100+((ry-100)*ed.maxwidth)].enemyy1;
				bx2=ed.level[rx-100+((ry-100)*ed.maxwidth)].enemyx2;
				by2=ed.level[rx-100+((ry-100)*ed.maxwidth)].enemyy2;

				if(warpx){ if(bx1==0 && bx2==320){ bx1=-100; bx2=420; } }
				if(warpy){ if(by1==0 && by2==240){ by1=-100; by2=340; } }

				obj.customenemy=ed.level[tsx+((ed.maxwidth)*tsy)].enemytype;
				obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8),(edentity[edi].y*8)- ((ry-100)*30*8), 56,
				edentity[edi].p1, 4, bx1, by1, bx2, by2);
				break;
				case 2: //Platforms and Threadmills
				if(edentity[edi].p1<=4){
					int bx1, by1, bx2, by2;
					bx1=ed.level[rx-100+((ry-100)*ed.maxwidth)].platx1;
					by1=ed.level[rx-100+((ry-100)*ed.maxwidth)].platy1;
					bx2=ed.level[rx-100+((ry-100)*ed.maxwidth)].platx2;
					by2=ed.level[rx-100+((ry-100)*ed.maxwidth)].platy2;

					if(warpx){ if(bx1==0 && bx2==320){ bx1=-100; bx2=420; } }
					if(warpy){ if(by1==0 && by2==240){ by1=-100; by2=340; } }

					obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8),(edentity[edi].y*8)- ((ry-100)*30*8), 2,
					edentity[edi].p1, ed.level[rx-100+((ry-100)*ed.mapwidth)].platv, bx1, by1, bx2, by2);
				}else if(edentity[edi].p1>=5 && edentity[edi].p1<=8){ //Threadmill
					obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8),(edentity[edi].y*8)- ((ry-100)*30*8), 2,
					edentity[edi].p1+3, 4);
				}
				break;
				case 3: //Disappearing platforms
				obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8),(edentity[edi].y*8)- ((ry-100)*30*8), 3);
				break;
				case 9:
				obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8),(edentity[edi].y*8)- ((ry-100)*30*8), 9, ed.findtrinket(edi));
				break;
				case 10: //Checkpoints
				obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8),(edentity[edi].y*8)- ((ry-100)*30*8), 10,
				edentity[edi].p1,((rx+(ry*100))*20)+tempcheckpoints);
				tempcheckpoints++;
				break;
				case 11: //Gravity Lines
				if(edentity[edi].p1==0){ //Horizontal
					obj.createentity((edentity[edi].p2*8),(edentity[edi].y*8)- ((ry-100)*30*8)+4, 11, edentity[edi].p3);
				}else{ //Vertical
					obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8)+3,(edentity[edi].p2*8), 12, edentity[edi].p3);
				}
				break;
				case 13: //Warp Tokens
				obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8),(edentity[edi].y*8)- ((ry-100)*30*8), 13, edentity[edi].p1, edentity[edi].p2);
				break;
				case 15: //Collectable crewmate
				obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8)-4,(edentity[edi].y*8)- ((ry-100)*30*8)+1, 55, ed.findcrewmate(edi), edentity[edi].p1, edentity[edi].p2);
				break;
				case 17: //Roomtext!
				{
				roomtexton = true;
				Roomtext text;
				text.x = edentity[edi].x - ((rx-100)*40);
				text.y = edentity[edi].y - ((ry-100)*30);
				text.text = edentity[edi].scriptname;
				roomtext.push_back(text);
				break;
				}
				case 18: //Terminals
				obj.customscript=edentity[edi].scriptname;
				obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8),(edentity[edi].y*8)- ((ry-100)*30*8)+8, 20, 1);
				obj.createblock(5, (edentity[edi].x*8)- ((rx-100)*40*8)-8, (edentity[edi].y*8)- ((ry-100)*30*8)+8, 20, 16, 35);
				break;
				case 19: //Script Box
				game.customscript[tempscriptbox]=edentity[edi].scriptname;
				obj.createblock(1, (edentity[edi].x*8)- ((rx-100)*40*8), (edentity[edi].y*8)- ((ry-100)*30*8),
								edentity[edi].p1*8, edentity[edi].p2*8, 300+tempscriptbox);
				tempscriptbox++;
				break;
				case 50: //Warp Lines
				obj.customwarpmode=true;
				if(edentity[edi].p1==0){ //
					obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8)+4,(edentity[edi].p2*8), 51, edentity[edi].p3);
				}else if(edentity[edi].p1==1){ //Horizontal, right
					obj.createentity((edentity[edi].x*8)- ((rx-100)*40*8)+4,(edentity[edi].p2*8), 52, edentity[edi].p3);
				}else if(edentity[edi].p1==2){ //Vertical, top
					obj.createentity((edentity[edi].p2*8),(edentity[edi].y*8)- ((ry-100)*30*8)+7, 53, edentity[edi].p3);
				}else if(edentity[edi].p1==3){
					obj.createentity((edentity[edi].p2*8),(edentity[edi].y*8)- ((ry-100)*30*8), 54, edentity[edi].p3);
				}
				break;
			}
			}
			}

		//do the appear/remove roomname here
		break;
#endif
	}
	//The room's loaded: now we fill out damage blocks based on the tiles.
	if (towermode)
	{

	}
	else
	{
		for (int j = 0; j < 29 + extrarow; j++)
		{
			for (int i = 0; i < 40; i++)
			{
				//Damage blocks
				if(tileset==0)
				{
					if (contents[i + vmult[j]] == 6 || contents[i + vmult[j]] == 8)
					{
						//sticking up
						obj.createblock(2, (i * 8), (j * 8)+4, 8, 4);
					}
					if (contents[i + vmult[j]] == 7 || contents[i + vmult[j]] == 9)
					{
						//Sticking down
						obj.createblock(2, (i * 8), (j * 8), 8, 4);
					}
					if (contents[i + vmult[j]] == 49 || contents[i + vmult[j]] == 50)
					{
						//left or right
						obj.createblock(2, (i * 8), (j * 8)+3, 8, 2);
					}
				}
				else if(tileset==1)
				{
					//if (contents[i + vmult[j]] >= 6 && contents[i + vmult[j]] <= 9) obj.createblock(2, (i * 8), (j * 8)+1, 8, 6);
					//if (contents[i + vmult[j]] >= 49 && contents[i + vmult[j]] <= 79) obj.createblock(2, (i * 8) + 1, (j * 8) + 1, 6, 6);
					if ((contents[i + vmult[j]] >= 63 && contents[i + vmult[j]] <= 74) ||
							(contents[i + vmult[j]] >= 6 && contents[i + vmult[j]] <= 9))
					{
						//sticking up) {
						if (contents[i + vmult[j]] < 10) contents[i + vmult[j]]++;
						//sticking up
						if(contents[i + vmult[j]]%2==0)
						{
							obj.createblock(2, (i * 8), (j * 8), 8, 4);
						}
						else
						{
							//Sticking down
							obj.createblock(2, (i * 8), (j * 8) + 4, 8, 4);
						}
						if (contents[i + vmult[j]] < 11) contents[i + vmult[j]]--;
					}
					if (contents[i + vmult[j]] >= 49 && contents[i + vmult[j]] <= 62)
					{
						//left or right
						obj.createblock(2, (i * 8), (j * 8)+3, 8, 2);
					}
				}
				else if(tileset==2)
				{
					if (contents[i + vmult[j]] == 6 || contents[i + vmult[j]] == 8)
					{
						//sticking up
						obj.createblock(2, (i * 8), (j * 8)+4, 8, 4);
					}
					if (contents[i + vmult[j]] == 7 || contents[i + vmult[j]] == 9)
					{
						//Sticking down
						obj.createblock(2, (i * 8), (j * 8), 8, 4);
					}
				}
				//Breakable blocks
				if (contents[i + vmult[j]] == 10)
				{
					contents[i + vmult[j]] = 0;
					obj.createentity(i * 8, j * 8, 4);
				}
				//Directional blocks
				if (contents[i + vmult[j]] >= 14 && contents[i + vmult[j]] <= 17)
				{
					obj.createblock(3, i * 8, j * 8, 8, 8, contents[i + vmult[j]]-14);
				}
			}
		}

		for (size_t i = 0; i < obj.entities.size(); i++)
		{
			if (obj.entities[i].type == 1 && obj.entities[i].behave >= 8 && obj.entities[i].behave < 10)
			{
				//put a block underneath
				temp = obj.entities[i].xp / 8.0f;
				temp2 = obj.entities[i].yp / 8.0f;
				settile(temp, temp2, 1);
				settile(temp+1, temp2, 1);
				settile(temp+2, temp2, 1);
				settile(temp+3, temp2, 1);
				if (obj.entities[i].w == 64)
				{
					settile(temp+4, temp2, 1);
					settile(temp+5, temp2, 1);
					settile(temp+6, temp2, 1);
					settile(temp+7, temp2, 1);
				}
			}
		}
	}

	//Special scripting: Create objects and triggers based on what crewmembers are rescued.
	if (!finalmode && !custommode)
	{
		//First up: the extra bits:
		//Vermilion's quest:
		if (rx == 100 && ry == 105)	 //On path to verdigris
		{
			if (game.crewstats[3] && !game.crewstats[4])
			{
				obj.createentity(87, 105, 18, 15, 0, 18);
				obj.createblock(5, 87-32, 0, 32+32+32, 240, 3);
			}
		}
		else if (rx == 107 && ry == 100)	//victoria
		{
			if (game.crewstats[3] && !game.crewstats[5])
			{
				obj.createentity(140, 137, 18, 15, 0, 18);
				obj.createblock(5, 140-32, 0, 32+32+32, 240, 3);
			}
		}
		else if (rx == 114 && ry == 109)
		{
			if (game.crewstats[3] && !game.crewstats[2])
			{
				obj.createentity(235, 81, 18, 15, 0, 18);
				obj.createblock(5, 235-32, 0, 32+32+32, 240, 3);
			}
		}

		//Verdigris fixing the ship
		if (rx == 101 && ry == 109)
		{
			if (game.crewstats[4])
			{
				if(game.crewrescued()>4 && game.crewrescued()!=6)
				{
					obj.createentity(175, 121, 18, 13, 0, 18);
					obj.createblock(5, 175-32, 0, 32+32+32, 240, 4);
				}
			}
		}
		else if (rx == 103 && ry == 109)
		{
			if (game.crewstats[4])
			{
				if(game.crewrescued()<=4 && game.crewrescued()!=6)
				{
					obj.createentity(53, 161, 18, 13, 1, 18);
					obj.createblock(5, 53-32, 0, 32+32+32, 240, 4);
				}
			}
		}

		if (rx == 104 && ry == 111)
		{
			//Red
			//First: is he rescued?
			if (game.crewstats[3])
			{
				//If so, red will always be at his post
				obj.createentity(107, 121, 18, 15, 0, 18);
				//What script do we use?
				obj.createblock(5, 107-32, 0, 32+32+32, 240, 3);
			}
		}
		else if (rx == 103 && ry == 111)
		{
			//Yellow
			//First: is he rescued?
			if (game.crewstats[2])
			{
				obj.createentity(198, 105, 18, 14, 0, 18);
				//What script do we use?
				obj.createblock(5, 198-32, 0, 32+32+32, 240, 2);
			}
		}
		else if (rx == 103 && ry == 110)
		{
			//Green
			//First: is he rescued?
			if (game.crewstats[4])
			{
				obj.createentity(242, 177, 18, 13, 0, 18);
				//What script do we use?
				obj.createblock(5, 242-32, 177-20, 32+32+32, 40, 4);
			}
		}
		else if (rx == 104 && ry == 110)
		{
			//Purple
			//First: is she rescued?
			if (game.crewstats[1])
			{
				obj.createentity(140, 177, 18, 20, 0, 18);
				//What script do we use?
				obj.createblock(5, 140-32, 0, 32+32+32, 240, 1);
			}
		}
		else if (rx == 102 && ry == 110)
		{
			//Blue
			//First: is she rescued?
			if (game.crewstats[5])
			{
				//A slight varation - she's upside down
				obj.createentity(249, 62, 18, 16, 0, 18);
				int j = obj.getcrewman(5);
				obj.entities[j].rule = 7;
				obj.entities[j].tile +=6;
				//What script do we use?
				obj.createblock(5, 249-32, 0, 32+32+32, 240, 5);
			}
		}
	}

	//Make sure our crewmates are facing the player if appliciable
	//Also make sure they're flipped if they're flipped
	for (size_t i = 0; i < obj.entities.size(); i++)
	{
		if (obj.entities[i].rule == 6 || obj.entities[i].rule == 7)
		{
			if (obj.entities[i].tile == 144 || obj.entities[i].tile == 144+6)
			{
				obj.entities[i].drawframe = 144;
			}
			if (obj.entities[i].state == 18)
			{
				//face the player
				int j = obj.getplayer();
				if (j > -1 && obj.entities[j].xp > obj.entities[i].xp + 5)
				{
					obj.entities[i].dir = 1;
				}
				else if (j > -1 && obj.entities[j].xp < obj.entities[i].xp - 5)
				{
					obj.entities[i].dir = 0;
					obj.entities[i].drawframe += 3;
				}
			}
		}
		if (obj.entities[i].rule == 7)
		{
			obj.entities[i].drawframe += 6;
		}
	}
}

void mapclass::twoframedelayfix()
{
	// Fixes the two-frame delay in custom levels that use scripts to spawn an entity upon room load.
	// Because when the room loads and newscript is set to run, newscript has already ran for that frame,
	// and when the script gets loaded script.run() has already ran for that frame, too.
	// A bit kludge-y, but it's the least we can do without changing the frame ordering.

	if (game.glitchrunnermode
	|| game.deathseq != -1
	// obj.checktrigger() sets obj.activetrigger
	|| obj.checktrigger() <= -1
	|| obj.activetrigger < 300)
	{
		return;
	}

	game.newscript = "custom_" + game.customscript[obj.activetrigger - 300];
	obj.removetrigger(obj.activetrigger);
	game.state = 0;
	game.statedelay = 0;
	script.load(game.newscript);
	if (script.running)
	{
		script.run();
		script.dontrunnextframe = true;
	}
}
