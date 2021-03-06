#include "../entity.h"
#include "../systems.h"

int Floor1(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	TransformSetScale(e, 100.f, 1.f, 100.f);
	AddColorRender(e, "cube.obj", 0.2f, 0.3f, 0.1f, 1.f);
	return 0;
}

int Floor2(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	TransformSetScale(e, 100.f, 1.f, 100.f);
	AddColorRender(e, "cube.obj", 0.3f, 0.5f, 0.9f, 1.f);
	return 0;
}
