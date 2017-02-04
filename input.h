#ifndef INPUT_H
#define INPUT_H

#include "base.h"

#include <stdbool.h>

enum InputLayer {
	INPUT_LAYER_DEFAULT = 1 << 0,
	INPUT_LAYER_CONSOLE = 1 << 1,
	INPUT_LAYER_WORLD = 1 << 2,
};

struct InputUpdate {
	bool mouseMoved;
	bool keyPressed;

	int *keysPressed;
	int numKeysPressed;

	double mouseX, mouseY;
};

void InitInput(GLFWwindow *);
void UpdateInput();
struct InputUpdate *InputGetUpdates();

bool InputKeyPressed(int scancode);
void InputGetMouse(double *, double *);
void InputEnable(uint32_t layers);
void InputDisable(uint32_t layers);

void InputRegisterKeyEvent(uint32_t, void (*)(int, int, int, int));
void InputRegisterMouseEvent(uint32_t, void (*)(double, double));
void InputRegisterMouseButtonEvent(uint32_t, void (*)(int), void (*)(int));

#endif
