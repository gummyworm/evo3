#include <stdio.h>

#include "base.h"

#include "debug.h"
#include "draw.h"
#include "entities.h"
#include "systems.h"
#include "thing.h"
#include "third-party/include/linmath.h"
#include "third-party/include/uthash.h"

struct entityToCamera {
	Entity e;
	struct Camera *camera;
	UT_hash_handle hh;
};

static struct entityToCamera *entitiesToCameras;
static struct Camera cameras[MAX_CAMERAS];
static int numCameras;

struct entityToRender {
	Entity e;
	struct Render *render;
	UT_hash_handle hh;
};

static struct entityToRender *entitiesToRenders;
static struct Render renders[MAX_RENDERS];
static int numRenders;

static GLFWwindow *win;

static void lmouse(int, int);

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

/* getView sets the view matrix v appropriately. */
static void getView(struct Camera *c, mat4x4 v) {
	vec3 eye, center, up;

	GetViewPos(c->e, &eye[0], &eye[1], &eye[2]);
	GetViewDir(c->e, &up[0], &up[1], &up[2]);
	vec3_add(center, eye, up);
	up[0] = 0;
	up[1] = 1;
	up[2] = 0;

	mat4x4_look_at(v, eye, center, up);
}

/* InitCameraSystem initializes the camera system. */
void InitCameraSystem(GLFWwindow *window) { win = window; }

/* doPass renders the given post-render pass for the camera c. */
static void doPass(struct Camera *c, int pass) {
	mat4x4 mvp;
	int width, height;

	glDisable(GL_DEPTH_TEST);

	glfwGetFramebufferSize(win, &width, &height);
	mat4x4_identity(mvp);
	mat4x4_ortho(mvp, 0, width, height, 0, 1.f, -1.f);
	TexRect(mvp, c->passes[pass].program, 0, 0, width, height, 0, 0, 1, 1,
	        c->passes[pass].color);
}

/* drawRender renders the given render entity using the given view/projection
 * matrices. */
void drawRender(mat4x4 v, mat4x4 p, int i) {
	mat4x4 m, mv, mvp, tmp;
	vec3 pos;
	float sx, sy, sz;

	if (!Enabled(renders[i].e))
		return;
	if (!GetPos(renders[i].e, pos))
		return;
	if (!GetScale(renders[i].e, &sx, &sy, &sz))
		return;

	mat4x4_identity(m);
	mat4x4_translate(tmp, pos[0], pos[1], pos[2]);
	mat4x4_scale_aniso(m, tmp, sx, sy, sz);
	mat4x4_mul(mv, v, m);
	mat4x4_mul(mvp, p, mv);

	if (GetMesh(renders[i].e) != NULL) {
		MeshDraw(renders[i].e, mvp);
	}
}

/* UpdateCameraSystem updates all cameras that have been created. */
void UpdateCameraSystem() {
	struct Camera *c;
	int i, j;
	mat4x4 v;
	vec3 pos;
	struct {
		float x, y, z;
	} rot;

	for (i = 0; i < numCameras; ++i) {
		GLuint program;
		int passes;

		c = cameras + i;
		if (!Enabled(c->e))
			continue;
		if (!GetPos(c->e, pos))
			return;
		if (!GetViewDir(c->e, &rot.x, &rot.y, &rot.z))
			return;

		passes = c->numPasses;
		if (!c->doPost)
			passes = 0;

		if (passes > 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, c->passes[0].fbo);
			glViewport(0, 0, c->passes[0].width,
			           c->passes[0].height);
			program = c->passes[0].program;
		} else {
			program = COLOR_PROGRAM;
		}
		glUseProgram(program);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClearDepth(100.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		{
			vec3 eye, center, up;
			GetPos(c->e, eye);
			vec3_add(eye, eye, c->eye);
			GetViewDir(c->e, &up[0], &up[1], &up[2]);
			vec3_add(center, eye, up);
			up[0] = 0;
			up[1] = 1;
			up[2] = 0;
			mat4x4_look_at(v, eye, center, up);
		}

		for (j = 0; j < numRenders; ++j)
			drawRender(v, cameras[i].projection, j);

		for (j = 0; j < passes; ++j) {
			if (j < (passes - 1)) {
				glBindFramebuffer(GL_FRAMEBUFFER,
				                  c->passes[j + 1].fbo);
				glViewport(0, 0, c->passes[j + 1].width,
				           c->passes[j + 1].height);
			} else {
				int w, h;
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glfwGetFramebufferSize(win, &w, &h);
				glViewport(0, 0, w, h);
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			doPass(c, j);
		}
	}
}

static void addPass(struct Camera *c, GLint program, int w, int h) {
	int p = c->numPasses;

	if (p > MAX_CAMERA_PASSES) {
		dwarnf("maximum number of camera passes (%d) exceeded\n",
		       MAX_CAMERA_PASSES);
		return;
	}

	/* RGBA8 texture, 24 bit depth texture */
	glGenTextures(1, &c->passes[p].color);
	glBindTexture(GL_TEXTURE_2D, c->passes[p].color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA,
	             GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &c->passes[p].depth);
	glBindRenderbuffer(GL_RENDERBUFFER, c->passes[p].depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &c->passes[p].fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, c->passes[p].fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	                       GL_TEXTURE_2D, c->passes[p].color, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	                          GL_RENDERBUFFER, c->passes[p].depth);

	{
		GLenum status;
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			dwarnf("FBO setup failed");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	c->passes[p].program = program;
	c->passes[p].width = w;
	c->passes[p].height = h;

	c->numPasses++;
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
	cameras[numCameras].dir[0] = 0;
	cameras[numCameras].dir[1] = 0;
	cameras[numCameras].dir[2] = 1;
	cameras[numCameras].layers = layers;
	cameras[numCameras].numPasses = 0;
	cameras[numCameras].doPost = false;

	int w, h;
	glfwGetFramebufferSize(win, &w, &h);
	addPass(cameras + numCameras, BAYER_PROGRAM, 256, 256);
	addPass(cameras + numCameras, TEXTURE_PROGRAM, 256, 256);

	InputRegisterMouseButtonEvent(e, INPUT_LAYER_DEFAULT, lmouse, NULL);

	numCameras++;
}

/* Remove camera removes the camera attached to e from the Camera system. */
void RemoveCamera(Entity e) {
	struct entityToCamera *c;

	if (entitiesToCameras == NULL)
		return;

	HASH_FIND_INT(entitiesToCameras, &e, c);
	if (c != NULL) {
		struct Camera *sys = c->camera;
		int sz = (cameras + numCameras) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToCameras, c);
		free(c);
	}
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

	near = 0.05f;
	far = 100.f;

	c->type = CAMERA_PERSPECTIVE;
	c->params.perspective.fov = fov;
	c->params.perspective.aspect = aspect;
	c->params.perspective.near = near;
	c->params.perspective.far = far;

	mat4x4_identity(c->projection);
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

	mat4x4_identity(c->projection);
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

/* AddColorRender adds a mesh loaded from filename and colors it (r,g,b,a,). */
void AddColorRender(Entity e, const char *filename, float r, float g, float b,
                    float a) {
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
		AddColorMesh(e, filename, r, g, b, a);
}

/* RemoveRender removes the render attached to e from the Render system. */
void RemoveRender(Entity e) {
	struct entityToRender *c;

	if (entitiesToRenders == NULL)
		return;

	HASH_FIND_INT(entitiesToRenders, &e, c);
	if (c != NULL) {
		struct Render *sys = c->render;
		int sz = (renders + numRenders) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToRenders, c);
		free(c);
	}
}

/* GetViewDir returns the direction vector the camera is viewing. */
bool GetViewDir(Entity e, float *x, float *y, float *z) {
	struct Camera *c;

	if ((c = getCamera(e)) == NULL)
		return false;

	*x = c->dir[0];
	*y = c->dir[1];
	*z = c->dir[2];
	return true;
}

/* SetViewDir sets the view direction of e's camera to (x, y, z). */
void SetViewDir(Entity e, float x, float y, float z) {
	struct Camera *c;

	if ((c = getCamera(e)) == NULL)
		return;

	c->dir[0] = x;
	c->dir[1] = y;
	c->dir[2] = z;
}

/* GetEye returns the position of the camera's eyepoint. */
bool GetEye(Entity e, float *x, float *y, float *z) {
	struct Camera *c;

	if ((c = getCamera(e)) == NULL)
		return false;

	*x = c->eye[0];
	*y = c->eye[1];
	*z = c->eye[2];
	return true;
}

/* SetEye sets the eye position of e's camera to (x, y, z). */
void SetEye(Entity e, float x, float y, float z) {
	struct Camera *c;

	if ((c = getCamera(e)) == NULL)
		return;

	c->eye[0] = x;
	c->eye[1] = y;
	c->eye[2] = z;
}

/* GetViewPos retrieves the eye point that the camera is viewing from PLUS the
 * transform of the entity that it is attached to. */
bool GetViewPos(Entity e, float *x, float *y, float *z) {
	struct Camera *c;
	vec3 t = {0, 0, 0};

	if ((c = getCamera(e)) == NULL)
		return false;

	GetPos(e, t);

	*x = c->eye[0] + t[0];
	*y = c->eye[1] + t[1];
	*z = c->eye[2] + t[2];
	return true;
}

/* WorldToScreen sets (sx, sy) to the screen coordiantes of the given (x, y, z)
 * world point as seen by the camera attached to e. If sx and sy are negative
 * the call either failed, or the point is not visible to the camera. */
void WorldToScreen(Entity e, float x, float y, float z, int *sx, int *sy) {
	vec4 projected;
	struct Camera *cam;

	*sx = -1;
	*sy = -1;

	cam = getCamera(e);
	if (cam == NULL)
		return;

	{
		mat4x4 v, pv;
		vec4 pt = {x, y, z, 1};

		getView(cam, v);
		mat4x4_mul(pv, cam->projection, v);
		mat4x4_mul_vec4(projected, pv, pt);
	}

	/* test if point is visible. */
	if (projected[3] <= 0)
		return;

	projected[0] /= projected[3];
	projected[1] /= projected[3];

	{
		int width, height;
		// glfwGetFramebufferSize(win, &width, &height);
		glfwGetWindowSize(win, &width, &height);
		*sx = (projected[0] + 1.f) / 2.f * (float)width;
		*sy = (1.f - projected[1]) / 2.f * (float)height;
	}
}

void ScreenToWorld(Entity e, float x, float y, float *wx, float *wy,
                   float *wz) {
	mat4x4 vp, ivp;
	GetViewProjection(e, vp);
	mat4x4_invert(ivp, vp);
	vec4 pos;

	vec4 in;
	int screenW, screenH;
	float winZ = 0.f;

	glfwGetFramebufferSize(win, &screenW, &screenH);

	in[0] = (2.f * ((float)(x - 0.f) / (screenW - 0.f))) - 1.f,
	in[1] = 1.f - (2.0f * ((float)(y - 0) / (screenH - 0)));
	in[2] = 2.f * winZ - 1.f;
	in[3] = 1.f;

	{
		mat4x4_mul_vec4(pos, ivp, in);
		pos[3] = 1.0 / pos[3];
	}

	*wx = pos[0] * pos[3];
	*wy = pos[1] * pos[3];
	*wz = pos[2] * pos[3];
}

/* Raycast casts a ray from the camera at the given screen coordinates. */
bool Raycast(Entity e, int x, int y, vec3 start, vec3 stop) {
	struct Camera *c;
	int width, height;
	mat4x4 pv, invVP;

	if ((c = getCamera(e)) == NULL)
		return false;

	glfwGetFramebufferSize(win, &width, &height);
	GetViewProjection(e, pv);
	mat4x4_invert(invVP, pv);

	{
		vec4 res, norm;
		x = width / 2;
		y = height / 2;
		vec4 scr = {(float)x / width * 0.5f - 1.f,
		            (float)y / height * 0.5f - 1.f, 1.f, 1.f};
		mat4x4_mul_vec4(res, invVP, scr);

		vec4_norm(norm, res);
		if (norm[3] == 0.f)
			return false;
		start[0] = norm[0];
		start[1] = norm[1];
		start[2] = norm[2];
	}

	{
		vec4 res;
		float z = 100.f;
		vec4 in = {(float)x / width * 2.f - 1.f,
		           (float)y / height * 2.f - 1.f, 2.f * z - 1.f, 1.f};
		mat4x4_mul_vec4(res, invVP, in);
		if (res[3] == 0.f)
			return false;
		stop[0] = res[0];
		stop[1] = res[1];
		stop[2] = res[2];
	}

	return true;
}

/* GetProjection sets projection to e's camera's projection matrix. */
void GetProjection(Entity e, mat4x4 projection) {
	struct Camera *c;

	c = getCamera(e);
	if (c == NULL)
		return;

	mat4x4_dup(projection, c->projection);
}

/* GetViewProjection sets proj to e's view * projection matrix. */
void GetViewProjection(Entity e, mat4x4 proj) {
	struct Camera *c;
	mat4x4 v;

	c = getCamera(e);
	if (c == NULL)
		return;

	getView(c, v);
	mat4x4_mul(proj, c->projection, v);
}

/* lmouse is the left mouse button callback. */
void lmouse(Entity e, int action) {
	if (action == GLFW_PRESS) {
		double x, y;
		vec3 start, stop;
		glfwGetCursorPos(win, &x, &y);
		if (Raycast(E_PLAYER, x, y, start, stop)) {
			dinfof("%f %f %f", start[0], start[1], start[2]);
			dinfof("%f %f %f", stop[0], stop[1], stop[2]);
		} else {
			dwarnf("raycast failed");
		}
	}
}
