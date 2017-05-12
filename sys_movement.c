#include "sys_movement.h"
#include "sys_time.h"
#include "sys_transform.h"
#include "third-party/include/uthash.h"

struct entityToMovement {
	Entity e;
	struct Movement *movement;
	UT_hash_handle hh;
};

static struct entityToMovement *entitiesToMovements;
static struct Movement movements[MAX_MOVEMENTS];
static int numMovements;

static int numUpdates;
static struct MovementUpdate updates[MAX_MOVEMENTS];

/* getMovement returns the movement attached to entity e (if there is one). */
static struct Movement *getMovement(Entity e) {
	struct entityToMovement *m;

	if (entitiesToMovements == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToMovements, &e, m);
	if (m == NULL)
		return NULL;

	return m->movement;
}

/* InitMovementSystem initializes the movement system. */
void InitMovementSystem() {}

/* UpdateMovementSystem updates all movements that have been created. */
void UpdateMovementSystem() {
	static float dt = 0.f;
	int i;
	vec3 newPos;

	dt = GetTime() - dt;
	for (i = 0; i < numMovements; ++i) {
		if (GetPos(movements[i].e, newPos)) {
			vec3 dpos;
			vec3_scale(dpos, movements[i].dir, dt);
			vec3_add(newPos, newPos, dpos);
			TransformSet(movements[i].e, newPos[0], newPos[1],
			             newPos[2]);
		}
	}
}

/* AddMovement adds a movement component to the entity e. */
void AddMovement(Entity e, float vel, vec3 dir) {
	struct entityToMovement *item;

	if (getMovement(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToMovement));
	item->movement = movements + numMovements;
	item->e = e;

	movements[numMovements].vel = vel;
	movements[numMovements].dir[0] = dir[0];
	movements[numMovements].dir[1] = dir[1];
	movements[numMovements].dir[2] = dir[2];

	HASH_ADD_INT(entitiesToMovements, e, item);
	numMovements++;
}

/* RemoveMovement removes the movement attached to e from the Movement system.
 */
void RemoveMovement(Entity e) {
	struct entityToMovement *c;

	if (entitiesToMovements == NULL)
		return;

	HASH_FIND_INT(entitiesToMovements, &e, c);
	if (c != NULL) {
		struct Movement *sys = c->movement;
		int sz = (movements + numMovements) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToMovements, c);
		free(c);
	}
}

/* GetMovementUpdates returns the movement updates this frame. */
struct MovementUpdate *GetMovementUpdates(int *num) {
	*num = numUpdates;
	return updates;
}
