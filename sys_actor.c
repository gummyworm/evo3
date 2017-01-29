#include "sys_actor.h"
#include "third-party/include/uthash.h"

struct entityToActor {
	Entity e;
	struct Actor *actor;
	UT_hash_handle hh;
};

static struct entityToActor *entitiesToActors;
static struct Actor actors[MAX_ACTORS];
static int numActors;

int numActorUpdates;
static struct ActorUpdate updates[MAX_ACTORS];

/* getActor returns the actor attached to entity e (if there is one). */
static struct Actor *getActor(Entity e) {
	struct entityToActor *t;

	if (entitiesToActors == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToActors, &e, t);
	if (t == NULL)
		return NULL;

	return t->actor;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct ActorUpdate *u) {
	updates[numActorUpdates++] = *u;
}

/* InitActorSystem initializes the actor system. */
void InitActorSystem() {}

/* UpdateActorSystem updates all actors that have been created. */
void UpdateActorSystem() {}

/* AddActor adds a actor component to the entity e. */
void AddActor(Entity e, const char *name) {
	struct entityToActor *item;

	if (getActor(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToActor));
	item->actor = actors + numActors;
	item->e = e;

	actors[numActors].e = e;
	actors[numActors].name = name;
	actors[numActors].numItems = 0;

	HASH_ADD_INT(entitiesToActors, e, item);
	numActors++;
}

/* InventoryAdd gives taker the thing attached to the entity e. */
void InventoryAdd(Entity taker, Entity e) {
	struct Actor *a;

	if ((a = getActor(taker)) == NULL)
		return;

	a->inventory[a->numItems++] = e;
}
