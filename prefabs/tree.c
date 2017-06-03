#include "../entity.h"
#include "../systems.h"

int Tree1(Entity e, float x, float y, float z) {
	float scale = 5.f;

	AddTransform(e, x, y, z);
	// AddColorRender(e, "tree.obj", 0.f, 1.f, 0.3f, 1.f);
	AddSprite(e, "res/tree.png", scale, scale);
	AddLabel(e, "TREE", 5.f);
	AddCollider(e, 1.f, 0);
	return 0;
}
