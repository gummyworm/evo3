#include "base.h"
#include "draw.h"
#include "entities.h"
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

static float dt;

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
		float wx, wy, wz;
		int x, y;
		InputGetMouse(&c->selection.x, &c->selection.y);
		WindowToGui(c->selection.x, c->selection.y, &x, &y);
		c->selection.x = x;
		c->selection.y = y;
		c->selection.w = 0;
		c->selection.h = 0;
		c->selection.selecting = true;

		ScreenToWorld(E_PLAYER, c->selection.x, c->selection.y, &wx,
		              &wy, &wz);
		dinfof("%f %f %f", wx, wy, wz);
	} else if (action == GLFW_RELEASE) {
		if (c->selection.selecting) {
			int i;
			int x, y;
			vec2 center, dim;
			center[0] = c->selection.x + dim[0] / 2.f;
			center[1] = c->selection.y + dim[1] / 2.f;
			GuiToWindow(c->selection.w, c->selection.h, &x, &y);
			dim[0] = x;
			dim[1] = y;
			GuiToWindow(c->selection.x, c->selection.y, &x, &y);
			center[0] = x + dim[0] / 2.f;
			center[1] = y + dim[1] / 2.f;

			for (i = 0; i < c->numSelected; ++i) {
				SetSpriteColor(c->selected[i], 0, 0, 0, 0);
			}
			c->numSelected =
			    GetIn2DBounds(c->selected, MAX_SELECTION, center,
			                  dim, SelectUnit);
			for (i = 0; i < c->numSelected; ++i) {
				SetSpriteColor(c->selected[i], 0, 1, 0, 0);
			}
		}
		c->selection.selecting = false;
	}
}

/* mousemove is the mouse motion callback. */
static void mousemove(Entity e, double x, double y) {
	struct Commander *c;
	if ((c = getCommander(e)) == NULL)
		return;
	if (c->selection.selecting) {
		int gx, gy;
		WindowToGui(x, y, &gx, &gy);
		c->selection.w = gx - c->selection.x;
		c->selection.h = gy - c->selection.y;
	}
}

/* scroll is the mouse scroll callback. */
static void scroll(Entity e, double dx, double dy) {
	UNUSED(dx);
	struct Commander *c;
	if ((c = getCommander(e)) == NULL)
		return;
	{
		float x, y, z;
		GetEye(e, &x, &y, &z);
		z += dy * c->zoomSpeed * dt;
		if (z < c->zoomLimits[0])
			z = c->zoomLimits[0];
		if (z > c->zoomLimits[1])
			z = c->zoomLimits[1];
		SetEye(e, x, y, z);
	}
}

/* key is the commander key callback. */
static void key(Entity e, int key, int scancode, int action, int mods) {
	UNUSED(scancode);
	UNUSED(action);
	UNUSED(mods);
	struct Commander *c;
	float x, y, z;

	if (!Enabled(e))
		return;
	if (!GetEye(e, &x, &y, &z))
		return;
	if ((c = getCommander(e)) == NULL)
		return;

	if (key == GLFW_KEY_LEFT)
		x += c->panSpeed[0] * dt;
	if (key == GLFW_KEY_RIGHT)
		x -= c->panSpeed[1] * dt;
	if (key == GLFW_KEY_UP)
		y += c->panSpeed[1] * dt;
	if (key == GLFW_KEY_DOWN)
		y -= c->panSpeed[1] * dt;

	SetEye(e, x, y, z);
}

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
	commanders[numCommanders].panSpeed[0] = 15.f;
	commanders[numCommanders].panSpeed[1] = 15.f;
	commanders[numCommanders].zoomSpeed = 5.f;
	commanders[numCommanders].zoomLimits[0] = -20.f;
	commanders[numCommanders].zoomLimits[1] = 20.f;
	commanders[numCommanders].selection.selecting = false;
	InputRegisterMouseButtonEvent(e, INPUT_LAYER_DEFAULT, lmouse, NULL);
	InputRegisterMouseEvent(e, INPUT_LAYER_DEFAULT, mousemove);
	InputRegisterMouseScrollEvent(e, INPUT_LAYER_DEFAULT, scroll);
	InputRegisterKeyEvent(e, INPUT_LAYER_DEFAULT, key);
	numCommanders++;
}

/* Initcommandersystem initializes the transform system. */
void InitCommanderSystem() {}

/* Updatecommandersystem updates all commanders that have been created. */
void UpdateCommanderSystem() {
	int i;
	mat4x4 proj;
	static float lastUpdate;

	dt = GetTime() - lastUpdate;
	lastUpdate = GetTime();

	GuiProjection(proj);
	for (i = 0; i < numCommanders; ++i) {
		struct Commander *c = commanders + i;
		if (c->selection.selecting) {
			Rect(proj, c->selection.x, c->selection.y,
			     c->selection.w, c->selection.h, 0x00ff0080);
		}
	}
}
