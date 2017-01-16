#include "third-party/include/linmath.h"
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    "void main()\n"
    "{\n"
    "  out_color = vec4(texture(tex0, out_texco));\n"
    "}\n";

/* makeProgram returns a compiled and linked shader from the given vertex and
 * fragment source. */
static GLint makeProgram(GLchar const *vertSource, GLchar const *fragSource) {
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

/* getColorProgram returns the color shader program. */
static GLint getColorProgram() {
	static GLint program;

	if (program != 0)
		return program;

	program = makeProgram(vertexShaderC, fragmentShaderC);
	return program;
}

/* getTextureProgram returns the texture shader program. */
static GLint getTextureProgram() {
	static GLint program;

	if (program != 0)
		return program;

	program = makeProgram(vertexShaderT, fragmentShaderT);
	return program;
}

/* Rect draws a w x h rectangle @ (x,y). */
void Rect(GLFWwindow *window, unsigned x, unsigned y, unsigned w, unsigned h,
          uint32_t rgba) {
	GLint program;
	static GLuint vao;
	static struct { GLuint col, pos; } buffs;
	static struct { GLint col, pos; } attrs;
	uint8_t r = (rgba & 0xff000000) >> 24;
	uint8_t g = (rgba & 0x00ff0000) >> 16;
	uint8_t b = (rgba & 0x0000ff00) >> 8;
	uint8_t a = (rgba & 0x000000ff);
	uint8_t c[] = {
	    r, g, b, a, r, g, b, a, r, g, b, a,
	    r, g, b, a, r, g, b, a, r, g, b, a,
	};
	uint16_t v[] = {x, y, 1, 1, x + w, y,     1, 1, x + w, y + h, 1, 1,
	                x, y, 1, 1, x + w, y + h, 1, 1, x,     y + h, 1, 1};

	if ((program = getColorProgram()) < 0)
		return;
	glUseProgram(program);

	glUseProgram(program);
	if (vao == 0) {
		glGenBuffers(1, &buffs.pos);
		glGenBuffers(1, &buffs.col);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		attrs.pos = glGetAttribLocation(program, "vPos");
		if (attrs.pos == -1)
			puts("error: could not find attribute \"pos\" "
			     "in "
			     "shader program.");
		attrs.col = glGetAttribLocation(program, "vCol");
		if (attrs.col == -1)
			puts("error: could not find attribute \"col\" "
			     "in "
			     "shader program.");
	}
	glBindVertexArray(vao);

	float ratio;
	int width, height;
	mat4x4 m, p, mvp;
	mat4x4_identity(m);
	mat4x4_rotate_Z(m, m, (float)glfwGetTime());
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	mat4x4_perspective(p, 45.0f, ratio, 0.0f, 100.0f);
	// mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	mat4x4_mul(mvp, p, m);

	GLint mvp_location;
	mvp_location = glGetUniformLocation(program, "MVP");
	if (mvp_location == -1) {
		puts("error: uniform \"MVP\" not found in shader "
		     "source\n");
	}
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.col);
	glBufferData(GL_ARRAY_BUFFER, sizeof(c), c, GL_STATIC_DRAW);

	glEnableVertexAttribArray(attrs.pos);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.pos);
	glVertexAttribPointer(attrs.pos, 4, GL_UNSIGNED_SHORT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(attrs.col);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.col);
	glVertexAttribPointer(attrs.col, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

/* TexRect draws a w x h rectangle @ (x,y). */
void TexRect(GLFWwindow *window, unsigned x, unsigned y, unsigned w, unsigned h,
             float clipx, float clipy, float clipw, float cliph, GLuint tex) {
	GLint program;
	static GLuint vao;
	static struct { GLuint texco, pos; } buffs;
	static struct { GLint texco, pos; } attrs;
	float u, v;
	u = clipx;
	v = clipy;

	uint16_t vd[] = {x, y, 1, 1, x + w, y,     1, 1, x + w, y + h, 1, 1,
	                 x, y, 1, 1, x + w, y + h, 1, 1, x,     y + h, 1, 1};

	float td[] = {u, v, u + clipw, v,         u + clipw, v - cliph,
	              u, v, u + clipw, v - cliph, u,         v - cliph};

	uint8_t c[] = {
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	};

	if ((program = getTextureProgram()) < 0)
		return;
	glUseProgram(program);

	if (vao == 0) {
		glGenBuffers(1, &buffs.pos);
		glGenBuffers(1, &buffs.texco);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		attrs.pos = glGetAttribLocation(program, "vPos");
		if (attrs.pos == -1)
			puts("error: could not find attribute \"pos\" "
			     "in "
			     "shader program.");
		attrs.texco = glGetAttribLocation(program, "vTexco");
		if (attrs.texco == -1)
			puts("error: could not find attribute \"texco\" "
			     "in "
			     "shader program.");
	}
	glBindVertexArray(vao);

	float ratio;
	int width, height;
	mat4x4 m, p, mvp;
	mat4x4_identity(m);
	mat4x4_translate(m, 0.f, 0.f, -7.0f);
	// mat4x4_rotate_Z(m, m, (float)glfwGetTime());
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	mat4x4_perspective(p, 45.0f, ratio, 0.0f, 100.0f);
	// mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	mat4x4_mul(mvp, p, m);

	GLint tex_location, mvp_location;

	if ((tex_location = glGetUniformLocation(program, "tex0")) < 0)
		return;
	if ((mvp_location = glGetUniformLocation(program, "MVP")) < 0)
		return;

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vd), vd, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.texco);
	glBufferData(GL_ARRAY_BUFFER, sizeof(td), td, GL_STATIC_DRAW);

	glEnableVertexAttribArray(attrs.pos);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.pos);
	glVertexAttribPointer(attrs.pos, 4, GL_UNSIGNED_SHORT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(attrs.texco);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.texco);
	glVertexAttribPointer(attrs.texco, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glUniform1i(tex_location, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

/* GetTexture returns a handle to the texture stored in the given file.
 */
GLuint GetTexture(const char *filename) {
	int width, height;
	GLuint tex;

	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);

	unsigned char *image =
	    SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
	             GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(image);
	return tex;
}

void DrawInit() {}
