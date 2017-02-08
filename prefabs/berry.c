#include "../entity.h"
#include "../systems.h"

int Berry(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddLabel(e, "BERRY", 0.8f);
	AddSprite(e, "res/strawberry.png", 1.f, 1.f);
	AddItem(e, "BERRY", "A tasty looking berry");
	return 0;
}

int Banana(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddLabel(e, "BANANA", 0.8f);
	AddSprite(e, "res/banana.png", 1.f, 1.f);
	AddItem(e, "BANANA", "A yellow banana");
	return 0;
}
