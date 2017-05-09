#ifndef SYS_GUN_H
#define SYS_GUN_H

#include "base.h"

#include "entity.h"
#include "third-party/include/linmath.h"
#include <stdbool.h>

enum { GUN_MAX_BULLETS_ALIVE = 2,

};

/* GunUpdate defines an update message that can be polled by interested
 * systems. */
struct GunUpdate {
	Entity e;
	float cooldown;
	Entity bullets[GUN_MAX_BULLETS_ALIVE];
};

/* Gun is a struct that represents the position/orientation of something.
 */
struct Gun {
	Entity e;
	Prefab projectile;
	float rate;

	struct GunUpdate update;
};

void InitGunSystem();
void UpdateGunSystem();
void AddGun(Entity, Prefab, float);
void RemoveGun(Entity);
void GunSystemClearUpdates();
struct GunUpdate *GetGunUpdates(int *);
struct GunUpdate *GetGunUpdate(Entity);

void GunFire(Entity);

extern int numGunUpdates;

#endif
