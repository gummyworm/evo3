#ifndef DRAW_H
#define DRAW_H

#include "base.h"

#include "shaders.h"
#include "third-party/include/linmath.h"
#include <stdbool.h>
#include <stdint.h>

void DrawInit();
void Rect(mat4x4, unsigned, unsigned, unsigned, unsigned, uint32_t);
void TexRect(mat4x4, GLint, float, float, float, float, float, float, float,
             float, GLuint);
void TexRectZ(mat4x4, GLint, float, float, float, float, float, float, float,
              float, float, GLuint);
void TexRectZRot(mat4x4, GLint, float, float, float, float, float, float, float,
                 float, float, float, GLuint);
void Text(mat4x4, int, int, unsigned, const char *);
GLuint GetTexture(const char *filename);
GLint makeProgram(GLchar const *, GLchar const *);

#endif
