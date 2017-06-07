#include "entities.h"
#include "sys_camera.h"
#include "sys_collision.h"
#include "sys_transform.h"
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

/* dist returns the distance between the colliders. */
static float dist(struct Collider *c1, struct Collider *c2) {
	vec3 p1, p2;
	if (TransformGetPos(c1->e, p1) && TransformGetPos(c2->e, p2))
		return sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) +
		            (p1[1] - p2[0]) * (p1[1] - p2[1]) +
		            (p1[2] - p2[2]) * (p2[2] - p2[2]));
	return -1.f;
}

/* GetCollider returns the collider attached to entity e (if there is one). */
struct Collider *GetCollider(Entity e) {
	struct entityToCollider *t;

	if (entitiesToColliders == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToColliders, &e, t);
	if (t == NULL)
		return NULL;

	return t->collider;
}

/* InitColliderSystem initializes the collider system. */
void InitColliderSystem() {
	if (entitiesToColliders != NULL) {
		struct entityToCollider *c, *tmp;
		HASH_ITER(hh, entitiesToColliders, c, tmp) {
			HASH_DEL(entitiesToColliders,
			         c); /* delete; users advances to next */
			free(c);     /* optional- if you want to free  */
		}
	}
	numColliders = 0;
}

/* UpdateColliderSystem updates all colliders that have been created. */
void UpdateColliderSystem() {
	int i;
	/* XXX: sort */

	for (i = 0; i < numColliders; ++i) {
		int j;
		struct Collider *c1;
		c1 = &colliders[i];

		for (j = 0; j < numColliders; ++j) {
			struct Collider *c2;
			if (i == j)
				continue;
			c2 = &colliders[j];
			if (dist(c1, c2) < (c1->radius + c2->radius)) {
				/* collision between c1 and c2 */
				c1->update =
				    (struct ColliderUpdate){.into = c2->e};
			}
		}
	}
}

/* AddCollider adds a collider component to the entity e. */
void AddCollider(Entity e, float r, enum CollisionLayer layers) {
	struct entityToCollider *item;

	if (GetCollider(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToCollider));
	item->collider = colliders + numColliders;
	item->e = e;

	colliders[numColliders].e = e;
	colliders[numColliders].radius = r;
	colliders[numColliders].layers = layers;

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
		numColliders--;
	}
}

/* GetColliderUpdate returns any collider updates for the entity e. */
struct ColliderUpdate *GetColliderUpdate(Entity e) {
	struct Collider *c;

	if ((c = GetCollider(e)))
		return &c->update;

	return NULL;
}

/* GetColliderUpdates returns the collider updates this frame. */
struct ColliderUpdate *GetColliderUpdates(int *num) {
	*num = numColliderUpdates;
	return updates;
}
