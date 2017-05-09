#include "../entity.h"
#include "../systems.h"

int Gun(Entity e) {
	AddTransform(e, 0, 0, 0);
	return 0;
}
