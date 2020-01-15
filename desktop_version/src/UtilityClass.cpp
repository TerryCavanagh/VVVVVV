#include "UtilityClass.h"

#include "SDL.h"

#include <sstream>

/* Used by UtilityClass::GCString to generate a button list */
const char* GCChar(SDL_GameControllerButton button)
{
	if (button == SDL_CONTROLLER_BUTTON_A)
	{
		return "A";
	}
	else if (button == SDL_CONTROLLER_BUTTON_B)
	{
		return "B";
	}
	else if (button == SDL_CONTROLLER_BUTTON_X)
	{
		return "X";
	}
	else if (button == SDL_CONTROLLER_BUTTON_Y)
	{
		return "Y";
	}
	else if (button == SDL_CONTROLLER_BUTTON_BACK)
	{
		return "BACK";
	}
	else if (button == SDL_CONTROLLER_BUTTON_GUIDE)
	{
		return "GUIDE";
	}
	else if (button == SDL_CONTROLLER_BUTTON_START)
	{
		return "START";
	}
	else if (button == SDL_CONTROLLER_BUTTON_LEFTSTICK)
	{
		return "L3";
	}
	else if (button == SDL_CONTROLLER_BUTTON_RIGHTSTICK)
	{
		return "R3";
	}
	else if (button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
	{
		return "LB";
	}
	else if (button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
	{
		return "RB";
	}
	SDL_assert(0 && "Unhandled button!");
	return NULL;
}

int ss_toi(std::string _s)
{
	std::istringstream i(_s);
	int x;
	i >> x;
	return x;
}

growing_vector<std::string> split(const std::string& s, char delim, growing_vector<std::string>& elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

growing_vector<std::string> split(const std::string& s, char delim)
{
	growing_vector<std::string> elems;
	return split(s, delim, elems);
}

UtilityClass::UtilityClass() :
	glow(0),
	glowdir(0)
{
	for (int i = 0; i < 30; i++)
	{
		splitseconds.push_back(int((i * 100) / 30));
	}

	slowsine = 0;
	globaltemp = 0;
	temp = 0;
	temp2 = 0;
}

std::string UtilityClass::String(int _v)
{
	std::ostringstream os;
	os << _v;
	return(os.str());
}

std::string UtilityClass::GCString(growing_vector<SDL_GameControllerButton> buttons)
{
	std::string retval = "";
	for (size_t i = 0; i < buttons.size(); i += 1)
	{
		retval += GCChar(buttons[i]);
		if ((i + 1) < buttons.size())
		{
			retval += ",";
		}
	}
	return retval;
}

std::string UtilityClass::twodigits(int t)
{
	if (t < 10)
	{
		return "0" + String(t);
	}
	if (t >= 100)
	{
		return "??";
	}
	return String(t);
}

std::string UtilityClass::timestring(int t)
{
	//given a time t in frames, return a time in seconds
	tempstring = "";
	temp = (t - (t % 30)) / 30;
	if (temp < 60)   //less than one minute
	{
		t = t % 30;
		tempstring = String(temp) + ":" + twodigits(splitseconds[t]);
	}
	else
	{
		temp2 = (temp - (temp % 60)) / 60;
		temp = temp % 60;
		t = t % 30;
		tempstring = String(temp2) + ":" + twodigits(temp) + ":" + twodigits(splitseconds[t]);
	}
	return tempstring;
}

std::string UtilityClass::number(int _t)
{
	const int BIGGEST_SMALL_NUMBER = 50;
	const char* smallnumbers[] = { "Zero", "One", "Two", "Three",
		"Four", "Five", "Six", "Seven", "Eight", "Nine",
		"Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen",
		"Sixteen", "Seventeen", "Eighteen", "Nineteen", "Twenty", "Twenty One",
		"Twenty Two", "Twenty Three", "Twenty Four", "Twenty Five",
		"Twenty Six", "Twenty Seven", "Twenty Eight", "Twenty Nine",
		"Thirty", "Thirty One", "Thirty Two", "Thirty Three", "Thirty Four",
		"Thirty Five", "Thirty Six", "Thirty Seven", "Thirty Eight",
		"Thirty Nine", "Forty Zero", "Forty One", "Forty Two", "Forty Three",
		"Forty Four", "Forty Five", "Forty Six", "Forty Seven", "Forty Eight",
		"Forty Nine", "Fifty" };

	if (_t <= BIGGEST_SMALL_NUMBER) {
		return smallnumbers[_t];
	}

	return "Lots";
}

bool UtilityClass::intersects(SDL_Rect A, SDL_Rect B)
{
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;
	//Calculate the sides of rect A
	leftA = A.x;
	rightA = A.x + A.w;
	topA = A.y;
	bottomA = A.y + A.h;
	//Calculate the sides of rect B
	leftB = B.x;
	rightB = B.x + B.w;
	topB = B.y;
	bottomB = B.y + B.h;

	if (bottomA <= topB)
	{
		return false;
	}
	if (topA >= bottomB)
	{
		return false;
	}
	if (rightA <= leftB)
	{
		return false;
	}
	if (leftA >= rightB)
	{
		return false;
	}
	//If none of the sides from A are outside B return true; }

	return true;
}

void UtilityClass::updateglow()
{
	slowsine++;
	if (slowsine >= 64) slowsine = 0;

	if (glowdir == 0) {
		glow += 2;
		if (glow >= 62) glowdir = 1;
	}
	else {
		glow -= 2;
		if (glow < 2) glowdir = 0;
	}
}
