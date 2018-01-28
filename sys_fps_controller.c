#include "sys_fps_controller.h"
#include "base.h"
#include "debug.h"
#include "draw.h"
#include "input.h"
#include "sys_camera.h"
#include "sys_gui.h"
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
C->aiming = false;
C->moving = false;
C->turning = false;
C->keyCodes.forward = GLFW_KEY_UP;
C->keyCodes.backward = GLFW_KEY_DOWN;
C->keyCodes.left = GLFW_KEY_COMMA;
C->keyCodes.right = GLFW_KEY_PERIOD;
C->keyCodes.turnL = GLFW_KEY_LEFT;
C->keyCodes.turnR = GLFW_KEY_RIGHT;
C->keyCodes.lookU = GLFW_KEY_UP;
C->keyCodes.lookD = GLFW_KEY_DOWN;
C->keyCodes.jump = GLFW_KEY_SPACE;
C->keyCodes.aim = GLFW_KEY_R;

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
	float cosa, sina;

	if ((f = GetFPSController(e)) == NULL)
		return;
	if (!Enabled(f->e))
		return;

	cosa = cos(f->angle);
	sina = sin(f->angle);

	if (key == f->keyCodes.aim) {
		f->aiming = action == GLFW_PRESS || action == GLFW_REPEAT;
	}

	if (key == f->keyCodes.forward) {
		f->dpos[0] = -sina;
		f->dpos[1] = 0;
		f->dpos[2] = -cosa;
		f->moving = !f->aiming &&
		            (action == GLFW_PRESS || action == GLFW_REPEAT);
	}
	if (key == f->keyCodes.backward) {
		f->dpos[0] = sina;
		f->dpos[1] = 0;
		f->dpos[2] = cosa;
		f->moving = !f->aiming &&
		            (action == GLFW_PRESS || action == GLFW_REPEAT);
	}
	if (key == f->keyCodes.left) {
		f->dpos[0] = -cosa;
		f->dpos[1] = 0;
		f->dpos[2] = sina;
		f->moving = action == GLFW_PRESS || action == GLFW_REPEAT;
	}
	if (key == f->keyCodes.right) {
		f->dpos[0] = cosa;
		f->dpos[1] = 0;
		f->dpos[2] = -sina;
		f->moving = action == GLFW_PRESS || action == GLFW_REPEAT;
	}
	if (key == f->keyCodes.turnL) {
		f->drot[1] = -1.f;
		f->turning = action == GLFW_PRESS || action == GLFW_REPEAT;
	}
	if (key == f->keyCodes.turnR) {
		f->drot[1] = 1.f;
		f->turning = action == GLFW_PRESS || action == GLFW_REPEAT;
	}
	if (key == f->keyCodes.lookU) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			f->drot[0] = -1.f;
		else
			f->drot[0] = 0.f;
	}
	if (key == f->keyCodes.lookD) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
			f->drot[0] = 1.f;
		else
			f->drot[0] = 0.f;
	}
	if (key == f->keyCodes.jump && f->canJump) {
		f->jumpTime = 0.f;
		f->canJump = false;
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

		if (f->moving) {
			vec3 dposn;
			float tscale = GetTimeDelta() * f->speed;
			vec3_norm(dposn, f->dpos);
			vec3_scale(f->dpos, dposn, tscale);
			TransformMove(f->e, f->dpos[0], f->dpos[1], f->dpos[2]);
		}

		if (f->turning) {
			f->angle -= f->drot[1] * GetTimeDelta();
			vec3 dir = {-sin(f->angle), 0, -cos(f->angle)};
			vec3_scale(dir, dir, GetTimeDelta() * f->turnSpeed);
			SetViewDir(f->e, dir[0], dir[1], dir[2]);
		}

		if (f->aiming) {
			mat4x4 mvp;

			f->pitch -= f->drot[1] * GetTimeDelta();
			vec3 dir = {-sin(f->angle), sin(f->pitch),
			            -cos(f->angle)};
			vec3_scale(dir, dir, GetTimeDelta() * f->turnSpeed);
			SetViewDir(f->e, dir[0], dir[1], dir[2]);

			GuiProjection(mvp);
			TexRect(mvp, TEXTURE_PROGRAM,
			        GUI_WIDTH / 2 - FPS_AIMER_W / 2,
			        GUI_HEIGHT / 2 - FPS_AIMER_H / 2, FPS_AIMER_W,
			        FPS_AIMER_H, 0, 0, 1, 1, f->aimTex);
		}
	}
}
