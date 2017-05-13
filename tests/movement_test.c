#include "../debug.h"
#include "../systems.h"
#include "tests.h"
#include <assert.h>

void testMovementSystem(struct Test *t) {
	const Entity e = 1;
	const vec3 start = {1.f, 2.f, 3.f};
	const float vel = 1.f;
	vec3 dir = {1.f, 0.f, 0.f};
	float dt = 1.f;
	vec3 pos;

	InitTimeSystem();
	InitTransformSystem();
	InitMovementSystem();

	AddTransform(e, start[0], start[1], start[2]);
	GetPos(e, pos);
	AddMovement(e, vel, dir);

	{
		SetTime(dt);
		UpdateMovementSystem();
		GetPos(e, pos);

		tassertf(t, pos[0], (start[0] + dir[0] * vel * dt));
		tassertf(t, pos[1], (start[1] + dir[1] * vel * dt));
		tassertf(t, pos[2], (start[2] + dir[2] * vel * dt));
	}
}
