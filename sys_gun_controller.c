#include "sys_gun_controller.h"
#include "base.h"
#include "debug.h"
#include "input.h"
#include "sys_gun.h"
#include "third-party/include/uthash.h"

struct entityToGunController {
	Entity e;
	struct GunController *gunController;
	UT_hash_handle hh;
};

static struct entityToGunController *entitiesToGunControllers;
static struct GunController gunControllers[MAX_FPS_CONTROLLERS];
static int numGunControllers;

static struct GunController *getGunController(Entity e);

/* key is the input callback to handle key events. */
static void key(Entity e, int key, int scancode, int action, int mods) {
	UNUSED(scancode);
	UNUSED(action);
	UNUSED(mods);

	struct GunController *g;

	if ((g = getGunController(e)) == NULL)
		return;
	if (!Enabled(g->e))
		return;

	if (key == g->keyCodes.fire) {
		GunFire(e);
	}
}

/* getGunController returns the fpsController attached to entity e (if there is
 * one). */
static struct GunController *getGunController(Entity e) {
	struct entityToGunController *g;

	if (entitiesToGunControllers == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToGunControllers, &e, g);
	return g->gunController;
}

/* InitGunControllerSystem initializes the fpsController system. */
void InitGunControllerSystem() {
	if (entitiesToGunControllers != NULL) {
		struct entityToGunController *g, *tmp;
		HASH_ITER(hh, entitiesToGunControllers, g, tmp) {
			HASH_DEL(entitiesToGunControllers, g);
			free(g);
		}
	}
	numGunControllers = 0;
}

/* UpdateGunControllerSystem updates all gunControllers that have been created.
 */
void UpdateGunControllerSystem() {}

/* AddGunController adds a fpsController component to the entity e. */
void AddGunController(Entity e) {
	struct entityToGunController *item;

	if (getGunController(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToGunController));
	item->gunController = gunControllers + numGunControllers;
	item->e = e;
	HASH_ADD_INT(entitiesToGunControllers, e, item);

	gunControllers[numGunControllers].e = e;
	gunControllers[numGunControllers].keyCodes.fire = GLFW_KEY_SPACE;

	InputRegisterKeyEvent(e, INPUT_LAYER_DEFAULT, key);
	numGunControllers++;
}

/* RemoveGunController removes the fpsController attached to e from the
 * GunController system. */
void RemoveGunController(Entity e) {
	struct entityToGunController *c;

	if (entitiesToGunControllers == NULL)
		return;

	HASH_FIND_INT(entitiesToGunControllers, &e, c);
	if (c != NULL) {
		struct GunController *sys = c->gunController;
		int sz = (gunControllers + numGunControllers) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToGunControllers, c);
		free(c);
		numGunControllers--;
	}
}
