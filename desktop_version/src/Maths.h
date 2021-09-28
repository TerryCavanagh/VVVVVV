#ifndef MATHGAME_H
#define MATHGAME_H

#include <stdlib.h>

//// This header holds Maths functions that emulate the functionality of flash's


//random
//Returns 0..1
float inline fRandom(void)
{
    return ( float(rand()) / float(RAND_MAX)) ;
}

inline int clamp(int x, int a, int b)
{
    return x < a ? a : (x > b ? b : x);
}

struct point
{
    int x;
    int y;
};

inline int VVV_min(const int a, const int b)
{
    if (a < b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

inline int VVV_max(const int a, const int b)
{
    if (a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

#endif /* MATHGAME_H */
