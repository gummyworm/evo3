#ifndef SYS_FPS_CONTROLLER_H
#define SYS_FPS_CONTROLLER_H

#include <stdint.h>
#include "entity.h"

enum { MAX_FPS_CONTROLLERS = 16 };

struct FPSController {
	Entity e;
	float speed;
	struct {
		int forward;
		int backward;
	} keyCodes;
};

struct FPSControllerUpdate {};

void InitFPSControllerSystem();
void AddFPSController(Entity);

#endif
