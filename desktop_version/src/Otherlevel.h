#ifndef OTHERLEVEL_H
#define OTHERLEVEL_H

#include "Game.h"
#include "Entity.h"

#include <string>
#include <vector>

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

    otherlevelclass();
    void addline(std::string t);
    std::vector<std::string> loadlevel(int rx, int ry , Game& game, entityclass& obj);

    std::string roomname;

    int roomtileset;
    int i;

    // roomtext thing in other level
    bool roomtexton;
    int roomtextx, roomtexty, roomtextnumlines;
    std::vector<std::string> roomtext;
};

#endif /* OTHERLEVEL_H */
