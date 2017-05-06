#ifndef SYS_MOVEMENT_H
#define SYS_MOVEMENT_H

#include "base.h"
#include "entity.h"

/* Movement is a component that represents the movement capabilities of an
 * entity. */
struct Movement {
	float speed;
	struct {
		float x, y, z;
	} dest;
};

/* MovementUpdate is the update message used to communicate entity movement. */
struct MovementUpdate {
	Entity e;
	float dx, dy, dz;
};

void InitMovementSystem();
void UpdateMovementSystem();
void AddMovement(Entity, float);
void RemoveMovement(Entity);
struct MovementUpdate *GetMovementUpdates(int *);
void MovementMoveTo(Entity, float, float, float);

#endif
