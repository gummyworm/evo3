#include "sys_fps_controller.h"
#include "base.h"
#include "debug.h"
#include "input.h"
#include "sys_camera.h"
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
		vec3 dpos, drot, dir;
		float cosx, cosy, sinx, siny;
		float pitchlim;
		bool rotate, translate;

		f = fpsControllers + i;

		if (!GetViewDir(f->e, &dir[0], &dir[1], &dir[2]))
			return;

		cosx = cos(dir[0]);
		cosy = cos(dir[0]);
		sinx = sin(dir[1]);
		siny = sin(dir[1]);
		pitchlim = cosx;

		rotate = false;
		translate = false;
		if (key == f->keyCodes.forward) {
			dpos[0] = siny * pitchlim;
			dpos[1] = -sinx;
			dpos[2] = -cosy * pitchlim;
			translate = true;
		} else if (key == f->keyCodes.backward) {
			dpos[0] = -siny * pitchlim;
			dpos[1] = sinx;
			dpos[2] = cosy * pitchlim;
			translate = true;
		} else if (key == f->keyCodes.left) {
			dpos[0] = -cosy;
			dpos[1] = -siny;
			dpos[2] = 0;
			translate = true;
		} else if (key == f->keyCodes.right) {
			dpos[0] = cosy;
			dpos[1] = siny;
			dpos[2] = 0;
			translate = true;
		} else if (key == f->keyCodes.turnL) {
			drot[0] = 0;
			drot[1] = 1;
			drot[2] = 0;
			rotate = true;
		} else if (key == f->keyCodes.turnR) {
			drot[0] = 0;
			drot[1] = -1;
			drot[2] = 0;
			rotate = true;
		}

		if (translate) {
			float tscale;
			tscale = dt * f->speed;

			vec3 dposn;
			vec3_norm(dposn, dpos);
			vec3_scale(dpos, dposn, tscale);
			dinfof("%f %f %f", dpos[0], dpos[1], dpos[2]);
			TransformMove(f->e, dpos[0], dpos[1], dpos[2]);
		}

		if (rotate) {
			float rscale;
			vec3 scaled, adjusted, set;
			rscale = dt * f->turnSpeed;

			vec3_scale(scaled, drot, rscale);
			vec3_add(adjusted, dir, scaled);
			vec3_norm(set, adjusted);

			SetViewDir(f->e, set[0], set[1], set[2]);
		}
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
	fpsControllers[numFPSControllers].turnSpeed = 10.0f;
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
