#ifndef ROOM_H
#define ROOM_H

#include <stdbool.h>

#include "entity.h"

enum { MAX_ROOMS = 8192,
};

/* Room defines a logical region of the world. */
struct Room {
	Entity e;
	struct {
		float x, y, z;
		float w, h, d;
	} bounds;

	const char *name;
	const char *brief;
	const char *desc;
};

/* RoomUpdate defines an update message that can be polled by
 * interested systems. */
struct RoomUpdate {
	Entity e;
};

void InitRoomSystem();
void UpdateRoomSystem();
void AddRoom(Entity, const char *, const char *, float, float, float, float,
             float, float);
Entity GetRoom(Entity);
bool RoomContains(Entity, float, float, float);
bool RoomContainsEntity(Entity, Entity);
const char *GetRoomDescription(Entity);

int ThingsInRoom(Entity, Entity *);

#endif
