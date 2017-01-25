#include "base.h"

#include "input.h"

static bool mouseMoved;

static int keysPressed[256];
static int numKeysPressed;
static double mouseX, mouseY;
static uint32_t enabled;

enum { MAX_KEY_CALLBACKS = 16,
       MAX_MOUSE_CALLBACKS = 16,
};

/* keyCallbacks contains the callbacks executed upon keyboard events. */
static struct {
	void (*key)(int, int, int, int);
	uint32_t layer;
} keyCallbacks[MAX_KEY_CALLBACKS];
static int numKeyCallbacks;

/* mouseCallbacks contains the callbacks to be executed on mouse events. */
struct {
	void (*moved)(double, double);
	uint32_t layer;
} mouseCallbacks[MAX_MOUSE_CALLBACKS];
static int numMouseCallbacks;

/* key is the GLFW callback to handle key events. */
void key(GLFWwindow *window, int key, int scancode, int action, int mods) {
	UNUSED(window);
	UNUSED(mods);
	UNUSED(key);
	int i;

	if (action == GLFW_PRESS) {
		keysPressed[numKeysPressed] = scancode;
		numKeysPressed++;
	}

	for (i = 0; i < numKeyCallbacks; ++i) {
		if ((keyCallbacks[i].layer & enabled) != 0)
			keyCallbacks[i].key(key, scancode, action, mods);
	}
}

/* mouseMove is the GLFW callback to handle mouse movement. */
void mouseMove(GLFWwindow *window, double x, double y) {
	UNUSED(window);
	int i;

	mouseMoved = true;
	mouseX = x;
	mouseY = y;

	for (i = 0; i < numMouseCallbacks; ++i) {
		if ((mouseCallbacks[i].layer & enabled) != 0)
			mouseCallbacks[i].moved(x, y);
	}
}

/* InitInput initializes the engine for input related events. */
void InitInput(GLFWwindow *win) {
	enabled = 0xffffffff;
	numKeyCallbacks = 0;
	numMouseCallbacks = 0;
	glfwSetKeyCallback(win, key);
	glfwSetCursorPosCallback(win, mouseMove);
}

/* UpdateInput updates input related state. */
void UpdateInput() { numKeysPressed = 0; }

/* InputGetUpdates returns any updates that have occurred since the last
 * update
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

/* InputEnable enables the given input layers. */
void InputEnable(uint32_t layers) { enabled |= layers; }

/* InputDisable disables the given input layers. */
void InputDisable(uint32_t layers) { enabled &= ~(layers); }

/* InputRegisterKeyEvent registers the given keyboard event callback. */
void InputRegisterKeyEvent(uint32_t layer,
                           void (*callback)(int, int, int, int)) {
	keyCallbacks[numKeyCallbacks].layer = layer;
	keyCallbacks[numKeyCallbacks].key = callback;
	numKeyCallbacks++;
}

/* InputRegisterMouseEvent registers the given mouse motion callback. */
void InputRegisterMouseEvent(uint32_t layer, void (*move)(double, double)) {
	mouseCallbacks[numMouseCallbacks].layer = layer;
	mouseCallbacks[numMouseCallbacks].moved = move;
	numMouseCallbacks++;
}
