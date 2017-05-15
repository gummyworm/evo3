#include "entity.h"

Entity NewEntity() {
	static Entity next = MAX_USER_ENTITIES;
	return next++;
}
