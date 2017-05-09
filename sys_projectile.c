#include "sys_projectile.h"
#include "sys_transform.h"
#include "third-party/include/uthash.h"

struct entityToProjectile {
	Entity e;
	struct Projectile *projectile;
	UT_hash_handle hh;
};

static struct entityToProjectile *entitiesToProjectiles;
static struct Projectile projectiles[MAX_PROJECTILES];
static int numProjectiles;

int numProjectileUpdates;
static struct ProjectileUpdate updates[MAX_PROJECTILES];

/* getProjectile returns the projectile attached to entity e (if there is one).
 */
static struct Projectile *getProjectile(Entity e) {
	struct entityToProjectile *t;

	if (entitiesToProjectiles == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToProjectiles, &e, t);
	if (t == NULL)
		return NULL;

	return t->projectile;
}

/* InitProjectileSystem initializes the projectile system. */
void InitProjectileSystem() {}

/* UpdateProjectileSystem updates all projectiles that have been created. */
void UpdateProjectileSystem() {
	int i;
	for (i = 0; i < numProjectiles; ++i) {
		vec3 res, pos;
		float vel = projectiles[i].vel;
		if (!GetPos(projectiles[i].e, &pos[0], &pos[1], &pos[2]))
			continue;
		vec3_scale(res, projectiles[i].dir, vel);
		TransformSet(projectiles[i].e, res[0], res[1], res[2]);
	}
}

/* AddProjectile adds a projectile component to the entity e. */
void AddProjectile(Entity e, vec3 dir, float vel) {
	struct entityToProjectile *item;

	if (getProjectile(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToProjectile));
	item->projectile = projectiles + numProjectiles;
	item->e = e;

	projectiles[numProjectiles].e = e;
	projectiles[numProjectiles].vel = vel;
	projectiles[numProjectiles].dir[0] = dir[0];
	projectiles[numProjectiles].dir[1] = dir[1];
	projectiles[numProjectiles].dir[2] = dir[2];

	HASH_ADD_INT(entitiesToProjectiles, e, item);
	numProjectiles++;
}

/* RemoveProjectile removes the projectile attached to e from the Projectile
 * system. */
void RemoveProjectile(Entity e) {
	struct entityToProjectile *c;

	if (entitiesToProjectiles == NULL)
		return;

	HASH_FIND_INT(entitiesToProjectiles, &e, c);
	if (c != NULL) {
		struct Projectile *sys = c->projectile;
		int sz = (projectiles + numProjectiles) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToProjectiles, c);
		free(c);
	}
}

/* GetProjectileUpdate returns any projectile updates for the entity e. */
struct ProjectileUpdate *GetProjectileUpdate(Entity e) {
	int i;

	for (i = 0; i < numProjectileUpdates; ++i) {
		if (updates[i].e == e)
			return updates + i;
	}

	return NULL;
}

/* GetProjectileUpdates returns the projectile updates this frame. */
struct ProjectileUpdate *GetProjectileUpdates(int *num) {
	*num = numProjectileUpdates;
	return updates;
}
