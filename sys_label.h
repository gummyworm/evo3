#ifndef SYS_LABEL_H
#define SYS_LABEL_H

#include "entity.h"
#include "thing.h"

enum { MAX_LABELS = 8192,
       LABEL_MAX_FONT_SIZE = 8,
       LABEL_MAX_CONTENT = 16,
};

enum { LABEL_TEXT,
       LABEL_RECT,
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
	struct LabelContent {
		int type;
		union {
			const char *text;
			struct {
				int w, h;
				uint32_t color;
			} rect;
		} data;
	} content[LABEL_MAX_CONTENT];
	int numContent;
};

void InitLabelSystem();
void AddLabel(Entity, const char *, float);
void RemoveLabel(Entity);
void UpdateLabelSystem();
void AddLabelRect(Entity, int, int, uint32_t);

#endif
