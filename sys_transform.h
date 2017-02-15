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
	struct {
		float x, y, z;
	} rot;
	struct {
		float x, y, z;
	} scale;
};

/* TransformUpdate defines an update message that can be polled by interested
 * systems. */
struct TransformUpdate {
	Entity e;
	float x, y, z;
};

void InitTransformSystem();
void UpdateTransformSystem();
void AddTransform(Entity, float, float, float);
void RemoveTransform(Entity);
void TransformSystemClearUpdates();
struct TransformUpdate *GetTransformUpdates(int *);
struct TransformUpdate *GetTransformUpdate(Entity);

void TransformMove(Entity, float, float, float);
void TransformSet(Entity, float, float, float);
void TransformRotate(Entity, float, float, float);
void TransformSetRotation(Entity, float, float, float);

void SetScale(Entity, float, float, float);

bool GetPos(Entity, float *, float *, float *);
bool GetRot(Entity, float *, float *, float *);
bool GetScale(Entity, float *, float *, float *);

int GetInBounds(Entity *, int, vec3, vec3, bool (*)(Entity));
int GetIn2DBounds(Entity *, int, vec2, vec2, bool (*)(Entity));

extern int numTransformUpdates;

#endif
