#include "sys_shatter.h"

SYSDEF(Shatter, enum CollisionLayer layers)
C->layers = layers;
C->e = e;
}

/* init initializes the Shatter system. */
void init() {}

/* update updates all Shatter components that have been created. */
void update(struct Shatter *components, int num) {
	int i;
	for (i = 0; i < num; ++i) {
		struct Collider *c;
		if ((c = GetCollider(components[i].e)))
			RemoveEntity(components[i].e);
	}
}
