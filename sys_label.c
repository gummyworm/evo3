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

int numLabelUpdates;
static struct LabelUpdate updates[MAX_LABELS];

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

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct LabelUpdate *u) {
	updates[numLabelUpdates++] = *u;
}

/* InitLabelSystem initializes the label system. */
void InitLabelSystem() {}

/* UpdateLabelSystem updates all labels that have been created. */
void UpdateLabelSystem() {
	int i;
	int sx, sy;
	for (i = 0; i < numLabels; ++i) {
		float x, y, z;

		if (!Enabled(labels[i].e))
			continue;

		if (!GetPos(labels[i].e, &x, &y, &z))
			continue;

		y += labels[i].offset;

		WorldToScreen(E_PLAYER, x, y, z, &sx, &sy);
		if (sx >= 0) {
			mat4x4 proj;
			ScreenToGui(sx, sy, &sx, &sy);
			GuiProjection(&proj);
			Text(proj, sx, sy, LABEL_FONT_SIZE, labels[i].text);
		}
	}
}

/* AddLabel adds a label component to the entity e. */
void AddLabel(Entity e, const char *text, float offset) {
	struct entityToLabel *item;

	if (getLabel(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToLabel));
	item->label = labels + numLabels;
	item->e = e;

	labels[numLabels].e = e;
	labels[numLabels].text = text;
	labels[numLabels].offset = offset;

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
	}
}
