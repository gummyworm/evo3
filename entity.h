#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>
#include <stdint.h>

typedef int Entity;
typedef int (*Prefab)(Entity);

void RemoveEntity(Entity);
void DisableEntity(Entity);
void EnableEntity(Entity);
bool Enabled(Entity);

#endif
