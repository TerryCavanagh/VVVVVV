#ifndef UTILITYCLASS_H
#define UTILITYCLASS_H

#include <SDL.h>
#include <vector>
#include <string>

int ss_toi(std::string _s);

std::vector<std::string> split(const std::string &s, char delim, std::vector<std::string> &elems);

std::vector<std::string> split(const std::string &s, char delim);

bool is_positive_num(const std::string& str, bool hex);

#define INBOUNDS(index, vector) ((int) index >= 0 && (int) index < (int) vector.size())


//helperClass
class UtilityClass
{
public:
    UtilityClass();

    static std::string String(int _v);

    static std::string GCString(std::vector<SDL_GameControllerButton> buttons);

    std::string twodigits(int t);

    std::string timestring(int t);

    std::string number(int _t);


    static bool intersects( SDL_Rect A, SDL_Rect B );

    void updateglow();

    int glow;
    int slowsine;
    int glowdir;
    int globaltemp;
    int temp;
    int temp2;
    std::vector<int> splitseconds;
};

extern UtilityClass help;

#endif /* UTILITYCLASS_H */
