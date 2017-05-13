#ifndef SYS_GUN_CONTROLLER_H
#define SYS_GUN_CONTROLLER_H

#include "entity.h"
#include <stdint.h>

struct GunController {
	Entity e;
	struct {
		int fire;
	} keyCodes;
};

void InitGunControllerSystem();
void UpdateGunControllerSystem();
void AddGunController(Entity);
void RemoveGunController(Entity);

#endif
