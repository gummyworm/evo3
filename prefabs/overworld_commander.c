#include "../entity.h"
#include "../systems.h"

int OverworldCommander(Entity e) {
	AddTransform(e, 0, 0, 0);
	AddCamera(e, 0);
	AddPartyMember(e, "res/banana.png");
	AddCommander(e);
	CameraPerspective(e, 45.0f, 640.0f / 480.0f);
	AddCommander(e);
	AddConsole(e);
	return 0;
}
