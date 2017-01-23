#include "../entity.h"
#include "../systems.h"

int Player(int e) {
	AddTransform(e, 0, 0, 0);
	AddCamera(e, 0);
	AddFPSController(e, 2.35f);
	CameraPerspective(e, 90.0f, 640.0f / 480.0f);
	return 0;
}
