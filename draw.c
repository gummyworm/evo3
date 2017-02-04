#include "draw.h"
#include "debug.h"
#include "third-party/include/uthash.h"
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

struct LoadedTexture {
	GLuint id;
	char filename[32];
	UT_hash_handle hh;
};

static struct LoadedTexture *loadedTextures;

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

/* getColorProgram returns the color shader program. */
GLint getColorProgram() {
	static GLint program;
	if (program != 0)
		return program;

	program = makeProgram(vertexShaderC, fragmentShaderC);
	return program;
}

/* getTextureProgram returns the texture shader program. */
GLint getTextureProgram() {
	static GLint program;
	if (program != 0)
		return program;

	program = makeProgram(vertexShaderT, fragmentShaderT);
	return program;
}

/* getBayerProgram returns the bayer filter shader program. */
GLint getBayerProgram() {
	static GLint program;
	if (program != 0)
		return program;

	program = makeProgram(vertexShaderT, bayerFrag);
	return program;
}

/* Rect draws a w x h rectangle @ (x,y). */
void Rect(mat4x4 mvp, unsigned x, unsigned y, unsigned w, unsigned h,
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

	GLint mvp_location;
	mvp_location = glGetUniformLocation(program, "MVP");
	if (mvp_location < -1) {
		puts("error: uniform \"MVP\" not found in shader "
		     "source\n");
		return;
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

/* TexRectZ draws a wxh rectangle @ (x,y,z).  */
void TexRectZ(mat4x4 mvp, GLint program, unsigned x, unsigned y, float z,
              unsigned w, unsigned h, float clipx, float clipy, float clipw,
              float cliph, GLuint tex) {
	static GLuint vao;
	static struct { GLuint texco, pos; } buffs;
	static struct { GLint texco, pos; } attrs;
	float u, v;
	u = clipx;
	v = clipy;

	float vd[] = {x, y, z, 1, x + w, y,     z, 1, x + w, y + h, z, 1,
	              x, y, z, 1, x + w, y + h, z, 1, x,     y + h, z, 1};

	float td[] = {u, v, u + clipw, v,         u + clipw, v + cliph,
	              u, v, u + clipw, v + cliph, u,         v + cliph};

	if (program < 0) {
		program = getTextureProgram();
		if (program < 0)
			return;
		dwarnf("no shader program specified");
	}
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

	{
		GLint tex_location, mvp_location;

		if ((mvp_location = glGetUniformLocation(program, "MVP")) < 0) {
			dwarnf("No MVP uniform found");
			return;
		}
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE,
		                   (const GLfloat *)mvp);

		if ((tex_location = glGetUniformLocation(program, "tex0")) <
		    0) {
			dwarnf("No tex0 uniform found");
			return;
		}
		glUniform1i(tex_location, 0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffs.pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vd), vd, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.texco);
	glBufferData(GL_ARRAY_BUFFER, sizeof(td), td, GL_STATIC_DRAW);

	glEnableVertexAttribArray(attrs.pos);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.pos);
	glVertexAttribPointer(attrs.pos, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(attrs.texco);
	glBindBuffer(GL_ARRAY_BUFFER, buffs.texco);
	glVertexAttribPointer(attrs.texco, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

/* TexRect draws a w x h rectangle @ (x,y). */
void TexRect(mat4x4 mvp, GLint program, unsigned x, unsigned y, unsigned w,
             unsigned h, float clipx, float clipy, float clipw, float cliph,
             GLuint tex) {
	TexRectZ(mvp, program, x, y, 1.f, w, h, clipx, clipy, clipw, cliph,
	         tex);
}

/* Text renders msg at (x,y). */
void Text(mat4x4 proj, unsigned x, unsigned y, unsigned sz, const char *msg) {
	unsigned i;
	unsigned cx, cy;
	float clipx, clipy, clipw, cliph;
	clipw = 1.f / 16.f;
	cliph = 1.f / 16.f;
	static GLuint fontTex;

	if (fontTex == 0) {
		fontTex = GetTexture("font.png");
	}

	cx = x;
	cy = y;
	for (i = 0; i < strlen(msg); ++i) {
		clipx = (msg[i] % 16) / 16.f;
		clipy = (msg[i] / 16) / 16.f;
		TexRect(proj, getTextureProgram(), cx, cy, sz, sz, clipx, clipy,
		        clipw, cliph, fontTex);
		cx += sz;
	}
}

/* GetTexture returns a handle to the texture stored in the given file.
 */
GLuint GetTexture(const char *filename) {
	int width, height;
	GLuint tex;
	struct LoadedTexture *lup;

	HASH_FIND_STR(loadedTextures, filename, lup);
	if (lup != NULL)
		return lup->id;

	unsigned char *image =
	    SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);

	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
	             GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(image);

	lup = malloc(sizeof(struct LoadedTexture));
	strcpy(lup->filename, filename);
	lup->id = tex;
	HASH_ADD_STR(loadedTextures, filename, lup);

	return tex;
}

void DrawInit() {}
