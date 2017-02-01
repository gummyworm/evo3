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

/* handleDrop provides default behavior for the DROP action. */
static bool handleDrop(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(prop);
	struct Thing *t;

	if ((t = getThing(self)) == NULL)
		return false;
	if (t->owner != actor)
		return false;

	sprintf(out, "You drop the %s", t->name);
	InventoryRemove(actor, self);
	t->owner = actor;
	EnableEntity(self);
	return true;
}

/* handleTake provides default behavior for the TAKE action. */
static bool handleTake(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(prop);
	struct Thing *t;

	if ((t = getThing(self)) == NULL)
		return false;

	sprintf(out, "You take the %s", t->name);
	InventoryAdd(actor, self);
	t->owner = actor;
	DisableEntity(self);
	return true;
}

/* handleOpenBox provides default behavior for the OPEN action for containers.
 */
static bool handleOpenBox(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(actor);
	UNUSED(prop);
	struct Thing *t;

	if ((t = getThing(self)) == NULL)
		return false;

	if (t->properties.container.open) {
		sprintf(out, "The %s is already open", t->name);
		return true;
	}

	t->properties.container.open = true;
	sprintf(out, "The %s is now open", t->name);
	return true;
}

/* handleCloseBox provides default behavior for the CLOSE action for containers.
 */
static bool handleCloseBox(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(actor);
	UNUSED(prop);
	struct Thing *t;
	int *p;

	if ((t = getThing(self)) == NULL)
		return false;

	if (!t->properties.container.open) {
		sprintf(out, "The %s is already closed", t->name);
		return true;
	}

	for (p = (int *)utarray_front(t->contents); p != NULL;
	     p = (int *)utarray_next(t->contents, p)) {
		DisableEntity(*p);
	}

	t->properties.container.open = false;
	sprintf(out, "The %s is now closed", t->name);
	return true;
}

/* handleEmptyBox provides default behavior for the EMPTY action for containers.
 */
static bool handleEmptyBox(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(actor);
	UNUSED(prop);
	struct Thing *t;
	int *p;

	if ((t = getThing(self)) == NULL)
		return false;

	if (!t->properties.container.open) {
		sprintf(out, "It is closed");
		return true;
	}

	t->properties.container.open = true;
	sprintf(out, "You empty the %s", t->name);

	for (p = (int *)utarray_front(t->contents); p != NULL;
	     p = (int *)utarray_next(t->contents, p)) {
		struct Thing *c;
		EnableEntity(*p);
		c = getThing(*p);
		if (c != NULL)
			c->owner = -1;
		utarray_erase(t->contents, utarray_eltidx(t->contents, p), 1);
	}
	return true;
}

/* handleLookBox provides default behavior for the LOOK action for containers.
 */
static bool handleLookBox(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(actor);
	UNUSED(prop);
	struct Thing *t;
	int *p;

	if ((t = getThing(self)) == NULL)
		return false;

	if (!t->properties.container.open) {
		sprintf(out, "%s\nIt is closed", t->desc);
		return true;
	}

	t->properties.container.open = true;
	sprintf(out, "The %s contains the following:\n", t->name);

	for (p = (int *)utarray_front(t->contents); p != NULL;
	     p = (int *)utarray_next(t->contents, p)) {
		const char *name = GetThingName(*p);
		if (name != NULL) {
			out += strlen(out);
			sprintf(out, "%s\n", name);
		}
	}
	return true;
}

/* AddItem creates a new Thing that responds to being TAKEn. */
void AddItem(Entity e, const char *name, const char *desc) {
	AddThing(e, name, desc);
	AddActionHandler(e, ACTION_TAKE, handleTake);
	AddActionHandler(e, ACTION_DROP, handleDrop);
}

/* AddContainer creates a new Thing that may hold other things. */
void AddContainer(Entity e, const char *name, const char *desc) {
	struct Thing *t;
	AddItem(e, name, desc);
	if ((t = getThing(e)) == NULL)
		return;
	AddActionHandler(e, ACTION_LOOK, handleLookBox);
	AddActionHandler(e, ACTION_OPEN, handleOpenBox);
	AddActionHandler(e, ACTION_CLOSE, handleCloseBox);
	AddActionHandler(e, ACTION_EMPTY, handleEmptyBox);

	utarray_new(t->contents, &ut_int_icd);
}

/* AddToContainer adds item to e's contents. */
void AddToContainer(Entity e, Entity item) {
	struct Thing *box, *t;

	if ((t = getThing(e)) == NULL)
		return;
	if ((box = getThing(e)) == NULL)
		return;

	if (box->contents != NULL)
		utarray_push_back(box->contents, &item);
	t->owner = e;
	DisableEntity(item);
}

/* GetFromContainer returns true if item was successfully removed from e. */
bool GetFromContainer(Entity e, Entity item) {
	struct Thing *t;
	int *p;

	if ((t = getThing(e)) == NULL)
		return false;

	if (t->contents == NULL)
		return false;

	for (p = (int *)utarray_front(t->contents); p != NULL;
	     p = (int *)utarray_next(t->contents, p)) {
		if (*p == item) {
			utarray_erase(t->contents,
			              utarray_eltidx(t->contents, p), 1);
			return true;
		}
	}

	return false;
}
