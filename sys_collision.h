#ifndef SYS_COLLIDER_H
#define SYS_COLLIDER_H

#include "entity.h"
#include "third-party/include/linmath.h"
#include <stdbool.h>

/* Collider is a struct that represents a bounds that can be collided with.
 */
struct Collider {
	Entity e;
	float radius;
};

/* ColliderUpdate defines a collider update message. */
struct ColliderUpdate {
	Entity e;
	Entity into;
};

void InitColliderSystem();
void UpdateColliderSystem();
void AddCollider(Entity, float);
void RemoveCollider(Entity);
void ColliderSystemClearUpdates();
struct ColliderUpdate *GetColliderUpdates(int *);
struct ColliderUpdate *GetColliderUpdate(Entity);

void ColliderSetRadius(Entity, float);

bool GetColliderRadius(Entity, float *);

extern int numColliderUpdates;

#endif
