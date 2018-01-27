#include "sys_quest.h"
#include "entities.h"
#include "sys_actor.h"
#include "sys_camera.h"
#include "sys_child.h"
#include "third-party/include/uthash.h"

SYSDEF(Quest, const char *name)
strncpy(C->name, name, sizeof(C->name));
}

/* init initializes the quest system. */
void init() {}

/* update updates all Quest components that have been created. */
void update(struct Quest *components, int num) {
	UNUSED(components);
	UNUSED(num);
}
