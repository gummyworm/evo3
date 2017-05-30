#ifndef SYS_TRANSFORM_H
#define SYS_TRANSFORM_H

#include "system.h"
#include "entity.h"
#include "third-party/include/linmath.h"
#include <stdbool.h>

/* TRANSFORM_MIN_Y is the minimum value that relative movement will position
 * a transform (via TransformMove). */
#define TRANSFORM_MIN_Y 1.f

SYSTEM(Transform, float, float, float) {
	Entity e;
	float x, y, z;
	vec3 dp;
	struct {float x, y, z;}rot;
	struct {float x, y, z;}scale;
};

/* TransformUpdate defines an update message that can be polled by interested
 * systems. */
struct TransformUpdate {
	Entity e;
	float x, y, z;
};

void TransformMove(Entity, float, float, float);
void TransformSet(Entity, float, float, float);
void TransformRotate(Entity, float, float, float);
void TransformSetRotation(Entity, float, float, float);

void TransformSetScale(Entity, float, float, float);
bool TransformGetPos(Entity, vec3);
bool TransformGetRot(Entity, float *, float *, float *);
bool TransformGetScale(Entity, float *, float *, float *);

int GetInBounds(Entity *, int, vec3, vec3, bool (*)(Entity));
int GetIn2DBounds(Entity *, int, vec2, vec2, bool (*)(Entity));

#endif
