#include "sys_commander.h"
#include "third-party/include/uthash.h"

struct entityToCommander {
	Entity e;
	struct Commander *commander;
	UT_hash_handle hh;
};

static struct entityToCommander *entitiesToCommanders;
struct Commander commanders[MAX_COMMANDERS];
static int numCommanders;

static int numUpdates;
static struct CommanderUpdate updates[MAX_COMMANDERS];

/* getCommander returns the commander attached to entity e (if there is one). */
static struct Commander *getCommander(Entity e) {
	struct entityToCommander *u;
	HASH_FIND_INT(entitiesToCommanders, &e, u);
	e = u->e;
	return commanders + e;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct CommanderUpdate *u) { updates[numUpdates++] = *u; }

/* NewCommander adds a commander component to the entity e. */
void NewCommander(Entity e) {
	struct entityToCommander *item;

	if (getCommander(e) != NULL) return;

	item = malloc(sizeof(struct entityToCommander));
	item->commander = commanders + numCommanders;
	item->e = e;

	HASH_ADD_INT(entitiesToCommanders, e, item);
	numCommanders++;
}

/* Initcommandersystem initializes the transform system. */
void InitCommanderSystem() {}

/* Updatecommandersystem updates all commanders that have been created. */
void UpdateCommanderSystem() { numUpdates = 0; }
