#ifndef SYS_MOVEMENT_H
#define SYS_MOVEMENT_H

#include "base.h"
#include "entity.h"

#include "third-party/include/linmath.h"

/* Movement is a component that represents the movement capabilities of an
 * entity. */
struct Movement {
	Entity e;
	float vel;
	vec3 dir;
};

/* MovementUpdate is the update message used to communicate entity movement. */
struct MovementUpdate {
	Entity e;
	float dx, dy, dz;
};

void InitMovementSystem();
void UpdateMovementSystem();
void AddMovement(Entity, float, vec3);
void RemoveMovement(Entity);
struct MovementUpdate *GetMovementUpdates(int *);
void MovementSetDir(Entity, vec3);
void MovementSetDirTo(Entity, Entity);

#endif
