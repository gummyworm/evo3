#include "sys_transform.h"
#include "third-party/include/uthash.h"

struct entityToTransform {
	Entity e;
	struct Transform *transform;
	UT_hash_handle hh;
};

static struct entityToTransform *entitiesToTransforms;
static struct Transform transforms[MAX_TRANSFORMS];
static int numTransforms;

int numTransformUpdates;
static struct TransformUpdate updates[MAX_TRANSFORMS];

/* getTransform returns the transform attached to entity e (if there is one). */
static struct Transform *getTransform(Entity e) {
	struct entityToTransform *t;

	if (entitiesToTransforms == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToTransforms, &e, t);
	if (t == NULL)
		return NULL;

	return t->transform;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct TransformUpdate *u) {
	updates[numTransformUpdates++] = *u;
}

/* InitTransformSystem initializes the transform system. */
void InitTransformSystem() {}

/* UpdateTransformSystem updates all transforms that have been created. */
void UpdateTransformSystem() {}

/* AddTransform adds a transform component to the entity e. */
void AddTransform(Entity e, float x, float y, float z) {
	struct entityToTransform *item;

	if (getTransform(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToTransform));
	item->transform = transforms + numTransforms;
	item->e = e;

	transforms[numTransforms].e = e;
	transforms[numTransforms].x = x;
	transforms[numTransforms].y = y;
	transforms[numTransforms].z = z;
	transforms[numTransforms].rot.x = 0;
	transforms[numTransforms].rot.y = 0;
	transforms[numTransforms].rot.z = 0;
	transforms[numTransforms].scale.x = 1;
	transforms[numTransforms].scale.y = 1;
	transforms[numTransforms].scale.z = 1;

	HASH_ADD_INT(entitiesToTransforms, e, item);
	numTransforms++;
}

/* RemoveTransform removes the transform attached to e from the Transform
 * system. */
void RemoveTransform(Entity e) {
	struct entityToTransform *c;

	if (entitiesToTransforms == NULL)
		return;

	HASH_FIND_INT(entitiesToTransforms, &e, c);
	if (c != NULL) {
		struct Transform *sys = c->transform;
		int sz = (transforms + numTransforms) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToTransforms, c);
		free(c);
	}
}

/* GetTransformUpdate returns any transform updates for the entity e. */
struct TransformUpdate *GetTransformUpdate(Entity e) {
	int i;

	for (i = 0; i < numTransformUpdates; ++i) {
		if (updates[i].e == e)
			return updates + i;
	}

	return NULL;
}

/* GetTransformUpdates returns the transform updates this frame. */
struct TransformUpdate *GetTransformUpdates(int *num) {
	*num = numTransformUpdates;
	return updates;
}

/* TransformMove moves the given entity by (dx, dy, dz) units. */
void TransformMove(Entity e, float dx, float dy, float dz) {
	struct Transform *t;

	t = getTransform(e);
	if (t == NULL)
		return;

	t->x += dx;
	t->y += dy;
	t->z += dz;

	{
		struct TransformUpdate u = {
		    .e = e, .x = t->x, .y = t->y, .z = t->z};
		addUpdate(&u);
	}
}

/* TransformSet sets the given entity to the position (x, y, z). */
void TransformSet(Entity e, float x, float y, float z) {
	struct Transform *t;

	t = getTransform(e);
	if (t == NULL)
		return;

	{
		struct TransformUpdate u = {
		    .e = e, t->x - x, t->y - y, t->z - z};
		addUpdate(&u);
	}

	t->x = x;
	t->y = y;
	t->z = z;
}

/* TransformRotate rotates the transform attached to e by dr degrees. */
void TransformRotate(Entity e, float dx, float dy, float dz) {
	struct Transform *t;

	t = getTransform(e);
	if (t == NULL)
		return;

	t->rot.x += dx;
	t->rot.y += dy;
	t->rot.z += dz;
}

/* TransformSetRotation sets the rotation of the transform attached to e
 * to r
 * degrees. */
void TransformSetRotation(Entity e, float x, float y, float z) {
	struct Transform *t;

	t = getTransform(e);
	if (t == NULL)
		return;

	t->rot.x = x;
	t->rot.y = y;
	t->rot.z = z;
}

/* GetPos sets x, y, and z to the position of entity e and returns the
 * success
 * of the operation. */
bool GetPos(Entity e, float *x, float *y, float *z) {
	struct Transform *t;
	if ((t = getTransform(e)) == NULL)
		return false;

	*x = t->x;
	*y = t->y;
	*z = t->z;
	return true;
}

/* GetRot sets the x, y, and z rotation of entity e and returns the
 * success. */
bool GetRot(Entity e, float *x, float *y, float *z) {
	struct Transform *t;
	if ((t = getTransform(e)) == NULL)
		return false;

	*x = t->rot.x;
	*y = t->rot.y;
	*z = t->rot.z;
	return true;
}

/* GetScale sets the x, y, and z scale of entity e and returns the
 * success. */
bool GetScale(Entity e, float *x, float *y, float *z) {
	struct Transform *t;
	if ((t = getTransform(e)) == NULL)
		return false;

	*x = t->scale.x;
	*y = t->scale.y;
	*z = t->scale.z;
	return true;
}

/* SetScale sets the transform attached to e's scale to {x, y, z}. */
void SetScale(Entity e, float x, float y, float z) {
	struct Transform *t;

	if ((t = getTransform(e)) == NULL)
		return;

	t->scale.x = x;
	t->scale.y = y;
	t->scale.z = z;
}

/* contains returns true if pt lies within the given bounds. */
static bool contains(vec3 pt, vec3 center, vec3 dim) {
	vec3 llnCorner = {center[0] - dim[0] / 2.f, center[1] - dim[1] / 2.f,
	                  center[2] - dim[2] / 2.f};
	return (pt[0] > llnCorner[0]) && (pt[0] < llnCorner[0] + dim[0]) &&
	       (pt[1] > llnCorner[1]) && (pt[1] < llnCorner[1] + dim[1]) &&
	       (pt[2] > llnCorner[2]) && (pt[2] < llnCorner[2] + dim[2]);
}

/* GetInArea sets up to max entities found in the selection area and returns
 * the number found.  A function, filter, may be passed.  If not NULL, it should
 * return true if an entity is to be included in found. */
int GetInBounds(Entity *found, int max, vec3 center, vec3 dim,
                bool (*filter)(Entity)) {
	int i, numFound;
	for (i = 0, numFound = 0; i < numTransforms && numFound < max; ++i) {
		vec3 pos = {transforms[i].x, transforms[i].y, transforms[i].z};
		if (contains(pos, center, dim)) {
			if (filter == NULL)
				found[numFound++] = transforms[i].e;
			else if (filter(transforms[i].e))
				found[numFound++] = transforms[i].e;
		}
	}
	return numFound;
}
