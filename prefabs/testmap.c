#include "../entity.h"
#include "../systems.h"

void TestMap(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddTileMap(e, "res/maps/test.json");
}
