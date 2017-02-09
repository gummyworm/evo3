#include "sys_unit.h"
#include "third-party/include/uthash.h"

struct entityToUnit {
	Entity e;
	struct Unit *unit;
	UT_hash_handle hh;
};

static struct entityToUnit *entitiesToUnits;
static struct Unit units[MAX_UNITS];
static int numUnits;

static int numUpdates;
static struct UnitUpdate updates[MAX_UNITS];

/* getUnit returns the unit attached to entity e (if there is one). */
static struct Unit *getUnit(Entity e) {
	struct entityToUnit *u;

	if (entitiesToUnits == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToUnits, &e, u);
	if (u == NULL)
		return NULL;

	return u->unit;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct UnitUpdate *u) { updates[numUpdates++] = *u; }

/* NewUnit adds a unit component to the entity e. */
void NewUnit(Entity e, const char *name, int hp) {
	struct entityToUnit *item;

	if (getUnit(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToUnit));
	item->unit = units + numUnits;
	item->e = e;

	units[numUnits].name = name;
	units[numUnits].hp = hp;

	HASH_ADD_INT(entitiesToUnits, e, item);
	numUnits++;
}

/* UnitHarm inflicts damage points of health to the unit attached to e. */
void UnitHarm(Entity e, int damage) {
	struct Unit *u;

	u = getUnit(e);
	if (u == NULL)
		return;

	{
		struct UnitUpdate u = {.e = e, .damageTaken = damage};
		addUpdate(&u);
	}

	u->hp -= damage;
}

/* Initunitsystem initializes the transform system. */
void InitUnitSystem() {}

/* Updateunitsystem updates all units that have been created. */
void UpdateUnitSystem() { numUpdates = 0; }

/* SelectUnit returns true if the entity e can be selected. */
bool SelectUnit(Entity e) { return getUnit(e) != NULL; }
