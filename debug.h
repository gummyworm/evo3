#ifndef DEBUG_H
#define DEBUG_H

#include <assert.h>
#include <stdio.h>

#define dprintf(...) printf(__VA_ARGS__)

#define dprintv3(v) printf("<%f, %f, %f>\n", v[0], v[1], v[2])

#define dinfof(...)                                                            \
	do {                                                                   \
		fprintf(stdout, "info [%s (%s) line %d]: ", __func__,          \
		        __FILE__, __LINE__);                                   \
		fprintf(stdout, __VA_ARGS__);                                  \
		fprintf(stdout, "\n");                                         \
	} while (0)

#define dwarnf(...)                                                            \
	do {                                                                   \
		fprintf(stderr, "warning [%s (%s) line %d]: ", __func__,       \
		        __FILE__, __LINE__);                                   \
		fprintf(stderr, __VA_ARGS__);                                  \
		fprintf(stderr, "\n");                                         \
	} while (0)

#define derrorf(...)                                                           \
	do {                                                                   \
		fprintf(stderr, "error [%s (%s) line %d]: ", __func__,         \
		        __FILE__, __LINE__);                                   \
		fprintf(stderr, __VA_ARGS__);                                  \
		fprintf(stderr, "\n");                                         \
	} while (0)

#define dassert(...) assert(__VA_ARGS__)

#define dassertf(actual, expected)                                             \
	do {                                                                   \
		if ((actual) != (expected))                                    \
			dprintf("expected %f but received %f\n", expected,     \
			        actual);                                       \
	} while (0)

#define dassertv3(actual, expected)                                            \
	do {                                                                   \
		if ((actual[0] != expected[0]) ||                              \
		    (actual[1] != expected[1]) || (actual[2] != expected[2]))  \
			dprintf("expected <%f, %f, %f> but received <%f, %f, " \
			        "%f>\n",                                       \
			        expected[0], expected[1], expected[2],         \
			        actual[0], actual[1], actual[2]);              \
	} while (0)

void dinstallhandlers();

#endif
