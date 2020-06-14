#ifndef TOWER_H
#define TOWER_H

#include <string>
#include <vector>

class towerclass
{
public:
    towerclass();

    int backat(int xp, int yp, int yoff);

    int at(int xp, int yp, int yoff);

    int miniat(int xp, int yp, int yoff);

    void loadminitower1();

    void loadminitower2();

    void loadbackground();

    void loadmap();

    //public var back:Array = new Array();
    //public var contents:Array = new Array();
    //public var minitower:Array = new Array();
    //public var vmult:Array = new Array();

    std::vector<int> back;
    std::vector<int> contents;
    std::vector<int> minitower;
    std::vector<int> vmult;

    bool minitowermode;
};





#endif /* TOWER_H */
