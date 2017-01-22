#include <GL/glew.h>

#include "debug.h"
#include "draw.h"
#include "sys_camera.h"
#include "sys_mesh.h"
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

static void drawPost(struct Camera *c);

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
	struct Camera *c;
	int i, j;
	for (i = 0; i < numCameras; ++i) {
		GLint vp[4];
		struct {
			float x, y, z;
		} pos;
		struct {
			float x, y, z;
		} rot;
		mat4x4 m, v, mv, mvp;
		mat4x4 translated, xrotated, yrotated;

		c = cameras + i;
		if (!GetPos(c->e, &pos.x, &pos.y, &pos.z))
			continue;
		if (!GetRot(c->e, &rot.x, &rot.y, &rot.z))
			continue;

		glBindFramebuffer(GL_FRAMEBUFFER, c->target.fbo);
		glGetIntegerv(GL_VIEWPORT, vp);
		glViewport(0, 0, TARGET_RES_X, TARGET_RES_Y);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		mat4x4_identity(translated);
		mat4x4_translate(translated, pos.x, pos.y, pos.z);
		mat4x4_rotate_X(xrotated, translated, rot.x);
		mat4x4_rotate_Y(yrotated, xrotated, rot.y);
		mat4x4_rotate_Z(v, yrotated, rot.z);

		for (j = 0; j < numRenders; ++j) {
			if (!GetPos(renders[i].e, &pos.x, &pos.y, &pos.z))
				continue;
			mat4x4_identity(m);
			mat4x4_translate(m, pos.x, pos.y, pos.z);
			mat4x4_mul(mv, v, m);
			mat4x4_mul(mvp, cameras[i].projection, mv);

			if (GetMesh(renders[i].e) != NULL) {
				MeshDraw(renders[i].e, mvp);
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(vp[0], vp[1], vp[2], vp[3]);

		glDisable(GL_DEPTH_TEST);
		drawPost(cameras + i);
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

	/* RGBA8 texture, 24 bit depth texture, TARGET_RES_X x TARGET_RES_Y */
	glGenTextures(1, &cameras[numCameras].target.color);
	glBindTexture(GL_TEXTURE_2D, cameras[numCameras].target.color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TARGET_RES_X, TARGET_RES_Y, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &cameras[numCameras].target.depth);
	glBindRenderbuffer(GL_RENDERBUFFER, cameras[numCameras].target.depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, TARGET_RES_X,
	                      TARGET_RES_Y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &cameras[numCameras].target.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, cameras[numCameras].target.fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	                       GL_TEXTURE_2D, cameras[numCameras].target.color,
	                       0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	                          GL_RENDERBUFFER,
	                          cameras[numCameras].target.depth);

	{
		GLenum status;
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			dwarnf("FBO setup failed");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

/* AddRender adds a mesh loaded from filename. */
void AddRender(Entity e, const char *filename) {
	struct entityToRender *item;

	if (getRender(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToRender));
	item->render = renders + numRenders;
	item->e = e;
	HASH_ADD_INT(entitiesToRenders, e, item);

	renders[numRenders].e = e;
	numRenders++;

	if (filename != NULL)
		AddMesh(e, filename);
}

/* drawPost renders c's target texture to the display. */
static void drawPost(struct Camera *c) {
	mat4x4 mvp;
	int width, height;

	glfwGetFramebufferSize(win, &width, &height);
	mat4x4_ortho(mvp, 0, width, height, 0, 1.f, -1.f);
	TexRect(mvp, 0, 0, width, height, 0, 0, 1, 1, c->target.color);
}
