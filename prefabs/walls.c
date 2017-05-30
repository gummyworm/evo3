#include "../entity.h"
#include "../systems.h"

int WallEW(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	TransformSetScale(e, 1.f, 10.f, 100.f);
	AddColorRender(e, "cube.obj", 0.2f, 0.4f, 0.15f, 1.f);
	return 0;
}

int WallNS(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	TransformSetScale(e, 100.f, 10.f, 1.f);
	AddColorRender(e, "cube.obj", 0.2f, 0.4f, 0.15f, 1.f);
	return 0;
}

int OverworldBG(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	TransformSetScale(e, 1000.f, 1000.f, 1.f);
	AddColorRender(e, "cube.obj", 0.2f, 0.4f, 0.15f, 1.f);
	return 0;
}
