#ifndef VNETWORK_H
#define VNETWORK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t NETWORK_init();

void NETWORK_shutdown();

void NETWORK_update();

void NETWORK_unlockAchievement(const char *name);

int32_t NETWORK_getAchievementProgress(const char *name);

void NETWORK_setAchievementProgress(const char *name, int32_t stat);

#ifdef __cplusplus
}
#endif

#endif /* VNETWORK_H */
