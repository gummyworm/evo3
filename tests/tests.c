#include "tests.h"
#include "../debug.h"
#include <stdio.h>

int main() {
	struct Test T;
	RUNTEST(testChildSystem);
	RUNTEST(testMovementSystem);
	RUNTEST(testTransformSystem);

	puts("passed");
	return 0;
}
