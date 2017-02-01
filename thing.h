#ifndef THING_H
#define THING_H

#include "third-party/include/utarray.h"
#include "third-party/include/uthash.h"

#include "entity.h"
#include <stdbool.h>

extern const char *ACTION_TAKE;

enum { MAX_THINGS = 8192,
       MAX_THINGS_PER_ROOM = 128,
};

/* Thing is a struct that represents an inanimate object's attributes. */
struct Thing {
	Entity e;
	Entity owner;

	const char *name;
	const char *brief;
	const char *desc;
	int weight;

	bool takeable;

	UT_array *contents;
	union {
		struct {
			bool open;
		} container;
		struct {
			bool open;
		} door;
	} properties;

	struct ActionHandler *actions;
};

/* Action returns true if self successfully handles the act associated with
 * this handler. out is written with any messages to be conveyed to the user. */
typedef bool (*Action)(Entity self, Entity prop, Entity actor, char *out);

/* ActionHandler defines callbacks to be executed when a given action
 * is performed on a Thing. */
struct ActionHandler {
	const char *action;
	Action handler;
	UT_hash_handle hh;
};

/* ThingUpdate defines an update message that can be polled by
 * interested systems. */
struct ThingUpdate {
	Entity e;
};

void InitThingSystem();
void UpdateThingSystem();
void AddThing(Entity, const char *, const char *);
void RemoveThing(Entity);
Entity GetThing(const char *);
struct Thing *GetThings(int *num);

const char *GetThingName(Entity e);
const char *GetThingDescription(Entity e);

void AddActionHandler(Entity, const char *, Action handler);
bool HandleAction(Entity, Entity, Entity, char *, char *);

void AddItem(Entity, const char *, const char *);
void AddContainer(Entity, const char *, const char *);
void AddToContainer(Entity, Entity);
bool GetFromContainer(Entity e, Entity item);

#endif
