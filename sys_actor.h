#ifndef ACTOR_H
#define ACTOR_H

#include "entity.h"
#include <stdbool.h>

enum { MAX_ACTORS = 8192,
       MAX_INVENTORY_SIZE = 1024,
};

/* Actor is a struct that represents an animate character */
struct Actor {
	Entity e;
	const char *name;
	const char *brief;
	const char *desc;

	Entity inventory[MAX_INVENTORY_SIZE];
	int numItems;
};

/* ActorUpdate defines updates that may be polled by interested systems. */
struct ActorUpdate {
	Entity e;
};

void InitActorSystem();
void UpdateActorSystem();
void AddActor(Entity, const char *);
void InventoryAdd(Entity, Entity);

#endif
