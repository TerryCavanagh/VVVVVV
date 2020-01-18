#ifndef LABCLASS_H
#define LABCLASS_H

#include "Game.h"
#include "Entity.h"

#include <vector>
#include <string>

class labclass
{
public:
    std::vector<growing_vector<std::string>> loadlevel(int rx, int ry , Game& game, entityclass& obj);

    std::string roomname;
    int coin, rcol;
};
#endif /* LABCLASS_H */
