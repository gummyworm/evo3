#include "sys_child.h"
#include "entities.h"
#include "sys_camera.h"
#include "sys_transform.h"
#include "third-party/include/uthash.h"

struct entityToChild {
	Entity e;
	struct Child *child;
	UT_hash_handle hh;
};

static struct entityToChild *entitiesToChilds;
static struct Child childs[MAX_CHILDS];
static int numChilds;

int numChildUpdates;
static struct ChildUpdate updates[MAX_COLLIDERS];

/* GetChild returns the child attached to entity e (if there is one). */
struct Child *GetChild(Entity e) {
	struct entityToChild *t;

	if (entitiesToChilds == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToChilds, &e, t);
	if (t == NULL)
		return NULL;

	return t->child;
}

/* InitChildSystem initializes the child system. */
void InitChildSystem() {
	if (entitiesToChilds != NULL) {
		struct entityToChild *t, *tmp;
		HASH_ITER(hh, entitiesToChilds, t, tmp) {
			HASH_DEL(entitiesToChilds,
			         t); /* delete; users advances to next */
			free(t);     /* optional- if you want to free  */
		}
	}
	numChilds = 0;
}

/* UpdateChildSystem updates all childs that have been created. */
void UpdateChildSystem() {
	int i;

	/* move each child relative to its parent. */
	for (i = 0; i < numChilds; ++i) {
		vec3 pos;
		if (!TransformGetPos(childs[i].parent, pos))
			continue;
		vec3_add(pos, pos, childs[i].offset);
		TransformSet(childs[i].e, pos[0], pos[1], pos[2]);
	}
}

/* AddChild adds a child component to the entity e. */
void AddChild(Entity e, float parent) {
	struct entityToChild *item;

	if (GetChild(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToChild));
	item->child = childs + numChilds;
	item->e = e;

	childs[numChilds].e = e;
	childs[numChilds].parent = parent;
	childs[numChilds].offset[0] = 0.f;
	childs[numChilds].offset[1] = 0.f;
	childs[numChilds].offset[2] = 0.f;

	HASH_ADD_INT(entitiesToChilds, e, item);
	numChilds++;
}

/* RemoveChild removes the child attached to e from the Child
 * system. */
void RemoveChild(Entity e) {
	struct entityToChild *c;

	if (entitiesToChilds == NULL)
		return;

	HASH_FIND_INT(entitiesToChilds, &e, c);
	if (c != NULL) {
		struct Child *sys = c->child;
		int sz = (childs + numChilds) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToChilds, c);
		free(c);
		numChilds--;
	}
}

/* GetChildUpdate returns any child updates for the entity e. */
struct ChildUpdate *GetChildUpdate(Entity e) {
	struct Child *c;

	if ((c = GetChild(e)))
		return &c->update;

	return NULL;
}

/* GetChildUpdates returns the child updates this frame. */
struct ChildUpdate *GetChildUpdates(int *num) {
	*num = numChildUpdates;
	return updates;
}

/* ChildGetParent returns the parent of the child component attached to e. */
Entity ChildGetParent(Entity e) {
	struct Child *child = GetChild(e);
	if (child != NULL)
		return child->parent;
	return 0;
}
