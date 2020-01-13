#include "Network.h"

#define NUM_BACKENDS 2
#define DECLARE_BACKEND(name) \
	extern int name##_init(); \
	extern void name##_shutdown(); \
	extern void name##_update(); \
	extern void name##_unlockAchievement(); \
	extern int32_t name##_getAchievementProgress(const char *name); \
	extern void name##_setAchievementProgress(const char *name, int32_t stat);
DECLARE_BACKEND(STEAM)
DECLARE_BACKEND(GOG)
#undef DECLARE_BACKEND

typedef struct NetworkBackend
{
	int32_t IsInit;
	int32_t (*Init)();
	void (*Shutdown)();
	void (*Update)();
	void (*UnlockAchievement)();
	int32_t (*GetAchievementProgress)(const char*);
	void (*SetAchievementProgress)(const char*, int32_t);
} NetworkBackend;

static NetworkBackend backends[NUM_BACKENDS];

int NETWORK_init()
{
	int32_t i, any = 0;
	#define ASSIGN_BACKEND(name, index) \
		backends[index].Init = name##_init; \
		backends[index].Shutdown = name##_shutdown; \
		backends[index].Update = name##_update; \
		backends[index].UnlockAchievement = name##_unlockAchievement; \
		backends[index].GetAchievementProgress = name##_getAchievementProgress; \
		backends[index].SetAchievementProgress = name##_setAchievementProgress;
	ASSIGN_BACKEND(STEAM, 0)
	ASSIGN_BACKEND(GOG, 1)
	#undef ASSIGN_BACKEND
	for (i = 0; i < NUM_BACKENDS; i += 1)
	{
		backends[i].IsInit = backends[i].Init();
		any |= backends[i].IsInit;
	}
	return any;
}

void NETWORK_shutdown()
{
	int32_t i;
	for (i = 0; i < NUM_BACKENDS; i += 1)
	if (backends[i].IsInit)
	{
		backends[i].Shutdown();
	}
}

void NETWORK_update()
{
	int32_t i;
	for (i = 0; i < NUM_BACKENDS; i += 1)
	if (backends[i].IsInit)
	{
		backends[i].Update();
	}
}

void NETWORK_unlockAchievement(const char *name)
{
	int32_t i;
	for (i = 0; i < NUM_BACKENDS; i += 1)
	if (backends[i].IsInit)
	{
		backends[i].UnlockAchievement(name);
	}
}

int32_t NETWORK_getAchievementProgress(const char *name)
{
	/* The highest stat gets priority, will eventually pass to the others */
	int32_t i, stat, max = 0;
	for (i = 0; i < NUM_BACKENDS; i += 1)
	if (backends[i].IsInit)
	{
		stat = backends[i].GetAchievementProgress(name);
		if (stat > max)
		{
			max = stat;
		}
	}
	return max;
}

void NETWORK_setAchievementProgress(const char *name, int32_t stat)
{
	int32_t i;
	for (i = 0; i < NUM_BACKENDS; i += 1)
	if (backends[i].IsInit)
	{
		backends[i].SetAchievementProgress(name, stat);
	}
}
