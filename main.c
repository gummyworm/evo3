#include <glad/glad.h>

#include "linmath.h"
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <stdio.h>
#include <stdlib.h>

#include "systems.h"

/* vertexShader is the vertex shader used for rendering. */
static GLchar const *vertexShader[] = {"#version 150\n"
                                       "in vec4 vPos;\n"
                                       "in vec2 vCol;\n"
                                       "out vec2 out_texco;\n"
                                       "uniform mat4 MVP;\n"
                                       "void main( void ) {"
                                       "    gl_Position = MVP * vPos;\n"
                                       "    out_texco = vCol;\n"
                                       "}\n"};

/* fragmentShader is the fragment shader used for rendering. */
static GLchar const *fragmentShader[] = {
    "#version 150\n"
    "in vec2 out_texco;\n"
    "out vec4 out_color;\n"
    "void main()\n"
    "{\n"
    "  out_color = vec4(out_texco, 1, 0);\n //texture(tex0, out_texco);\n"
    "}\n"};

static const struct {
	float x, y;
	float r, g;
} vertices[3] = {
    {-0.6f, -0.4f, 1.f, 0.f}, {0.6f, -0.4f, 0.f, 1.f}, {0.f, 0.6f, 0.f, 0.f}};

/*
static const struct {
        float x, y;
        float u, v;
} vertices[6] = {{-1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},
                 {1.0f, -1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f, 1.0f},
                 {1.0f, -1.0f, 1.0f, 0.0f}, {-1.0f, -1.0f, 0.0f, 0.0f}};

static const struct {
        float x, y;
        float r, g;
} vertices[3] = {
    {-0.6f, -0.4f, 1.f, 0.f}, {0.6f, -0.4f, 0.f, 1.f}, {0.f, 0.6f, 0.f, 0.f}};

static const char *vertexShader[] = {
    "#version 140\n"
    "uniform mat4 MVP;\n"
    "attribute vec3 vCol;\n"
    "attribute vec2 vPos;\n"
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
    "    color = vCol;\n"
    "}\n"};

static const char *fragmentShader[] = {"#version 140\n"
                                       "varying vec3 color;\n"
                                       "void main()\n"
                                       "{\n"
                                       "    gl_FragColor = vec4(color, 1.0);\n"
                                       "}\n"};

                                       */

/* onError is the GLFW callback for error handling. */
static void onError(int error, const char *description) {
	fprintf(stderr, "Error (%d): %s\n", error, description);
}

/* init initializes the various systems used in the game. */
static void init(GLFWwindow *win) {
	InitInput(win);
	InitUnitSystem();
	InitSpriteSystem();
	InitTransformSystem();
	InitCommanderSystem();
}

/* update updates the game. */
static void update() {
	UpdateTransformSystem();
	UpdateSpriteSystem();
}

/* TexRect draws a w x h rectangle @ (x,y). */
void TexRect(GLFWwindow *window, int frame, unsigned x, unsigned y, unsigned w,
             unsigned h) {
	const float ustep = 1.0f / 16;
	const float vstep = 1.0f / 16;
	GLint program;
	static GLuint vao;
	static GLuint tex;
	static struct { GLuint col, pos; } buffs;
	static struct { GLint col, pos; } attrs;
	float u, v;

	w = 1;
	h = 1;
	u = (frame % 16) / 16.0f;
	v = (frame / 16) / 16.0f;

	uint16_t vd[] = {x, y, 1, 1, x + w, y,     1, 1, x + w, y + h, 1, 1,
	                 x, y, 1, 1, x + w, y + h, 1, 1, x,     y + h, 1, 1};
	uint16_t td[] = {u, v, u + ustep, v,         u + ustep, v + vstep,
	                 u, v, u + ustep, v + vstep, u,         v + vstep};

	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	if (vao == 0) {
		glGenBuffers(1, &buffs.pos);
		glGenBuffers(1, &buffs.col);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		attrs.pos = glGetAttribLocation(program, "vPos");
		if (attrs.pos == -1)
			puts("error: could not find attribute \"pos\" in "
			     "shader program.");
		attrs.col = glGetAttribLocation(program, "vCol");
		if (attrs.col == -1)
			puts("error: could not find attribute \"col\" in "
			     "shader program.");
		tex = SOIL_load_OGL_texture(
		    "test.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
		    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y |
		        SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	}
	glBindVertexArray(vao);

	float ratio;
	int width, height;
	mat4x4 m, p, mvp;
	mat4x4_identity(m);
	mat4x4_rotate_Z(m, m, (float)glfwGetTime());
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	mat4x4_mul(mvp, p, m);

	// glUniform1i(tex_location, 0);
	GLint mvp_location;
	mvp_location = glGetUniformLocation(program, "MVP");
	if (mvp_location == -1) {
		puts("error: uniform \"MVP\" not found in shader source\n");
	}
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vd), vd, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.col);
	glBufferData(GL_ARRAY_BUFFER, sizeof(td), td, GL_STATIC_DRAW);

	glEnableVertexAttribArray(attrs.pos);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.pos);
	glVertexAttribPointer(attrs.pos, 4, GL_UNSIGNED_SHORT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(attrs.col);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.col);
	glVertexAttribPointer(attrs.col, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

/* Rect draws a w x h rectangle @ (x,y). */
void Rect(GLFWwindow *window, unsigned x, unsigned y, unsigned w, unsigned h,
          uint32_t rgba) {
	GLint program;
	static GLuint vao;
	static GLuint tex;
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

	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	if (vao == 0) {
		glGenBuffers(1, &buffs.pos);
		glGenBuffers(1, &buffs.col);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		attrs.pos = glGetAttribLocation(program, "vPos");
		if (attrs.pos == -1)
			puts("error: could not find attribute \"pos\" in "
			     "shader program.");
		attrs.col = glGetAttribLocation(program, "vCol");
		if (attrs.col == -1)
			puts("error: could not find attribute \"col\" in "
			     "shader program.");
		tex = SOIL_load_OGL_texture(
		    "test.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
		    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y |
		        SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	}
	glBindVertexArray(vao);

	float ratio;
	int width, height;
	mat4x4 m, p, mvp;
	mat4x4_identity(m);
	mat4x4_rotate_Z(m, m, (float)glfwGetTime());
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	mat4x4_mul(mvp, p, m);

	// glUniform1i(tex_location, 0);
	GLint mvp_location;
	mvp_location = glGetUniformLocation(program, "MVP");
	if (mvp_location == -1) {
		puts("error: uniform \"MVP\" not found in shader source\n");
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

int main() {
	GLFWwindow *window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint tex_location, mvp_location, vpos_location, vcol_location;
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
	init(window);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
	             GL_STATIC_DRAW);
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, vertexShader, NULL);
	glCompileShader(vertex_shader);
	{
		GLint compiled;
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
		if (compiled == GL_FALSE) {
			GLint logSz;
			GLchar *log;

			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH,
			              &logSz);
			log = malloc(logSz * sizeof(GLchar));
			glGetShaderInfoLog(vertex_shader, logSz, &logSz, log);
			puts("error: vertex shader compilation failed");
			puts(log);
			free(log);
			return -1;
		}
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, fragmentShader, NULL);
	glCompileShader(fragment_shader);
	{
		GLint compiled;
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
		if (compiled == GL_FALSE) {
			GLint logSz;
			GLchar *log;

			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH,
			              &logSz);
			log = malloc(logSz * sizeof(GLchar));
			glGetShaderInfoLog(fragment_shader, logSz, &logSz, log);
			puts("error: fragment shader compilation failed");
			puts(log);
			free(log);
			return -1;
		}
	}

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	{
		GLint linked;
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
	}

	mvp_location = glGetUniformLocation(program, "MVP");
	if (mvp_location == -1) {
		puts("error: uniform \"MVP\" not found in shader source\n");
		return -1;
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// tex_location = glGetUniformLocation(program, "tex0");
	vpos_location = glGetAttribLocation(program, "vPos");
	if (vpos_location == -1) {
		puts("error: attribute \"vPos\" not found in shader source\n");
		return -1;
	}
	vcol_location = glGetAttribLocation(program, "vCol");
	if (vcol_location == -1) {
		puts("error: attribute \"vCol\" not found in shader source\n");
		return -1;
	}

	/*
	glGenBuffers(1, &vbo) glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
	                      sizeof(float) * 4, (void *)0);
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 2, GL_FLOAT, GL_FALSE,
	                      sizeof(float) * 4, (void *)(sizeof(float) * 2));

	                      */
	glUseProgram(program);
	while (!glfwWindowShouldClose(window)) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		Rect(window, 0, 0, 1, 1, 0xffffffff);
		/*
		float ratio;
		int width, height;

		mat4x4 m, p, mvp;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		mat4x4_identity(m);
		mat4x4_rotate_Z(m, m, (float)glfwGetTime());
		mat4x4_ortho(p, -ratio, ratio, -2.f, 2.f, 2.f, -2.f);
		mat4x4_mul(mvp, p, m);
		glUseProgram(program);
		// glUniform1i(tex_location, 0);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE,
		                   (const GLfloat *)mvp);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		*/
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
