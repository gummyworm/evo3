#ifndef SYS_QUEST_H
#define SYS_QUEST_H

#include "base.h"
#include "entity.h"
#include "system.h"

SYSTEM(Quest, const char *name) {
	Entity e;
	char name[31];
};

struct Quest *GetQuest(Entity);

#endif
