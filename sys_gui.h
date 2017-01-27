#ifndef SYS_TRANSFORM_H
#define SYS_TRANSFORM_H

#include "base.h"

#include "entity.h"

enum { MAX_WIDGETS = 8192,
       GUI_MAX_CHILDREN = 32,
};

struct WidgetUpdate {
	Entity e;
};

struct Widget {
	unsigned x, y, width, height;
	uint32_t color;
	Entity e;
};

struct WidgetWindow {
	struct Widget w;
	struct Widget *children[GUI_MAX_CHILDREN];
};

void InitWidgetSystem(GLFWwindow *);
void UpdateWidgetSystem();
void AddWidget(Entity);

#endif
