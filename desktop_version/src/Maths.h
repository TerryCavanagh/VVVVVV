#ifndef MATHGAME_H
#define MATHGAME_H

#include <cmath>
#include <stdlib.h>

//// This header holds Maths functions that emulate the functionality of flash's


//random
//Returns 0..1
float inline fRandom()
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

#endif /* MATHGAME_H */
