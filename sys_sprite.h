#ifndef SYS_SPRITE_H
#define SYS_SPRITE_H

#include "entity.h"
#include "system.h"
#include <OpenGL/gl.h>

enum { MAX_SPRITES = 4096 };

/* Sprite is a struct that represents a movable, visible object in the world. */
struct Sprite {
	Entity e;
	int w, h;
	GLuint texture;
};

/* SpriteUpdate defines an update message that can be polled by interested
 * systems. */
struct SpriteUpdate {
	Entity e;
};

void InitSpriteSystem();
void UpdateSpriteSystem();
void AddSprite(Entity, const char *, float scale);
struct SpriteUpdate *GetSpriteUpdates(int *);

#endif
