#ifndef SYS_MAP_H
#define SYS_MAP_H

#include "base.h"

#include "entity.h"
#include <stdbool.h>

enum { MAX_MAPS = 1024 };

/* TileMap is a struct that represents a tile-based map. */
struct TileMap {
	Entity e;
	int w, h;
	GLuint tileset;
	int tilew, tileh;
	int *tiles;
};

/* TileMapUpdate defines an update message that can be polled by interested
 * systems. */
struct TileMapUpdate {
	Entity e;
	float x, y, z;
};

void InitTileMapSystem();
void UpdateTileMapSystem();
void AddTileMap(Entity, const char *);
void RemoveTileMap(Entity);
void TileMapSystemClearUpdates();
struct TileMapUpdate *GetTileMapUpdates(int *);
struct TileMapUpdate *GetTileMapUpdate(Entity);

extern int numTileMapUpdates;

#endif
