#ifndef SYS_SPRITE_H
#define SYS_SPRITE_H

#include "entity.h"
#include "system.h"
#include <OpenGL/gl.h>

enum { MAX_SPRITES = 4096 };

/* Sprite is a struct that represents a movable, visible object in the world. */
struct Sprite {
	int x, y;
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
void NewSprite(Entity, const char *);
struct SpriteUpdate *GetSpriteUpdates(int *);
void MoveSprite(Entity, int, int);

#endif
