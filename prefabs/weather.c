#include "../entity.h"
#include "../systems.h"

void Weather(Entity e, float x, float y, float z) {
	AddTransform(e, x, y, z);
	AddWeather(e, 72.f, 30.f);
}
