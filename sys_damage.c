#include "sys_damage.h"
#include "sys_transform.h"
#include "third-party/include/uthash.h"

struct entityToDamage {
	Entity e;
	struct Damage *damage;
	UT_hash_handle hh;
};

static struct entityToDamage *entitiesToDamages;
static struct Damage damages[MAX_DAMAGES];
static int numDamages;

int numDamageUpdates;
static struct DamageUpdate updates[MAX_PROJECTILES];

/* getDamage returns the damage attached to entity e (if there is one).
 */
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
		if (u->into != NULL)
			UnitHarm(u->into, damages[i].value;
	}
}

/* AddDamage adds a damage component to the entity e. */
void AddDamage(Entity e, vec3 dir, float vel) {
	struct entityToDamage *item;

	if (getDamage(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToDamage));
	item->damage = damages + numDamages;
	item->e = e;

	damages[numDamages].e = e;
	damages[numDamages].vel = vel;
	damages[numDamages].dir[0] = dir[0];
	damages[numDamages].dir[1] = dir[1];
	damages[numDamages].dir[2] = dir[2];

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

/* GetDamageUpdate returns any damage updates for the entity e. */
struct DamageUpdate *GetDamageUpdate(Entity e) {
	int i;

	for (i = 0; i < numDamageUpdates; ++i) {
		if (updates[i].e == e)
			return updates + i;
	}

	return NULL;
}

/* GetDamageUpdates returns the damage updates this frame. */
struct DamageUpdate *GetDamageUpdates(int *num) {
	*num = numDamageUpdates;
	return updates;
}
