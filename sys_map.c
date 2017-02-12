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
		m->tileh = 1.f / m->tilesetW;

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

/* drawMap renders the given map. */
static void drawMap(struct TileMap *m) {
	int i, j;
	mat4x4 proj;

	GetViewProjection(E_PLAYER, proj);

	for (i = 0; i < m->h; ++i) {
		for (j = 0; j < m->w; ++j) {
			int t = m->tiles[i * m->w + j];
			float u = m->tilew * (float)(t % m->tilesetW);
			float v = m->tileh * (float)(t / m->tilesetW);
			float ch = m->tilew;
			float cw = m->tileh;
			float x = TILE_W * (float)(m->w - j);
			float y = TILE_H * (float)(m->h - i);

			TexRect(proj, getTextureProgram(), x, y, TILE_W, TILE_H,
			        u, v, cw, ch, m->tileset);
		}
	}
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

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct TileMapUpdate *u) {
	updates[numTileMapUpdates++] = *u;
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
