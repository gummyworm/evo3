#include "base.h"

#include "debug.h"
#include "draw.h"
#include "systems.h"
#include <SOIL.h>
#include <stdio.h>
#include <stdlib.h>

#include "entities.h"

/* onError is the GLFW callback for error handling. */
static void onError(int error, const char *description) {
	fprintf(stderr, "Error (%d): %s\n", error, description);
}

/* init initializes the various systems used in the game. */
static void init(GLFWwindow *win) {
	srand(time(NULL));

	DrawInit();
	InitInput(win);

	InitUnitSystem();
	InitSpriteSystem();
	InitTimeSystem();
	InitTransformSystem();
	InitCommanderSystem();
	InitFPSControllerSystem();
	InitConsoleSystem();

	InitWidgetSystem(win);
	InitCameraSystem(win);
}

/* update updates the game. */
static void update() {
	/* pre render updates */
	UpdateTransformSystem();
	UpdateInput();
	UpdateFPSControllerSystem();

	/* render */
	glEnable(GL_DEPTH_TEST);
	UpdateCameraSystem();
	glEnable(GL_BLEND);
	UpdateSpriteSystem();

	/* post main render updates */
	glDisable(GL_DEPTH_TEST);
	UpdateConsoleSystem();
	UpdateWidgetSystem();
	UpdateLabelSystem();
}

/* test spawns test entities. */
extern void Player(int);
extern void TestRoom(int);
static void test() {
	// TestRoom(E_TEST_ROOM);
	GenerateRoom(E_TEST_ROOM, CANOPY);
	Player(E_PLAYER);

	AddTransform(E_APPLE, 0, 0, -7.0f);
	AddRender(E_APPLE, "person.obj");
	AddLabel(E_APPLE, "apple", 1.0f);
	AddContainer(E_APPLE, "APPLE", "It's a juicy red apple");

	AddTransform(E_ORANGE, 1, 0, -6.0f);
	AddThing(E_ORANGE, "ORANGE", "It's a bright citrus fruit");
	AddRender(E_ORANGE, "person.obj");
	AddLabel(E_ORANGE, "orange", 1.0f);
	AddToContainer(E_APPLE, E_ORANGE);
}

/* ClearUpdates resets all systems' updates. */
void ClearUpdates() { numTransformUpdates = 0; }

int main() {
	int frame;
	float t;
	GLFWwindow *window;
	mat4x4 proj;

	glfwSetErrorCallback(onError);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(640, 480, "game", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	init(window);
	test();

	t = GetTime();
	while (!glfwWindowShouldClose(window)) {
		int width, height;

		if (GetTime() - t > 1.0f) {
			dinfof("fps ~%d", frame);
			frame = 0;
			t = GetTime();
		}

		glfwGetFramebufferSize(window, &width, &height);
		mat4x4_ortho(proj, 0, width, height, 0, 1.f, -1.f);
		glViewport(0, 0, width, height);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		update();
		glfwSwapBuffers(window);

		ClearUpdates();
		glfwPollEvents();
		frame++;
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
