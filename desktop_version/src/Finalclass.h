#ifndef FINALCLASS_H
#define FINALCLASS_H

#include "Game.h"
#include "Entity.h"

#include <string>
#include <vector>

class finalclass
{
public:
    std::vector<int> loadlevel(int rx, int ry);

    std::string roomname;
    int coin, rcol;
    bool warpx, warpy;
};

#endif /* FINALCLASS_H */
