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
	for (i = 0; i < numLabels; ++i) {
		int sx, sy;
		int size;
		vec3 lpos;
		vec3 ppos;
		vec3 dist;

		if (!Enabled(labels[i].e))
			continue;

		if (!GetPos(labels[i].e, &lpos[0], &lpos[1], &lpos[2]))
			continue;

		lpos[1] += labels[i].offset;

		WorldToScreen(E_PLAYER, lpos[0], lpos[1], lpos[2], &sx, &sy);

		GetPos(E_PLAYER, &ppos[0], &ppos[1], &ppos[2]);
		vec3_sub(dist, lpos, ppos);
		size = 10.f / vec3_len(dist) * LABEL_FONT_SIZE;

		if (sx >= 0) {
			mat4x4 proj;
			ScreenToGui(sx, sy, &sx, &sy);
			GuiProjection(&proj);
			Text(proj, sx, sy, size, labels[i].text);
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
