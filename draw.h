#ifndef DRAW_H
#define DRAW_H

#include "third-party/include/linmath.h"
#include <GLFW/glfw3.h>
#include <stdint.h>

void DrawInit();
void Rect(mat4x4, unsigned, unsigned, unsigned, unsigned, uint32_t);
void TexRect(mat4x4, GLint, unsigned, unsigned, unsigned, unsigned, float,
             float, float, float, GLuint);
GLuint GetTexture(const char *filename);

GLint makeProgram(GLchar const *, GLchar const *);
GLint getTextureProgram();
GLint getColorProgram();
GLint getBayerProgram();

#endif
