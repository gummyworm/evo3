#include "sys_room.h"
#include "sys_transform.h"
#include "third-party/include/uthash.h"

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
void AddRoom(Entity e, const char *name, const char *desc, float x, float y,
             float z, float w, float h, float d) {
	struct entityToRoom *item;

	if (getRoom(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToRoom));
	item->room = rooms + numRooms;
	item->e = e;

	rooms[numRooms].e = e;
	rooms[numRooms].name = name;
	rooms[numRooms].desc = desc;
	rooms[numRooms].bounds.x = x;
	rooms[numRooms].bounds.y = y;
	rooms[numRooms].bounds.z = z;
	rooms[numRooms].bounds.w = w;
	rooms[numRooms].bounds.h = h;
	rooms[numRooms].bounds.d = d;

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

/* GetRoom returns the room that contains e (or -1 if no room does). */
Entity GetRoom(Entity e) {
	int i;
	float x, y, z;

	if ((GetPos(e, &x, &y, &z) == false))
		return -1;

	for (i = 0; i < numRooms; ++i) {
		if (RoomContains(rooms[i].e, x, y, z))
			return rooms[i].e;
	}

	return -1;
}

/* GetDescription returns e's room's description. */
const char *GetDescription(Entity e) {
	struct Room *room;

	room = getRoom(e);
	if (room == NULL)
		return NULL;

	return room->desc;
}
