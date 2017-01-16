#ifndef SYS_FPS_CONTROLLER_H
#define SYS_FPS_CONTROLLER_H

#include "entity.h"
#include <stdint.h>

enum { MAX_FPS_CONTROLLERS = 16 };

struct FPSController {
	struct {
		int forward;
		int backward;
	} keyCodes;
};

struct FPSControllerUpdate {};

void AddFPSController(Entity);

#endif
