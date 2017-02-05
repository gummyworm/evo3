#include "sys_party.h"
#include "draw.h"
#include "systems.h"
#include "third-party/include/uthash.h"

struct entityToPartyMember {
	Entity e;
	struct PartyMember *member;
	UT_hash_handle hh;
};

static struct entityToPartyMember *entitiesToPartyMembers;
static struct PartyMember partyMembers[MAX_PARTY_MEMBERS];
static int numPartyMembers;

int numPartyUpdates;
static struct PartyUpdate updates[MAX_PARTY_MEMBERS];

/* getParty returns the party attached to entity e (if there is one). */
static struct PartyMember *getParty(Entity e) {
	struct entityToPartyMember *t;

	if (entitiesToPartyMembers == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToPartyMembers, &e, t);
	if (t == NULL)
		return NULL;

	return t->member;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct PartyUpdate *u) {
	updates[numPartyUpdates++] = *u;
}

/* InitPartySystem initializes the party system. */
void InitPartySystem() {}

/* UpdatePartySystem updates all partys that have been created. */
void UpdatePartySystem() {
	int i;
	int x, y, w, h;
	mat4x4 proj;

	GuiProjection(proj);
	w = 20;
	h = 20;
	x = 0;
	y = 0;
	for (i = 0; i < numPartyMembers; ++i) {
		TexRect(proj, getTextureProgram(), x, y, w, h, 0.f, 0.f, 1.f,
		        1.f, partyMembers[i].portraitTexture);
		y += h;
	}
}

/* AddPartyMember adds a party member component to the entity e. */
void AddPartyMember(Entity e, const char *portrait) {
	struct entityToPartyMember *item;

	if (getParty(e) != NULL)
		return;
	item = malloc(sizeof(struct entityToPartyMember));
	item->member = partyMembers + numPartyMembers;
	item->e = e;
	HASH_ADD_INT(entitiesToPartyMembers, e, item);

	partyMembers[numPartyMembers].e = e;
	partyMembers[numPartyMembers].portraitTexture = GetTexture(portrait);

	numPartyMembers++;
}

/* RemovePartyMember removes the party member attached to e from the Party
 * system. */
void RemovePartyMember(Entity e) {
	struct entityToPartyMember *c;

	if (entitiesToPartyMembers == NULL)
		return;

	HASH_FIND_INT(entitiesToPartyMembers, &e, c);
	if (c != NULL) {
		struct PartyMember *sys = c->member;
		int sz = (partyMembers + numPartyMembers) - sys;
		memmove(sys, sys + 1, sz);
		HASH_DEL(entitiesToPartyMembers, c);
		free(c);
	}
}
