#include "../entity.h"
#include "../systems.h"

int Tree1(int e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddColorRender(e, "tree.obj", 0.f, 1.f, 0.3f, 1.f);
	AddLabel(e, "TREE", 0.8f);
	AddItem(e, "TREE",
	        "One of countless trees that comprise the massive jungle.");
	return 0;
}
