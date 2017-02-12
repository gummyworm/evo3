#include "draw.h"
#include "debug.h"
#include "shaders.h"
#include "third-party/include/uthash.h"
#include <SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct LoadedTexture {
	GLuint id;
	char filename[32];
	UT_hash_handle hh;
};

static struct LoadedTexture *loadedTextures;

/* Rect draws a w x h rectangle @ (x,y). */
void Rect(mat4x4 mvp, unsigned x, unsigned y, unsigned w, unsigned h,
          uint32_t rgba) {
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

	if (!UseProgram(COLOR_PROGRAM))
		return;

	if (vao == 0) {
		GLint cp;
		glGenBuffers(1, &buffs.pos);
		glGenBuffers(1, &buffs.col);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		cp = GetProgram(COLOR_PROGRAM);
		attrs.pos = glGetAttribLocation(cp, "vPos");
		if (attrs.pos == -1)
			puts("error: could not find attribute \"pos\" "
			     "in "
			     "shader program.");
		attrs.col = glGetAttribLocation(cp, "vCol");
		if (attrs.col == -1)
			puts("error: could not find attribute \"col\" "
			     "in "
			     "shader program.");
	}
	glBindVertexArray(vao);

	SetUMVP(COLOR_PROGRAM, mvp);

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

/* TexRectZRot draws a wxh rectangle @ (x,y,z) rotated by angle radians.  */
void TexRectZRot(mat4x4 mvp, int program, float x, float y, float z, float w,
                 float h, float clipx, float clipy, float clipw, float cliph,
                 float angle, GLuint tex) {
	static GLuint vao;
	static struct { GLuint texco, pos; } buffs;
	static struct { GLint texco, pos; } attrs;
	float u, v;
	static float vd[] = {-.5f, -.5f, 0.f, 1, .5f,  -.5f, 0.f, 1,
	                     .5f,  .5f,  0,   1, -.5f, -.5f, 0.f, 1,
	                     .5f,  .5f,  0.f, 1, -.5f, .5f,  0.f, 1};
	u = clipx;
	v = clipy;
	float td[] = {u, v, u + clipw, v,         u + clipw, v + cliph,
	              u, v, u + clipw, v + cliph, u,         v + cliph};

	if (!UseProgram(program)) {
		if (!UseProgram(TEXTURE_PROGRAM))
			return;
		dwarnf("no shader program specified");
	}

	if (vao == 0) {
		glGenBuffers(1, &buffs.pos);
		glGenBuffers(1, &buffs.texco);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		attrs.pos = glGetAttribLocation(GetProgram(program), "vPos");
		if (attrs.pos == -1)
			puts("error: could not find attribute \"pos\" "
			     "in "
			     "shader program.");
		attrs.texco =
		    glGetAttribLocation(GetProgram(program), "vTexco");
		if (attrs.texco == -1)
			puts("error: could not find attribute \"texco\" "
			     "in "
			     "shader program.");
	}
	glBindVertexArray(vao);

	{
		mat4x4 rot, trans;
		mat4x4 m, mv;
		mat4x4_identity(m);
		mat4x4_rotate_Z(rot, m, angle);
		mat4x4_translate(trans, x + w / 2.f, y + h / 2.f, z);
		mat4x4_mul(mv, trans, rot);
		mat4x4_scale_aniso(rot, mv, w, h, 0.f);
		mat4x4_mul(m, mvp, rot);
		SetUMVP(program, m);
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

/* TexRectZ draws a w x h rectangle @ (x,y,z). */
void TexRectZ(mat4x4 mvp, int program, float x, float y, float z, float w,
              float h, float clipx, float clipy, float clipw, float cliph,
              GLuint tex) {
	TexRectZRot(mvp, program, x, y, z, w, h, clipx, clipy, clipw, cliph,
	            0.f, tex);
}

/* TexRect draws a w x h rectangle @ (x,y). */
void TexRect(mat4x4 mvp, int program, float x, float y, float w, float h,
             float clipx, float clipy, float clipw, float cliph, GLuint tex) {
	TexRectZ(mvp, program, x, y, 1.f, w, h, clipx, clipy, clipw, cliph,
	         tex);
}

/* Text renders msg at (x,y). */
void Text(mat4x4 proj, int x, int y, unsigned sz, const char *msg) {
	unsigned i;
	unsigned cx, cy;
	float clipx, clipy, clipw, cliph;
	clipw = 1.f / 16.f;
	cliph = 1.f / 16.f;
	static GLuint fontTex;

	if (fontTex == 0) {
		if ((fontTex = GetTexture("font.png")) == 0)
			return;
	}

	cx = x;
	cy = y;
	for (i = 0; i < strlen(msg); ++i) {
		int c = msg[i];
		clipx = clipw * (c % 16);
		clipy = cliph * (c / 16);
		TexRect(proj, TEXTURE_PROGRAM, cx, cy, sz, sz, clipx, clipy,
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
