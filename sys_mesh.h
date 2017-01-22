#ifndef SYS_MESH_H
#define SYS_MESH_H

#include "entity.h"
#include "third-party/include/linmath.h"
#include <GLFW/glfw3.h>

enum { MAX_MESHES = 8192,
};

struct Mesh {
	Entity e;
	GLuint vao, program;
	GLuint texture;
	struct {
		GLuint pos, normal, texture, color, index;
	} vbos;
	struct {
		GLint pos, color, texco, normal;
	} attrs;

	unsigned numVertices, numFaces;
};

void AddMesh(Entity e, const char *);
void MeshDraw(Entity, mat4x4);
struct Mesh *GetMesh(Entity e);

#endif