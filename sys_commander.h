#ifndef SYS_COMMANDER_H
#define SYS_COMMANDER_H

#include "entity.h"

enum { MAX_COMMANDERS = 8 };

/* Commader is a struct that represents a commander component.
 */
struct Commander {
	Entity e;
	struct {
		double x, y, w, h;
		bool selecting;
	} selection;
};

/* CommanderUpdate defines an update message that can be polled by interested
 * systems. */
struct CommanderUpdate {
	Entity e;
};

void InitCommanderSystem();
void UpdateCommanderSystem();
void AddCommander(Entity);
struct CommanderUpdate *GetCommanderUpdates(int *);

#endif
