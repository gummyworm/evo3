#ifndef SHADERS_H
#define SHADERS_H

#include "base.h"
#include "third-party/include/linmath.h"
#include <stdbool.h>

enum { COLOR_PROGRAM,
       TEXTURE_PROGRAM,
       BAYER_PROGRAM,
       SHADOW_PROGRAM,
       NUM_PROGRAMS,
};

/* Program holds handles for a shaderand every possible uniform it may have. */
struct Program {
	GLint program;
	GLint mvp;
	GLint tex0;
	GLint color;
};

bool InitShaders();
bool UseProgram(int);
GLint GetProgram(int);
GLint GetUMVP(int);
bool SetUMVP(int, mat4x4);
GLint GetUTex(int);
bool SetUTex(int, int);
GLint GetUColor(int);
bool SetUColor(int, float, float, float, float);

#endif
