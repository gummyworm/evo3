#ifndef SYS_GUI_H
#define SYS_GUI_H

#include "base.h"
#include "input.h"
#include "third-party/include/linmath.h"

#include "entity.h"

enum { GUI_MAX_CHILDREN = 32,
};

enum { GUI_WIDTH = 320,
       GUI_HEIGHT = 200,
};

typedef void (*DrawEvent)(Entity, mat4x4, int, int);

struct WidgetUpdate {
	Entity e;
};

struct Widget {
	unsigned x, y, width, height;
	struct {
		int width, height;
	} border;
	uint32_t color;
	enum { NONE,
	       TEXTBOX,
	       WINDOW,
	} type;
	union {
		struct {
			unsigned fontsize;
			const char *text;
		} textbox;
		struct {
			DrawEvent render;
			MouseButtonEvent lmouse;
			MouseButtonEvent rmouse;
		} window;

	} data;
	Entity e;
};

struct WidgetWindow {
	struct Widget w;
	struct Widget *children[GUI_MAX_CHILDREN];
};

void InitWidgetSystem(GLFWwindow *);
void UpdateWidgetSystem();
void AddWidget(Entity);
void RemoveWidget(Entity);
void AddTextBox(Entity, unsigned, unsigned, const char *);
void AddRenderWindow(Entity, int, int, DrawEvent, MouseButtonEvent,
                     MouseButtonEvent);

void GuiProjection(mat4x4);
void ScreenToGui(int, int, int *, int *);
void WindowToGui(int, int, int *, int *);
void GuiToWindow(int, int, int *, int *);

bool GetWidgetPos(Entity, int *, int *);
bool GetRelWidgetPos(Entity, int, int, int *, int *);
bool GetWidgetDim(Entity, int *, int *);

#endif
