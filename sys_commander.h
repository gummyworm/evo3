#ifndef SYS_COMMANDER_H
#define SYS_COMMANDER_H

#include "entity.h"
#include "third-party/include/linmath.h"

enum { MAX_COMMANDERS = 8, MAX_SELECTION = 16 };

/* Commader is a struct that represents a commander component.
 */
struct Commander {
	Entity e;
	vec2 panSpeed;
	float zoomSpeed;
	vec2 zoomLimits;
	struct {
		double x, y, w, h;
		bool selecting;
	} selection;
	Entity selected[MAX_SELECTION];
	int numSelected;
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
