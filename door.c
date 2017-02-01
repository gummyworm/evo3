#include "systems.h"
#include "thing.h"

extern struct Thing *getThing(Entity e);

/* handleOpen provides default behavior for the OPEN action for doors. */
static bool handleOpen(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(actor);
	UNUSED(prop);
	struct Thing *t;

	if ((t = getThing(self)) == NULL)
		return false;

	if (t->properties.door.open) {
		sprintf(out, "The %s is already open", t->name);
		return true;
	}

	t->properties.door.open = true;
	sprintf(out, "The %s is now open", t->name);
	return true;
}

/* handleClose provides default behavior for the CLOSE action for doors */
static bool handleClose(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(actor);
	UNUSED(prop);
	struct Thing *t;
	int *p;

	if ((t = getThing(self)) == NULL)
		return false;

	if (!t->properties.door.open) {
		sprintf(out, "The %s is already closed", t->name);
		return true;
	}

	for (p = (int *)utarray_front(t->contents); p != NULL;
	     p = (int *)utarray_next(t->contents, p)) {
		DisableEntity(*p);
	}

	t->properties.door.open = false;
	sprintf(out, "The %s is now closed", t->name);
	return true;
}

/* AddDoor creates a new Thing of the Door type. */
void AddDoor(Entity e, const char *name, const char *desc) {
	AddThing(e, name, desc);
	AddActionHandler(e, ACTION_OPEN, handleOpen);
	AddActionHandler(e, ACTION_OPEN, handleClose);
}
