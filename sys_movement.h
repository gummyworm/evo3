#ifndef SYS_MOVEMENT_H
#define SYS_MOVEMENT_H

#include "entity.h"

enum { MAX_MOVEMENTS = 8192,
};

/* Movement is a component that represents the movement capabilities of an
 * entity. */
struct Movement {
	float speed;
};

/* MovementUpdate is the update message used to communicate entity movement. */
struct MovementUpdate {
	Entity e;
	float dx, dy, dz;
};

void InitMovementSystem();
void UpdateMovementSystem();
void AddMovement(Entity, float speed);
struct MovementUpdate *GetMovementUpdates(int *);

#endif
