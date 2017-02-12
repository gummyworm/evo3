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

	GetViewPos(E_PLAYER, &ppos[0], &ppos[1], &ppos[2]);
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
	return false;
}

/* GetSpriteTexture returns the sprite attached to e's texture. */
GLuint GetSpriteTexture(Entity e) {
	struct Sprite *s;

	if ((s = getSprite(e)) == NULL)
		return 0;

	return s->texture;
}

/* UpdateSpriteSystem updates all sprites that have been created. */
void UpdateSpriteSystem() {
	int i;
	mat4x4 proj;
	GLint loc;

	GuiProjection(proj);

	for (i = 0; i < numSprites; ++i) {
		struct Sprite *s;
		float z;
		int x, y, w, h;
		s = sprites + i;
		if (!GetSpriteBounds(s->e, &x, &y, &z, &w, &h))
			continue;
		if (s->castShadow)
			TexRectZ(proj, SHADOW_PROGRAM, x + 5, y + 5, 0.9f, w, h,
			         0, 0, 1, 1, s->texture);
	}
	UseProgram(TEXTURE_PROGRAM);
	for (i = 0; i < numSprites; ++i) {
		struct Sprite *s;
		float z;
		int x, y, w, h;

		s = sprites + i;
		if (!GetSpriteBounds(s->e, &x, &y, &z, &w, &h))
			continue;
		SetUColor(TEXTURE_PROGRAM, s->r, s->g, s->b, s->a);
		TexRectZ(proj, TEXTURE_PROGRAM, x, y, 1.f, w, h, 0, 0, 1, 1,
		         s->texture);
		if (loc >= 0)
			glUniform4f(loc, 0, 0, 0, 0);
	}
}

/* AddSprite creates a new sprite attached to the entity e. */
void AddSprite(Entity e, const char *filename, float xscale, float yscale) {
	struct entityToSprite *item;

	if (getSprite(e) != NULL)
		return;

	item = malloc(sizeof(struct entityToSprite));
	item->sprite = sprites + numSprites;
	item->e = e;

	sprites[numSprites].e = e;
	sprites[numSprites].w = xscale * 128;
	sprites[numSprites].h = yscale * 128;
	sprites[numSprites].texture = GetTexture(filename);
	sprites[numSprites].castShadow = true;

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
		if (!GetSpriteBounds(sprites[i].e, &x, &y, &z, &w, &h))
			continue;
		if ((px > x) && (px < (x + w)) && (py > y) && (py < (y + h))) {
			if (picked == 0 || z < pickedZ) {
				/* only pick "Thing"s and "Actor"s */
				if (GetThingName(sprites[i].e) ||
				    GetActorName(sprites[i].e)) {
					pickedZ = z;
					picked = sprites[i].e;
				}
			}
		}
	}

	return picked;
}

/* contains returns true if pt-(pt+sdim) overlaps the given bounds. */
static bool contains(vec2 l1, vec2 sdim, vec2 center, vec2 dim) {
	vec2 r1, r2;
	vec2 l2 = {center[0] - dim[0] / 2.f, center[1] - dim[1] / 2.f};
	vec2_add(r1, l1, sdim);
	vec2_add(r2, l2, dim);

	if (l2[0] > r2[0]) {
		float t;
		t = r2[0];
		r2[0] = l2[0];
		l2[0] = t;
	}
	if (l2[1] > r2[1]) {
		float t;
		t = r2[1];
		r2[1] = l2[1];
		l2[1] = t;
	}

	if ((l1[0] > r2[0]) || (l2[0] > r1[0]))
		return false;
	if ((l2[1] > r1[1]) || (r1[1] < l2[1]))
		return false;
	return true;
}

/* GetSpritesInBounds sets up to max entities found in the bounds and returns
 * the number found.  A function, filter, may be passed.  If not NULL, it should
 * return true if an entity is to be included in found. */
int GetSpritesInBounds(Entity *found, int max, vec2 center, vec2 dim,
                       bool (*filter)(Entity)) {
	int i, numFound;
	for (i = 0, numFound = 0; i < numSprites && numFound < max; ++i) {
		vec2 pos;
		vec2 sdim;
		int x, y;
		float z;
		int w, h;

		GetSpriteBounds(sprites[i].e, &x, &y, &z, &w, &h);
		pos[0] = x;
		pos[1] = y;
		sdim[0] = w;
		sdim[1] = h;
		if (contains(pos, sdim, center, dim)) {
			if (filter == NULL)
				found[numFound++] = sprites[i].e;
			else if (filter(sprites[i].e))
				found[numFound++] = sprites[i].e;
		}
	}
	return numFound;
}

/* SetSpriteColor sets the color of the sprite to (r,g,b,a). */
void SetSpriteColor(Entity e, float r, float g, float b, float a) {
	struct Sprite *s;

	if ((s = getSprite(e)) == NULL)
		return;
	s->r = r;
	s->g = g;
	s->b = b;
	s->a = a;
}
