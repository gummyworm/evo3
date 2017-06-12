#ifndef SYS_FPS_CONTROLLER_H
#define SYS_FPS_CONTROLLER_H

#include "base.h"

#include "entity.h"
#include "third-party/include/linmath.h"
#include <stdint.h>

enum { FPS_AIMER_W = 16,
       FPS_AIMER_H = 16,
};

struct FPSController {
	Entity e;
	float speed;
	float turnSpeed;

	float jumpSpeed;
	float jumpTime;
	float timeToJumpApex;

	vec3 dpos;
	float drot;

	bool canJump;
	bool aiming;
	bool moving;
	bool turning;

	float angle;
	struct {
		int forward;
		int backward;
		int left;
		int right;
		int turnL;
		int turnR;
		int jump;
		int aim;
	} keyCodes;

	GLuint aimTex;
};

struct FPSControllerUpdate {};
void InitFPSControllerSystem();
void UpdateFPSControllerSystem();
void AddFPSController(Entity, float);
void RemoveFPSController(Entity);

#endif
