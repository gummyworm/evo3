#include "draw.h"
#include "entities.h"
#include "system.h"
#include "systems.h"

extern struct Thing *getThing(Entity e);

/* drawContents renders a widget to display the contents of the box e. */
static void drawContents(Entity e, mat4x4 proj, int w, int h) {
	struct Thing *t;
	Entity *p;
	float x, y, itemW, itemH;

	if ((t = getThing(e)) == NULL)
		return;

	x = 0.f;
	y = 0.f;
	itemW = 32.f;
	itemH = 32.f;

	for (p = (int *)utarray_front(t->contents); p != NULL;
	     p = (int *)utarray_next(t->contents, p)) {
		GLuint tex;
		if ((tex = GetSpriteTexture(*p)) != 0) {
			if (y > h) {
				y = 0;
				x += itemW;
			}
			TexRect(proj, getTextureProgram(), x, y, itemW, itemH,
			        0, 0, 1, 1, tex);
			y += itemH;
		}
	}
}

/* lmouse is the left mouse button callback for containers. */
static void lmouse(Entity e, int action) {
	struct Thing *t;
	Entity *p;
	int i, j, itemW, itemH;
	int x, y, w, h;

	if (action != GLFW_PRESS)
		return;
	if ((t = getThing(e)) == NULL)
		return;

	i = 0;
	j = 0;
	itemW = 32.f;
	itemH = 32.f;

	{
		double ax, ay;
		int gx, gy;
		InputGetMouse(&ax, &ay);
		GetRelWidgetPos(e, ax, ay, &gx, &gy);
		ScreenToGui(gx, gy, &x, &y);
		GetWidgetDim(e, &w, &h);
	}

	for (p = (int *)utarray_front(t->contents); p != NULL;
	     p = (int *)utarray_next(t->contents, p)) {
		GLuint tex;
		char out[256];
		if ((tex = GetSpriteTexture(*p)) != 0) {
			if (x > i && x < (i + itemW) && y > j &&
			    y < (j + itemH)) {
				HandleAction(*p, 0, E_PLAYER,
				             (char *)ACTION_TAKE, out);
				utarray_erase(t->contents,
				              utarray_eltidx(t->contents, p),
				              1);
				return;
			}
			if (y > h) {
				j = 0;
				i += itemW;
			}
			i += itemW;
		}
	}
}

/* mouseHandler is called when the OPEN dialog receives a mouse event. */
static void mouseHandler(Entity self, int action) {
	struct Thing *t;

	if (action != GLFW_PRESS)
		return;
	if ((t = getThing(self)) == NULL)
		return;
}

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
	t->properties.box.opener = actor;
	sprintf(out, "The %s is now open", t->name);

	AddRenderWindow(self, 40, 20, drawContents, lmouse, NULL);

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
	RemoveWidget(t->e);
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

	SetType(e, THING_BOX);
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
