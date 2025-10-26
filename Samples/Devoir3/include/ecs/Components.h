#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "ecs/Entity.h"

struct Hierarchy
{
	Entity parent = INVALID_ENTITY;
	std::vector<Entity> children{};
};

struct RigidBody
{
	JPH::Body* body;
};

#endif
