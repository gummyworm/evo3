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

/* GetSpriteBounds sets the (x, y, w, h) rectangle that bounds the sprite
 * attached to e. z is set to the distance the sprite is from the player. */
bool GetSpriteBounds(Entity e, int *x, int *y, float *z, int *w, int *h) {
	struct Sprite *s;
	vec3 lpos;
	vec3 ppos;
	vec3 dist;

	if (!Enabled(e))
		return false;
	if ((s = getSprite(e)) == NULL)
		return false;
	if (!GetPos(e, &lpos[0], &lpos[1], &lpos[2]))
		return false;

	GetPos(E_PLAYER, &ppos[0], &ppos[1], &ppos[2]);
	vec3_sub(dist, lpos, ppos);

	WorldToScreen(E_PLAYER, lpos[0], lpos[1], lpos[2], x, y);

	*z = vec3_len(dist);
	*w = ((float)s->w) / *z;
	*h = ((float)s->h) / *z;

	if (*x >= 0 && *y >= 0) {
		ScreenToGui(*x, *y, x, y);
		*y -= *h;
		return true;
	}
	return true;
}

/* UpdateSpriteSystem updates all sprites that have been created. */
void UpdateSpriteSystem() {
	int i;
	mat4x4 proj;

	GuiProjection(&proj);

	for (i = 0; i < numSprites; ++i) {
		float z;
		int x, y, w, h;
		if (!GetSpriteBounds(sprites[i].e, &x, &y, &z, &w, &h))
			continue;
		TexRectZ(proj, getTextureProgram(), x, y, 1.f, w, h, 0, 0, 1, 1,
		         sprites[i].texture);
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
	sprites[numSprites].scale = scale;
	sprites[numSprites].w = scale * 128;
	sprites[numSprites].h = scale * 128;
	sprites[numSprites].texture = GetTexture(filename);

	HASH_ADD_INT(entitiesToSprites, e, item);
	numSprites++;
}

/* GetSpriteUpdates returns any updates made to sprites this frame and
* sets
* numUpdates to the number of sprites updated. */
struct SpriteUpdate *GetSpriteUpdates(int *num) {
	*num = numUpdates;
	return updates;
}

/* SpritePick returns the entity that contains the given screen coordinates
 * (or 0 if there were no matches). */
Entity SpritePick(Entity e, int px, int py) {
	UNUSED(e);
	int i;
	Entity picked;
	float pickedZ;

	picked = 0;
	pickedZ = 100000.f;

	for (i = 0; i < numSprites; ++i) {
		int x, y, w, h;
		float z;
		GetSpriteBounds(sprites[i].e, &x, &y, &z, &w, &h);
		if ((px > x) && (px < (x + w))) {
			if ((py > y) && (py < (y + h))) {
				if (picked == 0 || z < pickedZ) {
					if (GetThingName(sprites[i].e)) {
						pickedZ = z;
						picked = sprites[i].e;
					}
				}
			}
		}
	}

	return picked;
}
