#include "../entity.h"
#include "../systems.h"

int Berry(int e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddRender(e, "cube.obj");
	AddLabel(e, "BERRY", 0.8f);
	AddItem(e, "BERRY", "A tasty looking berry");
	return 0;
}
