#ifndef INPUT_H
#define INPUT_H

#include "base.h"
#include "entity.h"
#include <stdbool.h>

enum InputLayer {
	INPUT_LAYER_DEFAULT = 1 << 0,
	INPUT_LAYER_CONSOLE = 1 << 1,
	INPUT_LAYER_WORLD = 1 << 2,
};

enum { MAX_INPUTS = 1024,
};

typedef void (*KeyEvent)(Entity, int key, int scancode, int action, int mods);
typedef void (*MouseButtonEvent)(Entity, int action);
typedef void (*MouseMotionEvent)(Entity, double x, double y);

/* Input is a struct that represents a receiver of user input. */
struct Input {
	Entity e;
};

struct InputUpdate {
	bool mouseMoved;
	bool keyPressed;

	int *keysPressed;
	int numKeysPressed;

	double mouseX, mouseY;
};

void InitInput(GLFWwindow *);
void UpdateInputSystem();
struct InputUpdate *InputGetUpdates();
void AddInput(Entity);
void RemoveTransform(Entity);
void TransformSystemClearUpdates();
struct TransformUpdate *GetTransformUpdates(int *);
struct TransformUpdate *GetTransformUpdate(Entity);

bool InputKeyPressed(int scancode);
void InputGetMouse(double *, double *);
void InputEnable(uint32_t layers);
void InputDisable(uint32_t layers);

void InputRegisterKeyEvent(Entity, uint32_t, KeyEvent);
void InputRegisterMouseEvent(Entity, uint32_t, MouseMotionEvent);
void InputRegisterMouseButtonEvent(Entity, uint32_t, MouseButtonEvent,
                                   MouseButtonEvent);

#endif
