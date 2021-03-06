#ifndef PREFABS_H
#define PREFABS_H

#include "entity.h"

int GUIAlert(Entity, const char *);
int GUI(Entity);
int Berry(Entity, float, float, float);
int Player(Entity);
int OverworldCommander(Entity);
int Banana(Entity, float, float, float);
int Grass1(Entity, float, float, float);
int Tree1(Entity, float, float, float);
int Floor1(Entity, float, float, float);
int Floor2(Entity, float, float, float);
int WallEW(Entity, float, float, float);
int WallNS(Entity, float, float, float);
int OverworldBG(Entity, float, float, float);
int Bullet(Entity);
int Gun(Entity, Entity);
void Chest(Entity, float, float, float);
void Snake(Entity, float, float, float);
void Weather(Entity, float, float, float);
void Worm(Entity, float, float, float);
void TestMap(Entity, float, float, float);

#endif
