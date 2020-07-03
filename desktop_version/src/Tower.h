#ifndef TOWER_H
#define TOWER_H

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

    int back[40 * 120];
    int contents[40 * 700];
    int minitower[40 * 100];
    int vmult[40 * 700];

    bool minitowermode;
};





#endif /* TOWER_H */
