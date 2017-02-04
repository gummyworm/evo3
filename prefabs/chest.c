#include "../entity.h"
#include "../systems.h"

void Chest(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddSprite(e, "res/chest.png", 1.f);
	AddLabel(e, "chest", 1.0f);
	AddContainer(e, "CHEST", "GOOOOOOOOOLLLLLD");
}
