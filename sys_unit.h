#ifndef SYS_UNIT_H
#define SYS_UNIT_H

#include "entity.h"
#include "thing.h"

enum { MAX_UNITS = 8192,
       UNIT_MAX_ITEMS_CARRIED = 36,
};

/* UnitUpdate is a struct that describes actions a unit has taken this frame. */
struct UnitUpdate {
	Entity e;
	int damageTaken;
	struct Thing *taken;
	struct Thing *dropped;
};

/* Unit is a struct that stores the common attributes of units. */
struct Unit {
	const char *name;
	int hp;

	struct Thing items[UNIT_MAX_ITEMS_CARRIED];
};

void InitUnitSystem();
void NewUnit(Entity, const char *, int);
void UnitHarm(Entity, int damage);
bool SelectUnit(Entity);

#endif
