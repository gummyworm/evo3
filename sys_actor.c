#include "systems.h"
#include "third-party/include/uthash.h"

struct entityToActor {
	Entity e;
	struct Actor *actor;
	UT_hash_handle hh;
};

static struct entityToActor *entitiesToActors;
static struct Actor actors[MAX_ACTORS];
static int numActors;

static float dt;

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

/* InitActorSystem initializes the actor system. */
void InitActorSystem() {}

/* UpdateActorSystem updates all actors that have been created. */
void UpdateActorSystem() {
	static float lastUpdate;
	int i;

	dt = GetTime() - lastUpdate;
	lastUpdate = GetTime();
	for (i = 0; i < numActors; ++i) {
		struct Actor *a = actors + i;
		a->remarkTmr -= dt;
		if (a->remarkTmr <= 0.) {
			ActorRemark(a->e);
			a->remarkTmr = a->remarkInterval;
		}
	}
}

/* AddActor adds a actor component to the entity e. */
void AddActor(Entity e, const char *name, const char *desc) {
	struct entityToActor *item;

	if (getActor(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToActor));

	item->actor = actors + numActors;
	item->e = e;

	actors[numActors].e = e;
	actors[numActors].name = name;
	actors[numActors].desc = desc;
	actors[numActors].numItems = 0;
	actors[numActors].remarkTmr = 10.;
	actors[numActors].remarkInterval = 10.;

	HASH_ADD_INT(entitiesToActors, e, item);
	numActors++;
}

/* RemoveActor removes the actor attached to e from the Actor system. */
void RemoveActor(Entity e) {
	struct entityToActor *c;

	if (entitiesToActors == NULL)
		return;

	HASH_FIND_INT(entitiesToActors, &e, c);
	if (c != NULL) {
		struct Actor *sys = c->actor;
		int sz = (actors + numActors) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToActors, c);
		free(c);
	}
}

/* InventoryAdd gives taker the thing attached to the entity e. */
void InventoryAdd(Entity taker, Entity e) {
	struct Actor *a;

	if ((a = getActor(taker)) == NULL)
		return;

	a->inventory[a->numItems++] = e;
}

/* InventoryRemove removes e from actor's inventory. */
void InventoryRemove(Entity actor, Entity e) {
	int i;
	struct Actor *a;

	if ((a = getActor(actor)) == NULL)
		return;
	for (i = 0; i < a->numItems; ++i) {
		if (a->inventory[i] == e) {
			memmove(a->inventory + i, a->inventory + i + 1,
			        a->numItems - i);
			a->numItems--;
			break;
		}
	}
}

/* GetInventory returns the number of items in the invetory attached to e and
 * sets inv to the contents.*/
int GetInventory(Entity e, Entity **inv) {
	struct Actor *a;

	if ((a = getActor(e)) == NULL)
		return 0;

	*inv = a->inventory;
	return a->numItems;
}

/* GetActorName returns the name of the actor attached to e. */
const char *GetActorName(Entity e) {
	struct Actor *t;
	if ((t = getActor(e)) == NULL)
		return NULL;
	return t->name;
}

/* GetActorName returns the description of the actor attached to e. */
const char *GetActorDescription(Entity e) {
	struct Actor *t;
	if ((t = getActor(e)) == NULL)
		return NULL;
	return t->desc;
}

/* ActorRemark generates a remark for the actor attached to e based upon its
 * mood. */
void ActorRemark(Entity e) {
	struct Actor *t;
	if ((t = getActor(e)) == NULL)
		return;
}

/* GetHP gets the current HP of the actor attached to e. */
int GetHP(Entity e) {
	struct Actor *a;

	a = getActor(e);
	if (a == NULL)
		return -1;
	return a->currentStats.hp;
}

/* GetMaxHP gets the maximum HP of the actor attached to e. */
int GetMaxHP(Entity e) {
	struct Actor *a;

	a = getActor(e);
	if (a == NULL)
		return -1;
	return a->maximumStats.hp;
}
