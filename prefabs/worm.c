#include "../entity.h"
#include "../systems.h"

void Worm(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddLabel(e, "WORM", 1.8f);
	AddLabelRect(e, 10, 4, 0xff0000ff);
	AddSprite(e, "res/strawberry.png", 3.f, 2.f);
	AddUnit(e, "WORM", 100);
	AddActor(e, "WORM", "A faithuful anelidic beast");
}
