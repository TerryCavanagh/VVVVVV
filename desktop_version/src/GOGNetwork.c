#include <stdint.h>

/* Totally unimplemented right now! */

int32_t GOG_init()
{
	return 0;
}

void GOG_shutdown()
{
}

void GOG_update()
{
}

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) x=x;
#endif
void GOG_unlockAchievement(const char *name)
{
	UNREFERENCED_PARAMETER(name)
}

int32_t GOG_getAchievementProgress(const char *name)
{
	UNREFERENCED_PARAMETER(name)
	return 0;
}

void GOG_setAchievementProgress(const char *name, int32_t stat)
{
	UNREFERENCED_PARAMETER(name)
	UNREFERENCED_PARAMETER(stat)
}
