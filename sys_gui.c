#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION

#include "sys_gui.h"
#include <stdio.h>
#include "base.h"
#include "third-party/include/nuklear.h"
#include "third-party/include/nuklear_glfw_gl3.h"
#include "third-party/include/uthash.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

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

/* getWidget returns the widget attached to entity e (if there is one). */
static struct Widget *getWidget(Entity e) {
	struct entityToWidget *t;

	if (entitiesToWidgets == NULL) return NULL;

	HASH_FIND_INT(entitiesToWidgets, &e, t);
	if (t == NULL) return NULL;

	return t->widget;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct WidgetUpdate *u) { updates[numUpdates++] = *u; }

/* InitWidgetSystem initializes the widget system. */
void InitWidgetSystem(GLFWwindow *win) {
	ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
}

/* UpdateWidgetSystem updates all widgets that have been created. */
void UpdateWidgetSystem() {
	int i;
	for (i = 0; i < numWidgets; ++i) {
		struct Widget *w;

		w = widgets + i;
		DrawWidget(w);
	}
	nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER,
			MAX_ELEMENT_BUFFER);
	numUpdates = 0;
}

/* AddWidget adds a widget component to the entity e. */
void AddWidget(Entity e) {
	struct entityToWidget *item;

	if (getWidget(e) != NULL) return;
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
	if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
		     NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			 NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
		enum { EASY, HARD };
		static int op = EASY;
		static int property = 20;
		nk_layout_row_static(ctx, 30, 80, 1);
		if (nk_button_label(ctx, "button"))
			fprintf(stdout, "button pressed\n");

		nk_layout_row_dynamic(ctx, 30, 2);
		if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
		if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

		nk_layout_row_dynamic(ctx, 25, 1);
		nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

		nk_layout_row_dynamic(ctx, 20, 1);
		nk_label(ctx, "background:", NK_TEXT_LEFT);
		nk_layout_row_dynamic(ctx, 25, 1);
		if (nk_combo_begin_color(ctx, background,
					 nk_vec2(nk_widget_width(ctx), 400))) {
			nk_layout_row_dynamic(ctx, 120, 1);
			background = nk_color_picker(ctx, background, NK_RGBA);
			nk_layout_row_dynamic(ctx, 25, 1);
			background.r = (nk_byte)nk_propertyi(
			    ctx, "#R:", 0, background.r, 255, 1, 1);
			background.g = (nk_byte)nk_propertyi(
			    ctx, "#G:", 0, background.g, 255, 1, 1);
			background.b = (nk_byte)nk_propertyi(
			    ctx, "#B:", 0, background.b, 255, 1, 1);
			background.a = (nk_byte)nk_propertyi(
			    ctx, "#A:", 0, background.a, 255, 1, 1);
			nk_combo_end(ctx);
		}
	}
	nk_end(ctx);
}
