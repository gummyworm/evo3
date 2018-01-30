#include "../entity.h"
#include "../prefabs.h"
#include "../systems.h"

int GUI(Entity e) {
	AddTextBox(e, 10, 20, "TEST");
	return 0;
}

int GUIAlert(Entity e, const char *msg) {
	AddTextBox(e, 10, 20, msg);
	return 0;
}
