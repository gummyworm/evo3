#include "tests.h"
#include "../debug.h"
#include <stdio.h>

int main() {
	struct Test T;
	RUNTEST(testChildSystem);
	RUNTEST(testMovementSystem);
	RUNTEST(testTransformSystem);
	RUNTEST(testUnitSystem);
	RUNTEST(testShatterSystem);
	RUNTEST(testQuestSystem);

	return 0;
}
