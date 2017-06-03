#ifndef SYS_COLLIDER_H
#define SYS_COLLIDER_H

#include "entity.h"
#include "third-party/include/linmath.h"
#include <stdbool.h>

/* CollisionLayer is a layer that colliders may be flagged as. */
enum CollisionLayer {
	COLLISION_LAYER_ENVIRONMENT = 1 << 0,
	COLLISION_LAYER_ENEMY = 1 << 1,
};

/* ColliderUpdate defines a collider update message. */
struct ColliderUpdate {
	Entity into;
};

/* Collider is a struct that represents a bounds that can be collided with.
 */
struct Collider {
	Entity e;
	enum CollisionLayer layers;
	float radius;
	struct ColliderUpdate update;
};

void InitColliderSystem();
void UpdateColliderSystem();
void AddCollider(Entity, float, enum CollisionLayer);
void RemoveCollider(Entity);
void ColliderSystemClearUpdates();
struct Collider *GetCollider(Entity);
struct ColliderUpdate *GetColliderUpdates(int *);
struct ColliderUpdate *GetColliderUpdate(Entity);

void ColliderSetRadius(Entity, float);

bool GetColliderRadius(Entity, float *);

extern int numColliderUpdates;

#endif
