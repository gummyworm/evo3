#ifndef SYS_COMMANDER_H
#define SYS_COMMANDER_H

#include "entity.h"

enum { MAX_COMMANDERS = 8 };

/* Commader is a struct that represents a commander component.
 */
struct Commander {};

/* CommanderUpdate defines an update message that can be polled by interested
 * systems. */
struct CommanderUpdate {
	Entity e;
};

void InitCommanderSystem();
void UpdateCommanderSystem();
void NewCommander(Entity);
struct CommanderUpdate *GetCommanderUpdates(int *);

#endif
