#ifndef SYS_FPS_CONTROLLER_H
#define SYS_FPS_CONTROLLER_H

#include "entity.h"
#include <stdint.h>

struct FPSController {
	Entity e;
	float speed;
	float turnSpeed;

	float jumpSpeed;
	float jumpTime;
	float timeToJumpApex;
	bool canJump;

	float angle;
	struct {
		int forward;
		int backward;
		int left;
		int right;
		int turnL;
		int turnR;
		int jump;
	} keyCodes;
};

struct FPSControllerUpdate {};
void InitFPSControllerSystem();
void UpdateFPSControllerSystem();
void AddFPSController(Entity, float);
void RemoveFPSController(Entity);

#endif
