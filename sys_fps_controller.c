#include "sys_fps_controller.h"
#include "base.h"
#include "input.h"
#include "sys_time.h"
#include "sys_transform.h"
#include "third-party/include/uthash.h"

struct entityToFPSController {
	Entity e;
	struct FPSController *fpsController;
	UT_hash_handle hh;
};

static struct entityToFPSController *entitiesToFPSControllers;
static struct FPSController fpsControllers[MAX_FPS_CONTROLLERS];
static int numFPSControllers;

static float dt;
static int numUpdates;
static struct FPSControllerUpdate updates[MAX_FPS_CONTROLLERS];

/* key is the input callback to handle key events. */
static void key(int key, int scancode, int action, int mods) {
	UNUSED(scancode);
	UNUSED(action);
	UNUSED(mods);
	int i;

	for (i = 0; i < numFPSControllers; ++i) {
		struct FPSController *f;
		struct {
			float x, y, z;
		} dpos = {};
		struct {
			float x, y, z;
		} drot = {};
		vec3 pos, rot;
		float xa, ya;

		f = fpsControllers + i;

		GetPos(f->e, &pos[0], &pos[1], &pos[2]);
		GetRot(f->e, &rot[0], &rot[1], &rot[2]);
		xa = cos(rot[1]) * dt;
		ya = sin(rot[1]) * dt;

		if (key == f->keyCodes.forward) {
			dpos.x = f->speed * ya;
			dpos.z = f->speed * xa;
		} else if (key == f->keyCodes.backward) {
			dpos.x = -f->speed * ya;
			dpos.z = -f->speed * xa;
		} else if (key == f->keyCodes.left) {
			dpos.x = -f->speed * dt;
		} else if (key == f->keyCodes.right) {
			dpos.x = f->speed * dt;
		} else if (key == f->keyCodes.turnL) {
			drot.y = f->turnSpeed * dt;
		} else if (key == f->keyCodes.turnR) {
			drot.y = -f->turnSpeed * dt;
		}

		TransformMove(f->e, dpos.x, dpos.y, dpos.z);
		TransformRotate(f->e, drot.x, drot.y, drot.z);
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
void UpdateFPSControllerSystem() {
	static float lastUpdate = 0.0f;
	dt = GetTime() - lastUpdate;
	lastUpdate = GetTime();
	numUpdates = 0;
}

/* AddFPSController adds a fpsController component to the entity e. */
void AddFPSController(Entity e, float speed) {
	struct entityToFPSController *item;

	if (getFPSController(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToFPSController));
	item->fpsController = fpsControllers + numFPSControllers;
	item->e = e;
	HASH_ADD_INT(entitiesToFPSControllers, e, item);

	fpsControllers[numFPSControllers].e = e;
	fpsControllers[numFPSControllers].speed = speed;
	fpsControllers[numFPSControllers].turnSpeed = 1.0f;
	fpsControllers[numFPSControllers].keyCodes.forward = GLFW_KEY_W;
	fpsControllers[numFPSControllers].keyCodes.backward = GLFW_KEY_S;
	fpsControllers[numFPSControllers].keyCodes.left = GLFW_KEY_A;
	fpsControllers[numFPSControllers].keyCodes.right = GLFW_KEY_D;
	fpsControllers[numFPSControllers].keyCodes.turnL = GLFW_KEY_LEFT;
	fpsControllers[numFPSControllers].keyCodes.turnR = GLFW_KEY_RIGHT;

	InputRegisterKeyEvent(INPUT_LAYER_DEFAULT, key);

	numFPSControllers++;
}

/* GetFPSControllerUpdates returns the fpsController updates this frame. */
struct FPSControllerUpdate *GetFPSControllerUpdates(int *num) {
	*num = numUpdates;
	return updates;
}
