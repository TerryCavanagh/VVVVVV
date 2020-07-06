#ifndef FINALCLASS_H
#define FINALCLASS_H

#include "Game.h"
#include "Entity.h"

#include <string>

class finalclass
{
public:
    const int* loadlevel(int rx, int ry);

    std::string roomname;
    int rcol;
    bool warpx, warpy;
};

#endif /* FINALCLASS_H */
