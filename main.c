#include <glad/glad.h>

#include "linmath.h"
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <stdio.h>
#include <stdlib.h>

#include "draw.h"
#include "systems.h"

/* onError is the GLFW callback for error handling. */
static void onError(int error, const char *description) {
	fprintf(stderr, "Error (%d): %s\n", error, description);
}

/* init initializes the various systems used in the game. */
static void init(GLFWwindow *win) {
	DrawInit();
	InitInput(win);
	InitUnitSystem();
	InitSpriteSystem();
	InitTransformSystem();
	InitCommanderSystem();
	InitFPSControllerSystem();
}

/* update updates the game. */
static void update() {
	UpdateInput();
	UpdateTransformSystem();
	UpdateSpriteSystem();
}

/* test spawns test entities. */
static void test() {
	enum entities {
		E_PLAYER = 1,
		E_APPLE,
	};

	AddTransform(E_PLAYER, 0, 0, 0, 0);
	AddCamera(E_PLAYER, 0);
	CameraPerspective(E_PLAYER, 45.0f, 640.0f / 480.0f);
	AddFPSController(E_PLAYER);

	AddTransform(E_APPLE, 0, 0, 7.0f, 0);
	AddRender(E_APPLE);
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

		// Rect(window, 0, 0, 1, 1, 0xffffffff);
		TexRect(window, 0, 0, 1, 1, 0, 0, 1, 1, tex);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
