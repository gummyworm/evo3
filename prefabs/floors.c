#include "../entity.h"
#include "../systems.h"

int Floor1(int e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	SetScale(e, 100.f, 1.f, 100.f);
	AddColorRender(e, "cube.obj", 0.0f, 0.4f, 0.1f, 1.f);
	return 0;
}
