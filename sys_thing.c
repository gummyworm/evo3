#include "third-party/include/uthash.h"

#include "debug.h"
#include "thing.h"

struct entityToThing {
	Entity e;
	struct Thing *thing;
	UT_hash_handle hh;
};

static struct entityToThing *entitiesToThings;
static struct Thing things[MAX_THINGS];
static int numThings;

static int numUpdates;
static struct ThingUpdate updates[MAX_THINGS];

/* getThing returns the thing attached to entity e (if there is one). */
static struct Thing *getThing(Entity e) {
	struct entityToThing *t;

	if (entitiesToThings == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToThings, &e, t);
	if (t == NULL)
		return NULL;

	return t->thing;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct ThingUpdate *u) { updates[numUpdates++] = *u; }

/* InitThingSystem initializes the thing system. */
void InitThingSystem() {}

/* UpdateThingSystem updates all things that have been created. */
void UpdateThingSystem() { numUpdates = 0; }

/* AddThing adds a thing component to the entity e. */
void AddThing(Entity e, const char *name, const char *desc) {
	struct entityToThing *item;

	if (getThing(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToThing));
	item->thing = things + numThings;
	item->e = e;

	things[numThings].e = e;
	things[numThings].owner = 0;
	things[numThings].name = name;
	things[numThings].desc = desc;
	things[numThings].actions = NULL;

	HASH_ADD_INT(entitiesToThings, e, item);
	numThings++;
}

/* GetThing returns the entity that contains the Thing named name. */
Entity GetThing(const char *name) {
	int i, l;

	if (name == NULL)
		return -1;

	l = strlen(name);
	for (i = 0; i < numThings; ++i) {
		if (strncmp(things[i].name, name, l) == 0)
			return things[i].e;
	}
	return -1;
}

/* GetThings returns all the Things and sets num to the number of them. */
struct Thing *GetThings(int *num) {
	*num = numThings;
	return things;
}

/* GetThingName returns the name of the thing attached to e. */
const char *GetThingName(Entity e) {
	struct Thing *t;
	if ((t = getThing(e)) == NULL)
		return NULL;
	return t->name;
}

/* GetThingName returns the description of the thing attached to e. */
const char *GetThingDescription(Entity e) {
	struct Thing *t;
	if ((t = getThing(e)) == NULL)
		return NULL;
	return t->desc;
}
