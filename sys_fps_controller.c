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

static struct FPSController *getFPSController(Entity e);

/* key is the input callback to handle key events. */
static void key(Entity e, int key, int scancode, int action, int mods) {
	UNUSED(scancode);
	UNUSED(action);
	UNUSED(mods);

	struct FPSController *f;
	vec3 dpos;
	float angle;
	float cosa, sina;
	bool rotate, translate;

	if ((f = getFPSController(e)) == NULL)
		return;
	if (!Enabled(f->e))
		return;

	angle = f->angle;
	cosa = cos(angle);
	sina = sin(angle);

	rotate = false;
	translate = false;
	if (key == f->keyCodes.forward) {
		dpos[0] = -sina;
		dpos[1] = 0;
		dpos[2] = -cosa;
		translate = true;
	} else if (key == f->keyCodes.backward) {
		dpos[0] = sina;
		dpos[1] = 0;
		dpos[2] = cosa;
		translate = true;
	} else if (key == f->keyCodes.left) {
		dpos[0] = -cosa;
		dpos[1] = 0;
		dpos[2] = sina;
		translate = true;
	} else if (key == f->keyCodes.right) {
		dpos[0] = cosa;
		dpos[1] = 0;
		dpos[2] = -sina;
		translate = true;
	} else if (key == f->keyCodes.turnL) {
		f->angle += f->turnSpeed * dt;
		rotate = true;
	} else if (key == f->keyCodes.turnR) {
		f->angle -= f->turnSpeed * dt;
		rotate = true;
	}

	if (translate) {
		vec3 dposn;
		float tscale = dt * f->speed;
		vec3_norm(dposn, dpos);
		vec3_scale(dpos, dposn, tscale);
		TransformMove(f->e, dpos[0], dpos[1], dpos[2]);
	}

	if (rotate) {
		vec3 dir = {-sin(f->angle), 0, -cos(f->angle)};
		SetViewDir(f->e, dir[0], dir[1], dir[2]);
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

/* InitFPSControllerSystem initializes the fpsController system. */
void InitFPSControllerSystem() {
	if (entitiesToFPSControllers != NULL) {
		struct entityToFPSController *f, *tmp;
		HASH_ITER(hh, entitiesToFPSControllers, f, tmp) {
			HASH_DEL(entitiesToFPSControllers,
			         f); /* delete; users advances to next */
			free(f);     /* optional- if you want to free  */
		}
	}
	numFPSControllers = 0;
}

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
	fpsControllers[numFPSControllers].turnSpeed = 3.0f;
	fpsControllers[numFPSControllers].keyCodes.forward = GLFW_KEY_UP;
	fpsControllers[numFPSControllers].keyCodes.backward = GLFW_KEY_DOWN;
	fpsControllers[numFPSControllers].keyCodes.left = GLFW_KEY_COMMA;
	fpsControllers[numFPSControllers].keyCodes.right = GLFW_KEY_PERIOD;
	fpsControllers[numFPSControllers].keyCodes.turnL = GLFW_KEY_LEFT;
	fpsControllers[numFPSControllers].keyCodes.turnR = GLFW_KEY_RIGHT;

	InputRegisterKeyEvent(e, INPUT_LAYER_DEFAULT, key);
	numFPSControllers++;
}

/* RemoveFPSController removes the fpsController attached to e from the
 * FPSController system. */
void RemoveFPSController(Entity e) {
	struct entityToFPSController *c;

	if (entitiesToFPSControllers == NULL)
		return;

	HASH_FIND_INT(entitiesToFPSControllers, &e, c);
	if (c != NULL) {
		struct FPSController *sys = c->fpsController;
		int sz = (fpsControllers + numFPSControllers) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToFPSControllers, c);
		free(c);
	}
}

/* GetFPSControllerUpdates returns the fpsController updates this frame. */
struct FPSControllerUpdate *GetFPSControllerUpdates(int *num) {
	*num = numUpdates;
	return updates;
}
