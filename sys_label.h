#ifndef SYS_LABEL_H
#define SYS_LABEL_H

#include "entity.h"
#include "thing.h"

enum { MAX_LABELS = 8192,
       LABEL_MAX_FONT_SIZE = 16,
};

/* LabelUpdate describes updates to labels. */
struct LabelUpdate {
	Entity e;
};

/* Label is a component that is used to label objects in the world. */
struct Label {
	Entity e;
	float offset;
	const char *text;
};

void InitLabelSystem();
void AddLabel(Entity, const char *, float);
void RemoveLabel(Entity);
void UpdateLabelSystem();

#endif
