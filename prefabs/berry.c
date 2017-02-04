#include "../entity.h"
#include "../systems.h"

int Berry(int e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddLabel(e, "BERRY", 0.8f);
	AddSprite(e, "res/strawberry.png", 1.0f);
	AddItem(e, "BERRY", "A tasty looking berry");
	return 0;
}
