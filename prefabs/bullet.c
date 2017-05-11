#include "../entity.h"
#include "../systems.h"

int Bullet(Entity e) {
	AddTransform(e, 0, 0, 0);
	SetScale(e, .1f, .1f, .1f);

	AddDamage(e, 10.f);
	AddMovement(e, 0);
	AddColorRender(e, "cube.obj", 0.1f, 0.1f, 0.1f, 1.f);
	return 0;
}
