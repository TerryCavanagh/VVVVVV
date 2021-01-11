#pragma once

#include <string>

class warpclass
{
public:
	const short* loadlevel(int rx, int ry);
	std::string roomname;
	int rcol;
	bool warpx, warpy;
};
