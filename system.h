#ifndef SYSTEM_H
#define SYSTEM_H

#include "base.h"
#include "entity.h"

void InitSystems(GLFWwindow *);
void UpdateSystems();

#define MAX_COMPONENTS 8192

enum { SYS_ERR_NO_COMPONENT = -1,
};

#define SYSTEM(name, ...)                                                      \
	void Init##name##System();                                             \
	void Update##name##System();                                           \
	struct name *Get##name(Entity);                                        \
	struct name *Get##name##s(int *num);                                   \
	void Remove##name(Entity);                                             \
	void Add##name(Entity e, __VA_ARGS__);                                 \
	struct name

#define SYSDEF(name, ...)                                                      \
	struct entityToComponent {                                             \
		Entity e;                                                      \
		struct name *component;                                        \
		UT_hash_handle hh;                                             \
	};                                                                     \
	static struct name components[MAX_COMPONENTS];                         \
	static int numComponents;                                              \
	static struct entityToComponent *entitiesToComponents;                 \
	static void update(struct name *, int);                                \
	static void init();                                                    \
	struct name *Get##name(Entity e) {                                     \
		struct entityToComponent *c;                                   \
		if (entitiesToComponents == NULL)                              \
			return NULL;                                           \
		HASH_FIND_INT(entitiesToComponents, &e, c);                    \
		if (c == NULL)                                                 \
			return NULL;                                           \
		return c->component;                                           \
	}                                                                      \
	struct name *Get##name##s(int *num) {                                  \
		*num = numComponents;                                          \
		return components;                                             \
	}                                                                      \
	void Init##name##System() {                                            \
		if (entitiesToComponents != NULL) {                            \
			struct entityToComponent *c, *tmp;                     \
			HASH_ITER(hh, entitiesToComponents, c, tmp) {          \
				HASH_DEL(entitiesToComponents, c);             \
				free(c);                                       \
			}                                                      \
			init();                                                \
		}                                                              \
		numComponents = 0;                                             \
	}                                                                      \
	void Remove##name(Entity e) {                                          \
		struct entityToComponent *c;                                   \
		if (entitiesToComponents == NULL)                              \
			return;                                                \
		HASH_FIND_INT(entitiesToComponents, &e, c);                    \
		if (c != NULL) {                                               \
			struct name *sys = c->component;                       \
			int sz = (components + numComponents) - sys;           \
			memmove(sys, sys + 1, sz);                             \
			HASH_DEL(entitiesToComponents, c);                     \
			free(c);                                               \
			numComponents--;                                       \
		}                                                              \
	}                                                                      \
	void Update##name##System() { update(components, numComponents); }     \
	void Add##name(Entity e, __VA_ARGS__) {                                \
		{                                                              \
			struct entityToComponent *c;                           \
			if (Get##name(e) != NULL)                              \
				return;                                        \
			c = malloc(sizeof(struct entityToComponent));          \
			c->component = components + numComponents;             \
			c->e = e;                                              \
			components[numComponents].e = e;                       \
			HASH_ADD_INT(entitiesToComponents, e, c);              \
		}                                                              \
		struct name *C = (components + numComponents);                 \
		numComponents++;

/* SYSPROC declares a method for the system name. These methods are safe to
 * call via the SYSCALL macro. */
#define SYSFUNC(name, method, ...) name##method(Entity e, __VA_ARGS__)

/* C gets a component from the system sys and assigns it the name name.
 * This macro is only valid within a method defined via SYSFUNC. */
#define COMPONENT(sys, name) struct sys *name = Get##sys(e)

/* SYSCALL attempts to invoke method on the entity that C belongs to. */
#define SYSCALL(sys, method, C, ...)                                           \
	do {                                                                   \
		struct sys *s = Get##sys(C->e);                                \
		if (s != NULL)                                                 \
			sys##method(C->e, __VA_ARGS__);                        \
		else                                                           \
			Err = SYS_ERR_NO_COMPONENT;                            \
	} while (0)

#define GETCOMPONENT(sys, entity, C)                                           \
	                ((C = Get ## sys(entity))

/* Err is a global error code that contains the result of the status of the last
 * procedure call. */
extern int Err;

#endif
