#include "sys_damage.h"
#include "sys_collision.h"
#include "sys_damage.h"
#include "sys_unit.h"
#include "third-party/include/uthash.h"

struct entityToDamage {
	Entity e;
	struct Damage *damage;
	UT_hash_handle hh;
};

static struct entityToDamage *entitiesToDamages;
static struct Damage damages[MAX_DAMAGES];
static int numDamages;

/* getDamage returns the damage attached to entity e (if there is one). */
static struct Damage *getDamage(Entity e) {
	struct entityToDamage *t;

	if (entitiesToDamages == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToDamages, &e, t);
	if (t == NULL)
		return NULL;

	return t->damage;
}

/* InitDamageSystem initializes the damage system. */
void InitDamageSystem() {}

/* UpdateDamageSystem updates all damages that have been created. */
void UpdateDamageSystem() {
	int i;
	for (i = 0; i < numDamages; ++i) {
		struct ColliderUpdate *u = GetColliderUpdate(damages[i].e);
		if (u->into != 0)
			UnitHarm(u->into, damages[i].value);
	}
}

/* AddDamage adds a damage component to the entity e. */
void AddDamage(Entity e, float value) {
	struct entityToDamage *item;

	if (getDamage(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToDamage));
	item->damage = damages + numDamages;
	item->e = e;

	damages[numDamages].e = e;
	damages[numDamages].value = value;

	HASH_ADD_INT(entitiesToDamages, e, item);
	numDamages++;
}

/* RemoveDamage removes the damage attached to e from the Damage
 * system. */
void RemoveDamage(Entity e) {
	struct entityToDamage *c;

	if (entitiesToDamages == NULL)
		return;

	HASH_FIND_INT(entitiesToDamages, &e, c);
	if (c != NULL) {
		struct Damage *sys = c->damage;
		int sz = (damages + numDamages) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToDamages, c);
		free(c);
	}
}
