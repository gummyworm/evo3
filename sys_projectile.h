#ifndef SYS_PROJECTILE_H
#define SYS_PROJECTILE_H

#include "base.h"

#include "entity.h"
#include "third-party/include/linmath.h"
#include <stdbool.h>

/* Projectile is a struct that represents the position/orientation of something.
 */
struct Projectile {
	Entity e;
	float vel;
	vec3 dir;
};

/* ProjectileUpdate defines an update message that can be polled by interested
 * systems. */
struct ProjectileUpdate {
	Entity e;
	float vel;
};

void InitProjectileSystem();
void UpdateProjectileSystem();
void AddProjectile(Entity, vec3, float);
void RemoveProjectile(Entity);
void ProjectileSystemClearUpdates();
struct ProjectileUpdate *GetProjectileUpdates(int *);
struct ProjectileUpdate *GetProjectileUpdate(Entity);

extern int numProjectileUpdates;

#endif
