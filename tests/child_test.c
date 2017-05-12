#include "../systems.h"
#include <assert.h>

void testChildSystem() {
	Entity c = 1;
	Entity p = 2;

	const float px = 1.f;
	const float py = 1.f;
	const float pz = 1.f;

	InitTransformSystem();
	InitChildSystem();

	AddTransform(p, px, py, pz);
	AddTransform(c, 0, 0, 0);
	AddChild(c, p);

	/* assert the child is updated to the parent's position. */
	UpdateChildSystem();
	{
		vec3 pos;
		GetPos(c, pos);
		assert(pos[0] == px);
		assert(pos[1] == px);
		assert(pos[2] == px);
	}
}
