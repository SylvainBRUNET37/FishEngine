#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>

#include "rendering/ecs/Entity.h"

struct Hierarchy
{
	Entity parent = INVALID_ENTITY;
	std::vector<Entity> children{};
};

#endif
