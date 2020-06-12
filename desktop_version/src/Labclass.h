#ifndef LABCLASS_H
#define LABCLASS_H

#include "Game.h"
#include "Entity.h"

#include <vector>
#include <string>

class labclass
{
public:
    std::vector<int>  loadlevel(int rx, int ry);

    std::string roomname;
    int coin, rcol;
};
#endif /* LABCLASS_H */
