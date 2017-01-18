#ifndef DRAW_H
#define DRAW_H

#include "linmath.h"
#include <GLFW/glfw3.h>
#include <stdint.h>

void DrawInit();
void Rect(GLFWwindow *, mat4x4, unsigned, unsigned, unsigned, unsigned,
          uint32_t);
void TexRect(GLFWwindow *, unsigned, unsigned, unsigned, unsigned, float, float,
             float, float, GLuint);
GLuint GetTexture(const char *filename);

#endif
