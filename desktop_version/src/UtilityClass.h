#ifndef UTILITYCLASS_H
#define UTILITYCLASS_H

#include <SDL.h>
#include <vector>
#include <string>

template<typename T>
class growing_vector : public std::vector<T> {
public:
    template<typename Dummy = void>
    growing_vector<T>(const std::vector<T>& v) : std::vector<T>(v) {}
    using std::vector<T>::vector;

    template<typename T2 = T>
    T2 const& operator[](typename std::vector<T2>::size_type index) const {
        typename std::vector<T>::size_type needed_size = index + 1;
        if (this->size() < needed_size) {
            this->resize(needed_size);
        }
        return std::vector<T>::operator[](index);
    }
    template<typename T2 = T>
    T2& operator[](typename std::vector<T2>::size_type index) {
        typename std::vector<T>::size_type needed_size = index + 1;
        if (this->size() < needed_size) {
            this->resize(needed_size);
        }
        return std::vector<T>::operator[](index);
    }
};

int ss_toi(std::string _s);

growing_vector<std::string> split(const std::string& s, char delim, growing_vector<std::string>& elems);

growing_vector<std::string> split(const std::string& s, char delim);


//helperClass
class UtilityClass
{
public:
    UtilityClass();

    static std::string String(int _v);

    static std::string GCString(growing_vector<SDL_GameControllerButton> buttons);

    std::string twodigits(int t);

    std::string timestring(int t);

    std::string number(int _t);


    static bool intersects(SDL_Rect A, SDL_Rect B);

    void updateglow();

    int glow;
    int slowsine;
    int glowdir;
    int globaltemp;
    int temp;
    int temp2;
    std::string tempstring;
    growing_vector<int> splitseconds;
};

#endif /* UTILITYCLASS_H */
