#include "../debug.h"
#include "../systems.h"
#include "tests.h"
#include <assert.h>

void testUnitSystem(struct Test *t) {
	const vec3 start = {1.f, 2.f, 3.f};
	const Entity e = 1;
	const int startHP = 100;

	InitUnitSystem();
	InitTransformSystem();

	AddTransform(e, start[0], start[1], start[2]);
	AddUnit(e, "unit", startHP);

	{
		struct Unit *units;
		int num;

		UnitHarm(e, startHP - 1);

		/* assert unit was harmed, but not removed from engine. */
		UpdateUnitSystem();
		units = GetUnits(&num);
		tassert(t, num == 1);
		tassert(t, units[0].hp == 1);

		/* harm unit for rest of its hp */
		UnitHarm(e, 1);
		tassert(t, units[0].hp == 0);
		UpdateUnitSystem();

		/* assert unit was removed from engine. */
		units = GetUnits(&num);
		tassert(t, num == 0);
	}
}
