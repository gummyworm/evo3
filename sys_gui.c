#include "base.h"
#include <string.h>

#include "draw.h"

#include "systems.h"
#include "third-party/include/linmath.h"
#include "third-party/include/uthash.h"
#include <stdio.h>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define GUI_NEAR_PLANE -1.f
#define GUI_FAR_PLANE 1.f

struct entityToWidget {
	Entity e;
	struct Widget *widget;
	UT_hash_handle hh;
};

static struct entityToWidget *entitiesToWidgets;
static struct Widget widgets[MAX_WIDGETS];
static int numWidgets;

static int numUpdates;
static struct WidgetUpdate updates[MAX_WIDGETS];

static void DrawWidget(struct Widget *);

static GLFWwindow *win;

static struct { GLuint overlay; } skin;

/* drawTextbox renders the textbox w. */
static void drawTextbox(struct Widget *w) {
	mat4x4 proj;
	mat4x4_identity(proj);
	GuiProjection(proj);
	Text(proj, w->x, w->y, w->data.textbox.fontsize, w->data.textbox.text);
}

/* drawWindow renders the window w. */
static void drawWindow(struct Widget *w) {
	mat4x4 proj, gui, trans;

	{
		GLint vp[4];
		float xs, ys;
		glGetIntegerv(GL_VIEWPORT, vp);

		xs = (float)vp[2] / GUI_WIDTH;
		ys = (float)vp[3] / GUI_HEIGHT;

		glEnable(GL_SCISSOR_TEST);
		glScissor(w->x * xs, ys * (GUI_HEIGHT - w->y - w->height),
		          xs * w->width, ys * w->height);
	}

	GuiProjection(proj);
	Rect(proj, w->x, w->y + w->border.height, w->width,
	     w->height - w->border.height, 0x000000ff);

	mat4x4_translate(trans, w->x, w->y + w->border.height, 0.f);
	GuiProjection(gui);
	mat4x4_mul(proj, gui, trans);
	w->data.window.render(w->e, proj, w->width, w->height);

	GuiProjection(proj);
	TexRect(proj, TEXTURE_PROGRAM, w->x, w->y, w->width, w->height, 0.f,
	        0.f, 1.f, 1.f, skin.overlay);

	glDisable(GL_SCISSOR_TEST);
}

/* getWidget returns the widget attached to entity e (if there is one). */
static struct Widget *getWidget(Entity e) {
	struct entityToWidget *t;

	if (entitiesToWidgets == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToWidgets, &e, t);
	if (t == NULL)
		return NULL;

	return t->widget;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct WidgetUpdate *u) { updates[numUpdates++] = *u; }

/* addWidget is a helper function that copies w to the GUI system. */
static void addWidget(Entity e, struct Widget *w) {
	struct entityToWidget *item;

	if (getWidget(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToWidget));
	item->widget = widgets + numWidgets;
	item->e = e;

	memcpy(widgets + numWidgets, w, sizeof(struct Widget));
	widgets[numWidgets].e = e;

	HASH_ADD_INT(entitiesToWidgets, e, item);
	numWidgets++;
}

/* RemoveWidget removes the widget attached to e from the Widget system. */
void RemoveWidget(Entity e) {
	struct entityToWidget *c;

	if (entitiesToWidgets == NULL)
		return;

	HASH_FIND_INT(entitiesToWidgets, &e, c);
	if (c != NULL) {
		struct Widget *sys = c->widget;
		int sz = (widgets + numWidgets) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToWidgets, c);
		free(c);
		numWidgets--;
	}
}

/* InitWidgetSystem initializes the widget system. */
void InitWidgetSystem(GLFWwindow *w) {
	win = w;
	skin.overlay = GetTexture("res/consoleoverlay.png");
}

/* UpdateWidgetSystem updates all widgets that have been created. */
void UpdateWidgetSystem() {
	int i;

	for (i = 0; i < numWidgets; ++i) {
		struct Widget *w;
		w = widgets + i;

		if (!Enabled(w->e))
			continue;

		switch (w->type) {
		case TEXTBOX:
			drawTextbox(w);
		case WINDOW:
			drawWindow(w);
		case NONE:
		default:
			break;
		}
	}

	numUpdates = 0;
}

/* AddWidget adds a widget component to the entity e. */
void AddWidget(Entity e) {
	struct entityToWidget *item;

	if (getWidget(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToWidget));
	item->widget = widgets + numWidgets;
	item->e = e;

	widgets[numWidgets].e = e;

	HASH_ADD_INT(entitiesToWidgets, e, item);
	numWidgets++;
}

/* GetWidgetUpdates returns the widget updates this frame. */
struct WidgetUpdate *GetWidgetUpdates(int *num) {
	*num = numUpdates;
	return updates;
}

/* DrawWidget renders the widget w. */
static void DrawWidget(struct Widget *w) {
	mat4x4 proj;
	mat4x4_identity(proj);
	GuiProjection(proj);

	Rect(proj, w->x, w->y, w->width, w->height, w->color);
}

/* AddTextBox adds a TextBox widget to entity e. */
void AddTextBox(Entity e, unsigned x, unsigned y, const char *text) {
	struct Widget w = {
	    .x = x,
	    .y = y,
	    .width = 100,
	    .height = 32,
	    .color = 0x00000000,
	    .type = TEXTBOX,
	    .data = {.textbox = {.text = text, .fontsize = 32}},
	    .border = {.width = 10, .height = 10},
	};
	addWidget(e, &w);
}

static void ldispatch(Entity e, int action) {
	struct Widget *w;

	if ((w = getWidget(e)) == NULL)
		return;

	if (w->data.window.lmouse)
		w->data.window.lmouse(e, action);
}

static void rdispatch(Entity e, int action) {
	struct Widget *w;

	if ((w = getWidget(e)) == NULL)
		return;

	if (w->data.window.rmouse)
		w->data.window.rmouse(e, action);
}

/* AddRenderWindow adds a Window widget to entity e. */
void AddRenderWindow(Entity e, int x, int y,
                     void (*render)(Entity, mat4x4, int, int),
                     MouseButtonEvent lmouse, MouseButtonEvent rmouse) {
	struct Widget w = {
	    .e = e,
	    .x = x,
	    .y = y,
	    .width = GUI_WIDTH - 100,
	    .height = GUI_HEIGHT - 100,
	    .color = 0x00000000,
	    .type = WINDOW,
	    .data = {.window = {.render = render,
	                        .lmouse = lmouse,
	                        .rmouse = rmouse}},
	    .border = {.width = 10, .height = 10},
	};
	InputRegisterMouseButtonEvent(e, INPUT_LAYER_DEFAULT, ldispatch,
	                              rdispatch);
	addWidget(e, &w);
}

/* GuiProjection sets proj to the standard GUI projection matrix. */
void GuiProjection(mat4x4 proj) {
	mat4x4_ortho(proj, 0, GUI_WIDTH, GUI_HEIGHT, 0, GUI_NEAR_PLANE,
	             GUI_FAR_PLANE);
}

/* ScreenToGui sets (x, y) to the GUI coordinates that correspond to the given
 * screen coordiantes (sx, sy). */
void ScreenToGui(int sx, int sy, int *x, int *y) {
	int w, h;
	glfwGetFramebufferSize(win, &w, &h);
	*x = (int)(sx * ((float)GUI_WIDTH / (float)w));
	*y = (int)(sy * ((float)GUI_HEIGHT / (float)h));
}

/* WindowToGui sets (x, y) to the GUI coordinates that correspond to the given
 * window coordiantes (sx, sy). */
void WindowToGui(int sx, int sy, int *x, int *y) {
	int w, h;
	glfwGetWindowSize(win, &w, &h);
	*x = (int)(sx * ((float)GUI_WIDTH / (float)w));
	*y = (int)(sy * ((float)GUI_HEIGHT / (float)h));
}

/* GetWidgetPos sets x and y to the (x, y) coordinates of the widget attached to
 * e. */
bool GetWidgetPos(Entity e, int *x, int *y) {
	struct Widget *w;

	if ((w = getWidget(e)) != NULL)
		return false;

	*x = w->x;
	*y = w->y;
	return true;
}

/* GetRelWidgetPos sets x and y to the relative coordinates of (x, y) from the
 * upper left corner of the widget attached to e. */
bool GetRelWidgetPos(Entity e, int x, int y, int *rx, int *ry) {
	struct Widget *w;

	if ((w = getWidget(e)) == NULL)
		return false;

	*rx = x - w->x - w->border.width;
	*ry = y - w->y - w->border.height;
	return true;
}

/* GetWidgetDim sets w and h to the dimensions of the widget attached to e. */
bool GetWidgetDim(Entity e, int *w, int *h) {
	struct Widget *widget;

	if ((widget = getWidget(e)) == NULL)
		return false;

	*w = widget->width;
	*h = widget->height;
	return true;
}
