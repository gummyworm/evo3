#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <stdbool.h>

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

#endif
