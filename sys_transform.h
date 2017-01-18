#ifndef SYS_TRANSFORM_H
#define SYS_TRANSFORM_H

#include "entity.h"
#include <stdbool.h>

enum { MAX_TRANSFORMS = 8192 };

/* Transform is a struct that represents the position/orientation of something.
 */
struct Transform {
	int x, y, z;
	float rot;
};

/* TransformUpdate defines an update message that can be polled by interested
 * systems. */
struct TransformUpdate {
	Entity e;
	int dx, dy, dz;
	float dr;
};

void InitTransformSystem();
void UpdateTransformSystem();
void AddTransform(Entity, int, int, int, float);
struct TransformUpdate *GetTransformUpdates(int *);

void TransformMove(Entity, int, int, int);
void TransformSet(Entity, int, int, int);
void TransformRotate(Entity, float);
void TransformSetRotation(Entity, float);

bool GetPos(Entity, float *, float *, float *);

#endif
