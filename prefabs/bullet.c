#include "../entity.h"
#include "../systems.h"

int Bullet(Entity e) {
	vec3 dir = {0.f, 0.f, 1.f};

	AddTransform(e, 0, 0, 0);
	TransformSetScale(e, .1f, .1f, .1f);

	AddDamage(e, 10.f);
	AddMovement(e, 19.0f, dir);
	AddCollider(e, 1.f, 0);
	AddColorRender(e, "cube.obj", 0.1f, 0.1f, 0.1f, 1.f);
	return 0;
}
