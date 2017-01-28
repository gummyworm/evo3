#ifndef THING_H
#define THING_H

#include "entity.h"

enum { MAX_THINGS = 8192,
       MAX_THINGS_PER_ROOM = 128,
};

/* Thing is a struct that represents an inanimate object's attributes. */
struct Thing {
	Entity e;
	const char *name;
	const char *brief;
	const char *desc;
	int weight;
};

/* ThingUpdate defines an update message that can be polled by
 * interested systems. */
struct ThingUpdate {
	Entity e;
};

void InitThingSystem();
void UpdateThingSystem();
void AddThing(Entity, const char *);
Entity GetThing(const char *);

#endif
