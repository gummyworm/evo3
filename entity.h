#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_USER_ENTITIES 0x10000

typedef int Entity;
typedef int (*Prefab)(Entity);

void RemoveEntity(Entity);
void DisableEntity(Entity);
void EnableEntity(Entity);
bool Enabled(Entity);
Entity NewEntity();

#endif
