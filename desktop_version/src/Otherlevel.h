#ifndef OTHERLEVEL_H
#define OTHERLEVEL_H

#include "Game.h"
#include "Entity.h"

#include <string>
#include <vector>

struct Roomtext
{
    int x, y;
    std::string text;
};

class otherlevelclass
{
public:
    enum
    {
        BLOCK = 0,
        TRIGGER,
        DAMAGE,
        DIRECTIONAL,
        SAFE,
        ACTIVITY
    };

    void addline(std::string t);
    std::vector<std::string> loadlevel(int rx, int ry);

    std::string roomname;

    int roomtileset;
    int i;

    // roomtext thing in other level
    bool roomtexton;
    std::vector<Roomtext> roomtext;
};

#endif /* OTHERLEVEL_H */
