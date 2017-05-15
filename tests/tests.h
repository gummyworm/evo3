#ifndef TESTS_H
#define TESTS_H

#include "../base.h"
#include "../debug.h"
#include <stdbool.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

struct Test {
	int err;
};

void testChildSystem(struct Test *t);
void testMovementSystem(struct Test *t);
void testTransformSystem(struct Test *t);
void testUnitSystem(struct Test *t);

#define RUNTEST(X)                                                             \
	do {                                                                   \
		T.err = 0;                                                     \
		X(&T);                                                         \
		printf("[%s]:", #X);                                           \
		if (T.err != 0)                                                \
			printf(ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET "\n");   \
		else                                                           \
			printf(ANSI_COLOR_GREEN "PASSED" ANSI_COLOR_RESET      \
			                        "\n");                         \
	} while (0);

#define tassert(t, condition)                                                  \
	if (!(condition)) {                                                    \
		derrorf("assertion failed");                                   \
		t->err = -1;                                                   \
	}

#define tassertf(t, expected, actual)                                          \
	do {                                                                   \
		if ((actual) != (expected)) {                                  \
			derrorf("expected %f but received %f", expected,       \
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
			        "%f>",                                         \
			        expected[0], expected[1], expected[2],         \
			        actual[0], actual[1], actual[2]);              \
			t->err = -1;                                           \
		}                                                              \
	} while (0)

#define TEST(NAME) bool NAME(struct Test *T)

#endif
