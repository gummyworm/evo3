#ifndef SYS_FPS_CONTROLLER_H
#define SYS_FPS_CONTROLLER_H

#include "entity.h"
#include <stdint.h>

enum { MAX_FPS_CONTROLLERS = 16 };

struct FPSController {
	Entity e;
	float speed;
	float turnSpeed;

	float angle;
	struct {
		int forward;
		int backward;
		int left;
		int right;
		int turnL;
		int turnR;
	} keyCodes;
};

struct FPSControllerUpdate {};

void InitFPSControllerSystem();
void UpdateFPSControllerSystem();
void AddFPSController(Entity, float);
void RemoveFPSController(Entity);

#endif
