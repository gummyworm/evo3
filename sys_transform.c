#include "sys_transform.h"
#include "third-party/include/uthash.h"

struct entityToTransform {
	Entity e;
	struct Transform *transform;
	UT_hash_handle hh;
};

static struct entityToTransform *entitiesToTransforms;
struct Transform transforms[MAX_TRANSFORMS];
static int numTransforms;

static int numUpdates;
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
static void addUpdate(struct TransformUpdate *u) { updates[numUpdates++] = *u; }

/* InitTransformSystem initializes the transform system. */
void InitTransformSystem() {}

/* UpdateTransformSystem updates all transforms that have been created. */
void UpdateTransformSystem() { numUpdates = 0; }

/* AddTransform adds a transform component to the entity e. */
void AddTransform(Entity e, int x, int y, int z, float rot) {
	struct entityToTransform *item;

	if (getTransform(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToTransform));
	item->transform = transforms + numTransforms;
	item->e = e;

	transforms[numTransforms].x = x;
	transforms[numTransforms].y = y;
	transforms[numTransforms].z = z;
	transforms[numTransforms].rot = rot;

	HASH_ADD_INT(entitiesToTransforms, e, item);
	numTransforms++;
}

/* GetTransformUpdates returns the transform updates this frame. */
struct TransformUpdate *GetTransformUpdates(int *num) {
	*num = numUpdates;
	return updates;
}

/* TransformMove moves the given entity by (dx, dy, dz) units. */
void TransformMove(Entity e, int dx, int dy, int dz) {
	struct Transform *t;

	t = getTransform(e);
	if (t == NULL)
		return;

	t->x += dx;
	t->y += dy;
	t->z += dz;
}

/* TransformSet sets the given entity to the position (x, y, z). */
void TransforSet(Entity e, int x, int y, int z) {
	struct Transform *t;

	t = getTransform(e);
	if (t == NULL)
		return;

	{
		struct TransformUpdate u = {e, t->x - x, t->y - y, t->z - z};
		addUpdate(&u);
	}

	t->x = x;
	t->y = y;
	t->z = z;
}

/* TransformRotate rotates the transform attached to e by dr degrees. */
void TransformRotate(Entity e, float dr) {
	struct Transform *t;

	t = getTransform(e);
	if (t == NULL)
		return;

	{
		struct TransformUpdate u = {e, 0, 0, 0, t->rot - dr};
		addUpdate(&u);
	}

	t->rot += dr;
}

/* TransformSetRotation sets the rotation of the transform attached to e to r
 * degrees. */
void TransformSetRotation(Entity e, float r) {
	struct Transform *t;

	t = getTransform(e);
	if (t == NULL)
		return;

	{
		struct TransformUpdate u = {e, 0, 0, 0, r};
		addUpdate(&u);
	}

	t->rot = r;
}

/* GetPos sets x, y, and z to the position of entity e and returns the success
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
