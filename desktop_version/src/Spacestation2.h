#ifndef SPACESTATION2_H
#define SPACESTATION2_H

#include "Game.h"
#include "Entity.h"

#include <string>
#include <vector>

class spacestation2class
{
public:
	std::vector<int> loadlevel(int rx, int ry);
	std::string roomname;
};

#endif /* SPACESTATION2_H */
