#include "prefabs.h"
#include "systems.h"

/* genFloor generates a floor and attaches it to the entity e. */
void genFloor(Entity e) { Floor1(e, 1, -1, 1); }

void genCeiling(Entity e) { Floor2(e, 0, 10, 0); }

/* genWalls creates 4 walls, one for each provided entity. */
void genWalls(Entity n, Entity s, Entity e, Entity w) {
	WallNS(n, 0, 0, -10.f);
	WallNS(s, 0, 0, 10.f);
	WallEW(e, -10.f, 0.f, 0.f);
	WallEW(w, 10.f, 0.f, 0.f);
}
