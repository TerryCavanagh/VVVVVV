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

void GOG_update(void)
{
}

void GOG_unlockAchievement(const char *name)
{
}

int32_t GOG_getAchievementProgress(const char *name)
{
	return 0;
}

void GOG_setAchievementProgress(const char *name, int32_t stat)
{
}

#endif /* MAKEANDPLAY */
