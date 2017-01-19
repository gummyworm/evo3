#include "sys_camera.h"
#include "draw.h"
#include "sys_transform.h"
#include "third-party/include/linmath.h"
#include "third-party/include/uthash.h"
#include <stdio.h>

struct entityToCamera {
	Entity e;
	struct Camera *camera;
	UT_hash_handle hh;
};

static struct entityToCamera *entitiesToCameras;
static struct Camera cameras[MAX_CAMERAS];
static int numCameras;

static int numUpdates;
static struct CameraUpdate updates[MAX_CAMERAS];

struct entityToRender {
	Entity e;
	struct Render *render;
	UT_hash_handle hh;
};

static struct entityToRender *entitiesToRenders;
static struct Render renders[MAX_RENDERS];
static int numRenders;

static GLFWwindow *win;

/* getCamera returns the camera attached to entity e (if there is one). */
static struct Camera *getCamera(Entity e) {
	struct entityToCamera *c;

	if (entitiesToCameras == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToCameras, &e, c);
	if (c == NULL)
		return NULL;

	return c->camera;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct CameraUpdate *u) { updates[numUpdates++] = *u; }

/* InitCameraSystem initializes the camera system. */
void InitCameraSystem(GLFWwindow *window) { win = window; }

/* UpdateCameraSystem updates all cameras that have been created. */
void UpdateCameraSystem() {
	int i, j;
	for (i = 0; i < numCameras; ++i) {
		float x, y, z;
		vec4 rot;
		mat4x4 m, v, mv, mvp;

		if (!GetPos(cameras[i].e, &x, &y, &z))
			continue;
		rot = GetRot(cameras[i].e);

		mat4x4_identity(v);
		mat4x4_translate(v, x, y, z);
		mat4x4_rotate_X(v, rot[0], 1.0f);
		printf("%f\n", z);
		for (j = 0; j < numRenders; ++j) {
			if (!GetPos(renders[i].e, &x, &y, &z))
				continue;
			mat4x4_translate(m, x, y, z);
			mat4x4_mul(mv, v, m);
			mat4x4_mul(mvp, cameras[i].projection, mv);
			Rect(win, mvp, 0, 0, 1, 1, 0xff00ffff);
		}
	}
	numUpdates = 0;
}

/* AddCamera adds a camera component to the entity e. */
void AddCamera(Entity e, uint32_t layers) {
	struct entityToCamera *item;

	if (getCamera(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToCamera));
	item->camera = cameras + numCameras;
	item->e = e;
	HASH_ADD_INT(entitiesToCameras, e, item);

	cameras[numCameras].e = e;
	cameras[numCameras].layers = layers;
	numCameras++;
}

/* GetCameraUpdates returns the camera updates this frame. */
struct CameraUpdate *GetCameraUpdates(int *num) {
	*num = numUpdates;
	return updates;
}

/* CameraPerspective sets the parameters of the camera attached to e. */
void CameraPerspective(Entity e, float fov, float aspect) {
	struct Camera *c;
	float near, far;

	c = getCamera(e);
	if (c == NULL) {
		printf("warning: no camera component found on entity %d\n", e);
		return;
	}

	near = 0.f;
	far = 100.f;

	c->type = CAMERA_PERSPECTIVE;
	c->params.perspective.fov = fov;
	c->params.perspective.aspect = aspect;
	c->params.perspective.near = near;
	c->params.perspective.far = far;

	mat4x4_perspective(c->projection, fov, aspect, near, far);
}

/* CameraOrtho sets the parameters of the camera attached to e. */
void CameraOrtho(Entity e, float aspect, float left, float right, float top,
                 float bot) {
	struct Camera *c;

	if ((c = getCamera(e)) == NULL)
		return;

	c->type = CAMERA_ORTHO;
	c->params.ortho.aspect = aspect;
	c->params.ortho.left = left;
	c->params.ortho.right = right;
	c->params.ortho.top = top;
	c->params.ortho.bot = bot;

	mat4x4_ortho(c->projection, -aspect, aspect, left, right, top, bot);
}

/* getRender returns the render attached to entity e (if there is one).
 */
static struct Render *getRender(Entity e) {
	struct entityToRender *c;

	if (entitiesToRenders == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToRenders, &e, c);
	if (c == NULL)
		return NULL;

	return c->render;
}

void AddRender(Entity e) {
	struct entityToRender *item;

	if (getRender(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToRender));
	item->render = renders + numRenders;
	item->e = e;
	HASH_ADD_INT(entitiesToRenders, e, item);

	renders[numRenders].e = e;
	numRenders++;
}
