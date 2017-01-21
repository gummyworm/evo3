#include <glad/glad.h>

#include "linmath.h"
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <stdio.h>
#include <stdlib.h>

#include "draw.h"
#include "sys_fps_controller.h"
#include "systems.h"

enum testEntities {
	E_PLAYER = 1,
	E_APPLE,
};

/* onError is the GLFW callback for error handling. */
static void onError(int error, const char *description) {
	fprintf(stderr, "Error (%d): %s\n", error, description);
}

/* init initializes the various systems used in the game. */
static void init(GLFWwindow *win) {
	DrawInit();
	InitInput(win);
	InitCameraSystem(win);
	InitUnitSystem();
	InitSpriteSystem();
	InitTimeSystem();
	InitTransformSystem();
	InitCommanderSystem();
	InitFPSControllerSystem();
}

/* update updates the game. */
static void update() {
	UpdateInput();
	UpdateTransformSystem();
	UpdateSpriteSystem();
	UpdateCameraSystem();
	UpdateFPSControllerSystem();
}

/* test spawns test entities. */
extern void Player(int);
static void test() {
	Player(E_PLAYER);

	AddTransform(E_APPLE, 0, 0, -7.0f);
	AddRender(E_APPLE, "person.obj");
}

int main() {
	GLFWwindow *window;
	glfwSetErrorCallback(onError);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	init(window);
	test();
	GLuint tex = GetTexture("test.png");

	while (!glfwWindowShouldClose(window)) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// Rect(window, NULL, 0, 0, 1, 1, 0xffffffff);
		// TexRect(window, NULL, 0, 0, 1, 1, 0, 0, 1, 1, tex);
		update();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
