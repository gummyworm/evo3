#ifndef SYS_SHATTER_H
#define SYS_SHATTER_H

#include "base.h"

#include "entity.h"
#include "sys_collision.h"
#include "system.h"

SYSTEM(Shatter, enum CollisionLayer) {
	Entity e;
	enum CollisionLayer layers;
};

struct Shatter *GetShatter(Entity);
void RemoveShatter(Entity);

#endif
