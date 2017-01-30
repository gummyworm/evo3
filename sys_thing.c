#include <stdio.h>
#include <string.h>

#include "third-party/include/uthash.h"

#include "debug.h"
#include "sys_actor.h"
#include "thing.h"

const char *ACTION_TAKE = "TAKE";

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

/* RemoveThing removes the thing attached to e from the Thing system. */
void RemoveThing(Entity e) {
	struct entityToThing *c;

	if (entitiesToThings == NULL)
		return;

	HASH_FIND_INT(entitiesToThings, &e, c);
	if (c != NULL) {
		struct Thing *sys = c->thing;
		int sz = (things + numThings) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToThings, c);
		free(c);
	}
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

/* AddActionHandler adds a handler to respond to action. */
void AddActionHandler(Entity e, const char *action, Action handler) {
	struct ActionHandler *h;
	struct Thing *t;
	if ((t = getThing(e)) == NULL)
		return;

	h = malloc(sizeof(struct ActionHandler));
	h->action = action;
	h->handler = handler;
	HASH_ADD_STR(t->actions, action, h);
}

/* HandleAction delivers the given action to the thing attached to e and
 * responds accordingly (if a handler for such action exists). */
bool HandleAction(Entity e, Entity actor, char *action, char *outBuff) {
	struct ActionHandler *h;
	struct Thing *t;
	if ((t = getThing(e)) == NULL)
		return false;

	HASH_FIND_STR(t->actions, action, h);
	if (h != NULL)
		return h->handler(e, actor, outBuff);
	return false;
}

/* handleTake provides default behavior for the TAKE action. */
static bool handleTake(Entity self, Entity actor, char *out) {
	struct Thing *t;

	if ((t = getThing(self)) == NULL)
		return false;

	sprintf(out, "You take the %s", t->name);
	InventoryAdd(actor, self);
	t->owner = actor;
	return true;
}

/* AddItem creates a new Thing that responds to being TAKEn. */
void AddItem(Entity e, const char *name, const char *desc) {
	AddThing(e, name, desc);
	AddActionHandler(e, ACTION_TAKE, handleTake);
}
