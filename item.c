#include "systems.h"
#include "thing.h"

extern struct Thing *getThing(Entity e);

/* handleDrop provides default behavior for the DROP action. */
static bool handleDrop(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(prop);
	struct Thing *t;
	vec3 pos;
	float dx, dy, dz;

	if ((t = getThing(self)) == NULL)
		return false;
	if (t->owner != actor)
		return false;

	if (TransformGetPos(actor, pos) && GetViewDir(actor, &dx, &dy, &dz))
		TransformSet(self, pos[0] + (dx * 2), -1, pos[2] + (dz * 2));

	t->owner = 0;
	EnableEntity(self);
	InventoryRemove(actor, self);

	if (out)
		sprintf(out, "You drop the %s", t->name);
	return true;
}

/* handleTake provides default behavior for the TAKE action. */
static bool handleTake(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(prop);
	struct Thing *t;

	if ((t = getThing(self)) == NULL)
		return false;

	if (out)
		sprintf(out, "You take the %s", t->name);

	InventoryAdd(actor, self);
	t->owner = actor;
	DisableEntity(self);
	return true;
}

/* AddItem creates a new Thing that responds to being TAKEn. */
void AddItem(Entity e, const char *name, const char *desc) {
	AddThing(e, name, desc);
	AddActionHandler(e, ACTION_TAKE, handleTake);
	AddActionHandler(e, ACTION_DROP, handleDrop);
	SetType(e, THING_ITEM);
}
