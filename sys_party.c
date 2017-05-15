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

/* InitPartySystem initializes the party system. */
void InitPartySystem() {}

/* UpdatePartySystem updates all partys that have been created. */
void UpdatePartySystem() {
	static GLuint overlay;
	int i;
	int x, y, w, h;
	int hpw, manaw;
	mat4x4 proj;

	if (overlay == 0)
		if ((overlay = GetTexture("res/portraitoverlay.png")) == 0)
			return;

	GuiProjection(proj);
	w = 30;
	h = 33;
	x = 0;
	y = 0;
	hpw = 2;
	manaw = 2;
	for (i = 0; i < 5; ++i) {
		Rect(proj, x, y, w, h, 0x000000ff);
		if (i < numPartyMembers) {
			int currHp, maxHp;
			TexRect(proj, TEXTURE_PROGRAM, x, y, w, h, 0.f, 0.f,
			        1.f, 1.f, partyMembers[i].portraitTexture);

			currHp = GetHP(partyMembers[i].e);
			maxHp = GetMaxHP(partyMembers[i].e);
			Rect(proj, x + w, y, hpw, h, 0x000000ff);
			if (maxHp == 0)
				maxHp = currHp;
			Rect(proj, x + w, y, hpw, (float)(currHp / maxHp) * h,
			     0xf00000ff);
		}
		TexRect(proj, TEXTURE_PROGRAM, x, y, w, h, 0.f, 0.f, 1.f, 1.f,
		        overlay);
		y += h;
	}

	w = 30;
	h = 33;
	x = GUI_WIDTH - w;
	y = 0;
	hpw = 2;
	manaw = 2;
	for (i = 0; i < 5; ++i) {
		Rect(proj, x, y, w, h, 0x000000ff);
		if ((i + 5) < numPartyMembers) {
			int currHp, maxHp;
			TexRect(proj, TEXTURE_PROGRAM, x, y, w, h, 0.f, 0.f,
			        1.f, 1.f, partyMembers[i].portraitTexture);

			currHp = GetHP(partyMembers[i].e);
			maxHp = GetMaxHP(partyMembers[i].e);
			if (maxHp == 0)
				maxHp = currHp;
			Rect(proj, x - hpw, y, hpw, h, 0x000000ff);
			Rect(proj, x - hpw, y, hpw, (float)(currHp / maxHp) * h,
			     0xf00000ff);
		}
		TexRect(proj, TEXTURE_PROGRAM, x, y, w, h, 0.f, 0.f, 1.f, 1.f,
		        overlay);
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
		numPartyMembers--;
	}
}
