#include "sys_fps_controller.h"
#include "input.h"
#include "sys_transform.h"
#include "third-party/include/uthash.h"

struct entityToFPSController {
	Entity e;
	struct FPSController *fpsController;
	UT_hash_handle hh;
};

static struct entityToFPSController *entitiesToFPSControllers;
struct FPSController fpsControllers[MAX_FPS_CONTROLLERS];
static int numFPSControllers;

static int numUpdates;
static struct FPSControllerUpdate updates[MAX_FPS_CONTROLLERS];

/* key is the input callback to handle key events. */
static void key(int key, int scancode, int action, int mods) {
	int i;

	for (i = 0; i < numFPSControllers; ++i) {
		struct FPSController *f;
		f = fpsControllers + i;
		TransformMove(f->e, f->speed, 0, 0);
	}
}

/* getFPSController returns the fpsController attached to entity e (if there is
 * one). */
static struct FPSController *getFPSController(Entity e) {
	struct entityToFPSController *f;

	if (entitiesToFPSControllers == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToFPSControllers, &e, f);
	return f->fpsController;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct FPSControllerUpdate *u) {
	updates[numUpdates++] = *u;
}

/* InitFPSControllerSystem initializes the fpsController system. */
void InitFPSControllerSystem() {}

/* UpdateFPSControllerSystem updates all fpsControllers that have been created.
 */
void UpdateFPSControllerSystem() { numUpdates = 0; }

/* AddFPSController adds a fpsController component to the entity e. */
void AddFPSController(Entity e) {
	struct entityToFPSController *item;

	if (getFPSController(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToFPSController));
	item->fpsController = fpsControllers + numFPSControllers;
	item->e = e;
	HASH_ADD_INT(entitiesToFPSControllers, e, item);

	fpsControllers[numFPSControllers].e = e;
	fpsControllers[numFPSControllers].speed = 1.0f;
	fpsControllers[numFPSControllers].keyCodes.forward = 'w';
	fpsControllers[numFPSControllers].keyCodes.backward = 's';

	InputRegisterKeyEvent(INPUT_LAYER_DEFAULT, key);

	numFPSControllers++;
}

/* GetFPSControllerUpdates returns the fpsController updates this frame. */
struct FPSControllerUpdate *GetFPSControllerUpdates(int *num) {
	*num = numUpdates;
	return updates;
}
