#include "third-party/include/uthash.h"

#include "sys_room.h"

struct entityToRoom {
	Entity e;
	struct Room *room;
	UT_hash_handle hh;
};

static struct entityToRoom *entitiesToRooms;
static struct Room rooms[MAX_ROOMS];
static int numRooms;

static int numUpdates;
static struct RoomUpdate updates[MAX_ROOMS];

/* getRoom returns the room attached to entity e (if there is one). */
static struct Room *getRoom(Entity e) {
	struct entityToRoom *t;

	if (entitiesToRooms == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToRooms, &e, t);
	if (t == NULL)
		return NULL;

	return t->room;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct RoomUpdate *u) { updates[numUpdates++] = *u; }

/* InitRoomSystem initializes the room system. */
void InitRoomSystem() {}

/* UpdateRoomSystem updates all rooms that have been created. */
void UpdateRoomSystem() { numUpdates = 0; }

/* AddRoom adds a room component to the entity e. */
void AddRoom(Entity e, const char *name) {
	struct entityToRoom *item;

	if (getRoom(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToRoom));
	item->room = rooms + numRooms;
	item->e = e;

	rooms[numRooms].e = e;
	rooms[numRooms].name = name;

	HASH_ADD_INT(entitiesToRooms, e, item);
	numRooms++;
}

/* RoomContains returns true if (x, y, z) exists within e's room. */
bool RoomContains(Entity e, float x, float y, float z) {
	struct Room *room;

	room = getRoom(e);
	if (room == NULL)
		return false;

	if ((x >= room->bounds.x && x <= (room->bounds.x + room->bounds.w)) &&
	    (y >= room->bounds.y && y <= (room->bounds.y + room->bounds.h)) &&
	    (z >= room->bounds.z && z <= (room->bounds.z + room->bounds.d)))
		return true;
}
