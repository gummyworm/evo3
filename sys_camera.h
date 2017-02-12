#ifndef SYS_CAMERA_H
#define SYS_CAMERA_H

#include "base.h"

#include "entity.h"
#include "third-party/include/linmath.h"
#include <stdbool.h>
#include <stdint.h>

enum { MAX_CAMERAS = 16,
       MAX_CAMERA_PASSES = 2,
       MAX_RENDERS = 1024,
       TARGET_RES_X = 256,
       TARGET_RES_Y = 256,
};

/* Camera is a struct the view through which the game is rendered. */
struct Camera {
	Entity e;

	vec3 eye;
	vec3 dir;

	bool doPost;
	struct {
		int width, height;
		GLuint fbo;
		GLuint color, depth;

		GLuint vao;
		GLuint program;
	} passes[MAX_CAMERA_PASSES];
	int numPasses;

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
void AddColorRender(Entity, const char *, float, float, float, float);
void RemoveRender(Entity);

void InitCameraSystem();
void UpdateCameraSystem();
void AddCamera(Entity, uint32_t);
struct CameraUpdate *GetCameraUpdate(int *);

void AddCamera(Entity, uint32_t);
void RemoveCamera(Entity);
void CameraPerspective(Entity, float, float);
void CameraOrtho(Entity, float, float, float, float, float);
bool GetViewDir(Entity, float *, float *, float *);
void SetViewDir(Entity, float, float, float);
void SetEye(Entity, float, float, float);
bool GetEye(Entity, float *, float *, float *);
bool GetViewPos(Entity, float *, float *, float *);

void WorldToScreen(Entity, float, float, float, int *, int *);
void GetProjection(Entity, mat4x4);
void GetViewProjection(Entity, mat4x4);
bool Raycast(Entity, int, int, vec3, vec3);

#endif
