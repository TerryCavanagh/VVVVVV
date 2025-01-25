#ifndef VNETWORK_H
#define VNETWORK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t NETWORK_init(void);

void NETWORK_shutdown(void);

int32_t NETWORK_update(void);

void NETWORK_unlockAchievement(const char *name);

int32_t NETWORK_getAchievementProgress(const char *name);

void NETWORK_setAchievementProgress(const char *name, int32_t stat);

void NETWORK_setRPC(const char *area, const char *roomname);

#ifdef __cplusplus
}
#endif

#endif /* VNETWORK_H */
