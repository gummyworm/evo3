#include "../entity.h"
#include "../systems.h"

int TestRoom(Entity e) {
	AddTransform(e, 0, 0, 0);
	AddRoom(e, "Test Room", "This is a test room", -100, -100, -100, 200,
	        200, 200);
	return 0;
}
