#include <stdio.h>
#include <string.h>

#include "third-party/include/uthash.h"

#include "debug.h"
#include "sys_actor.h"
#include "thing.h"

const char *ACTION_TAKE = "TAKE";
const char *ACTION_DROP = "DROP";
const char *ACTION_LOOK = "LOOK";
const char *ACTION_OPEN = "OPEN";
const char *ACTION_CLOSE = "CLOSE";
const char *ACTION_EMPTY = "EMPTY";

struct entityToThing {
	Entity e;
	struct Thing *thing;
	UT_hash_handle hh;
};

static struct entityToThing *entitiesToThings;
static struct Thing things[MAX_THINGS];
static int numThings;

/* getThing returns the thing attached to entity e (if there is one). */
struct Thing *getThing(Entity e) {
	struct entityToThing *t;

	if (entitiesToThings == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToThings, &e, t);
	if (t == NULL)
		return NULL;

	return t->thing;
}

/* InitThingSystem initializes the thing system. */
void InitThingSystem() {}

/* UpdateThingSystem updates all things that have been created. */
void UpdateThingSystem() {}

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
		numThings--;
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
bool HandleAction(Entity e, Entity prop, Entity actor, char *action,
                  char *outBuff) {
	struct ActionHandler *h;
	struct Thing *t;
	if ((t = getThing(e)) == NULL)
		return false;

	HASH_FIND_STR(t->actions, action, h);
	if (h != NULL)
		return h->handler(e, prop, actor, outBuff);
	return false;
}

/* GetType returns the thing type of the Thing attached to the entity e. */
int GetType(Entity e) {
	struct Thing *t;
	if ((t = getThing(e)) == NULL)
		return THING_NOTHING;

	return t->type;
}

/* SetType sets the type of the thing attached to the entity e to type. */
void SetType(Entity e, int type) {
	struct Thing *t;
	if ((t = getThing(e)) == NULL)
		return;

	t->type = type;
}
