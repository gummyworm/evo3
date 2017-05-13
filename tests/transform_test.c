#include "../debug.h"
#include "../systems.h"
#include "tests.h"
#include <assert.h>

void testTransformSystem(struct Test *t) {
	const Entity e = 1;
	vec3 pos;

	InitTransformSystem();

	{
		const vec3 start = {1.f, 2.f, 3.f};
		AddTransform(e, start[0], start[1], start[2]);
		tassert(t, GetPos(e, pos));
		tassertv3(t, start, pos);
	}

	{
		const vec3 newPos = {6.f, 7.f, 8.f};
		TransformSet(e, newPos[0], newPos[1], newPos[2]);
		tassert(t, GetPos(e, pos));
		tassertv3(t, newPos, pos);
	}
}
