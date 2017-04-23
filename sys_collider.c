#include "entities.h"
#include "sys_camera.h"
#include "sys_collision.h"
#include "third-party/include/uthash.h"

struct entityToCollider {
	Entity e;
	struct Collider *collider;
	UT_hash_handle hh;
};

static struct entityToCollider *entitiesToColliders;
static struct Collider colliders[MAX_COLLIDERS];
static int numColliders;

int numColliderUpdates;
static struct ColliderUpdate updates[MAX_COLLIDERS];

/* getCollider returns the collider attached to entity e (if there is one). */
static struct Collider *getCollider(Entity e) {
	struct entityToCollider *t;

	if (entitiesToColliders == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToColliders, &e, t);
	if (t == NULL)
		return NULL;

	return t->collider;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct ColliderUpdate *u) {
	updates[numColliderUpdates++] = *u;
}

/* InitColliderSystem initializes the collider system. */
void InitColliderSystem() {}

/* UpdateColliderSystem updates all colliders that have been created. */
void UpdateColliderSystem() {}

/* AddCollider adds a collider component to the entity e. */
void AddCollider(Entity e, float r) {
	struct entityToCollider *item;

	if (getCollider(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToCollider));
	item->collider = colliders + numColliders;
	item->e = e;

	colliders[numColliders].e = e;
	colliders[numColliders].radius = r;

	HASH_ADD_INT(entitiesToColliders, e, item);
	numColliders++;
}

/* RemoveCollider removes the collider attached to e from the Collider
 * system. */
void RemoveCollider(Entity e) {
	struct entityToCollider *c;

	if (entitiesToColliders == NULL)
		return;

	HASH_FIND_INT(entitiesToColliders, &e, c);
	if (c != NULL) {
		struct Collider *sys = c->collider;
		int sz = (colliders + numColliders) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToColliders, c);
		free(c);
	}
}

/* GetColliderUpdate returns any collider updates for the entity e. */
struct ColliderUpdate *GetColliderUpdate(Entity e) {
	int i;

	for (i = 0; i < numColliderUpdates; ++i) {
		if (updates[i].e == e)
			return updates + i;
	}

	return NULL;
}

/* GetColliderUpdates returns the collider updates this frame. */
struct ColliderUpdate *GetColliderUpdates(int *num) {
	*num = numColliderUpdates;
	return updates;
}
