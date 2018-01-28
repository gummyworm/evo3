#include "../systems.h"
#include "tests.h"
#include <assert.h>

static bool pass;
static bool chk() { return true; }
void done() { pass = true; }

void testQuestSystem(struct Test *t) {
	Entity e = 1;

	InitTransformSystem();
	InitQuestSystem();

	AddQuest(e, "test quest", chk, done);

	UpdateQuestSystem();
	tassert(t, GetQuest(e));
	UpdateQuestSystem();

	tassert(t, pass);
}
