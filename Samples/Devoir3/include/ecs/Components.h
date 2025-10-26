#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <tuple>
#include <vector>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "ComponentPool.h"
#include "ecs/Entity.h"
#include "rendering/core/Transform.h"
#include "rendering/graphics/Mesh.h"

struct Hierarchy
{
	Entity parent = INVALID_ENTITY;
	std::vector<Entity> children{};
};

struct RigidBody
{
	JPH::Body* body;
};

// The list of components
using Components = std::tuple
<
	Mesh,
	RigidBody,
	Transform,
	Hierarchy
>;

#endif
