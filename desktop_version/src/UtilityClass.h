#ifndef UTILITYCLASS_H
#define UTILITYCLASS_H

#include <SDL.h>
#include <string>
#include <vector>

int ss_toi(std::string _s);

std::vector<std::string> split(const std::string &s, char delim, std::vector<std::string> &elems);

std::vector<std::string> split(const std::string &s, char delim);

bool is_number(const char* str);

bool is_positive_num(const std::string& str, bool hex);

bool endsWith(const std::string& str, const std::string& suffix);

#define INBOUNDS_VEC(index, vector) ((int) index >= 0 && (int) index < (int) vector.size())
#define INBOUNDS_ARR(index, array) ((int) index >= 0 && (int) index < (int) SDL_arraysize(array))

#define WHINE_ONCE(message) \
    static bool whine = true; \
    if (whine) \
    { \
        whine = false; \
        puts(message); \
    }


//helperClass
class UtilityClass
{
public:
    UtilityClass();

    static std::string String(int _v);

    static int Int(const char* str, int fallback = 0);

    static std::string GCString(std::vector<SDL_GameControllerButton> buttons);

    std::string twodigits(int t);

    std::string timestring(int t);

    std::string number(int _t);


    static bool intersects( SDL_Rect A, SDL_Rect B );

    void updateglow();

    int glow;
    int slowsine;
    int glowdir;
    int splitseconds[30];
};

#ifndef HELP_DEFINITION
extern UtilityClass help;
#endif

#endif /* UTILITYCLASS_H */
