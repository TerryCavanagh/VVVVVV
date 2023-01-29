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

#endif /* MATHGAME_H */
