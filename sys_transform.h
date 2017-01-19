#ifndef SYS_TRANSFORM_H
#define SYS_TRANSFORM_H

#include "entity.h"
#include "third-party/include/linmath.h"
#include <stdbool.h>

enum { MAX_TRANSFORMS = 8192 };

/* Transform is a struct that represents the position/orientation of something.
 */
struct Transform {
	Entity e;
	float x, y, z;
	vec4 rot;
};

/* TransformUpdate defines an update message that can be polled by interested
 * systems. */
struct TransformUpdate {
	Entity e;
	float dx, dy, dz;
	float dr;
};

void InitTransformSystem();
void UpdateTransformSystem();
void AddTransform(Entity, float, float, float, float);
struct TransformUpdate *GetTransformUpdates(int *);

void TransformMove(Entity, float, float, float);
void TransformSet(Entity, float, float, float);
void TransformRotate(Entity, float);
void TransformSetRotation(Entity, float);

bool GetPos(Entity, float *, float *, float *);
vec4 *GetRot(Entity);

#endif
