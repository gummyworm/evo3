#include "shaders.h"
#include <stdlib.h>
#include <string.h>

static struct Program programs[NUM_PROGRAMS];

/* vertexShaderC is the vertex shader used for rendering with color. */
static GLchar const *vertexShaderC = "#version 150\n"
                                     "in vec4 vPos;\n"
                                     "in vec4 vCol;\n"
                                     "out vec4 oCol;\n"
                                     "uniform mat4 MVP;\n"
                                     "void main( void ) {"
                                     "    gl_Position = MVP * vPos;\n"
                                     "    oCol = vCol;\n"
                                     "}\n";

/* fragmentShaderC is the fragment shader used for rendering. */
static GLchar const *fragmentShaderC = "#version 150\n"
                                       "in vec4 oCol ;\n"
                                       "out vec4 out_color;\n"
                                       "void main()\n"
                                       "{\n"
                                       "  out_color = oCol;\n"
                                       "}\n";

/* vertexShaderT is the vertex shader used for rendering textured items. */
static GLchar const *vertexShaderT = "#version 150\n"
                                     "in vec4 vPos;\n"
                                     "in vec2 vTexco;\n"
                                     "out vec2 out_texco;\n"
                                     "uniform mat4 MVP;\n"
                                     "void main( void ) {"
                                     "    gl_Position = MVP * vPos;\n"
                                     "    out_texco = vTexco;\n"
                                     "}\n";

/* fragmentShaderT is the fragment shader used for rendering textured items. */
static GLchar const *fragmentShaderT =
    "#version 150\n"
    "in vec2 out_texco;\n"
    "out vec4 out_color;\n"
    "uniform sampler2D tex0;\n"
    "uniform vec4 color;\n"
    "void main()\n"
    "{\n"
    "  out_color = vec4(texture(tex0, out_texco)) + color;\n"
    "}\n";

/* vertexShaderShadow is the vertex shader used for rendering shadows. */
static GLchar const *vertexShaderShadow = "#version 150\n"
                                          "in vec4 vPos;\n"
                                          "in vec2 vTexco;\n"
                                          "out vec2 out_texco;\n"
                                          "uniform mat4 MVP;\n"
                                          "void main( void ) {"
                                          "    gl_Position = MVP * vPos;\n"
                                          "    out_texco = vTexco;\n"
                                          "}\n";

/* fragmentShaderShadow is the fragment shader used for rendering shadows . */
static GLchar const *fragmentShaderShadow =
    "#version 150\n"
    "in vec2 out_texco;\n"
    "out vec4 out_color;\n"
    "uniform sampler2D tex0;\n"
    "void main()\n"
    "{\n"
    "  out_color = mix(vec4(0,0,0,0), texture(tex0, out_texco).aaaa, .5);\n"
    "}\n";

/* bayerFrag is the bayer dither fragment shader source. */
static GLchar const *bayerFrag =
    "#version 150\n"
    "uniform sampler2D tex0;\n"
    "float Scale = 1.0;\n"
    "in vec2 out_texco;\n"
    "out vec4 oColor;\n"
    "float find_closest(int x, int y, float c0)\n"
    "{\n"
    "int dither[64] = int[64](0, 32, 8, 40, 2, 34, 10, 42,\n"
    "    48, 16, 56, 24, 50, 18, 58, 26,\n"
    "    12, 44, 4, 36, 14, 46, 6, 38,\n"
    "    60, 28, 52, 20, 62, 30, 54, 22,\n"
    "   3, 35, 11, 43, 1, 33, 9, 41,\n"
    "   51, 19, 59, 27, 49, 17, 57, 25,\n"
    "   15, 47, 7, 39, 13, 45, 5, 37,\n"
    "   63, 31, 55, 23, 61, 29, 53, 21);\n"
    "\n"
    "  float limit = 0.0;\n"
    "  if(x < 8)\n"
    "  {\n"
    "    limit = (dither[x*8+y]+1)/64.0;\n"
    "  }\n"
    "  if(c0 < limit)\n"
    "    return 0.0;\n"
    "  return 1.0;\n"
    "}\n"
    "void main(void)\n"
    "{\n"
    "  vec4 lum = vec4(0.299, 0.587, 0.114, 0);\n"
    "  float grayscale = dot(texture(tex0, out_texco.xy), lum);\n"
    "  vec3 rgb = texture(tex0, out_texco).rgb;\n"
    "\n"
    "  vec2 xy = gl_FragCoord.xy * Scale;\n"
    "  int x = int(mod(xy.x, 8));\n"
    "  int y = int(mod(xy.y, 8));\n"
    "\n"
    "  vec3 finalRGB;\n"
    "  finalRGB.r = find_closest(x, y, rgb.r);\n"
    "  finalRGB.g = find_closest(x, y, rgb.g);\n"
    "  finalRGB.b = find_closest(x, y, rgb.b);\n"
    "\n"
    "  float final = find_closest(x, y, grayscale);\n"
    "  oColor = vec4(finalRGB, 1.0);\n"
    "}\n";

/* makeProgram returns a compiled and linked shader from the given vertex
 * and
 * fragment source. */
GLint makeProgram(GLchar const *vertSource, GLchar const *fragSource) {
	GLint program;
	GLint compiled, linked;
	GLint fs, vs;
	fs = 0;
	vs = 0;

	/* create shader program (if not already built) */
	GLint len;
	/* vertex shader */
	vs = glCreateShader(GL_VERTEX_SHADER);
	len = strlen(vertSource);
	glShaderSource(vs, 1, &vertSource, &len);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GLint logSz;
		GLchar *log;

		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logSz);
		log = malloc(logSz * sizeof(GLchar));
		glGetShaderInfoLog(vs, logSz, &logSz, log);
		puts("error: vertex shader compilation failed");
		puts(log);
		free(log);
		return -1;
	}

	/* fragment shader */
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	len = strlen(fragSource);
	glShaderSource(fs, 1, &fragSource, &len);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GLint logSz;
		GLchar *log;

		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &logSz);
		log = malloc(logSz * sizeof(GLchar));
		glGetShaderInfoLog(fs, logSz, &logSz, log);
		puts("error: fragment shader compilation failed");
		puts(log);
		free(log);
		return -1;
	}

	/* link shaders */
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE) {
		GLint logSz;
		GLchar *log;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSz);
		log = malloc(logSz * sizeof(GLchar));
		glGetProgramInfoLog(program, logSz, &logSz, log);
		puts("error: shader program failed to link");
		puts(log);
		free(log);
		return -1;
	}
	return program;
}

/* InitShaders compiles all the various shader programs. */
bool InitShaders() {
	{
		programs[COLOR_PROGRAM].program =
		    makeProgram(vertexShaderC, fragmentShaderC);
		programs[COLOR_PROGRAM].mvp = glGetUniformLocation(
		    programs[COLOR_PROGRAM].program, "MVP");
		programs[COLOR_PROGRAM].tex0 = -1;
		programs[COLOR_PROGRAM].color = -1;
	}

	{
		programs[TEXTURE_PROGRAM].program =
		    makeProgram(vertexShaderT, fragmentShaderT);
		programs[TEXTURE_PROGRAM].mvp = glGetUniformLocation(
		    programs[TEXTURE_PROGRAM].program, "MVP");
		programs[TEXTURE_PROGRAM].tex0 = glGetUniformLocation(
		    programs[TEXTURE_PROGRAM].program, "tex0");
		programs[TEXTURE_PROGRAM].color = glGetUniformLocation(
		    programs[TEXTURE_PROGRAM].program, "color");
	}

	{
		programs[SHADOW_PROGRAM].program =
		    makeProgram(vertexShaderShadow, fragmentShaderShadow);
		programs[SHADOW_PROGRAM].mvp = glGetUniformLocation(
		    programs[SHADOW_PROGRAM].program, "MVP");
		programs[SHADOW_PROGRAM].tex0 = glGetUniformLocation(
		    programs[SHADOW_PROGRAM].program, "tex0");
		programs[SHADOW_PROGRAM].color = -1;
	}

	{
		programs[BAYER_PROGRAM].program =
		    makeProgram(vertexShaderT, bayerFrag);
		programs[BAYER_PROGRAM].mvp = glGetUniformLocation(
		    programs[BAYER_PROGRAM].program, "MVP");
		programs[BAYER_PROGRAM].tex0 = glGetUniformLocation(
		    programs[BAYER_PROGRAM].program, "tex0");
		programs[BAYER_PROGRAM].color = -1;
	}
}
/* GetUMVP returns the ID of the MVP uniform for the provided program. */
GLint GetUMVP(int program) { return programs[program].mvp; }
bool SetUMVP(int program, mat4x4 mvp) {
	GLint loc = programs[program].mvp;
	if ((programs[program].program >= 0) && (loc >= 0)) {
		glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat *)mvp);
		return true;
	} else {
		dwarnf("No MVP uniform in shader");
	}
	return false;
}

/* GetUTex returns the ID of the tex0 uniform for the provided program. */
GLint GetUTex(int program) { return programs[program].tex0; }
bool SetUTex(int program, int loc) {
	GLint uloc = programs[program].tex0;
	if ((programs[program].program >= 0) && (uloc >= 0)) {
		glUniform1i(uloc, loc);
		return true;
	} else {
		dwarnf("No tex0 uniform in shader");
	}
	return false;
}

/* GetUColor returns the ID of the color uniform for the provided program. */
GLint GetUColor(int program) { return programs[program].color; }
bool SetUColor(int program, float r, float g, float b, float a) {
	GLint loc = programs[program].color;
	if ((programs[program].program >= 0) && (loc >= 0)) {
		glUniform4f(programs[program].color, r, g, b, a);
		return true;
	} else {
		dwarnf("No color uniform in shader");
	}
	return false;
}

/* GetProgram returns the handle of the shader program for the given shader. */
GLint GetProgram(int program) { return programs[program].program; }

/* UseProgram uses the given program. */
bool UseProgram(int program) {
	if (programs[program].program >= 0) {
		glUseProgram(programs[program].program);
		return true;
	}
	return false;
}
