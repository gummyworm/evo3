#include "sys_map.h"
#include "debug.h"
#include "draw.h"
#include "entities.h"
#include "systems.h"
#include "third-party/include/cJSON.h"
#include "third-party/include/uthash.h"

struct entityToTileMap {
	Entity e;
	struct TileMap *tileMap;
	UT_hash_handle hh;
};

static struct entityToTileMap *entitiesToTileMaps;
static struct TileMap tileMaps[MAX_MAPS];
static int numTileMaps;

int numTileMapUpdates;
static struct TileMapUpdate updates[MAX_MAPS];

/* parseMap reads the given json map (exported from Tiled) into m. */
static void parseMap(struct TileMap *m, const char *json) {
	int i, numTiles;
	char *contents;
	long fsize;
	cJSON *root, *layer, *data;
	FILE *f = fopen(json, "r");

	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	contents = malloc(fsize + 1);
	fread(contents, fsize, 1, f);
	fclose(f);
	contents[fsize] = '\0';

	root = cJSON_Parse(contents);

	m->h = cJSON_GetObjectItem(root, "height")->valueint;
	m->w = cJSON_GetObjectItem(root, "width")->valueint;
	{
		float imgw, imgh, tw, th;
		cJSON *tileset = cJSON_GetObjectItem(root, "tilesets");
		tileset = tileset->child;

		imgw = cJSON_GetObjectItem(tileset, "imagewidth")->valueint;
		imgh = cJSON_GetObjectItem(tileset, "imageheight")->valueint;
		tw = cJSON_GetObjectItem(tileset, "tilewidth")->valueint;
		th = cJSON_GetObjectItem(tileset, "tileheight")->valueint;
		m->tilesetW = imgw / tw;
		m->tilesetH = imgh / th;
		m->tilew = 1.f / m->tilesetW;
		m->tileh = 1.f / m->tilesetH;

		m->tileset = GetTexture(
		    cJSON_GetObjectItem(tileset, "image")->valuestring);
	}

	layer = cJSON_GetObjectItem(root, "layers");
	layer = layer->child;
	data = cJSON_GetObjectItem(layer, "data");

	numTiles = cJSON_GetArraySize(data);
	dassert(numTiles == (m->w * m->h));
	m->tiles = malloc(numTiles * sizeof(int));

	for (i = 0; i < numTiles; i++)
		m->tiles[i] = cJSON_GetArrayItem(data, i)->valueint;
}

/* bufferMap buffers the tile data for batched rendering. */
static void bufferMap(struct TileMap *m) {
	int i, j;
	float *v, *t;

	v = malloc(sizeof(float) * 18 * m->w * m->h);
	t = malloc(sizeof(float) * 12 * m->w * m->h);
	for (i = 0; i < m->h; ++i) {
		for (j = 0; j < m->w; ++j) {
			int id = m->tiles[i * m->w + j];
			float cx = m->tilew * (float)(id % m->tilesetW);
			float cy = m->tileh * (float)(id / m->tilesetW);
			float cw = m->tilew;
			float ch = m->tileh;
			int vi = 18 * (i * m->w + j);
			int ti = 12 * (i * m->w + j);

			v[vi + 0] = -j;
			v[vi + 1] = -i;
			v[vi + 2] = 0;

			v[vi + 3] = -j - 1;
			v[vi + 4] = -i;
			v[vi + 5] = 0;

			v[vi + 6] = -j - 1;
			v[vi + 7] = -i - 1;
			v[vi + 8] = 0;

			v[vi + 9] = -j;
			v[vi + 10] = -i;
			v[vi + 11] = 0;

			v[vi + 12] = -j - 1;
			v[vi + 13] = -i - 1;
			v[vi + 14] = 0;

			v[vi + 15] = -j;
			v[vi + 16] = -i - 1;
			v[vi + 17] = 0;

			t[ti + 0] = cx;
			t[ti + 1] = cy;
			t[ti + 2] = cx + cw;
			t[ti + 3] = cy;
			t[ti + 4] = cx + cw;
			t[ti + 5] = cy + ch;

			t[ti + 6] = cx;
			t[ti + 7] = cy;
			t[ti + 8] = cx + cw;
			t[ti + 9] = cy + ch;
			t[ti + 10] = cx;
			t[ti + 11] = cy + ch;
		}
	}
	m->numVertices = m->w * m->h * 6;

	m->program = TEXTURE_PROGRAM;
	UseProgram(m->program);
	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);

	glGenBuffers(1, &m->vbos.pos);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbos.pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * m->numVertices, v,
	             GL_STATIC_DRAW);
	free(v);

	glGenBuffers(1, &m->vbos.tex);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbos.tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * m->numVertices, t,
	             GL_STATIC_DRAW);
	free(t);

	m->attrs.pos = glGetAttribLocation(GetProgram(m->program), "vPos");
	m->attrs.tex = glGetAttribLocation(GetProgram(m->program), "vTexco");

	glEnableVertexAttribArray(m->attrs.pos);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbos.pos);
	glVertexAttribPointer(m->attrs.pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(m->attrs.tex);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbos.tex);
	glVertexAttribPointer(m->attrs.tex, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* drawMap renders the given map. */
static void drawMap(struct TileMap *m) {
	mat4x4 proj;

	GetViewProjection(E_PLAYER, proj);

	UseProgram(m->program);
	SetUMVP(m->program, proj);
	SetUTex(m->program, 0);

	glBindVertexArray(m->vao);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m->tileset);
	glDrawArrays(GL_TRIANGLES, 0, m->numVertices);
	glBindVertexArray(0);
}

/* getTileMap returns the tileMap attached to entity e (if there is
 * one). */
static struct TileMap *getTileMap(Entity e) {
	struct entityToTileMap *t;

	if (entitiesToTileMaps == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToTileMaps, &e, t);
	if (t == NULL)
		return NULL;

	return t->tileMap;
}

/* InitTileMapSystem initializes the tileMap system. */
void InitTileMapSystem() {}

/* UpdateTileMapSystem updates all tileMaps that have been created. */
void UpdateTileMapSystem() {
	int i;

	for (i = 0; i < numTileMaps; ++i)
		drawMap(tileMaps + i);
}

/* AddTileMap adds a tileMap component to the entity e. */
void AddTileMap(Entity e, const char *file) {
	struct entityToTileMap *item;

	if (getTileMap(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToTileMap));
	item->tileMap = tileMaps + numTileMaps;
	item->e = e;

	tileMaps[numTileMaps].e = e;
	parseMap(tileMaps + numTileMaps, file);
	bufferMap(tileMaps + numTileMaps);

	HASH_ADD_INT(entitiesToTileMaps, e, item);
	numTileMaps++;
}

/* RemoveTileMap removes the tileMap attached to e from the TileMap
 * system. */
void RemoveTileMap(Entity e) {
	struct entityToTileMap *c;

	if (entitiesToTileMaps == NULL)
		return;

	HASH_FIND_INT(entitiesToTileMaps, &e, c);
	if (c != NULL) {
		struct TileMap *sys = c->tileMap;
		int sz = (tileMaps + numTileMaps) - sys;

		if (sys->tiles)
			free(sys->tiles);

		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToTileMaps, c);
		free(c);
	}
}

/* GetTileMapUpdate returns any tileMap updates for the entity e. */
struct TileMapUpdate *GetTileMapUpdate(Entity e) {
	int i;

	for (i = 0; i < numTileMapUpdates; ++i) {
		if (updates[i].e == e)
			return updates + i;
	}

	return NULL;
}

/* GetTileMapUpdates returns the tileMap updates this frame. */
struct TileMapUpdate *GetTileMapUpdates(int *num) {
	*num = numTileMapUpdates;
	return updates;
}
