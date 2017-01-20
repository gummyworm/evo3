#include "sys_mesh.h"
#include "third-party/include/uthash.h"
#include <assimp/cimport.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct entityToMesh {
	Entity e;
	struct Mesh *mesh;
	UT_hash_handle hh;
};

static struct entityToMesh *entitiesToMeshes;
static struct Mesh meshes[MAX_MESHES];
static int numMeshes;

static int numUpdates;
static struct MeshUpdate updates[MAX_MESHES];

static void MeshLoad(struct Mesh *, const char *);

/* getMesh returns the mesh attached to entity e (if there is one). */
static struct Mesh *getMesh(Entity e) {
	struct entityToMesh *t;

	if (entitiesToMeshes == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToMeshes, &e, t);
	if (t == NULL)
		return NULL;

	return t->mesh;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct MeshUpdate *u) { updates[numUpdates++] = *u; }

/* InitMeshSystem initializes the mesh system. */
void InitMeshSystem() {}

/* UpdateMeshSystem updates all meshs that have been created. */
void UpdateMeshSystem() { numUpdates = 0; }

/* AddMesh adds a mesh component to the entity e. */
void AddMesh(Entity e, const char *filename) {
	struct entityToMesh *item;

	if (getMesh(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToMesh));
	item->mesh = meshes + numMeshes;
	item->e = e;

	meshes[numMeshes].e = e;

	MeshLoad(meshes + numMeshes, filename);
	numMeshes++;
}

/* GetMeshUpdates returns the mesh updates this frame. */
struct MeshUpdate *GetMeshUpdates(int *num) {
	*num = numUpdates;
	return updates;
}

/* MeshLoad loads m with the mesh described by filename. */
void MeshLoad(struct Mesh *m, const char *filename) {
	unsigned int i;
	struct aiMesh *iMesh;
	const struct aiScene *scene = aiImportFile(
	    filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
	                  aiProcess_JoinIdenticalVertices |
	                  aiProcess_SortByPType);

	float *vertices, *colors, *texcos, *normals;
	GLshort *faces;

	if (scene == NULL) {
		return;
	}
	if (scene->mNumMeshes == 0) {
		return;
	}

	/* get the vertices */
	vertices = malloc(sizeof(float) * 3 * scene->mMeshes[0]->mNumVertices);
	for (i = 0, iMesh = scene->mMeshes[0]; i < iMesh->mNumVertices; ++i) {
		vertices[i * 3 + 0] = iMesh->mVertices[i].x;
		vertices[i * 3 + 1] = iMesh->mVertices[i].y;
		vertices[i * 3 + 2] = iMesh->mVertices[i].z;

		if (iMesh->mNormals) {
			normals[i * 3 + 0] = iMesh->mNormals[i].x;
			normals[i * 3 + 1] = iMesh->mNormals[i].y;
			normals[i * 3 + 2] = iMesh->mNormals[i].z;
		}

		if (iMesh->mTextureCoords[0]) {
			texcos[i * 2 + 0] = iMesh->mTextureCoords[i][0].x;
			texcos[i * 2 + 1] = iMesh->mTextureCoords[i][0].y;
		}

		if (iMesh->mColors[0]) {
			colors[i * 4 + 0] = iMesh->mColors[i][0].r;
			colors[i * 4 + 1] = iMesh->mColors[i][0].g;
			colors[i * 4 + 2] = iMesh->mColors[i][0].b;
			colors[i * 4 + 3] = iMesh->mColors[i][0].a;
		}
	}
	m->numVertices = iMesh->mNumVertices;

	/* get the indices for the faces */
	for (i = 0; i < iMesh->mNumFaces; ++i) {
		faces[i * 3 + 0] = iMesh->mFaces[i].mIndices[0];
		faces[i * 3 + 1] = iMesh->mFaces[i].mIndices[1];
		faces[i * 3 + 2] = iMesh->mFaces[i].mIndices[2];
	}
	m->numFaces = iMesh->mNumFaces;

	glGenBuffers(1, &m->vao);
	glBindBuffer(GL_ARRAY_BUFFER, m->vao);

	if (vertices) {
		glGenBuffers(1, &m->vbos.pos);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.pos);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 3 * iMesh->mNumVertices, vertices,
		             GL_STATIC_DRAW);
	}

	if (colors) {
		glGenBuffers(1, &m->vbos.color);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.color);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 4 * iMesh->mNumVertices, colors,
		             GL_STATIC_DRAW);
	}

	if (texcos) {
		glGenBuffers(1, &m->vbos.texture);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.texture);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 2 * iMesh->mNumVertices, texcos,
		             GL_STATIC_DRAW);
	}

	if (normals) {
		glGenBuffers(1, &m->vbos.normal);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.normal);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 3 * iMesh->mNumVertices, normals,
		             GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
