#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#define dprintf(...) printf(__VA_ARGS__)
#define dinfof(...)                                                            \
	do {                                                                   \
		fprintf(stdout, "info (%s (%s) line %d): ", __func__,          \
		        __FILE__, __LINE__);                                   \
		fprintf(stdout, __VA_ARGS__);                                  \
		fprintf(stdout, "\n");                                         \
	} while (0)

#define dwarnf(...)                                                            \
	do {                                                                   \
		fprintf(stderr, "warning (%s (%s) line %d): ", __func__,       \
		        __FILE__, __LINE__);                                   \
		fprintf(stderr, __VA_ARGS__);                                  \
		fprintf(stderr, "\n");                                         \
	} while (0)

#define derrorf(...)                                                           \
	do {                                                                   \
		fprintf(stderr, "error (%s (%s) line %d): ", __func__,         \
		        __FILE__, __LINE__);                                   \
		fprintf(stderr, __VA_ARGS__);                                  \
		fprintf(stderr, "\n");                                         \
	} while (0)

#endif
