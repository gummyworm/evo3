#ifndef SYS_MESH_H
#define SYS_MESH_H

#include "base.h"

#include "entity.h"
#include "third-party/include/linmath.h"

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

void AddMesh(Entity, const char *);
void AddColorMesh(Entity, const char *, float, float, float, float);
void RemoveMesh(Entity);
void MeshDraw(Entity, mat4x4);
struct Mesh *GetMesh(Entity);
void MeshLoad(struct Mesh *m, const char *);
void MeshLoadAndColor(struct Mesh *, const char *, float, float, float, float);

#endif
