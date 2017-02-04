#include "draw.h"
#include "entities.h"
#include "systems.h"
#include "third-party/include/uthash.h"
#include <SOIL.h>

struct entityToSprite {
	Entity e;
	struct Sprite *sprite;
	UT_hash_handle hh;
};

static struct entityToSprite *entitiesToSprites;
static struct Sprite sprites[MAX_SPRITES];
static int numSprites;

static int numUpdates;
static struct SpriteUpdate updates[MAX_SPRITES];

/* getSprite returns the sprite attached to entity e (if there is one). */
static struct Sprite *getSprite(Entity e) {
	struct entityToSprite *s;

	if (entitiesToSprites == NULL)
		return NULL;

	HASH_FIND_INT(entitiesToSprites, &e, s);
	if (s == NULL)
		return NULL;

	return s->sprite;
}

/* addUpdate adds a new update for this frame. */
static void addUpdate(struct SpriteUpdate *u) { updates[numUpdates++] = *u; }

/* InitSpriteSystem initializes the sprite system. */
void InitSpriteSystem() {}

/* UpdateSpriteSystem updates all sprites that have been created. */
void UpdateSpriteSystem() {
	int i;
	for (i = 0; i < numSprites; ++i) {
		int sx, sy;
		float z;
		int w, h;
		vec3 lpos;
		vec3 ppos;
		vec3 dist;

		if (!Enabled(sprites[i].e))
			continue;

		if (!GetPos(sprites[i].e, &lpos[0], &lpos[1], &lpos[2]))
			continue;

		WorldToScreen(E_PLAYER, lpos[0], lpos[1], lpos[2], &sx, &sy);

		GetPos(E_PLAYER, &ppos[0], &ppos[1], &ppos[2]);
		vec3_sub(dist, lpos, ppos);

		z = vec3_len(dist);
		w = ((float)sprites[i].w) / z;
		h = ((float)sprites[i].h) / z;

		if (sx >= 0) {
			mat4x4 proj;
			GuiProjection(&proj);
			ScreenToGui(sx, sy, &sx, &sy);
			TexRectZ(proj, getTextureProgram(), sx, sy, 1.f, w, h,
			         0, 0, 1, 1, sprites[i].texture);
		}
	}
}

/* AddSprite creates a new sprite attached to the entity e. */
void AddSprite(Entity e, const char *filename, float scale) {
	struct entityToSprite *item;

	if (getSprite(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToSprite));
	item->sprite = sprites + numSprites;
	item->e = e;

	sprites[numSprites].e = e;
	sprites[numSprites].w = scale * 128;
	sprites[numSprites].h = scale * 128;
	sprites[numSprites].texture = GetTexture(filename);

	HASH_ADD_INT(entitiesToSprites, e, item);
	numSprites++;
}

/* GetSpriteUpdates returns any updates made to sprites this frame and sets
* numUpdates to the number of sprites updated. */
struct SpriteUpdate *GetSpriteUpdates(int *num) {
	*num = numUpdates;
	return updates;
}
