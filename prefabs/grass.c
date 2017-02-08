#include "../entity.h"
#include "../systems.h"

int Grass1(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddSprite(e, "res/grass.png", 5.f, 1.f);
	return 0;
}
