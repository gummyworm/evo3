#ifndef SYS_CAMERA_H
#define SYS_CAMERA_H

#include "entity.h"
#include "third-party/include/linmath.h"
#include <stdint.h>

enum { MAX_CAMERAS = 16,
       MAX_RENDERS = 1024,
};

/* Camera is a struct the view through which the game is rendered. */
struct Camera {
	Entity e;

	enum { CAMERA_ORTHO,
	       CAMERA_PERSPECTIVE,
	} type;
	union {
		struct {
			float fov;
			float aspect;
			float near, far;
		} perspective;
		struct {
			float aspect;
			float left, right;
			float top, bot;
		} ortho;
	} params;
	mat4x4 projection, view;
	uint32_t layers;
};

/* Render is a component that is visible to cameras. */
struct Render {
	Entity e;
};

/* CameraUpdate defines an update message that can be polled by
 * interested
 * systems. */
struct CameraUpdate {
	Entity e;
	int dx, dy, dz;
	float dr;
};

void AddRender(Entity, const char *);

void InitCameraSystem();
void UpdateCameraSystem();
void AddCamera(Entity, uint32_t);
struct CameraUpdate *GetCameraUpdate(int *);

void AddCamera(Entity, uint32_t);
void CameraPerspective(Entity, float, float);
void CameraOrtho(Entity, float, float, float, float, float);

#endif
