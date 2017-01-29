#ifndef THING_H
#define THING_H

#include "third-party/include/uthash.h"

#include "entity.h"
#include <stdbool.h>

enum { MAX_THINGS = 8192,
       MAX_THINGS_PER_ROOM = 128,
};

/* Thing is a struct that represents an inanimate object's attributes. */
struct Thing {
	Entity e;
	Entity owner;

	const char *name;
	const char *brief;
	const char *desc;
	int weight;

	bool takeable;

	struct ActionHandler *actions;
};

/* ActionHandler defines callbacks to be executed when a given action
 * is performed on a Thing. */
struct ActionHandler {
	const char *action;
	void (*handle)(struct Thing *self, char *out);
	UT_hash_handle hh;
};

/* ThingUpdate defines an update message that can be polled by
 * interested systems. */
struct ThingUpdate {
	Entity e;
};

void InitThingSystem();
void UpdateThingSystem();
void AddThing(Entity, const char *, const char *);
Entity GetThing(const char *);
struct Thing *GetThings(int *num);

const char *GetThingName(Entity e);
const char *GetThingDescription(Entity e);

#endif
