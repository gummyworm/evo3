#include "sys_gun.h"
#include "sys_time.h"
#include "sys_transform.h"
#include "third-party/include/uthash.h"

struct entityToGun {
	Entity e;
	struct Gun *gun;
	UT_hash_handle hh;
};

static struct entityToGun *entitiesToGuns;
static struct Gun guns[MAX_GUNS];
static int numGuns;

/* getGun returns the gun attached to entity e (if there is one). */
static struct Gun *getGun(Entity e) {
	struct entityToGun *t;

	if (entitiesToGuns == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToGuns, &e, t);
	if (t == NULL)
		return NULL;

	return t->gun;
}

/* InitGunSystem initializes the gun system. */
void InitGunSystem() {}

/* UpdateGunSystem updates all guns that have been created. */
void UpdateGunSystem() {
	int i;
	for (i = 0; i < numGuns; ++i) {
		guns[i].update.cooldown -= GetTimeDelta();
	}
}

/* AddGun adds a gun component to the entity e. */
void AddGun(Entity e, Prefab projectile, float rate) {
	struct entityToGun *item;

	if (getGun(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToGun));
	item->gun = guns + numGuns;
	item->e = e;

	guns[numGuns].e = e;
	guns[numGuns].projectile = projectile;
	guns[numGuns].rate = rate;
	guns[numGuns].update.cooldown = 0.f;

	HASH_ADD_INT(entitiesToGuns, e, item);
	numGuns++;
}

/* RemoveGun removes the gun attached to e from the Gun system. */
void RemoveGun(Entity e) {
	struct entityToGun *c;

	if (entitiesToGuns == NULL)
		return;

	HASH_FIND_INT(entitiesToGuns, &e, c);
	if (c != NULL) {
		struct Gun *sys = c->gun;
		int sz = (guns + numGuns) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToGuns, c);
		free(c);
		numGuns--;
	}
}

/* GunFire spawns the projectile of the gun attached to e if the gun
 * attached to e is off cooldown. */
void GunFire(Entity e) {
	struct Gun *g;

	if ((g = getGun(e)) == NULL)
		return;
	if (g->update.cooldown > 0)
		return;

	{
		vec3 pos;
		Entity bullet = NewEntity();
		g->projectile(bullet);
		TransformGetPos(e, pos);
		TransformSet(bullet, pos[0], pos[1], pos[2]);
	}
	g->update.cooldown = g->rate;
}
