#ifndef SYS_SPRITE_H
#define SYS_SPRITE_H

#include "entity.h"
#include "system.h"
#include "third-party/include/linmath.h"
#include <OpenGL/gl.h>

enum { MAX_SPRITES = 4096 };

/* Sprite is a struct that represents a movable, visible object in the world. */
struct Sprite {
	Entity e;
	int w, h;
	float scale;
	GLuint texture;
	float r, g, b, a;
};

/* SpriteUpdate defines an update message that can be polled by interested
 * systems. */
struct SpriteUpdate {
	Entity e;
};

void InitSpriteSystem();
void UpdateSpriteSystem();
void AddSprite(Entity, const char *, float, float);
bool GetSpriteBounds(Entity, int *, int *, float *, int *, int *);
GLuint GetSpriteTexture(Entity);
Entity SpritePick(Entity, int, int);
void SetSpriteColor(Entity, float, float, float, float);
struct SpriteUpdate *GetSpriteUpdates(int *);
int GetSpritesInBounds(Entity *, int, vec2, vec2, bool (*)(Entity));

#endif
