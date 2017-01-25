#ifndef SYS_TRANSFORM_H
#define SYS_TRANSFORM_H

#include "base.h"

#include "entity.h"

enum { MAX_WIDGETS = 8192,
};

struct WidgetUpdate {
	Entity e;
};

struct Widget {
	Entity e;
};

void InitWidgetSystem(GLFWwindow *);
void UpdateWidgetSystem();
void AddWidget(Entity e);

#endif
