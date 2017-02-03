#include "sys_room.h"
#include "entities.h"
#include "sys_transform.h"
#include "thing.h"
#include "third-party/include/uthash.h"

struct entityToRoom {
	Entity e;
	struct Room *room;
	UT_hash_handle hh;
};

extern int Berry(int e, float x, float y, float z);

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

/* RemoveRoom removes the room attached to e from the Room system. */
void RemoveRoom(Entity e) {
	struct entityToRoom *c;

	if (entitiesToRooms == NULL)
		return;

	HASH_FIND_INT(entitiesToRooms, &e, c);
	if (c != NULL) {
		struct Room *sys = c->room;
		int sz = (rooms + numRooms) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToRooms, c);
		free(c);
	}
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

	return false;
}

/* RoomContainsEntity returns true if target's transform resides within the room
 * attached to e. */
bool RoomContainsEntity(Entity e, Entity target) {
	float x, y, z;
	if (!GetPos(target, &x, &y, &z))
		return false;
	return RoomContains(e, x, y, z);
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

/* GetRoomDescription returns e's room's description. */
const char *GetRoomDescription(Entity e) {
	struct Room *room;

	room = getRoom(e);
	if (room == NULL)
		return NULL;

	return room->desc;
}

/* ThingsInRoom returns all the Things in the room attached to e and returns
 * the number found. */
int ThingsInRoom(Entity e, Entity *found) {
	struct Thing *things;
	int i, num, count;

	things = GetThings(&num);
	for (i = 0, count = 0; i < num; ++i) {
		float x, y, z;

		if (GetPos(things[i].e, &x, &y, &z) && RoomContains(e, x, y, z))
			found[count++] = things[i].e;
	}
	return count;
}

extern void genFloor(Entity e);

/* GenerateRoom creates a new room and attaches it to e.  It then adds a number
 * of additional entities (influenced by the given room type). */
void GenerateRoom(Entity e, enum RoomType type) {
	const int size = 10;
	int i;

	AddRoom(e, "GENERATED ROOM", "This test room was generated", -size,
	        -size, -size, size * 2, size * 2, size * 2);

	switch (type) {
	case FLOOR:
		break;
	case CANOPY:
		break;
	}

	genFloor(E_ROOM);
	for (i = 1; i < 10; i += 2) {
		float x = rand() % (size * 2) - size;
		float y = 0;
		float z = rand() % (size * 2) - size;

		Tree1(E_ROOM + i, x + .5f, y, z + .1f);
		Berry(E_ROOM + i + 1, x, y, z);
	}
}
