#include "UtilityClass.h"

#include "SDL.h"

#include <sstream>

/* Used by UtilityClass::GCString to generate a button list */
const char *GCChar(SDL_GameControllerButton button)
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

int ss_toi( std::string _s )
{
	std::istringstream i(_s);
	int x;
	i >> x;
	return x;
}

std::vector<std::string> split( const std::string &s, char delim, std::vector<std::string> &elems )
{
	std::stringstream ss(s);
	std::string item;
	while(std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split( const std::string &s, char delim )
{
	std::vector<std::string> elems;
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

std::string UtilityClass::String( int _v )
{
	std::ostringstream os;
	os << _v;
	return(os.str());
}

std::string UtilityClass::GCString(std::vector<SDL_GameControllerButton> buttons)
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

std::string UtilityClass::twodigits( int t )
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

std::string UtilityClass::timestring( int t )
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

std::string UtilityClass::number( int _t )
{
	switch(_t)
	{
	case 0:
		return "Zero";
		break;
	case 1:
		return "One";
		break;
	case 2:
		return "Two";
		break;
	case 3:
		return "Three";
		break;
	case 4:
		return "Four";
		break;
	case 5:
		return "Five";
		break;
	case 6:
		return "Six";
		break;
	case 7:
		return "Seven";
		break;
	case 8:
		return "Eight";
		break;
	case 9:
		return "Nine";
		break;
	case 10:
		return "Ten";
		break;
	case 11:
		return "Eleven";
		break;
	case 12:
		return "Twelve";
		break;
	case 13:
		return "Thirteen";
		break;
	case 14:
		return "Fourteen";
		break;
	case 15:
		return "Fifteen";
		break;
	case 16:
		return "Sixteen";
		break;
	case 17:
		return "Seventeen";
		break;
	case 18:
		return "Eighteen";
		break;
	case 19:
		return "Nineteen";
		break;
	case 20:
		return "Twenty";
		break;
	case 21:
		return "Twenty One";
		break;
	case 22:
		return "Twenty Two";
		break;
	case 23:
		return "Twenty Three";
		break;
	case 24:
		return "Twenty Four";
		break;
	case 25:
		return "Twenty Five";
		break;
	case 26:
		return "Twenty Six";
		break;
	case 27:
		return "Twenty Seven";
		break;
	case 28:
		return "Twenty Eight";
		break;
	case 29:
		return "Twenty Nine";
		break;
	case 30:
		return "Thirty";
		break;
  case 31:
		return "Thirty One";
		break;
	case 32:
		return "Thirty Two";
		break;
	case 33:
		return "Thirty Three";
		break;
	case 34:
		return "Thirty Four";
		break;
	case 35:
		return "Thirty Five";
		break;
	case 36:
		return "Thirty Six";
		break;
	case 37:
		return "Thirty Seven";
		break;
	case 38:
		return "Thirty Eight";
		break;
	case 39:
		return "Thirty Nine";
		break;
	case 40:
		return "Forty";
		break;
  case 41:
		return "Forty One";
		break;
	case 42:
		return "Forty Two";
		break;
	case 43:
		return "Forty Three";
		break;
	case 44:
		return "Forty Four";
		break;
	case 45:
		return "Forty Five";
		break;
	case 46:
		return "Forty Six";
		break;
	case 47:
		return "Forty Seven";
		break;
	case 48:
		return "Forty Eight";
		break;
	case 49:
		return "Forty Nine";
		break;
	case 50:
		return "Fifty";
		break;
	}
	return "Lots";
}

bool UtilityClass::intersects( SDL_Rect A, SDL_Rect B )
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

	if( bottomA <= topB )
	{
		return false;
	}
	if( topA >= bottomB )
	{
		return false;
	}
	if( rightA <= leftB )
	{
		return false;
	}
	if( leftA >= rightB )
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
		glow+=2;
		if (glow >= 62) glowdir = 1;
	}else {
		glow-=2;
		if (glow < 2) glowdir = 0;
	}
}
