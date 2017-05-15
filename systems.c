#include "systems.h"

struct DisabledSet {
	Entity e;
	UT_hash_handle hh;
};
static struct DisabledSet *disabled;

/* RemoveEntity attempts to remove e from all systems. */
void RemoveEntity(Entity e) {
	RemoveRender(e);
	RemoveCamera(e);
	RemoveConsole(e);
	RemoveTransform(e);
	RemoveRoom(e);
	RemoveThing(e);
	RemovePartyMember(e);
	RemoveMovement(e);
	RemoveMesh(e);
	RemoveDamage(e);
	RemoveCollider(e);
	RemoveGunController(e);
	RemoveFPSController(e);
	RemoveGun(e);
	RemoveChild(e);
	RemoveRoom(e);
	RemoveUnit(e);
}

/* DisableEntity notifies systems that e is not to be updated. */
void DisableEntity(Entity e) {
	struct DisabledSet *item;

	HASH_FIND_INT(disabled, &e, item);
	if (item == NULL) {
		item = malloc(sizeof(struct DisabledSet));
		item->e = e;
		HASH_ADD_INT(disabled, e, item);
	}
}

/* EnableEntity enables an entity that has been disabled. */
void EnableEntity(Entity e) {
	struct DisabledSet *item;

	HASH_FIND_INT(disabled, &e, item);
	if (item != NULL) {
		HASH_DEL(disabled, item);
		free(item);
	}
}

/* Enabled returns true if the entity e should be updated. */
bool Enabled(Entity e) {
	struct DisabledSet *item = NULL;
	HASH_FIND_INT(disabled, &e, item);
	return item == NULL;
}
