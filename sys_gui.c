#include "base.h"
#include <string.h>

#include "draw.h"

#include "sys_gui.h"
#include "third-party/include/linmath.h"
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

static void DrawWidget(struct Widget *);

static mat4x4 proj;

/* drawTextbox renders the textbox w. */
static void drawTextbox(struct Widget *w) {
	mat4x4_identity(proj);
	Text(proj, w->x, w->y, w->data.textbox.fontsize, w->data.textbox.text);
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

		switch (w->type) {
		case TEXTBOX:
			drawTextbox(w);
		case NONE:
		default:
			break;
		}
	}

	numUpdates = 0;
}

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

/* AddTextBox adds a TextBox widget to entity e. */
void AddTextBox(Entity e, unsigned x, unsigned y, const char *text) {
	struct Widget w = {.x = x,
	                   .y = y,
	                   .width = 100,
	                   .height = 32,
	                   .color = 0x00000000,
	                   .type = TEXTBOX,
	                   .data = {.textbox = {.text = text, .fontsize = 32}}};
	addWidget(e, &w);
}
