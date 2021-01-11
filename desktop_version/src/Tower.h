#pragma once

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

    short back[40 * 120];
    short contents[40 * 700];
    short minitower[40 * 100];
    int vmult[40 * 700];

    bool minitowermode;
};
