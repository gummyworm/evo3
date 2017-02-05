#include "../entity.h"
#include "../systems.h"

void Snake(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddLabel(e, "SNAKE", 1.8f);
	AddLabelRect(e, 10, 4, 0xff0000ff);
	AddSprite(e, "res/snake2.png", 3.f, 2.f);
	AddActor(e, "SNAKE", "A frightening looking serpent");
}
