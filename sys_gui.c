#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION

#include "base.h"

#include "sys_gui.h"
#include "third-party/include/linmath.h"
#include "third-party/include/nuklear.h"
#include "third-party/include/nuklear_glfw_gl3.h"
#include "third-party/include/uthash.h"
#include <stdio.h>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

enum { GUI_WIDTH = 320,
       GUI_HEIGHT = 200,
};

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

static struct nk_context *ctx;
static struct nk_color background;

static void DrawWidget(struct Widget *);

static mat4x4 proj;

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

/* InitWidgetSystem initializes the widget system. */
void InitWidgetSystem(GLFWwindow *win) {
	UNUSED(win);
	mat4x4_ortho(proj, 0, GUI_WIDTH, GUI_HEIGHT, 0, 1.f, -1.f);
}

/* UpdateWidgetSystem updates all widgets that have been created. */
void UpdateWidgetSystem() {
	int i;

	for (i = 0; i < numWidgets; ++i) {
		struct Widget *w;
		w = widgets + i;
		DrawWidget(w);
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
	UNUSED(w);
	Rect(proj, w->x, w->y, w->width, w->height, w->color);
}
