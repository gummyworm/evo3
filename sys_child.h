#ifndef SYS_CHILD_H
#define SYS_CHILD_H

#include "entity.h"
#include "third-party/include/linmath.h"
#include <stdbool.h>

/* ChildUpdate defines a child update message. */
struct ChildUpdate {};

/* Child is a struct that represents a bounds that can be collided with.
 */
struct Child {
	Entity e;
	Entity parent;
	struct ChildUpdate update;
	vec3 offset;
};

void InitChildSystem();
void UpdateChildSystem();
void AddChild(Entity, float);
void RemoveChild(Entity);
void ChildSystemClearUpdates();
struct ChildUpdate *GetChildUpdates(int *);
struct ChildUpdate *GetChildUpdate(Entity);
struct Child *GetChild(Entity);

extern int numChildUpdates;

#endif
