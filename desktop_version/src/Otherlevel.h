#ifndef OTHERLEVEL_H
#define OTHERLEVEL_H

#include "Game.h"
#include "Entity.h"

#include <string>
#include <vector>

class otherlevelclass
{
public:
    const int* loadlevel(int rx, int ry);

    std::string roomname;

    int roomtileset;
};

#endif /* OTHERLEVEL_H */
