#ifndef SYS_PARTY_H
#define SYS_PARTY_H

#include "entity.h"
#include "system.h"
#include <OpenGL/gl.h>

enum { MAX_PARTY_MEMBERS = 16 };

/* PartyMember is a struct that represents a member of the player's party. */
struct PartyMember {
	Entity e;
	GLuint portraitTexture;
};

/* PartyUpdate defines an update message that can be polled by interested
 * systems. */
struct PartyUpdate {
	Entity e;
};

void InitPartySystem();
void UpdatePartySystem();
void AddPartyMember(Entity, const char *);
void RemovePartyMember(Entity);

#endif
