#include "MakeAndPlay.h"

#ifndef MAKEANDPLAY

#include <stdint.h>

/* Totally unimplemented right now! */

int32_t GOG_init(void)
{
    return 0;
}

void GOG_shutdown(void)
{
}

int32_t GOG_update(void)
{
     return 0;
}

void GOG_unlockAchievement(const char* name)
{
}

void GOG_setRPC(const char* area, const char* roomname)
{
}


#endif /* MAKEANDPLAY */
