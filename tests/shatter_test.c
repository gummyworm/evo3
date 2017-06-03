#include "../systems.h"
#include "tests.h"
#include <assert.h>

void testShatterSystem(struct Test *t) {
	Entity e1 = 1;
	Entity e2 = 2;
	enum CollisionLayer l = COLLISION_LAYER_ENEMY;

	InitTransformSystem();
	InitColliderSystem();
	InitShatterSystem();

	AddTransform(e1, 0, 0, 0);
	AddCollider(e1, 1.f, l);

	AddTransform(e2, 0.4f, 0, 0);
	AddCollider(e2, 1.f, 0);
	AddShatter(e2, l);

	/* assert entity is removed after update */
	UpdateColliderSystem();
	tassert(t, GetShatter(e2));
	UpdateShatterSystem();
	tassert(t, !GetShatter(e2));
}
