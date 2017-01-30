#include "sys_mesh.h"
#include "debug.h"
#include "draw.h"
#include "sys_transform.h"
#include "third-party/include/uthash.h"
#include <assimp/cimport.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

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
	HASH_ADD_INT(entitiesToMeshes, e, item);

	meshes[numMeshes].e = e;

	MeshLoad(meshes + numMeshes, filename);
	dinfof("loaded mesh %s", filename);
	numMeshes++;
}

/* RemoveMesh removes the mesh attached to e from the Mesh system. */
void RemoveMesh(Entity e) {
	struct entityToMesh *c;

	if (entitiesToMeshes == NULL)
		return;

	HASH_FIND_INT(entitiesToMeshes, &e, c);
	if (c != NULL) {
		struct Mesh *sys = c->mesh;
		int sz = (meshes + numMeshes) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToMeshes, c);
		free(c);
	}
}

/* MeshLoad loads m with the mesh described by filename. */
void MeshLoad(struct Mesh *m, const char *filename) {
	unsigned i, j;
	struct aiMesh *iMesh;
	const struct aiScene *scene = aiImportFile(
	    filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
	                  aiProcess_JoinIdenticalVertices |
	                  aiProcess_SortByPType);

	float *vertices, *colors, *texcos, *normals, *v, *c, *vn, *t;
	GLshort *faces, *f;

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

	m->numVertices = 0;
	m->numFaces = 0;
	for (i = 0; i < scene->mNumMeshes; ++i) {
		iMesh = scene->mMeshes[i];
		m->numVertices += iMesh->mNumVertices;
		m->numFaces += iMesh->mNumFaces;
	}

	iMesh = scene->mMeshes[0];
	vertices = malloc(sizeof(float) * 3 * m->numVertices);
	colors = malloc(sizeof(float) * 4 * m->numVertices);
	if (iMesh->mTextureCoords[0])
		texcos = malloc(sizeof(float) * 2 * m->numVertices);
	if (iMesh->mNormals)
		normals = malloc(sizeof(float) * 3 * m->numVertices);

	if (m->numFaces > 0)
		faces = malloc(sizeof(GLshort) * 3 * 3 * m->numFaces);

	/* get the vertices */
	v = vertices;
	c = colors;
	vn = normals;
	t = texcos;
	f = faces;
	for (j = 0; j < scene->mNumMeshes; ++j) {
		iMesh = scene->mMeshes[j];
		for (i = 0; i < iMesh->mNumVertices; ++i) {
			*v++ = iMesh->mVertices[i].x;
			*v++ = iMesh->mVertices[i].y;
			*v++ = iMesh->mVertices[i].z;

			if (iMesh->mNormals) {
				*vn++ = iMesh->mNormals[i].x;
				*vn++ = iMesh->mNormals[i].y;
				*vn++ = iMesh->mNormals[i].z;
			}

			if (iMesh->mTextureCoords[0]) {
				*t++ = iMesh->mTextureCoords[i][0].x;
				*t++ = iMesh->mTextureCoords[i][0].y;
			}

			if (iMesh->mColors[0]) {
				*c++ = iMesh->mColors[i][0].r;
				*c++ = iMesh->mColors[i][0].g;
				*c++ = iMesh->mColors[i][0].b;
				*c++ = iMesh->mColors[i][0].a;
			} else {
				*c++ = 0.8f;
				*c++ = 0.4f;
				*c++ = 0.41f;
				*c++ = 1.0f;
			}
		}

		if (iMesh->mFaces) {
			for (i = 0; i < iMesh->mNumFaces; ++i) {
				*f++ = iMesh->mFaces[i].mIndices[0];
				*f++ = iMesh->mFaces[i].mIndices[1];
				*f++ = iMesh->mFaces[i].mIndices[2];
			}
		}
	}

	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);

	if (vertices) {
		glGenBuffers(1, &m->vbos.pos);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.pos);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 3 * m->numVertices, vertices,
		             GL_STATIC_DRAW);
		free(vertices);
	}

	if (colors) {
		glGenBuffers(1, &m->vbos.color);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.color);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 4 * m->numVertices, colors,
		             GL_STATIC_DRAW);
		free(colors);
	}

	if (texcos) {
		glGenBuffers(1, &m->vbos.texture);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.texture);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 2 * m->numVertices, texcos,
		             GL_STATIC_DRAW);
		free(texcos);
	}

	if (normals) {
		glGenBuffers(1, &m->vbos.normal);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.normal);
		glBufferData(GL_ARRAY_BUFFER,
		             sizeof(float) * 3 * m->numVertices, normals,
		             GL_STATIC_DRAW);
		free(normals);
	}

	if (faces > 0) {
		glGenBuffers(1, &m->vbos.index);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->vbos.index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		             sizeof(GLshort) * 3 * m->numFaces, faces,
		             GL_STATIC_DRAW);
	}

	m->program = makeProgram(vertexShaderC, fragmentShaderC);
	m->attrs.pos = glGetAttribLocation(m->program, "vPos");
	m->attrs.color = glGetAttribLocation(m->program, "vCol");

	if (m->attrs.pos >= 0) {
		glEnableVertexAttribArray(m->attrs.pos);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.pos);
		glVertexAttribPointer(m->attrs.pos, 3, GL_FLOAT, GL_FALSE, 0,
		                      0);
	} else {
		dwarnf("no position attribute found in shader program");
	}

	if (m->attrs.color >= 0) {
		glEnableVertexAttribArray(m->attrs.color);
		glBindBuffer(GL_ARRAY_BUFFER, m->vbos.color);
		glVertexAttribPointer(m->attrs.color, 4, GL_FLOAT, GL_FALSE, 0,
		                      0);
	} else {
		dwarnf("no color attribute found in shader program");
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	aiReleaseImport(scene);
}

/* MeshDraw renders the e's mesh using the given modelview-projection
 * matrix. */
void MeshDraw(Entity e, mat4x4 mvp) {
	struct Mesh *m;
	GLint tex_location, mvp_location;

	m = GetMesh(e);
	if (m == NULL)
		return;

	glUseProgram(m->program);
	if ((tex_location = glGetUniformLocation(m->program, "tex0")) >= 0) {
		glEnable(GL_TEXTURE_2D);
		glUniform1i(tex_location, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m->texture);
	} else {
		glDisable(GL_TEXTURE_2D);
	}

	if ((mvp_location = glGetUniformLocation(m->program, "MVP")) < 0) {
		dwarnf("no MVP uniform found in program");
		return;
	}

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)mvp);
	glBindVertexArray(m->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->vbos.index);
	if (m->numFaces > 0)
		glDrawElements(GL_TRIANGLES, m->numFaces * 3, GL_UNSIGNED_SHORT,
		               0);
	else
		glDrawArrays(GL_TRIANGLES, 0, m->numVertices);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}
