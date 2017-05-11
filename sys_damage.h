#ifndef SYS_PROJECTILE_H
#define SYS_PROJECTILE_H

#include "base.h"

#include "entity.h"
#include "third-party/include/linmath.h"
#include <stdbool.h>

struct Damage {
	Entity e;
	float value;
};

/* DamageUpdate defines an update message that can be polled by interested
 * systems. */
struct DamageUpdate {
	Entity e;
	float vel;
	float time;
};

void InitDamageSystem();
void UpdateDamageSystem();
void AddDamage(Entity, vec3, float);
void RemoveDamage(Entity);
void DamageSystemClearUpdates();
struct DamageUpdate *GetDamageUpdates(int *);
struct DamageUpdate *GetDamageUpdate(Entity);

extern int numDamageUpdates;

#endif
