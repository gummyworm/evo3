#ifndef PREFABS_H
#define PREFABS_H

#include "entity.h"

int Berry(Entity, float, float, float);
int Banana(Entity, float, float, float);
int Grass1(Entity, float, float, float);
int Tree1(Entity, float, float, float);
int Floor1(Entity, float, float, float);
int Floor2(Entity, float, float, float);
int WallEW(Entity, float, float, float);
int WallNS(Entity, float, float, float);
void Chest(Entity, float, float, float);
void Snake(Entity, float, float, float);
void Weather(Entity e, float x, float y, float z);

#endif
