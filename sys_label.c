#include "sys_label.h"
#include "draw.h"
#include "entities.h"
#include "systems.h"
#include "third-party/include/uthash.h"

struct entityToLabel {
	Entity e;
	struct Label *label;
	UT_hash_handle hh;
};

static struct entityToLabel *entitiesToLabels;
static struct Label labels[MAX_LABELS];
static int numLabels;

/* getLabel returns the label attached to entity e (if there is one). */
static struct Label *getLabel(Entity e) {
	struct entityToLabel *t;

	if (entitiesToLabels == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToLabels, &e, t);
	if (t == NULL)
		return NULL;

	return t->label;
}

/* InitLabelSystem initializes the label system. */
void InitLabelSystem() {}

/* drawLabel renders the given label. */
void drawLabel(struct Label *l) {
	int sx, sy;
	int i;
	int size;
	vec3 lpos;
	vec3 ppos;
	vec3 dist;

	if (!Enabled(l->e))
		return;
	if (!GetPos(l->e, lpos))
		return;

	lpos[1] += l->offset;

	WorldToScreen(E_PLAYER, lpos[0], lpos[1], lpos[2], &sx, &sy);

	GetViewPos(E_PLAYER, &ppos[0], &ppos[1], &ppos[2]);
	vec3_sub(dist, lpos, ppos);

	size = 10.f / vec3_len(dist) * LABEL_MAX_FONT_SIZE;
	if (size > LABEL_MAX_FONT_SIZE)
		size = LABEL_MAX_FONT_SIZE;

	if (sx < 0 || sy < 0)
		return;

	mat4x4 proj;
	ScreenToGui(sx, sy, &sx, &sy);
	GuiProjection(proj);

	for (i = 0; i < l->numContent; ++i) {
		struct LabelContent *c = l->content + i;
		switch (c->type) {
		case LABEL_TEXT:
			Text(proj, sx, sy, size, c->data.text);
			break;
		case LABEL_RECT:
			Rect(proj, sx, sy, c->data.rect.w, c->data.rect.h,
			     c->data.rect.color);
		}
		sy += size;
	}
}

/* UpdateLabelSystem updates all labels that have been created. */
void UpdateLabelSystem() {
	int i;

	for (i = 0; i < numLabels; ++i) {
		drawLabel(labels + i);
	}
}

/* AddLabel adds a label component to the entity e. */
void AddLabel(Entity e, const char *text, float offset) {
	struct entityToLabel *item;
	struct Label *l;

	if ((l = getLabel(e)) != NULL) {
		l->content[0] = (struct LabelContent){.type = LABEL_TEXT,
		                                      .data = {.text = text}};
		l->offset = offset;
		labels[numLabels].numContent = 1;
		return;
	}

	item = malloc(sizeof(struct entityToLabel));
	item->label = labels + numLabels;
	item->e = e;

	labels[numLabels].e = e;
	labels[numLabels].offset = offset;
	labels[numLabels].content[0] =
	    (struct LabelContent){.type = LABEL_TEXT, .data = {.text = text}};
	labels[numLabels].numContent = 1;

	HASH_ADD_INT(entitiesToLabels, e, item);
	numLabels++;
}

/* RemoveLabel removes the label attached to e from the Label system. */
void RemoveLabel(Entity e) {
	struct entityToLabel *c;

	if (entitiesToLabels == NULL)
		return;

	HASH_FIND_INT(entitiesToLabels, &e, c);
	if (c != NULL) {
		struct Label *sys = c->label;
		int sz = (labels + numLabels) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToLabels, c);
		free(c);
		numLabels--;
	}
}

/* AddLabelRect adds a rectangle to the Label content of the label attached to
 * e. */
void AddLabelRect(Entity e, int w, int h, uint32_t color) {
	struct Label *l;

	if ((l = getLabel(e)) == NULL)
		return;

	l->content[l->numContent].type = LABEL_RECT;
	l->content[l->numContent].data.rect.w = w;
	l->content[l->numContent].data.rect.h = h;
	l->content[l->numContent].data.rect.color = color;

	l->numContent++;
}
