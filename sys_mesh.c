#include "sys_mesh.h"
#include "debug.h"
#include "sys_transform.h"
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

static void MeshLoad(struct Mesh *, const char *);

/* GetMesh returns the mesh attached to entity e (if there is one). */
struct Mesh *GetMesh(Entity e) {
	struct entityToMesh *m;

	if (entitiesToMeshes == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToMeshes, &e, m);
	if (m == NULL)
		return NULL;

	return m->mesh;
}

/* AddMesh adds a mesh component to the entity e. */
void AddMesh(Entity e, const char *filename) {
	struct entityToMesh *item;

	if (GetMesh(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToMesh));
	item->mesh = meshes + numMeshes;
	item->e = e;

	meshes[numMeshes].e = e;

	MeshLoad(meshes + numMeshes, filename);
	numMeshes++;
}

/* MeshLoad loads m with the mesh described by filename. */
void MeshLoad(struct Mesh *m, const char *filename) {
	unsigned i;
	struct aiMesh *iMesh;
	const struct aiScene *scene = aiImportFile(
	    filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
	                  aiProcess_JoinIdenticalVertices |
	                  aiProcess_SortByPType);

	float *vertices, *colors, *texcos, *normals;
	GLshort *faces;

	if (scene == NULL) {
		dwarnf("no scene found in file %s", filename);
		return;
	}
	if (scene->mNumMeshes == 0) {
		dwarnf("no mesh found in scene for file %s", filename);
		return;
	}

	m->numVertices = scene->mMeshes[0]->mNumVertices;
	m->numFaces = scene->mMeshes[0]->mNumFaces;

	vertices = normals = colors = texcos = NULL;
	faces = NULL;

	iMesh = scene->mMeshes[0];
	if (iMesh->mVertices)
		vertices = malloc(sizeof(float) * 3 * m->numVertices);
	if (iMesh->mTextureCoords[0])
		texcos = malloc(sizeof(float) * 2 * m->numVertices);
	if (iMesh->mNormals)
		normals = malloc(sizeof(float) * 3 * m->numVertices);
	if (iMesh->mColors[0])
		colors = malloc(sizeof(float) * 4 * m->numVertices);
	if (m->numFaces > 0)
		faces = malloc(sizeof(GLshort) * 3 * 3 * m->numFaces);

	/* get the vertices */
	for (i = 0; i < m->numVertices; ++i) {
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

	if (iMesh->mFaces) {
		for (i = 0; i < m->numFaces; ++i) {
			faces[i * 3 + 0] = iMesh->mFaces[i].mIndices[0];
			faces[i * 3 + 1] = iMesh->mFaces[i].mIndices[1];
			faces[i * 3 + 2] = iMesh->mFaces[i].mIndices[2];
		}
	}

	glGenBuffers(1, &m->vao);
	glBindBuffer(GL_ARRAY_BUFFER, m->vao);

	if (vertices) {
		glGenBuffers(1, &m->vbos.pos);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.pos);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 3 * iMesh->mNumVertices, vertices,
		             GL_STATIC_DRAW);
		free(vertices);
	}

	if (colors) {
		glGenBuffers(1, &m->vbos.color);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.color);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 4 * iMesh->mNumVertices, colors,
		             GL_STATIC_DRAW);
		free(colors);
	}

	if (texcos) {
		glGenBuffers(1, &m->vbos.texture);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.texture);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 2 * iMesh->mNumVertices, texcos,
		             GL_STATIC_DRAW);
		free(texcos);
	}

	if (normals) {
		glGenBuffers(1, &m->vbos.normal);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.normal);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 3 * iMesh->mNumVertices, normals,
		             GL_STATIC_DRAW);
		free(normals);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* MeshDraw renders the e's mesh using the given modelview-projection matrix. */
void MeshDraw(Entity e, mat4x4 mvp) {
	struct Mesh *m;
	GLint tex_location, mvp_location;

	m = GetMesh(e);
	if (m == NULL)
		return;

	glUseProgram(m->program);
	if ((tex_location = glGetUniformLocation(m->program, "tex0")) > 0) {
		glUniform1i(tex_location, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m->texture);
	}

	if ((mvp_location = glGetUniformLocation(m->program, "MVP")) < 0)
		return;
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);

	glBindVertexArray(m->vao);
	if (m->numFaces > 0)
		glDrawElements(GL_TRIANGLES, m->numFaces * 3, GL_UNSIGNED_SHORT,
		               0);
	else
		glDrawArrays(GL_TRIANGLES, 0, m->numVertices);

	glBindVertexArray(0);
}
