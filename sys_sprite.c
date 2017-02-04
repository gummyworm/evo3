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

		w = sprites[i].w / vec3_len(dist);
		h = sprites[i].h / vec3_len(dist);

		if (sx >= 0) {
			mat4x4 proj;
			ScreenToGui(sx, sy, &sx, &sy);
			GuiProjection(&proj);
			TexRect(proj, getTextureProgram(), sx, sy, w, h, 0, 0,
			        1, 1, sprites[i].texture);
		}
	}
}

/* NewSprite creates a new sprite attached to the entity e. */
void NewSprite(Entity e, const char *filename) {
	struct entityToSprite *item;

	if (getSprite(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToSprite));
	item->sprite = sprites + numSprites;
	item->e = e;

	sprites[numSprites].texture = SOIL_load_OGL_texture(
	    filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
	    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
	        SOIL_FLAG_COMPRESS_TO_DXT);

	HASH_ADD_INT(entitiesToSprites, e, item);
	numSprites++;
}

/* GetSpriteUpdates returns any updates made to sprites this frame and sets
* numUpdates to the number of sprites updated. */
struct SpriteUpdate *GetSpriteUpdates(int *num) {
	*num = numUpdates;
	return updates;
}

/* MoveSprite moves the sprite associated with the entity e by (dx, dy). */
void MoveSprite(Entity e, int dx, int dy) {
	UNUSED(dx);
	UNUSED(dy);

	{
		struct SpriteUpdate u = {e};
		addUpdate(&u);
	}
}
