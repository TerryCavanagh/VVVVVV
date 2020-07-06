#ifndef LABCLASS_H
#define LABCLASS_H

#include "Game.h"
#include "Entity.h"

#include <string>

class labclass
{
public:
    const int* loadlevel(int rx, int ry);

    std::string roomname;
    int rcol;
};
#endif /* LABCLASS_H */
