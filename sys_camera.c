#include "sys_camera.h"
#include "third-party/include/uthash.h"

struct entityToCamera {
	Entity e;
	struct Camera *camera;
	UT_hash_handle hh;
};

static struct entityToCamera *entitiesToCameras;
struct Camera cameras[MAX_CAMERAS];
static int numCameras;

static int numUpdates;
static struct CameraUpdate updates[MAX_CAMERAS];

/* getCamera returns the camera attached to entity e (if there is one). */
static struct Camera *getCamera(Entity e) {
	struct entityToCamera *t;
	HASH_FIND_INT(entitiesToCameras, &e, t);
	e = t->e;
	return cameras + e;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct CameraUpdate *u) { updates[numUpdates++] = *u; }

/* InitCameraSystem initializes the camera system. */
void InitCameraSystem() {}

/* UpdateCameraSystem updates all cameras that have been created. */
void UpdateCameraSystem() { numUpdates = 0; }

/* AddCamera adds a camera component to the entity e. */
void AddCamera(Entity e, uint32_t layers) {
	struct entityToCamera *item;

	if (getCamera(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToCamera));
	item->camera = cameras + numCameras;
	item->e = e;
	HASH_ADD_INT(entitiesToCameras, e, item);

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

	if ((c = getCamera(e)) == NULL)
		return;

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

	c->type = CAMERA_PERSPECTIVE;
	c->params.ortho.aspect = aspect;
	c->params.ortho.left = left;
	c->params.ortho.right = right;
	c->params.ortho.top = top;
	c->params.ortho.bot = bot;

	mat4x4_ortho(c->projection, -aspect, aspect, left, right, top, bot);
}
