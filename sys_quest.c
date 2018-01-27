#include "sys_quest.h"
#include "entities.h"
#include "sys_actor.h"
#include "sys_camera.h"
#include "sys_child.h"
#include "third-party/include/uthash.h"

SYSDEF(Quest, const char *name, bool *completeFlag, void (*onComplete)(Entity))
strncpy(C->name, name, sizeof(C->name));
C->completed = completeFlag;
C->onComplete = onComplete;
}

/* init initializes the quest system. */
void init() {}

/* update updates all Quest components that have been created. */
void update(struct Quest *components, int num) {
	int i;

	/* check if a quest has been completed. */
	for (i = 0; i < num; ++i) {
		struct Quest *q;
		if ((q = GetQuest(components[i].e))) {
			if (*q->completed) {
				q->onComplete(components[i].e);
			}
		}
	}
}
