#ifndef OTHERLEVEL_H
#define OTHERLEVEL_H

#include <string>

class otherlevelclass
{
public:
    const short* loadlevel(int rx, int ry);

    std::string roomname;

    int roomtileset;
};

#endif /* OTHERLEVEL_H */
