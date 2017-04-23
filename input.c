#include "base.h"
#include "systems.h"

static bool mouseMoved;

static int keysPressed[256];
static int numKeysPressed;
static double mouseX, mouseY;
static uint32_t enabled;

enum { MAX_KEY_CALLBACKS = 16,
       MAX_MOUSE_CALLBACKS = 16,
};

struct entityToInput {
	Entity e;
	struct Input *input;
	UT_hash_handle hh;
};

static struct entityToInput *entitiesToInputs;
static struct Input inputs[MAX_INPUTS];
static int numInputs;

int numInputUpdates;
static struct InputUpdate updates[MAX_INPUTS];

/* keyCallbacks contains the callbacks executed upon keyboard events. */
static struct {
	Entity e;
	KeyEvent key;
	uint32_t layer;
} keyCallbacks[MAX_KEY_CALLBACKS];
static int numKeyCallbacks;

/* mouseCallbacks contains the callbacks to be executed on mouse events. */
struct {
	Entity e;
	MouseMotionEvent moved;
	uint32_t layer;
} mouseCallbacks[MAX_MOUSE_CALLBACKS];
static int numMouseCallbacks;

/* mouseButtonCallbacks contains the callbacks to be executed on mouse button
 * events. */
struct {
	Entity e;
	MouseButtonEvent left;
	MouseButtonEvent right;
	uint32_t layer;
} mouseButtonCallbacks[MAX_MOUSE_CALLBACKS];
static int numMouseButtonCallbacks;

/* mouseScrollCallbacks contains the callbacks to be executed on mouse scroll
 * events. */
struct {
	Entity e;
	MouseScrollEvent scroll;
	uint32_t layer;
} mouseScrollCallbacks[MAX_MOUSE_CALLBACKS];
static int numMouseScrollCallbacks;

/* getInput returns the input attached to entity e (if there is one). */
static struct Input *getInput(Entity e) {
	struct entityToInput *t;

	if (entitiesToInputs == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToInputs, &e, t);
	if (t == NULL)
		return NULL;

	return t->input;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct InputUpdate *u) {
	updates[numInputUpdates++] = *u;
}

/* InitInputSystem initializes the input system. */
void InitInputSystem() {}

/* UpdateInputSystem updates all inputs that have been created. */
void UpdateInputSystem() { numKeysPressed = 0; }

void TransformSystemClearUpdates() { numInputUpdates = 0; }

/* AddInput adds a input component to the entity e. */
void AddInput(Entity e) {
	struct entityToInput *item;

	if (getInput(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToInput));
	item->input = inputs + numInputs;
	item->e = e;

	inputs[numInputs].e = e;

	HASH_ADD_INT(entitiesToInputs, e, item);
	numInputs++;
}

/* RemoveInput removes the input attached to e from the Input
 * system. */
void RemoveInput(Entity e) {
	struct entityToInput *c;

	if (entitiesToInputs == NULL)
		return;

	HASH_FIND_INT(entitiesToInputs, &e, c);
	if (c != NULL) {
		struct Input *sys = c->input;
		int sz = (inputs + numInputs) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToInputs, c);
		free(c);
		numInputs--;
	}
}

/* GetInputUpdate returns any input updates for the entity e. */
struct InputUpdate *GetInputUpdate(Entity e) {}

/* GetInputUpdates returns the input updates this frame. */
struct InputUpdate *GetInputUpdates(int *num) {
	*num = numInputUpdates;
	return updates;
}
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
		if ((keyCallbacks[i].layer & enabled) != 0) {
			Entity e = keyCallbacks[i].e;
			keyCallbacks[i].key(e, key, scancode, action, mods);
		}
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
		if ((mouseCallbacks[i].layer & enabled) != 0) {
			Entity e = mouseCallbacks[i].e;
			mouseCallbacks[i].moved(e, x, y);
		}
	}
}

/* mouseButton is the GLFW callback to handle mouse button events. */
void mouseButton(GLFWwindow *window, int button, int action, int mods) {
	UNUSED(window);
	UNUSED(mods);
	int i;

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		for (i = 0; i < numMouseButtonCallbacks; ++i) {
			if ((mouseButtonCallbacks[i].layer & enabled) != 0) {
				if (mouseButtonCallbacks[i].left) {
					Entity e = mouseButtonCallbacks[i].e;
					mouseButtonCallbacks[i].left(e, action);
				}
			}
		}
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		for (i = 0; i < numMouseButtonCallbacks; ++i) {
			if ((mouseButtonCallbacks[i].layer & enabled) != 0) {
				if (mouseButtonCallbacks[i].right) {
					Entity e = mouseButtonCallbacks[i].e;
					mouseButtonCallbacks[i].right(e,
					                              action);
				}
			}
		}
	}
}

/* mouseScroll is the GLFW callback to handle mouse scrolling. */
static void mouseScroll(GLFWwindow *window, double x, double y) {
	UNUSED(window);
	int i;

	for (i = 0; i < numMouseScrollCallbacks; ++i) {
		if ((mouseScrollCallbacks[i].layer & enabled) != 0) {
			Entity e = mouseScrollCallbacks[i].e;
			mouseScrollCallbacks[i].scroll(e, x, y);
		}
	}
}

/* InitInput initializes the engine for input related events. */
void InitInput(GLFWwindow *win) {
	enabled = 0xffffffff;
	numKeyCallbacks = 0;
	numMouseCallbacks = 0;
	numMouseButtonCallbacks = 0;
	glfwSetKeyCallback(win, key);
	glfwSetCursorPosCallback(win, mouseMove);
	glfwSetMouseButtonCallback(win, mouseButton);
	glfwSetScrollCallback(win, mouseScroll);
}

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
void InputRegisterKeyEvent(Entity e, uint32_t layer, KeyEvent evt) {
	keyCallbacks[numKeyCallbacks].e = e;
	keyCallbacks[numKeyCallbacks].layer = layer;
	keyCallbacks[numKeyCallbacks].key = evt;
	numKeyCallbacks++;
}

/* InputRegisterMouseEvent registers the given mouse motion callback. */
void InputRegisterMouseEvent(Entity e, uint32_t layer, MouseMotionEvent move) {
	mouseCallbacks[numMouseCallbacks].e = e;
	mouseCallbacks[numMouseCallbacks].layer = layer;
	mouseCallbacks[numMouseCallbacks].moved = move;
	numMouseCallbacks++;
}

/* InputRegisterMouseButtonEvent registers the given mouse button callbacks. */
void InputRegisterMouseButtonEvent(Entity e, uint32_t layer, MouseButtonEvent l,
                                   MouseButtonEvent r) {
	mouseButtonCallbacks[numMouseButtonCallbacks].e = e;
	mouseButtonCallbacks[numMouseButtonCallbacks].layer = layer;
	mouseButtonCallbacks[numMouseButtonCallbacks].left = l;
	mouseButtonCallbacks[numMouseButtonCallbacks].right = r;
	numMouseButtonCallbacks++;
}

/* InputRegisterMouseScrollEvent registers the given mouse scroll callback. */
void InputRegisterMouseScrollEvent(Entity e, uint32_t layer,
                                   MouseScrollEvent s) {
	mouseScrollCallbacks[numMouseScrollCallbacks].e = e;
	mouseScrollCallbacks[numMouseScrollCallbacks].layer = layer;
	mouseScrollCallbacks[numMouseScrollCallbacks].scroll = s;
	numMouseScrollCallbacks++;
}
