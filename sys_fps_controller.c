#include "sys_fps_controller.h"
#include "base.h"
#include "debug.h"
#include "input.h"
#include "sys_camera.h"
#include "sys_time.h"
#include "sys_transform.h"
#include "third-party/include/uthash.h"

static void key(Entity e, int key, int scancode, int action, int mods);

SYSDEF(FPSController, float speed)
C->speed = speed;
C->turnSpeed = 3.0f;
C->jumpSpeed = 0.2f;
C->timeToJumpApex = 0.4f;
C->canJump = true;
C->keyCodes.forward = GLFW_KEY_UP;
C->keyCodes.backward = GLFW_KEY_DOWN;
C->keyCodes.left = GLFW_KEY_COMMA;
C->keyCodes.right = GLFW_KEY_PERIOD;
C->keyCodes.turnL = GLFW_KEY_LEFT;
C->keyCodes.turnR = GLFW_KEY_RIGHT;
C->keyCodes.jump = GLFW_KEY_SPACE;

InputRegisterKeyEvent(e, INPUT_LAYER_DEFAULT, key);
}

/* init initializes the FPSController system. */
void init() {}

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

	if ((f = GetFPSController(e)) == NULL)
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
	}
	if (key == f->keyCodes.backward) {
		dpos[0] = sina;
		dpos[1] = 0;
		dpos[2] = cosa;
		translate = true;
	}
	if (key == f->keyCodes.left) {
		dpos[0] = -cosa;
		dpos[1] = 0;
		dpos[2] = sina;
		translate = true;
	}
	if (key == f->keyCodes.right) {
		dpos[0] = cosa;
		dpos[1] = 0;
		dpos[2] = -sina;
		translate = true;
	}
	if (key == f->keyCodes.turnL) {
		f->angle += f->turnSpeed * GetTimeDelta();
		rotate = true;
	}
	if (key == f->keyCodes.turnR) {
		f->angle -= f->turnSpeed * GetTimeDelta();
		rotate = true;
	}
	if (key == f->keyCodes.jump && f->canJump) {
		f->jumpTime = 0.f;
		f->canJump = false;
	}

	if (translate) {
		vec3 dposn;
		float tscale = GetTimeDelta() * f->speed;
		vec3_norm(dposn, dpos);
		vec3_scale(dpos, dposn, tscale);
		TransformMove(f->e, dpos[0], dpos[1], dpos[2]);
	}

	if (rotate) {
		vec3 dir = {-sin(f->angle), 0, -cos(f->angle)};
		SetViewDir(f->e, dir[0], dir[1], dir[2]);
	}
}

/* update updates all fpsControllers that have been created.
 */
void update() {
	int i;
	for (i = 0; i < numComponents; ++i) {
		struct FPSController *f = components + i;
		if (f->jumpTime < f->timeToJumpApex) {
			TransformMove(f->e, 0.0f, f->jumpSpeed, 0.f);
		} else {
			vec3 pos;
			TransformMove(f->e, 0.0f, -f->jumpSpeed, 0.f);
			if (TransformGetPos(f->e, pos) &&
			    pos[1] == TRANSFORM_MIN_Y)
				f->canJump = true;
		}
		f->jumpTime += GetTimeDelta();
	}
}
