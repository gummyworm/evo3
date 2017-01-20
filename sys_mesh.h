#ifndef SYS_MESH_H
#define SYS_MESH_H

#include "entity.h"
#include <GLFW/glfw3.h>

enum { MAX_MESHES = 8192,
};

struct MeshUpdate {
	Entity e;
};

struct Mesh {
	Entity e;
	GLuint vao;
	struct {
		GLuint pos, normal, texture, color, index;
	} vbos;

	int numVertices, numFaces;
};

void InitMeshSystem();
void AddMesh(Entity e, const char *);

#endif
