#include "sys_map.h"
#include "draw.h"
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
static void parseMap(struct TileMap *m, const char *json) {}

/* getTileMap returns the tileMap attached to entity e (if there is one). */
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
void UpdateTileMapSystem() {}

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
