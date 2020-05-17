#ifndef WARPCLASS_H
#define WARPCLASS_H

#include "Game.h"
#include "Entity.h"

#include <string>
#include <vector>

class warpclass
{
public:
	std::vector<int> loadlevel(int rx, int ry);
	std::string roomname;
	int coin, rcol;
	bool warpx, warpy;
};

#endif /* WARPCLASS_H */
