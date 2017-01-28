#ifndef THING_H
#define THING_H

#include "entity.h"

enum { MAX_THINGS = 8192 };

/* Thing is a struct that represents an inanimate object's attributes. */
struct Thing {
	Entity e;
	const char *name;
	int weight;
};

/* ThingUpdate defines an update message that can be polled by interested
 * systems. */
struct ThingUpdate {
	Entity e;
};

void InitThingSystem();
void UpdateThingSystem();
void AddThing(Entity, const char *);
Entity GetThing(const char *);

#endif
