#ifndef DRAW_H
#define DRAW_H

#include <GLFW/glfw3.h>
#include <stdint.h>

void DrawInit();
void Rect(GLFWwindow *, unsigned, unsigned, unsigned, unsigned, uint32_t);
void TexRect(GLFWwindow *, unsigned, unsigned, unsigned, GLuint, uint32_t,
	     float, float, float, float);
GLuint GetTexture(const char *filename);

#endif
