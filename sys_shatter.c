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
		if ((c = GetCollider(components[i].e))) {
			struct Collider *into = GetCollider(c->update.into);
			if (into == NULL) {
				continue;
			}
			if (into->layers & components[i].layers) {
				RemoveEntity(components[i].e);
			}
		}
	}
}
