#include "base.h"
#include "draw.h"
#include "systems.h"
#include "third-party/include/uthash.h"

struct entityToCommander {
	Entity e;
	struct Commander *commander;
	UT_hash_handle hh;
};

static struct entityToCommander *entitiesToCommanders;
static struct Commander commanders[MAX_COMMANDERS];
static int numCommanders;

static int numUpdates;
static struct CommanderUpdate updates[MAX_COMMANDERS];

/* getCommander returns the commander attached to entity e (if there is one). */
static struct Commander *getCommander(Entity e) {
	struct entityToCommander *c;

	if (entitiesToCommanders == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToCommanders, &e, c);
	if (c == NULL)
		return NULL;

	return c->commander;
}

/* lmouse is the left mouse button callback. */
static void lmouse(Entity e, int action) {
	struct Commander *c;
	if ((c = getCommander(e)) == NULL)
		return;
	if (action == GLFW_PRESS) {
		int x, y;
		InputGetMouse(&c->selection.x, &c->selection.y);
		ScreenToGui(c->selection.x, c->selection.y, &x, &y);
		c->selection.x = x;
		c->selection.y = y;
		c->selection.w = 0;
		c->selection.h = 0;
		c->selection.selecting = true;
	} else if (action == GLFW_RELEASE) {
		if (c->selection.selecting) {
		}
		c->selection.selecting = false;
	}
}

static void mousemove(Entity e, double x, double y) {
	struct Commander *c;
	if ((c = getCommander(e)) == NULL)
		return;
	if (c->selection.selecting) {
		int gx, gy;
		ScreenToGui(x, y, &gx, &gy);
		c->selection.w = gx - c->selection.x;
		c->selection.h = gy - c->selection.y;
	}
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct CommanderUpdate *u) { updates[numUpdates++] = *u; }

/* AddCommander adds a commander component to the entity e. */
void AddCommander(Entity e) {
	struct entityToCommander *item;

	if (getCommander(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToCommander));
	item->commander = commanders + numCommanders;
	item->e = e;

	HASH_ADD_INT(entitiesToCommanders, e, item);
	commanders[numCommanders].e = e;
	commanders[numCommanders].selection.selecting = false;
	InputRegisterMouseButtonEvent(e, INPUT_LAYER_DEFAULT, lmouse, NULL);
	InputRegisterMouseEvent(e, INPUT_LAYER_DEFAULT, mousemove);
	numCommanders++;
}

/* Initcommandersystem initializes the transform system. */
void InitCommanderSystem() {}

/* Updatecommandersystem updates all commanders that have been created. */
void UpdateCommanderSystem() {
	int i;
	mat4x4 proj;

	GuiProjection(proj);
	for (i = 0; i < numCommanders; ++i) {
		struct Commander *c = commanders + i;
		if (c->selection.selecting) {
			Rect(proj, c->selection.x, c->selection.y,
			     c->selection.w, c->selection.h, 0x00ff0080);
		}
	}
}
