#ifndef XOSHIRO_H
#define XOSHIRO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

void xoshiro_seed(uint32_t s);

uint32_t xoshiro_next(void);

float xoshiro_rand(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* XOSHIRO_H */
