#ifndef SYS_UNIT_H
#define SYS_UNIT_H

#include "base.h"
#include "entity.h"
#include "thing.h"

enum { UNIT_MAX_ITEMS_CARRIED = 36,
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
	Entity e;
	const char *name;
	int hp;
	Prefab deathPrefab;

	struct Thing items[UNIT_MAX_ITEMS_CARRIED];
};

void InitUnitSystem();
void UpdateUnitSystem();
void AddUnit(Entity, const char *, int);
void RemoveUnit(Entity);
void UnitHarm(Entity, int damage);
bool SelectUnit(Entity);
struct Unit *GetUnits(int *);

#endif
