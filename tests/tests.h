#ifndef TESTS_H
#define TESTS_H

#include "../base.h"
#include "../debug.h"
#include <stdbool.h>

struct Test {
	int err;
};

#define RUNTEST(X)                                                             \
	do {                                                                   \
		T.err = 0;                                                     \
		X(&T);                                                         \
		if (T.err != 0)                                                \
			dinfof("FAIL");                                        \
		else                                                           \
			dinfof("PASSED");                                      \
	} while (0);

#define tassert(t, condition)                                                  \
	if (!(condition)) {                                                    \
		t->err = -1;                                                   \
	}

#define tassertf(t, expected, actual)                                          \
	do {                                                                   \
		if ((actual) != (expected)) {                                  \
			derrorf("expected %f but received %f\n", expected,     \
			        actual);                                       \
			t->err = -1;                                           \
		}                                                              \
	} while (0)

#define tassertv3(t, expected, actual)                                         \
	do {                                                                   \
		if ((actual[0] != expected[0]) ||                              \
		    (actual[1] != expected[1]) ||                              \
		    (actual[2] != expected[2])) {                              \
			derrorf("expected <%f, %f, %f> but received <%f, %f, " \
			        "%f>\n",                                       \
			        expected[0], expected[1], expected[2],         \
			        actual[0], actual[1], actual[2]);              \
			t->err = -1;                                           \
		}                                                              \
	} while (0)

void testChildSystem(struct Test *t);
void testMovementSystem(struct Test *t);
void testTransformSystem(struct Test *t);

#define TEST(NAME) bool NAME(struct Test *T)

#endif
