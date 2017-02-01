#include "system.h"
#include "thing.h"

extern struct Thing *getThing(Entity e);

/* handleOpenBox provides default behavior for the OPEN action for
 * containers.
 */
static bool handleOpenBox(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(actor);
	UNUSED(prop);
	struct Thing *t;

	if ((t = getThing(self)) == NULL)
		return false;

	if (t->properties.box.open) {
		sprintf(out, "The %s is already open", t->name);
		return true;
	}

	t->properties.box.open = true;
	sprintf(out, "The %s is now open", t->name);
	return true;
}

/* handleCloseBox provides default behavior for the CLOSE action for
 * containers.
 */
static bool handleCloseBox(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(actor);
	UNUSED(prop);
	struct Thing *t;
	int *p;

	if ((t = getThing(self)) == NULL)
		return false;

	if (!t->properties.box.open) {
		sprintf(out, "The %s is already closed", t->name);
		return true;
	}

	for (p = (int *)utarray_front(t->contents); p != NULL;
	     p = (int *)utarray_next(t->contents, p)) {
		DisableEntity(*p);
	}

	t->properties.box.open = false;
	sprintf(out, "The %s is now closed", t->name);
	return true;
}

/* handleEmptyBox provides default behavior for the EMPTY action for
 * containers.
 */
static bool handleEmptyBox(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(actor);
	UNUSED(prop);
	struct Thing *t;
	int *p;

	if ((t = getThing(self)) == NULL)
		return false;

	if (!t->properties.box.open) {
		sprintf(out, "It is closed");
		return true;
	}

	t->properties.box.open = true;
	sprintf(out, "You empty the %s", t->name);

	for (p = (int *)utarray_front(t->contents); p != NULL;
	     p = (int *)utarray_next(t->contents, p)) {
		struct Thing *c;
		EnableEntity(*p);
		c = getThing(*p);
		if (c != NULL)
			c->owner = -1;
		utarray_erase(t->contents, utarray_eltidx(t->contents, p), 1);
	}
	return true;
}

/* handleLookBox provides default behavior for the LOOK action for
 * containers.
 */
static bool handleLookBox(Entity self, Entity prop, Entity actor, char *out) {
	UNUSED(actor);
	UNUSED(prop);
	struct Thing *t;
	int *p;

	if ((t = getThing(self)) == NULL)
		return false;

	if (!t->properties.box.open) {
		sprintf(out, "%s\nIt is closed", t->desc);
		return true;
	}

	t->properties.box.open = true;
	sprintf(out, "The %s contains the following:\n", t->name);

	for (p = (int *)utarray_front(t->contents); p != NULL;
	     p = (int *)utarray_next(t->contents, p)) {
		const char *name = GetThingName(*p);
		if (name != NULL) {
			out += strlen(out);
			sprintf(out, "%s\n", name);
		}
	}
	return true;
}

/* AddContainer creates a new Thing that may hold other things. */
void AddContainer(Entity e, const char *name, const char *desc) {
	struct Thing *t;
	AddItem(e, name, desc);
	if ((t = getThing(e)) == NULL)
		return;
	AddActionHandler(e, ACTION_LOOK, handleLookBox);
	AddActionHandler(e, ACTION_OPEN, handleOpenBox);
	AddActionHandler(e, ACTION_CLOSE, handleCloseBox);
	AddActionHandler(e, ACTION_EMPTY, handleEmptyBox);

	utarray_new(t->contents, &ut_int_icd);
}

/* AddToContainer adds item to e's contents. */
void AddToContainer(Entity e, Entity item) {
	struct Thing *box, *t;

	if ((t = getThing(e)) == NULL)
		return;
	if ((box = getThing(e)) == NULL)
		return;

	if (box->contents != NULL)
		utarray_push_back(box->contents, &item);
	t->owner = e;
	DisableEntity(item);
}

/* GetFromContainer returns true if item was successfully removed from e. */
bool GetFromContainer(Entity e, Entity item) {
	struct Thing *t;
	int *p;

	if ((t = getThing(e)) == NULL)
		return false;

	if (t->contents == NULL)
		return false;

	for (p = (int *)utarray_front(t->contents); p != NULL;
	     p = (int *)utarray_next(t->contents, p)) {
		if (*p == item) {
			utarray_erase(t->contents,
			              utarray_eltidx(t->contents, p), 1);
			return true;
		}
	}

	return false;
}
