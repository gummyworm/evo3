#include "sys_movement.h"
#include "sys_movement.h"
#include "third-party/include/uthash.h"

struct entityToMovement {
	Entity e;
	struct Movement *movement;
	UT_hash_handle hh;
};

static struct entityToMovement *entitiesToMovements;
struct Movement movements[MAX_MOVEMENTS];
static int numMovements;

static int numUpdates;
static struct MovementUpdate updates[MAX_MOVEMENTS];

/* getMovement returns the movement attached to entity e (if there is one). */
static struct Movement *getMovement(Entity e) {
	struct entityToMovement *t;
	HASH_FIND_INT(entitiesToMovements, &e, t);
	e = t->e;
	return movements + e;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct MovementUpdate *u) { updates[numUpdates++] = *u; }

/* InitMovementSystem initializes the movement system. */
void InitMovementSystem() {}

/* UpdateMovementSystem updates all movements that have been created. */
void UpdateMovementSystem() { numUpdates = 0; }

/* AddMovement adds a movement component to the entity e. */
void AddMovement(Entity e, float speed) {
	struct entityToMovement *item;

	if (getMovement(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToMovement));
	item->movement = movements + numMovements;
	item->e = e;

	movements[numMovements].speed = speed;

	HASH_ADD_INT(entitiesToMovements, e, item);
	numMovements++;
}

/* GetMovementUpdates returns the movement updates this frame. */
struct MovementUpdate *GetMovementUpdates(int *num) {
	*num = numUpdates;
	return updates;
}

/* MovementMoveTo moves the entity e to (x, y, z). */
void MovementMoveTo(Entity e, float x, float y, float z) {
	struct Movement *m;
	m = getMovement(e);

	if (m == NULL)
		return;

	m->dest.x = x;
	m->dest.y = y;
	m->dest.z = z;
}
