#ifndef SYS_QUEST_H
#define SYS_QUEST_H

#include "base.h"
#include "entity.h"
#include "system.h"

SYSTEM(Quest, const char *name, bool (*isComplete)(),
       void (*onComplete)(Entity)) {
	Entity e;
	bool (*isComplete)();
	void (*onComplete)(Entity);
	char name[31];
};

struct Quest *GetQuest(Entity);

#endif
