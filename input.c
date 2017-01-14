#include "input.h"
#include "base.h"

static bool mouseMoved;

static int keysPressed[256];
static int numKeysPressed;
static double mouseX, mouseY;

/* key is the GLFW callback to handle key events. */
void key(GLFWwindow *window, int key, int scancode, int action, int mods) {
	UNUSED(window);
	UNUSED(mods);
	UNUSED(key);

	if (action == GLFW_PRESS) {
		keysPressed[numKeysPressed] = scancode;
		numKeysPressed++;
	}
}

/* mouseMove is the GLFW callback to handle mouse movement. */
void mouseMove(GLFWwindow *window, double x, double y) {
	UNUSED(window);

	mouseMoved = true;
	mouseX = x;
	mouseY = y;
}

/* InitInput initializes the engine for input related events. */
void InitInput(GLFWwindow *win) {
	glfwSetKeyCallback(win, key);
	glfwSetCursorPosCallback(win, mouseMove);
}

/* UpdateInput updates input related state. */
void UpdateInput() { numKeysPressed = 0; }

/* InputGetUpdates returns any updates that have occurred since the last update
 * to the input system. */
struct InputUpdate *InputGetUpdates() {
	static struct InputUpdate u;

	u = (struct InputUpdate){mouseMoved,     keysPressed > 0, keysPressed,
	                         numKeysPressed, mouseX,          mouseY};
	return &u;
}

/* InputKeyPressed returns the state of the given key. */
bool InputKeyPressed(int scancode) {
	int i;
	for (i = 0; i < numKeysPressed; ++i) {
		if (keysPressed[i] == scancode)
			return true;
	}

	return false;
}

/* InputGetMouse sets x and y to the current mouse coordinates. */
void InputGetMouse(double *x, double *y) {
	*x = mouseX;
	*y = mouseY;
}
