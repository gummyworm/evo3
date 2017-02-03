#include "../entity.h"
#include "../systems.h"

int Berry(int e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddColorRender(e, "cube.obj", 1.f, 0.2f, 1.f, 1.f);
	AddLabel(e, "BERRY", 0.8f);
	AddItem(e, "BERRY", "A tasty looking berry");
	return 0;
}
