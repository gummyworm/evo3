#ifndef ACTOR_H
#define ACTOR_H

#include "base.h"

#include "entity.h"
#include <stdbool.h>

enum { MAX_ACTORS = 8192,
       MAX_INVENTORY_SIZE = 1024,
};

struct ActorStats {
	int hp;
};

/* Actor is a struct that represents an animate character */
struct Actor {
	Entity e;
	const char *name;
	const char *brief;
	const char *desc;

	struct ActorStats currentStats;
	struct ActorStats baseStats;
	struct ActorStats maximumStats;

	Entity inventory[MAX_INVENTORY_SIZE];
	int numItems;
};

/* ActorUpdate defines updates that may be polled by interested systems. */
struct ActorUpdate {
	Entity e;
};

void InitActorSystem();
void UpdateActorSystem();
void AddActor(Entity, const char *, const char *);
void RemoveActor(Entity);
void InventoryRemove(Entity, Entity);
void InventoryAdd(Entity, Entity);
int GetInventory(Entity, Entity **);
const char *GetActorName(Entity);
const char *GetActorDescription(Entity);

#endif
