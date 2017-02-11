#ifndef ROOM_GEN_H
#define ROOM_GEN_H

#include "entity.h"

void genFloor(Entity);
void genCeiling(Entity);
void genWalls(Entity, Entity, Entity, Entity);
void genOverworld(Entity);

#endif
