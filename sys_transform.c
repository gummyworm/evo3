#include "sys_transform.h"
#include "entities.h"
#include "sys_actor.h"
#include "sys_camera.h"
#include "sys_child.h"
#include "third-party/include/uthash.h"

SYSDEF(Transform, float x, float y, float z)
C->x = x;
C->y = y;
C->z = z;
C->rot.x = 0;
C->rot.y = 0;
C->rot.z = 0;
C->scale.x = 1;
C->scale.y = 1;
C->scale.z = 1;
}

/* TransformMove moves the given entity by (dx, dy, dz) units. */
void SYSFUNC(Transform, Move, float dx, float dy, float dz) {
	COMPONENT(Transform, C);
	if ((C->y + dy) < TRANSFORM_MIN_Y)
		dy = 0.f;
	C->x += dx;
	C->y += dy;
	C->z += dz;
	C->dp[0] = dx;
	C->dp[1] = dy;
	C->dp[2] = dz;
}

/* TransformSet sets the given entity to the position (x, y, z). */
void SYSFUNC(Transform, Set, float x, float y, float z) {
	COMPONENT(Transform, C);
	C->dp[0] = C->x - x;
	C->dp[1] = C->y - y;
	C->dp[2] = C->z - z;

	C->x = x;
	C->y = y;
	C->z = z;
}

/* TransformRotate rotates the transform attached to e by dr degrees. */
void SYSFUNC(Transform, Rotate, float dx, float dy, float dz) {
	COMPONENT(Transform, C);
	C->rot.x += dx;
	C->rot.y += dy;
	C->rot.z += dz;
}

/* TransformSetRotation sets the rotation of the transform attached to e
 * to r degrees. */
void SYSFUNC(Transform, SetRotation, float x, float y, float z) {
	COMPONENT(Transform, C);
	C->rot.x = x;
	C->rot.y = y;
	C->rot.z = z;
}

/* GetPos sets pos to the position of entity e and returns the success of the
 * operation. */
bool SYSFUNC(Transform, GetPos, vec3 pos) {
	COMPONENT(Transform, C);
	if (C == NULL)
		return false;

	pos[0] = C->x;
	pos[1] = C->y;
	pos[2] = C->z;
	return true;
}

/* GetRot sets the x, y, and z rotation of entity e and returns
 * the
 * success. */
bool SYSFUNC(Transform, GetRot, float *x, float *y, float *z) {
	COMPONENT(Transform, C);
	if (C == NULL)
		return false;
	*x = C->rot.x;
	*y = C->rot.y;
	*z = C->rot.z;
	return true;
}

/* GetScale sets the x, y, and z scale of entity e and returns
 * the
 * success. */
bool SYSFUNC(Transform, GetScale, float *x, float *y, float *z) {
	COMPONENT(Transform, C);
	if (C == NULL)
		return false;
	*x = C->scale.x;
	*y = C->scale.y;
	*z = C->scale.z;
	return true;
}

/* SetScale sets the transform attached to e's scale to {x, y,
 * z}. */
void SYSFUNC(Transform, SetScale, float x, float y, float z) {
	COMPONENT(Transform, C);
	if (C == NULL)
		return;
	C->scale.x = x;
	C->scale.y = y;
	C->scale.z = z;
}

/* init initializes the Transform system. */
void init() {}

/* update updates all Transform components that have been created. */
void update(struct Transform *transforms, int num) {
	UNUSED(transforms);
	UNUSED(num);
}

/* contains returns true if pt lies within the given bounds. */
static bool contains(vec3 pt, vec3 center, vec3 dim) {
	vec3 llnCorner = {center[0] - dim[0] / 2.f, center[1] - dim[1] / 2.f,
	                  center[2] - dim[2] / 2.f};
	return (pt[0] > llnCorner[0]) && (pt[0] < llnCorner[0] + dim[0]) &&
	       (pt[1] > llnCorner[1]) && (pt[1] < llnCorner[1] + dim[1]) &&
	       (pt[2] > llnCorner[2]) && (pt[2] < llnCorner[2] + dim[2]);
}

/* contains returns true if pt lies within the given 2D bounds.
 */
static bool contains2d(vec2 pt, vec2 center, vec2 dim) {
	vec3 llnCorner = {center[0] - dim[0] / 2.f, center[1] - dim[1] / 2.f,
	                  center[2] - dim[2] / 2.f};
	return (pt[0] > llnCorner[0]) && (pt[0] < llnCorner[0] + dim[0]) &&
	       (pt[1] > llnCorner[1]) && (pt[1] < llnCorner[1] + dim[1]);
}

/* GetInBounds sets up to max entities found in the selection
 * area and
 * returns
 * the number found.  A function, filter, may be passed.  If not
 * NULL,
 * it should
 * return true if an entity is to be included in found. */
int GetInBounds(Entity *found, int max, vec3 center, vec3 dim,
                bool (*filter)(Entity)) {
	int i, numFound;
	for (i = 0, numFound = 0; i < numComponents && numFound < max; ++i) {
		vec3 pos = {components[i].x, components[i].y, components[i].z};
		if (contains(pos, center, dim)) {
			if (filter == NULL)
				found[numFound++] = components[i].e;
			else if (filter(components[i].e))
				found[numFound++] = components[i].e;
		}
	}
	return numFound;
}

int GetIn2DBounds(Entity *found, int max, vec2 center, vec2 dim,
                  bool (*filter)(Entity)) {
	int i, numFound;
	for (i = 0, numFound = 0; i < numComponents && numFound < max; ++i) {
		int x, y;
		vec3 pos = {components[i].x, components[i].y, components[i].z};
		WorldToScreen(E_PLAYER, pos[0], pos[1], pos[2], &x, &y);

		vec2 pos2d = {(float)x, (float)y};
		if (contains2d(pos2d, center, dim)) {
			if (filter == NULL)
				found[numFound++] = components[i].e;
			else if (filter(components[i].e))
				found[numFound++] = components[i].e;
		}
	}
	return numFound;
}
