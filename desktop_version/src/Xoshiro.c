#include <stdint.h>

#include "Vlogging.h"

/* Implements the xoshiro128+ PRNG. */

static uint32_t rotl(const uint32_t x, const int k)
{
    return (x << k) | (x >> (32 - k));
}

static uint32_t s[4];

static uint32_t splitmix32(uint32_t* x)
{
    uint32_t z = (*x += 0x9e3779b9UL);
    z = (z ^ (z >> 15)) * 0xbf58476dUL;
    z = (z ^ (z >> 13)) * 0x94d049bbUL;
    return z ^ (z >> 16);
}

static void seed(
    const uint32_t s0,
    const uint32_t s1,
    const uint32_t s2,
    const uint32_t s3
) {
    s[0] = s0;
    s[1] = s1;
    s[2] = s2;
    s[3] = s3;
}

uint32_t xoshiro_next(void)
{
    const uint32_t result = s[0] + s[3];

    const uint32_t t = s[1] << 9;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;

    s[3] = rotl(s[3], 11);

    vlog_debug("Next xoshiro is %u.", result);

    return result;
}

void xoshiro_seed(uint32_t s)
{
    vlog_debug("Xoshiro seeded with %u.", s);

    const uint32_t s0 = splitmix32(&s);
    const uint32_t s1 = splitmix32(&s);
    const uint32_t s2 = splitmix32(&s);
    const uint32_t s3 = splitmix32(&s);
    seed(s0, s1, s2, s3);
}

float xoshiro_rand(void)
{
    return ((float) xoshiro_next()) / ((float) UINT32_MAX);
}
