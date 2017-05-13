#include "../entity.h"
#include "../prefabs.h"
#include "../systems.h"

int Gun(Entity e, Entity holder) {
	AddChild(e, holder);
	AddGun(e, Bullet, 0.5f);
	AddGunController(e);
	return 0;
}
